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
#include "misc.h"
#include "extsimkernels/spicecompat.h"


Inductor::Inductor()
{
  Description = QObject::tr("inductor");

  Arcs.append(new Arc(-18, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,4)));
  Arcs.append(new Arc( -6, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,4)));
  Arcs.append(new Arc(  6, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,4)));
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


QString Inductor::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);

    s += QString(" %1 %2 ").arg(Ports.at(0)->Connection->Name)
            .arg(Ports.at(1)->Connection->Name); // output source nodes
    s.replace(" gnd ", " 0 ");

    s += " "+spicecompat::normalize_value(Props.at(0)->Value) + " ";
    QString val = Props.at(1)->Value; // add inial volatge if presents
    val.remove(' ').toUpper();
    if (!val.isEmpty()) {
        s += " IC=" + val;
    }
    s += '\n';

    return s;
}

Element* Inductor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Inductor");
  BitmapFile = (char *) "inductor";  // bitmap file name without suffix

  if(getNewOne)  return new Inductor();
  return 0;
}
