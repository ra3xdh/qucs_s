/***************************************************************************
                         SDTF.cpp  -  description
                   --------------------------------------
    begin                  : Mon Nov 30 2015
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

#include "SDTF.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


SDTF::SDTF()
{
  Description = QObject::tr("S domain transfer function block:\nSeven line XSPICE specification. ");
  Simulator = spicecompat::simSpice;


  Lines.append(new qucs::Line(-80,  0,-70,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 80,  0, 70,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line( -70,   35, -70, -35,QPen(Qt::blue,3)));
  Lines.append(new qucs::Line( -70,  -35,  70, -35,QPen(Qt::blue,3)));
  Lines.append(new qucs::Line(  70,  -35,  70,  35,QPen(Qt::blue,3))); 
  Lines.append(new qucs::Line(  70,   35, -70,  35,QPen(Qt::blue,3))); 
  
  Texts.append(new Text(  -60, -25,"    S  Domain    ",Qt::blue,12.0,1.0,0));
  Texts.append(new Text(  -60,   5,"Transfer Function",Qt::blue,12.0,1.0,0));
  
  Ports.append(new Port( -80,  0));  // PIn
  Ports.append(new Port(  80,  0));   // POut

  x1 = -70; y1 = -44;
  x2 =  70; y2 =  44;

  tx = x1+4;
  ty = y2+4;
  Model = "SDTF";
  SpiceModel = "A";
  Name  = "SDTF";
  
  Props.append(new Property("A", " A_XSDFTmod", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", ".MODEL A_XSDFTmod s_xfer ( gain=1.0", false,".model line"));
  Props.append(new Property("A_Line 3", "+ num_coeff=[1.0]", false,".model line"));
  Props.append(new Property("A_Line 4", "+ den_coeff=[1.0 1.09775 1.0]", false,".model line"));
  Props.append(new Property("A_Line 5", "+ int_ic=[0 0 0]", false,".model line"));
  Props.append(new Property("A_Line 6", "+ denormalized_freq=1500.0 )", false,".model line")); 
  Props.append(new Property("A_Line 7", "", false,".model line"));   

}

SDTF::~SDTF()
{
}

Component* SDTF::newOne()
{
  return new SDTF();
}

Element* SDTF::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("SDTF");
  BitmapFile = (char *) "SDTF"; 

  if(getNewOne)  return new SDTF();
  return 0;
}

QString SDTF::netlist()
{
    return QString("");
}

QString SDTF::spice_netlist(bool)
{
  
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QString P1 = Ports.at(0)->Connection->Name;
    if (P1=="gnd") P1 = "0";
    QString P2 = Ports.at(1)->Connection->Name;
    if (P2=="gnd") P2 = "0";
    s += " " + P1 + " " + P2 + " ";
    
    QString A = Props.at(0)->Value;
    QString A_Line_2 = Props.at(1)->Value;
    QString A_Line_3 = Props.at(2)->Value;    
    QString A_Line_4 = Props.at(3)->Value;    
    QString A_Line_5 = Props.at(4)->Value;
    QString A_Line_6 = Props.at(5)->Value;
    QString A_Line_7 = Props.at(6)->Value;   
     
    if(  A.length()        > 0)    s += QString("%1").arg(A);
    if(  A_Line_2.length() > 0 )   s += QString("\n%1").arg(A_Line_2);
    if(  A_Line_3.length() > 0 )   s += QString("\n%1").arg(A_Line_3);
    if(  A_Line_4.length() > 0 )   s += QString("\n%1").arg(A_Line_4);
    if(  A_Line_5.length() > 0 )   s += QString("\n%1").arg(A_Line_5);
    if(  A_Line_6.length() > 0 )   s += QString("\n%1").arg(A_Line_6);
    if(  A_Line_7.length() > 0 )   s += QString("\n%1").arg(A_Line_7);
    s += "\n";
    return s;
}
