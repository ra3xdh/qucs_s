/***************************************************************************
                       msrstub.cpp  -  description
                      -----------------------------
    begin                : Sun Mar 15 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "msrstub.h"

MSrstub::MSrstub()
{
  Description = QObject::tr("microstrip radial stub");

  Arcs.append(new qucs::Arc( -26, -26, 52, 52,16*45, 16*90,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-5,   0,  5,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-5,   0,-18,-18,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 5,   0, 18,-18,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 0,   0,  0, 10,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(0, 10));

  x1 = -22; y1 =-30;
  x2 =  22; y2 = 10;

  tx = x1+4;
  ty = y2+4;
  Model = "MRSTUB";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.append(new Property("ri", "1 mm", false,
	QObject::tr("inner radius")));
  Props.append(new Property("ro", "10 mm", true,
	QObject::tr("outer radius")));
  Props.append(new Property("alpha", "90", true,
	QObject::tr("stub angle")+" ("+QObject::tr ("degrees")+")"));
}

MSrstub::~MSrstub()
{
}

Component* MSrstub::newOne()
{
  return new MSrstub();
}

Element* MSrstub::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Radial Stub");
  BitmapFile = (char *) "msrstub";

  if(getNewOne)  return new MSrstub();
  return 0;
}
