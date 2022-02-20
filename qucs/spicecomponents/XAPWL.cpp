/***************************************************************************
                         XAPWL.cpp  -  description
                   --------------------------------------
    begin                  : Thurs April 21 2016
    copyright              : (C) by Mike Brinson (mbrin72043@yahoo.co.uk),


 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "XAPWL.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


XAPWL::XAPWL()
{
  Description = QObject::tr("PWL controlled voltage source:\nSeven line XSPICE specification. ");


  Lines.append(new qucs::Line(-80,  0,-70,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 80,  0, 70,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line( -70,   35, -70, -35,QPen(Qt::blue,3)));
  Lines.append(new qucs::Line( -70,  -35,  70, -35,QPen(Qt::blue,3)));
  Lines.append(new qucs::Line(  70,  -35,  70,  35,QPen(Qt::blue,3))); 
  Lines.append(new qucs::Line(  70,   35, -70,  35,QPen(Qt::blue,3))); 
  
  Texts.append(new Text(  -60, -25," PWL controlled ",Qt::blue,12.0,1.0,0));
  Texts.append(new Text(  -60,   5," voltage source ",Qt::blue,12.0,1.0,0));
  
  Ports.append(new Port( -80,  0));  // PIn
  Ports.append(new Port(  80,  0));   // POut

  x1 = -70; y1 = -44;
  x2 =  70; y2 =  44;

  tx = x1+4;
  ty = y2+4;
  Model = "XAPWL";
  SpiceModel = "A";
  Name  = "XAPWL";
  
  Props.append(new Property("A", " A_PWLmod", true,"Parameter list and\n .model spec."));
  Props.append(new Property("A_Line 2", ".MODEL A_PWLmod pwl ( ", false,".model line"));
  Props.append(new Property("A_Line 3", "+ x_array=[-2.0 -1.0 2.0 4.0 5.0]", false,".model line"));
  Props.append(new Property("A_Line 4", "+ y_array=[-2.0 -0.2 0.1 2.0 10.0]", false,".model line"));
  Props.append(new Property("A_Line 5", "+ input_domain=0.05", false,".model line"));
  Props.append(new Property("A_Line 6", "+ fraction=TRUE )", false,".model line")); 
  Props.append(new Property("A_Line 7", "", false,".model line"));   

}

XAPWL::~XAPWL()
{
}

Component* XAPWL::newOne()
{
  return new XAPWL();
}

Element* XAPWL::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("XAPWL");
  BitmapFile = (char *) "XAPWL"; 

  if(getNewOne)  return new XAPWL();
  return 0;
}

QString XAPWL::netlist()
{
    return QString("");
}

QString XAPWL::spice_netlist(bool)
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
     
    if(  A.length()        > 0)    s += QString("%1\n").arg(A);
    if(  A_Line_2.length() > 0 )   s += QString("%1\n").arg(A_Line_2);
    if(  A_Line_3.length() > 0 )   s += QString("%1\n").arg(A_Line_3);   
    if(  A_Line_4.length() > 0 )   s += QString("%1\n").arg(A_Line_4); 
    if(  A_Line_5.length() > 0 )   s += QString("%1\n").arg(A_Line_5);
    if(  A_Line_6.length() > 0 )   s += QString("%1\n").arg(A_Line_6);
    if(  A_Line_7.length() > 0 )   s += QString("%1\n").arg(A_Line_7);   
    return s;
}
