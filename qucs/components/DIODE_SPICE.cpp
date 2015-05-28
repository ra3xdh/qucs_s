/***************************************************************************
                         DIODE_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Wed. 27 May 2015
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
#include "DIODE_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


DIODE_SPICE::DIODE_SPICE()
{
    Description = QObject::tr("Diode SPICE format");


    Lines.append(new Line(-30,  0, -20,  0,QPen(Qt::darkBlue,3)));
    Lines.append(new Line( -20, 0, -6,   0,QPen(Qt::red,3)));
    Lines.append(new Line(  6,  0, 20,   0,QPen(Qt::red,3)));
    Lines.append(new Line( 20,  0, 30,   0,QPen(Qt::darkBlue,3)));    
 
 
    Lines.append(new Line( -6, -9, -6,  9,QPen(Qt::red,3)));
    Lines.append(new Line(  6, -9,  6,  9,QPen(Qt::red,3)));
    Lines.append(new Line( -6,  0,  6, -9,QPen(Qt::red,3)));
    Lines.append(new Line( -6,  0,  6,  9,QPen(Qt::red,3)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;

    tx = x1+4;
    ty = y2+4;

    Model = "DIODE_SPICE";
    SpiceModel = "D";
    Name  = "D";

    Props.append(new Property("D", "", true,"Expression"));
    Props.append(new Property("D_Line 2", "", false,"Expression"));
    Props.append(new Property("D_Line 3", "", false,"Expression"));
    Props.append(new Property("D _Line 4", "", false,"Expression"));
    Props.append(new Property("D _Line 5", "", false,"Expression"));

}

DIODE_SPICE::~DIODE_SPICE()
{
}

Component* DIODE_SPICE::newOne()
{
  return new DIODE_SPICE();
}

Element* DIODE_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(" Diode");
  BitmapFile = (char *) "DIODE_SPICE";

  if(getNewOne)  return new DIODE_SPICE();
  return 0;
}

QString DIODE_SPICE::netlist()
{
    return QString("");
}

QString DIODE_SPICE::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString D= Props.at(0)->Value;
    QString D_Line_2= Props.at(1)->Value;
    QString D_Line_3= Props.at(2)->Value;
    QString D_Line_4= Props.at(3)->Value;
    QString D_Line_5= Props.at(4)->Value;

    if(  D.length()  > 0)          s += QString("%1\n").arg(D);
    if(  D_Line_2.length() > 0 )   s += QString("%1\n").arg(D_Line_2);
    if(  D_Line_3.length() > 0 )   s += QString("%1\n").arg(D_Line_3);
    if(  D_Line_4.length() > 0 )   s += QString("%1\n").arg(D_Line_4);
    if(  D_Line_5.length() > 0 )   s += QString("%1\n").arg(D_Line_5);
 
    return s;
}
