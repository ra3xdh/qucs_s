/***************************************************************************
                               vhdlfile.cpp
                              --------------
    begin                : Sat Apr 15 2006
    copyright            : (C) 2006 by Michael Margraf
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
#include <QtGui>
#include "vhdlfile.h"
#include "qucs.h"
//Added by qt3to4:
#include <Q3TextStream>
#include "main.h"
#include "schematic.h"

#include <qregexp.h>
#include <qdir.h>
#include <qfileinfo.h>


VHDL_File::VHDL_File()
{
  Type = isDigitalComponent;
  Description = QObject::tr("VHDL file");

  Props.append(new Property("File", "sub.vhdl", false,
		QObject::tr("Name of VHDL file")));

  Model = "VHDL";
  Name  = "X";

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0));
}

// -------------------------------------------------------
Component* VHDL_File::newOne()
{
  VHDL_File *p = new VHDL_File();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* VHDL_File::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("VHDL file");
  BitmapFile = (char *) "vhdlfile";

  if(getNewOne) {
    VHDL_File *p = new VHDL_File();
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString VHDL_File::vhdlCode(int)
{
  QString s;
  Port *pp = Ports.first();
  if(pp) {
    s = "  " + Name + ": entity " + EntityName;

    // output all generic properties
    Property *pr = Props.at(1);
    if (pr) {
      s += " generic map (";
      s += pr->Value;
      for(pr = Props.next(); pr != 0; pr = Props.next())
	s += ", " + pr->Value;
      s += ")";
    }

    // output all node names
    s += " port map (";
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
QString VHDL_File::loadFile()
{
  QString File(Props.getFirst()->Value);
  QFileInfo Info(File);
  if(Info.isRelative())
    File = QucsSettings.QucsWorkDir.filePath(File);

  QFile f(File);
  if(!f.open(QIODevice::ReadOnly))
    return QString("");

  Q3TextStream stream(&f);
  File = stream.read();   // QString is better for "find" function
  f.close();

  // parse ports, i.e. network connections; and generics, i.e. parameters
  VHDL_File_Info VInfo(File);
  GenNames = VInfo.GenNames;
  EntityName = VInfo.EntityName;
  TypeNames = VInfo.TypeNames;
  GenTypes = VInfo.GenTypes;
  GenDefs = VInfo.GenDefs;
  return VInfo.PortNames;
}

// -------------------------------------------------------
void VHDL_File::createSymbol()
{
  QFontMetrics  metrics(QucsSettings.font);   // get size of text
  int fHeight = metrics.lineSpacing();

  int No = 0;
  TypeNames = "";
  QString tmp, PortNames = loadFile();
  if(!PortNames.isEmpty())
    No = PortNames.count(',') + 1;

  #define HALFWIDTH  17
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  tmp = QObject::tr("vhdl");
  int w = metrics.width(tmp);
  Texts.append(new Text(w/-2, fHeight/-2, tmp));

  int y = 15-h, i = 0;
  Port *pp;
  while(i<No) {
    Lines.append(new Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    pp = new Port(-30,  y);
    Ports.append(pp);
    pp->Type = TypeNames.section(',', i, i);
    tmp = PortNames.section(',', i, i);
    w = metrics.width(tmp);
    Texts.append(new Text(-19-w, y-fHeight-2, tmp));
    i++;

    if(i == No) break;
    Lines.append(new Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
    pp = new Port( 30,  y);
    Ports.append(pp);
    pp->Type = TypeNames.section(',', i, i);
    tmp = PortNames.section(',', i, i);
    Texts.append(new Text( 20, y-fHeight-2, tmp));
    y += 60;
    i++;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  tx = x1+4;
  ty = y2+4;

  // now create/modify properties
  No = 0;
  if(!GenNames.isEmpty())
    No = (GenNames.count(',')) + 1;
  Property * pr = Props.at(1);
  for(i=0; i<No; i++) {
    if (!pr) {
      pr = new Property(GenNames.section(',', i, i),
			GenDefs.section(',', i, i), true,
			QObject::tr("generic variable")+
			" "+QString::number(i+1));
      Props.append(pr);
      pr = 0;
    }
    else {
      pr->Description =
	QObject::tr("generic variable")+" "+QString::number(i+1);
      pr->Name = GenNames.section(',', i, i);
      pr = Props.next();
    }
  }
  // remove remaining properties if necessary
  y=Props.count()-1;
  for(i=No; i<y; i++) {
    Props.removeLast();
  }
}

// -------------------------------------------------------
QString VHDL_File::getSubcircuitFile()
{
  // construct full filename
  QString FileName = Props.getFirst()->Value;
  return properAbsFileName(FileName);
}

// -------------------------------------------------------
bool VHDL_File::createSubNetlist(QTextStream *stream)
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

  // write the whole VHDL file into the netlist output
  QByteArray FileContent = f.readAll();
  f.close();
  (*stream) << '\n';
  //? stream->writeRawBytes(FileContent.data(), FileContent.size());
  (*stream) << FileContent.data();
  (*stream) << '\n';
  return true;
}

// -------------------------------------------------------
VHDL_File_Info::VHDL_File_Info()
{
  EntityName = "";
  PortNames = "";
  TypeNames = "";
  GenTypes = "";
  GenNames = "";
  GenDefs = "";
}

// -------------------------------------------------------
VHDL_File_Info::VHDL_File_Info(QString File, bool isfile)
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
  PortNames = "";
  int i=0, j, k=0;
  while((i=File.find("--", i)) >= 0) { // remove all VHDL comments
    j = File.find('\n', i+2);          // This also finds "--" within a ...
    if(j < 0)                          // string, but as no strings are ...
      File = File.left(i);             // allowed in entity headers, it ...
    else                               // does not matter.
      File.remove(i, j-i);
  }

  QRegExp Expr;
  Expr.setCaseSensitive(false);
  for(;;) {
    k--;
    Expr.setPattern("\\bentity\\b");  // start of last entity
    k = File.findRev(Expr, k);
    if(k < 0)
      return;

    Expr.setPattern("\\bend\\b");    // end of last entity
    i = File.find(Expr, k+7);
    if(i < 0)
      return;
    s = File.mid(k+7, i-k-7);  // cut out entity declaration

    Expr.setPattern("\\b");
    i = s.find(Expr);
    if(i < 0)
      return;
    j = s.find(Expr, i+1);
    if(j < 0)
      return;
    EntityName = s.mid(i, j-i);  // save entity name

    i = s.find(Expr, j+1);
    if(i < 0)
      return;
    j = s.find(Expr, i+1);
    if(j < 0)
      return;
    if(s.mid(i, j-i).lower() == "is")   // really found start of entity ?
      break;

    if(k < 1)    // already searched the whole text
      return;
  }

  // parse ports, i.e. network connections; and generics, i.e. parameters
  GenNames = parseGenerics(s,j);
  PortNames = parsePorts(s,j);
}

// -------------------------------------------------------
QString VHDL_File_Info::parsePorts(QString s, int j)
{
  QRegExp Expr;
  Expr.setCaseSensitive(false);
  int i, p, l, k;

  Expr.setPattern("\\bport\\b");  // start of interface definition
  i = s.find(Expr, j+1);
  if(i < 0)
    return QString("");
  // find opening (
  i = s.find('(', i+4) + 1;
  if(i <= 0)
    return QString("");

  // find closing (
  p = i;
  j = i-1;
  do {
    j = s.find(')', j+1);
    if(j < 0)
      return QString("");
    p = s.find('(', p+1);
    if(p >= 0 && p > j) p = -1;
  } while (p >= 0);

  s = s.mid(i, j-i);
  s.remove('\n');
  s.remove('\t');

  // find port names and types in parameter specification
  l = i = 0;    // remove all VHDL identifiers (e.g. ": out bit;")
  QString types = "", t;
  while((i=s.find(':', i)) >= 0) {
    j = s.find(';', i+2);
    if(j < 0) {
      t = s.mid(i+1);
      t.remove(';');
      t = t.simplifyWhiteSpace();
      s = s.left(i);
    } else {
      t = s.mid(i+1, j-i);
      t.remove(';');
      t = t.simplifyWhiteSpace();
      s.remove(i, j-i);
    }
    if ((k = t.find(' ')) >= 0)
      t = t.mid(k+1);
    t = t.simplifyWhiteSpace();
    k = s.find(';',l+2);
    k = (s.mid(l,k-l).count(',')) + 1;
    while (k-->0) types = types + t + ",";
    i--;
    l = i;
  }

  s.remove(' ');
  s.replace(';', ',');
  TypeNames=types=types.left(types.length()-1);
  return s;
}

// -------------------------------------------------------
QString VHDL_File_Info::parseGenerics(QString s, int j)
{
  QRegExp Expr;
  Expr.setCaseSensitive(false);
  int i, p, l, k, n;

  Expr.setPattern("\\bgeneric\\b");
  i = s.find(Expr, j+1);
  if(i < 0)
    return QString("");
  // find opening (
  i = s.find('(', i+4) + 1;
  if(i <= 0)
    return QString("");

  // find closing (
  p = i;
  j = i-1;
  do {
    j = s.find(')', j+1);
    if(j < 0)
      return QString("");
    p = s.find('(', p+1);
    if(p >= 0 && p > j) p = -1;
  } while (p >= 0);

  s = s.mid(i, j-i);
  s.remove('\n');
  s.remove('\t');

  // find generic names, types and defaults in parameter specification
  l = i = 0;
  QString types = "", t, defs = "", d;
  while((i=s.find(':', i)) >= 0) {
    j = s.find(';', i+2);
    n = s.find(":=", i+2);
    d = "";
    if(n >= 0 && (n < j || j < 0) ) {
      j = s.find(';', n+2);
      if(j < 0) {
	d = s.mid(n+2);
	d = d.simplifyWhiteSpace();
	s = s.left(n);
      } else {
	d = s.mid(n+2, j-n-1);
	d.remove(';');
	d = d.simplifyWhiteSpace();
	s.remove(n, j-n);
      }
      j = s.find(';', n);
    }
    if(j < 0) {
      t = s.mid(i+1);
      t.remove(';');
      t = t.simplifyWhiteSpace();
      s = s.left(i);
    } else {
      t = s.mid(i+1, j-i);
      t.remove(';');
      t = t.simplifyWhiteSpace();
      s.remove(i, j-i);
    }
    if ((k = t.find(' ')) >= 0)
      t = t.mid(k+1);
    t = t.simplifyWhiteSpace();
    k = s.find(';',l+2);
    k = (s.mid(l,k-l).count(',')) + 1;
    while (k-->0) {
      types = types + t + ",";
      defs = defs + d + ",";
    }
    i--;
    l = i;
  }

  s.remove(' ');
  s.replace(';', ',');
  GenTypes=types=types.left(types.length()-1);
  GenDefs=defs=defs.left(defs.length()-1);
  return s;
}
