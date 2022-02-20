/***************************************************************************
                               noise_vv.cpp
                              --------------
    begin                : Sat Aug 20 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include "noise_vv.h"


Noise_vv::Noise_vv()
{
  Description = QObject::tr("correlated current sources");

  // left noise source
  Arcs.append(new qucs::Arc(-42,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 30,-30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,-30,-30,-12,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-29, 12,-42, -1,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-24, 10,-40, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,  7,-37,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-18,  2,-32,-12,QPen(Qt::darkBlue,2)));

  // right noise source
  Arcs.append(new qucs::Arc( 18,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, 30, 30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,-30, 30,-12,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( 31, 12, 18, -1,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 36, 10, 20, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 40,  7, 23,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 42,  2, 28,-12,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-18,  0, 18,  0,QPen(Qt::darkBlue,3)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -44; y1 = -30;
  x2 =  44; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "VVnoise";
  Name  = "SRC";

  Props.append(new Property("v1", "1e-6", true,
		QObject::tr("voltage power spectral density of source 1")));
  Props.append(new Property("v2", "1e-6", true,
		QObject::tr("voltage power spectral density of source 2")));
  Props.append(new Property("C", "0.5", true,
		QObject::tr("normalized correlation coefficient")));
  Props.append(new Property("e", "0", false,
		QObject::tr("frequency exponent")));
  Props.append(new Property("c", "1", false,
		QObject::tr("frequency coefficient")));
  Props.append(new Property("a", "0", false,
		QObject::tr("additive frequency term")));
}

Noise_vv::~Noise_vv()
{
}

Component* Noise_vv::newOne()
{
  return new Noise_vv();
}

Element* Noise_vv::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Correlated Noise Sources");
  BitmapFile = (char *) "noise_vv";

  if(getNewOne)  return new Noise_vv();
  return 0;
}
