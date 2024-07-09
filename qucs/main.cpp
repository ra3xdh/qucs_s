/***************************************************************************
                                 main.cpp
                                ----------
    begin                : Thu Aug 28 2003
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
/*!
 * \file main.cpp
 * \brief Implementation of the main application.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <locale.h>

#include <QApplication>
#include <QString>
#include <QStringList>
//#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QtSvg>

#include "qucs.h"
#include "main.h"
#include "node.h"
#include "printerwriter.h"
#include "imagewriter.h"
#include "schematic.h"
#include "settings.h"
#include "module.h"
#include "misc.h"


#include "extsimkernels/ngspice.h"
#include "extsimkernels/xyce.h"

#if defined(_WIN32) ||defined(__MINGW32__)
#include <windows.h>  //for OutputDebugString
#endif

#if defined(_WIN32) ||defined(__MINGW32__)
#define executableSuffix ".exe"
#else
#define executableSuffix ""
#endif

tQucsSettings QucsSettings;

QucsApp *QucsMain = 0;  // the Qucs application itself
QString lastDir;    // to remember last directory for several dialogs
QStringList qucsPathList;
VersionTriplet QucsVersion; // Qucs version string

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs_s");

    QucsSettings.DefaultSimulator = _settings::Get().item<int>("DefaultSimulator");
    QucsSettings.firstRun = _settings::Get().item<bool>("firstRun");

    /*** Temporarily continue to use QucsSettings to make sure all settings convert okay and remain compatible ***/
    QucsSettings.font.fromString(_settings::Get().item<QString>("font"));
    QucsSettings.appFont.fromString(_settings::Get().item<QString>("appFont"));
    QucsSettings.textFont.fromString(_settings::Get().item<QString>("textFont"));
    QucsSettings.largeFontSize = _settings::Get().item<double>("LargeFontSize");
    QucsSettings.maxUndo = _settings::Get().item<int>("maxUndo");
    QucsSettings.NodeWiring = _settings::Get().item<int>("NodeWiring");
    QucsSettings.BGColor = _settings::Get().item<QString>("BGColor");
    QucsSettings.Editor = _settings::Get().item<QString>("Editor");
    QucsSettings.FileTypes = _settings::Get().item<QStringList>("FileTypes");
    QucsSettings.Language = _settings::Get().item<QString>("Language");

    // Editor syntax highlighting settings.
    QucsSettings.Comment = _settings::Get().item<QString>("Comment");
    QucsSettings.String = _settings::Get().item<QString>("String");
    QucsSettings.Integer = _settings::Get().item<QString>("Integer");
    QucsSettings.Real = _settings::Get().item<QString>("Real");
    QucsSettings.Character = _settings::Get().item<QString>("Character");
    QucsSettings.Type = _settings::Get().item<QString>("Type");
    QucsSettings.Attribute = _settings::Get().item<QString>("Attribute");
    QucsSettings.Directive = _settings::Get().item<QString>("Directive");
    QucsSettings.Task = _settings::Get().item<QString>("Task");

    QucsSettings.panelIconsTheme = _settings::Get().item<int>("panelIconsTheme");
    QucsSettings.compIconsTheme = _settings::Get().item<int>("compIconsTheme");

    // TODO: Convert this to the new settings model.
    if(settings.contains("Qucsator")) {
        QucsSettings.Qucsator = settings.value("Qucsator").toString();
        QFileInfo inf(QucsSettings.Qucsator);
        QucsSettings.QucsatorDir = inf.canonicalPath() + QDir::separator();
        if (QucsSettings.Qucsconv.isEmpty())
            QucsSettings.Qucsconv = QucsSettings.QucsatorDir + QDir::separator() + "qucsconv_rf" + executableSuffix;
    } else {
        QucsSettings.Qucsator = QucsSettings.BinDir + "qucsator_rf" + executableSuffix;
        QucsSettings.QucsatorDir = QucsSettings.BinDir;
        if (QucsSettings.Qucsconv.isEmpty())
            QucsSettings.Qucsconv = QucsSettings.BinDir + "qucsconv_rf" + executableSuffix;
    }

    QucsSettings.AdmsXmlBinDir.setPath(_settings::Get().item<QString>("AdmsXmlBinDir"));
    QucsSettings.AscoBinDir.setPath(_settings::Get().item<QString>("AscoBinDir"));
    QucsSettings.NgspiceExecutable = _settings::Get().item<QString>("NgspiceExecutable");
    QucsSettings.XyceExecutable = _settings::Get().item<QString>("XyceExecutable");
    QucsSettings.XyceParExecutable = _settings::Get().item<QString>("XyceParExecutable");
    QucsSettings.SpiceOpusExecutable = _settings::Get().item<QString>("SpiceOpusExecutable");
    QucsSettings.NProcs = _settings::Get().item<int>("Nprocs");
   
    // TODO: Currently the default settings cannot include other settings during initialisation. This is a 
    // problem for this setting as it needs to include the QucsWorkDir setting. Therefore, set the default to an
    // empty string and populate it here by brute force.
    QucsSettings.S4Qworkdir = _settings::Get().item<QString>("S4Q_workdir");
    if (QucsSettings.S4Qworkdir == "")
      QucsSettings.S4Qworkdir = QDir::toNativeSeparators(QucsSettings.QucsWorkDir.absolutePath()+"/spice4qucs");

    QucsSettings.SimParameters = _settings::Get().item<QString>("SimParameters");
    QucsSettings.OctaveExecutable = _settings::Get().item<QString>("OctaveExecutable");
    QucsSettings.OctaveExecutable = _settings::Get().item<QString>("OctaveBinDir");
    QucsSettings.OpenVAFExecutable = _settings::Get().item<QString>("OpenVAFExecutable");

    QucsSettings.RFLayoutExecutable = _settings::Get().item<QString>("RFLayoutExecutable");

    QucsSettings.qucsWorkspaceDir.setPath(_settings::Get().item<QString>("QucsHomeDir"));
    QucsSettings.QucsWorkDir.setPath( QucsSettings.qucsWorkspaceDir.path());
    QucsSettings.tempFilesDir.setPath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    QucsSettings.IgnoreFutureVersion = _settings::Get().item<bool>("IgnoreVersion");
    QucsSettings.GraphAntiAliasing = _settings::Get().item<bool>("GraphAntiAliasing");
    QucsSettings.TextAntiAliasing = _settings::Get().item<bool>("TextAntiAliasing");
    QucsSettings.fullTraceName = _settings::Get().item<bool>("fullTraceName");
    QucsSettings.FileToolbar = _settings::Get().item<bool>("FileToolbar");
    QucsSettings.EditToolbar = _settings::Get().item<bool>("EditToolbar");
    QucsSettings.ViewToolbar = _settings::Get().item<bool>("ViewToolbar");
    QucsSettings.WorkToolbar = _settings::Get().item<bool>("WorkToolbar");
    QucsSettings.SimulateToolbar = _settings::Get().item<bool>("SimulateToolbar");
    QucsSettings.RecentDocs = _settings::Get().item<QString>("RecentDocs").split("*",qucs::SkipEmptyParts);
    QucsSettings.numRecentDocs = QucsSettings.RecentDocs.count();
    QucsSettings.spiceExtensions << "*.sp" << "*.cir" << "*.spc" << "*.spi";

    // If present read in the list of directory paths in which Qucs should
    // search for subcircuit schematics
    int npaths = settings.beginReadArray("Paths");
    for (int i = 0; i < npaths; ++i)
    {
        settings.setArrayIndex(i);
        QString apath = settings.value("path").toString();
        qucsPathList.append(apath);
    }
    settings.endArray();

    QucsSettings.numRecentDocs = 0;

    return true;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings()
{
    QSettings settings ("qucs","qucs_s");

    // Note: It is not really necessary to take the following reference, but it 
    // arguably makes the code slightly cleaner - thoughts? To be clear:
    // qs.item<int>() is identical to _settings::get().item<int>()
    settingsManager& qs = _settings::Get();    

    qs.setItem<int>("DefaultSimulator", QucsSettings.DefaultSimulator);
    qs.setItem<bool>("firstRun", false);
    qs.setItem<QString>("font", QucsSettings.font.toString());
    qs.setItem<QString>("appFont", QucsSettings.appFont.toString());
    qs.setItem<QString>("textFont", QucsSettings.textFont.toString());
    qs.setItem<QByteArray>("MainWindowGeometry", QucsMain->saveGeometry());
    
    // store LargeFontSize as a string, so it will be also human-readable in the settings file (will be a @Variant() otherwise)
    qs.setItem<QString>("LargeFontSize", QString::number(QucsSettings.largeFontSize));
    qs.setItem<unsigned int>("maxUndo", QucsSettings.maxUndo);
    qs.setItem<unsigned int>("NodeWiring", QucsSettings.NodeWiring);
    qs.setItem<QString>("BGColor", QucsSettings.BGColor.name());
    qs.setItem<QString>("Editor", QucsSettings.Editor);
    qs.setItem<QStringList>("FileTypes", QucsSettings.FileTypes);
    qs.setItem<QString>("Language", QucsSettings.Language);
    qs.setItem<QString>("Comment", QucsSettings.Comment.name());
    qs.setItem<QString>("String", QucsSettings.String.name());
    qs.setItem<QString>("Integer", QucsSettings.Integer.name());
    qs.setItem<QString>("Real", QucsSettings.Real.name());
    qs.setItem<QString>("Character", QucsSettings.Character.name());
    qs.setItem<QString>("Type", QucsSettings.Type.name());
    qs.setItem<QString>("Attribute", QucsSettings.Attribute.name());
    qs.setItem<QString>("Directive", QucsSettings.Directive.name());
    qs.setItem<QString>("Task", QucsSettings.Task.name());
    qs.setItem<QString>("AdmsXmlBinDir", QucsSettings.AdmsXmlBinDir.canonicalPath());
    qs.setItem<QString>("AscoBinDir", QucsSettings.AscoBinDir.canonicalPath());    
    qs.setItem<QString>("NgspiceExecutable",QucsSettings.NgspiceExecutable);
    qs.setItem<QString>("XyceExecutable",QucsSettings.XyceExecutable);
    qs.setItem<QString>("XyceParExecutable",QucsSettings.XyceParExecutable);
    qs.setItem<QString>("SpiceOpusExecutable",QucsSettings.SpiceOpusExecutable);
    qs.setItem<QString>("Qucsator",QucsSettings.Qucsator);
    qs.setItem<int>("Nprocs",QucsSettings.NProcs);
    qs.setItem<QString>("S4Q_workdir",QucsSettings.S4Qworkdir);
    qs.setItem<QString>("SimParameters",QucsSettings.SimParameters);    
    qs.setItem<QString>("OctaveExecutable",QucsSettings.OctaveExecutable);
    qs.setItem<QString>("OpenVAFExecutable",QucsSettings.OpenVAFExecutable);
    qs.setItem<QString>("QucsHomeDir", QucsSettings.qucsWorkspaceDir.canonicalPath());
    qs.setItem<bool>("IgnoreVersion", QucsSettings.IgnoreFutureVersion);
    qs.setItem<bool>("GraphAntiAliasing", QucsSettings.GraphAntiAliasing);
    qs.setItem<bool>("TextAntiAliasing", QucsSettings.TextAntiAliasing);
    qs.setItem<bool>("fullTraceName",QucsSettings.fullTraceName);
    qs.setItem<int>("panelIconsTheme",QucsSettings.panelIconsTheme);
    qs.setItem<int>("compIconsTheme",QucsSettings.compIconsTheme);
    qs.setItem<bool>("FileToolbar", QucsSettings.FileToolbar);
    qs.setItem<bool>("EditToolbar", QucsSettings.EditToolbar);
    qs.setItem<bool>("ViewToolbar", QucsSettings.ViewToolbar);
    qs.setItem<bool>("WorkToolbar", QucsSettings.WorkToolbar);
    qs.setItem<bool>("SimulateToolbar", QucsSettings.SimulateToolbar);

    // Copy the list of directory paths in which Qucs should
    // search for subcircuit schematics from qucsPathList
    settings.remove("Paths");
    settings.beginWriteArray("Paths");
    int i = 0;
    for (QString& path: qucsPathList) {
         settings.setArrayIndex(i);
         settings.setValue("path", path);
         i++;
     }
     settings.endArray();

  return true;
}

/*!
 * \brief qucsMessageOutput handles qDebug, qWarning, qCritical, qFatal.
 * \param type Message type (Qt enum)
 * \param msg Message
 *
 * The message handler is used to get control of the messages.
 * Particularly on Windows, as the messages are sent to the debugger and do not
 * show on the terminal. The handler could also be extended to create a log
 * mechanism.
 * <http://qt-project.org/doc/qt-4.8/debug.html#warning-and-debugging-messages>
 * <http://qt-project.org/doc/qt-4.8/qtglobal.html#qInstallMsgHandler>
 */
void qucsMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
    }
    fflush(stderr);
}

Schematic *openSchematic(QString schematic)
{
  qDebug() << "*** try to load schematic :" << schematic;

  QFile file(schematic);  // save simulator messages
  if(file.open(QIODevice::ReadOnly)) {
    file.close();
  }
  else {
    fprintf(stderr, "Error: Could not load schematic %s\n", schematic.toLatin1().data());
    return NULL;
  }

  // populate Modules list
  //Module::registerModules ();

  // new schematic from file
  Schematic *sch = new Schematic(0, schematic);

  // load schematic file if possible
  if(!sch->loadDocument()) {
    fprintf(stderr, "Error: Could not load schematic %s\n", schematic.toLatin1().data());
    delete sch;
    return NULL;
  }
  return sch;
}

int doNetlist(QString schematic, QString netlist)
{
  QucsSettings.DefaultSimulator = spicecompat::simQucsator;
  Module::registerModules();
  Schematic *sch = openSchematic(schematic);
  if (sch == NULL) {
    return 1;
  }

  qDebug() << "*** try to write netlist  :" << netlist;

  QStringList Collect;

  QPlainTextEdit *ErrText = new QPlainTextEdit();  //dummy
  QFile NetlistFile;
  QTextStream   Stream;

  Collect.clear();  // clear list for NodeSets, SPICE components etc.

  NetlistFile.setFileName(netlist);
  if(!NetlistFile.open(QIODevice::WriteOnly)) {
    fprintf(stderr, "Error: Could not load netlist %s\n", netlist.toLatin1().data());
    return -1;
  }

  Stream.setDevice(&NetlistFile);
  int SimPorts = sch->prepareNetlist(Stream, Collect, ErrText);

  if(SimPorts < -5) {
    NetlistFile.close();
    QByteArray ba = netlist.toLatin1();
    fprintf(stderr, "Error: Could not prepare netlist %s\n", ba.data());
    /// \todo better handling for error/warnings
    qCritical() << ErrText->toPlainText();
    return 1;
  }

  // output NodeSets, SPICE simulations etc.
  for(QStringList::Iterator it = Collect.begin();
  it != Collect.end(); ++it) {
    // don't put library includes into netlist...
    if ((*it).right(4) != ".lst" &&
    (*it).right(5) != ".vhdl" &&
    (*it).right(4) != ".vhd" &&
    (*it).right(2) != ".v") {
      Stream << *it << '\n';
    }
  }

  Stream << '\n';

  QString SimTime = sch->createNetlist(Stream, SimPorts);
  delete(sch);

  NetlistFile.close();

  return 0;
}

int runNgspice(QString schematic, QString dataset)
{
    QucsSettings.DefaultSimulator = spicecompat::simNgspice;
    Schematic *sch = openSchematic(schematic);
    if (sch == NULL) {
      return 1;
    }

    Ngspice *ngspice = new Ngspice(sch);
    ngspice->slotSimulate();
    bool ok = ngspice->waitEndOfSimulation();
    if (!ok) {
        fprintf(stderr, "Ngspice timed out or start error!\n");
        delete ngspice;
        return -1;
    } else {
        ngspice->convertToQucsData(dataset);
    }

    delete ngspice;
    return 0;
}

int runXyce(QString schematic, QString dataset)
{
    QucsSettings.DefaultSimulator = spicecompat::simXyce;
    Schematic *sch = openSchematic(schematic);
    if (sch == NULL) {
      return 1;
    }

    Xyce *xyce = new Xyce(sch);
    xyce->slotSimulate();
    bool ok = xyce->waitEndOfSimulation();
    if (!ok) {
        fprintf(stderr, "Xyce timed out or start error!\n");
        delete xyce;
        return -1;
    } else {
        xyce->convertToQucsData(dataset);
    }

    delete xyce;
    return 0;
}

int doNgspiceNetlist(QString schematic, QString netlist)
{
    QucsSettings.DefaultSimulator = spicecompat::simNgspice;
    Module::registerModules();
    Schematic *sch = openSchematic(schematic);
    if (sch == NULL) {
      return 1;
    }
    Ngspice *ngspice = new Ngspice(sch);
    ngspice->SaveNetlist(netlist);
    delete ngspice;

    if (!QFile::exists(netlist)) return -1;
    else return 0;
}

int doXyceNetlist(QString schematic, QString netlist)
{
    QucsSettings.DefaultSimulator = spicecompat::simXyce;
    Module::registerModules();
    Schematic *sch = openSchematic(schematic);
    if (sch == NULL) {
      return 1;
    }
    Xyce *xyce = new Xyce(sch);
    xyce->SaveNetlist(netlist);
    delete xyce;

    if (!QFile::exists(netlist)) return -1;
    else return 0;
}

int doPrint(QString schematic, QString printFile,
    QString page, int dpi, QString color, QString orientation)
{
  QucsSettings.DefaultSimulator = spicecompat::simQucsator;
  Schematic *sch = openSchematic(schematic);
  if (sch == NULL) {
    return 1;
  }

  sch->Nodes = &(sch->DocNodes);
  sch->Wires = &(sch->DocWires);
  sch->Diagrams = &(sch->DocDiags);
  sch->Paintings = &(sch->DocPaints);
  sch->Components = &(sch->DocComps);
  sch->reloadGraphs();

  qDebug() << "*** try to print file  :" << printFile;

  // determine filetype
  if (printFile.endsWith(".pdf")) {
    //initial printer
    PrinterWriter *Printer = new PrinterWriter();
    Printer->setFitToPage(true);
    Printer->noGuiPrint(sch, printFile, page, dpi, color, orientation);
  } else {
    ImageWriter *Printer = new ImageWriter("");
    Printer->noGuiPrint(sch, printFile, color);
  }
  return 0;
}

/*!
 * \brief createIcons Create component icons (png) from command line.
 */
void createIcons() {

  int nCats = 0, nComps = 0;

  if(!QDir("./bitmaps_generated").exists()){
    QDir().mkdir("bitmaps_generated");
  }
  Module::registerModules ();
  QStringList cats = Category::getCategories ();

  for (const QString& category: cats) {

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // crash with diagrams, skip
    if(category == "diagrams") break;

    char * File;
    QString Name;

    for (Module *Mod: Comps) {
      if (Mod->info) {

        Element *e = (Mod->info) (Name, File, true);

        Component *c = (Component* ) e;

        QList<qucs::Line *> Lines      = c->Lines;
        QList<struct qucs::Arc *> Arcs = c-> Arcs;
        QList<qucs::Rect *> Rects      = c-> Rects;
        QList<qucs::Ellips *> Ellips     = c-> Ellipses;
        QList<Port *> Ports      = c->Ports;
        QList<Text*> Texts       = c->Texts;

        QGraphicsScene *scene = new QGraphicsScene();

        for (qucs::Line *l : Lines) {
          scene->addLine(l->x1, l->y1, l->x2, l->y2, l->style);
        }

        for (struct qucs::Arc *a: Arcs) {
          // we need an open item here; QGraphisEllipseItem draws a filled ellipse and doesn't do the job here...
          QPainterPath *path = new QPainterPath();
          // the components do not contain the angles in degrees but in 1/16th degrees -> conversion needed
          path->arcMoveTo(a->x,a->y,a->w,a->h,a->angle/16);
          path->arcTo(a->x,a->y,a->w,a->h,a->angle/16,a->arclen/16);
          scene->addPath(*path);
        }

        for(qucs::Rect *a: Rects) {
          scene->addRect(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        for(qucs::Ellips *a: Ellips) {
          scene->addEllipse(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        for(Port *p: Ports) {
          scene->addEllipse(p->x-4, p->y-4, 8, 8, QPen(Qt::red));
        }

        for(Text *t: Texts) {
          QFont myFont;
          myFont.setPointSize(10);
          QGraphicsTextItem* item  = new QGraphicsTextItem(t->s);
          item->setX(t->x);
          item->setY(t->y);
          item->setFont(myFont);

          scene->addItem(item);
        }

        // this uses the size of the component as icon size
        // Qt bug ? The returned sceneRect() is often 1 px short on bottom
        //   and right sides without anti-aliasing. 1 px more missing on top
        //   and left when anti-aliasing is used
        QRectF rScene = scene->sceneRect().adjusted(-1,-1,1,1);
        // image and scene need to be the same size, since render()
        //   will fill the entire image, otherwise the scaling will
        //   introduce artifacts
        QSize sImage = rScene.size().toSize(); // rounding seems not to be an issue
        // ARGB32_Premultiplied is faster (Qt docs)
        //QImage image(sImage.toSize(), QImage::Format_ARGB32);
        QImage image(sImage, QImage::Format_ARGB32_Premultiplied);
        // this uses a fixed size for the icon (32 x 32)
        //QImage image(32, 32, QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QPainter painter(&image);
        QPainter::RenderHints hints = QPainter::RenderHints();
        // Ask to antialias drawings if requested
        if (QucsSettings.GraphAntiAliasing) hints |= QPainter::Antialiasing;
        // Ask to antialias text if requested
        if (QucsSettings.TextAntiAliasing) hints |= QPainter::TextAntialiasing;
        painter.setRenderHints(hints);

        // pass target and source size eplicitly, otherwise sceneRect() is used
        //   for the source size, which is often wrong (see comment above)
        scene->render(&painter, image.rect(), rScene);

        image.save("./bitmaps_generated/" + QString(File) + ".png");

        fprintf(stdout, "[%s] %s\n", category.toLatin1().data(), File);
      }
      nComps++;
    } // module
    nCats++;
  } // category
  fprintf(stdout, "Created %i component icons from %i categories\n", nComps, nCats);
}

/*!
 * \brief createDocData Create data used for documentation.
 *
 * It creates the following:
 *  - list of categories: categories.txt
 *  - category directory, ex.: ./lumped components/
 *    - CSV with component data fields. Ex [component#]_data.csv
 *    - CSV with component properties. Ex [component#]_props.csv
 */
void createDocData() {

  QMap<int, QString> typeMap;
  typeMap.insert(0x30000, "Component");
  typeMap.insert(0x30002, "ComponentText");
  typeMap.insert(0x10000, "AnalogComponent");
  typeMap.insert(0x20000, "DigitalComponent") ;

  Module::registerModules ();
  QStringList cats = Category::getCategories ();
  int nCats = cats.size();

  QStringList catHeader;
  catHeader << "# Note: auto-generated file (changes will be lost on update)";
  QFile file("categories.txt");
  if (!file.open(QFile::WriteOnly | QFile::Text)) return;
  QTextStream out(&file);
  out << cats.join("\n");
  file.close();

  int nComps = 0;

  // table for quick reference, schematic and netlist entry
  for (const QString& category: cats) {

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") break;

    // one dir per category
    QString curDir = "./"+category+"/";
    qDebug() << "Creating dir:" << curDir;
    if(!QDir(curDir).exists()){
        QDir().mkdir(curDir);
    }

    char * File;
    QString Name;

    int num = 0; // component id inside category

    for (Module *Mod: Comps) {
        num += 1;

        nComps += 1;

        Element *e = (Mod->info) (Name, File, true);
        Component *c = (Component* ) e;

        // object info
        QStringList compData;

        compData << "# Note: auto-generated file (changes will be lost on update)";
        compData << "Caption; "           + Name;
        compData << "Description; "       + c->Description;
        compData << "Identifier; ``"      + c->Model + "``"; // backticks for reST verbatim
        compData << "Default name; ``"    + c->Name  + "``";
        compData << "Type; "              + typeMap.value(c->Type);
        compData << "Bitmap file; "       + QString(File);
        compData << "Properties; "        + QString::number(c->Props.count());
        compData << "Category; "          + category;

        // 001_data.csv - CSV file with component data
        QString ID = QString("%1").arg(num,3,'d',0,'0');
        QString objDataFile;
        objDataFile = QString("%1_data.csv").arg( ID  ) ;

        QFile file(curDir + objDataFile);
        if (!file.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream out(&file);
        out << compData.join("\n");
        file.close();
        fprintf(stdout, "[%s] %s %s \n", category.toLatin1().data(), c->Model.toLatin1().data(), file.fileName().toLatin1().data());

        QStringList compProps;
        compProps << "# Note: auto-generated file (changes will be lost on update)";
        compProps << QString("# %1; %2; %3; %4").arg(  "Name", "Value", "Display", "Description");
        for (Property *prop : c->Props) {
          compProps << QString("%1; \"%2\"; %3; \"%4\"").arg(
                         prop->Name,
                         prop->Value,
                         prop->display?"yes":"no",
                         prop->Description.replace("\"","\"\"")); // escape quote in quote
        }

        // 001_props.csv - CSV file with component properties
        QString objPropFile = QString("%1_prop.csv").arg( ID ) ;

        QFile fileProps(curDir + objPropFile );
        if (!fileProps.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream outProps(&fileProps);
        outProps << compProps.join("\n");
        compProps.clear();
        file.close();
        fprintf(stdout, "[%s] %s %s \n", category.toLatin1().data(), c->Model.toLatin1().data(), fileProps.fileName().toLatin1().data());
    } // module
  } // category
  fprintf(stdout, "Created data for %i components from %i categories\n", nComps, nCats);
}

/*!
 * \brief createListNetEntry prints to stdout the available netlist formats
 *
 * Prints the default component entries format for:
 *  - Qucs schematic
 *  - Qucsator netlist
 */
void createListComponentEntry(){

  Module::registerModules ();
  QStringList cats = Category::getCategories ();
  // table for quick reference, schematic and netlist entry
  for (QString category: cats) {

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") break;

    char * File;
    QString Name;

    for (Module *Mod: Comps) {
      Element *e = (Mod->info) (Name, File, true);
      Component *c = (Component* ) e;

      QString qucsEntry = c->save();
      fprintf(stdout, "%s; qucs    ; %s\n", c->Model.toLatin1().data(), qucsEntry.toLatin1().data());

      // add dummy ports/wires, avoid segfault
      int port = 0;
      for (Port *p: c->Ports) {
        Node *n = new Node(0,0);
        n->Name="_net"+QString::number(port);
        p->Connection = n;
        port +=1;
      }

      // skip Subcircuit, segfault, there is nothing to netlist
      if (c->Model == "Sub" or c->Model == ".Opt") {
        fprintf(stdout, "WARNING, qucsator netlist not generated for %s\n\n", c->Model.toLatin1().data());
        continue;
      }

      QString qucsatorEntry = c->getNetlist();
      fprintf(stdout, "%s; qucsator; %s\n", c->Model.toLatin1().data(), qucsatorEntry.toLatin1().data());
      } // module
    } // category
}

// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################
int main(int argc, char *argv[])
{
  qInstallMessageHandler(qucsMessageOutput);
  // set the Qucs version string
  QucsVersion = VersionTriplet(PACKAGE_VERSION);

  // apply default settings
  //QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.largeFontSize = 16.0;
  QucsSettings.maxUndo = 20;
  QucsSettings.NodeWiring = 0;

#if QT_VERSION < 0x060000
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling,true);
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps,true);
#endif

  // initially center the application
  QApplication a(argc, argv);
  //QDesktopWidget *d = a.desktop();
  QucsSettings.font = QApplication::font();
  QucsSettings.appFont = QApplication::font();
  QucsSettings.textFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  QucsSettings.font.setPointSize(12);

  // default
  QString QucsWorkdirPath = QDir::homePath()+QDir::toNativeSeparators ("/QucsWorkspace");
  QucsSettings.qucsWorkspaceDir.setPath(QucsWorkdirPath);
  QucsSettings.QucsWorkDir.setPath(QucsSettings.qucsWorkspaceDir.canonicalPath());

  // load existing settings (if any)
  loadSettings();

  QDir().mkpath(QucsSettings.qucsWorkspaceDir.absolutePath());
  QDir().mkpath(QucsSettings.tempFilesDir.absolutePath());

  // continue to set up overrides or default settings (some are saved on exit)

  // check for relocation env variable
  QDir QucsDir;
  QString QucsApplicationPath = QCoreApplication::applicationDirPath();
#ifdef __APPLE__
  QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
#else
  QucsDir.setPath(QucsApplicationPath);
  QucsDir.cdUp();
#endif

  QucsSettings.BinDir =      QucsDir.absolutePath() + "/bin/";
  QucsSettings.LangDir =     QucsDir.canonicalPath() + "/share/" QUCS_NAME "/lang/";

  QucsSettings.LibDir =      QucsDir.canonicalPath() + "/share/" QUCS_NAME "/library/";
  QucsSettings.OctaveDir =   QucsDir.canonicalPath() + "/share/" QUCS_NAME "/octave/";
  QucsSettings.ExamplesDir = QucsDir.canonicalPath() + "/share/" QUCS_NAME "/examples/";
  QucsSettings.DocDir =      QucsDir.canonicalPath() + "/share/" QUCS_NAME "/docs/";
  QucsSettings.Editor = "qucs";

  /// \todo Make the setting up of all executables below more consistent
  char *var = NULL; // Don't use QUCSDIR with Qucs-S
  var = getenv("QUCSATOR");
  if(var != NULL) {
      QucsSettings.QucsatorVar = QString(var);
  }
  else {
      QucsSettings.QucsatorVar = "";
  }

  var = getenv("QUCSCONV");
  if(var != NULL) {
      QucsSettings.Qucsconv = QString(var);
  }


  var = getenv("ADMSXMLBINDIR");
  if(var != NULL) {
      QucsSettings.AdmsXmlBinDir.setPath(QString(var));
  }
  else {
      // default admsXml bindir same as Qucs
      QString admsExec;
#if defined(_WIN32) || defined(__MINGW32__)
      admsExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"admsXml.exe");
#else
      admsExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"admsXml");
#endif
      QFile adms(admsExec);
      if(adms.exists())
        QucsSettings.AdmsXmlBinDir.setPath(QucsSettings.BinDir);
  }

  var = getenv("ASCOBINDIR");
  if(var != NULL)  {
      QucsSettings.AscoBinDir.setPath(QString(var));
  }
  else  {
      // default ASCO bindir same as Qucs
      QString ascoExec;
#if defined(_WIN32) || defined(__MINGW32__)
      ascoExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"asco.exe");
#else
      ascoExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"asco");
#endif
      QFile asco(ascoExec);
      if(asco.exists())
        QucsSettings.AscoBinDir.setPath(QucsSettings.BinDir);
  }


  var = getenv("QUCS_OCTAVE");
  if (var != NULL) {
      QucsSettings.QucsOctave = QString(var);
  } else {
      QucsSettings.QucsOctave.clear();
  }

  if(!QucsSettings.BGColor.isValid())
    QucsSettings.BGColor.setRgb(255, 250, 225);

  // syntax highlighting
  if(!QucsSettings.Comment.isValid())
    QucsSettings.Comment = Qt::gray;
  if(!QucsSettings.String.isValid())
    QucsSettings.String = Qt::red;
  if(!QucsSettings.Integer.isValid())
    QucsSettings.Integer = Qt::blue;
  if(!QucsSettings.Real.isValid())
    QucsSettings.Real = Qt::darkMagenta;
  if(!QucsSettings.Character.isValid())
    QucsSettings.Character = Qt::magenta;
  if(!QucsSettings.Type.isValid())
    QucsSettings.Type = Qt::darkRed;
  if(!QucsSettings.Attribute.isValid())
    QucsSettings.Attribute = Qt::darkCyan;
  if(!QucsSettings.Directive.isValid())
    QucsSettings.Directive = Qt::darkCyan;
  if(!QucsSettings.Task.isValid())
    QucsSettings.Task = Qt::darkRed;

  QucsSettings.sysDefaultFont = QApplication::font();
  QApplication::setFont(QucsSettings.appFont);

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty()) {
      QLocale loc;
      lang = loc.name();
//    lang = QTextCodec::locale();
  }
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  QApplication::installTranslator( &tor );

  // This seems to be necessary on a few system to make strtod()
  // work properly !???!
  setlocale (LC_NUMERIC, "C");

  QString inputfile;
  QString outputfile;

  bool netlist_flag = false;
  bool print_flag = false;
  bool ngspice_flag = false;
  bool xyce_flag = false;
  bool run_flag = false;
  QString page = "A4";
  int dpi = 96;
  QString color = "RGB";
  QString orientation = "portraid";

  // simple command line parser
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      fprintf(stdout,
  "Usage: %s [-hv] \n"
  "       qucs -n -i FILENAME -o FILENAME\n"
  "       qucs -p -i FILENAME -o FILENAME.[pdf|png|svg|eps] \n\n"
  "  -h, --help     display this help and exit\n"
  "  -v, --version  display version information and exit\n"
  "  -n, --netlist  convert Qucs schematic into netlist\n"
  "  -p, --print    print Qucs schematic to file (eps needs inkscape)\n"
  "    --page [A4|A3|B4|B5]         set print page size (default A4)\n"
  "    --dpi NUMBER                 set dpi value (default 96)\n"
  "    --color [RGB|RGB]            set color mode (default RGB)\n"
  "    --orin [portraid|landscape]  set orientation (default portraid)\n"
  "  -i FILENAME    use file as input schematic\n"
  "  -o FILENAME    use file as output netlist\n"
  "     --ngspice   create Ngspice netlist\n"
  "     --xyce      Xyce netlist\n"
  "     --run       execute Ngspice/Xyce immediately\n"
  "  -icons         create component icons under ./bitmaps_generated\n"
  "  -doc           dump data for documentation:\n"
  "                 * file with of categories: categories.txt\n"
  "                 * one directory per category (e.g. ./lumped components/)\n"
  "                   - CSV file with component data ([comp#]_data.csv)\n"
  "                   - CSV file with component properties. ([comp#]_props.csv)\n"
  "  -list-entries  list component entry formats for schematic and netlist\n"
  , argv[0]);
      return 0;
    }
    else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
#ifdef GIT
      fprintf(stdout, "qucs s" PACKAGE_VERSION " (" GIT ")" "\n");
#else
      fprintf(stdout, "Qucs " PACKAGE_VERSION "\n");
#endif
      return 0;
    }
    else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--netlist")) {
      netlist_flag = true;
    }
    else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--print")) {
      print_flag = true;
    }
    else if (!strcmp(argv[i], "--page")) {
      page = argv[++i];
    }
    else if (!strcmp(argv[i], "--dpi")) {
      dpi = QString(argv[++i]).toInt();
    }
    else if (!strcmp(argv[i], "--color")) {
      color = argv[++i];
    }
    else if (!strcmp(argv[i], "--orin")) {
      orientation = argv[++i];
    }
    else if (!strcmp(argv[i], "-i")) {
      inputfile = argv[++i];
    }
    else if (!strcmp(argv[i], "-o")) {
      outputfile = argv[++i];
    }
    else if (!strcmp(argv[i], "--ngspice")) {
      ngspice_flag = true;
    }
    else if (!strcmp(argv[i], "--xyce")) {
      xyce_flag = true;
    }
    else if (!strcmp(argv[i], "--run")) {
      run_flag = true;
    }
    else if(!strcmp(argv[i], "-icons")) {
      createIcons();
      return 0;
    }
    else if(!strcmp(argv[i], "-doc")) {
      createDocData();
      return 0;
    }
    else if(!strcmp(argv[i], "-list-entries")) {
      createListComponentEntry();
      return 0;
    }
    else {
      fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
      return -1;
    }
  }

  // check operation and its required arguments
  if (netlist_flag and print_flag) {
    fprintf(stderr, "Error: --print and --netlist cannot be used together\n");
    return -1;
  } else if (((ngspice_flag||xyce_flag) && print_flag)||
             (run_flag && print_flag))
  {
    fprintf(stderr, "Error: --print and Ngspice/Xyce cannot be used together\n");
    return -1;
  } else if (netlist_flag or print_flag) {
    if (inputfile.isEmpty()) {
      fprintf(stderr, "Error: Expected input file.\n");
      return -1;
    }
    if (outputfile.isEmpty()) {
      fprintf(stderr, "Error: Expected output file.\n");
      return -1;
    }
    // create netlist from schematic
    if (netlist_flag) {
        if (!run_flag) {
            if (ngspice_flag) return doNgspiceNetlist(inputfile, outputfile);
            else if (xyce_flag) return doXyceNetlist(inputfile, outputfile);
            else return doNetlist(inputfile, outputfile);
        } else {
            if (ngspice_flag) return runNgspice(inputfile, outputfile);
            else if (xyce_flag) return runXyce(inputfile, outputfile);
            else return 1;
        }
    } else if (print_flag) {
      return doPrint(inputfile, outputfile,
          page, dpi, color, orientation);
    }
  }

  QucsMain = new QucsApp();
  //1a.setMainWidget(QucsMain);

  QucsMain->show();
  int result = a.exec();
  //saveApplSettings(QucsMain);
  return result;
}
