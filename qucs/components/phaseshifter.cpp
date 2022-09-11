/***************************************************************************
                          phaseshifter.cpp  -  description
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

#include "phaseshifter.h"


Phaseshifter::Phaseshifter()
{
  Description = QObject::tr("phase shifter");

  Lines.append(new qucs::Line(-14,-14, 14,-14,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14, 14, 14, 14,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14,-14,-14, 14,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 14,-14, 14, 14,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -9, -9, 17, 17, 0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 10, 10,-10,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 14,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -17;
  x2 =  30; y2 =  17;

  tx = x1+4;
  ty = y2+4;
  Model = "PShift";
  Name  = "X";

  Props.append(new Property("phi", "90", true,
		QObject::tr("phase shift in degree")));
  Props.append(new Property("Zref", "50 Ohm", false,
		QObject::tr("reference impedance")));
}

Phaseshifter::~Phaseshifter()
{
}

Component* Phaseshifter::newOne()
{
  return new Phaseshifter();
}

Element* Phaseshifter::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Phase Shifter");
  BitmapFile = (char *) "pshifter";

  if(getNewOne)  return new Phaseshifter();
  return 0;
}
