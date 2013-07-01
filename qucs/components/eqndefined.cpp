/***************************************************************************
                               eqndefined.cpp
                              ----------------
    begin                : Thu Apr 19 2007
    copyright            : (C) 2007 by Stefan Jahn
    email                : stefan@lkcc.org
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
#include "eqndefined.h"
#include "main.h"
#include "schematic.h"

#include <qfileinfo.h>


EqnDefined::EqnDefined()
{
  Description = QObject::tr("equation defined device");

  Model = "EDD";
  Name  = "D";

  // first properties !!!
  Props.append(new Property("Type", "explicit", false,
		QObject::tr("type of equations")+" [explicit, implicit]"));
  Props.append(new Property("Branches", "1", false,
		QObject::tr("number of branches")));

  // last properties
  Props.append(new Property("I1", "0", true,
		QObject::tr("current equation") + " 1"));
  Props.append(new Property("Q1", "0", false,
		QObject::tr("charge equation") + " 1"));

  createSymbol();
}

// -------------------------------------------------------
Component* EqnDefined::newOne()
{
  EqnDefined* p = new EqnDefined();
  p->Props.at(0)->Value = Props.at(0)->Value;
  p->Props.at(1)->Value = Props.at(1)->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* EqnDefined::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation Defined Device");
  BitmapFile = (char *) "edd";

  if(getNewOne) {
    EqnDefined* p = new EqnDefined();
    p->Props.at(0)->Value = "explicit";
    p->Props.at(1)->Value = "1";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString EqnDefined::netlist()
{
  QString s = Model+":"+Name;
  QString e = "\n";

  // output all node names
  for(Port *p1 = Ports.first(); p1 != 0; p1 = Ports.next())
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  Property *p2 = Props.at(2);
  while(p2) {
    s += " "+p2->Name+"=\""+Name+"."+p2->Name+"\"";
    e += "  Eqn:Eqn"+Name+p2->Name+" "+
      Name+"."+p2->Name+"=\""+p2->Value+"\" Export=\"no\"\n";
    p2 = Props.next();
  }

  return s+e;
}

// -------------------------------------------------------
void EqnDefined::createSymbol()
{
  int i, PortDistance = 60;

  // adjust branch number
  int Num = Props.at(1)->Value.toInt();
  if(Num < 1) Num = 1;
  else if(Num > 4) {
    PortDistance = 40;
    if(Num > 8) Num = 8;
  }
  Props.at(1)->Value = QString::number(Num);

  // adjust property number
  int NumProps = (Props.count() - 2) / 2;
  if (NumProps < Num) {
    for(i = NumProps; i < Num; i++) {
      Props.append(new Property("I"+QString::number(i+1), "0", false,
		QObject::tr("current equation") + " " +QString::number(i+1)));
      Props.append(new Property("Q"+QString::number(i+1), "0", false,
		QObject::tr("charge equation") + " " +QString::number(i+1)));
    }
  } else {
    for(i = Num; i < NumProps; i++) {
      Props.removeLast();
      Props.removeLast();
    }
  }

  // adjust property names
  Property * p1 = Props.at(2);
  for(i = 1; i <= Num; i++) {
    p1->Name = "I"+QString::number(i);
    p1 = Props.next();
    p1->Name = "Q"+QString::number(i);
    p1 = Props.next();
  }

  // draw symbol
  int h = (PortDistance/2)*((Num-1)) + PortDistance/2;
  Lines.append(new Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2)));


  i=0;
  int y = PortDistance/2-h, yh;
  while(i<Num) {
    i++;
    Lines.append(new Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30, y));

    Lines.append(new Line( 7,y-3, 10, y,QPen(Qt::black,1)));
    Lines.append(new Line( 7,y+3, 10, y,QPen(Qt::black,1)));
    Lines.append(new Line(-10, y, 10, y,QPen(Qt::black,1)));

    if (i > 1) {
      yh = y-PortDistance/2;
      Lines.append(new Line(-15, yh, 15, yh, QPen(Qt::darkBlue,2)));
    }

    Lines.append(new Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30, y));
    Texts.append(new Text( 19,y-14,QString::number(i)));
    y += PortDistance;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;

  QFontMetrics  metrics(QucsSettings.font);   // get size of text
  tx = x1+4;
  ty = y1 - 2*metrics.lineSpacing() - 4;
}
