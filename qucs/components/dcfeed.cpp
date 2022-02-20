/***************************************************************************
                          dcfeed.cpp  -  description
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

#include "dcfeed.h"


dcFeed::dcFeed()
{
  Description = QObject::tr("dc feed");

  Arcs.append(new qucs::Arc(-17, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -6, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  5, -6, 12, 12,  0, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-17,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 17,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-23,-13, 23,-13,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-23, 13, 23, 13,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-23,-13,-23, 13,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 23,-13, 23, 13,QPen(Qt::darkBlue,1)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -15;
  x2 =  30; y2 =  16;

  tx = x1+4;
  ty = y2+4;
  Model = "DCFeed";
  Name  = "L";

  Props.append(new Property("L", "1 uH", false,
	QObject::tr("for transient simulation: inductance in Henry")));
}

dcFeed::~dcFeed()
{
}

Component* dcFeed::newOne()
{
  return new dcFeed();
}

Element* dcFeed::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Feed");
  BitmapFile = (char *) "dcfeed";

  if(getNewOne)  return new dcFeed();
  return 0;
}
