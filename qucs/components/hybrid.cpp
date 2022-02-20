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

#include "hybrid.h"

Hybrid::Hybrid()
{
  Description = QObject::tr("hybrid (unsymmetrical 3dB coupler)");

  Lines.append(new qucs::Line(-14,-14, 14,-14,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14, 14, 14, 14,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14,-14,-14, 14,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 14,-14, 14, 14,QPen(Qt::darkBlue,2)));

  Arcs.append(new qucs::Arc(-28,-28, 28, 28, 16*270, 16*90,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(  0,-28, 28, 28, 16*180, 16*90,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(-28,  0, 28, 28,      0, 16*90,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(  0,  0, 28, 28,  16*90, 16*90,QPen(Qt::darkBlue,1)));

  Arcs.append(new qucs::Arc(-11,-11, 4, 6, 0, 16*360,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(-11,  5, 4, 6, 0, 16*360,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(  6,-11, 4, 6, 0, 16*360,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(  6,  5, 4, 6, 0, 16*360,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 8, -12, 8, -4,QPen(Qt::darkBlue,1)));

  Lines.append(new qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 14,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,-30,  0,-14,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 14,  0, 30,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0, 30));
  Ports.append(new Port(  0,-30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "Hybrid";
  Name  = "X";

  Props.append(new Property("phi", "90", true,
		QObject::tr("phase shift in degree")));
  Props.append(new Property("Zref", "50 Ohm", false,
		QObject::tr("reference impedance")));
}

// -------------------------------------------------------------------
Component* Hybrid::newOne()
{
  return new Hybrid();
}

Element* Hybrid::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Hybrid");
  BitmapFile = (char *) "hybrid";

  if(getNewOne)  return new Hybrid();
  return 0;
}
