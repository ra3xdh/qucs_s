/***************************************************************************
                          capacitor.cpp  -  description
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

#include "capacitor.h"
#include "node.h"
#include "schematic.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


Capacitor::Capacitor()
{
  Description = QObject::tr("capacitor");

  Props.append(new Property("C", "1 pF", true,
		QObject::tr("capacitance in Farad")));
  Props.append(new Property("V", "", false,
		QObject::tr("initial voltage for transient simulation")));
  Props.append(new Property("Symbol", "neutral", false,
	QObject::tr("schematic symbol")+" [neutral, polar]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "C";
  SpiceModel = "C";
  Name  = "C";
}

Component* Capacitor::newOne()
{
  return new Capacitor();
}

Element* Capacitor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Capacitor");
  BitmapFile = (char *) "capacitor";

  if(getNewOne)  return new Capacitor();
  return 0;
}

QString Capacitor::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);

    s += QString(" %1 %2 ").arg(Ports.at(0)->Connection->Name)
            .arg(Ports.at(1)->Connection->Name); // output  nodes
    s.replace(" gnd ", " 0 ");

    s += " "+spicecompat::normalize_value(Props.at(0)->Value) + " ";
    QString val = Props.at(1)->Value; // add inial volatge if presents
    val.remove(' ').toUpper();
    if (!val.isEmpty()) {
        s += " IC=" + val;
    }

    return s+'\n';
}

void Capacitor::createSymbol()
{
  if(Props.getLast()->Value.at(0) == 'n') {
    Lines.append(new Line( -4,-11, -4, 11,QPen(Qt::darkBlue,4)));
    Lines.append(new Line(  4,-11,  4, 11,QPen(Qt::darkBlue,4)));
  }
  else {
    Lines.append(new Line(-11, -5,-11,-11,QPen(Qt::red,1)));
    Lines.append(new Line(-14, -8, -8, -8,QPen(Qt::red,1)));
    Lines.append(new Line( -4,-11, -4, 11,QPen(Qt::darkBlue,3)));
    Arcs.append(new Arc(4,-12, 20, 24, 16*122, 16*116,QPen(Qt::darkBlue,3)));
  }

  Lines.append(new Line(-30,  0, -4,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  4,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;
}
