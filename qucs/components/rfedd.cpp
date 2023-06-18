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
#include "rfedd.h"
#include "main.h"
#include "schematic.h"

#include <QFileInfo>


RFedd::RFedd()
{
  Description = QObject::tr("equation defined RF device");
  Simulator = spicecompat::simQucsator;

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
  for (Port *p1 : Ports)
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
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0); 
  int fHeight = smallmetrics.lineSpacing();
  QString tmp;
  int w, i;

  // adjust port number
  int No = Props.at(1)->Value.toInt();
  if(No < 1) No = 1;
  if(No > 8) No = 8;
  Props.at(1)->Value = QString::number(No);

  // adjust property number and names
  int NumProps = Props.count() - 3; // Type, Ports, duringDC
  if (NumProps < No * No) { // number of ports was increased, add properties
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
  } else { // number of ports was decreased, remove properties
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
  Lines.append(new qucs::Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  // component text name, centered
  tmp = QObject::tr("RF");
  w = smallmetrics.boundingRect(tmp).width();
  Texts.append(new Text(-w/2, -fHeight/2, tmp)); // text centered in box

  i = 0;
  int y = 15-h;
  while(i<No) { // add ports lines and numbers
    // left side
    Lines.append(new qucs::Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    tmp = QString::number(i+1);
    w = smallmetrics.boundingRect(tmp).width();
    Texts.append(new Text(-25-w, y-fHeight-2, tmp)); // text right-aligned
    i++;

    if(i == No) break; // if odd number of ports there will be one port less on the right side
    // right side
    Lines.append(new qucs::Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    tmp = QString::number(i+1);
    Texts.append(new Text(25, y-fHeight-2, tmp)); // text left-aligned
    y += 60;
    i++;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);   // use the screen-compatible metric
  tx = x1+4;
  ty = y1 - metrics.lineSpacing() - 4;
}
