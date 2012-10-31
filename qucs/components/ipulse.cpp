/***************************************************************************
                                ipulse.cpp
                               ------------
    begin                : Sat Sep 18 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "ipulse.h"


iPulse::iPulse()
{
  Description = QObject::tr("ideal current pulse source");

  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));

  // little pulse symbol
  Lines.append(new Line( 13,  7, 13, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 19, 10, 19, 14,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 13, 14, 13, 17,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 13, 10, 19, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 13, 14, 19, 14,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  20;

  tx = x1+4;
  ty = y2+4;
  Model = "Ipulse";
  Name  = "I";

  Props.append(new Property("I1", "0", true,
		QObject::tr("current before and after the pulse")));
  Props.append(new Property("I2", "1 A", true,
		QObject::tr("current of the pulse")));
  Props.append(new Property("T1", "0", true,
		QObject::tr("start time of the pulse")));
  Props.append(new Property("T2", "1 ms", true,
		QObject::tr("ending time of the pulse")));
  Props.append(new Property("Tr", "1 ns", false,
		QObject::tr("rise time of the leading edge")));
  Props.append(new Property("Tf", "1 ns", false,
		QObject::tr("fall time of the trailing edge")));

  rotate();  // fix historical flaw
}

iPulse::~iPulse()
{
}

Component* iPulse::newOne()
{
  return new iPulse();
}

Element* iPulse::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Current Pulse");
  BitmapFile = (char *) "ipulse";

  if(getNewOne)  return new iPulse();
  return 0;
}
