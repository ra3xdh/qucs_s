/***************************************************************************
                         Cmeter_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Wed. 22 April 2015
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
#include "Cmeter_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


Cmeter_SPICE::Cmeter_SPICE()
{
    Description = QObject::tr("XSPICE A(cmeter):");
 
  Lines.append(new Line(-30,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  0, -4,  0,QPen(Qt::blue,3)));
  Lines.append(new Line(4,  0, 20,  0,QPen(Qt::blue,3)));
  Lines.append(new Line(  -4, 4, -4, -4, QPen(Qt::blue,3)));
  Lines.append(new Line(  4,  4,  4, -4, QPen(Qt::blue,3)));
 
  Lines.append(new Line(-20,-31, 20,-31,QPen(Qt::blue,3)));
  Lines.append(new Line(-20,  9, 20,  9,QPen(Qt::blue,3)));
  Lines.append(new Line(-20,-31,-20,  9,QPen(Qt::blue,3)));
  Lines.append(new Line( 20,-31, 20,  9,QPen(Qt::blue,3)));

  Lines.append(new Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  Arcs.append(new Arc(-20,-23, 39, 39, 16*50, 16*80,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-11,-24, -2, -9,QPen(Qt::red,2)));
  

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -34;
  x2 =  40; y2 =  12;

  tx = x1+4;
  ty = y2+4;
 
    Model = "Cmeter_SPICE";
    SpiceModel = "A";
    Name  = "Cmeter";

    Props.append(new Property("A ", "", true,"Netlist specification."));
    Props.append(new Property("A_Line 2 ", "", false,"Model specification."));
  
  //  rotate();  // fix historical flaw
}

Cmeter_SPICE::~Cmeter_SPICE()
{
}

Component* Cmeter_SPICE::newOne()
{
  return new Cmeter_SPICE();
}

Element* Cmeter_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("A(cmeter)");
  BitmapFile = (char *) "Cmeter_SPICE";

  if(getNewOne)  return new Cmeter_SPICE();
  return 0;
}

QString Cmeter_SPICE::netlist()
{
    return QString("");
}

QString Cmeter_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }

    QString Cmeter= Props.at(0)->Value;
    QString Cmeter_Line_2= Props.at(1)->Value;
 
    if(  Cmeter.length()        > 0 )   s += QString("%1\n").arg(Cmeter);
    if(  Cmeter_Line_2.length() > 0 )   s += QString("%1\n").arg(Cmeter_Line_2);
 
    return s;
}
