/***************************************************************************
                               coplanar.cpp
                              --------------
    begin                : Fri May 13 2005
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

#include "cpwstep.h"


CPWstep::CPWstep()
{
  Description = QObject::tr("coplanar step");

  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  6,-10, 24,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6, 10, 12, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-14, -6,  4, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-22,  6, -4,  6,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  6,-10,  4, -6,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6, 10, -4,  6,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-14, -6,-22,  6,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 24,-10, 12, 10,QPen(Qt::darkBlue,2)));

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

  Lines.append(new Line(-22, -4,-26,  4,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-24;
  x2 =  30; y2 = 24;

  tx = x1+4;
  ty = y2+4;
  Model = "CSTEP";
  Name  = "CL";

  Props.append(new Property("Subst", "Subst1", true,
		QObject::tr("name of substrate definition")));
  Props.append(new Property("W1", "1 mm", true,
		QObject::tr("width of line 1")));
  Props.append(new Property("W2", "2 mm", true,
		QObject::tr("width of line 2")));
  Props.append(new Property("S", "3 mm", true,
		QObject::tr("distance between ground planes")));
  Props.append(new Property("Backside", "Air", false,
		QObject::tr("material at the backside of the substrate")+
		" [Metal, Air]"));
}

CPWstep::~CPWstep()
{
}

Component* CPWstep::newOne()
{
  return new CPWstep();
}

Element* CPWstep::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coplanar Step");
  BitmapFile = (char *) "cpwstep";

  if(getNewOne)  return new CPWstep();
  return 0;
}
