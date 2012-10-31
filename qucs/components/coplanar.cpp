/***************************************************************************
                               coplanar.cpp
                              --------------
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

#include "coplanar.h"


Coplanar::Coplanar()
{
  Description = QObject::tr("coplanar line");

  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-13, -8, 23, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-23,  8, 13,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 23, -8, 13,  8,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-25,-13, 25,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-21, 24,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  8,-21, 16,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-21,  8,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8,-21,  0,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-21, -8,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-24,-21,-16,-13,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line(-25, 13, 25, 13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-24, 13,-16, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, 13, -8, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8, 13,  0, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 13,  8, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  8, 13, 16, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16, 13, 24, 21,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-24;
  x2 =  30; y2 = 24;

  tx = x1+4;
  ty = y2+4;
  Model = "CLIN";
  Name  = "CL";

  Props.append(new Property("Subst", "Subst1", true,
		QObject::tr("name of substrate definition")));
  Props.append(new Property("W", "1 mm", true,
		QObject::tr("width of the line")));
  Props.append(new Property("S", "1 mm", true,
		QObject::tr("width of a gap")));
  Props.append(new Property("L", "10 mm", true,
		QObject::tr("length of the line")));
  Props.append(new Property("Backside", "Air", false,
		QObject::tr("material at the backside of the substrate")+
		" [Metal, Air]"));
  Props.append(new Property("Approx", "yes", false,
		QObject::tr("use approximation instead of precise equation")+
		" [yes, no]"));
}

Coplanar::~Coplanar()
{
}

Component* Coplanar::newOne()
{
  return new Coplanar();
}

Element* Coplanar::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coplanar Line");
  BitmapFile = (char *) "coplanar";

  if(getNewOne)  return new Coplanar();
  return 0;
}
