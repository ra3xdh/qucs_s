/***************************************************************************
                         gNL.cpp  -  description
                   --------------------------------------
    begin                   : Tue. Mar 31 2015
    copyright               : (C) by Mike Brinson (mbrin72043@yahoo.co.uk),
                            : Vadim Kuznetsov (ra3xdh@gmail.com)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gNL.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


gNL::gNL()
{
  Description = QObject::tr("SPICE G (VOL, VALUE, TABLE, POLY):\nMultiple line ngspice non-linear G specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");
  Simulator = spicecompat::simSpice;
  // Value, Table and POLY forms are allowed: should work with ngspice and Xyce.

  Ellipses.append(new qucs::Ellips(-12,-12, 24, 24, QPen(Qt::blue,3)));
  Texts.append(new Text(26, 6,"GNL",Qt::blue,12.0,0.0,-1.0));
  // pins
  Lines.append(new qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 14,  0,QPen(Qt::darkBlue,2)));
  // arrow
  Lines.append(new qucs::Line( -7,  0,  6,  0,QPen(Qt::blue,3, Qt::SolidLine, Qt::FlatCap)));
  Polylines.append(new qucs::Polyline(
    std::vector<QPointF>{{0, -4},{6, 0}, {0, 4}}, QPen(Qt::blue, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "gNL";
  SpiceModel = "G";
  Name  = "G";

  Props.append(new Property("G", "", true,"Non-linear spec.\n(with control nodes)"));
  Props.append(new Property("Line_2", "", false,"+ continuation line 1"));
  Props.append(new Property("Line_3", "", false,"+ continuation line 2"));
  Props.append(new Property("Line_4", "", false,"+ continuation line 3"));
  Props.append(new Property("Line_5", "", false,"+ continuation line 4"));

    rotate();  // fix historical flaw
}

gNL::~gNL()
{
}

Component* gNL::newOne()
{
  return new gNL();
}

Element* gNL::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("G");
  BitmapFile = (char *) "gNL";

  if(getNewOne)  return new gNL();
  return 0;
}

QString gNL::netlist()
{
    return QString();
}

QString gNL::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    QString s = spicecompat::check_refdes(Name,SpiceModel);
    s += " " + spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
    s += " " + spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
    QString G= Props.at(0)->Value;
    QString Line_2 = Props.at(1)->Value;
    QString Line_3 = Props.at(2)->Value;
    QString Line_4 = Props.at(3)->Value;
    QString Line_5 = Props.at(4)->Value;

    if(  G.length()      > 0 )   s += QStringLiteral(" %1").arg(G);
    if(  Line_2.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_2);
    if(  Line_3.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_3);
    if(  Line_4.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_4);
    if(  Line_5.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_5);

    s += "\n";

    return s;
}
