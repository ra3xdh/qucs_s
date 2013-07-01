/***************************************************************************
                               rfedd.cpp
                              ----------------
    begin                : Sub Feb 17 2008
    copyright            : (C) 2008 by Stefan Jahn
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
#include "rfedd.h"
#include "main.h"
#include "schematic.h"

#include <qfileinfo.h>



RFedd::RFedd()
{
  Description = QObject::tr("equation defined RF device");

  Model = "RFEDD";
  Name  = "RF";

  // first properties !!!
  Props.append(new Property("Type", "Y", false,
		QObject::tr("type of parameters")+" [Y, Z, S]"));
  Props.append(new Property("Ports", "2", false,
		QObject::tr("number of ports")));
  Props.append(new Property("duringDC", "open", false,
		QObject::tr("representation during DC analysis")+
			    " [open, short, unspecified, zerofrequency]"));

  // last properties
  Props.append(new Property("P11", "0", false,
		QObject::tr("parameter equation") + " 11"));
  Props.append(new Property("P12", "0", false,
		QObject::tr("parameter equation") + " 12"));
  Props.append(new Property("P21", "0", false,
		QObject::tr("parameter equation") + " 21"));
  Props.append(new Property("P22", "0", false,
		QObject::tr("parameter equation") + " 22"));

  createSymbol();
}

// -------------------------------------------------------
Component* RFedd::newOne()
{
  RFedd* p = new RFedd();
  p->Props.at(0)->Value = Props.at(0)->Value;
  p->Props.at(1)->Value = Props.at(1)->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* RFedd::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation Defined RF Device");
  BitmapFile = (char *) "rfedd";

  if(getNewOne) {
    RFedd* p = new RFedd();
    p->Props.at(0)->Value = "Y";
    p->Props.at(1)->Value = "2";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString RFedd::netlist()
{
  QString s = Model+":"+Name;
  QString e = "\n";
  QString n, p;

  // output all node names
  for(Port *p1 = Ports.first(); p1 != 0; p1 = Ports.next())
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  Property *p2;
  p2 = Props.at(0);
  s += " "+p2->Name+"=\""+p2->Value+"\"";
  p = p2->Value;
  p2 = Props.at(2);
  s += " "+p2->Name+"=\""+p2->Value+"\"";
  p2 = Props.at(3);
  while(p2) {
    n = p2->Name.mid(1);
    s += " "+p2->Name+"=\""+Name+"."+p+n+"\"";
    e += "  Eqn:Eqn"+Name+p2->Name+" "+
      Name+"."+p+n+"=\""+p2->Value+"\" Export=\"no\"\n";
    p2 = Props.next();
  }

  return s+e;
}

// -------------------------------------------------------
void RFedd::createSymbol()
{
  QFontMetrics  metrics(QucsSettings.font);   // get size of text
  int fHeight = metrics.lineSpacing();
  int w, i;
  QString tmp;

  // adjust port number
  int No = Props.at(1)->Value.toInt();
  if(No < 1) No = 1;
  if(No > 8) No = 8;
  Props.at(1)->Value = QString::number(No);

  // adjust property number and names
  int NumProps = Props.count() - 3;
  if (NumProps < No * No) {
    for(i = 0; i < NumProps; i++) {
      tmp=QString::number((i)/No+1)+QString::number((i)%No+1);
      Props.at(i+3)->Name="P"+tmp;
      Props.at(i+3)->Description=QObject::tr("parameter equation") + " " +tmp;
    }
    for(i = NumProps; i < No * No; i++) {
      tmp=QString::number((i)/No+1)+QString::number((i)%No+1);
      Props.append(new Property("P"+tmp, "0", false,
		QObject::tr("parameter equation") + " " +tmp));
    }
  } else {
    for(i = No * No; i < NumProps; i++) {
      Props.removeLast();
    }
    for(i = 0; i < No * No; i++) {
      tmp=QString::number((i)/No+1)+QString::number((i)%No+1);
      Props.at(i+3)->Name="P"+tmp;
      Props.at(i+3)->Description=QObject::tr("parameter equation") + " " +tmp;
    }
  }

  // draw symbol
  #define HALFWIDTH  17
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  i = fHeight/2;
  tmp = QObject::tr("RF");
  w = metrics.width(tmp);
  Texts.append(new Text(w/-2, -i, tmp));

  i = 0;
  int y = 15-h;
  while(i<No) {
    Lines.append(new Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    tmp = QString::number(i+1);
    w = metrics.width(tmp);
    Texts.append(new Text(-20-w, y-fHeight-2, tmp));
    i++;

    if(i == No) break;
    Lines.append(new Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    tmp = QString::number(i+1);
    Texts.append(new Text( 20, y-fHeight-2, tmp));
    y += 60;
    i++;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;

  tx = x1+4;
  ty = y1 - fHeight - 4;
}
