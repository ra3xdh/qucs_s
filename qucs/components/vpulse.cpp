/***************************************************************************
                                vpulse.cpp
                               ------------
    begin                : Sat Sep 18 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "vpulse.h"
#include "node.h"
#include "misc.h"


vPulse::vPulse()
{
  Description = QObject::tr("ideal voltage pulse source");

  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Lines.append(new Line(  6, -3,  6,  3,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6, -7, -6, -3,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6,  3, -6,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6, -3,  6, -3,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6,  3,  6,  3,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Vpulse";
  Name  = "V";
  SpiceModel = "V";

  Props.append(new Property("U1", "0 V", true,
		QObject::tr("voltage before and after the pulse")));
  Props.append(new Property("U2", "1 V", true,
		QObject::tr("voltage of the pulse")));
  Props.append(new Property("T1", "0", true,
		QObject::tr("start time of the pulse")));
  Props.append(new Property("T2", "1 ms", true,
		QObject::tr("ending time of the pulse")));
  Props.append(new Property("Tr", "1 ns", false,
		QObject::tr("rise time of the leading edge")));
  Props.append(new Property("Tf", "1 ns", false,
		QObject::tr("fall time of the trailing edge")));

  rotate();  // fix historical flaw
}

vPulse::~vPulse()
{
}

QString vPulse::spice_netlist(bool isXyce)
{
    QString s=check_spice_refdes();

    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }



    double T1,T2,Pw,VL,VH,Tr,Tf,fac;
    QString unit;

    str2num(Props.at(0)->Value,VL,unit,fac); // VL
    VL *= fac;
    str2num(Props.at(1)->Value,VH,unit,fac); // VH
    VH *= fac;
    str2num(Props.at(2)->Value,T1,unit,fac); // Td
    T1 *= fac;
    str2num(Props.at(4)->Value,Tr,unit,fac); // Tr
    Tr *= fac;
    str2num(Props.at(5)->Value,Tf,unit,fac); // Tf
    Tf *= fac;
    str2num(Props.at(3)->Value,T2,unit,fac); //T2
    Pw = T2*fac - T1;

    s += QString(" DC PULSE(%1 %2 %3 %4 %5 %6) AC %7\n").arg(VL).arg(VH).arg(T1).arg(Tr).arg(Tf).arg(Pw).arg(VH);

    return s;
}

Component* vPulse::newOne()
{
  return new vPulse();
}

Element* vPulse::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Voltage Pulse");
  BitmapFile = (char *) "vpulse";

  if(getNewOne)  return new vPulse();
  return 0;
}
