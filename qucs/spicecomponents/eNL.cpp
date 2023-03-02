/***************************************************************************
                         eNL.cpp  -  description
                   --------------------------------------
    begin                  : Fri Mar 27 2015
    copyright              : (C) by Mike Brinson (mbrin72043@yahoo.co.uk),
						   :  Vadim Kuznetsov (ra3xdh@gmail.com)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "eNL.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


eNL::eNL()
{
  Description = QObject::tr("SPICE E (CUR, VALUE, TABLE, POLY):\nMultiple line ngspice non-linear E specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");
  // Value, Table and POLY forms are allowed.

  Arcs.append(new qucs::Arc(-14,-14, 28, 28,     0, 16*360,QPen(Qt::blue,3)));
  Texts.append(new Text(30,12,"ENL",Qt::blue,10.0,0.0,-1.0));
  Lines.append(new qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18,  -5, 18, -11,QPen(Qt::red,2)));
  Lines.append(new qucs::Line( 21,  -8, 15,  -8,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(-18,  -5,-18, -11,QPen(Qt::black,2))); 

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "eNL";
  SpiceModel = "E";
  Name  = "E";

  Props.append(new Property("E", "", true,"Non-linear spec.\n(with control nodes)"));
  Props.append(new Property("Line_2", "", false,"+ continuation line 1"));
  Props.append(new Property("Line_3", "", false,"+ continuation line 2"));
  Props.append(new Property("Line_4", "", false,"+ continuation line 3"));
  Props.append(new Property("Line_5", "", false,"+ continuation line 4"));
  
  rotate();  // fix historical flaw
}

eNL::~eNL()
{
}

Component* eNL::newOne()
{
  return new eNL();
}

Element* eNL::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("E");
  BitmapFile = (char *) "eNL"; 

  if(getNewOne)  return new eNL();
  return 0;
}

QString eNL::netlist()
{
    return QString("");
}

QString eNL::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
    QString E = Props.at(0)->Value;
    QString Line_2 = Props.at(1)->Value; 
    QString Line_3 = Props.at(2)->Value;
    QString Line_4 = Props.at(3)->Value;
    QString Line_5 = Props.at(4)->Value;

    
    if(  E.length()  > 0)        s += QString("%1").arg(E);
    if(  Line_2.length() > 0 )   s += QString("\n%1").arg(Line_2);
    if(  Line_3.length() > 0 )   s += QString("\n%1").arg(Line_3);
    if(  Line_4.length() > 0 )   s += QString("\n%1").arg(Line_4);
    if(  Line_5.length() > 0 )   s += QString("\n%1").arg(Line_5);
    s += "\n";
    return s;
}
