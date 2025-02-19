/***************************************************************************
                              schematic_file.cpp
                             --------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QtCore>
#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include <QPlainTextEdit>
#include <qt3_compat/q3ptrlist.h>
#include <QTextStream>
#include <QList>
#include <QProcess>
#include <QDebug>

#include "main.h"
#include "node.h"
#include "schematic.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/spicefile.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/libcomp.h"
#include "components/sparamfile.h"
#include "module.h"
#include "misc.h"
#include "extsimkernels/abstractspicekernel.h"
#include "extsimkernels/s2spice.h"
#include "osdi/osdi_0_3.h"


// Here the subcircuits, SPICE components etc are collected. It must be
// global to also work within the subcircuits.
SubMap FileList;

// Dummy function for osdi_log callback.
// Without it, the program will crash if print or display function called
// in verilog-a model.
extern void osdi_log_skip(void *handle, char* msg, uint32_t lvl)
{
  (void)handle;
  (void)msg;
  (void)lvl;
}

namespace shim {
  // Historically Qucs-S has been using Q3PtrList a lot and it's not so easy
  // to replace it. The goal of this shim is to help with transition to new
  // container type.
  // Wherever possible a "barrier" is made by replacing Q3PtrList with QList
  // to stop its propagating. QList is passed instead of original Q3PtrList
  // and then its contents are copied back to Q3PtrList as if it was passed
  // in the first place
template <typename T>
void copyToQ3PtrList(const QList<T*>& src, Q3PtrList<T>& dst) {
  for (auto* item : src) {
    dst.append(item);
  }
}
} // namespace shim

// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to copy the selected elements into the clipboard.
QString Schematic::createClipboardFile()
{
  int z=0;  // counts selected elements
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  QString s("<Qucs Schematic " PACKAGE_VERSION ">\n");

  // Build element document.
  s += "<Components>\n";
  for(pc = a_Components->first(); pc != 0; pc = a_Components->next())
    if(pc->isSelected) {
      s += pc->save()+"\n";  z++; }
  s += "</Components>\n";

  s += "<Wires>\n";
  for(pw = a_Wires->first(); pw != 0; pw = a_Wires->next())
    if(pw->isSelected) {
      z++;
      if(pw->Label) if(!pw->Label->isSelected) {
        s += pw->save().section('"', 0, 0)+"\"\" 0 0 0>\n";
        continue;
      }
      s += pw->save()+"\n";
    }
  for(Node *pn = a_Nodes->first(); pn != 0; pn = a_Nodes->next())
    if(pn->Label) if(pn->Label->isSelected) {
      s += pn->Label->save()+"\n";  z++; }
  s += "</Wires>\n";

  s += "<Diagrams>\n";
  for(pd = a_Diagrams->first(); pd != 0; pd = a_Diagrams->next())
    if(pd->isSelected) {
      s += pd->save()+"\n";  z++; }
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(pp = a_Paintings->first(); pp != 0; pp = a_Paintings->next())
    if(pp->isSelected)
      if(pp->Name.at(0) != '.') {  // subcircuit specific -> do not copy
        s += "<"+pp->save()+">\n";  z++; }
  s += "</Paintings>\n";

  if(z == 0) return "";   // return empty if no selection

  return s;
}

// -------------------------------------------------------------
// Only read fields without loading them.
bool Schematic::loadIntoNothing(QTextStream *stream)
{
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
  }

  QMessageBox::critical(0, QObject::tr("Error"),
  QObject::tr("Format Error:\n'Painting' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
// Paste from clipboard.
bool Schematic::pasteFromClipboard(QTextStream *stream, QList<Element*> *pe)
{
  QString Line;

  Line = stream->readLine();
  if(Line.left(16) != "<Qucs Schematic ")   // wrong file type ?
    return false;

  QString s = PACKAGE_VERSION;
  Line = Line.mid(16, Line.length()-17);
  if(Line != s) {  // wrong version number ?
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Wrong document version: ")+Line);
    return false;
  }

  // read content in symbol edit mode *************************
  if(a_symbolMode) {
    while(!stream->atEnd()) {
      Line = stream->readLine();
      if(Line == "<Components>") {
        if(!loadIntoNothing(stream)) return false; }
      else
      if(Line == "<Wires>") {
        if(!loadIntoNothing(stream)) return false; }
      else
      if(Line == "<Diagrams>") {
        if(!loadIntoNothing(stream)) return false; }
      else
      if(Line == "<Paintings>") {
        if(!loadPaintings(stream, (QList<Painting*>*)pe)) return false; }
      else {
        QMessageBox::critical(0, QObject::tr("Error"),
        QObject::tr("Clipboard Format Error:\nUnknown field!"));
        return false;
      }
    }

    return true;
  }

  // read content in schematic edit mode *************************
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<Components>") {
      if(!loadComponents(stream, (QList<Component*>*)pe)) return false; }
    else
    if(Line == "<Wires>") {
      if(!loadWires(stream, pe)) return false; }
    else
    if(Line == "<Diagrams>") {
      if(!loadDiagrams(stream, (QList<Diagram*>*)pe)) return false; }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(stream, (QList<Painting*>*)pe)) return false; }
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Clipboard Format Error:\nUnknown field!"));
      return false;
    }
  }

  return true;
}

// -------------------------------------------------------------
int Schematic::saveSymbolCpp (void)
{
  QFileInfo info (a_DocName);
  QString cppfile = info.absolutePath () + QDir::separator() + a_DataSet;
  QFile file (cppfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
    QObject::tr("Cannot save C++ file \"%1\"!").arg(cppfile));
    return -1;
  }

  QTextStream stream (&file);

  // automatically compute boundings of drawing
  int xmin = INT_MAX;
  int ymin = INT_MAX;
  int xmax = INT_MIN;
  int ymax = INT_MIN;
  int x1, y1, x2, y2;
  int maxNum = 0;
  Painting * pp;

  stream << "  // symbol drawing code\n";
  for (pp = a_SymbolPaints.first (); pp != 0; pp = a_SymbolPaints.next ()) {
    if (pp->Name == ".ID ") continue;
    if (pp->Name == ".PortSym ") {
      if (((PortSymbol*)pp)->numberStr.toInt() > maxNum)
        maxNum = ((PortSymbol*)pp)->numberStr.toInt();
      x1 = ((PortSymbol*)pp)->cx;
      y1 = ((PortSymbol*)pp)->cy;
      if (x1 < xmin) xmin = x1;
      if (x1 > xmax) xmax = x1;
      if (y1 < ymin) ymin = y1;
      if (y1 > ymax) ymax = y1;
      continue;
    }
    pp->Bounding (x1, y1, x2, y2);
    if (x1 < xmin) xmin = x1;
    if (x2 > xmax) xmax = x2;
    if (y1 < ymin) ymin = y1;
    if (y2 > ymax) ymax = y2;
    stream << "  " << pp->saveCpp () << "\n";
  }

  stream << "\n  // terminal definitions\n";
  for (int i = 1; i <= maxNum; i++) {
    for (pp = a_SymbolPaints.first (); pp != 0; pp = a_SymbolPaints.next ()) {
      if (pp->Name == ".PortSym ") {
        if (((PortSymbol*)pp)->numberStr.toInt() == i) {
          stream << "  " << pp->saveCpp () << "\n";
        }
      }
    }
  }

  stream << "\n  // symbol boundings\n"
    << "  x1 = " << xmin << "; " << "  y1 = " << ymin << ";\n"
    << "  x2 = " << xmax << "; " << "  y2 = " << ymax << ";\n";

  stream << "\n  // property text position\n";
  for (pp = a_SymbolPaints.first (); pp != 0; pp = a_SymbolPaints.next ())
    if (pp->Name == ".ID ")
      stream << "  " << pp->saveCpp () << "\n";

  file.close ();
  return 0;
}

int Schematic::savePropsJSON()
{
  QFileInfo info (a_DocName);
  QString jsonfile = info.absolutePath () + QDir::separator()
                     + info.baseName() + "_props.json";
  QString vafilename = info.absolutePath () + QDir::separator()
                       + info.baseName() + ".va";
  QString osdifile = info.absolutePath() + QDir::separator()
                     + info.baseName() + ".osdi";

  QFile vafile(vafilename);
  if (!vafile.open (QIODevice::ReadOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
                          QObject::tr("Cannot open Verilog-A file \"%1\"!").arg(vafilename));
    return -1;
  }

  // if no osdi file exits, generete json file in the old way
  if (!QFile::exists(osdifile)){
    QString module;
    QStringList prop_name;
    QStringList prop_val;
    QTextStream vastream (&vafile);
    while(!vastream.atEnd()) {
      QString line = vastream.readLine();
      line = line.toLower();
      if (line.contains("module")) {
        auto tokens = line.split(QRegularExpression("[\\s()]"));
        if (tokens.count() > 1) module = tokens.at(1);
            module = module.trimmed();
        continue;
      }
      if (line.contains("parameter")) {
        auto tokens = line.split(QRegularExpression("[\\s=;]"), Qt::SkipEmptyParts);
        if (tokens.count() >= 4) {
          for(int ic = 0; ic <= tokens.count(); ic++) {
            if (tokens.at(ic) == "parameter") {
              prop_name.append(tokens.at(ic+2));
              prop_val.append(tokens.at(ic+3));
              break;
            }
          }
        }
      }
    }
    vafile.close();

    QFile file (jsonfile);

    if (!file.open (QIODevice::WriteOnly)) {
      QMessageBox::critical (0, QObject::tr("Error"),
                            QObject::tr("Cannot save JSON props file \"%1\"!").arg(jsonfile));
      return -1;
    }

    QTextStream stream (&file);

    stream << "{\n";

    stream << QStringLiteral("  \"description\" : \"%1 verilog device\",\n").arg(module);
    stream << "  \"property\" : [\n";
    auto name = prop_name.begin();
    auto val = prop_val.begin();
    for(; name != prop_name.end(); name++,val++) {
      stream << QStringLiteral("    { \"name\" : \"%1\", \"value\" : \"%2\", \"display\" : \"false\", \"desc\" : \"-\"},\n")
                    .arg(*name,*val);
    }
    stream << "  ],\n\n";
    stream << "  \"tx\" : 4,\n";
    stream << "  \"ty\" : 4,\n";
    stream << QStringLiteral("  \"Model\" : \"%1\",\n").arg(module);
    stream << "  \"NetName\" : \"T\",\n\n\n";
    stream << QStringLiteral("  \"SymName\" : \"%1\",\n").arg(module);
    stream << QStringLiteral("  \"BitmapFile\" : \"%1\",\n").arg(module);

    stream << "}";

    file.close ();
  }else{
    QString module;
    QStringList prop_name;
    QStringList prop_val;
    QStringList prop_disp;
    QStringList prop_desc;

    QLibrary osdilib (osdifile);
    if (!osdilib.load()){
      QMessageBox::critical(0, QObject::tr("Error"),
                            QObject::tr("No valid osdi file. Re-compile verilog-a file first!"));
      return -1;
    }

    // log function is disabled here.
    // the dummy function osdi_log_backup is assigned to callback pointer
    // to avoid program crash.
    void** osdi_log_ = reinterpret_cast<void**>(osdilib.resolve("osdi_log"));
    void* osdi_log_backup = nullptr;
    if (osdi_log_){
      osdi_log_backup = *osdi_log_;
    }
    *osdi_log_ = (void*)osdi_log_skip;

    OsdiDescriptor* descriptors = reinterpret_cast<OsdiDescriptor*>(osdilib.resolve("OSDI_DESCRIPTORS"));
    auto descriptor = descriptors[0];
    void* handler = nullptr;

    // OsdiSimParas and OsdiInitInfo have to be initialized before setup_model and setup_instance
    std::vector<char*> sim_params_names_vec={nullptr};
    std::vector<double> sim_params_vals_vec={};
    std::vector<char*> sim_params_str_vec = {nullptr};
    OsdiSimParas sim_params = {
                        .names = sim_params_names_vec.data(),
                        .vals = sim_params_vals_vec.data(),
                        .names_str = sim_params_str_vec.data(),
                        .vals_str = nullptr
    };
    OsdiInitInfo sim_info = {
        .flags = 0,
        .num_errors = 0,
        .errors = nullptr
    };

    void* model = calloc(1,descriptor.model_size);
    void* instance = calloc(1,descriptor.instance_size);

    descriptor.setup_model(handler,model,&sim_params,&sim_info);
    descriptor.setup_instance(handler,instance,model,300,descriptor.num_terminals,&sim_params,&sim_info);

    module = QString(descriptor.name);
    for(uint32_t i=1;i<descriptor.num_params;i++) {
      auto param = descriptor.param_opvar+i;
      void* value;
      if (i<descriptor.num_instance_params){
        value = descriptor.access(instance,model,i,ACCESS_FLAG_INSTANCE);
        prop_disp.append("true");
      }else{
        value = descriptor.access(instance,model,i,ACCESS_FLAG_READ);
        prop_disp.append("false");
      }

      switch (param->flags & PARA_TY_MASK)
      {
      case PARA_TY_INT:
        prop_val.append(QString::number(*static_cast<uint32_t*>(value)));
        break;
      case PARA_TY_REAL:
        prop_val.append(QString::number(*static_cast<double*>(value)));
        break;
      case PARA_TY_STR:
        prop_val.append(QString(static_cast<char*>(value)));
        break;
      default:
        prop_val.append("");
        break;
      }

      prop_name.append(param->name[0]);
      prop_desc.append(param->description);
    }

    free(model);
    free(instance);
    if (osdi_log_backup) {
      *osdi_log_ = osdi_log_backup;
    }
    osdilib.unload();

    QFile file (jsonfile);

    if (!file.open (QIODevice::WriteOnly)) {
      QMessageBox::critical (0, QObject::tr("Error"),
                            QObject::tr("Cannot save JSON props file \"%1\"!").arg(jsonfile));
      return -1;
    }

    QTextStream stream (&file);

    stream << "{\n";

    stream << QStringLiteral("  \"description\" : \"%1 verilog device\",\n").arg(module);
    stream << "  \"property\" : [\n";
    auto name = prop_name.begin();
    auto val = prop_val.begin();
    auto disp = prop_disp.begin();
    auto desc = prop_desc.begin();
    for(; name != prop_name.end(); name++,val++,disp++,desc++) {
      stream << QStringLiteral("    { \"name\" : \"%1\", \"value\" : \"%2\", \"display\" : \"%3\", \"desc\" : \"%4\"},\n")
                    .arg(*name,*val,*disp,*desc);
    }
    stream << "  ],\n\n";
    stream << "  \"tx\" : 4,\n";
    stream << "  \"ty\" : 4,\n";
    stream << QStringLiteral("  \"Model\" : \"%1\",\n").arg(module);
    stream << "  \"NetName\" : \"T\",\n\n\n";
    stream << QStringLiteral("  \"SymName\" : \"%1\",\n").arg(module);
    stream << QStringLiteral("  \"BitmapFile\" : \"%1\",\n").arg(module);

    stream << "}";

    file.close ();
  }
  return 0;
}

// save symbol paintings in JSON format
int Schematic::saveSymbolJSON()
{
  QFileInfo info (a_DocName);
  QString jsonfile = info.absolutePath () + QDir::separator()
                   + info.baseName() + "_sym.json";

  qDebug() << "saveSymbolJson for " << jsonfile;

  QFile file (jsonfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
		   QObject::tr("Cannot save JSON symbol file \"%1\"!").arg(jsonfile));
    return -1;
  }

  QTextStream stream (&file);

  // automatically compute boundings of drawing
  int xmin = INT_MAX;
  int ymin = INT_MAX;
  int xmax = INT_MIN;
  int ymax = INT_MIN;
  int x1, y1, x2, y2;
  int maxNum = 0;
  Painting * pp;

  stream << "{\n";

  stream << "\"paintings\" : [\n";

  // symbol drawing code"
  for (pp = a_SymbolPaints.first (); pp != 0; pp = a_SymbolPaints.next ()) {
    if (pp->Name == ".ID ") continue;
    if (pp->Name == ".PortSym ") {
      if (((PortSymbol*)pp)->numberStr.toInt() > maxNum)
        maxNum = ((PortSymbol*)pp)->numberStr.toInt();
      x1 = ((PortSymbol*)pp)->cx;
      y1 = ((PortSymbol*)pp)->cy;
      if (x1 < xmin) xmin = x1;
      if (x1 > xmax) xmax = x1;
      if (y1 < ymin) ymin = y1;
      if (y1 > ymax) ymax = y1;
      continue;
    }
    pp->Bounding (x1, y1, x2, y2);
    if (x1 < xmin) xmin = x1;
    if (x2 > xmax) xmax = x2;
    if (y1 < ymin) ymin = y1;
    if (y2 > ymax) ymax = y2;
    stream << "  " << pp->saveJSON() << "\n";
  }

  // terminal definitions
  //stream << "terminal \n";
  for (int i = 1; i <= maxNum; i++) {
    for (pp = a_SymbolPaints.first (); pp != 0; pp = a_SymbolPaints.next ()) {
      if (pp->Name == ".PortSym ") {
        if (((PortSymbol*)pp)->numberStr.toInt() == i) {
          stream << "  " << pp->saveJSON () << "\n";
        }
      }
    }
  }

  stream << "],\n"; //end of paintings JSON array

  // symbol boundings
  stream
    << "  \"x1\" : " << xmin << ",\n" << "  \"y1\" : " << ymin << ",\n"
    << "  \"x2\" : " << xmax << ",\n" << "  \"y2\" : " << ymax << ",\n";

  // property text position
  for (pp = a_SymbolPaints.first (); pp != 0; pp = a_SymbolPaints.next ())
    if (pp->Name == ".ID ")
      stream << "  " << pp->saveJSON () << "\n";

  stream << "}\n";

  file.close ();
  return 0;


}

// -------------------------------------------------------------
// Returns the number of subcircuit ports.
int Schematic::saveDocument()
{
  QFile file(a_DocName);
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
    QObject::tr("Cannot save document!"));
    return -1;
  }

  QTextStream stream(&file);

  stream << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  // Special case of saving a file when we want to save *only*
  // the symbol defintion (i.e. to create a "symbol file")
  if (a_DocName.endsWith(".sym")) {
      stream << "<Symbol>\n";
      for(auto* pp : a_SymbolPaints) {
          stream << "  <" << pp->save() << ">\n";
      }
      stream << "</Symbol>\n";
      file.close();
      return 0;
  }

  stream << "<Properties>\n";
  if(a_symbolMode) {
    stream << "  <View=" << a_tmpViewX1<<","<<a_tmpViewY1<<","
      << a_tmpViewX2<<","<<a_tmpViewY2<< ",";
    stream <<a_tmpScale<<","<<a_tmpPosX<<","<<a_tmpPosY << ">\n";
  }
  else {
    stream << "  <View=" << a_ViewX1<<","<<a_ViewY1<<","
      << a_ViewX2<<","<<a_ViewY2<< ",";
    stream << a_Scale <<","<<contentsX()<<","<<contentsY() << ">\n";
  }
  stream << "  <Grid=" << a_GridX<<","<<a_GridY<<","
    << a_GridOn << ">\n";
  stream << "  <DataSet=" << a_DataSet << ">\n";
  stream << "  <DataDisplay=" << a_DataDisplay << ">\n";
  stream << "  <OpenDisplay=" << a_SimOpenDpl << ">\n";
  stream << "  <Script=" << a_Script << ">\n";
  stream << "  <RunScript=" << a_SimRunScript << ">\n";
  stream << "  <showFrame=" << a_showFrame << ">\n";

  QString t;
  misc::convert2ASCII(t = a_Frame_Text0);
  stream << "  <FrameText0=" << t << ">\n";
  misc::convert2ASCII(t = a_Frame_Text1);
  stream << "  <FrameText1=" << t << ">\n";
  misc::convert2ASCII(t = a_Frame_Text2);
  stream << "  <FrameText2=" << t << ">\n";
  misc::convert2ASCII(t = a_Frame_Text3);
  stream << "  <FrameText3=" << t << ">\n";
  stream << "</Properties>\n";

  Painting *pp;
  stream << "<Symbol>\n";     // save all paintings for symbol
  for(pp = a_SymbolPaints.first(); pp != 0; pp = a_SymbolPaints.next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Symbol>\n";

  stream << "<Components>\n";    // save all components
  for(Component *pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next())
    stream << "  " << pc->save() << "\n";
  stream << "</Components>\n";

  stream << "<Wires>\n";    // save all wires
  for(Wire *pw = a_DocWires.first(); pw != 0; pw = a_DocWires.next())
    stream << "  " << pw->save() << "\n";

  // save all labeled nodes as wires
  for(Node *pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next())
    if(pn->Label) stream << "  " << pn->Label->save() << "\n";
  stream << "</Wires>\n";

  stream << "<Diagrams>\n";    // save all diagrams
  for(Diagram *pd = a_DocDiags.first(); pd != 0; pd = a_DocDiags.next())
    stream << "  " << pd->save() << "\n";
  stream << "</Diagrams>\n";

  stream << "<Paintings>\n";     // save all paintings
  for(pp = a_DocPaints.first(); pp != 0; pp = a_DocPaints.next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Paintings>\n";

  file.close();

  // additionally save symbol C++ code if in a symbol drawing and the
  // associated file is a Verilog-A file
  if (fileSuffix () == "sym") {
    if (fileSuffix (a_DataDisplay) == "va") {
      saveSymbolCpp ();
      saveSymbolJSON ();
      if (QucsSettings.DefaultSimulator == spicecompat::simNgspice) {
          savePropsJSON();
      } else if (QucsSettings.DefaultSimulator == spicecompat::simQucsator) {
          // TODO slit this into another method, or merge into saveSymbolJSON
          // handle errors in separate
          qDebug() << "  -> Run adms for symbol";

          QString vaFile;

    //      QDir prefix = QDir(QucsSettings.BinDir);

          QFileInfo inf(QucsSettings.Qucsator);
          QString QucsatorPath = inf.path()+QDir::separator();
          QDir include = QDir(QucsatorPath+"../include/qucs-core");

          //pick admsXml from settings
          QString admsXml = QucsSettings.AdmsXmlBinDir.canonicalPath();

#if defined(_WIN32) || defined(__MINGW32__)
          admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml.exe");
    #else
          admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml");
    #endif

          QString workDir = QucsSettings.QucsWorkDir.absolutePath();

          qDebug() << "App path : " << qApp->applicationDirPath();
          qDebug() << "workdir"  << workDir;
          qDebug() << "workspacedir"  << QucsSettings.qucsWorkspaceDir.absolutePath();

          vaFile = QucsSettings.QucsWorkDir.filePath(fileBase()+".va");

          QStringList Arguments;
          Arguments << QDir::toNativeSeparators(vaFile)
                    << "-I" << QDir::toNativeSeparators(include.absolutePath())
                    << "-e" << QDir::toNativeSeparators(include.absoluteFilePath("qucsMODULEguiJSONsymbol.xml"))
                    << "-A" << "dyload";

    //      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    //      env.insert("PATH", env.value("PATH") );

          QFile file(admsXml);
          if ( !file.exists() ){
            QMessageBox::critical(this, tr("Error"),
                                  tr("Program admsXml not found: %1\n\n"
                                      "Set the admsXml location on the application settings.").arg(admsXml));
            return -1;
          }

          qDebug() << "Command: " << admsXml << Arguments.join(" ");

          // need to cd into project to run admsXml?
          QDir::setCurrent(workDir);

          QProcess builder;
          builder.setProcessChannelMode(QProcess::MergedChannels);

          builder.start(admsXml, Arguments);


          // how to capture [warning]? need to modify admsXml?
          // TODO put stdout, stderr into a dock window, not messagebox
          if (!builder.waitForFinished()) {
            QString cmdString = QStringLiteral("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
            cmdString = cmdString + builder.errorString();
            QMessageBox::critical(this, tr("Error"), cmdString);
          }
          else {
            QString cmdString = QStringLiteral("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
            cmdString = cmdString + builder.readAll();
            QMessageBox::information(this, tr("Status"), cmdString);
          }
      }



      // Append _sym.json into _props.json, save into _symbol.json
      QFile f1(QucsSettings.QucsWorkDir.filePath(fileBase()+"_props.json"));
      QFile f2(QucsSettings.QucsWorkDir.filePath(fileBase()+"_sym.json"));
      f1.open(QIODevice::ReadOnly | QIODevice::Text);
      f2.open(QIODevice::ReadOnly | QIODevice::Text);

      QString dat1 = QString(f1.readAll());
      QString dat2 = QString(f2.readAll());
      QString finalJSON = dat1.append(dat2);

      // remove joining point
      finalJSON = finalJSON.replace("}{", "");

      QFile f3(QucsSettings.QucsWorkDir.filePath(fileBase()+"_symbol.json"));
      f3.open(QIODevice::WriteOnly | QIODevice::Text);
      QTextStream out(&f3);
      out << finalJSON;

      f1.close();
      f2.close();
      f3.close();

      // TODO choose icon, default to something or provided png

    } // if DataDisplay va
  } // if suffix .sym

  return 0;
}

// -------------------------------------------------------------
bool Schematic::loadProperties(QTextStream *stream)
{
  bool ok = true;
  QString Line, cstr, nstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;  // field end ?
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line.at(0) != '<') {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nWrong property field limiter!"));
      return false;
    }
    if(Line.at(Line.length()-1) != '>') {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nWrong property field limiter!"));
      return false;
    }
    Line = Line.mid(1, Line.length()-2);   // cut off start and end character

    cstr = Line.section('=',0,0);    // property type
    nstr = Line.section('=',1,1);    // property value
    if(cstr == "View") {
      a_ViewX1 = nstr.section(',',0,0).toInt(&ok); if(ok) {
      a_ViewY1 = nstr.section(',',1,1).toInt(&ok); if(ok) {
      a_ViewX2 = nstr.section(',',2,2).toInt(&ok); if(ok) {
      a_ViewY2 = nstr.section(',',3,3).toInt(&ok); if(ok) {
      a_Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) {
      a_tmpViewX1 = nstr.section(',',5,5).toInt(&ok); if(ok)
      a_tmpViewY1 = nstr.section(',',6,6).toInt(&ok); }}}}}
    }
    else if(cstr == "Grid") {
      a_GridX = nstr.section(',',0,0).toInt(&ok); if(ok) {
      a_GridY = nstr.section(',',1,1).toInt(&ok); if(ok) {
      if(nstr.section(',',2,2).toInt(&ok) == 0) a_GridOn = false;
      else a_GridOn = true; }}
    }
    else if(cstr == "DataSet") a_DataSet = nstr;
    else if(cstr == "DataDisplay") a_DataDisplay = nstr;
    else if(cstr == "OpenDisplay")
    if(nstr.toInt(&ok) == 0) a_SimOpenDpl = false;
    else a_SimOpenDpl = true;
    else if(cstr == "Script") a_Script = nstr;
    else if(cstr == "RunScript")
    if(nstr.toInt(&ok) == 0) a_SimRunScript = false;
    else a_SimRunScript = true;
    else if(cstr == "showFrame")
    a_showFrame = nstr.at(0).toLatin1() - '0';
    else if(cstr == "FrameText0") misc::convert2Unicode(a_Frame_Text0 = nstr);
    else if(cstr == "FrameText1") misc::convert2Unicode(a_Frame_Text1 = nstr);
    else if(cstr == "FrameText2") misc::convert2Unicode(a_Frame_Text2 = nstr);
    else if(cstr == "FrameText3") misc::convert2Unicode(a_Frame_Text3 = nstr);
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nUnknown property: ")+cstr);
      return false;
    }
    if(!ok) {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nNumber expected in property field!"));
      return false;
    }
  }

  QMessageBox::critical(0, QObject::tr("Error"),
               QObject::tr("Format Error:\n'Property' field is not closed!"));
  return false;
}

// ---------------------------------------------------
// Inserts a component without performing logic for wire optimization.
void Schematic::simpleInsertComponent(Component *c)
{
  Node *pn;
  int x, y;
  // connect every node of component
  for (Port *pp : c->Ports) {
    x = pp->x+c->cx;
    y = pp->y+c->cy;

    // check if new node lies upon existing node
    for(pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next())
      if(pn->cx == x) if(pn->cy == y) {
        if (!pn->DType.isEmpty()) {
          pp->Type = pn->DType;
        }
        if (!pp->Type.isEmpty()) {
          pn->DType = pp->Type;
        }
        break;
      }

    if(pn == nullptr) { // create new node, if no existing one lies at this position
      pn = new Node(x, y);
      a_DocNodes.append(pn);
    }
    pn->connect(c);  // connect schematic node to component node
    if (!pp->Type.isEmpty()) {
      pn->DType = pp->Type;
    }

    pp->Connection = pn;  // connect component node to schematic node
  }

  a_DocComps.append(c);
}

// -------------------------------------------------------------
bool Schematic::loadComponents(QTextStream *stream, QList<Component*> *List)
{
  QString Line, cstr;
  Component *c;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    /// \todo enable user to load partial schematic, skip unknown components
    c = getComponentFromName(Line, this);
    if(!c) return false;

    if(List) {  // "paste" ?
      int z;
      for(z=c->Name.length()-1; z>=0; z--) // cut off number of component name
        if(!c->Name.at(z).isDigit()) break;
      c->Name = c->Name.left(z+1);
      List->append(c);
    }
    else  simpleInsertComponent(c);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
	   QObject::tr("Format Error:\n'Component' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
// Inserts a wire without performing logic for optimizing.
void Schematic::simpleInsertWire(Wire *pw)
{
  Node *pn;
  // check if first wire node lies upon existing node
  for(pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next())
    if(pn->cx == pw->x1) if(pn->cy == pw->y1) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x1, pw->y1);
    a_DocNodes.append(pn);
  }

  if(pw->x1 == pw->x2) if(pw->y1 == pw->y2) {
    pn->Label = pw->Label;   // wire with length zero are just node labels
    if (pn->Label) {
      pn->Label->Type = isNodeLabel;
      pn->Label->pOwner = pn;
    }
    delete pw;           // delete wire because this is not a wire
    return;
  }
  pn->connect(pw);  // connect schematic node to component node
  pw->Port1 = pn;

  // check if second wire node lies upon existing node
  for(pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next())
    if(pn->cx == pw->x2) if(pn->cy == pw->y2) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x2, pw->y2);
    a_DocNodes.append(pn);
  }
  pn->connect(pw);  // connect schematic node to component node
  pw->Port2 = pn;

  a_DocWires.append(pw);
}

// -------------------------------------------------------------
bool Schematic::loadWires(QTextStream *stream, QList<Element*> *List)
{
  Wire *w;
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    // (Node*)4 =  move all ports (later on)
    w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
    if(!w->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nWrong 'wire' line format!"));
      delete w;
      return false;
    }
    if(List) {
      if(w->x1 == w->x2) if(w->y1 == w->y2) if(w->Label) {
        w->Label->Type = isMovingLabel;
        List->append(w->Label);
        delete w;
        continue;
      }
      List->append(w);
      if(w->Label)  List->append(w->Label);
    }
    else simpleInsertWire(w);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
  QObject::tr("Format Error:\n'Wire' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
bool Schematic::loadDiagrams(QTextStream *stream, QList<Diagram*> *List)
{
  Diagram *d;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    cstr = Line.section(' ',0,0);    // diagram type
         if(cstr == "<Rect") d = new RectDiagram();
    else if(cstr == "<Polar") d = new PolarDiagram();
    else if(cstr == "<Tab") d = new TabDiagram();
    else if(cstr == "<Smith") d = new SmithDiagram();
    else if(cstr == "<ySmith") d = new SmithDiagram(0,0,false);
    else if(cstr == "<PS") d = new PSDiagram();
    else if(cstr == "<SP") d = new PSDiagram(0,0,false);
    else if(cstr == "<Rect3D") d = new Rect3DDiagram();
    else if(cstr == "<Curve") d = new CurveDiagram();
    else if(cstr == "<Time") d = new TimingDiagram();
    else if(cstr == "<Truth") d = new TruthDiagram();
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nUnknown diagram!"));
      return false;
    }

    if(!d->load(Line, stream)) {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nWrong 'diagram' line format!"));
      delete d;
      return false;
    }
    List->append(d);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
  QObject::tr("Format Error:\n'Diagram' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
bool Schematic::loadPaintings(QTextStream *stream, QList<Painting*> *List)
{
  Painting *p=0;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if (Line.trimmed().isEmpty()) continue;

    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;

    Line = Line.trimmed();
    if(Line.isEmpty()) continue;
    if( (Line.at(0) != '<') || (Line.at(Line.length()-1) != '>')) {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nWrong 'painting' line delimiter!"));
      return false;
    }
    Line = Line.mid(1, Line.length()-2);  // cut off start and end character

    cstr = Line.section(' ',0,0);    // painting type
         if(cstr == "Line") p = new GraphicLine();
    else if(cstr == "EArc") p = new EllipseArc();
    else if(cstr == ".PortSym") p = new PortSymbol();
    else if(cstr == ".ID") p = new ID_Text();
    else if(cstr == "Text") p = new GraphicText();
    else if(cstr == "Rectangle") p = new qucs::Rectangle();
    else if(cstr == "Arrow") p = new Arrow();
    else if(cstr == "Ellipse") p = new qucs::Ellipse();
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("Format Error:\nUnknown painting!"));
      return false;
    }

    if(!p->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"),
        QObject::tr("Format Error:\nWrong 'painting' line format!"));
      delete p;
      return false;
    }
    List->append(p);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
  QObject::tr("Format Error:\n'Painting' field is not closed!"));
  return false;
}

/*!
 * \brief Schematic::loadDocument tries to load a schematic document.
 * \return true/false in case of success/failure
 */
bool Schematic::loadDocument()
{
  QFile file(a_DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    /// \todo implement unified error/warning handling GUI and CLI
    if (QucsMain != nullptr)
      QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load document: ")+a_DocName);
    else
      qCritical() << "Schematic::loadDocument:"
                  << QObject::tr("Cannot load document: ")+a_DocName;
    return false;
  }

  // Keep reference to source file (the schematic file)
  setFileInfo(a_DocName);

  QString Line;
  QTextStream stream(&file);

  // read header **************************
  do {
    if(stream.atEnd()) {
      file.close();
      return true;
    }

    Line = stream.readLine();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    QMessageBox::critical(0, QObject::tr("Error"),
    QObject::tr("Wrong document type: ")+a_DocName);
    return false;
  }

  Line = Line.mid(16, Line.length()-17);
  if(!misc::checkVersion(Line)) { // wrong version number ?

    QMessageBox::StandardButton result;
    result = QMessageBox::warning(0,
                                  QObject::tr("Warning"),
                                  QObject::tr("Wrong document version \n") +
                                              a_DocName + "\n" +
                                  QObject::tr("Try to open it anyway?"),
                                  QMessageBox::Yes|QMessageBox::No);

    if (result==QMessageBox::No) {
        file.close();
        return false;
    }

    //QMessageBox::critical(0, QObject::tr("Error"),
        // QObject::tr("Wrong document version: ")+Line);
  }

  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line == "<Symbol>") {
      QList<Painting*> paintings;
      if (!loadPaintings(&stream, &paintings)) {
        file.close();
        return false;
      }
      shim::copyToQ3PtrList(paintings, a_SymbolPaints);
    }
    else
    if(Line == "<Properties>") {
      if(!loadProperties(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Components>") {
      if(!loadComponents(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Wires>") {
      if(!loadWires(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Diagrams>") {
      QList<Diagram*> diagrams;
      if (!loadDiagrams(&stream, &diagrams)) { file.close(); return false; }
      shim::copyToQ3PtrList(diagrams, a_DocDiags);
    }
    else
    if(Line == "<Paintings>") {
      QList<Painting*> paintings;
      if (!loadPaintings(&stream, &paintings)) { file.close(); return false; }
      shim::copyToQ3PtrList(paintings, a_DocPaints);
    }
    else {
       qDebug() << Line;
       QMessageBox::critical(0, QObject::tr("Error"),
      QObject::tr("File Format Error:\nUnknown field!"));
      file.close();
      return false;
    }
  }

  file.close();
  return true;
}

// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to save state for undo operation.
QString Schematic::createUndoString(char Op)
{
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  // Build element document.
  QString s = "  \n";
  s.replace(0,1,Op);
  for(pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next())
    s += pc->save()+"\n";
  s += "</>\n";  // short end flag

  for(pw = a_DocWires.first(); pw != 0; pw = a_DocWires.next())
    s += pw->save()+"\n";
  // save all labeled nodes as wires
  for(Node *pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next())
    if(pn->Label) s += pn->Label->save()+"\n";
  s += "</>\n";

  for(pd = a_DocDiags.first(); pd != 0; pd = a_DocDiags.next())
    s += pd->save()+"\n";
  s += "</>\n";

  for(pp = a_DocPaints.first(); pp != 0; pp = a_DocPaints.next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
}

// -------------------------------------------------------------
// Same as "createUndoString(char Op)" but for symbol edit mode.
QString Schematic::createSymbolUndoString(char Op)
{
  Painting *pp;

  // Build element document.
  QString s = "  \n";
  s.replace(0,1,Op);
  s += "</>\n";  // short end flag for components
  s += "</>\n";  // short end flag for wires
  s += "</>\n";  // short end flag for diagrams

  for(pp = a_SymbolPaints.first(); pp != 0; pp = a_SymbolPaints.next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
}

// -------------------------------------------------------------
// Is quite similar to "loadDocument()" but with less error checking.
// Used for "undo" function.
bool Schematic::rebuild(QString *s)
{
  a_DocWires.clear();	// delete whole document
  a_DocNodes.clear();
  a_DocComps.clear();
  a_DocDiags.clear();
  a_DocPaints.clear();

  QString Line;
  QTextStream stream(s, QIODevice::ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  if(!loadComponents(&stream))  return false;
  if(!loadWires(&stream))  return false;
  {
    QList<Diagram*> diagrams;
    if (!loadDiagrams(&stream, &diagrams)) return false;
    shim::copyToQ3PtrList(diagrams, a_DocDiags);
  }
  {
    QList<Painting*> paintings;
    if (!loadPaintings(&stream, &paintings)) return false;
    shim::copyToQ3PtrList(paintings, a_DocPaints);
  }

  return true;
}

// -------------------------------------------------------------
// Same as "rebuild(QString *s)" but for symbol edit mode.
bool Schematic::rebuildSymbol(QString *s)
{
  a_SymbolPaints.clear();	// delete whole document

  QString Line;
  QTextStream stream(s, QIODevice::ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  Line = stream.readLine();  // skip components
  Line = stream.readLine();  // skip wires
  Line = stream.readLine();  // skip diagrams

  {
    QList<Painting*> paintings;
    if (!loadPaintings(&stream, &paintings)) return false;
    shim::copyToQ3PtrList(paintings, a_SymbolPaints);
  }
  return true;
}


// ***************************************************************
// *****                                                     *****
// *****             Functions to create netlist             *****
// *****                                                     *****
// ***************************************************************

void Schematic::createNodeSet(QStringList& Collect, int& countInit,
          Conductor *pw, Node *p1)
{
  if(pw->Label)
    if(!pw->Label->initValue.isEmpty())
      Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
                     p1->Name + " U=\"" + pw->Label->initValue + "\"");
}

// ---------------------------------------------------
void Schematic::throughAllNodes(bool User, QStringList& Collect,
        int& countInit)
{
  Node *pn;
  int z=0;

  for(pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next()) {
    if(pn->Name.isEmpty() == User) {
      continue;  // already named ?
    }
    if(!User) {
      if(a_isAnalog)
        pn->Name = "_net";
      else
        pn->Name = "net_net";   // VHDL names must not begin with '_'
      pn->Name += QString::number(z++);  // create numbered node name
    }
    else if(pn->State) {
      continue;  // already worked on
    }

    if(a_isAnalog) createNodeSet(Collect, countInit, pn, pn);

    pn->State = 1;
    propagateNode(Collect, countInit, pn);
  }
}

// ----------------------------------------------------------
// Checks whether this file is a qucs file and whether it is an subcircuit.
// It returns the number of subcircuit ports.
int Schematic::testFile(const QString& DocName)
{
  QFile file(DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    return -1;
  }

  QString Line;
  // .........................................
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.readAll();
  file.close();
  QTextStream stream(&FileString, QIODevice::ReadOnly);


  // read header ........................
  do {
    if(stream.atEnd()) {
      file.close();
      return -2;
    }
    Line = stream.readLine();
    Line = Line.trimmed();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    return -3;
  }

  Line = Line.mid(16, Line.length()-17);
  if(!misc::checkVersion(Line)) { // wrong version number ?
      if (!QucsSettings.IgnoreFutureVersion) {
          file.close();
          return -4;
      }
    //file.close();
    //return -4;
  }

  // read content ....................
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Components>") break;
  }

  int z=0;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "</Components>") {
      file.close();
      return z;       // return number of ports
    }

    Line = Line.trimmed();
    QString s = Line.section(' ',0,0);    // component type
    if(s == "<Port") z++;
  }
  return -5;  // component field not closed
}

// ---------------------------------------------------
// Collects the signal names for digital simulations.
void Schematic::collectDigitalSignals(void)
{
  Node *pn;

  for(pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next()) {
    DigMap::Iterator it = a_Signals.find(pn->Name);
    if(it == a_Signals.end()) { // avoid redeclaration of signal
      a_Signals.insert(pn->Name, DigSignal(pn->Name, pn->DType));
    } else if (!pn->DType.isEmpty()) {
      it.value().Type = pn->DType;
    }
  }
}

// ---------------------------------------------------
// Propagates the given node to connected component ports.
void Schematic::propagateNode(QStringList& Collect,
              int& countInit, Node *pn)
{
  bool setName=false;
  Q3PtrList<Node> Cons;
  Node *p2;
  Wire *pw;

  Cons.append(pn);
  for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
    for(auto* pe : *p2)
      if(pe->Type == isWire) {
        pw = (Wire*)pe;
        if(p2 != pw->Port1) {
          if(pw->Port1->Name.isEmpty()) {
            pw->Port1->Name = pn->Name;
            pw->Port1->State = 1;
            Cons.append(pw->Port1);
            setName = true;
          }
        }
        else {
          if(pw->Port2->Name.isEmpty()) {
            pw->Port2->Name = pn->Name;
            pw->Port2->State = 1;
            Cons.append(pw->Port2);
            setName = true;
          }
        }
        if(setName) {
          Cons.findRef(p2);   // back to current Connection
          if (a_isAnalog) createNodeSet(Collect, countInit, pw, pn);
            setName = false;
        }
      }
  Cons.clear();
}

#include <iostream>

/*!
 * \brief Schematic::throughAllComps
 * Goes through all schematic components and allows special component
 * handling, e.g. like subcircuit netlisting.
 * \param stream is a pointer to the text stream used to collect the netlist
 * \param countInit is the reference to a counter for nodesets (initial conditions)
 * \param Collect is the reference to a list of collected nodesets
 * \param ErrText is pointer to the QPlainTextEdit used for error messages
 * \param NumPorts counter for the number of ports
 * \return true in case of success (false otherwise)
 */
bool Schematic::throughAllComps(QTextStream *stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts)
{
  bool r;
  QString s;

  // give the ground nodes the name "gnd", and insert subcircuits etc.
  Q3PtrListIterator<Component> it(a_DocComps);
  Component *pc;
  while((pc = it.current()) != 0) {
    ++it;
    if(pc->isActive != COMP_IS_ACTIVE) continue;

    // check analog/digital typed components
    if(a_isAnalog) {
      if((pc->Type & isAnalogComponent) == 0) {
        ErrText->appendPlainText(QObject::tr("ERROR: Component \"%1\" has no analog model.").arg(pc->Name));
        return false;
      }
    } else {
      if((pc->Type & isDigitalComponent) == 0) {
        ErrText->appendPlainText(QObject::tr("ERROR: Component \"%1\" has no digital model.").arg(pc->Name));
        return false;
      }
    }

    // handle ground symbol
    if(pc->Model == "GND") {
      pc->Ports.first()->Connection->Name = "gnd";
      continue;
    }

    // handle subcircuits
    if(pc->Model == "Sub")
    {
      int i;
      // tell the subcircuit it belongs to this schematic
      pc->setSchematic (this);
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end())
      {
        if (!it.value().PortTypes.isEmpty())
        {
          i = 0;
          // apply in/out signal types of subcircuit
          for (Port *pp : pc->Ports)
          {
            pp->Type = it.value().PortTypes[i];
            pp->Connection->DType = pp->Type;
            i++;
          }
        }
        continue;   // insert each subcircuit just one time
      }

      // The subcircuit has not previously been added
      SubFile sub = SubFile("SCH", f);
      FileList.insert(f, sub);


      // load subcircuit schematic
      s = pc->Props.first()->Value;
      Schematic *d = new Schematic(0, pc->getSubcircuitFile());
      if(!d->loadDocument())      // load document if possible
      {
          delete d;
          /// \todo implement error/warning message dispatcher for GUI and CLI modes.
          QString message = QObject::tr("ERROR: Cannot load subcircuit \"%1\".").arg(s);
          if (QucsMain != nullptr) // GUI is running
            ErrText->appendPlainText(message);
          else // command line
            qCritical() << "Schematic::throughAllComps" << message;
          return false;
      }
      d->a_DocName = s;
      d->a_isVerilog = a_isVerilog;
      d->a_isAnalog = a_isAnalog;
      d->a_creatingLib = a_creatingLib;
      r = d->createSubNetlist(stream, countInit, Collect, ErrText, NumPorts);
      if (r)
      {
        i = 0;
        // save in/out signal types of subcircuit
        for (Port *pp : pc->Ports)
        {
            //if(i>=d->a_PortTypes.count())break;
            pp->Type = d->a_PortTypes[i];
            pp->Connection->DType = pp->Type;
            i++;
        }
        sub.PortTypes = d->a_PortTypes;
        FileList.insert(f,sub);
        //FileList.replace(f, sub);
      }
      delete d;
      if(!r)
      {
        return false;
      }
      continue;
    } // if(pc->Model == "Sub")

    if(LibComp* lib = dynamic_cast</*const*/LibComp*>(pc)) {
      if(a_creatingLib) {
        ErrText->appendPlainText(
        QObject::tr("WARNING: Skipping library component \"%1\".").
        arg(pc->Name));
        continue;
      }
      QString scfile = pc->getSubcircuitFile();
      s = scfile + "/" + pc->Props.at(1)->Value;
      SubMap::Iterator it = FileList.find(s);
      if(it != FileList.end())
        continue;   // insert each library subcircuit just one time
      FileList.insert(s, SubFile("LIB", s));

      unsigned whatisit = a_isAnalog?1:(a_isVerilog?4:2);
      if(a_isAnalog) {
        if (QucsSettings.DefaultSimulator!=spicecompat::simQucsator) {
            if (QucsSettings.DefaultSimulator==spicecompat::simXyce)
                whatisit = 16;
            else whatisit = 8;
        } else whatisit = 1;
      }
      r = lib->createSubNetlist(stream, Collect, whatisit);

      if(!r) {
        ErrText->appendPlainText(
        QObject::tr("ERROR: \"%1\": Cannot load library component \"%2\" from \"%3\"").
        arg(pc->Name, pc->Props.at(1)->Value, scfile));
        return false;
      }
      continue;
    }

    // handle SPICE subcircuit components
    if(pc->Model == "SPICE") {
      s = pc->Props.first()->Value;
      // tell the spice component it belongs to this schematic
      pc->setSchematic (this);
      if(s.isEmpty()) {
        ErrText->appendPlainText(QObject::tr("ERROR: No file name in SPICE component \"%1\".").
                        arg(pc->Name));
        return false;
      }
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end())
        continue;   // insert each spice component just one time
      FileList.insert(f, SubFile("CIR", f));

      SpiceFile *sf = (SpiceFile*)pc;
      if (QucsSettings.DefaultSimulator != spicecompat::simQucsator)
          r = sf->createSpiceSubckt(stream);
      else r = sf->createSubNetlist(stream);
      ErrText->appendPlainText(sf->getErrorText());
      if(!r){
        return false;
      }
      continue;
    }

    if (pc->Model == "SPfile" &&
        QucsSettings.DefaultSimulator == spicecompat::simNgspice) {
        QString f = pc->getSubcircuitFile();
        QString sub_name = "Sub_" + pc->Model + "_" + pc->Name;
        S2Spice *conv = new S2Spice();
        conv->setFile(f);
        conv->setDeviceName(sub_name);
        bool r = conv->convertTouchstone(stream);
        QString msg = conv->getErrText();
        if (!r) {
            QMessageBox::warning(this,tr("Netlist error"), msg);
            return false;
        } else if (!msg.isEmpty()) {
            QMessageBox::warning(this,tr("S2Spice warning"), msg);
        }
        delete conv;
    }

    // handle digital file subcircuits
    if(pc->Model == "VHDL" || pc->Model == "Verilog") {
      if(a_isVerilog && pc->Model == "VHDL")
        continue;
      if(!a_isVerilog && pc->Model == "Verilog")
        continue;
      s = pc->Props.front()->Value;
      if(s.isEmpty()) {
        ErrText->appendPlainText(QObject::tr("ERROR: No file name in %1 component \"%2\".").
          arg(pc->Model).
          arg(pc->Name));
        return false;
      }
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end())
        continue;   // insert each vhdl/verilog component just one time
      s = ((pc->Model == "VHDL") ? "VHD" : "VER");
      FileList.insert(f, SubFile(s, f));

      if(pc->Model == "VHDL") {
        VHDL_File *vf = (VHDL_File*)pc;
        r = vf->createSubNetlist(stream);
        ErrText->appendPlainText(vf->getErrorText());
        if(!r) {
          return false;
        }
      }
      if(pc->Model == "Verilog") {
        Verilog_File *vf = (Verilog_File*)pc;
        r = vf->createSubNetlist(stream);
        ErrText->appendPlainText(vf->getErrorText());
        if(!r) {
          return false;
        }
      }
      continue;
    }
  }
  return true;
}

// ---------------------------------------------------
// Follows the wire lines in order to determine the node names for
// each component. Output into "stream", NodeSets are collected in
// "Collect" and counted with "countInit".
bool Schematic::giveNodeNames(QTextStream *stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts)
{
  // delete the node names
  for(Node *pn = a_DocNodes.first(); pn != 0; pn = a_DocNodes.next()) {
    pn->State = 0;
    if(pn->Label) {
      if(a_isAnalog)
        pn->Name = pn->Label->Name;
      else
        pn->Name = "net" + pn->Label->Name;
    }
    else pn->Name = "";
  }

  // set the wire names to the connected node
  for(Wire *pw = a_DocWires.first(); pw != 0; pw = a_DocWires.next())
    if(pw->Label != 0) {
      if(a_isAnalog)
        pw->Port1->Name = pw->Label->Name;
      else  // avoid to use reserved VHDL words
        pw->Port1->Name = "net" + pw->Label->Name;
    }

  // go through components
  if(!throughAllComps(stream, countInit, Collect, ErrText, NumPorts)){
    fprintf(stderr, "Error: Could not go throughAllComps\n");
    return false;
  }

  // work on named nodes first in order to preserve the user given names
  throughAllNodes(true, Collect, countInit);

  // give names to the remaining (unnamed) nodes
  throughAllNodes(false, Collect, countInit);

  if(!a_isAnalog) // collect all node names for VHDL signal declaration
    collectDigitalSignals();

  return true;
}

// ---------------------------------------------------
bool Schematic::createLibNetlist(QTextStream *stream, QPlainTextEdit *ErrText,
          int NumPorts)
{
  int countInit = 0;
  QStringList Collect;
  Collect.clear();
  FileList.clear();
  a_Signals.clear();
  // Apply node names and collect subcircuits and file include
  a_creatingLib = true;
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts)) {
    a_creatingLib = false;
    return false;
  }
  a_creatingLib = false;

  // Marking start of actual top-level subcircuit
  QString c;
  if(!a_isAnalog) {
    if (a_isVerilog)
      c = "///";
    else
      c = "---";
  }
  else c = "###";
  (*stream) << "\n" << c << " TOP LEVEL MARK " << c << "\n";

  // Emit subcircuit components
  createSubNetlistPlain(stream, ErrText, NumPorts);

  a_Signals.clear();  // was filled in "giveNodeNames()"
  return true;
}

//#define VHDL_SIGNAL_TYPE "bit"
//#define VHDL_LIBRARIES   ""
#define VHDL_SIGNAL_TYPE "std_logic"
#define VHDL_LIBRARIES   "\nlibrary ieee;\nuse ieee.std_logic_1164.all;\n"

// ---------------------------------------------------
void Schematic::createSubNetlistPlain(QTextStream *stream, QPlainTextEdit *ErrText,
                                      int NumPorts)
{
  int i, z;
  QString s;
  QStringList SubcircuitPortNames;
  QStringList SubcircuitPortTypes;
  QStringList InPorts;
  QStringList OutPorts;
  QStringList InOutPorts;
  QStringList::iterator it_name;
  QStringList::iterator it_type;
  Component *pc;

  // probably creating a library currently
  QTextStream * tstream = stream;
  QFile ofile;
  if(a_creatingLib) {
    QString f = misc::properAbsFileName(a_DocName) + ".lst";
    ofile.setFileName(f);
    if(!ofile.open(QIODevice::WriteOnly)) {
      ErrText->appendPlainText(tr("ERROR: Cannot create library file \"%s\".").arg(f));
      return;
    }
    tstream = new QTextStream(&ofile);
  }

  // collect subcircuit ports and sort their node names into
  // "SubcircuitPortNames"
  a_PortTypes.clear();
  for(pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
    if(pc->Model.at(0) == '.') { // no simulations in subcircuits
      ErrText->appendPlainText(
        QObject::tr("WARNING: Ignore simulation component in subcircuit \"%1\".").arg(a_DocName)+"\n");
      continue;
    }
    else if(pc->Model == "Port") {
      i = pc->Props.first()->Value.toInt();
      for(z=SubcircuitPortNames.size(); z<i; z++) { // add empty port names
        SubcircuitPortNames.append(" ");
        SubcircuitPortTypes.append(" ");
      }
      it_name = SubcircuitPortNames.begin();
      it_type = SubcircuitPortTypes.begin();
      for(int n=1;n<i;n++)
      {
        it_name++;
        it_type++;
      }
      (*it_name) = pc->Ports.first()->Connection->Name;
      DigMap::Iterator it = a_Signals.find(*it_name);
      if(it!=a_Signals.end())
        (*it_type) = it.value().Type;
      // propagate type to port symbol
      pc->Ports.first()->Connection->DType = *it_type;

      if(!a_isAnalog) {
        if (a_isVerilog) {
          a_Signals.remove(*it_name); // remove node name
          switch(pc->Props.at(1)->Value.at(0).toLatin1()) {
            case 'a':
              InOutPorts.append(*it_name);
              break;
            case 'o':
              OutPorts.append(*it_name);
              break;
              default:
                InPorts.append(*it_name);
          }
        }
        else {
          // remove node name of output port
          a_Signals.remove(*it_name);
          switch(pc->Props.at(1)->Value.at(0).toLatin1()) {
            case 'a':
              (*it_name) += " : inout"; // attribute "analog" is "inout"
              break;
            case 'o': // output ports need workaround
              a_Signals.insert(*it_name, DigSignal(*it_name, *it_type));
              (*it_name) = "net_out" + (*it_name);
              (*it_name) += " : " + pc->Props.at(1)->Value;
              break;
            default:
              (*it_name) += " : " + pc->Props.at(1)->Value;
          }
          (*it_name) += " " + ((*it_type).isEmpty() ?
          VHDL_SIGNAL_TYPE : (*it_type));
        }
      }
    }
  }

  // remove empty subcircuit ports (missing port numbers)
  for(it_name = SubcircuitPortNames.begin(),
      it_type = SubcircuitPortTypes.begin();
      it_name != SubcircuitPortNames.end(); ) {
    if(*it_name == " ") {
      it_name = SubcircuitPortNames.erase(it_name);
      it_type = SubcircuitPortTypes.erase(it_type);
    } else {
      a_PortTypes.append(*it_type);
      it_name++;
      it_type++;
    }
  }

  QString f = misc::properFileName(a_DocName);
  QString Type = misc::properName(f);

  Painting *pi;



  if (QucsSettings.DefaultSimulator == spicecompat::simQucsator ||
      !a_isAnalog) {

        if(a_isAnalog) {
            // ..... analog subcircuit ...................................
            (*tstream) << "\n.Def:" << Type << " " << SubcircuitPortNames.join(" ");
            for(pi = a_SymbolPaints.first(); pi != 0; pi = a_SymbolPaints.next())
              if(pi->Name == ".ID ") {
                ID_Text *pid = (ID_Text*)pi;
                QList<SubParameter *>::const_iterator it;
                for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
                  s = (*it)->Name; // keep 'Name' unchanged
                  (*tstream) << " " << s.replace("=", "=\"") << '"';
                }
                break;
              }
            (*tstream) << '\n';

            // write all components with node names into netlist file
            for(pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next())
              (*tstream) << pc->getNetlist();

            (*tstream) << ".Def:End\n";

          }
          else {
            if (a_isVerilog) {
              // ..... digital subcircuit ...................................
              (*tstream) << "\nmodule Sub_" << Type << " ("
                      << SubcircuitPortNames.join(", ") << ");\n";

              // subcircuit in/out connections
              if(!InPorts.isEmpty())
                (*tstream) << " input " << InPorts.join(", ") << ";\n";
              if(!OutPorts.isEmpty())
                (*tstream) << " output " << OutPorts.join(", ") << ";\n";
              if(!InOutPorts.isEmpty())
                (*tstream) << " inout " << InOutPorts.join(", ") << ";\n";

              // subcircuit connections
              if(!a_Signals.isEmpty()) {
                QList<DigSignal> values = a_Signals.values();
                QList<DigSignal>::const_iterator it;
                for (it = values.constBegin(); it != values.constEnd(); ++it) {
                  (*tstream) << " wire " << (*it).Name << ";\n";
                }
              }
              (*tstream) << "\n";

              // subcircuit parameters
              for(pi = a_SymbolPaints.first(); pi != 0; pi = a_SymbolPaints.next())
                if(pi->Name == ".ID ") {
                  QList<SubParameter *>::const_iterator it;
                  ID_Text *pid = (ID_Text*)pi;
                  for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
                    s = (*it)->Name.section('=', 0,0);
                    QString v = misc::Verilog_Param((*it)->Name.section('=', 1,1));
                    (*tstream) << " parameter " << s << " = " << v << ";\n";
                  }
                  (*tstream) << "\n";
                  break;
                }

              // write all equations into netlist file
              for(pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
                if(pc->Model == "Eqn") {
                  (*tstream) << pc->get_Verilog_Code(NumPorts);
                }
              }

              if(a_Signals.find("gnd") != a_Signals.end())
              (*tstream) << " assign gnd = 0;\n"; // should appear only once

              // write all components into netlist file
              for(pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
                if(pc->Model != "Eqn") {
                  s = pc->get_Verilog_Code(NumPorts);
                  if(s.length()>0 && s.at(0) == '\xA7') {  //section symbol
                    ErrText->insertPlainText(s.mid(1));
                  }
                  else (*tstream) << s;
                }
              }

              (*tstream) << "endmodule\n";
            } else {
              // ..... digital subcircuit ...................................
              (*tstream) << VHDL_LIBRARIES;
              (*tstream) << "entity Sub_" << Type << " is\n";

              QString generic_str;
              for(pi = a_SymbolPaints.first(); pi != 0; pi = a_SymbolPaints.next()) {
                if(pi->Name == ".ID ") {
                  ID_Text *pid = (ID_Text*)pi;
                  QList<SubParameter *>::const_iterator it;



                  for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
                    s = (*it)->Name;
                    QString t = (*it)->Type.isEmpty() ? "real" : (*it)->Type;
                    generic_str += s.replace("=", " : "+t+" := ") + ";\n ";
                  }


                  break;
                }
              }
              if (!generic_str.isEmpty()) {
                (*tstream) << " generic (";
                (*tstream) << generic_str;
                (*tstream) << ");\n";
              }

              (*tstream) << " port ("
                        << SubcircuitPortNames.join(";\n ") << ");\n";


              (*tstream) << "end entity;\n"
                          << "use work.all;\n"
                          << "architecture Arch_Sub_" << Type << " of Sub_" << Type
                          << " is\n";

              if(!a_Signals.isEmpty()) {
                QList<DigSignal> values = a_Signals.values();
                QList<DigSignal>::const_iterator it;
                for (it = values.constBegin(); it != values.constEnd(); ++it) {
                  (*tstream) << " signal " << (*it).Name << " : "
                  << ((*it).Type.isEmpty() ?
                  VHDL_SIGNAL_TYPE : (*it).Type) << ";\n";
                }
              }

              // write all equations into netlist file
              for(pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
                if(pc->Model == "Eqn") {
                  ErrText->insertPlainText(
                              QObject::tr("WARNING: Equations in \"%1\" are 'time' typed.").
                  arg(pc->Name));
                  (*tstream) << pc->get_VHDL_Code(NumPorts);
                }
              }

              (*tstream) << "begin\n";

              if(a_Signals.find("gnd") != a_Signals.end())
              (*tstream) << " gnd <= '0';\n"; // should appear only once

              // write all components into netlist file
              for(pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
                if(pc->Model != "Eqn") {
                    s = pc->get_VHDL_Code(NumPorts);
                    if(s.length()>0 && s.at(0) == '\xA7') {  //section symbol
                      ErrText->insertPlainText(s.mid(1));
                  }
                  else (*tstream) << s;
                }
              }

              (*tstream) << "end architecture;\n";
            }
          }

    }


  // close file
  if(a_creatingLib) {
    ofile.close();
    delete tstream;
  }
}
// ---------------------------------------------------
// Write the netlist as subcircuit to the text stream 'stream'.
bool Schematic::createSubNetlist(QTextStream *stream, int& countInit,
                     QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts)
{
//  int Collect_count = Collect.count();   // position for this subcircuit

  // TODO: NodeSets have to be put into the subcircuit block.
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts)){
    fprintf(stderr, "Error giving NodeNames in createSubNetlist\n");
    return false;
  }

/*  Example for TODO
      for(it = Collect.at(Collect_count); it != Collect.end(); )
      if((*it).left(4) == "use ") {  // output all subcircuit uses
        (*stream) << (*it);
        it = Collect.remove(it);
      }
      else it++;*/

  // Emit subcircuit components
   createSubNetlistPlain(stream, ErrText, NumPorts);
   if (QucsSettings.DefaultSimulator != spicecompat::simQucsator &&
       a_isAnalog) {
      AbstractSpiceKernel *kern = new AbstractSpiceKernel(this);
      QStringList err_lst;
      if (!kern->checkSchematic(err_lst)) {
          QString s = QStringLiteral("Subcircuit %1 contains SPICE-incompatible components.\n"
                              "Check these components: %2 \n")
                  .arg(this->a_DocName).arg(err_lst.join("; "));
          ErrText->insertPlainText(s);
          return false;
      }
      kern->createSubNetlsit(*stream);

      delete kern;
  }


  a_Signals.clear();  // was filled in "giveNodeNames()"
  return true;
}

// ---------------------------------------------------
// Detect simulation domain (analog/digital) by looking at component types.
bool Schematic::isDigitalCircuit()
{
  for(Component *pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
      if(pc->isActive == COMP_IS_OPEN) continue;
      if(pc->Model.at(0) == '.' && pc->Model == ".Digi") {
          return true;  // Verilog simulation detected
      }
  }
  return false;  // Verilog simulation not found
}

// ---------------------------------------------------
// Determines the node names and writes subcircuits into  file.
int Schematic::prepareNetlist(QTextStream& stream, QStringList& Collect,
                              QPlainTextEdit *ErrText)
{
  if(a_showBias > 0) a_showBias = -1;  // do not show DC bias anymore

  a_isVerilog = false;
  a_isAnalog = true;
  bool isTruthTable = false;
  int allTypes = 0, NumPorts = 0;

  // Detect simulation domain (analog/digital) by looking at component types.
  for(Component *pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
    if(pc->isActive == COMP_IS_OPEN) continue;
    if(pc->Model.at(0) == '.') {
      if(pc->Model == ".Digi") {
        if(allTypes & isDigitalComponent) {
          ErrText->appendPlainText(
             QObject::tr("ERROR: Only one digital simulation allowed."));
          return -10;
        }
        if(pc->Props.front()->Value != "TimeList")
          isTruthTable = true;
        if(pc->Props.back()->Value != "VHDL")
          a_isVerilog = true;
        allTypes |= isDigitalComponent;
        a_isAnalog = false;
      }
      else allTypes |= isAnalogComponent;
      if((allTypes & isComponent) == isComponent) {
        ErrText->appendPlainText(
           QObject::tr("ERROR: Analog and digital simulations cannot be mixed."));
        return -10;
      }
    }
    else if(pc->Model == "DigiSource") NumPorts++;
  }

  if((allTypes & isAnalogComponent) == 0) {
    if(allTypes == 0) {
      // If no simulation exists, assume analog simulation. There may
      // be a simulation within a SPICE file. Otherwise Qucsator will
      // output an error.
      a_isAnalog = true;
      allTypes |= isAnalogComponent;
      NumPorts = -1;
    }
    else {
      if(NumPorts < 1 && isTruthTable) {
        ErrText->appendPlainText(
           QObject::tr("ERROR: Digital simulation needs at least one digital source."));
        return -10;
      }
      if(!isTruthTable) NumPorts = 0;
    }
  }
  else {
    NumPorts = -1;
    a_isAnalog = true;
  }

  // first line is documentation
  bool has_header = true;
  if(allTypes & isAnalogComponent) {
    if (QucsSettings.DefaultSimulator != spicecompat::simQucsator) {
      has_header = false;
    } else {
      stream << '#';
    }
  } else if (a_isVerilog) {
    stream << "//";
  } else {
    stream << "--";
  }
  if (has_header) {
    stream << " Qucs " << PACKAGE_VERSION << "  " << a_DocName << "\n";
  }

  // set timescale property for verilog schematics
  if (a_isVerilog) {
    stream << "\n`timescale 1ps/100fs\n";
  }

  int countInit = 0;  // counts the nodesets to give them unique names

  if(!giveNodeNames(&stream, countInit, Collect, ErrText, NumPorts)){
    fprintf(stderr, "Error giving NodeNames\n");
    return -10;
  }

  if(allTypes & isAnalogComponent){
    return NumPorts;
  }

  if (!a_isVerilog) {
    stream << VHDL_LIBRARIES;
    stream << "entity TestBench is\n"
      << "end entity;\n"
      << "use work.all;\n";
  }
  return NumPorts;
}

// ---------------------------------------------------
// Write the beginning of digital netlist to the text stream 'stream'.
void Schematic::beginNetlistDigital(QTextStream& stream)
{
  if (a_isVerilog) {
    stream << "module TestBench ();\n";
    QList<DigSignal> values = a_Signals.values();
    QList<DigSignal>::const_iterator it;
    for (it = values.constBegin(); it != values.constEnd(); ++it) {
      stream << "  wire " << (*it).Name << ";\n";
    }
    stream << "\n";
  } else {
    stream << "architecture Arch_TestBench of TestBench is\n";
    QList<DigSignal> values = a_Signals.values();
    QList<DigSignal>::const_iterator it;
    for (it = values.constBegin(); it != values.constEnd(); ++it) {
      stream << "  signal " << (*it).Name << " : "
        << ((*it).Type.isEmpty() ?
        VHDL_SIGNAL_TYPE : (*it).Type) << ";\n";
    }
    stream << "begin\n";
  }

  if(a_Signals.find("gnd") != a_Signals.end()) {
    if (a_isVerilog) {
      stream << "  assign gnd = 0;\n";
    } else {
      stream << "  gnd <= '0';\n";  // should appear only once
    }
  }
}

// ---------------------------------------------------
// Write the end of digital netlist to the text stream 'stream'.
void Schematic::endNetlistDigital(QTextStream& stream)
{
  if (a_isVerilog) {
  } else {
    stream << "end architecture;\n";
  }
}

// ---------------------------------------------------
// write all components with node names into the netlist file
QString Schematic::createNetlist(QTextStream& stream, int NumPorts)
{
  if(!a_isAnalog) {
    beginNetlistDigital(stream);
  }

  a_Signals.clear();  // was filled in "giveNodeNames()"
  FileList.clear();

  QString s, Time;
  for(Component *pc = a_DocComps.first(); pc != 0; pc = a_DocComps.next()) {
    if(a_isAnalog) {
      s = pc->getNetlist();
    }
    else {
      if(pc->Model == ".Digi" && pc->isActive) {  // simulation component ?
        if(NumPorts > 0) { // truth table simulation ?
          if (a_isVerilog)
            Time = QString::number((1 << NumPorts));
          else
            Time = QString::number((1 << NumPorts) - 1) + " ns";
        } else {
          Time = pc->Props.at(1)->Value;
        if (a_isVerilog) {
          if(!misc::Verilog_Time(Time, pc->Name)) return Time;
        } else {
          if(!misc::VHDL_Time(Time, pc->Name)) return Time;  // wrong time format
        }
        }
      }
      if (a_isVerilog) {
        s = pc->get_Verilog_Code(NumPorts);
      } else {
        s = pc->get_VHDL_Code(NumPorts);
      }
      if (s.length()>0 && s.at(0) == '\xA7'){
          return s; // return error
      }
    }
    stream << s;
  }

  if(!a_isAnalog) {
    endNetlistDigital(stream);
  }

  return Time;
}


void Schematic::clearSignalsAndFileList()
{
    a_Signals.clear();  // was filled in "giveNodeNames()"
    FileList.clear();
}

void Schematic::clearSignals()
{
    a_Signals.clear();
}
// vim:ts=8:sw=2:noet
