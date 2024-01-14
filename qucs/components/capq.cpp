/*
* capq.cpp - Lossy capacitor implementation
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
#include "capq.h"


CapQ::CapQ()
{
  Description = QObject::tr("Lossy capacitor");

  //Lines connection device and ports
  Lines.append(new qucs::Line( -4,-11, -4, 11,QPen(Qt::darkBlue,4)));
  Lines.append(new qucs::Line(  4,-11,  4, 11,QPen(Qt::darkBlue,4)));


  Lines.append(new qucs::Line(-30,  0, -4,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  4,  0, 30,  0,QPen(Qt::darkBlue,2)));


  //Draw Q
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
  Model = "CAPQ";
  SpiceModel = "C";
  Name  = "CQ";

  Props.append(new Property("C", "1 pF", true,
		QObject::tr("Capacitance")));
  Props.append(new Property("Q", "100", true,
		QObject::tr("Quality factor")));
  Props.append(new Property("f", "100 MHz", false,
		QObject::tr("Frequency at which Q is measured")));
  Props.append(new Property("Mode", "Linear", false,
		QObject::tr("Q frequency profile")+
		" [Linear, SquareRoot, Constant]"));
  Props.append(new Property("Temp", "26.85", false,
                QObject::tr("simulation temperature in degree Celsius (Qucsator only)")));
}
CapQ::~CapQ()
{
}

Component* CapQ::newOne()
{
  return new CapQ();
}

Element* CapQ::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Capacitor with Q");
  BitmapFile = (char *) "capq";

  if(getNewOne)  return new CapQ();
  return 0;
}

QString CapQ::spice_netlist(bool isXyce)
{
    Q_UNUSED(isXyce);
    QString s;
    QString pin1 = Ports.at(0)->Connection->Name;
    pin1 = spicecompat::normalize_node_name(pin1);
    QString pin2 = Ports.at(1)->Connection->Name;
    pin2 = spicecompat::normalize_node_name(pin2);
    QString Cname = spicecompat::check_refdes(Name, SpiceModel);
    QString Rname = "R" + Name;

    QString C = getProperty("C")->Value;
    C = spicecompat::normalize_value(C);
    QString Q = getProperty("Q")->Value;
    QString f0 = getProperty("f")->Value;
    f0 = spicecompat::normalize_value(f0);

    QString res_eq;
    QString double_pi = "8*atan(1)";
    QString mode = getProperty("Mode")->Value;
    if (mode == "Constant") {
        res_eq = QString("%1*(%2)*hertz/(%3)").arg(double_pi).arg(C).arg(Q);
    } else if (mode == "Linear") {
        res_eq = QString("%1*(%2)*(%3)/(%4)").arg(double_pi).arg(C).arg(f0).arg(Q);
    } else if (mode == "SquareRoot") {
        res_eq = QString("%1*(%2)*sqrt(hertz*(%3))/(%4)").arg(double_pi).arg(C).arg(f0).arg(Q);
    }

    s = QString("%1 %2 %3 C='%4'\n").arg(Cname).arg(pin1).arg(pin2).arg(C);
    s += QString("%1 %2 %3 R='1/((%4)+1e-8)'\n").arg(Rname).arg(pin1).arg(pin2).arg(res_eq);

    return s;
}
