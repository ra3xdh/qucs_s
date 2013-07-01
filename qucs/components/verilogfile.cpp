/***************************************************************************
                              verilogfile.cpp
                              ---------------
    begin                : Sat Mar 31 2007
    copyright            : (C) 2007 by Stefan Jahn
    email                : stefa@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QtGui>
#include "verilogfile.h"
#include "qucs.h"
//Added by qt3to4:
#include <Q3TextStream>
#include "main.h"
#include "schematic.h"

#include <qregexp.h>
#include <qdir.h>
#include <qfileinfo.h>


Verilog_File::Verilog_File()
{
  Type = isDigitalComponent;
  Description = QObject::tr("Verilog file");

  Props.append(new Property("File", "sub.v", false,
		QObject::tr("Name of Verilog file")));

  Model = "Verilog";
  Name  = "X";

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0));
}

// -------------------------------------------------------
Component* Verilog_File::newOne()
{
  Verilog_File *p = new Verilog_File();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* Verilog_File::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Verilog file");
  BitmapFile = (char *) "vhdlfile";

  if(getNewOne) {
    Verilog_File *p = new Verilog_File();
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString Verilog_File::verilogCode(int)
{
  QString s;
  Port *pp = Ports.first();
  if(pp) {
    s = "  " + ModuleName + " " + Name + " (";

    // output all node names
    if(pp)  s += pp->Connection->Name;
    for(pp = Ports.next(); pp != 0; pp = Ports.next())
      s += ", "+pp->Connection->Name;   // node names

    s += ");\n";
  }
  return s;
}

// -------------------------------------------------------
// Returns a comma separated list of the port names of the last
// entity in this file.
QString Verilog_File::loadFile()
{
  QString s, File(Props.getFirst()->Value);
  QFileInfo Info(File);
  if(Info.isRelative())
    File = QucsSettings.QucsWorkDir.filePath(File);

  QFile f(File);
  if(!f.open(QIODevice::ReadOnly))
    return QString("");

  Q3TextStream stream(&f);
  File = stream.read();   // QString is better for "find" function
  f.close();

  // parse ports, i.e. network connections
  Verilog_File_Info VInfo(File);
  ModuleName = VInfo.ModuleName;
  return VInfo.PortNames;
}

// -------------------------------------------------------
void Verilog_File::createSymbol()
{
  QFontMetrics  metrics(QucsSettings.font);   // get size of text
  int fHeight = metrics.lineSpacing();

  int No = 0;
  QString tmp, PortNames = loadFile();
  if(!PortNames.isEmpty())
    No = PortNames.count(',') + 1;


  #define HALFWIDTH  24
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  tmp = QObject::tr("verilog");
  int w = metrics.width(tmp);
  Texts.append(new Text(w/-2, fHeight/-2, tmp));


  int y = 15-h, i = 0;
  while(i<No) {
    Lines.append(new Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    tmp = PortNames.section(',', i, i);
    w = metrics.width(tmp);
    Texts.append(new Text(-26-w, y-fHeight-2, tmp));
    i++;

    if(i == No) break;
    Lines.append(new Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    tmp = PortNames.section(',', i, i);
    Texts.append(new Text( 27, y-fHeight-2, tmp));
    y += 60;
    i++;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  tx = x1+4;
  ty = y2+4;
}

// -------------------------------------------------------
QString Verilog_File::getSubcircuitFile()
{
  // construct full filename
  QString FileName = Props.getFirst()->Value;
  return properAbsFileName(FileName);
}

// -------------------------------------------------------
bool Verilog_File::createSubNetlist(QTextStream *stream)
{
  ErrText = "";

  // check filename
  QString FileName = Props.getFirst()->Value;
  if(FileName.isEmpty()) {
    ErrText += QObject::tr("ERROR: No file name in %1 component \"%2\".").
      arg(Model).arg(Name);
    return false;
  }

  // construct full filename
  FileName = getSubcircuitFile();

  // open file for reading
  QFile f(FileName);
  if(!f.open(QIODevice::ReadOnly)) {
    ErrText += QObject::tr("ERROR: Cannot open %1 file \"%2\".").
      arg(Model).arg(FileName);
    return false;
  }

  // write the whole Verilog file into the netlist output
  QByteArray FileContent = f.readAll();
  f.close();
  (*stream) << '\n';
  //?stream->writeRawBytes(FileContent.data(), FileContent.size());
  (*stream) << FileContent.data();
  (*stream) << '\n';
  return true;
}

// -------------------------------------------------------
Verilog_File_Info::Verilog_File_Info()
{
  ModuleName = "";
  PortNames = "";
}

// -------------------------------------------------------
Verilog_File_Info::Verilog_File_Info(QString File, bool isfile)
{
  if (isfile) {
    QFile f(File);
    if(!f.open(QIODevice::ReadOnly))
      File = "";
    else {
      QByteArray FileContent = f.readAll();
      File = QString(FileContent);
    }
    f.close();
  }
  
  QString s;
  int i=0, j, k=0;
  while((i=File.find("//", i)) >= 0) { // remove all Verilog comments
    j = File.find('\n', i+2);          // (This also finds "//" within a ...
    if(j < 0)                          //  string, but as no strings are ...
      File = File.left(i);             //  allowed in module headers, it ...
    else                               //  does not matter.)
      File.remove(i, j-i);
  }

  i=0;
  while((i=File.find("/*", i)) >= 0) { // remove all Verilog comments
    j = File.find("*/", i+2);          // (This also finds "/*" within a ...
    if(j < 0)                          //  string, but as no strings are ...
      File = File.left(i);             //  allowed in module headers, it ...
    else                               //  does not matter.)
      File.remove(i, j-i+2);
  }

  QRegExp Expr,Expr1;
  Expr.setCaseSensitive(true);
  Expr1.setCaseSensitive(true);
  k--;
  Expr.setPattern("\\bmodule\\b");  // start of last module
  k = File.findRev(Expr, k);
  if(k < 0)
    return;

  Expr.setPattern("\\bendmodule\\b");    // end of last module
  i = File.find(Expr, k+7);
  if(i < 0)
    return;
  s = File.mid(k+7, i-k-7);  // cut out module declaration

  Expr.setPattern("\\b");
  i = s.find(Expr);
  if(i < 0)
    return;
  j = s.find(Expr, i+1);
  if(j < 0)
    return;
  ModuleName = s.mid(i, j-i);  // save module name

  i = s.find('(', j);
  if(i < 0)
    return;

  j = s.find(')', i);
  if(j < 0)
    return;
  s = s.mid(i+1, j-i-1);

  // parse ports, i.e. network connections; and generics, i.e. parameters
  PortNames = parsePorts (s, 0);
}

// -------------------------------------------------------
QString Verilog_File_Info::parsePorts(QString s, int i)
{
  QRegExp Expr,Expr1;
  Expr.setCaseSensitive(true);
  Expr1.setCaseSensitive(true);

  int j;
  i = 0;    // remove all Verilog identifiers (e.g. "input")
  Expr.setPattern("(\\binput\\b|\\boutput\\b|\\binout\\b)");
  Expr1.setPattern("(\\b)");
  while((i=s.find(Expr, i)) >= 0) {
    j = s.find(Expr1, i+1);
    if(j < 0)
      s = s.left(i);
    else
      s.remove(i, j-i);
  }

  s.remove(' ');
  s.remove('\n');
  s.remove('\t');
  return s;
}
