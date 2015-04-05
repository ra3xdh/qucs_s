/***************************************************************************
                         L_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Sat. 4 April 2015
    copyright              : (C) 2015 by Mike Brinson
    email                    : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "L_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


L_SPICE::L_SPICE()
{
  Description = QObject::tr("L SPICE format");
  
   Arcs.append(new Arc(-18, -6, 12, 12,  0, 16*180,QPen(Qt::darkRed,4)));
  Arcs.append(new Arc( -6, -6, 12, 12,  0, 16*180,QPen(Qt::darkRed,4)));
  Arcs.append(new Arc(  6, -6, 12, 12,  0, 16*180,QPen(Qt::darkRed,4)));
  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -10;
  x2 =  30; y2 =   6;

  tx = x1+4;
  ty = y2+4;

   Model = "L_SPICE";
  SpiceModel = "L";
  Name  = "L";

 Props.append(new Property("L  ", "", true,"Expression"));
 Props.append(new Property("L  ", "", false,"Expression"));
 Props.append(new Property("L  ", "", false,"Expression"));
 Props.append(new Property("L  ", "", false,"Expression"));
 Props.append(new Property("L  ", "", false,"Expression"));

 
 
  rotate();  // fix historical flaw
}

L_SPICE::~L_SPICE()
{
}

Component* L_SPICE::newOne()
{
  return new L_SPICE();
}

Element* L_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Inductor");
  BitmapFile = (char *) "L_SPICE";

  if(getNewOne)  return new L_SPICE();
  return 0;
}

QString L_SPICE::netlist()
{
    return QString("");
}

QString L_SPICE::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }


QString Line1 =  Props.at(0)->Value;
QString Line2 = Props.at(1)->Value;
QString Line3 = Props.at(2)->Value;
QString Line4 = Props.at(3)->Value;
QString Line5 = Props.at(4)->Value;
  
    if(Line1 != "")   s += QString("%1\n").arg(Line1);
    if(Line2 != "")   s += QString("%1\n").arg(Line2);
    if(Line3 != "")   s += QString("%1\n").arg(Line3);
    if(Line4 != "")   s += QString("%1\n").arg(Line4);
    if(Line5 != "")   s += QString("%1\n").arg(Line5); 
    
 
    return s;
}
