/***************************************************************************
                         SE_V_diff.cpp  -  description
                   --------------------------------------
    begin                  : Wed Oct 7 2015
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

#include "SE_V_diff.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


SE_V_diff::SE_V_diff()
{
  Description = QObject::tr("XSPICE A_diff_SE block:\nTwo line XSPICE specification. ");


  Lines.append(new Line(-60,  0,-40,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 60,  0, 40,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line( -40,   40, -40, -40,QPen(Qt::blue,3)));
  Lines.append(new Line( -40,  -40,  40, -20,QPen(Qt::blue,3)));
  Lines.append(new Line(  40,  -20,  40,  20,QPen(Qt::blue,3))); 
  Lines.append(new Line(  40,   20, -40,  40,QPen(Qt::blue,3))); 
  
  Lines.append(new Line(   -5, -15, -5,  -5,QPen(Qt::blue,3)));   // 1  d
  Lines.append(new Line(   -5, -5,  -10, -5,QPen(Qt::blue,3)));   // 2
  Lines.append(new Line(   -10,-5,  -10, -10,QPen(Qt::blue,3)));  // 3
  Lines.append(new Line(   -10,-10,  -5, -10,QPen(Qt::blue,3)));  // 4  
  
  Lines.append(new Line(   -15, 15,  15, -15,QPen(Qt::blue,3)));  // 5  /  
  
  Lines.append(new Line(   10,  5, 10,  15,QPen(Qt::blue,3)));    // 6  d
  Lines.append(new Line(   10, 15,  5,  15,QPen(Qt::blue,3)));    // 7
  Lines.append(new Line(    5, 15,  5,  10,QPen(Qt::blue,3)));    // 8
  Lines.append(new Line(   10, 10,  5,  10,QPen(Qt::blue,3)));    // 9  
  
  Lines.append(new Line(   15,  5, 15,  15,QPen(Qt::blue,3)));    // 10  t
  Lines.append(new Line(   15, 15, 18,  15,QPen(Qt::blue,3)));    // 11
  Lines.append(new Line(   12,  8, 18,   8,QPen(Qt::blue,3)));    // 12  

  Ports.append(new Port( -60,  0));  // P1
  Ports.append(new Port(  60,  0));  // P2

  x1 = -60; y1 = -44;
  x2 =  60; y2 =  44;

  tx = x1+4;
  ty = y2+4;
  Model = "SE_V_diff";
  SpiceModel = "A";
  Name  = "A_diff_SE";
  
  Props.append(new Property("A", "", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", "", false,".model line"));

}

SE_V_diff::~SE_V_diff()
{
}

Component* SE_V_diff::newOne()
{
  return new SE_V_diff();
}

Element* SE_V_diff::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("A_diff_SE");
  BitmapFile = (char *) "SE_V_diff"; 

  if(getNewOne)  return new SE_V_diff();
  return 0;
}

QString SE_V_diff::netlist()
{
    return QString("");
}

QString SE_V_diff::spice_netlist(bool)
{
  
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QString P1 = Ports.at(0)->Connection->Name;
    if (P1=="gnd") P1 = "0";
    QString P2 = Ports.at(1)->Connection->Name;
    if (P2=="gnd") P2 = "0";
    s += " %v( " + P1 +" ) %v( " + P2 + " ) ";
    
 
 
    QString A= Props.at(0)->Value;
    QString A_Line_2= Props.at(1)->Value;
 
    if(  A.length()        > 0)    s += QString("%1\n").arg(A);
    if(  A_Line_2.length() > 0 )   s += QString("%1\n").arg(A_Line_2);
 
    return s;
}
