/***************************************************************************
    copyright            : (C) 2010 by Michael Margraf
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
#include "node.h"

#include "ctline.h"
#include "extsimkernels/spicecompat.h"

CoupledTLine::CoupledTLine()
{
  Description = QObject::tr("coupled transmission lines");
  Simulator = spicecompat::simQucsator + spicecompat::simNgspice;

  Arcs.append(new qucs::Arc(-28,-40, 18, 38,16*232, 16*33,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-28,  2, 18, 38, 16*95, 16*33,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-20,-2,-16,-2,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20, 2,-16, 2,QPen(Qt::darkBlue,2)));

  Arcs.append(new qucs::Arc( 10,-40, 18, 38,16*270, 16*40,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 10,  2, 18, 38, 16*50, 16*40,QPen(Qt::darkBlue,2)));

  Arcs.append(new qucs::Arc(-38,-10, 16, 28, 16*45, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-38,-18, 16, 28,16*270, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 22,-10, 16, 28, 16*90, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 22,-18, 16, 28,16*225, 16*45,QPen(Qt::darkBlue,2)));

  // shield
  Arcs.append(new qucs::Arc(-20, -9, 8, 18,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 11, -9, 8, 18,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-16,  9, 16,  9,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,-10));
  Ports.append(new Port( 30,-10));
  Ports.append(new Port( 30, 10));
  Ports.append(new Port(-30, 10));

  x1 = -30; y1 =-12;
  x2 =  30; y2 = 12;

  tx = x1+4;
  ty = y2+4;
  Model = "CTLIN";
  Name  = "Line";
  SpiceModel = "A";

  Props.append(new Property("Ze", "50 Ohm", true,
		QObject::tr("characteristic impedance of even mode")));
  Props.append(new Property("Zo", "50 Ohm", true,
		QObject::tr("characteristic impedance of odd mode")));
  Props.append(new Property("L", "1 mm", true,
		QObject::tr("electrical length of the line")));
  Props.append(new Property("Ere", "1", false,
		QObject::tr("relative dielectric constant of even mode")));
  Props.append(new Property("Ero", "1", false,
		QObject::tr("relative dielectric constant of odd mode")));
  Props.append(new Property("Ae", "0 dB", false,
		QObject::tr("attenuation factor per length of even mode")));
  Props.append(new Property("Ao", "0 dB", false,
		QObject::tr("attenuation factor per length of odd mode")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}


// -------------------------------------------------------------------
Component* CoupledTLine::newOne()
{
  return new CoupledTLine();
}

Element* CoupledTLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coupled Transmission Line");
  BitmapFile = (char *) "ctline";

  if(getNewOne)  return new CoupledTLine();
  return 0;
}


QString CoupledTLine::spice_netlist(spicecompat::SpiceDialect dialect)
{
  QString s;
  if (dialect != spicecompat::SPICEDefault) return s;
  QString p1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
  QString p2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
  QString p3 = spicecompat::normalize_node_name(Ports.at(2)->Connection->Name);
  QString p4 = spicecompat::normalize_node_name(Ports.at(3)->Connection->Name);

  QString L = spicecompat::normalize_value(getProperty("L")->Value);
  QString Ze = spicecompat::normalize_value(getProperty("Ze")->Value);
  QString Zo = spicecompat::normalize_value(getProperty("Zo")->Value);
  QString Ere = spicecompat::normalize_value(getProperty("Ere")->Value);
  QString Ero = spicecompat::normalize_value(getProperty("Ero")->Value);
  QString Ae = spicecompat::normalize_value(getProperty("Ae")->Value);
  QString Ao = spicecompat::normalize_value(getProperty("Ao")->Value);

  s = QString("A_%1 %hd(%2 0) %hd(%3 0) %hd(%4 0) %hd(%5 0)"
              " %vd(%2 0) %vd(%3 0) %vd(%4 0) %vd(%5 0) MODEL_%1\n")
              .arg(Name).arg(p1).arg(p2).arg(p3).arg(p4);
  s += QString(".MODEL MODEL_%1 CPLINE(L=%2 ze=%3 zo=%4 ere=%5 ero=%6 ae=%7 ao=%8)\n")
           .arg(Name).arg(L).arg(Ze).arg(Zo).arg(Ere).arg(Ero).arg(Ae).arg(Ao);

  return s;
}
