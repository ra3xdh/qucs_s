/***************************************************************************
                          biast.cpp  -  description
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

#include "biast.h"
#include "extsimkernels/spicecompat.h"
#include "node.h"


BiasT::BiasT()
{
  Description = QObject::tr("bias t");
  Simulator = spicecompat::simAll;

  Arcs.append(new qucs::Arc( -3,  2, 6, 6, 16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -3,  8, 6, 6, 16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -3, 14, 6, 6, 16*270, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-22,-10, 22,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-22,-10,-22, 22,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-22, 22, 22, 22,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 22,-10, 22, 22,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-13, -6,-13,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -9, -6, -9,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -9,  0, 22,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-22,  0,-13,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-22,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 22,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,  0,  0,  2,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 20,  0, 22,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 22,  0, 30,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "BiasT";
  Name  = "X";
  SpiceModel="L";

  Props.append(new Property("L", "1 uH", false,
	QObject::tr("for transient simulation: inductance in Henry")));
  Props.append(new Property("C", "1 uF", false,
	QObject::tr("for transient simulation: capacitance in Farad")));
}

BiasT::~BiasT()
{
}

Component* BiasT::newOne()
{
  return new BiasT();
}

Element* BiasT::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Bias T");
  BitmapFile = (char *) "biast";

  if(getNewOne)  return new BiasT();
  return 0;
}

QString BiasT::spice_netlist(bool isXyce)
{
    Q_UNUSED(isXyce);
    QString s;
    QString L = spicecompat::normalize_value(getProperty("L")->Value);
    QString C = spicecompat::normalize_value(getProperty("C")->Value);
    QString pin1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
    QString pin2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
    QString pin3 = spicecompat::normalize_node_name(Ports.at(2)->Connection->Name);
    s += QString("C_%1 %2 %3 %4\n").arg(Name).arg(pin1).arg(pin2).arg(C);
    s += QString("L_%1 %2 %3 %4\n").arg(Name).arg(pin2).arg(pin3).arg(L);
    return s;
}
