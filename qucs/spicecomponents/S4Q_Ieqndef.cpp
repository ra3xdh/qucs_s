/***************************************************************************
                         S4Q_Ieqndef.cpp  -  description
                   ---------------------------------------
    begin                    : 13 September 2015
    copyright                : (C) 2015 by Vadim Kaznetsov and Mike Brinson
    email                    : ra3xdh@gmil.co and mbrin72043@yahoo.co.uk
 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "S4Q_Ieqndef.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"
#include "extsimkernels/verilogawriter.h"


S4Q_Ieqndef::S4Q_Ieqndef()
{
  Description = QObject::tr("SPICE B (I type):\nMultiple line ngspice or Xyce B specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.  "); 

  Arcs.append(new qucs::Arc(-14,-14, 28, 28,     0, 16*360,QPen(Qt::darkRed,3)));
 
  Lines.append(new qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 14,  0,QPen(Qt::darkBlue,2)));
 
  
  Lines.append(new qucs::Line( -8,  0, 8, 0,QPen(Qt::darkRed,3)));
  Lines.append(new qucs::Line( -8,  0, -4,  -4,QPen(Qt::darkRed,3)));
  Lines.append(new qucs::Line( -8,  0, -4,   4,QPen(Qt::darkRed,3)));
  
  Texts.append(new Text(30,-30,"Eqn",Qt::darkRed,10.0,0.0,-1.0));
  

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "S4Q_Ieqndef";
  SpiceModel = "B";
  Name  = "B";
  
  Props.append(new Property("I", "", true,"B(I) specification."));
  Props.append(new Property("Line_2", "", false,"+ continuation line 1"));
  Props.append(new Property("Line_3", "", false,"+ continuation line 2"));
  Props.append(new Property("Line_4", "", false,"+ continuation line 3"));
  Props.append(new Property("Line_5", "", false,"+ continuation line 4"));



// Props.append(new Property("I", "0", true,"Expression"));

  rotate();  // fix historical flaw
}

S4Q_Ieqndef::~S4Q_Ieqndef()
{
}

Component* S4Q_Ieqndef::newOne()
{
  return new S4Q_Ieqndef();
}

Element* S4Q_Ieqndef::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("B source (I)");
  BitmapFile = (char *) "S4Q_Ieqndef";

  if(getNewOne)  return new S4Q_Ieqndef();
  return 0;
}

QString S4Q_Ieqndef::netlist()
{
    return QString("");
}

QString S4Q_Ieqndef::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam  + " ";   // node names
    }
    
    QString VI  = Props.at(0)-> Name;
    QString VI2 = Props.at(0)->Value;
    QString Line_2 = Props.at(1)->Value;
    QString Line_3 = Props.at(2)->Value;
    QString Line_4 = Props.at(3)->Value;
    QString Line_5 = Props.at(4)->Value;

    s += QString(" %1 = %2 \n").arg(VI).arg(VI2);
    if(  Line_2.length() > 0 )   s += QString("%1").arg(Line_2);
    if(  Line_3.length() > 0 )   s += QString("\n%1").arg(Line_3);
    if(  Line_4.length() > 0 )   s += QString("\n%1").arg(Line_4);
    if(  Line_5.length() > 0 )   s += QString("\n%1").arg(Line_5);
    s += "\n";

    return s;
}

QString S4Q_Ieqndef::va_code()
{
    QString s;
    QString plus = Ports.at(0)->Connection->Name;
    QString minus = Ports.at(1)->Connection->Name;
    QString Src;
    if (Props.at(0)->Name=="I") Src = vacompat::normalize_current(plus,minus,true);
    else Src = vacompat::normalize_voltage(plus,minus); // Voltage contribution is reserved for future
    // B-source may be polar
    if (plus=="gnd") s = QString(" %1 <+ -(%2); // %3 source\n").arg(Src).arg(Props.at(0)->Value).arg(Name);
    else s = QString(" %1 <+ %2; // %3 source\n").arg(Src).arg(Props.at(0)->Value).arg(Name);
    return s;
}
