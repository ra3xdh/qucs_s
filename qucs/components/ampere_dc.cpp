/***************************************************************************
                               ampere_dc.cpp
                              ---------------
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

#include "ampere_dc.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


Ampere_dc::Ampere_dc()
{
  Description = QObject::tr("ideal dc current source");

  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Idc";
  Name  = "I";
  SpiceModel = "I";

  Props.append(new Property("I", "1 mA", true,
		QObject::tr("current in Ampere")));

  rotate();  // fix historical flaw
}

Ampere_dc::~Ampere_dc()
{
}

QString Ampere_dc::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);

    QString plus = Ports.at(1)->Connection->Name;
    if (plus=="gnd") plus = "0";
    QString minus = Ports.at(0)->Connection->Name;
    if (minus=="gnd") minus = "0";
    s += QString(" %1 %2 DC %3\n").arg(plus).arg(minus)
            .arg(spicecompat::normalize_value(Props.at(0)->Value));
    return s;
}
Component* Ampere_dc::newOne()
{
  return new Ampere_dc();
}

Element* Ampere_dc::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Current Source");
  BitmapFile = (char *) "dc_current";

  if(getNewOne)  return new Ampere_dc();
  return 0;
}
