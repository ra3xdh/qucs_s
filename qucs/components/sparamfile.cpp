/***************************************************************************
                               sparamfile.cpp
                              ----------------
    begin                : Sat Aug 23 2003
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
#include <QtGui>
#include "sparamfile.h"
#include "main.h"
#include "schematic.h"

#include <qfileinfo.h>



SParamFile::SParamFile()
{
  Description = QObject::tr("S parameter file");

  Model = "SPfile";
  Name  = "X";

  // must be the first property !!!
  Props.append(new Property("File", "test.s1p", true,
		QObject::tr("name of the s parameter file")));
  Props.append(new Property("Data", "rectangular", false,
		QObject::tr("data type")+" [rectangular, polar]"));
  Props.append(new Property("Interpolator", "linear", false,
		QObject::tr("interpolation type")+" [linear, cubic]"));
  Props.append(new Property("duringDC", "open", false,
		QObject::tr("representation during DC analysis")+
			    " [open, short, shortall, unspecified]"));

  // must be the last property !!!
  Props.append(new Property("Ports", "1", false,
		QObject::tr("number of ports")));

  createSymbol();
}

// -------------------------------------------------------
Component* SParamFile::newOne()
{
  SParamFile* p = new SParamFile();
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* SParamFile::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-port S parameter file");
  BitmapFile = (char *) "spfile3";

  if(getNewOne) {
    SParamFile* p = new SParamFile();
    p->Props.getFirst()->Value = "test.s3p";
    p->Props.getLast()->Value = "3";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
Element* SParamFile::info1(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("1-port S parameter file");
  BitmapFile = (char *) "spfile1";

  if(getNewOne)  return new SParamFile();
  return 0;
}

// -------------------------------------------------------
Element* SParamFile::info2(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2-port S parameter file");
  BitmapFile = (char *) "spfile2";

  if(getNewOne) {
    SParamFile* p = new SParamFile();
    p->Props.getFirst()->Value = "test.s2p";
    p->Props.getLast()->Value = "2";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString SParamFile::getSubcircuitFile()
{
  // construct full filename
  QString FileName = Props.getFirst()->Value;
  return properAbsFileName(FileName);
}

// -------------------------------------------------------
QString SParamFile::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  for(Port *p1 = Ports.first(); p1 != 0; p1 = Ports.next())
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  Property *p2 = Props.first();
  s += " "+p2->Name+"=\"{"+getSubcircuitFile()+"}\"";

  // data type
  p2 = Props.next();
  s += " "+p2->Name+"=\""+p2->Value+"\"";

  // interpolator type
  p2 = Props.next();
  s += " "+p2->Name+"=\""+p2->Value+"\"";

  // DC property
  p2 = Props.next();
  s += " "+p2->Name+"=\""+p2->Value+"\"\n";

  return s;
}

// -------------------------------------------------------
void SParamFile::createSymbol()
{
  int PortDistance = 60;
  int Num = Props.getLast()->Value.toInt();
  if(Num < 1) Num = 1;
  else if(Num > 8) {
    PortDistance = 20;
    if(Num > 40) Num = 40;
  }
  Props.getLast()->Value = QString::number(Num);

  int h = (PortDistance/2)*((Num-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2)));
  Texts.append(new Text( -9, -6,QObject::tr("file")));


  int i=0, y = 15-h;
  while(i<Num) {
    i++;
    Lines.append(new Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30, y));
    Texts.append(new Text(-25,y-14,QString::number(i)));

    if(i == Num) break;
    i++;
    Lines.append(new Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30, y));
    Texts.append(new Text( 19,y-14,QString::number(i)));
    y += PortDistance;
  }

  Lines.append(new Line( 0, h, 0,h+15,QPen(Qt::darkBlue,2)));
  Texts.append(new Text( 4, h,"Ref"));
  Ports.append(new Port( 0,h+15));    // 'Ref' port

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+15;

  QFontMetrics  metrics(QucsSettings.font);   // get size of text
  tx = x1+4;
  ty = y1 - 2*metrics.lineSpacing() - 4;
}
