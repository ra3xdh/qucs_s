/***************************************************************************
                                  diac.cpp
                                 ----------
    begin                : Sun Dec 23 2007
    copyright            : (C) 2007 by Michael Margraf
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

#include "diac.h"


Diac::Diac()
{
  Description = QObject::tr("diac (bidirectional trigger diode)");

  Lines.append(new qucs::Line(  0,-30,  0, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 30,  0,  6,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-18,  6, 18,  6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-18, -6, 18, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -9,  6,-18, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -9,  6,  0, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  9, -6,  0,  6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  9, -6, 18,  6,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));

  x1 = -20; y1 = -30;
  x2 =  20; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "Diac";
  Name  = "D";

  Props.append(new Property("Vbo", "30 V", true,
	QObject::tr("(bidirectional) breakover voltage")));
  Props.append(new Property("Ibo", "50 uA", false,
	QObject::tr("(bidirectional) breakover current")));
  Props.append(new Property("Cj0", "10 pF", false,
	QObject::tr("parasitic capacitance")));
  Props.append(new Property("Is", "1e-10 A", false,
	QObject::tr("saturation current")));
  Props.append(new Property("N", "2", false,
	QObject::tr("emission coefficient")));
  Props.append(new Property("Ri", "10 Ohm", false,
	QObject::tr("intrinsic junction resistance")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature")));
}

Component* Diac::newOne()
{
  return new Diac();
}

Element* Diac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Diac");
  BitmapFile = (char *) "diac";

  if(getNewOne)  return new Diac();
  return 0;
}
