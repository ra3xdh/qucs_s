/***************************************************************************
                               noise_iv.cpp
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

#include "noise_iv.h"
#include "extsimkernels/spicecompat.h"


Noise_iv::Noise_iv()
{
  Description = QObject::tr("correlated current sources");
  Simulator = spicecompat::simQucsator;

  // left noise source
  Ellipses.append(new qucs::Ellips(-42,-12, 24, 24, QPen(Qt::darkBlue,2)));
  // pins
  Lines.append(new qucs::Line(-30, 30,-30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,-30,-30,-12,QPen(Qt::darkBlue,2)));
  // arrow
  Lines.append(new qucs::Line( -30,  7,  -30,  -6,QPen(Qt::darkBlue,3, Qt::SolidLine, Qt::FlatCap)));
  Polylines.append(new qucs::Polyline(
    std::vector<QPointF>{{-34, 0},{-30, -6}, {-26, 0}}, QPen(Qt::darkBlue, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));
  // diagonal strokes
  Lines.append(new qucs::Line(-29, 12,-42, -1,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line(-24, 10,-27,  7,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line(-37, -3,-40, -6,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line(-20,  7,-25,  2,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line(-34, -7,-37,-10,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line(-18,  1,-31,-12,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));

  // right noise source
  Ellipses.append(new qucs::Ellips( 18,-12, 24, 24, QPen(Qt::darkBlue,2)));
  // pins
  Lines.append(new qucs::Line( 30, 30, 30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,-30, 30,-12,QPen(Qt::darkBlue,2)));
  // diagonal strokes
  Lines.append(new qucs::Line( 31, 12, 18, -1,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line( 36, 10, 20, -6,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line( 40,  7, 23,-10,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.append(new qucs::Line( 42,  2, 28,-12,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  // horizontal connecting bar
  Lines.append(new qucs::Line(-18,  0, 18,  0,QPen(Qt::darkBlue,3, Qt::SolidLine, Qt::FlatCap)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -44; y1 = -30;
  x2 =  44; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "IVnoise";
  Name  = "SRC";

  Props.append(new Property("i1", "1e-6", true,
		QObject::tr("current power spectral density of source 1")));
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

Noise_iv::~Noise_iv()
{
}

Component* Noise_iv::newOne()
{
  return new Noise_iv();
}

Element* Noise_iv::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Correlated Noise Sources");
  BitmapFile = (char *) "noise_iv";

  if(getNewOne)  return new Noise_iv();
  return 0;
}
