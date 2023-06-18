/***************************************************************************
                        vcresistor.cpp  -  description
                             -------------------
    begin                : Feb 13 2014
    copyright            : (C) 2014 by Richard Crozier
    email                : richard dot crozier at yahoo dot co dot uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "vcresistor.h"
#include "extsimkernels/spicecompat.h"


vcresistor::vcresistor()
{
  Description = QObject::tr("voltage controlled resistor");
  Simulator = spicecompat::simQucsator;

  // The resistor shape
  Lines.append(new qucs::Line(5, 18, 5, -18, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(17, 18, 17, -18, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(5, 18, 17, 18, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(5, -18, 17, -18, QPen(Qt::darkBlue,2)));

  // horizontal lines on top and bottom of left hand side
  Lines.append(new qucs::Line(-30,-30,-12,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 30,-12, 30,QPen(Qt::darkBlue,2)));
  // horizontal lines on top and bottom of right hand side
  Lines.append(new qucs::Line( 11,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11, 30, 30, 30,QPen(Qt::darkBlue,2)));
  // vertical lines on top and bottom of left hand side
  Lines.append(new qucs::Line(-12,-30,-12,-23,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12, 30,-12, 23,QPen(Qt::darkBlue,2)));
  // vertical lines on top and bottom of right hand side
  Lines.append(new qucs::Line( 11,-30, 11,-18,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 11, 30, 11, 18,QPen(Qt::darkBlue,2)));

  // downward pointing arrow
  Lines.append(new qucs::Line(-12,-18,-12, 18,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-12, 18,-17,  9,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-12, 18, -7,  9,QPen(Qt::darkBlue,1)));

  Lines.append(new qucs::Line(-25,-27, 25,-27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 25,-27, 25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 25, 27,-25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line(-25, 27,-25,-27,QPen(Qt::darkGray,1)));


  Ports.append(new Port(-30,-30));
  Ports.append(new Port(-30, 30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));


  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "vcresistor";
  Name  = "VCR";

  Props.append(new Property("gain", "1", true,
		QObject::tr("resistance gain")));
}

vcresistor::~vcresistor()
{
}

Component* vcresistor::newOne()
{
  return new vcresistor();
}

Element* vcresistor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Voltage Controlled Resistor");
  BitmapFile = (char *) "vcresistor";

  if(getNewOne)  return new vcresistor();
  return 0;
}

