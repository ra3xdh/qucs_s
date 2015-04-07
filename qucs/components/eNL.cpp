/***************************************************************************
                         eNL.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 27 2015
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

#include "eNL.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


eNL::eNL()
{
  Description = QObject::tr("Equation defined (E-type) voltage source");
  // Value, Table and POLY forms are allowed: should work with ngspice and Xyce.

  Arcs.append(new Arc(-14,-14, 28, 28,     0, 16*360,QPen(Qt::cyan,3)));
  Texts.append(new Text(20,12,"ENL",Qt::cyan,10.0,0.0,-1.0));
  Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  -5, 18, -11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  -8, 15,  -8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  -5,-18, -11,QPen(Qt::black,1))); 

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "eNL";
  SpiceModel = "E";
  Name  = "E";

  Props.append(new Property("E", "", true,"Expression"));
  Props.append(new Property("Line_2", "", false,"Expression"));
  Props.append(new Property("Line_3", "", false,"Expression"));
  Props.append(new Property("Line_4", "", false,"Expression"));
  Props.append(new Property("Line_5", "", false,"Expression"));
  Props.append(new Property("Line_6", "", false,"Expression"));
  Props.append(new Property("Line_7", "", false,"Expression"));
  Props.append(new Property("Line_8", "", false,"Expression"));
  Props.append(new Property("Line_9", "", false,"Expression"));
  Props.append(new Property("Line_10", "", false,"Expression"));
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
  Name = QObject::tr("E Non-linear independent voltage source");
  BitmapFile = (char *) "eNL";

  if(getNewOne)  return new eNL();
  return 0;
}

QString eNL::netlist()
{
    return QString("");
}

QString eNL::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }
    QString E = Props.at(0)->Value;
    QString Line_2 = Props.at(1)->Value; 
    QString Line_3 = Props.at(2)->Value;
    QString Line_4 = Props.at(3)->Value;
    QString Line_5 = Props.at(4)->Value;
    QString Line_6 = Props.at(5)->Value;
    QString Line_7 = Props.at(6)->Value;
    QString Line_8 = Props.at(7)->Value;
    QString Line_9 = Props.at(8)->Value;
    QString Line_10 = Props.at(9)->Value;
    
 if(  E.length()  > 2)        s += QString("%1\n").arg(E);
    if(  Line_2.length() > 2 )     s += QString("%1\n").arg(Line_2);
    if(  Line_3.length() > 2 )   s += QString("%1\n").arg(Line_3);
    if(  Line_4.length() > 2 )   s += QString("%1\n").arg(Line_4);
    if(  Line_5.length() > 2 )   s += QString("%1\n").arg(Line_5);
    if(  Line_6.length () > 2 )   s += QString("%1\n").arg(Line_6);
    if(  Line_7.length ()  > 2 )   s += QString("%1\n").arg(Line_7);
    if(  Line_8.length()  > 2)   s += QString("%1\n").arg(Line_8);
    if(  Line_9.length()  > 2 )   s += QString("%1\n").arg(Line_9);
    if(  Line_10.length() > 2 ) s += QString("%1\n").arg(Line_10);
 
    return s;
}
