/***************************************************************************
                          inductor.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inductor.h"
#include "node.h"
#include "schematic.h"
#include "main.h"


Inductor::Inductor()
{
  Description = QObject::tr("inductor");

  Arcs.append(new Arc(-18, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( -6, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc(  6, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -10;
  x2 =  30; y2 =   6;

  tx = x1+4;
  ty = y2+4;
  Model = "L";
  Name  = "L";
  SpiceModel = "L";

  Props.append(new Property("L", "1 nH", true,
		QObject::tr("inductance in Henry")));
  Props.append(new Property("I", "", false,
		QObject::tr("initial current for transient simulation")));
}

Inductor::~Inductor()
{
}

Component* Inductor::newOne()
{
  return new Inductor();
}


QString Inductor::spice_netlist()
{
    QString s=check_spice_refdes();

    // output all node names
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

    QString val = Props.at(0)->Value;
    double Ind,fac;
    QString unit;
    str2num(val,Ind,unit,fac);
    Ind *=fac;
    s += " "+QString::number(Ind) + "\n";

    return s;
}

Element* Inductor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Inductor");
  BitmapFile = (char *) "inductor";  // bitmap file name without suffix

  if(getNewOne)  return new Inductor();
  return 0;
}
