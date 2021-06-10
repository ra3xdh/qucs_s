/***************************************************************************
                         Icouple.cpp  -  description
                   --------------------------------------
    begin                  : Wed 11 Nov 2015
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

#include "Icouple.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


Icouple::Icouple()
{
  Description = QObject::tr("XSPICE coupled inductor block:\ntwo line XSPICE specification. ");

  Lines.append(new Line(-40,  -40, -40,  -30,QPen(Qt::darkBlue,2)));  // L1
  Lines.append(new Line(-40,  -30, -40,  -25,QPen(Qt::blue,3)));      // L2 
  Lines.append(new Line(-40,  -25, -30,  -25,QPen(Qt::blue,3)));      // L3 
  Lines.append(new Line(-30,  -25, -30,  -15,QPen(Qt::blue,3)));      // L4 
  Lines.append(new Line(-30,  -15, -40,  -15,QPen(Qt::blue,3)));      // L5 
  Lines.append(new Line(-30,  -15, -40,  -15,QPen(Qt::blue,3)));      // L6 
  Lines.append(new Line(-40,  -15, -40,   -5,QPen(Qt::blue,3)));      // L7
  Lines.append(new Line(-40,   -5, -30,   -5,QPen(Qt::blue,3)));      // L8
  Lines.append(new Line(-30,   -5, -30,    5,QPen(Qt::blue,3)));      // L9
  Lines.append(new Line(-30,    5, -40,    5,QPen(Qt::blue,3)));      // L10
  Lines.append(new Line(-40,    5, -40,   15,QPen(Qt::blue,3)));      // L11
  Lines.append(new Line(-40,   15, -30,   15,QPen(Qt::blue,3)));      // L12
  Lines.append(new Line(-30,   15, -30,   25,QPen(Qt::blue,3)));      // L13
  Lines.append(new Line(-30,   25, -40,   25,QPen(Qt::blue,3)));      // L14  
  Lines.append(new Line(-40,   25, -40,   30,QPen(Qt::blue,3)));      // L15 
  Lines.append(new Line(-40,   30, -40,   40,QPen(Qt::darkBlue,2)));  // L16  
  
  Lines.append(new Line(  40,  -40, 40,  -30,QPen(Qt::darkBlue,2)));  // L17
  Lines.append(new Line(  40,  -30, 40,  -20,QPen(Qt::blue,3)));      // L18
  Lines.append(new Line(  40,  -20, 20,    0,QPen(Qt::blue,3)));      // L19
  Lines.append(new Line(  20,    0, 40,   20,QPen(Qt::blue,3)));      // L20
  Lines.append(new Line(  40,   20, 60,    0,QPen(Qt::blue,3)));      // L21
  Lines.append(new Line(  60,   00, 40,  -20,QPen(Qt::blue,3)));      // L22 
  Lines.append(new Line(  40,   20, 40,   30,QPen(Qt::blue,3)));      // L23
  Lines.append(new Line(  40,   30, 40,   40,QPen(Qt::darkBlue,2)));  // L24  
  
  Lines.append(new Line( 20,  -20, 20,  -10,QPen(Qt::red,3)));        // +
  Lines.append(new Line( 15,  -15, 25,  -15,QPen(Qt::red,3)));
  Lines.append(new Line( 15,   20, 25,   20,QPen(Qt::black,3)));      // -  
    
  Ports.append(new Port( -40,  -40));  // Pplus
  Ports.append(new Port( -40,   40));  // Pneg
  Ports.append(new Port(  40,  -40));  // PVplus
  Ports.append(new Port(  40,   40));  // PVneg


  x1 = -45; y1 = -45;
  x2 =  50; y2 =  45;

  tx = x1+5;
  ty = y2+5;
  Model = "Icouple";
  SpiceModel = "A";
  Name  = "ICOUPLE";
  
  Props.append(new Property("A ", "", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", "", false,".model line"));
 
  //rotate();  // fix historical flaw
}

Icouple::~Icouple()
{
}

Component* Icouple::newOne()
{
  return new Icouple();
}

Element* Icouple::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Icouple");
  BitmapFile = (char *) "Icouple"; 

  if(getNewOne)  return new Icouple();
  return 0;
}

QString Icouple::netlist()
{
    return QString("");
}

QString Icouple::spice_netlist(bool)
{
  
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QString P1 = Ports.at(0)->Connection->Name;
    QString P2 = Ports.at(1)->Connection->Name;
    QString P3 = Ports.at(2)->Connection->Name;
    QString P4 = Ports.at(3)->Connection->Name;
    
     s += " " + P1 + " " + P2 + " " + P3 + " " + P4 + " ";
    
    QString A= Props.at(0)->Value;
    QString A_Line_2= Props.at(1)->Value;
    if(  A.length()        > 0)    s += QString("%1").arg(A);
    if(  A_Line_2.length() > 0 )   s += QString("\n%1").arg(A_Line_2);
    s += "\n";

    return s;
}
