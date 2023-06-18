/***************************************************************************
                               cpwshort.cpp
                             ----------------
    begin                : Sun May 01 2005
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

#include "cpwshort.h"
#include "extsimkernels/spicecompat.h"


CPWshort::CPWshort()
{
  Description = QObject::tr("coplanar short");
  Simulator = spicecompat::simQucsator;

  Lines.append(new qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-13, -8,  3, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-23,  8, -7,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-25,-13,  6,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-25, 13,-10, 13,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  6,-13,  3, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -7,  8,-10, 13,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-24,-21,-16,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-16,-21, -8,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -8,-21,  0,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,-21, 12, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  8,-21, 12,-17,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(  4, -9, 12, -1,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  1, -4, 12,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -2,  1, 12, 15,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-25, 18,-22, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-22, 13,-14, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14, 13, -6, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -8, 11,  2, 21,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -5,  6, 10, 21,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));

  x1 = -30; y1 =-24;
  x2 =  14; y2 = 24;

  tx = x1+4;
  ty = y2+4;
  Model = "CSHORT";
  Name  = "CL";

  Props.append(new Property("Subst", "Subst1", true,
		QObject::tr("name of substrate definition")));
  Props.append(new Property("W", "1 mm", true,
		QObject::tr("width of the line")));
  Props.append(new Property("S", "1 mm", true,
		QObject::tr("width of a gap")));
  Props.append(new Property("Backside", "Air", false,
		QObject::tr("material at the backside of the substrate")+
		" [Metal, Air]"));
}

CPWshort::~CPWshort()
{
}

Component* CPWshort::newOne()
{
  return new CPWshort();
}

Element* CPWshort::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coplanar Short");
  BitmapFile = (char *) "cpwshort";

  if(getNewOne)  return new CPWshort();
  return 0;
}
