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
#include "misc.h"
#include "extsimkernels/spicecompat.h"


Volt_ac::Volt_ac()
{
  Description = QObject::tr("ideal ac voltage source");

  Arcs.append(new qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -3, -7,  7,  7,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -3,  0,  7,  7, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new qucs::Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new qucs::Line(-18,  5,-18, 11,QPen(Qt::black,1)));

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
  Props.append(new Property("f", "1 kHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("Phase", "0", false,
		QObject::tr("initial phase in degrees")));
  Props.append(new Property("Theta", "0", false,
		QObject::tr("damping factor (transient simulation only)")));
  Props.append(new Property("VO", "0", false,
                            QObject::tr("offset voltage (SPICE only)")));
  Props.append(new Property("TD", "0", false,
                            QObject::tr("delay time (SPICE only)")));

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

QString Volt_ac::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

    QString volts = spicecompat::normalize_value(Props.at(0)->Value);
    QString freq = spicecompat::normalize_value(Props.at(1)->Value);

    QString phase = Props.at(2)->Value;
    phase.remove(' ');
    if (phase.isEmpty()) phase = "0";

    QString theta = Props.at(3)->Value;
    theta.remove(' ');
    if (theta.isEmpty()) theta="0";

    QString VO = spicecompat::normalize_value(getProperty("VO")->Value);
    QString TD = spicecompat::normalize_value(getProperty("TD")->Value);

    s += QString(" DC %1 SIN(%1 %2 %3 %4 %5 %6) AC %7 ACPHASE %8\n")
            .arg(VO).arg(volts).arg(freq).arg(TD).arg(theta).arg(phase).arg(volts).arg(phase);
    return s;
}

QString Volt_ac::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  for (Port *p1 : Ports)
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  for(unsigned int i=0; i <= Props.count()-3; i++)
    if(Props.at(i)->Name != "Symbol")
      s += " "+Props.at(i)->Name+"=\""+Props.at(i)->Value+"\"";

  return s + '\n';
}
