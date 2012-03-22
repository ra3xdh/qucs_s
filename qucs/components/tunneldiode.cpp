/***************************************************************************
    copyright            : (C) 2008 by Michael Margraf
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

#include "tunneldiode.h"

TunnelDiode::TunnelDiode()
{
  Description = QObject::tr("resonance tunnel diode");

  Props.append(new Property("Ip", "4 mA", true,
	QObject::tr("peak current")));
  Props.append(new Property("Iv", "0.6 mA", true,
	QObject::tr("valley current")));
  Props.append(new Property("Vv", "0.8 V", true,
	QObject::tr("valley voltage")));
  Props.append(new Property("Wr", "2.7e-20", false,
	QObject::tr("resonance energy in Ws")));
  Props.append(new Property("eta", "1e-20", false,
	QObject::tr("Fermi energy in Ws")));
  Props.append(new Property("dW", "4.5e-21", false,
	QObject::tr("resonance width in Ws")));
  Props.append(new Property("Tmax", "0.95", false,
	QObject::tr("maximum of transmission")));
  Props.append(new Property("de", "0.9", false,
	QObject::tr("fitting factor for electron density")));
  Props.append(new Property("dv", "2.0", false,
	QObject::tr("fitting factor for voltage drop")));
  Props.append(new Property("nv", "16", false,
	QObject::tr("fitting factor for diode current")));

  Props.append(new Property("Cj0", "80 fF", false,
	QObject::tr("zero-bias depletion capacitance")));
  Props.append(new Property("M", "0.5", false,
	QObject::tr("grading coefficient")));
  Props.append(new Property("Vj", "0.5 V", false,
	QObject::tr("junction potential")));
  Props.append(new Property("te", "0.6 ps", false,
	QObject::tr("life-time of electrons")));

  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("Area", "1.0", false,
	QObject::tr("default area for diode")));

  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 12,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-12, -9,-12,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, -9,  0,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 12, -9, 12,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-12, -9, 12,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-12,  9, 12, -9,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;

  tx = x1+4;
  ty = y2+4;
  Model = "RTD";
  Name  = "D";
}

// -------------------------------------------------------------------
Component* TunnelDiode::newOne()
{
  return new TunnelDiode();
}

Element* TunnelDiode::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Tunnel Diode");
  BitmapFile = (char *) "tunneldiode";

  if(getNewOne)  return new TunnelDiode();
  return 0;
}
