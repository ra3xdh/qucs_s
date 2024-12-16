/***************************************************************************
                        vcresistor.cpp  -  description
                             -------------------
    begin                : Feb 13 2014
    copyright            : (C) 2014 by Richard Crozier
    email                : richard dot crozier at yahoo dot co dot uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "vcresistor.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


vcresistor::vcresistor()
{
  Description = QObject::tr("voltage controlled resistor");
  Simulator = spicecompat::simAll;

  // The resistor shape
  Lines.append(new qucs::Line(5, 18, 5, -18, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(17, 18, 17, -18, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(5, 18, 17, 18, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(5, -18, 17, -18, QPen(Qt::darkBlue,2)));

  // horizontal lines on top and bottom of left hand side
  Lines.append(new qucs::Line(-30,-30,-12,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 30,-12, 30,QPen(Qt::darkBlue,2)));
  // horizontal lines on top and bottom of right hand side
  Lines.append(new qucs::Line( 11,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11, 30, 30, 30,QPen(Qt::darkBlue,2)));
  // vertical lines on top and bottom of left hand side
  Lines.append(new qucs::Line(-12,-30,-12,-23,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12, 30,-12, 23,QPen(Qt::darkBlue,2)));
  // vertical lines on top and bottom of right hand side
  Lines.append(new qucs::Line( 11,-30, 11,-18,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11, 30, 11, 18,QPen(Qt::darkBlue,2)));

  // downward pointing arrow
  Lines.append(new qucs::Line(-12,-18,-12, 18,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-12, 18,-17,  9,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-12, 18, -7,  9,QPen(Qt::darkBlue,1)));

  Lines.append(new qucs::Line(-25,-27, 25,-27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 25,-27, 25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 25, 27,-25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line(-25, 27,-25,-27,QPen(Qt::darkGray,1)));


  Ports.append(new Port(-30,-30));
  Ports.append(new Port(-30, 30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));


  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "vcresistor";
  Name  = "VCR";
  SpiceModel = "R";

  Props.append(new Property("gain", "1", true,
		QObject::tr("resistance gain")));
}

vcresistor::~vcresistor()
{
}

Component* vcresistor::newOne()
{
  return new vcresistor();
}

Element* vcresistor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Voltage Controlled Resistor");
  BitmapFile = (char *) "vcresistor";

  if(getNewOne)  return new vcresistor();
  return 0;
}

QString vcresistor::netlist()
{
  QString s;
  QString EDD_Name = "EDD" + Name;
  QString in1 = Ports.at(0)->Connection->Name;
  QString in2 = Ports.at(1)->Connection->Name;
  QString out1 = Ports.at(2)->Connection->Name;
  QString out2 = Ports.at(3)->Connection->Name;
  QString gain = getProperty("gain")->Value;
  s = QStringLiteral("EDD:%1 %2 %3 %4 %5 I1=\"%1.I1\" Q1=\"%1.Q1\" I2=\"%1.I2\" Q2=\"%1.Q2\"\n").arg(EDD_Name,in1,in2,out1,out2);
  s += QStringLiteral("Eqn:Eqn%1I1 %1.I1=\"0\" Export=\"no\"\n").arg(EDD_Name);
  s += QStringLiteral("Eqn:Eqn%1Q1 %1.Q1=\"0\" Export=\"no\"\n").arg(EDD_Name);
  s += QStringLiteral("Eqn:Eqn%1I2 %1.I2=\"V2/(1e-20+abs(V1*(%2)))\" Export=\"no\"\n").arg(EDD_Name,gain);
  s += QStringLiteral("Eqn:Eqn%1Q2 %1.Q2=\"0\" Export=\"no\"\n").arg(EDD_Name);
  return s;
}

QString vcresistor::spice_netlist(bool isXyce, bool)
{
  Q_UNUSED(isXyce);
  QString s;
  QString gain = spicecompat::normalize_value(getProperty("gain")->Value);
  QString in1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
  QString in2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
  QString out1 = spicecompat::normalize_node_name(Ports.at(2)->Connection->Name);
  QString out2 = spicecompat::normalize_node_name(Ports.at(3)->Connection->Name);
  s = QStringLiteral("R%1 %2 %3 R='1e-15+abs(V(%4,%5)*(%6))'\n").arg(Name, out1, out2, in1, in2, gain);
  return s;
}

