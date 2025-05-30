/***************************************************************************
                          mscoupled.cpp  -  description
                             -------------------
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
#include "node.h"
#include "mscoupled.h"
#include "extsimkernels/spicecompat.h"


MScoupled::MScoupled()
{
  Description = QObject::tr("coupled microstrip line");
  Simulator = spicecompat::simQucsator + spicecompat::simNgspice;

  Lines.append(new qucs::Line(-30,-12,-16,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,-30,-30,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 20,-12, 30,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,-30, 30,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-11,-20, 25,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-21, -4, 15, -4,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-11,-20,-21, -4,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 25,-20, 15, -4,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-30, 12,-20, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 30,-30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 16, 12, 30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, 30, 30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-15,  4, 21,  4,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-25, 20, 11, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-15,  4,-25, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 21,  4, 11, 20,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 =-33;
  x2 =  30; y2 = 33;

  tx = x1+4;
  ty = y2+4;
  Model = "MCOUPLED";
  Name  = "MS";
  SpiceModel = "A";

  Props.append(new Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.append(new Property("W", "1 mm", true,
	QObject::tr("width of the line")));
  Props.append(new Property("L", "10 mm", true,
	QObject::tr("length of the line")));
  Props.append(new Property("S", "1 mm", true,
	QObject::tr("spacing between the lines")));
  Props.append(new Property("Model", "Kirschning", false,
	QObject::tr("microstrip model")+" [Kirschning, Hammerstad]"));
  Props.append(new Property("DispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+
	" [Kirschning, Getsinger]"));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
}

MScoupled::~MScoupled()
{
}

Component* MScoupled::newOne()
{
  return new MScoupled();
}

Element* MScoupled::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coupled Microstrip Line");
  BitmapFile = (char *) "mscoupled";

  if(getNewOne)  return new MScoupled();
  return 0;
}

QString MScoupled::spice_netlist(spicecompat::SpiceDialect dialect)
{
  QString s;
  if (dialect != spicecompat::SPICEDefault) return s;
  QString subline = getSpiceSubstrateLine();
  QString p1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
  QString p2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
  QString p3 = spicecompat::normalize_node_name(Ports.at(2)->Connection->Name);
  QString p4 = spicecompat::normalize_node_name(Ports.at(3)->Connection->Name);

  QString L = spicecompat::normalize_value(getProperty("L")->Value);
  QString W = spicecompat::normalize_value(getProperty("W")->Value);
  QString S = spicecompat::normalize_value(getProperty("S")->Value);

  int Mod = spicecompat::strToMSlineModel(getProperty("Model")->Value);
  int Disp = spicecompat::strToDispModel(getProperty("DispModel")->Value);

  s = QString("A_%1 %hd(%2 0) %hd(%3 0) %hd(%4 0) %hd(%5 0)"
              " %vd(%2 0) %vd(%3 0) %vd(%4 0) %vd(%5 0) MODEL_%1\n")
          .arg(Name).arg(p1).arg(p2).arg(p3).arg(p4);
  s += QString(".MODEL MODEL_%1 CPMLIN(L=%2 W=%3 S=%4 model=%4 disp=%5 %6)\n")
           .arg(Name).arg(L).arg(W).arg(S).arg(Mod).arg(Disp).arg(subline);

  return s;
}
