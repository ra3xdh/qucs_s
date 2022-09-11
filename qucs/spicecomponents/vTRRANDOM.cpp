/***************************************************************************
                         vTRRANDOM.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Tue. 31 March 2015
    copyright              : (C) 2015 by Mike Brinson
    email                    : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                                                                    *
 *   This program is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or            *
 *   (at your option) any later version.                                                                            *
 *                                                                                                                     *
 ***************************************************************************/
#include "vTRRANDOM.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h" 


vTRRANDOM::vTRRANDOM()
{
  Description = QObject::tr("SPICE V(TRRANDOM):");

  // normal voltage source symbol
  Arcs.append(new qucs::Arc(-12,-12, 24, 24, 0, 16*360,QPen(Qt::blue,3)));
   Texts.append(new Text(36, 4,"TRR",Qt::blue,10.0,0.0,-1.0)); 
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(-12,  1,  1,-12,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(-10,  6,  6,-10,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line( -7, 10, 10, -7,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line( -2, 12, 12, -2,QPen(Qt::darkBlue,3)));
  
  Lines.append(new qucs::Line( 18,  -5, 18, -11,QPen(Qt::red,2)));
  Lines.append(new qucs::Line( 21,  -8, 15,  -8,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(-18,  -5,-18, -11,QPen(Qt::black,2))); 


  
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "vTRRANDOM";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("Type",  "2", true,
		QObject::tr(" Distribution selector (1 to 4)")));
  Props.append(new Property("Ts",  "1m", true,
		QObject::tr("Duration of each random voltage value")));
  Props.append(new Property("Td",  "0", true,
		QObject::tr("Time delay before random voltages output ( for time < Td  Vout  =  0 V)")));
  Props.append(new Property("Param1",  "1", true,
		QObject::tr("Changes with  different values of Type.")));
   Props.append(new Property("Param2",  "0", true,
		QObject::tr("Changes with different values of Type")));
 
  rotate();  // fix historical flaw
}

vTRRANDOM::~vTRRANDOM()
{
}

Component* vTRRANDOM::newOne()
{
  return new vTRRANDOM();
}

Element* vTRRANDOM::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("V(TRRANDOM)");
  BitmapFile = (char *) "vTRRANDOM";

  if(getNewOne)  return new vTRRANDOM();
  return 0;
}

QString vTRRANDOM::netlist()
{
    return QString("");
}

QString vTRRANDOM::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

   QString Type= spicecompat::normalize_value(Props.at(0)->Value);
   QString Ts= spicecompat::normalize_value(Props.at(1)->Value);
   QString Td= spicecompat::normalize_value(Props.at(2)->Value);
   QString Param1 = spicecompat::normalize_value(Props.at(3)->Value);
   QString Param2 = spicecompat::normalize_value(Props.at(4)->Value);

    s += QString(" DC 0 AC 0 TRRANDOM(%1 %2 %3 %4 %5 ) \n").arg(Type).arg(Ts).arg(Td).arg(Param1).arg(Param2);
    return s;
}
