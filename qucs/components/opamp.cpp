/***************************************************************************
                          opamp.cpp  -  description
                             -------------------
    begin                : Sun Oct 31 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    SPICE model added by Vadim Kuznetsov <ra3xdh@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "opamp.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


OpAmp::OpAmp()
{
  Description = QObject::tr("operational amplifier");

  Lines.append(new Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 40,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-20,-35,-20, 35,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-35, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 35, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-16, 19, -9, 19,QPen(Qt::black,1)));
  Lines.append(new Line(-16,-19, -9,-19,QPen(Qt::red,1)));
  Lines.append(new Line(-13,-22,-13,-15,QPen(Qt::red,1)));

  Ports.append(new Port(-30, 20));
  Ports.append(new Port(-30,-20));
  Ports.append(new Port( 40,  0));

  x1 = -30; y1 = -38;
  x2 =  30; y2 =  38;

  tx = x1+4;
  ty = y2+4;
  Model = "OpAmp";
  Name  = "OP";
  SpiceModel = "B";

  Props.append(new Property("G", "1e6", true,
		QObject::tr("voltage gain")));
  Props.append(new Property("Umax", "15 V", false,
	QObject::tr("absolute value of maximum and minimum output voltage")));
}

OpAmp::~OpAmp()
{
}

Component* OpAmp::newOne()
{
  return new OpAmp();
}

Element* OpAmp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("OpAmp");
  BitmapFile = (char *) "opamp";

  if(getNewOne)  return new OpAmp();
  return 0;
}

QString OpAmp::spice_netlist(bool isXyce)
{
    QString in_p = Ports.at(0)->Connection->Name;
    if (in_p=="gnd") in_p="0";
    QString in_m = Ports.at(1)->Connection->Name;
    if (in_m=="gnd") in_m="0";
    QString out = Ports.at(2)->Connection->Name;
    if (out=="gnd") out="0";

    QString G = spicecompat::normalize_value(Props.at(0)->Value);
    QString Vmax = spicecompat::normalize_value(Props.at(1)->Value);

    QString s;
    s = QString("B_%1 %2 0 V = ").arg(Name).arg(out);

    if (isXyce) {
        s += QString("%1*V(%2,%3)*stp(%4-%1*V(%2,%3))*stp(%1*V(%2,%3)-(-%4))"
                    "+%4*stp(%1*V(%2,%3)-%4)"
                    "+(-%4)*stp((-%4)-%1*V(%2,%3))\n").arg(G).arg(in_p).arg(in_m).arg(Vmax);
    } else {
        s += QString("%1*V(%2,%3)*u(%4-%1*V(%2,%3))*u(%1*V(%2,%3)-(-%4))"
                    "+%4*u(%1*V(%2,%3)-%4)"
                    "+(-%4)*u((-%4)-%1*V(%2,%3))\n").arg(G).arg(in_p).arg(in_m).arg(Vmax);
    }
    return s;
}
