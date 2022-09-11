/*
 * phototransistor.cpp - device implementations for phototransistor module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "phototransistor.h"

phototransistor::phototransistor()
{
  Description = QObject::tr ("Phototransistor verilog device");

  Props.append (new Property ("Bf", "100", false,
    QObject::tr ("forward beta")));
  Props.append (new Property ("Br", "0.1", false,
    QObject::tr ("reverse beta")));
  Props.append (new Property ("Is", "1e-10", false,
    QObject::tr ("dark current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("Nf", "1", false,
    QObject::tr ("forward emission coefficient")));
  Props.append (new Property ("Nr", "1", false,
    QObject::tr ("reverse emission coefficient")));
  Props.append (new Property ("Vaf", "100", false,
    QObject::tr ("forward early voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Var", "100", false,
    QObject::tr ("reverse early voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Mje", "0.33", false,
    QObject::tr ("base-emitter junction exponential factor")));
  Props.append (new Property ("Vje", "0.75", false,
    QObject::tr ("base-emitter junction built-in potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Cje", "1e-12", false,
    QObject::tr ("base-emitter zero-bias depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("Mjc", "0.33", false,
    QObject::tr ("base-collector junction exponential factor")));
  Props.append (new Property ("Vjc", "0.75", false,
    QObject::tr ("base-collector junction built-in potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Cjc", "2e-12", false,
    QObject::tr ("base-collector zero-bias depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("Tr", "100n", false,
    QObject::tr ("ideal reverse transit time")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("Tf", "0.1n", false,
    QObject::tr ("ideal forward transit time")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("Ikf", "10", false,
    QObject::tr ("high current corner for forward beta")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("Ikr", "10", false,
    QObject::tr ("high current corner for reverse beta")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("Rc", "10", false,
    QObject::tr ("collector series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("Re", "1", false,
    QObject::tr ("emitter series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("Rb", "100", false,
    QObject::tr ("base series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("Kf", "1e-12", false,
    QObject::tr ("flicker noise coefficient")));
  Props.append (new Property ("Ffe", "1", false,
    QObject::tr ("flicker noise coefficient")));
  Props.append (new Property ("Af", "1", false,
    QObject::tr ("flicker noise exponent")));
  Props.append (new Property ("Responsivity", "1.5", false,
    QObject::tr ("responsivity at relative selectivity=100%")
    +" ("+QObject::tr ("A/W")+")"));
  Props.append (new Property ("P0", "2.6122e3", false,
    QObject::tr ("relative selectivity polynomial coefficient")));
  Props.append (new Property ("P1", "-1.489e1", false,
    QObject::tr ("relative selectivity polynomial coefficient")));
  Props.append (new Property ("P2", "3.0332e-2", false,
    QObject::tr ("relative selectivity polynomial coefficient")));
  Props.append (new Property ("P3", "-2.5708e-5", false,
    QObject::tr ("relative selectivity polynomial coefficient")));
  Props.append (new Property ("P4", "7.6923e-9", false,
    QObject::tr ("relative selectivity polynomial coefficient")));
  Props.append (new Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 +24;
  Model = "phototransistor";
  Name  = "PT";
}

Component * phototransistor::newOne()
{
  phototransistor * p = new phototransistor();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * phototransistor::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Phototransistor");
  BitmapFile = (char *) "phototransistor";

  if(getNewOne) return new phototransistor();
  return 0;
}

void phototransistor::createSymbol()
{
  Arcs.append(new qucs::Arc(-25,-20, 40, 40,  0,16*360,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-50, -50, -40, -50,QPen(Qt::green,2)));
  Lines.append(new qucs::Line(-40, -50, -40, -30,QPen(Qt::green,2)));
  Lines.append(new qucs::Line(-40, -30, -50, -30,QPen(Qt::green,2)));

  // green arrow
  Lines.append(new qucs::Line(-40, -40, -16, -16,QPen(Qt::green,2)));
  Lines.append(new qucs::Line(-16, -16, -16, -23,QPen(Qt::green,2)));
  Lines.append(new qucs::Line(-16, -16, -23, -16,QPen(Qt::green,2)));

  // P
  Lines.append(new qucs::Line(-60, -55, -60, -65,QPen(Qt::black,2)));
  Lines.append(new qucs::Line(-60, -65, -55, -65,QPen(Qt::black,2)));
  Lines.append(new qucs::Line(-55, -65, -55, -60,QPen(Qt::black,2)));
  Lines.append(new qucs::Line(-60, -60, -55, -60,QPen(Qt::black,2)));

  // W
  Lines.append(new qucs::Line(-63, -40, -60, -35,QPen(Qt::black,2)));
  Lines.append(new qucs::Line(-60, -35, -57, -40,QPen(Qt::black,2)));
  Lines.append(new qucs::Line(-57, -40, -54, -35,QPen(Qt::black,2)));
  Lines.append(new qucs::Line(-54, -35, -51, -40,QPen(Qt::black,2)));

  Ports.append(new Port(  0,-30)); // Collector
  Ports.append(new Port(-30,  0)); // Base
  Ports.append(new Port(  0, 30)); // Emitter
  Ports.append(new Port(-50,-50)); // Power
  Ports.append(new Port(-50,-30)); // Wavelength

  x1 = -50; y1 = -60;
  x2 =  20; y2 =  30;
}
