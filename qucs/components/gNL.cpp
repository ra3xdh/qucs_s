/***************************************************************************
                         gNL.cpp  -  description
                   --------------------------------------
    begin                    : Tue. Mar 31 2015
    copyright              : (C) by Mike Brinson (mbrin72043@yahoo.co.uk),
														Vadim Kuznetsov (ra3xdh@gmail.com)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gNL.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


gNL::gNL()
{
  Description = QObject::tr("Equation defined (E-type) voltage source");
  // Value, Table and POLY forms are allowed: should work with ngspice and Xyce.

  Arcs.append(new Arc(-14,-14, 28, 28,     0, 16*360,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(20, 12,"GNL",Qt::black,10.0,0.0,-1.0));
  Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));
 
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "gNL";
  SpiceModel = "G";
  Name  = "G";

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

gNL::~gNL()
{
}

Component* gNL::newOne()
{
  return new gNL();
}

Element* gNL::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("G Non-linear");
  BitmapFile = (char *) "gNL";

  if(getNewOne)  return new gNL();
  return 0;
}

QString gNL::netlist()
{
    return QString("");
}

QString gNL::spice_netlist(bool isXyce)
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
