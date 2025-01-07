/***************************************************************************
                          dcblock.cpp  -  description
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

#include "dcblock.h"
#include "extsimkernels/spicecompat.h"
#include "node.h"

dcBlock::dcBlock()
{
  Description = QObject::tr("dc block");
  Simulator = spicecompat::simAll;

  Lines.append(new qucs::Line(- 4,-11, -4, 11,QPen(Qt::darkBlue,4)));
  Lines.append(new qucs::Line(  4,-11,  4, 11,QPen(Qt::darkBlue,4)));
  Lines.append(new qucs::Line(-30,  0, -4,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  4,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-23,-14, 23,-14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-23, 14, 23, 14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-23,-14,-23, 14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 23,-14, 23, 14,QPen(Qt::darkBlue,1)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -16;
  x2 =  30; y2 =  17;

  tx = x1+4;
  ty = y2+4;
  Model = "DCBlock";
  Name  = "C";
  SpiceModel = "C";

  Props.append(new Property("C", "1 uF", false,
    QObject::tr("for transient simulation: capacitance in Farad")));
}

dcBlock::~dcBlock()
{
}

Component* dcBlock::newOne()
{
  return new dcBlock();
}

Element* dcBlock::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Block");
  BitmapFile = (char *) "dcblock";

  if(getNewOne)  return new dcBlock();
  return 0;
}

QString dcBlock::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
  Q_UNUSED(dialect);

  QString p1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
  QString p2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
  QString val = spicecompat::normalize_value(getProperty("C")->Value);
  QString s;
  QString name = spicecompat::check_refdes(Name, SpiceModel);
  s = QStringLiteral("%1 %2 %3 %4\n").arg(name, p1, p2, val);
  return s;
}

QString dcBlock::cdl_netlist()
{
    return spice_netlist(spicecompat::CDL);
}
