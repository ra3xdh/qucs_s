/***************************************************************************
                              tline_4port.cpp
                             -----------------
    begin                : Nov 12 2006
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

#include "tline_4port.h"


TLine_4Port::TLine_4Port()
{
  Description = QObject::tr("ideal 4-terminal transmission line");

  Arcs.append(new qucs::Arc(-28,-40, 18, 38,16*232, 16*33,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(-28,  2, 18, 38, 16*95, 16*33,QPen(Qt::darkBlue,1)));

  Lines.append(new qucs::Line(-20,-2, 20,-2,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20, 2, 20, 2,QPen(Qt::darkBlue,2)));

  Arcs.append(new qucs::Arc( 10,-40, 18, 38,16*270, 16*40,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc( 10,  2, 18, 38, 16*50, 16*40,QPen(Qt::darkBlue,1)));

  Arcs.append(new qucs::Arc(-38,-10, 16, 28, 16*45, 16*45,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc(-38,-18, 16, 28,16*270, 16*45,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc( 22,-10, 16, 28, 16*90, 16*45,QPen(Qt::darkBlue,1)));
  Arcs.append(new qucs::Arc( 22,-18, 16, 28,16*225, 16*45,QPen(Qt::darkBlue,1)));

  Ports.append(new Port(-30,-10));
  Ports.append(new Port( 30,-10));
  Ports.append(new Port( 30, 10));
  Ports.append(new Port(-30, 10));

  x1 = -30; y1 =-12;
  x2 =  30; y2 = 12;

  tx = x1+4;
  ty = y2+4;
  Model = "TLIN4P";
  Name  = "Line";

  Props.append(new Property("Z", "50 Ohm", true,
		QObject::tr("characteristic impedance")));
  Props.append(new Property("L", "1 mm", true,
		QObject::tr("electrical length of the line")));
  Props.append(new Property("Alpha", "0 dB", false,
		QObject::tr("attenuation factor per length in 1/m")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

TLine_4Port::~TLine_4Port()
{
}

Component* TLine_4Port::newOne()
{
  return new TLine_4Port();
}

Element* TLine_4Port::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4-Terminal Transmission Line");
  BitmapFile = (char *) "tline_4port";

  if(getNewOne)  return new TLine_4Port();
  return 0;
}
