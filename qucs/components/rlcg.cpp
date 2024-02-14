/***************************************************************************
                                rlcg.cpp
                               ----------
    begin                : Wed Apr 1 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "main.h"
#include "rlcg.h"

#include "extsimkernels/spicecompat.h"
#include "node.h"

#include <QFontMetrics>

RLCG::RLCG()
{
  Description = QObject::tr("RLCG transmission line");
  Simulator = spicecompat::simAll;

  Lines.append(new qucs::Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-28,  7, 28,  7,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-28, 14,-21,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-21, 14,-14,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14, 14, -7,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -7, 14,  0,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 14,  7,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  7, 14, 14,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 14, 14, 21,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 21, 14, 28,  7,QPen(Qt::darkBlue,2)));

  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0); 
  int fHeight = smallmetrics.lineSpacing();
  QString tmp = QObject::tr("RLCG");
  int w = smallmetrics.boundingRect(tmp).width();
  Texts.append(new Text(w/-2, -fHeight, tmp));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 = -fHeight;
  x2 =  30; y2 = 16;

  tx = x1+4;
  ty = y2+4;
  Model = "RLCG";
  Name  = "Line";
  SpiceModel = "LTRA";

  Props.append(new Property("R", "0.0", false,
		QObject::tr("resistive load")+" ("+QObject::tr ("Ohm/m")+")"));
  Props.append(new Property("L", "0.6e-6", true,
		QObject::tr("inductive load")+" ("+QObject::tr ("H/m")+")"));
  Props.append(new Property("C", "240e-12", true,
		QObject::tr("capacitive load")+" ("+QObject::tr ("F/m")+")"));
  Props.append(new Property("G", "0.0", false,
		QObject::tr("conductive load")+" ("+QObject::tr ("S/m")+")"));
  Props.append(new Property("Length", "1 mm", true,
		QObject::tr("electrical length of the line")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

RLCG::~RLCG()
{
}

Component* RLCG::newOne()
{
  return new RLCG();
}

Element* RLCG::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("RLCG Transmission Line");
  BitmapFile = (char *) "rlcg";

  if(getNewOne)  return new RLCG();
  return 0;
}

QString RLCG::spice_netlist(bool isXyce)
{
    Q_UNUSED(isXyce);
    QString s;
    QString in = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
    QString out = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
    QString R = spicecompat::normalize_value(getProperty("R")->Value);
    QString L = spicecompat::normalize_value(getProperty("L")->Value);
    QString C = spicecompat::normalize_value(getProperty("C")->Value);
    QString G = spicecompat::normalize_value(getProperty("G")->Value);
    QString LEN = spicecompat::normalize_value(getProperty("Length")->Value);
    QString modname = "mod_" + Name;
    s += QString("O%1 %2 0 %3 0 %4\n").arg(Name).arg(in).arg(out).arg(modname);
    s += QString(".MODEL %1 LTRA(R=%2 C=%3 L=%4 G=%5 LEN=%6)\n")
            .arg(modname).arg(R).arg(C).arg(L).arg(G).arg(LEN);
    return s;
}
