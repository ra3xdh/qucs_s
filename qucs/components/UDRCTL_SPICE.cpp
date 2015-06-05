/***************************************************************************
                         UDRCTL_SPICE.cpp  -  description
                   --------------------------------------
    begin                  : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                  : gn.kraut@t-online.de
    spice4qucs code added  : Fri. 15 May 2015
    copyright              : (C) 2015 by Mike Brinson
    email                  : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "UDRCTL_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


UDRCTL_SPICE::UDRCTL_SPICE()
{
  Description = QObject::tr("Uniform Distributed RC transmission Line");

   
  Lines.append(new Line(  -60,  0, -45,   0,QPen(Qt::darkBlue,2))); 
  Lines.append(new Line(  -45, 20,  45,  20,QPen(Qt::cyan,4)));
  Lines.append(new Line(   60,  0,  45,   0,QPen(Qt::darkBlue,2)));  
  Lines.append(new Line(   0,  20,   0,  40,QPen(Qt::darkBlue,2))); 
  
  Lines.append(new Line(-45,    0,  -37,  0,QPen(Qt::cyan,4)));
  Lines.append(new Line(-37,    0,  -30, -10,QPen(Qt::cyan,4)));
  Lines.append(new Line(-30,  -10,  -15,  10,QPen(Qt::cyan,4)));
  Lines.append(new Line(-15,   10,   0,  -10,QPen(Qt::cyan,4)));
  Lines.append(new Line(  0,  -10,   15,  10,QPen(Qt::cyan,4)));
  Lines.append(new Line(  15,  10,   30, -10,QPen(Qt::cyan,4))); 
  Lines.append(new Line(  30, -10,  37,   0,QPen(Qt::cyan,4))); 
  Lines.append(new Line(  37,   0,  45,   0,QPen(Qt::cyan,4)));   



    
  Ports.append(new Port(  -60, 0)); // P1
  Ports.append(new Port(   60, 0)); // P2
  Ports.append(new Port(    0, 40)); // P3

  x1 = -60; y1 = -20;
  x2 =  60; y2 =  40;

    tx = x1+4;
    ty = y2+4;

    Model = "UDRCTL_SPICE";
    SpiceModel = "U";
    Name  = "U";

    Props.append(new Property("U",        "", true,"Expression"));
    Props.append(new Property("U_Line 2", "", false,"Expression"));
    Props.append(new Property("U_Line 3", "", false,"Expression"));
    Props.append(new Property("U_Line 4", "", false,"Expression"));
    Props.append(new Property("U_Line 5", "", false,"Expression"));

//    rotate();  // fix historical flaw
}

UDRCTL_SPICE::~UDRCTL_SPICE()
{
}

Component* UDRCTL_SPICE::newOne()
{
  return new UDRCTL_SPICE();
}

Element* UDRCTL_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("UDRCTL");
  BitmapFile = (char *) "UDRCTL_SPICE";

  if(getNewOne)  return new UDRCTL_SPICE();
  return 0;
}

QString UDRCTL_SPICE::netlist()
{
    return QString("");
}

QString UDRCTL_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString U= Props.at(0)->Value;
    QString U_Line_2= Props.at(1)->Value;
    QString U_Line_3= Props.at(2)->Value;
    QString U_Line_4= Props.at(3)->Value;
    QString U_Line_5= Props.at(4)->Value;

    if(  U.length()  > 0)          s += QString("%1\n").arg(U);
    if(  U_Line_2.length() > 0 )   s += QString("%1\n").arg(U_Line_2);
    if(  U_Line_3.length() > 0 )   s += QString("%1\n").arg(U_Line_3);
    if(  U_Line_4.length() > 0 )   s += QString("%1\n").arg(U_Line_4);
    if(  U_Line_5.length() >  0 )  s += QString("%1\n").arg(U_Line_5);
 
    return s;
}
