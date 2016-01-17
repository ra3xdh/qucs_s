/***************************************************************************
                         DE_V_int.cpp  -  description
                   --------------------------------------
    begin                  : Sat Oct 3 2015
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

#include "DE_V_int.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


DE_V_int::DE_V_int()
{
  Description = QObject::tr("XSPICE A_int_DE block:\nTwo line XSPICE specification. ");

  Lines.append(new Line(-50,  -20, -30,  -20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-50,   20, -30,   20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 50,  -10,  30,  -10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 50,   10,  30,   10,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line( -30,   30, -30, -30,QPen(Qt::blue,3)));
  Lines.append(new Line( -30,  -30,  30, -10,QPen(Qt::blue,3)));
  Lines.append(new Line(  30,  -10,  30,  10,QPen(Qt::blue,3))); 
  Lines.append(new Line(  30,   10, -30,  30,QPen(Qt::blue,3))); 
  
   Lines.append(new Line(   -10, -10, -10, 10,QPen(Qt::blue,3)));
  Lines.append(new Line(   -10, -10,  -5,-10,QPen(Qt::blue,3)));
  Lines.append(new Line(   -10,  10, -15, 10,QPen(Qt::blue,3))); 

  Lines.append(new Line( -43,  -40, -37, -40,QPen(Qt::red,3)));  // Input pins
  Lines.append(new Line( -40,  -37, -40,  -43,QPen(Qt::red,3)));
  Lines.append(new Line( -43,   40, -37,  40,QPen(Qt::black,3))); 
  
  Lines.append(new Line( 43,   -40, 37,  -40,QPen(Qt::red,3)));  // Output pins
  Lines.append(new Line( 40,   -37, 40,  -43,QPen(Qt::red,3)));
  Lines.append(new Line( 43,   40,  37,  40,QPen(Qt::black,3))); 
  
  
  Ports.append(new Port( -50,  -20));  // PIN+
  Ports.append(new Port( -50,   20));  // PIN-
  Ports.append(new Port(  50,  -10));  // POUT+
  Ports.append(new Port(  50,   10));  // POUT-

  x1 = -40; y1 = -24;
  x2 =  40; y2 =  24;

  tx = x1+12;
  ty = y2+12;

  Model = "SE_V_int";
  SpiceModel = "A";
  Name  = "A_int_SE";
  
  Props.append(new Property("A", "", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", "", false,".model line"));

}

DE_V_int::~DE_V_int()
{
}

Component* DE_V_int::newOne()
{
  return new DE_V_int();
}

Element* DE_V_int::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("A_int_DE");
  BitmapFile = (char *) "DE_V_int"; 

  if(getNewOne)  return new DE_V_int();
  return 0;
}

QString DE_V_int::netlist()
{
    return QString("");
}

QString DE_V_int::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QString P1 = Ports.at(0)->Connection->Name;
    if (P1=="gnd") P1 = "0";
    QString P2 = Ports.at(1)->Connection->Name;
    if (P2=="gnd") P2 = "0";
    QString P3 = Ports.at(2)->Connection->Name;
    if (P3=="gnd") P3 = "0";
    QString P4 = Ports.at(3)->Connection->Name;
    if (P4=="gnd") P4 = "0";
    
    s += " %vd( " + P1 + " " + P2 + " ) %vd( " + P3 + " " + P4 + " ) ";
    
    QString A= Props.at(0)->Value;
    QString A_Line_2= Props.at(1)->Value;
 
    if(  A.length()        > 0)    s += QString("%1\n").arg(A);
    if(  A_Line_2.length() > 0 )   s += QString("%1\n").arg(A_Line_2);
 
    return s;
}
