/***************************************************************************
                         vPWL.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Thurs. 26 March 2015
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
#include "vPWL.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


vPWL::vPWL()
{
  Description = QObject::tr("PWL voltage source");

  // normal voltage source symbol
  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(20, 12,"PWL",Qt::black,10.0,0.0,-1.0));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  -5, 18, -11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  -8, 15,  -8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  -5,-18, -11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "vPWL";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("Line1", "", true,"Expression"));
 Props.append(new Property("Line2", "", false,"Expression"));
 Props.append(new Property("Line3", "", false,"Expression"));
 Props.append(new Property("Line4", "", false,"Expression"));
 Props.append(new Property("Line5", "", false,"Expression"));
 Props.append(new Property("Line6", "", false,"Expression"));
 Props.append(new Property("Line7", "", false,"Expression"));
 Props.append(new Property("Line8", "", false,"Expression"));
 Props.append(new Property("Line9", "", false,"Expression"));
 Props.append(new Property("Line10", "", false,"Expression"));
 
 
  rotate();  // fix historical flaw
}

vPWL::~vPWL()
{
}

Component* vPWL::newOne()
{
  return new vPWL();
}

Element* vPWL::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("PWL");
  BitmapFile = (char *) "vPWL";

  if(getNewOne)  return new vPWL();
  return 0;
}

QString vPWL::netlist()
{
    return QString("");
}

QString vPWL::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }


QString Line1= Props.at(0)->Value;
QString Line2= Props.at(1)->Value;
QString Line3= Props.at(2)->Value;
QString Line4= Props.at(3)->Value;
QString Line5= Props.at(4)->Value;
QString Line6= Props.at(5)->Value;
QString Line7= Props.at(6)->Value;
QString Line8= Props.at(7)->Value;
QString Line9= Props.at(8)->Value;
QString Line10= Props.at(9)->Value;

    s += QString(" DC 0  AC 0  ");
  
    if(Line1 !="")   s += QString("%1\n").arg(Line1);
    if(Line2 !="")   s += QString("%1\n").arg(Line2);
    if(Line3 !="")   s += QString("%1\n").arg(Line3);
    if(Line4 !="")   s += QString("%1\n").arg(Line4);
    if(Line5 !="")   s += QString("%1\n").arg(Line5);
    if(Line6 !="")   s += QString("%1\n").arg(Line6);
    if(Line7 !="")   s += QString("%1\n").arg(Line7);
    if(Line8 !="")   s += QString("%1\n").arg(Line8);
    if(Line9 !="")   s += QString("%1\n").arg(Line9);
    if(Line10 !="") s += QString("%1\n").arg(Line10);
    return s;
}
