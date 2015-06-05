/***************************************************************************
                         NPN_SPICE.cpp  -  description
                   --------------------------------------
    begin                     : Fri Mar 9 2007
    copyright                 : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Thur. 28 May 2015
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
#include "NPN_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


NPN_SPICE::NPN_SPICE()
{
    Description = QObject::tr("NPN BJT SPICE format");

  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::red,3)));
  
  Lines.append(new Line(-30,  0,-20,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-20,  0,-10,  0,QPen(Qt::red,3)));
  
  Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::red,3)));
  
  Lines.append(new Line(  0,-15,  0,-20,QPen(Qt::red,3)));
  Lines.append(new Line(  0,-20,  0,-30,QPen(Qt::darkBlue,3)));
    
  Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::red,3)));
  
  Lines.append(new Line(  0, 15,  0, 20,QPen(Qt::red,3)));
  Lines.append(new Line(  0, 20,  0, 30,QPen(Qt::darkBlue,3)));

  Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::red,3)));
  Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::red,3)));
 

  Ports.append(new Port(0, -30));
  Ports.append(new Port(-30, 0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "NPN_SPICE";
    SpiceModel = "Q";
    Name  = "Q";

    Props.append(new Property("Q", "", true,"Expression"));
    Props.append(new Property("Q_Line 2", "", false,"Expression"));
    Props.append(new Property("Q_Line 3", "", false,"Expression"));
    Props.append(new Property("Q _Line 4", "", false,"Expression"));
    Props.append(new Property("Q _Line 5", "", false,"Expression"));

}

NPN_SPICE::~NPN_SPICE()
{
}

Component* NPN_SPICE::newOne()
{
  return new NPN_SPICE();
}

Element* NPN_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(" NPN BJT");
  BitmapFile = (char *) "NPN_SPICE";

  if(getNewOne)  return new NPN_SPICE();
  return 0;
}

QString NPN_SPICE::netlist()
{
    return QString("");
}

QString NPN_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString Q= Props.at(0)->Value;
    QString Q_Line_2= Props.at(1)->Value;
    QString Q_Line_3= Props.at(2)->Value;
    QString Q_Line_4= Props.at(3)->Value;
    QString Q_Line_5= Props.at(4)->Value;

    if(  Q.length()  > 0)          s += QString("%1\n").arg(Q);
    if(  Q_Line_2.length() > 0 )   s += QString("%1\n").arg(Q_Line_2);
    if(  Q_Line_3.length() > 0 )   s += QString("%1\n").arg(Q_Line_3);
    if(  Q_Line_4.length() > 0 )   s += QString("%1\n").arg(Q_Line_4);
    if(  Q_Line_5.length() > 0 )   s += QString("%1\n").arg(Q_Line_5);
 
    return s;
}
