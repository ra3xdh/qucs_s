/***************************************************************************
                                vexp.cpp
                               ----------
    begin                : Fri Mar 9 2007
    copyright          : (C) 2007 by Gunther Kraut
    email                : gn.kraut@t-online.de
    spice4qucs code added  Wed. 18 March 2015
    copyright          : (C) 2015 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                                                                     *
 *   This program is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or           *
 *   (at your option) any later version.                                                                          *
 *                                                                                                                   *
 ***************************************************************************/

#include "vexp.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


vExp::vExp()
{
  Description = QObject::tr("exponential voltage source");

  // normal voltage source symbol
  Ellipses.append(new qucs::Ellips(-12,-12, 24, 24,  QPen(Qt::darkBlue,2)));
  // pins
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  // plus sign
  Lines.append(new qucs::Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new qucs::Line( 18,  5, 18,  11,QPen(Qt::red,1)));
  // minus sign
  Lines.append(new qucs::Line(-18,  5,-18, 11,QPen(Qt::black,1)));
  Texts.append(new Text(8, -10, "Exp", Qt::darkBlue, 12, 0.0, -1.0));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Vexp";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("U1", "0 V", true,
		QObject::tr("voltage before rising edge")));
  Props.append(new Property("U2", "1 V", true,
		QObject::tr("maximum voltage of the pulse")));
  Props.append(new Property("T1", "0", true,
		QObject::tr("start time of the exponentially rising edge")));
  Props.append(new Property("T2", "1 ms", true,
		QObject::tr("start of exponential decay")));
  Props.append(new Property("Tr", "1 ns", false,
		QObject::tr("rise time of the rising edge")));
  Props.append(new Property("Tf", "1 ns", false,
		QObject::tr("fall time of the falling edge")));

  rotate();  // fix historical flaw
}

vExp::~vExp()
{
}

Component* vExp::newOne()
{
  return new vExp();
}

Element* vExp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Exponential Voltage Pulse");
  BitmapFile = (char *) "vexp";

  if(getNewOne)  return new vExp();
  return 0;
}

QString vExp::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

   QString U1= spicecompat::normalize_value(Props.at(0)->Value);
   QString U2 = spicecompat::normalize_value(Props.at(1)->Value);
   QString T1 = spicecompat::normalize_value(Props.at(2)->Value);
   QString T2 = spicecompat::normalize_value(Props.at(3)->Value);
   QString Tr = spicecompat::normalize_value(Props.at(4)->Value);
   QString Tf = spicecompat::normalize_value(Props.at(5)->Value);

    s += QStringLiteral(" DC 0 EXP(%1 %2 %3 %4 %5 %6) AC 0\n").arg(U1).arg(U2).arg(T1).arg(Tr).arg(T2).arg(Tf);
    return s;
}
