/*
 * mod_amp.cpp - device implementations for mod_amp module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "mod_amp.h"
#include "extsimkernels/spicecompat.h"

mod_amp::mod_amp()
{
  Description = QObject::tr ("Modular Operational Amplifier verilog device");
  Simulator = spicecompat::simQucsator;

  Props.append (new Property ("GBP", "1e6", false,
    QObject::tr ("Gain bandwidth product (Hz)")));
  Props.append (new Property ("AOLDC", "106.0", false,
    QObject::tr ("Open-loop differential gain at DC (dB)")));
  Props.append (new Property ("FP2", "3e6", false,
    QObject::tr ("Second pole frequency (Hz)")));
  Props.append (new Property ("RO", "75", false,
    QObject::tr ("Output resistance (Ohm)")));
  Props.append (new Property ("CD", "1e-12", false,
    QObject::tr ("Differential input capacitance (F)")));
  Props.append (new Property ("RD", "2e6", false,
    QObject::tr ("Differential input resistance (Ohm)")));
  Props.append (new Property ("IOFF", "20e-9", false,
    QObject::tr ("Input offset current (A)")));
  Props.append (new Property ("IB", "80e-9", false,
    QObject::tr ("Input bias current (A)")));
  Props.append (new Property ("VOFF", "7e-4", false,
    QObject::tr ("Input offset voltage (V)")));
  Props.append (new Property ("CMRRDC", "90.0", false,
    QObject::tr ("Common-mode rejection ratio at DC (dB)")));
  Props.append (new Property ("FCM", "200.0", false,
    QObject::tr ("Common-mode zero corner frequency (Hz)")));
  Props.append (new Property ("PSRT", "5e5", false,
    QObject::tr ("Positive slew rate (V/s)")));
  Props.append (new Property ("NSRT", "5e5", false,
    QObject::tr ("Negative slew rate (V/s)")));
  Props.append (new Property ("VLIMP", "14", false,
    QObject::tr ("Positive output voltage limit (V)")));
  Props.append (new Property ("VLIMN", "-14", false,
    QObject::tr ("Negative output voltage limit (V)")));
  Props.append (new Property ("ILMAX", "35e-3", false,
    QObject::tr ("Maximum DC output current (A)")));
 Props.append (new Property ("CSCALE", "50", false,
    QObject::tr ("Current limit scale factor")));
  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "mod_amp";
  Name  = "OP";
}

Component * mod_amp::newOne()
{
  mod_amp * p = new mod_amp();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * mod_amp::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Modular OpAmp");
  BitmapFile = (char *) "mod_amp";

  if(getNewOne) return new mod_amp();
  return 0;
}

void mod_amp::createSymbol()
{

  Lines.append(new qucs::Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 40,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-20,-35,-20, 35,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,-35, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20, 35, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-16, 19, -9, 19,QPen(Qt::black,2)));
  Lines.append(new qucs::Line(-16,-19, -9,-19,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(-13,-22,-13,-15,QPen(Qt::red,2)));

  Lines.append(new qucs::Line(-10, -10, -10, 10,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(-10, -10,  0,   0,QPen(Qt::red,2)));
  Lines.append(new qucs::Line(  0,  0,  10, -10,QPen(Qt::red,2)));
  Lines.append(new qucs::Line( 10, -10, 10,  10,QPen(Qt::red,2)));
  
  Ports.append(new Port(-30,-20));
  Ports.append(new Port(-30, 20));
  Ports.append(new Port( 40,  0));

  x1 = -30; y1 = -38;
  x2 =  40; y2 =  38;

}
