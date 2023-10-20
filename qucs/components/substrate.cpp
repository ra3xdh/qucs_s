/***************************************************************************
                          substrate.cpp  -  description
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

#include "substrate.h"
#include "extsimkernels/spicecompat.h"


Substrate::Substrate()
{
  Description = QObject::tr("substrate definition");
  Simulator = spicecompat::simQucsator;

  Lines.append(new qucs::Line(-30,-16, 30,-16,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,-12, 30,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 16, 30, 16,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 12, 30, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,-16,-30, 16,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,-16, 30, 16,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-30,-16, 16,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,-16, 80,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,-12, 80,-36,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, 12, 80,-16,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, 16, 80,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 16,-40, 80,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 80,-40, 80,-12,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(-30,  0,-18,-12,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-22, 12,  2,-12,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( -2, 12, 22,-12,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 18, 12, 30,  0,QPen(Qt::darkBlue,1)));

  Lines.append(new qucs::Line( 30,  1, 37,  8,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 37,-15, 52,  0,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 52,-22, 66, -8,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( 66,-30, 80,-16,QPen(Qt::darkBlue,1)));

  x1 = -34; y1 =-44;
  x2 =  84; y2 = 20;

  tx = x1+4;
  ty = y2+4;
  icon_dx = -18;
  Model = "SUBST";
  Name  = "Subst";

  Props.append(new Property("er", "9.8", true,
		QObject::tr("relative permittivity")));
  Props.append(new Property("h", "1 mm", true,
		QObject::tr("thickness in meters")));
  Props.append(new Property("t", "35 um", true,
		QObject::tr("thickness of metalization")));
  Props.append(new Property("tand", "2e-4", true,
		QObject::tr("loss tangent")));
  Props.append(new Property("rho", "0.022e-6", true,
		QObject::tr("specific resistance of metal")));
  Props.append(new Property("D", "0.15e-6", true,
		QObject::tr("rms substrate roughness")));
}

Substrate::~Substrate()
{
}

Component* Substrate::newOne()
{
  return new Substrate();
}

Element* Substrate::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Substrate");
  BitmapFile = (char *) "substrate";

  if(getNewOne)  return new Substrate();
  return 0;
}
