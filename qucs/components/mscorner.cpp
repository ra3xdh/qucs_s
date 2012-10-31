/***************************************************************************
                          mscorner.cpp  -  description
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

#include "mscorner.h"


MScorner::MScorner()
{
  Description = QObject::tr("microstrip corner");

  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 18,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18, -8,  8, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18,  8, -8,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18, -8,-18,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8,  8, -8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  8, -8,  8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8, 18,  8, 18,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port(  0,30));

  x1 = -30; y1 =-11;
  x2 =  11; y2 = 30;

  tx = x2+4;
  ty = y1+4;
  Model = "MCORN";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
		QObject::tr("substrate")));
  Props.append(new Property("W", "1 mm", true,
		QObject::tr("width of line")));
}

MScorner::~MScorner()
{
}

Component* MScorner::newOne()
{
  return new MScorner();
}

Element* MScorner::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Corner");
  BitmapFile = (char *) "mscorner";

  if(getNewOne)  return new MScorner();
  return 0;
}
