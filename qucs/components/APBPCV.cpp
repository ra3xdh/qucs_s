/***************************************************************************
                         APBPCV.cpp  -  description
                   --------------------------------------
    begin                  : Wed 12 Nov 2015
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

#include "APBPCV.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


APBPCV::APBPCV()
{
  Description = QObject::tr("XSPICE three input voltage adder block:\nTwo line XSPICE specification. ");

  Lines.append(new Line(-50,  -20, -30,  -20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-50,    0, -30,    0,QPen(Qt::darkBlue,2))); 
  Lines.append(new Line(-50,   20, -30,   20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 50,    0,  30,    0,QPen(Qt::darkBlue,2)));

  
  Lines.append(new Line( -30,   30, -30, -30,QPen(Qt::blue,3)));
  Lines.append(new Line( -30,  -30,  30, -10,QPen(Qt::blue,3)));
  Lines.append(new Line(  30,  -10,  30,  10,QPen(Qt::blue,3))); 
  Lines.append(new Line(  30,   10, -30,  30,QPen(Qt::blue,3)));
  
  Lines.append(new Line(   0,  -10,  0,  10,QPen(Qt::blue,3)));   // +
  Lines.append(new Line( -10,    0, 10,   0,QPen(Qt::blue,3)));
    
  Ports.append(new Port( -50,  -20));  // PA
  Ports.append(new Port( -50,   20));  // PB
  Ports.append(new Port( -50,    0));  // PC
  Ports.append(new Port(  50,    0));  // POut


  x1 = -40; y1 = -24;
  x2 =  40; y2 =  24;

  tx = x1+12;
  ty = y2+12;
  Model = "APBPCV";
  SpiceModel = "A";
  Name  = "ADD";
  
  Props.append(new Property("A ", "", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", "", false,".model line"));

  //rotate();  // fix historical flaw
}

APBPCV::~APBPCV()
{
}

Component* APBPCV::newOne()
{
  return new APBPCV();
}

Element* APBPCV::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ADD");
  BitmapFile = (char *) "APBPCV"; 

  if(getNewOne)  return new APBPCV();
  return 0;
}

QString APBPCV::netlist()
{
    return QString("");
}

QString APBPCV::spice_netlist(bool)
{
  
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QString P1 = Ports.at(0)->Connection->Name;
    QString P2 = Ports.at(1)->Connection->Name;
    QString P3 = Ports.at(2)->Connection->Name;
    QString P4 = Ports.at(3)->Connection->Name;
    
     s += " [ " + P1 + " " + P2 + " " + P3 + " ] " + P4 + "  ";
    
    QString A= Props.at(0)->Value;
    QString A_Line_2= Props.at(1)->Value;
 
    if(  A.length()        > 0)    s += QString("%1\n").arg(A);
    if(  A_Line_2.length() > 0 )   s += QString("%1\n").arg(A_Line_2);
 
    return s;
}
