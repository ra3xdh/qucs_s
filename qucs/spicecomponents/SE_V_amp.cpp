/***************************************************************************
                         SE_V_amp.cpp  -  description
                   --------------------------------------
    begin                  : Fri Mar 27 2015
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

#include "SE_V_amp.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


SE_V_amp::SE_V_amp()
{
  Description = QObject::tr("XSPICE A_gain_SE block:\nTwo line XSPICE specification. ");

 // Texts.append(new Text( 15,-10,"A",Qt::blue,16.0,0.0,1.0));
  Lines.append(new Line(-40,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 40,  0, 20,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line( -20,   20, -20, -20,QPen(Qt::blue,3)));
  Lines.append(new Line( -20,  -20,  20, -10,QPen(Qt::blue,3)));
  Lines.append(new Line(  20,  -10,  20,  10,QPen(Qt::blue,3))); 
  Lines.append(new Line(  20,   10, -20,  20,QPen(Qt::blue,3))); 
  
  Lines.append(new Line( -5,   5, -5, -5,QPen(Qt::blue,3)));
  Lines.append(new Line( -5,  -5,  5, -5,QPen(Qt::blue,3)));
  Lines.append(new Line(  5,  -5,  5,  5,QPen(Qt::blue,3))); 
  Lines.append(new Line(  -5,  0,  5,  0,QPen(Qt::blue,3))); 
  
  Ports.append(new Port( -40,  0));  // P1
  Ports.append(new Port(  40,  0));  // P2

  x1 = -40; y1 = -24;
  x2 =  40; y2 =  24;

  tx = x1+4;
  ty = y2+4;
  Model = "SE_V_amp";
  SpiceModel = "A";
  Name  = "A_gain_SE";
  
  Props.append(new Property("A", "", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", "", false,".model line"));

  //rotate();  // fix historical flaw
}

SE_V_amp::~SE_V_amp()
{
}

Component* SE_V_amp::newOne()
{
  return new SE_V_amp();
}

Element* SE_V_amp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("A_gain_SE");
  BitmapFile = (char *) "SE_V_amp"; 

  if(getNewOne)  return new SE_V_amp();
  return 0;
}

QString SE_V_amp::netlist()
{
    return QString("");
}

QString SE_V_amp::spice_netlist(bool)
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
