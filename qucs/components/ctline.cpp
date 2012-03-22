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

#include "ctline.h"

CoupledTLine::CoupledTLine()
{
  Description = QObject::tr("coupled transmission lines");

  Arcs.append(new Arc(-28,-40, 18, 38,16*232, 16*33,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc(-28,  2, 18, 38, 16*95, 16*33,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-20,-2,-16,-2,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 2,-16, 2,QPen(Qt::darkBlue,2)));

  Arcs.append(new Arc( 10,-40, 18, 38,16*270, 16*40,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( 10,  2, 18, 38, 16*50, 16*40,QPen(Qt::darkBlue,2)));

  Arcs.append(new Arc(-38,-10, 16, 28, 16*45, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc(-38,-18, 16, 28,16*270, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( 22,-10, 16, 28, 16*90, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( 22,-18, 16, 28,16*225, 16*45,QPen(Qt::darkBlue,2)));

  // shield
  Arcs.append(new Arc(-20, -9, 8, 18,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( 11, -9, 8, 18,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,  9, 16,  9,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,-10));
  Ports.append(new Port( 30,-10));
  Ports.append(new Port( 30, 10));
  Ports.append(new Port(-30, 10));

  x1 = -30; y1 =-12;
  x2 =  30; y2 = 12;

  tx = x1+4;
  ty = y2+4;
  Model = "CTLIN";
  Name  = "Line";

  Props.append(new Property("Ze", "50 Ohm", true,
		QObject::tr("characteristic impedance of even mode")));
  Props.append(new Property("Zo", "50 Ohm", true,
		QObject::tr("characteristic impedance of odd mode")));
  Props.append(new Property("L", "1 mm", true,
		QObject::tr("electrical length of the line")));
  Props.append(new Property("Ere", "1", false,
		QObject::tr("relative dielectric constant of even mode")));
  Props.append(new Property("Ero", "1", false,
		QObject::tr("relative dielectric constant of odd mode")));
  Props.append(new Property("Ae", "0 dB", false,
		QObject::tr("attenuation factor per length of even mode")));
  Props.append(new Property("Ao", "0 dB", false,
		QObject::tr("attenuation factor per length of odd mode")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}


// -------------------------------------------------------------------
Component* CoupledTLine::newOne()
{
  return new CoupledTLine();
}

Element* CoupledTLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coupled Transmission Line");
  BitmapFile = (char *) "ctline";

  if(getNewOne)  return new CoupledTLine();
  return 0;
}
