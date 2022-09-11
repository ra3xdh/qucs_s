/***************************************************************************
                              rectline.cpp
                             -----------------
    copyright            : (C) 2008 by ROUCARIES Bastien
    email                : roucaries.bastien@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "rectline.h"


RectLine::RectLine()
{
  Description = QObject::tr("Rectangular Waveguide");

  Lines.append(new qucs::Line(-30,  0,-17,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-14, -7, 18, -7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14, -7,-14, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-14, 11, 18, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18, -7, 18, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,-13, 12,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,-13,-20,  5,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-20,-13,-14, -7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 12,-13, 18, -7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,  5,-14, 11,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-16;
  x2 =  30; y2 = 14;

  tx = x1+4;
  ty = y2+4;
  Model = "RECTLINE";
  Name  = "Line";

  Props.append(new Property("a", "2.95 mm", true,
		QObject::tr("widest side")));
  Props.append(new Property("b", "0.9 mm", true,
		QObject::tr("shortest side")));
  Props.append(new Property("L", "1500 mm", true,
		QObject::tr("mechanical length of the line")));
  Props.append(new Property("er", "1", false,
		QObject::tr("relative permittivity of dielectric")));
  Props.append(new Property("mur", "1", false,
		QObject::tr("relative permeability of conductor")));
  Props.append(new Property("tand", "0", false,
		QObject::tr("loss tangent")));
  Props.append(new Property("rho", "0.022e-6", false,
		QObject::tr("specific resistance of conductor")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("Material", "unspecified", false,
		QObject::tr("material parameter for temperature model")+
			    " [unspecified, Copper, StainlessSteel, Gold]"));
}

RectLine::~RectLine()
{
}

Component* RectLine::newOne()
{
  return new RectLine();
}

Element* RectLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Rectangular Waveguide");
  BitmapFile = (char *) "rectline";

  if(getNewOne)  return new RectLine();
  return 0;
}
