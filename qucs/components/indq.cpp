/*
* indq.cpp - Lossy capacitor implementation
*
* copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
*
* This is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this package; see the file COPYING.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
* Boston, MA 02110-1301, USA.
*
*
*/

#include "node.h"
#include "indq.h"


IndQ::IndQ()
{
  Description = QObject::tr("Lossy inductor");

  //Spiral
  Arcs.append(new qucs::Arc(-18, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -6, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  6, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));



  //Draw Q character
  //Horizontal lines
  Lines.append(new qucs::Line( 10,  -10, 17,  -10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,  -17, 17,  -17,QPen(Qt::darkBlue,2)));
  //Vertical lines
  Lines.append(new qucs::Line( 17,  -10, 17,  -17,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,  -10, 10,  -17,QPen(Qt::darkBlue,2)));
  //Middle line
  Lines.append(new qucs::Line( 18,  -9, 14,  -13,QPen(Qt::darkBlue,2)));



  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;

  tx = x1+4;
  ty = y2+4;
  Model = "INDQ";
  SpiceModel = "L";
  Name  = "LQ";

  Property::Builder b;

  b.visible().simulator(spicecompat::simAll);
  Props.append(b.property("L", "1 nH",
               QObject::tr("Inductance")));
  Props.append(b.property("Q", "100",
               QObject::tr("Quality factor")));

  b.hidden().simulator(spicecompat::simAll);
  Props.append(b.property("f", "100 MHz",
               QObject::tr("Frequency at which Q is measured")));
  Props.append(b.property("Mode", "Linear",
               QObject::tr("Q frequency profile") +
               " [Linear, SquareRoot, Constant]"));

  b.hidden().simulator(spicecompat::simQucsator);
  Props.append(b.property("Temp", "26.85",
        QObject::tr("simulation temperature in degree Celsius (Qucsator only)")));
}
IndQ::~IndQ()
{
}

Component* IndQ::newOne()
{
  return new IndQ();
}

Element* IndQ::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Inductor with Q");
  BitmapFile = (char *) "indq";

  if(getNewOne)  return new IndQ();
  return 0;
}

QString IndQ::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    QString s;
    QString pin1 = Ports.at(0)->Connection->getName();
    pin1 = spicecompat::normalize_node_name(pin1);
    QString pin2 = Ports.at(1)->Connection->getName();
    pin2 = spicecompat::normalize_node_name(pin2);
    QString pin_int = QStringLiteral("_net_%1").arg(Name);
    QString Lname = spicecompat::check_refdes(Name, SpiceModel);
    QString Rname = "R" + Name;

    QString L = getProperty("L")->Value;
    L = spicecompat::normalize_value(L);
    QString Q = getProperty("Q")->Value;
    QString f0 = getProperty("f")->Value;
    f0 = spicecompat::normalize_value(f0);

    QString res_eq;
    QString double_pi = "8*atan(1)";
    QString mode = getProperty("Mode")->Value;
    if (mode == "Constant") {
        res_eq = QStringLiteral("%1*(%2)*hertz/(%3)").arg(double_pi).arg(L).arg(Q);
    } else if (mode == "Linear") {
        res_eq = QStringLiteral("%1*(%2)*(%3)/(%4)").arg(double_pi).arg(L).arg(f0).arg(Q);
    } else if (mode == "SquareRoot") {
        res_eq = QStringLiteral("%1*(%2)*sqrt(hertz*(%3))/(%4)").arg(double_pi).arg(L).arg(f0).arg(Q);
    }

    s = QStringLiteral("%1 %2 %3 L='%4'\n").arg(Lname).arg(pin1).arg(pin_int).arg(L);
    s += QStringLiteral("%1 %2 %3 R='%4'\n").arg(Rname).arg(pin_int).arg(pin2).arg(res_eq);

    return s;
}

