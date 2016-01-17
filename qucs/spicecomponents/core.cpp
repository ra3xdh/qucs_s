/***************************************************************************
                         core.cpp  -  description
                   --------------------------------------
    begin                  : Sun 22 Nov 2015
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

#include "core.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


core::core()
{
  Description = QObject::tr("XSPICE core block:\nseven line XSPICE specification. ");

  Lines.append(new Line(-40,   0,  -30,     0,QPen(Qt::darkBlue,2)));  // L1
  Lines.append(new Line(-30, -15,  30,    -15,QPen(Qt::blue,3)));      // L2 
  Lines.append(new Line( 30, -15,  30,     15,QPen(Qt::blue,3)));      // L3 
  Lines.append(new Line( 30,  15, -30,     15,QPen(Qt::blue,3)));      // L4 
  Lines.append(new Line(-30,  15, -30,    -15,QPen(Qt::blue,3)));  // L5 
  

  Lines.append(new Line(-25,   -5,  25,   -5,QPen(Qt::black,3)));      // L7
  Lines.append(new Line(-25,    0,  25,    0,QPen(Qt::black,3)));      // L8
  Lines.append(new Line(-25,    5,  25,    5,QPen(Qt::black,3)));      // L9
 
  Lines.append(new Line(40,     0, 30,     0,QPen(Qt::darkBlue,2)));  // L16  
  
 
  Lines.append(new Line( -40,  -30, -40,  -20,QPen(Qt::red,3)));        // +
  Lines.append(new Line( -45,  -25, -35,  -25,QPen(Qt::red,3)));
  Lines.append(new Line(  45,  -25,  35,  -25,QPen(Qt::black,3)));      // -  
    
  Ports.append(new Port( -40,    0));  // Pin
  Ports.append(new Port(  40,    0));  // Pout

  x1 = -35; y1 = -30;
  x2 =  35; y2 =  30;

  tx = x1+5;
  ty = y2+5;
  Model = "core";
  SpiceModel = "A";
  Name  = "CORE";
  
  Props.append(new Property("A ", "", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", "", false,".model line"));
  Props.append(new Property("A_Line 3", "", false,".model line"));
  Props.append(new Property("A_Line 4", "", false,".model line"));
  Props.append(new Property("A_Line 5", "", false,".model line"));
  Props.append(new Property("A_Line 6", "", false,".model line"));
  Props.append(new Property("A_Line 7", "", false,".model line"));

  //rotate();  // fix historical flaw
}

core::~core()
{
}

Component* core::newOne()
{
  return new core();
}

Element* core::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("core");
  BitmapFile = (char *) "core"; 

  if(getNewOne)  return new core();
  return 0;
}

QString core::netlist()
{
    return QString("");
}

QString core::spice_netlist(bool)
{
  
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QString P1 = Ports.at(0)->Connection->Name;
    QString P2 = Ports.at(1)->Connection->Name;

    
     s += " " + P1 + " " + P2 + "  ";
    
    QString A= Props.at(0)->Value;
    QString A_Line_2= Props.at(1)->Value;
    QString A_Line_3= Props.at(2)->Value; 
    QString A_Line_4= Props.at(3)->Value;
    QString A_Line_5= Props.at(4)->Value; 
    QString A_Line_6= Props.at(5)->Value;
    QString A_Line_7= Props.at(6)->Value; 
    if(  A.length()        > 0)    s += QString("%1\n").arg(A);
    if(  A_Line_2.length() > 0 )   s += QString("%1\n").arg(A_Line_2);
    if(  A_Line_3.length() > 0 )   s += QString("%1\n").arg(A_Line_3);
    if(  A_Line_4.length() > 0 )   s += QString("%1\n").arg(A_Line_4);
    if(  A_Line_5.length() > 0 )   s += QString("%1\n").arg(A_Line_5);
    if(  A_Line_6.length() > 0 )   s += QString("%1\n").arg(A_Line_6);
    if(  A_Line_7.length() > 0 )   s += QString("%1\n").arg(A_Line_7);
 
    return s;
}
