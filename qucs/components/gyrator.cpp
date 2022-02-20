/***************************************************************************
                          gyrator.cpp  -  description
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

#include "gyrator.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


Gyrator::Gyrator()
{
  Description = QObject::tr("gyrator (impedance inverter)");

  Arcs.append(new qucs::Arc(  3, -9, 18, 18, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-21, -9, 18, 18,16*270, 16*180,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-30,-30,-12,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 30,-12, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 12,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 12, 30, 30, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( 12,-30, 12, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12,-30,-12, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-22,-22, 22,-22,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 22,-22, 22, 22,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 22, 22,-22, 22,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line(-22, 22,-22,-22,QPen(Qt::darkGray,1)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "Gyrator";
  Name  = "X";
  SpiceModel = "*";

  Props.append(new Property("R", "50 Ohm", true,
		QObject::tr("gyrator ratio")));
  Props.append(new Property("Zref", "50 Ohm", false,
		QObject::tr("reference impedance")));
}

Gyrator::~Gyrator()
{
}

Component* Gyrator::newOne()
{
  return new Gyrator();
}

Element* Gyrator::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Gyrator");
  BitmapFile = (char *) "gyrator";

  if(getNewOne)  return new Gyrator();
  return 0;
}

QString Gyrator::spice_netlist(bool)
{
    QString s;
    QString R = spicecompat::normalize_value(Props.at(0)->Value);
    QString n1 = Ports.at(0)->Connection->Name;
    if (n1=="gnd") n1="0";
    QString n2 = Ports.at(1)->Connection->Name;
    if (n2=="gnd") n2="0";
    QString n3 = Ports.at(2)->Connection->Name;
    if (n3=="gnd") n3="0";
    QString n4 = Ports.at(3)->Connection->Name;
    if (n4=="gnd") n4="0";
    s +=  QString("B%1_1 %2 %3 I=(1/(%4))*(V(%5)-V(%6))\n").arg(Name).arg(n1).arg(n4).arg(R).arg(n2).arg(n3);
    s +=  QString("B%1_2 %2 %3 I=-1.0*(1/(%4))*(V(%5)-V(%6))\n").arg(Name).arg(n2).arg(n3).arg(R).arg(n1).arg(n4);
    return s;
}
