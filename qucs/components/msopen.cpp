/***************************************************************************
                          msopen.cpp  -  description
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

#include "msopen.h"
#include "extsimkernels/spicecompat.h"


MSopen::MSopen()
{
  Description = QObject::tr("microstrip open");
  Simulator = spicecompat::simQucsator;

  Lines.append(new qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-13, -8, 13, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-23,  8,  3,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 13, -8,  3,  8,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));

  x1 = -30; y1 =-11;
  x2 =  16; y2 = 11;

  tx = x1+4;
  ty = y2+4;
  icon_dx = 6;
  Model = "MOPEN";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.append(new Property("W", "1 mm", true,
	QObject::tr("width of the line")));
  Props.append(new Property("MSModel", "Hammerstad", false,
	QObject::tr("quasi-static microstrip model")+
	" [Hammerstad, Wheeler, Schneider]"));
  Props.append(new Property("MSDispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+" [Kirschning, Kobayashi, "
	"Yamashita, Hammerstad, Getsinger, Schneider, Pramanick]"));
  Props.append(new Property("Model", "Kirschning", false,
	QObject::tr("microstrip open end model")+" [Kirschning, Hammerstad, "
	"Alexopoulos]"));
}

MSopen::~MSopen()
{
}

Component* MSopen::newOne()
{
  return new MSopen();
}

Element* MSopen::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Open");
  BitmapFile = (char *) "msopen";

  if(getNewOne)  return new MSopen();
  return 0;
}
