/***************************************************************************
                                iexp.cpp
                               ---------------
    begin                     : Tue Mar 06 2007
    copyright               : (C) 2007 by Gunther Kraut
    email                      : gn.kraut@t-online.de
    spice4qucs code added  Wed. 18 March 2015
    copyright               : (C) 2015 by Mike Brinson
    email                     : mbrin72043@yahoo.co.uk 

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "iexp.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"

iExp::iExp()
{
  Description = QObject::tr("exponential current source");

  // normal current source symbol
  Ellipses.append(new qucs::Ellips(-12,-12, 24, 24,QPen(Qt::darkBlue,2)));
  // pins
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  // arrow
  Lines.append(new qucs::Line( -7,  0,  6,  0,QPen(Qt::darkBlue,3, Qt::SolidLine, Qt::FlatCap)));
  Polylines.append(new qucs::Polyline(
    std::vector<QPointF>{{0, -4},{6, 0}, {0, 4}}, QPen(Qt::darkBlue, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));

  Texts.append(new Text(25, -30, "Exp", Qt::darkBlue, 12, 0.0, -1.0));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  20;

  tx = x1+4;
  ty = y2+8;
  Model = "Iexp";
  SpiceModel = "I";
  Name  = "I";

  Props.append(new Property("I1", "0", true,
		QObject::tr("current before rising edge")));
  Props.append(new Property("I2", "1 A", true,
		QObject::tr("maximum current of the pulse")));
  Props.append(new Property("T1", "0", true,
		QObject::tr("start time of the exponentially rising edge")));
  Props.append(new Property("T2", "1 ms", true,
		QObject::tr("start of exponential decay")));
  Props.append(new Property("Tr", "1 ns", false,
		QObject::tr("time constant of the rising edge")));
  Props.append(new Property("Tf", "1 ns", false,
		QObject::tr("time constant of the falling edge")));

  rotate();  // fix historical flaw
}

iExp::~iExp()
{
}

Component* iExp::newOne()
{
  return new iExp();
}

Element* iExp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Exponential Current Pulse");
  BitmapFile = (char *) "iexp";

  if(getNewOne)  return new iExp();
  return 0;
}

QString iExp::spice_netlist(bool, bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);

   s += " " + spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
   s += " " + spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);

   QString U1= spicecompat::normalize_value(Props.at(0)->Value);
   QString U2 = spicecompat::normalize_value(Props.at(1)->Value);
   QString T1 = spicecompat::normalize_value(Props.at(2)->Value);
   QString T2 = spicecompat::normalize_value(Props.at(3)->Value);
   QString Tr = spicecompat::normalize_value(Props.at(4)->Value);
   QString Tf = spicecompat::normalize_value(Props.at(5)->Value);

    s += QStringLiteral(" DC 0 EXP(%1 %2 %3 %4 %5 %6) AC 0\n").arg(U1).arg(U2).arg(T1).arg(Tr).arg(T2).arg(Tf);
    return s;
}

