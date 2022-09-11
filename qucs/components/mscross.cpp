/***************************************************************************
                          mscross.cpp  -  description
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

#include "mscross.h"


MScross::MScross()
{
  Description = QObject::tr("microstrip cross");

  Model = "MCROSS";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
		QObject::tr("substrate")));
  Props.append(new Property("W1", "1 mm", true,
		QObject::tr("width of line 1")));
  Props.append(new Property("W2", "2 mm", true,
		QObject::tr("width of line 2")));
  Props.append(new Property("W3", "1 mm", true,
		QObject::tr("width of line 3")));
  Props.append(new Property("W4", "2 mm", true,
		QObject::tr("width of line 4")));
  Props.append(new Property("MSModel", "Hammerstad", false,
	QObject::tr("quasi-static microstrip model")+
	" [Hammerstad, Wheeler, Schneider]"));
  Props.append(new Property("MSDispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+" [Kirschning, Kobayashi, "
	"Yamashita, Hammerstad, Getsinger, Schneider, Pramanick]"));
  Props.append(new Property("Symbol", "showNumbers", false,
	QObject::tr("show port numbers in symbol or not")+
	" [showNumbers, noNumbers]"));

  createSymbol();
}

MScross::~MScross()
{
}

Component* MScross::newOne()
{
  return new MScross();
}

Element* MScross::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Cross");
  BitmapFile = (char *) "mscross";

  if(getNewOne)  return new MScross();
  return 0;
}

void MScross::createSymbol()
{
  Lines.append(new qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 18,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,-30,  0,-18,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-18, -8, -8, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-18,  8, -8,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-18, -8,-18,  8,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(  8, -8, 18, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  8,  8, 18,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18, -8, 18,  8,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( -8,  8, -8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  8,  8,  8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -8, 18,  8, 18,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( -8,-18, -8, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  8,-18,  8, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -8,-18,  8,-18,QPen(Qt::darkBlue,2)));

  if(Props.getLast()->Value.at(0) != 'n') {
    Texts.append(new Text(-26,  3, "1"));
    Texts.append(new Text(-10,-30, "2"));
    Texts.append(new Text( 21,-13, "3"));
    Texts.append(new Text(  4, 18, "4"));
  }

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 =-30;
  x2 =  30; y2 = 30;

  tx = x1+4;
  ty = y2+4;
}
