/***************************************************************************
                          iprobe.cpp  -  description
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

#include "iprobe.h"
#include "node.h"


iProbe::iProbe()
{
  Description = QObject::tr("current probe");

  // "contacts"
  Lines.append(new qucs::Line(-30,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 20,  0,QPen(Qt::darkBlue,2)));

  // arrow
  Lines.append(new qucs::Line(-20,  0, 20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -4, -4,  4,  0,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line(  4,  0, -4,  4,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));

  // outer frame
  Lines.append(new qucs::Line(-20,-31, 20,-31,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,  9, 20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,-31,-20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 20,-31, 20,  9,QPen(Qt::darkBlue,2)));

  // gauge frame
  Lines.append(new qucs::Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  // gauge
  Arcs.append(new qucs::Arc(-20,-23, 39, 39, 16*50, 16*80,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-11,-24, -2, -9,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -34;
  x2 =  30; y2 =  12;

  tx = x1+4;
  ty = y2+4;
  Model = "IProbe";
  Name  = "Pr";
}

iProbe::~iProbe()
{
}

Component* iProbe::newOne()
{
  return new iProbe();
}

Element* iProbe::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Current Probe");
  BitmapFile = (char *) "iprobe";

  if(getNewOne)  return new iProbe();
  return 0;
}

QString iProbe::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    QString s = QStringLiteral("V%1 %2 %3 DC 0\n").arg(Name).arg(Ports.at(0)->Connection->Name)
            .arg(Ports.at(1)->Connection->Name);
    return s;
}

/*!
 * \brief iProbe::getProbeVariable Get current probe variable
 * \param dialect Spice dialect used
 * \return Current probe variable in Ngspice or Xyce notation
 */
QString iProbe::getProbeVariable(spicecompat::SpiceDialect dialect)
{
    QString s;
    if (dialect == spicecompat::SPICEXyce) {
        s = QStringLiteral("I(V%1)").arg(Name);
    } else {
        s = QStringLiteral("V%1#branch").arg(Name);
    }
    return s;
}
