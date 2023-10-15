/***************************************************************************
                               mutual2.cpp
                              --------------
    begin                : Sat Aug 20 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include "main.h"
#include "mutual2.h"

#include<QFontMetrics>


Mutual2::Mutual2()
{
  Description = QObject::tr("three mutual inductors");
  Simulator = spicecompat::simAll;

  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0);

  int w;
  QString stmp;

  Arcs.append(new qucs::Arc(-16,-58,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16,-46,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16,-34,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16, 46,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16, 34,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16, 22,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  4,-18,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  4, -6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  4,  6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-58,-10,-70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-70,-30,-70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,-18, 10,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 58,-10, 70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 70,-30, 70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10, 18, 10, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10, 30, 30, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-10,-30,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-22,-10,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 10,-30, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 10,-10, 22,QPen(Qt::darkBlue,2)));

  stmp = "1"; w = smallmetrics.boundingRect(stmp).width(); // compute width to right-align
  Texts.append(new Text(-13-w,-61,stmp));
  stmp = "2"; w = smallmetrics.boundingRect(stmp).width(); // compute width to right-align
  Texts.append(new Text(-13-w, 18,stmp));
  Texts.append(new Text( 13,-22,"3")); // left-aligned, no need to compute width

  Lines.append(new qucs::Line(  0,-57,  0, 57,QPen(Qt::darkBlue,1,Qt::DashLine)));

  Ports.append(new Port(-30,-70));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 70));
  Ports.append(new Port(-30, 10));
  Ports.append(new Port(-30,-10));

  x1 = -33; y1 = -74;
  x2 =  33; y2 =  74;

  tx = x1+4;
  ty = y2+4;
  Model = "MUT2";
  Name  = "Tr";
  SpiceModel = "K";

  Props.append(new Property("L1", "1 mH", false,
		QObject::tr("inductance of coil 1")));
  Props.append(new Property("L2", "1 mH", false,
		QObject::tr("inductance of coil 2")));
  Props.append(new Property("L3", "1 mH", false,
		QObject::tr("inductance of coil 3")));
  Props.append(new Property("k12", "0.9", false,
		QObject::tr("coupling factor between coil 1 and 2")));
  Props.append(new Property("k13", "0.9", false,
		QObject::tr("coupling factor between coil 1 and 3")));
  Props.append(new Property("k23", "0.9", false,
		QObject::tr("coupling factor between coil 2 and 3")));
}

Mutual2::~Mutual2()
{
}

Component* Mutual2::newOne()
{
  return new Mutual2();
}

Element* Mutual2::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("3 Mutual Inductors");
  BitmapFile = (char *) "mutual2";

  if(getNewOne)  return new Mutual2();
  return 0;
}

QString Mutual2::spice_netlist(bool isXyce)
{
    Q_UNUSED(isXyce);
    Q_UNUSED(isXyce);
    QString l1 = "L" + Name + "_L1";
    QString l2 = "L" + Name + "_L2";
    QString l3 = "L" + Name + "_L3";
    QString k12 = "K12_" + Name;
    QString k13 = "K13_" + Name;
    QString k23 = "K23_" + Name;
    QString ind1 = spicecompat::normalize_value(getProperty("L1")->Value);
    QString ind2 = spicecompat::normalize_value(getProperty("L2")->Value);
    QString ind3 = spicecompat::normalize_value(getProperty("L3")->Value);
    QString s = QString("%1 %2 %3 %4\n").arg(l1)
            .arg(spicecompat::normalize_node_name(Ports.at(0)->Connection->Name))
            .arg(spicecompat::normalize_node_name(Ports.at(5)->Connection->Name))
            .arg(ind1);
    s += QString("%1 %2 %3 %4\n").arg(l2)
            .arg(spicecompat::normalize_node_name(Ports.at(4)->Connection->Name))
            .arg(spicecompat::normalize_node_name(Ports.at(3)->Connection->Name))
            .arg(ind2);
    s += QString("%1 %2 %3 %4\n").arg(l3)
            .arg(spicecompat::normalize_node_name(Ports.at(1)->Connection->Name))
            .arg(spicecompat::normalize_node_name(Ports.at(2)->Connection->Name))
            .arg(ind3);
    s += QString("%1 %2 %3 %4\n").arg(k12).arg(l1).arg(l2)
            .arg(spicecompat::normalize_value(getProperty("k12")->Value));
    s += QString("%1 %2 %3 %4\n").arg(k13).arg(l1).arg(l3)
            .arg(spicecompat::normalize_value(getProperty("k12")->Value));
    s += QString("%1 %2 %3 %4\n").arg(k23).arg(l2).arg(l3)
            .arg(spicecompat::normalize_value(getProperty("k23")->Value));
    return s;
}

