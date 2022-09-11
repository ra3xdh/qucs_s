/***************************************************************************
                               coupler.cpp
                              -------------
    begin                : Tue Jan 03 2006
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

#include "coupler.h"


Coupler::Coupler()
{
  Description = QObject::tr("ideal coupler");

  Lines.append(new qucs::Line(-23,-24, 23,-24,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 23,-24, 23, 24,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line( 23, 24,-23, 24,QPen(Qt::darkGray,1)));
  Lines.append(new qucs::Line(-23, 24,-23,-24,QPen(Qt::darkGray,1)));

  Lines.append(new qucs::Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,-20, 20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,-20, 20,-20,QPen(Qt::darkBlue,4)));
  Lines.append(new qucs::Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, 20, 20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20, 20, 20, 20,QPen(Qt::darkBlue,4)));

  Lines.append(new qucs::Line( 14, 14,-14,-14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-14,-14, -9,-14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-14,-14,-14, -9,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(  9, 14, 14, 14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 14,  9, 14, 14,QPen(Qt::darkBlue,1)));
  
  Lines.append(new qucs::Line( 14,-14,-14, 14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 14,-14,  9,-14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 14,-14, 14, -9,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-14, 14, -9, 14,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-14, 14,-14,  9,QPen(Qt::darkBlue,1)));

  Ports.append(new Port(-30,-20));
  Ports.append(new Port( 30,-20));
  Ports.append(new Port( 30, 20));
  Ports.append(new Port(-30, 20));


  x1 = -30; y1 = -25;
  x2 =  30; y2 =  25;

  tx = x1+4;
  ty = y2+4;
  Model = "Coupler";
  Name  = "X";

  Props.append(new Property("k", "0.7071", true,
		QObject::tr("coupling factor")));
  Props.append(new Property("phi", "180", true,
		QObject::tr("phase shift of coupling path in degree")));
  Props.append(new Property("Z", "50 Ohm", false,
		QObject::tr("reference impedance")));
}

Coupler::~Coupler()
{
}

Component* Coupler::newOne()
{
  return new Coupler();
}

Element* Coupler::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coupler");
  BitmapFile = (char *) "coupler";

  if(getNewOne)  return new Coupler();
  return 0;
}
