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

  Lines.append(new Line(-40, -10,-25, -10,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line(-25, -18, 25, -18,QPen(Qt::cyan,3)));
  Lines.append(new Line( 25, -18, 25,  -2,QPen(Qt::cyan,3)));
  Lines.append(new Line( 25,  -2,-25,  -2,QPen(Qt::cyan,3)));
  Lines.append(new Line(-25,  -2,-25, -18,QPen(Qt::cyan,3)));
  
  Lines.append(new Line( 25, -10, 35, -10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,  -2,  0,  10,QPen(Qt::darkBlue,2))); 
  
  //Sows series R
  Lines.append(new Line( -25, -10, -20, -10,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line( -20, -12, -10, -12,QPen(Qt::darkBlue,2))); 
  Lines.append(new Line( -10, -12, -10, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -10, -8,  -20, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -20, -8,  -20, -12,QPen(Qt::darkBlue,2))); 

  Lines.append(new Line( -10, -10, -5, -10,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line( -5 , -12,   5, -12,QPen(Qt::darkBlue,2))); 
  Lines.append(new Line(  5,  -12,   5, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  5,   -8,  -5, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  -8,   -5, -12,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line(   5, -10,  10, -10,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line(  10, -12,  20, -12,QPen(Qt::darkBlue,2))); 
  Lines.append(new Line(  20, -12,  20, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  20, -8,   10, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  10, -8,   10, -12,QPen(Qt::darkBlue,2))); 
  
  Lines.append(new Line(   40, -10,  25, -10,QPen(Qt::darkBlue,2))); 


    
  Ports.append(new Port(  -40,-10)); // P1
  Ports.append(new Port(   40,-10)); // P2
  Ports.append(new Port(    0, 10)); // P3

  x1 = -40; y1 = -20;
  x2 =  40; y2 =  15;

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

QString UDRCTL_SPICE::spice_netlist(bool isXyce)
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
