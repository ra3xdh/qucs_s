/***************************************************************************
                          iprobe.cpp  -  description
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

#include "iprobe.h"


iProbe::iProbe()
{
  Description = QObject::tr("current probe");

  Lines.append(new Line(-30,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  0, 20,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  4,  0, -4, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  4,  0, -4,  4,QPen(Qt::darkBlue,3)));

  Lines.append(new Line(-20,-31, 20,-31,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  9, 20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-31,-20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-31, 20,  9,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  Arcs.append(new Arc(-20,-23, 39, 39, 16*50, 16*80,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-11,-24, -2, -9,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -34;
  x2 =  30; y2 =  12;

  tx = x1+4;
  ty = y2+4;
  Model = "IProbe";
  Name  = "Pr";
}

iProbe::~iProbe()
{
}

Component* iProbe::newOne()
{
  return new iProbe();
}

Element* iProbe::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Current Probe");
  BitmapFile = (char *) "iprobe";

  if(getNewOne)  return new iProbe();
  return 0;
}
