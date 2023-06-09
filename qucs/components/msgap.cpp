/***************************************************************************
                          msgap.cpp  -  description
                             -------------------
    begin                : Sat Jul 17 2004
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

#include "msgap.h"
#include "extsimkernels/spicecompat.h"


MSgap::MSgap()
{
  Description = QObject::tr("microstrip gap");
  Simulator = spicecompat::simQucsator;

  Lines.append(new qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-13, -8,  0, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10, -8, 23, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-23,  8,-10,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,  8, 13,  8,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 23, -8, 13,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, -8,-10,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10, -8,  0,  8,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-22, -4,-26,  4,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-11;
  x2 =  30; y2 = 11;

  tx = x1+4;
  ty = y2+4;
  Model = "MGAP";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.append(new Property("W1", "1 mm", true,
	QObject::tr("width of the line 1")));
  Props.append(new Property("W2", "1 mm", true,
	QObject::tr("width of the line 2")));
  Props.append(new Property("S", "1 mm", true,
	QObject::tr("spacing between the microstrip ends")));
  Props.append(new Property("MSModel", "Hammerstad", false,
	QObject::tr("quasi-static microstrip model")+
	" [Hammerstad, Wheeler, Schneider]"));
  Props.append(new Property("MSDispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+" [Kirschning, Kobayashi, "
	"Yamashita, Hammerstad, Getsinger, Schneider, Pramanick]"));
}

MSgap::~MSgap()
{
}

Component* MSgap::newOne()
{
  return new MSgap();
}

Element* MSgap::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Gap");
  BitmapFile = (char *) "msgap";

  if(getNewOne)  return new MSgap();
  return 0;
}
