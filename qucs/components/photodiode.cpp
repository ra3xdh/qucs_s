/*
 * photodiode.cpp - device implementations for photodiode module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "photodiode.h"

photodiode::photodiode()
{
  Description = QObject::tr ("Photodiode verilog device");

  Props.append (new Property ("N", "1.35", false,
    QObject::tr ("photodiode emission coefficient")));
  Props.append (new Property ("Rseries", "1e-3", false,
    QObject::tr ("series lead resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("Is", "0.34e-12", false,
    QObject::tr ("diode dark current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("Bv", "60", false,
    QObject::tr ("reverse breakdown voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Ibv", "1e-3", false,
    QObject::tr ("current at reverse breakdown voltage")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("Vj", "0.7", false,
    QObject::tr ("junction potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Cj0", "60e-12", false,
    QObject::tr ("zero-bias junction capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("M", "0.5", false,
    QObject::tr ("grading coefficient")));
  Props.append (new Property ("Area", "1.0", false,
    QObject::tr ("diode relative area")));
  Props.append (new Property ("Tnom", "26.85", false,
    QObject::tr ("parameter measurement temperature")
    +" ("+QObject::tr ("Celsius")+")"));
  Props.append (new Property ("Fc", "0.5", false,
    QObject::tr ("forward-bias depletion capacitance coefficient")));
  Props.append (new Property ("Tt", "10e-9", false,
    QObject::tr ("transit time")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("Xti", "3.0", false,
    QObject::tr ("saturation current temperature exponent")));
  Props.append (new Property ("Eg", "1.16", false,
    QObject::tr ("energy gap")
    +" ("+QObject::tr ("eV")+")"));
  Props.append (new Property ("Responsivity", "0.5", false,
    QObject::tr ("responsivity")
    +" ("+QObject::tr ("A/W")+")"));
  Props.append (new Property ("Rsh", "5e8", false,
    QObject::tr ("shunt resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("QEpercent", "80", false,
    QObject::tr ("quantum efficiency")
    +" ("+QObject::tr ("%")+")"));
  Props.append (new Property ("Lambda", "900", false,
    QObject::tr ("light wavelength")
    +" ("+QObject::tr ("nm")+")"));
  Props.append (new Property ("LEVEL", "1", false,
    QObject::tr ("responsivity calculator selector")));
  Props.append (new Property ("Kf", "1e-12", false,
    QObject::tr ("flicker noise coefficient")));
  Props.append (new Property ("Af", "1.0", false,
    QObject::tr ("flicker noise exponent")));
  Props.append (new Property ("Ffe", "1.0", false,
    QObject::tr ("flicker noise frequency exponent")));
  Props.append (new Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "photodiode";
  Name  = "PD";
}

Component * photodiode::newOne()
{
  photodiode * p = new photodiode();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * photodiode::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Photodiode");
  BitmapFile = (char *) "photodiode";

  if(getNewOne) return new photodiode();
  return 0;
}

void photodiode::createSymbol()
{
  Arcs.append(new Arc(-12,-12, 24, 24, 0, 16*360,QPen(Qt::red,2)));
  Lines.append(new Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6, -9, -6,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  6, -9,  6,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 12,  0,  30,QPen(Qt::green,2)));

  Ports.append(new Port( 30, 0));
  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 0, 30));

  x1 = -30; y1 = -20;
  x2 =  30; y2 =  30;
}
