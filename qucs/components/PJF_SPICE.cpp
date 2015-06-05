/***************************************************************************
                         PJF_SPICE.cpp  -  description
                   --------------------------------------
    begin                     : Fri Mar 9 2007
    copyright                 : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Fri. 29 May 2015
    copyright                 : (C) 2015 by Mike Brinson
    email                     : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "PJF_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


PJF_SPICE::PJF_SPICE()
{
    Description = QObject::tr("PJF JFT SPICE format");
  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::red,3)));
  
  Lines.append(new Line(-30,  0,-20,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-20,  0,-10,  0,QPen(Qt::red,3)));  
  
  Lines.append(new Line(-10,-10,  0,-10,QPen(Qt::red,3)));
  Lines.append(new Line(  0,-10,  0,-20,QPen(Qt::red,3)));
  Lines.append(new Line(  0,-20,  0,-30,QPen(Qt::darkBlue,3))); 
  
  Lines.append(new Line(-10, 10,  0, 10,QPen(Qt::red,3)));
  Lines.append(new Line(  0, 10,  0, 20,QPen(Qt::red,3)));
  Lines.append(new Line(  0, 20,  0, 30,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line(-16, -5,-11,  0,QPen(Qt::red,3)));
  Lines.append(new Line(-16,  5,-11,  0,QPen(Qt::red,3)));
  
// P
  Lines.append(new Line( 10, 30,  10, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 10, 20,  20, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 20, 20,  20, 25,QPen(Qt::red,2))); 
  Lines.append(new Line( 20, 25,  10, 25,QPen(Qt::red,2)));    
//J
  Lines.append(new Line( 25, 20,  35, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 30, 20,  30, 30,QPen(Qt::red,2)));  
  Lines.append(new Line( 30, 30,  25, 30,QPen(Qt::red,2)));

//F
  Lines.append(new Line( 40, 30,  40, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 40, 20,  45, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 40, 25,  45, 25,QPen(Qt::red,2)));  



  Ports.append(new Port(  0,-30)); //D
  Ports.append(new Port(-30,  0)); //G
  Ports.append(new Port(  0, 30)); //S

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "PJF_SPICE";
    SpiceModel = "J";
    Name  = "J";

    Props.append(new Property("J", "", true,"Expression"));
    Props.append(new Property("J_Line 2", "", false,"Expression"));
    Props.append(new Property("J_Line 3", "", false,"Expression"));
    Props.append(new Property("J _Line 4", "", false,"Expression"));
    Props.append(new Property("J _Line 5", "", false,"Expression"));

}

PJF_SPICE::~PJF_SPICE()
{
}

Component* PJF_SPICE::newOne()
{
  return new PJF_SPICE();
}

Element* PJF_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(" PJF FET");
  BitmapFile = (char *) "PJF_SPICE";

  if(getNewOne)  return new PJF_SPICE();
  return 0;
}

QString PJF_SPICE::netlist()
{
    return QString("");
}

QString PJF_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString J= Props.at(0)->Value;
    QString J_Line_2= Props.at(1)->Value;
    QString J_Line_3= Props.at(2)->Value;
    QString J_Line_4= Props.at(3)->Value;
    QString J_Line_5= Props.at(4)->Value;

    if(  J.length()  > 0)          s += QString("%1\n").arg(J);
    if(  J_Line_2.length() > 0 )   s += QString("%1\n").arg(J_Line_2);
    if(  J_Line_3.length() > 0 )   s += QString("%1\n").arg(J_Line_3);
    if(  J_Line_4.length() > 0 )   s += QString("%1\n").arg(J_Line_4);
    if(  J_Line_5.length() > 0 )   s += QString("%1\n").arg(J_Line_5);
 
    return s;
}
