/*
 * MESFET.cpp - device implementations for MESFET module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "MESFET.h"

MESFET::MESFET()
{
  Description = QObject::tr ("MESFET verilog device");

  Props.append (new Property ("LEVEL", "1", false,
    QObject::tr ("model selector")));
  Props.append (new Property ("Vto", "-1.8", false,
    QObject::tr ("pinch-off voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Beta", "3e-3", false,
    QObject::tr ("transconductance parameter")
    +" ("+QObject::tr ("A/(V*V)")+")"));
  Props.append (new Property ("Alpha", "2.25", false,
    QObject::tr ("saturation voltage parameter")
    +" ("+QObject::tr ("1/V")+")"));
  Props.append (new Property ("Lambda", "0.05", false,
    QObject::tr ("channel length modulation parameter")
    +" ("+QObject::tr ("1/V")+")"));
  Props.append (new Property ("B", "0.3", false,
    QObject::tr ("doping profile parameter")
    +" ("+QObject::tr ("1/V")+")"));
  Props.append (new Property ("Qp", "2.1", false,
    QObject::tr ("power law exponent parameter")));
  Props.append (new Property ("Delta", "0.1", false,
    QObject::tr ("power feedback parameter")
    +" ("+QObject::tr ("1/W")+")"));
  Props.append (new Property ("Vmax", "0.5", false,
    QObject::tr ("maximum junction voltage limit before capacitance limiting")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Vdelta1", "0.3", false,
    QObject::tr ("capacitance saturation transition voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Vdelta2", "0.2", false,
    QObject::tr ("capacitance threshold transition voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Gamma", "0.015", false,
    QObject::tr ("dc drain pull coefficient")));
  Props.append (new Property ("Nsc", "1", false,
    QObject::tr ("subthreshold conductance parameter")));
  Props.append (new Property ("Is", "1e-14", false,
    QObject::tr ("diode saturation current")
    +" ("+QObject::tr ("I")+")"));
  Props.append (new Property ("N", "1", false,
    QObject::tr ("diode emission coefficient")));
  Props.append (new Property ("Vbi", "1.0", false,
    QObject::tr ("built-in gate potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Bv", "60", false,
    QObject::tr ("gate-drain junction reverse bias breakdown voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Xti", "3.0", false,
    QObject::tr ("diode saturation current temperature coefficient")));
  Props.append (new Property ("Fc", "0.5", false,
    QObject::tr ("forward-bias depletion capacitance coefficient")));
  Props.append (new Property ("Tau", "1e-9", false,
    QObject::tr ("transit time under gate")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("Rin", "1e-3", false,
    QObject::tr ("channel resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("Area", "1", false,
    QObject::tr ("area factor")));
  Props.append (new Property ("Eg", "1.11", false,
    QObject::tr ("energy gap")
    +" ("+QObject::tr ("eV")+")"));
  Props.append (new Property ("M", "0.5", false,
    QObject::tr ("grading coefficient")));
  Props.append (new Property ("Cgd", "0.2e-12", false,
    QObject::tr ("zero bias gate-drain junction capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("Cgs", "1e-12", false,
    QObject::tr ("zero bias gate-source junction capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("Cds", "1e-12", false,
    QObject::tr ("zero bias drain-source junction capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("Betatc", "0", false,
    QObject::tr ("Beta temperature coefficient")
    +" ("+QObject::tr ("%/Celsius")+")"));
  Props.append (new Property ("Alphatc", "0", false,
    QObject::tr ("Alpha temperature coefficient")
    +" ("+QObject::tr ("%/Celsius")+")"));
  Props.append (new Property ("Gammatc", "0", false,
    QObject::tr ("Gamma temperature coefficient")
    +" ("+QObject::tr ("%/Celsius")+")"));
  Props.append (new Property ("Ng", "2.65", false,
    QObject::tr ("Subthreshold slope gate parameter")));
  Props.append (new Property ("Nd", "-0.19", false,
    QObject::tr ("subthreshold drain pull parameter")));
  Props.append (new Property ("ILEVELS", "3", false,
    QObject::tr ("gate-source current equation selector")));
  Props.append (new Property ("ILEVELD", "3", false,
    QObject::tr ("gate-drain current equation selector")));
  Props.append (new Property ("QLEVELS", "2", false,
    QObject::tr ("gate-source charge equation selector")));
  Props.append (new Property ("QLEVELD", "2", false,
    QObject::tr ("gate-drain charge equation selector")));
  Props.append (new Property ("QLEVELDS", "2", false,
    QObject::tr ("drain-source charge equation selector")));
  Props.append (new Property ("Vtotc", "0", false,
    QObject::tr ("Vto temperature coefficient")));
  Props.append (new Property ("Rg", "5.1", false,
    QObject::tr ("gate resistance")
    +" ("+QObject::tr ("Ohms")+")"));
  Props.append (new Property ("Rd", "1.3", false,
    QObject::tr ("drain resistance")
    +" ("+QObject::tr ("Ohms")+")"));
  Props.append (new Property ("Rs", "1.3", false,
    QObject::tr ("source resistance")
    +" ("+QObject::tr ("Ohms")+")"));
  Props.append (new Property ("Rgtc", "0", false,
    QObject::tr ("gate resistance temperature coefficient")
    +" ("+QObject::tr ("1/Celsius")+")"));
  Props.append (new Property ("Rdtc", "0", false,
    QObject::tr ("drain resistance temperature coefficient")
    +" ("+QObject::tr ("1/Celsius")+")"));
  Props.append (new Property ("Rstc", "0", false,
    QObject::tr ("source resistance temperature coefficient")
    +" ("+QObject::tr ("1/Celsius")+")"));
  Props.append (new Property ("Ibv", "1e-3", false,
    QObject::tr ("gate reverse breakdown currrent")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("Rf", "10", false,
    QObject::tr ("forward bias slope resistance")
    +" ("+QObject::tr ("Ohms")+")"));
  Props.append (new Property ("R1", "10", false,
    QObject::tr ("breakdown slope resistance")
    +" ("+QObject::tr ("Ohms")+")"));
  Props.append (new Property ("Af", "1", false,
    QObject::tr ("flicker noise exponent")));
  Props.append (new Property ("Kf", "0", false,
    QObject::tr ("flicker noise coefficient")));
  Props.append (new Property ("Gdsnoi", "1", false,
    QObject::tr ("shot noise coefficient")));
  Props.append (new Property ("Tnom", "26.85", false,
    QObject::tr ("parameter measurement temperature")
    +" ("+QObject::tr ("Celsius")+")"));
  Props.append (new Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "MESFET";
  Name  = "T";
}

Component * MESFET::newOne()
{
  MESFET * p = new MESFET();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * MESFET::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("MESFET");
  BitmapFile = (char *) "MESFET";

  if(getNewOne) return new MESFET();
  return 0;
}

void MESFET::createSymbol()
{
  // put in here symbol drawing code and terminal definitions
  Lines.append(new qucs::Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-10,  0,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,-10,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 10,  0, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 10,  0, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( -4, 24,  4, 20,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(  0,-30));
  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;
}
