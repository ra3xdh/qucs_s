/***************************************************************************
                         NMF MESFET_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Sun. 19 April 2015
    copyright              : (C) 2015 by Mike Brinson
    email                    : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "MESFET_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


MESFET_SPICE::MESFET_SPICE()
{
    Description = QObject::tr("NMF MESFET SPICE format");

  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::red,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::red,3)));
  Lines.append(new Line(-10,-10,  0,-10,QPen(Qt::red,3)));
  Lines.append(new Line(  0,-10,  0,-20,QPen(Qt::red,3)));
  Lines.append(new Line(  0,-20,  0,-30,QPen(Qt::darkBlue,3))); 

  Lines.append(new Line(-10, 10,  0, 10,QPen(Qt::red,3)));
  Lines.append(new Line(  0, 10,  0, 20,QPen(Qt::red,3)));
  Lines.append(new Line(  0, 20,  0, 30,QPen(Qt::darkBlue,3)));  

// N
  Lines.append(new Line( 10, 30,  10, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 10, 20,  20, 30,QPen(Qt::red,2)));
  Lines.append(new Line( 20, 30,  20, 20,QPen(Qt::red,2)));  
//M
  Lines.append(new Line( 25, 30,  25, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 25, 20,  30, 25,QPen(Qt::red,2)));  
  Lines.append(new Line( 30, 25,  35, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 35, 20,  35, 30,QPen(Qt::red,2))); 
//F
  Lines.append(new Line( 40, 30,  40, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 40, 20,  45, 20,QPen(Qt::red,2)));
  Lines.append(new Line( 40, 25,  45, 25,QPen(Qt::red,2)));  
     
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "MESFET_SPICE";
    SpiceModel = "Z";
    Name  = "Z";

    Props.append(new Property("Z", "", true,"Expression"));
    Props.append(new Property("Z_Line 2", "", false,"Expression"));
    Props.append(new Property("Z_Line 3", "", false,"Expression"));
    Props.append(new Property("Z _Line 4", "", false,"Expression"));
    Props.append(new Property("Z _Line 5", "", false,"Expression"));

}

MESFET_SPICE::~MESFET_SPICE()
{
}

Component* MESFET_SPICE::newOne()
{
  return new MESFET_SPICE();
}

Element* MESFET_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("NMF MESFET");
  BitmapFile = (char *) "MESFET_SPICE";

  if(getNewOne)  return new MESFET_SPICE();
  return 0;
}

QString MESFET_SPICE::netlist()
{
    return QString("");
}

QString MESFET_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString Z= Props.at(0)->Value;
    QString Z_Line_2= Props.at(1)->Value;
    QString Z_Line_3= Props.at(2)->Value;
    QString Z_Line_4= Props.at(3)->Value;
    QString Z_Line_5= Props.at(4)->Value;

     if(  Z.length()  > 0)                 s += QString("%1\n").arg(Z);
    if(  Z_Line_2.length() > 0 )   s += QString("%1\n").arg(Z_Line_2);
    if(  Z_Line_3.length() > 0 )   s += QString("%1\n").arg(Z_Line_3);
    if(  Z_Line_4.length() > 0 )   s += QString("%1\n").arg(Z_Line_4);
    if( Z_Line_5.length() >  0 )   s += QString("%1\n").arg(Z_Line_5);
 
    return s;
}
