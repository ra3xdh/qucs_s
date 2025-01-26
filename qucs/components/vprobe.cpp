/***************************************************************************
                                vprobe.cpp
                               ------------
    begin                : Sat Feb 18 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "vprobe.h"
#include "node.h"


vProbe::vProbe()
{
  Description = QObject::tr("voltage probe");

  Lines.append(new qucs::Line(-20,-31, 20,-31,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,  9, 20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,-31,-20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 20,-31, 20,  9,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  Arcs.append(new qucs::Arc(-20,-23, 39, 39, 16*50, 16*80,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-11,-24, -2, -9,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-10,  0,-10,  6,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(-13,  3, -7,  3,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(  7,  3, 13,  3,QPen(Qt::black,2)));

  Lines.append(new qucs::Line(-10,  9,-10, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,  9, 10, 20,QPen(Qt::darkBlue,2)));
  Ports.append(new Port(-10, 20));
  Ports.append(new Port( 10, 20));

  x1 = -24; y1 = -35;
  x2 =  24; y2 =  20;

  tx = x2+4;
  ty = y1+4;
  Model = "VProbe";
  Name  = "Pr";
}

vProbe::~vProbe()
{
}

Component* vProbe::newOne()
{
  return new vProbe();
}

Element* vProbe::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Voltage Probe");
  BitmapFile = (char *) "vprobe";

  if(getNewOne)  return new vProbe();
  return 0;
}

QString vProbe::getProbeVariable(spicecompat::SpiceDialect)
{
    return Name;
}

QString vProbe::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    QString s = QStringLiteral("E%1 %2 0 %3 %4 1.0\nR%1%2 %2 0 1E8\nR%1%3 %3 %4 1E8\n").arg(Name).arg(Name)
            .arg(Ports.at(0)->Connection->Name).arg(Ports.at(1)->Connection->Name);
    return s;
}
