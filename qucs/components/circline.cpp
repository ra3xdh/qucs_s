/*
 * circline.cpp
 *
 * copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */

#include "circline.h"
#include "extsimkernels/spicecompat.h"


CircLine::CircLine()
{
  Description = QObject::tr("Circular Waveguide");
  Simulator = spicecompat::simQucsator;

  Arcs.append(new qucs::Arc(-20, -9, 18, 18,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-18, -7, 14, 14,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 4, -9, 18, 18,16*270, 16*180,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 19,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12, -9, 12, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12,  9, 12,  9,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-16;
  x2 =  30; y2 = 14;

  tx = x1+4;
  ty = y2+4;
  Model = "CIRCLINE";
  Name  = "Line";

  Props.append(new Property("a", "2.95 mm", true,
		QObject::tr("Radius")));
  Props.append(new Property("L", "1500 mm", true,
		QObject::tr("Mechanical length of the line")));
  Props.append(new Property("er", "1", false,
		QObject::tr("Relative permittivity of dielectric")));
  Props.append(new Property("mur", "1", false,
		QObject::tr("Relative permeability of conductor")));
  Props.append(new Property("tand", "0", false,
		QObject::tr("Loss tangent")));
  Props.append(new Property("rho", "0.022e-6", false,
		QObject::tr("Specific resistance of conductor")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("Simulation temperature in degree Celsius")));
  Props.append(new Property("Material", "unspecified", false,
		QObject::tr("Material parameter for temperature model")+
			    " [unspecified, Copper, StainlessSteel, Gold]"));
}

CircLine::~CircLine()
{
}

Component* CircLine::newOne()
{
  return new CircLine();
}

Element* CircLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Circular Waveguide");
  BitmapFile = (char *) "circline";

  if(getNewOne)  return new CircLine();
  return 0;
}
