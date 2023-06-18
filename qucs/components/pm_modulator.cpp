/***************************************************************************
                               pm_modulator.cpp
                              ------------------
    begin                : Sat Feb 25 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "pm_modulator.h"
#include "extsimkernels/spicecompat.h"


PM_Modulator::PM_Modulator()
{
  Description = QObject::tr("ac voltage source with phase modulator");
  Simulator = spicecompat::simQucsator;

  Arcs.append(new qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -7, -4,  7,  7,     0, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  0, -4,  7,  7,16*180, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 30,  0, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,-30,  0,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  5,-18, 11,-18,QPen(Qt::red,1)));
  Lines.append(new qucs::Line(  8,-21,  8,-15,QPen(Qt::red,1)));
  Lines.append(new qucs::Line(  5, 18, 11, 18,QPen(Qt::black,1)));

  Lines.append(new qucs::Line(-12,  0,-30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12,  0,-17,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-12,  0,-17, -5,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-30,-22, QObject::tr("PM"), Qt::black, 10.0,1.0,0.0));

  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -30;
  x2 =  14; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "PM_Mod";
  Name  = "V";

  Props.append(new Property("U", "1 V", true,
		QObject::tr("peak voltage in Volts")));
  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("Phase", "0", false,
		QObject::tr("initial phase in degrees")));
  Props.append(new Property("M", "1.0", false,
		QObject::tr("modulation index")));
}

PM_Modulator::~PM_Modulator()
{
}

Component* PM_Modulator::newOne()
{
  return new PM_Modulator();
}

Element* PM_Modulator::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("PM modulated Source");
  BitmapFile = (char *) "pm_mod";

  if(getNewOne)  return new PM_Modulator();
  return 0;
}
