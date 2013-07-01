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
#include <QtGui>
#include <qmessagebox.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <q3process.h>
#include <q3textedit.h>
#include <QTextEdit>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3ValueList>

#include "main.h"
#include "node.h"
#include "schematic.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/spicefile.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/libcomp.h"


// Here the subcircuits, SPICE components etc are collected. It must be
// global to also work within the subcircuits.
SubMap FileList;


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
  for(pc = Components->first(); pc != 0; pc = Components->next())
    if(pc->isSelected) {
      s += pc->save()+"\n";  z++; }
  s += "</Components>\n";

  s += "<Wires>\n";
  for(pw = Wires->first(); pw != 0; pw = Wires->next())
    if(pw->isSelected) {
      z++;
      if(pw->Label) if(!pw->Label->isSelected) {
	s += pw->save().section('"', 0, 0)+"\"\" 0 0 0>\n";
	continue;
      }
      s += pw->save()+"\n";
    }
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label) if(pn->Label->isSelected) {
      s += pn->Label->save()+"\n";  z++; }
  s += "</Wires>\n";

  s += "<Diagrams>\n";
  for(pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    if(pd->isSelected) {
      s += pd->save()+"\n";  z++; }
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(pp = Paintings->first(); pp != 0; pp = Paintings->next())
    if(pp->isSelected)
      if(pp->Name.at(0) != '.') {  // subcircuit specific -> do not copy
        s += "<"+pp->save()+">\n";  z++; }
  s += "</Paintings>\n";

  if(z == 0) return "";   // return empty if no selection

  return s;
}

// -------------------------------------------------------------
// Only read fields without loading them.
bool Schematic::loadIntoNothing(Q3TextStream *stream)
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
bool Schematic::pasteFromClipboard(Q3TextStream *stream, Q3PtrList<Element> *pe)
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
  if(symbolMode) {
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
        if(!loadPaintings(stream, (Q3PtrList<Painting>*)pe)) return false; }
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
      if(!loadComponents(stream, (Q3PtrList<Component>*)pe)) return false; }
    else
    if(Line == "<Wires>") {
      if(!loadWires(stream, pe)) return false; }
    else
    if(Line == "<Diagrams>") {
      if(!loadDiagrams(stream, (Q3PtrList<Diagram>*)pe)) return false; }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(stream, (Q3PtrList<Painting>*)pe)) return false; }
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
  QFileInfo info (DocName);
  QString cppfile = info.dirPath () + QDir::separator() + DataSet;
  QFile file (cppfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
		   QObject::tr("Cannot save C++ file \"%1\"!").arg(cppfile));
    return -1;
  }

  Q3TextStream stream (&file);

  // automatically compute boundings of drawing
  int xmin = INT_MAX;
  int ymin = INT_MAX;
  int xmax = INT_MIN;
  int ymax = INT_MIN;
  int x1, y1, x2, y2;
  int maxNum = 0;
  Painting * pp;

  stream << "  // symbol drawing code\n";
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
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
    for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
      if (pp->Name == ".PortSym ")
	if (((PortSymbol*)pp)->numberStr.toInt() == i)
	  stream << "  " << pp->saveCpp () << "\n";
    }
  }

  stream << "\n  // symbol boundings\n"
	 << "  x1 = " << xmin << "; " << "  y1 = " << ymin << ";\n"
	 << "  x2 = " << xmax << "; " << "  y2 = " << ymax << ";\n";

  stream << "\n  // property text position\n";
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ())
    if (pp->Name == ".ID ")
      stream << "  " << pp->saveCpp () << "\n";

  file.close ();
  return 0;
}

// -------------------------------------------------------------
// Returns the number of subcircuit ports.
int Schematic::saveDocument()
{
  QFile file(DocName);
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
				QObject::tr("Cannot save document!"));
    return -1;
  }

  Q3TextStream stream(&file);

  stream << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  stream << "<Properties>\n";
  if(symbolMode) {
    stream << "  <View=" << tmpViewX1<<","<<tmpViewY1<<","
			 << tmpViewX2<<","<<tmpViewY2<< ",";
    stream <<tmpScale<<","<<tmpPosX<<","<<tmpPosY << ">\n";
  }
  else {
    stream << "  <View=" << ViewX1<<","<<ViewY1<<","
			 << ViewX2<<","<<ViewY2<< ",";
    stream << Scale <<","<<contentsX()<<","<<contentsY() << ">\n";
  }
  stream << "  <Grid=" << GridX<<","<<GridY<<","
			<< GridOn << ">\n";
  stream << "  <DataSet=" << DataSet << ">\n";
  stream << "  <DataDisplay=" << DataDisplay << ">\n";
  stream << "  <OpenDisplay=" << SimOpenDpl << ">\n";
  stream << "  <Script=" << Script << ">\n";
  stream << "  <RunScript=" << SimRunScript << ">\n";
  stream << "  <showFrame=" << showFrame << ">\n";

  QString t;
  convert2ASCII(t = Frame_Text0);
  stream << "  <FrameText0=" << t << ">\n";
  convert2ASCII(t = Frame_Text1);
  stream << "  <FrameText1=" << t << ">\n";
  convert2ASCII(t = Frame_Text2);
  stream << "  <FrameText2=" << t << ">\n";
  convert2ASCII(t = Frame_Text3);
  stream << "  <FrameText3=" << t << ">\n";
  stream << "</Properties>\n";

  Painting *pp;
  stream << "<Symbol>\n";     // save all paintings for symbol
  for(pp = SymbolPaints.first(); pp != 0; pp = SymbolPaints.next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Symbol>\n";

  stream << "<Components>\n";    // save all components
  for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next())
    stream << "  " << pc->save() << "\n";
  stream << "</Components>\n";

  stream << "<Wires>\n";    // save all wires
  for(Wire *pw = DocWires.first(); pw != 0; pw = DocWires.next())
    stream << "  " << pw->save() << "\n";

  // save all labeled nodes as wires
  for(Node *pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->Label) stream << "  " << pn->Label->save() << "\n";
  stream << "</Wires>\n";

  stream << "<Diagrams>\n";    // save all diagrams
  for(Diagram *pd = DocDiags.first(); pd != 0; pd = DocDiags.next())
    stream << "  " << pd->save() << "\n";
  stream << "</Diagrams>\n";

  stream << "<Paintings>\n";     // save all paintings
  for(pp = DocPaints.first(); pp != 0; pp = DocPaints.next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Paintings>\n";

  file.close();

  // additionally save symbol C++ code if in a symbol drawing and the
  // associated file is a Verilog-A file
  if (fileSuffix () == "sym") {
    if (fileSuffix (DataDisplay) == "va") {
      saveSymbolCpp ();
    }
  }

  return 0;
}

// -------------------------------------------------------------
bool Schematic::loadProperties(Q3TextStream *stream)
{
  bool ok = true;
  QString Line, cstr, nstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;  // field end ?
    Line = Line.stripWhiteSpace();
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
		ViewX1 = nstr.section(',',0,0).toInt(&ok); if(ok) {
		ViewY1 = nstr.section(',',1,1).toInt(&ok); if(ok) {
		ViewX2 = nstr.section(',',2,2).toInt(&ok); if(ok) {
		ViewY2 = nstr.section(',',3,3).toInt(&ok); if(ok) {
		Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) {
		tmpViewX1 = nstr.section(',',5,5).toInt(&ok); if(ok)
		tmpViewY1 = nstr.section(',',6,6).toInt(&ok); }}}}} }
    else if(cstr == "Grid") {
		GridX = nstr.section(',',0,0).toInt(&ok); if(ok) {
		GridY = nstr.section(',',1,1).toInt(&ok); if(ok) {
		if(nstr.section(',',2,2).toInt(&ok) == 0) GridOn = false;
		else GridOn = true; }} }
    else if(cstr == "DataSet") DataSet = nstr;
    else if(cstr == "DataDisplay") DataDisplay = nstr;
    else if(cstr == "OpenDisplay")
		if(nstr.toInt(&ok) == 0) SimOpenDpl = false;
		else SimOpenDpl = true;
    else if(cstr == "Script") Script = nstr;
    else if(cstr == "RunScript")
		if(nstr.toInt(&ok) == 0) SimRunScript = false;
		else SimRunScript = true;
    else if(cstr == "showFrame")
		showFrame = nstr.at(0).latin1() - '0';
    else if(cstr == "FrameText0") convert2Unicode(Frame_Text0 = nstr);
    else if(cstr == "FrameText1") convert2Unicode(Frame_Text1 = nstr);
    else if(cstr == "FrameText2") convert2Unicode(Frame_Text2 = nstr);
    else if(cstr == "FrameText3") convert2Unicode(Frame_Text3 = nstr);
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
  for(Port *pp = c->Ports.first(); pp != 0; pp = c->Ports.next()) {
    x = pp->x+c->cx;
    y = pp->y+c->cy;

    // check if new node lies upon existing node
    for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
      if(pn->cx == x) if(pn->cy == y) {
	if (!pn->DType.isEmpty()) {
	  pp->Type = pn->DType;
	}
	if (!pp->Type.isEmpty()) {
	  pn->DType = pp->Type;
	}
	break;
      }

    if(pn == 0) { // create new node, if no existing one lies at this position
      pn = new Node(x, y);
      DocNodes.append(pn);
    }
    pn->Connections.append(c);  // connect schematic node to component node
    if (!pp->Type.isEmpty()) {
      pn->DType = pp->Type;
    }

    pp->Connection = pn;  // connect component node to schematic node
  }

  DocComps.append(c);
}

// -------------------------------------------------------------
bool Schematic::loadComponents(Q3TextStream *stream, Q3PtrList<Component> *List)
{
  QString Line, cstr;
  Component *c;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    c = getComponentFromName(Line);
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
  for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->cx == pw->x1) if(pn->cy == pw->y1) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x1, pw->y1);
    DocNodes.append(pn);
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
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port1 = pn;

  // check if second wire node lies upon existing node
  for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->cx == pw->x2) if(pn->cy == pw->y2) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x2, pw->y2);
    DocNodes.append(pn);
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port2 = pn;

  DocWires.append(pw);
}

// -------------------------------------------------------------
bool Schematic::loadWires(Q3TextStream *stream, Q3PtrList<Element> *List)
{
  Wire *w;
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.stripWhiteSpace();
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
bool Schematic::loadDiagrams(Q3TextStream *stream, Q3PtrList<Diagram> *List)
{
  Diagram *d;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.stripWhiteSpace();
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
bool Schematic::loadPaintings(Q3TextStream *stream, Q3PtrList<Painting> *List)
{
  Painting *p=0;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;

    Line = Line.stripWhiteSpace();
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
    else if(cstr == "Rectangle") p = new Rectangle();
    else if(cstr == "Arrow") p = new Arrow();
    else if(cstr == "Ellipse") p = new Ellipse();
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

// -------------------------------------------------------------
bool Schematic::loadDocument()
{
  QFile file(DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load document: ")+DocName);
    return false;
  }

  QString Line;
  Q3TextStream stream(&file);

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
 		 QObject::tr("Wrong document type: ")+DocName);
    return false;
  }

  Line = Line.mid(16, Line.length()-17);
  if(!checkVersion(Line)) { // wrong version number ?
    file.close();
    QMessageBox::critical(0, QObject::tr("Error"),
		 QObject::tr("Wrong document version: ")+Line);
    return false;
  }

  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    if(Line == "<Symbol>") {
      if(!loadPaintings(&stream, &SymbolPaints)) {
	file.close();
	return false;
      }
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
      if(!loadDiagrams(&stream, &DocDiags)) { file.close(); return false; } }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(&stream, &DocPaints)) { file.close(); return false; } }
    else {
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
  for(pc = DocComps.first(); pc != 0; pc = DocComps.next())
    s += pc->save()+"\n";
  s += "</>\n";  // short end flag

  for(pw = DocWires.first(); pw != 0; pw = DocWires.next())
    s += pw->save()+"\n";
  // save all labeled nodes as wires
  for(Node *pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->Label) s += pn->Label->save()+"\n";
  s += "</>\n";

  for(pd = DocDiags.first(); pd != 0; pd = DocDiags.next())
    s += pd->save()+"\n";
  s += "</>\n";

  for(pp = DocPaints.first(); pp != 0; pp = DocPaints.next())
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

  for(pp = SymbolPaints.first(); pp != 0; pp = SymbolPaints.next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
}

// -------------------------------------------------------------
// Is quite similiar to "loadDocument()" but with less error checking.
// Used for "undo" function.
bool Schematic::rebuild(QString *s)
{
  DocWires.clear();	// delete whole document
  DocNodes.clear();
  DocComps.clear();
  DocDiags.clear();
  DocPaints.clear();

  QString Line;
  Q3TextStream stream(s, QIODevice::ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  if(!loadComponents(&stream))  return false;
  if(!loadWires(&stream))  return false;
  if(!loadDiagrams(&stream, &DocDiags))  return false;
  if(!loadPaintings(&stream, &DocPaints)) return false;

  return true;
}

// -------------------------------------------------------------
// Same as "rebuild(QString *s)" but for symbol edit mode.
bool Schematic::rebuildSymbol(QString *s)
{
  SymbolPaints.clear();	// delete whole document

  QString Line;
  Q3TextStream stream(s, QIODevice::ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  Line = stream.readLine();  // skip components
  Line = stream.readLine();  // skip wires
  Line = stream.readLine();  // skip diagrams
  if(!loadPaintings(&stream, &SymbolPaints)) return false;

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

  for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next()) {
    if(pn->Name.isEmpty() == User) {
      continue;  // already named ?
    }
    if(!User) {
      if(isAnalog)
	pn->Name = "_net";
      else
	pn->Name = "net_net";   // VHDL names must not begin with '_'
      pn->Name += QString::number(z++);  // create numbered node name
    }
    else if(pn->State) {
      continue;  // already worked on
    }

    if(isAnalog) createNodeSet(Collect, countInit, pn, pn);

    pn->State = 1;
    propagateNode(Collect, countInit, pn);
  }
}

// ---------------------------------------------------
// Collects the signal names for digital simulations.
void Schematic::collectDigitalSignals(void)
{
  Node *pn;

  for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next()) {
    DigMap::Iterator it = Signals.find(pn->Name);
    if(it == Signals.end()) { // avoid redeclaration of signal
      Signals.insert(pn->Name, DigSignal(pn->Name, pn->DType));
    } else if (!pn->DType.isEmpty()) {
      it.data().Type = pn->DType;
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
  Element *pe;

  Cons.append(pn);
  for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
    for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
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
	  if (isAnalog) createNodeSet(Collect, countInit, pw, pn);
	  setName = false;
	}
      }
  Cons.clear();
}

#include <iostream>
// --------------------------------------------------- 
// Goes through all schematic components and allows special component
// handling, e.g. like subcircuit netlisting.
bool Schematic::throughAllComps(QTextStream *stream, int& countInit,
                   QStringList& Collect, QTextEdit *ErrText, int NumPorts)
{
  bool r;
  QString s;

  // give the ground nodes the name "gnd", and insert subcircuits etc.
  Q3PtrListIterator<Component> it(DocComps);
  Component *pc;
  while((pc = it.current()) != 0) {
    ++it;
    if(pc->isActive != COMP_IS_ACTIVE) continue;

    // check analog/digital typed components
    if(isAnalog) {
      if((pc->Type & isAnalogComponent) == 0) {
        ErrText->insert(QObject::tr("ERROR: Component \"%1\" has no analog model.").arg(pc->Name));
        return false;
      }
    } else {
      if((pc->Type & isDigitalComponent) == 0) {
        ErrText->insert(QObject::tr("ERROR: Component \"%1\" has no digital model.").arg(pc->Name));
        return false;
      }
    }

    // handle ground symbol
    if(pc->Model == "GND") {
      pc->Ports.getFirst()->Connection->Name = "gnd";
      continue;
    }

    // handle subcircuits
    if(pc->Model == "Sub") {
      int i;
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end()) {
	if (!it.data().PortTypes.isEmpty()) {
	  i = 0;
	  // apply in/out signal types of subcircuit
	  for(Port *pp = pc->Ports.first(); pp; pp = pc->Ports.next(), i++) {
	    pp->Type = it.data().PortTypes[i];
	    pp->Connection->DType = pp->Type;
	  }
	}
        continue;   // insert each subcircuit just one time
      }
      SubFile sub = SubFile("SCH", f);
      FileList.insert(f, sub);

      // load subcircuit schematic
      s = pc->Props.first()->Value;
      Schematic *d = new Schematic(0, QucsSettings.QucsWorkDir.filePath(s));
      if(!d->loadDocument()) {  // load document if possible
        delete d;
        ErrText->insert(QObject::tr("ERROR: Cannot load subcircuit \"%1\".").arg(s));
        return false;
      }
      d->DocName = s;
      d->isVerilog = isVerilog;
      d->isAnalog = isAnalog;
      d->creatingLib = creatingLib;
      r = d->createSubNetlist(stream, countInit, Collect, ErrText, NumPorts);
      if (r) {
	      i = 0;
	      // save in/out signal types of subcircuit
	      for(Port *pp = pc->Ports.first(); pp; pp = pc->Ports.next(), i++) {
          //if(i>=d->PortTypes.count())break;
	        pp->Type = d->PortTypes[i];
	        pp->Connection->DType = pp->Type;
	      }
	      sub.PortTypes = d->PortTypes;
	      FileList.replace(f, sub);
      }
      delete d;
      if(!r) return false;
      continue;
    }

    // handle library symbols
    if(pc->Model == "Lib") {
      if(creatingLib) {
	ErrText->insert(
	    QObject::tr("WARNING: Skipping library component \"%1\".").
	    arg(pc->Name));
	continue;
      }
      s = pc->getSubcircuitFile() + "/" + pc->Props.at(1)->Value;
      SubMap::Iterator it = FileList.find(s);
      if(it != FileList.end())
        continue;   // insert each library subcircuit just one time
      FileList.insert(s, SubFile("LIB", s));

      if(isAnalog)
	r = ((LibComp*)pc)->createSubNetlist(stream, Collect, 1);
      else {
	if(isVerilog)
	  r = ((LibComp*)pc)->createSubNetlist(stream, Collect, 4);
	else
	  r = ((LibComp*)pc)->createSubNetlist(stream, Collect, 2);
      }
      if(!r) {
	ErrText->insert(
	    QObject::tr("ERROR: Cannot load library component \"%1\".").
	    arg(pc->Name));
	return false;
      }
      continue;
    }

    // handle SPICE subcircuit components
    if(pc->Model == "SPICE") {
      s = pc->Props.first()->Value;
      if(s.isEmpty()) {
        ErrText->insert(QObject::tr("ERROR: No file name in SPICE component \"%1\".").
                        arg(pc->Name));
        return false;
      }
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end())
        continue;   // insert each spice component just one time
      FileList.insert(f, SubFile("CIR", f));

      SpiceFile *sf = (SpiceFile*)pc;
      r = sf->createSubNetlist(stream);
      ErrText->insert(sf->getErrorText());
      if(!r) return false;
      continue;
    }

    // handle digital file subcircuits
    if(pc->Model == "VHDL" || pc->Model == "Verilog") {
      if(isVerilog && pc->Model == "VHDL")
	continue;
      if(!isVerilog && pc->Model == "Verilog")
	continue;
      s = pc->Props.getFirst()->Value;
      if(s.isEmpty()) {
        ErrText->insert(QObject::tr("ERROR: No file name in %1 component \"%2\".").
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
	ErrText->insert(vf->getErrorText());
	if(!r) return false;
      }
      if(pc->Model == "Verilog") {
	Verilog_File *vf = (Verilog_File*)pc;
	r = vf->createSubNetlist(stream);
	ErrText->insert(vf->getErrorText());
	if(!r) return false;
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
                   QStringList& Collect, QTextEdit *ErrText, int NumPorts)
{
  // delete the node names
  for(Node *pn = DocNodes.first(); pn != 0; pn = DocNodes.next()) {
    pn->State = 0;
    if(pn->Label) {
      if(isAnalog)
        pn->Name = pn->Label->Name;
      else
        pn->Name = "net" + pn->Label->Name;
    }
    else pn->Name = "";
  }

  // set the wire names to the connected node
  for(Wire *pw = DocWires.first(); pw != 0; pw = DocWires.next())
    if(pw->Label != 0) {
      if(isAnalog)
        pw->Port1->Name = pw->Label->Name;
      else  // avoid to use reserved VHDL words
        pw->Port1->Name = "net" + pw->Label->Name;
    }

  // go through components
  if(!throughAllComps(stream, countInit, Collect, ErrText, NumPorts))
    return false;

  // work on named nodes first in order to preserve the user given names
  throughAllNodes(true, Collect, countInit);

  // give names to the remaining (unnamed) nodes
  throughAllNodes(false, Collect, countInit);

  if(!isAnalog) // collect all node names for VHDL signal declaration
    collectDigitalSignals();

  return true;
}

// ---------------------------------------------------
bool Schematic::createLibNetlist(QTextStream *stream, QTextEdit *ErrText,
				 int NumPorts)
{
  int countInit = 0;
  QStringList Collect;
  Collect.clear();
  FileList.clear();
  Signals.clear();
  // Apply node names and collect subcircuits and file include
  creatingLib = true;
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts)) {
    creatingLib = false;
    return false;
  }
  creatingLib = false;

  // Marking start of actual top-level subcircuit
  QString c;
  if(!isAnalog) {
    if (isVerilog)
      c = "///";
    else
      c = "---";
  }
  else c = "###";
  (*stream) << "\n" << c << " TOP LEVEL MARK " << c << "\n";

  // Emit subcircuit components
  createSubNetlistPlain(stream, ErrText, NumPorts);

  Signals.clear();  // was filled in "giveNodeNames()"
  return true;
}

//#define VHDL_SIGNAL_TYPE "bit"
//#define VHDL_LIBRARIES   ""
#define VHDL_SIGNAL_TYPE "std_logic"
#define VHDL_LIBRARIES   "\nlibrary ieee;\nuse ieee.std_logic_1164.all;\n"

// ---------------------------------------------------
void Schematic::createSubNetlistPlain(QTextStream *stream, QTextEdit *ErrText,
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
  if(creatingLib) {
    QString f = properAbsFileName(DocName) + ".lst";
    ofile.setName(f);
    if(!ofile.open(IO_WriteOnly)) {
      ErrText->insert(tr("ERROR: Cannot create library file \"%s\".").arg(f));
      return;
    }
    tstream = new QTextStream(&ofile);
  }

  // collect subcircuit ports and sort their node names into
  // "SubcircuitPortNames"
  PortTypes.clear();
  for(pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
    if(pc->Model.at(0) == '.') { // no simulations in subcircuits
      ErrText->insert(
        QObject::tr("WARNING: Ignore simulation component in subcircuit \"%1\".").arg(DocName)+"\n");
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
      (*it_name) = pc->Ports.getFirst()->Connection->Name;
      DigMap::Iterator it = Signals.find(*it_name);
      if(it!=Signals.end())
        (*it_type) = it.data().Type;
      // propagate type to port symbol
      pc->Ports.getFirst()->Connection->DType = *it_type;

      if(!isAnalog) {
        if (isVerilog) {
          Signals.erase(*it_name); // remove node name
          switch(pc->Props.at(1)->Value.at(0).latin1()) {
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
          Signals.erase(*it_name);
          switch(pc->Props.at(1)->Value.at(0).latin1()) {
            case 'a':
              (*it_name) += " : inout"; // attribute "analog" is "inout"
              break;
            case 'o': // output ports need workaround
              Signals.insert(*it_name, DigSignal(*it_name, *it_type));
              (*it_name) = "net_out" + (*it_name);
              // no "break;" here !!!
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
      it_name = SubcircuitPortNames.remove(it_name);
      it_type = SubcircuitPortTypes.remove(it_type);
    } else {
      PortTypes.append(*it_type);
      it_name++;
      it_type++;
    }
  }

  QString f = properFileName(DocName);
  QString Type = properName(f);

  Painting *pi;
  if(isAnalog) {
    // ..... analog subcircuit ...................................
    (*tstream) << "\n.Def:" << Type << " " << SubcircuitPortNames.join(" ");
    for(pi = SymbolPaints.first(); pi != 0; pi = SymbolPaints.next())
      if(pi->Name == ".ID ") {
        SubParameter *pp;
        ID_Text *pid = (ID_Text*)pi;
        for(pp = pid->Parameter.first(); pp != 0; pp = pid->Parameter.next()) {
          s = pp->Name; // keep 'Name' unchanged
          (*tstream) << " " << s.replace("=", "=\"") << '"';
        }
        break;
      }
    (*tstream) << '\n';

    // write all components with node names into netlist file
    for(pc = DocComps.first(); pc != 0; pc = DocComps.next())
      (*tstream) << pc->getNetlist();

    (*tstream) << ".Def:End\n";

  }
  else {
    if (isVerilog) {
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
      if(!Signals.isEmpty()) {
        Q3ValueList<DigSignal> values = Signals.values();
        Q3ValueList<DigSignal>::iterator it;
        for (it = values.begin(); it != values.end(); ++it) {
          (*tstream) << " wire " << (*it).Name << ";\n";
        }
      }
      (*tstream) << "\n";

      // subcircuit parameters
      for(pi = SymbolPaints.first(); pi != 0; pi = SymbolPaints.next())
        if(pi->Name == ".ID ") {
          SubParameter *pp;
          ID_Text *pid = (ID_Text*)pi;
          if(pid->Parameter.first()) {
            for(pp = pid->Parameter.first(); pp != 0;) {
              s = pp->Name.section('=', 0,0);
              QString v = Verilog_Param(pp->Name.section('=', 1,1));
              (*tstream) << " parameter " << s << " = " << v << ";\n";
              pp = pid->Parameter.next();
            }
            (*tstream) << "\n";
          }
          break;
        }

      // write all equations into netlist file
      for(pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
        if(pc->Model == "Eqn") {
          (*tstream) << pc->get_Verilog_Code(NumPorts);
        }
      }

      if(Signals.find("gnd") != Signals.end())
      (*tstream) << " assign gnd = 0;\n"; // should appear only once

      // write all components into netlist file
      for(pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
        if(pc->Model != "Eqn") {
          s = pc->get_Verilog_Code(NumPorts);
          if(s.length()>0 && s.at(0) == '\xA7') {  //section symbol
            ErrText->insert(s.mid(1));
          }
          else (*tstream) << s;
        }
      }

      (*tstream) << "endmodule\n";
    } else {
      // ..... digital subcircuit ...................................
      (*tstream) << VHDL_LIBRARIES;
      (*tstream) << "entity Sub_" << Type << " is\n"
                << " port ("
                << SubcircuitPortNames.join(";\n ") << ");\n";

      for(pi = SymbolPaints.first(); pi != 0; pi = SymbolPaints.next())
        if(pi->Name == ".ID ") {
          SubParameter *pp;
          ID_Text *pid = (ID_Text*)pi;
          if(pid->Parameter.first()) {
            (*tstream) << " generic (";
            for(pp = pid->Parameter.first(); pp != 0;) {
              s = pp->Name;
              QString t = pp->Type.isEmpty() ? "real" : pp->Type;
              (*tstream) << s.replace("=", " : "+t+" := ");
              pp = pid->Parameter.next();
              if(pp) (*tstream) << ";\n ";
            }
            (*tstream) << ");\n";
          }
          break;
        }

      (*tstream) << "end entity;\n"
                  << "use work.all;\n"
                  << "architecture Arch_Sub_" << Type << " of Sub_" << Type
                  << " is\n";

      if(!Signals.isEmpty()) {
        Q3ValueList<DigSignal> values = Signals.values();
        Q3ValueList<DigSignal>::iterator it;
        for (it = values.begin(); it != values.end(); ++it) {
          (*tstream) << " signal " << (*it).Name << " : "
          << ((*it).Type.isEmpty() ?
          VHDL_SIGNAL_TYPE : (*it).Type) << ";\n";
        }
      }

      // write all equations into netlist file
      for(pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
        if(pc->Model == "Eqn") {
          ErrText->insert(
                      QObject::tr("WARNING: Equations in \"%1\" are 'time' typed.").
          arg(pc->Name));
          (*tstream) << pc->get_VHDL_Code(NumPorts);
        }
      }

      (*tstream) << "begin\n";

      if(Signals.find("gnd") != Signals.end())
      (*tstream) << " gnd <= '0';\n"; // should appear only once

      // write all components into netlist file
      for(pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
        if(pc->Model != "Eqn") {
            s = pc->get_VHDL_Code(NumPorts);
            if(s.length()>0 && s.at(0) == '\xA7') {  //section symbol
              ErrText->insert(s.mid(1));
          }
          else (*tstream) << s;
        }
      }

      (*tstream) << "end architecture;\n";
    }
  }

  // close file
  if(creatingLib) {
    ofile.close();
    delete tstream;
  }
}
// ---------------------------------------------------
// Write the netlist as subcircuit to the text stream 'stream'.
bool Schematic::createSubNetlist(QTextStream *stream, int& countInit,
                     QStringList& Collect, QTextEdit *ErrText, int NumPorts)
{
//  int Collect_count = Collect.count();   // position for this subcircuit

  // TODO: NodeSets have to be put into the subcircuit block.
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts))
    return false;

/*  Example for TODO
      for(it = Collect.at(Collect_count); it != Collect.end(); )
      if((*it).left(4) == "use ") {  // output all subcircuit uses
        (*stream) << (*it);
        it = Collect.remove(it);
      }
      else it++;*/

  // Emit subcircuit components
  createSubNetlistPlain(stream, ErrText, NumPorts);

  Signals.clear();  // was filled in "giveNodeNames()"
  return true;
}

// ---------------------------------------------------
// Determines the node names and writes subcircuits into netlist file.
int Schematic::prepareNetlist(QTextStream& stream, QStringList& Collect,
                              QTextEdit *ErrText)
{
  if(showBias > 0) showBias = -1;  // do not show DC bias anymore

  isVerilog = false;
  isAnalog = true;
  bool isTruthTable = false;
  int allTypes = 0, NumPorts = 0;

  // Detect simulation domain (analog/digital) by looking at component types.
  for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
    if(pc->isActive == COMP_IS_OPEN) continue;
    if(pc->Model.at(0) == '.') {
      if(pc->Model == ".Digi") {
        if(allTypes & isDigitalComponent) {
          ErrText->insert(
             QObject::tr("ERROR: Only one digital simulation allowed."));
          return -10;
        }
        if(pc->Props.getFirst()->Value != "TimeList")
          isTruthTable = true;
	      if(pc->Props.getLast()->Value != "VHDL")
	        isVerilog = true;
        allTypes |= isDigitalComponent;
	      isAnalog = false;
      }
      else allTypes |= isAnalogComponent;
      if((allTypes & isComponent) == isComponent) {
        ErrText->insert(
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
      isAnalog = true;
      allTypes |= isAnalogComponent;
      NumPorts = -1;
    }
    else {
      if(NumPorts < 1 && isTruthTable) {
        ErrText->insert(
           QObject::tr("ERROR: Digital simulation needs at least one digital source."));
        return -10;
      }
      if(!isTruthTable) NumPorts = 0;
    }
  }
  else {
    NumPorts = -1;
    isAnalog = true;
  }

  // first line is documentation
  if(allTypes & isAnalogComponent)
    stream << "#";
  else if (isVerilog)
    stream << "//";
  else
    stream << "--";
  stream << " Qucs " << PACKAGE_VERSION << "  " << DocName << "\n";

  // set timescale property for verilog schematics
  if (isVerilog) {
    stream << "\n`timescale 1ps/100fs\n";
  }

  int countInit = 0;  // counts the nodesets to give them unique names
  if(!giveNodeNames(&stream, countInit, Collect, ErrText, NumPorts))
    return -10;

  if(allTypes & isAnalogComponent)
    return NumPorts;

  if (!isVerilog) {
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
  if (isVerilog) {
    stream << "module TestBench ();\n";
    Q3ValueList<DigSignal> values = Signals.values();
    Q3ValueList<DigSignal>::iterator it;
    for (it = values.begin(); it != values.end(); ++it) {
      stream << "  wire " << (*it).Name << ";\n";
    }
    stream << "\n";
  } else {
    stream << "architecture Arch_TestBench of TestBench is\n";
    Q3ValueList<DigSignal> values = Signals.values();
    Q3ValueList<DigSignal>::iterator it;
    for (it = values.begin(); it != values.end(); ++it) {
      stream << "  signal " << (*it).Name << " : "
	     << ((*it).Type.isEmpty() ?
		 VHDL_SIGNAL_TYPE : (*it).Type) << ";\n";
    }
    stream << "begin\n";
  }

  if(Signals.find("gnd") != Signals.end()) {
    if (isVerilog) {
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
  if (isVerilog) {
  } else {
    stream << "end architecture;\n";
  }
}

// ---------------------------------------------------
// write all components with node names into the netlist file
QString Schematic::createNetlist(QTextStream& stream, int NumPorts)
{
  if(!isAnalog) {
    beginNetlistDigital(stream);
  }

  Signals.clear();  // was filled in "giveNodeNames()"
  FileList.clear();

  QString s, Time;
  for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
    if(isAnalog) {
      s = pc->getNetlist();
    }
    else {
      if(pc->Model == ".Digi" && pc->isActive) {  // simulation component ?
        if(NumPorts > 0) { // truth table simulation ?
	  if (isVerilog)
	    Time = QString::number((1 << NumPorts));
	  else
	    Time = QString::number((1 << NumPorts) - 1) + " ns";
        } else {
          Time = pc->Props.at(1)->Value;
	  if (isVerilog) {
	    if(!Verilog_Time(Time, pc->Name)) return Time;
	  } else {
	    if(!VHDL_Time(Time, pc->Name)) return Time;  // wrong time format
	  }
        }
      }
      if (isVerilog) {
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

  if(!isAnalog) {
    endNetlistDigital(stream);
  }

  return Time;
}
