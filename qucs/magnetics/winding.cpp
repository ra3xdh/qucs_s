/***************************************************************************
                         Icouple.cpp  -  description
                   --------------------------------------
    begin                  : Wed 11 Nov 2015
    copyright              : (C) by Mike Brinson (mbrin72043@yahoo.co.uk),
						   :  Vadim Kuznetsov (ra3xdh@gmail.com)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "winding.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"

#include "main.h"
#include "schematic.h"


Winding::Winding()
{
  Description = QObject::tr("Winding");
  Simulator = spicecompat::simSpice;

  Lines.append(new qucs::Line(0,  -40, 0,  -30,QPen(Qt::darkRed,2)));  // L1
  Lines.append(new qucs::Line(0,  -30, 0,  -25,QPen(Qt::darkRed,3)));      // L2
  Lines.append(new qucs::Line(0,  -25, 10,  -25,QPen(Qt::darkRed,3)));      // L3
  Lines.append(new qucs::Line(10,  -25, 10,  -15,QPen(Qt::darkRed,3)));      // L4
  Lines.append(new qucs::Line(10,  -15, 0,  -15,QPen(Qt::darkRed,3)));      // L5
  Lines.append(new qucs::Line(10,  -15, 0,  -15,QPen(Qt::darkRed,3)));      // L6
  Lines.append(new qucs::Line(0,  -15, 0,   -5,QPen(Qt::darkRed,3)));      // L7
  Lines.append(new qucs::Line(0,   -5, 10,   -5,QPen(Qt::darkRed,3)));      // L8
  Lines.append(new qucs::Line(10,   -5, 10,    5,QPen(Qt::darkRed,3)));      // L9
  Lines.append(new qucs::Line(10,    5, 0,    5,QPen(Qt::darkRed,3)));      // L10
  Lines.append(new qucs::Line(0,    5, 0,   15,QPen(Qt::darkRed,3)));      // L11
  Lines.append(new qucs::Line(0,   15, 10,   15,QPen(Qt::darkRed,3)));      // L12
  Lines.append(new qucs::Line(10,   15, 10,   25,QPen(Qt::darkRed,3)));      // L13
  Lines.append(new qucs::Line(10,   25, 0,   25,QPen(Qt::darkRed,3)));      // L14
  Lines.append(new qucs::Line(0,   25, 0,   30,QPen(Qt::darkRed,3)));      // L15
  Lines.append(new qucs::Line(0,   30, 0,   40,QPen(Qt::darkBlue,2)));  // L16

  Arcs.append(new qucs::Arc(-18, -20,  5,  5,  0, 16*360,QPen(Qt::darkRed,3)));

  Lines.append(new qucs::Line(16,   -25, 16,   25,QPen(Qt::black,1)));
  Lines.append(new qucs::Line(20,   -25, 20,   25,QPen(Qt::black,1)));
  Lines.append(new qucs::Line(24,   -25, 24,   25,QPen(Qt::black,1)));

  Ports.append(new Port( -00,  -40));  // Pplus
  Ports.append(new Port( -00,   40));  // Pneg
  //Ports.append(new Port(  40,  -40));  // PVplus
  //Ports.append(new Port(  40,   40));  // PVneg

  x1 = -25; y1 = -45;
  x2 =  45; y2 =  45;

  tx = x1+5;
  ty = y2+5;
  Model = "WINDING";
  SpiceModel = "X";
  Name  = "W";

  Props.append(new Property("CORE", "CORE1", true,  QObject::tr("CORE component name")));
  Props.append(new Property("N", "10", true, QObject::tr("Number of turns")));
  Props.append(new Property("Rs", "0.1", true, QObject::tr("Series resistance")));

}

Winding::~Winding()
{
}

Component* Winding::newOne()
{
  return new Winding();
}

Element* Winding::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Winding");
  BitmapFile = (char *) "winding";

  if(getNewOne)  return new Winding();
  return 0;
}

QString Winding::netlist()
{
    return QString();
}

QString Winding::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QString P1 = Ports.at(0)->Connection->Name;
    QString P2 = Ports.at(1)->Connection->Name;

    QString CORE = getProperty("CORE")->Value;
    QString Nt = spicecompat::normalize_value(getProperty("N")->Value);
    QString Rs = spicecompat::normalize_value(getProperty("Rs")->Value);

    if (containingSchematic == nullptr) return s;

    QString H_node, B_node;
    for (Component *pc: *containingSchematic->a_Components) {
      if (pc->Name == CORE) {
        QString BH = pc->getProperty("BHprobes")->Value;
        if (BH == "true") {
          H_node = pc->Ports.at(0)->Connection->Name;
          B_node = pc->Ports.at(1)->Connection->Name;
        } else {
          H_node = "net_" + pc->Name + "_H_node";
          B_node = "net_" + pc->Name + "_B_node";
        }
      }
    }

    s = "X_" + Name;
    s += QString(" %1 %2 %3 %4 winding N=%5 Rs=%6\n")
             .arg(P1).arg(P2).arg(H_node).arg(B_node).arg(Nt).arg(Rs);
    s += "\n";

    return s;
}

QString Winding::getSpiceLibrary()
{
  QString f = spicecompat::getSpiceLibPath("winding.cir");
  QString s = QString (".INCLUDE \"%1\"\n").arg(f);
  return s;
}
