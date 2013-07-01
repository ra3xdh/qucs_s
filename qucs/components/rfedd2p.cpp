/***************************************************************************
                               rfedd2p.cpp
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
#include "rfedd2p.h"
#include "main.h"
#include "schematic.h"

#include <qfileinfo.h>



RFedd2P::RFedd2P()
{
  Description = QObject::tr("equation defined 2-port RF device");

  Model = "RFEDD2P";
  Name  = "RF";

  // first properties !!!
  Props.append(new Property("Type", "Y", false,
		QObject::tr("type of parameters")+" [Y, Z, S, H, G, A, T]"));
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
Component* RFedd2P::newOne()
{
  RFedd2P* p = new RFedd2P();
  p->Props.at(0)->Value = Props.at(0)->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* RFedd2P::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation Defined 2-port RF Device");
  BitmapFile = (char *) "rfedd";

  if(getNewOne) {
    RFedd2P* p = new RFedd2P();
    p->Props.at(0)->Value = "Y";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString RFedd2P::netlist()
{
  QString s = "RFEDD:"+Name;
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
  p2 = Props.at(1);
  s += " "+p2->Name+"=\""+p2->Value+"\"";
  p2 = Props.at(2);
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
void RFedd2P::createSymbol()
{
  QFontMetrics  metrics(QucsSettings.font);   // get size of text
  int fHeight = metrics.lineSpacing();
  int w, i;
  QString tmp;

  // draw symbol
  #define HALFWIDTH  17
  int h = 15;
  Lines.append(new Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  i = fHeight/2;
  tmp = Props.at(0)->Value;
  w = metrics.width(tmp);
  Texts.append(new Text(w/-2, -i, tmp));

  i = 0;
  int y = 15-h;
  Lines.append(new Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
  Ports.append(new Port(-30,  y));
  tmp = QString::number(i+1);
  w = metrics.width(tmp);
  Texts.append(new Text(-20-w, y-fHeight-2, tmp));
  i++;

  Lines.append(new Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
  Ports.append(new Port( 30,  y));
  tmp = QString::number(i+1);
  Texts.append(new Text( 20, y-fHeight-2, tmp));
  y += 60;
  i++;

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;

  tx = x1+4;
  ty = y1 - fHeight - 4;
}
