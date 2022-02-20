/***************************************************************************
                              twistedpair.cpp
                             -----------------
    begin                : Dec 19 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "twistedpair.h"


TwistedPair::TwistedPair()
{
  Description = QObject::tr("twisted pair transmission line");

  Arcs.append(new qucs::Arc(-25,-36, 18, 38,16*230, 16*68,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(-25, -2, 18, 38, 16*62, 16*68,QPen(Qt::darkBlue,1)));

  Arcs.append(new qucs::Arc(-17,-36, 18, 38,16*242, 16*56,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(-17, -2, 18, 38, 16*62, 16*56,QPen(Qt::darkBlue,1)));

  Arcs.append(new qucs::Arc( -9,-36, 18, 38,16*242, 16*56,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc( -9, -2, 18, 38, 16*62, 16*56,QPen(Qt::darkBlue,1)));

  Arcs.append(new qucs::Arc( -1,-36, 18, 38,16*242, 16*56,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc( -1, -2, 18, 38, 16*62, 16*56,QPen(Qt::darkBlue,1)));

  Arcs.append(new qucs::Arc(  7,-36, 18, 38,16*242, 16*68,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(  7, -2, 18, 38, 16*50, 16*68,QPen(Qt::darkBlue,1)));

  Arcs.append(new qucs::Arc(-40,-10, 20, 33, 16*32, 16*58,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-40,-23, 20, 33,16*270, 16*58,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 20,-10, 20, 33, 16*90, 16*58,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 20,-23, 20, 33,16*212, 16*58,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,-10));
  Ports.append(new Port( 30,-10));
  Ports.append(new Port( 30, 10));
  Ports.append(new Port(-30, 10));

  x1 = -30; y1 =-12;
  x2 =  30; y2 = 12;

  tx = x1+4;
  ty = y2+4;
  Model = "TWIST";
  Name  = "Line";

  Props.append(new Property("d", "0.5 mm", true,
		QObject::tr("diameter of conductor")));
  Props.append(new Property("D", "0.8 mm", true,
		QObject::tr("diameter of wire (conductor and insulator)")));
  Props.append(new Property("L", "1.5", true,
		QObject::tr("physical length of the line")));
  Props.append(new Property("T", "100", false,
		QObject::tr("twists per length in 1/m")));
  Props.append(new Property("er", "4", false,
		QObject::tr("dielectric constant of insulator")));
  Props.append(new Property("mur", "1", false,
		QObject::tr("relative permeability of conductor")));
  Props.append(new Property("rho", "0.022e-6", false,
		QObject::tr("specific resistance of conductor")));
  Props.append(new Property("tand", "4e-4", false,
		QObject::tr("loss tangent")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

TwistedPair::~TwistedPair()
{
}

Component* TwistedPair::newOne()
{
  return new TwistedPair();
}

Element* TwistedPair::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Twisted-Pair");
  BitmapFile = (char *) "twistedpair";

  if(getNewOne)  return new TwistedPair();
  return 0;
}
