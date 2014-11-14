/***************************************************************************
                               volt_ac.cpp
                              -------------
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

#include "volt_ac.h"
#include "node.h"
#include "main.h"


Volt_ac::Volt_ac()
{
  Description = QObject::tr("ideal ac voltage source");

  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( -3, -7,  7,  7,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( -3,  0,  7,  7, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Vac";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("U", "1 V", true,
		QObject::tr("peak voltage in Volts")));
  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("Phase", "0", false,
		QObject::tr("initial phase in degrees")));
  Props.append(new Property("Theta", "0", false,
		QObject::tr("damping factor (transient simulation only)")));

  rotate();  // fix historical flaw
}

Volt_ac::~Volt_ac()
{
}

Component* Volt_ac::newOne()
{
  return new Volt_ac();
}

Element* Volt_ac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac Voltage Source");
  BitmapFile = (char *) "ac_voltage";

  if(getNewOne)  return new Volt_ac();
  return 0;
}

QString Volt_ac::spice_netlist()
{
    QString s=check_spice_refdes();
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

    double freq,volts,fac;
    QString unit;

    str2num(Props.at(0)->Value,volts,unit,fac);
    volts *=fac;

    str2num(Props.at(1)->Value,freq,unit,fac);
    freq *=fac;

    QString theta = Props.at(3)->Value;
    theta.remove(' ');
    if (theta.isEmpty()) theta="0";
    s += QString(" DC SIN(0 %1 %2 0 %3) AC\n").arg(volts).arg(freq).arg(theta);
    return s;
}
