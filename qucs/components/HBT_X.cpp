/*
 * HBT_X.cpp - device implementations for HBT_X module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "HBT_X.h"

HBT_X::HBT_X()
{
  Description = QObject::tr ("HBT model by Ferdinand-Braun-Institut (FBH), Berlin");

  Props.append (new Property ("Mode", "1", false,
    QObject::tr ("Ignored")));
  Props.append (new Property ("Noise", "1", false,
    QObject::tr ("Ignored")));
  Props.append (new Property ("Debug", "0", false,
    QObject::tr ("Ignored")));
  Props.append (new Property ("DebugPlus", "0", false,
    QObject::tr ("Ignored")));
  Props.append (new Property ("Temp", "25.0", false,
    QObject::tr ("Device operating temperature, Celsius")));
  Props.append (new Property ("Rth", "0.1", false,
    QObject::tr ("Thermal resistance, K/W")));
  Props.append (new Property ("Cth", "700e-9", false,
    QObject::tr ("Thermal capacitance")));
  Props.append (new Property ("N", "1", false,
    QObject::tr ("Scaling factor, number of emitter fingers")));
  Props.append (new Property ("L", "30e-6", false,
    QObject::tr ("Length of emitter finger, m")));
  Props.append (new Property ("W", "1e-6", false,
    QObject::tr ("Width of emitter finger, m")));
  Props.append (new Property ("Jsf", "20e-24", false,
    QObject::tr ("Forward saturation current density, A/um^2")));
  Props.append (new Property ("nf", "1.0", false,
    QObject::tr ("Forward current emission coefficient")));
  Props.append (new Property ("Vg", "1.3", false,
    QObject::tr ("Forward thermal activation energy, V, (0 == disables temperature dependence)")));
  Props.append (new Property ("Jse", "0.0", false,
    QObject::tr ("B-E leakage saturation current density, A/um^2")));
  Props.append (new Property ("ne", "0.0", false,
    QObject::tr ("B-E leakage emission coefficient")));
  Props.append (new Property ("Rbxx", "1e6", false,
    QObject::tr ("Limiting resistor of B-E leakage diode, Ohm")));
  Props.append (new Property ("Vgb", "0.0", false,
    QObject::tr ("B-E leakage thermal activation energy, V, (0 == disables temperature dependence)")));
  Props.append (new Property ("Jsee", "0.0", false,
    QObject::tr ("2nd B-E leakage saturation current density, A/um^2")));
  Props.append (new Property ("nee", "0.0", false,
    QObject::tr ("2nd B-E leakage emission coefficient")));
  Props.append (new Property ("Rbbxx", "1e6", false,
    QObject::tr ("2nd Limiting resistor of B-E leakage diode, Ohm")));
  Props.append (new Property ("Vgbb", "0.0", false,
    QObject::tr ("2nd B-E leakage thermal activation energy, V, (0 == disables temperature dependence)")));
  Props.append (new Property ("Jsr", "20e-18", false,
    QObject::tr ("Reverse saturation current density, A/um^2")));
  Props.append (new Property ("nr", "1.0", false,
    QObject::tr ("Reverse current emission coefficient")));
  Props.append (new Property ("Vgr", "0.0", false,
    QObject::tr ("Reverse thermal activation energy, V, (0 == disables temperature dependence)")));
  Props.append (new Property ("XCjc", "0.5", false,
    QObject::tr ("Fraction of Cjc that goes to internal base node")));
  Props.append (new Property ("Jsc", "0.0", false,
    QObject::tr ("B-C leakage saturation current density, A/um^2 (0. switches off diode)")));
  Props.append (new Property ("nc", "0.0", false,
    QObject::tr ("B-C leakage emission coefficient (0. switches off diode)")));
  Props.append (new Property ("Rcxx", "1e6", false,
    QObject::tr ("Limiting resistor of B-C leakage diode, Ohm")));
  Props.append (new Property ("Vgc", "0.0", false,
    QObject::tr ("B-C leakage thermal activation energy, V, (0 == disables temperature dependence)")));
  Props.append (new Property ("Bf", "100.0", false,
    QObject::tr ("Ideal forward beta")));
  Props.append (new Property ("kBeta", "0.0", false,
    QObject::tr ("Temperature coefficient of forward current gain, -1/K,  (0 == disables temperature dependence)")));
  Props.append (new Property ("Br", "1.0", false,
    QObject::tr ("Ideal reverse beta")));
  Props.append (new Property ("VAF", "0.0", false,
    QObject::tr ("Forward Early voltage, V,  (0 == disables Early Effect)")));
  Props.append (new Property ("VAR", "0.0", false,
    QObject::tr ("Reverse Early voltage, V, (0 == disables Early Effect)")));
  Props.append (new Property ("IKF", "0.0", false,
    QObject::tr ("Forward high-injection knee current, A, (0 == disables Webster Effect)")));
  Props.append (new Property ("IKR", "0.0", false,
    QObject::tr ("Reverse high-injection knee current, A, (0 == disables Webster Effect)")));
  Props.append (new Property ("Mc", "0.0", false,
    QObject::tr ("C-E breakdown exponent, (0 == disables collector break-down)")));
  Props.append (new Property ("BVceo", "0.0", false,
    QObject::tr ("C-E breakdown voltage, V, (0 == disables collector break-down)")));
  Props.append (new Property ("kc", "0.0", false,
    QObject::tr ("C-E breakdown factor, (0 == disables collector break-down)")));
  Props.append (new Property ("BVebo", "0.0", false,
    QObject::tr ("B-E breakdown voltage, V, (0 == disables emitter break-down)")));
  Props.append (new Property ("Tr", "1.0e-15", false,
    QObject::tr ("Ideal reverse transit time, s")));
  Props.append (new Property ("Trx", "1.0e-15", false,
    QObject::tr ("Extrinsic BC diffusion capacitance, F")));
  Props.append (new Property ("Tf", "1.0e-12", false,
    QObject::tr ("Ideal forward transit time, s")));
  Props.append (new Property ("Tft", "0.0", false,
    QObject::tr ("Temperature coefficient of forward transit time")));
  Props.append (new Property ("Thcs", "0.0", false,
    QObject::tr ("Excess transit time coefficient at base push-out")));
  Props.append (new Property ("Ahc", "0.0", false,
    QObject::tr ("Smoothing parameter for Thcs")));
  Props.append (new Property ("Cje", "1.0e-15", false,
    QObject::tr ("B-E zero-bias depletion capacitance, F/um^2")));
  Props.append (new Property ("mje", "0.5", false,
    QObject::tr ("B-E junction exponential factor")));
  Props.append (new Property ("Vje", "1.3", false,
    QObject::tr ("B-E junction built-in potential, V")));
  Props.append (new Property ("Cjc", "1.0e-15", false,
    QObject::tr ("B-C zero-bias depletion capacitance, F/um^2")));
  Props.append (new Property ("mjc", "0.5", false,
    QObject::tr ("B-C junction exponential factor")));
  Props.append (new Property ("Vjc", "1.3", false,
    QObject::tr ("B-C junction built-in potential, V")));
  Props.append (new Property ("kjc", "1.0", false,
    QObject::tr ("not used")));
  Props.append (new Property ("Cmin", "0.1e-15", false,
    QObject::tr ("Minimum B-C depletion capacitance (Vbc dependence), F/um^2")));
  Props.append (new Property ("J0", "1e-3", false,
    QObject::tr ("Collector current where Cbc reaches Cmin, A/um^2 (0 == disables Cbc reduction)")));
  Props.append (new Property ("XJ0", "1.0", false,
    QObject::tr ("Fraction of Cmin, lower limit of BC capacitance (Ic dependence)")));
  Props.append (new Property ("Rci0", "1e-3", false,
    QObject::tr ("Onset of base push-out at low voltages, Ohm*um^2 (0 == disables base push-out)")));
  Props.append (new Property ("Jk", "4e-4", false,
    QObject::tr ("Onset of base push-out at high voltages, A/um^2, (0 == disables base push-out)")));
  Props.append (new Property ("RJk", "1e-3", false,
    QObject::tr ("Slope of Jk at high currents , Ohm*um^2")));
  Props.append (new Property ("Vces", "1e-3", false,
    QObject::tr ("Voltage shift of base push-out onset, V")));
  Props.append (new Property ("Rc", "1.0", false,
    QObject::tr ("Collector resistance, Ohm/finger")));
  Props.append (new Property ("Re", "1.0", false,
    QObject::tr ("Emitter resistance, Ohm/finger")));
  Props.append (new Property ("Rb", "1.0", false,
    QObject::tr ("Extrinsic base resistance, Ohm/finger")));
  Props.append (new Property ("Rb2", "1.0", false,
    QObject::tr ("Inner Base ohmic resistance, Ohm/finger")));
  Props.append (new Property ("Lc", "0.0", false,
    QObject::tr ("Collector inductance, H")));
  Props.append (new Property ("Le", "0.0", false,
    QObject::tr ("Emitter inductance, H")));
  Props.append (new Property ("Lb", "0.0", false,
    QObject::tr ("Base inductance, H")));
  Props.append (new Property ("Cq", "0.0", false,
    QObject::tr ("Extrinsic B-C capacitance, F")));
  Props.append (new Property ("Cpb", "0.0", false,
    QObject::tr ("Extrinsic base capacitance, F")));
  Props.append (new Property ("Cpc", "0.0", false,
    QObject::tr ("Extrinsic collector capacitance, F")));
  Props.append (new Property ("Kfb", "0.0", false,
    QObject::tr ("Flicker-noise coefficient")));
  Props.append (new Property ("Afb", "0.0", false,
    QObject::tr ("Flicker-noise exponent")));
  Props.append (new Property ("Ffeb", "0.0", false,
    QObject::tr ("Flicker-noise frequency exponent")));
  Props.append (new Property ("Kb", "0.0", false,
    QObject::tr ("Burst noise coefficient")));
  Props.append (new Property ("Ab", "0.0", false,
    QObject::tr ("Burst noise exponent")));
  Props.append (new Property ("Fb", "0.0", false,
    QObject::tr ("Burst noise corner frequency, Hz")));
  Props.append (new Property ("Kfe", "0.0", false,
    QObject::tr ("Flicker-noise coefficient")));
  Props.append (new Property ("Afe", "0.0", false,
    QObject::tr ("Flicker-noise exponent")));
  Props.append (new Property ("Ffee", "0.0", false,
    QObject::tr ("Flicker-noise frequency exponent")));
  Props.append (new Property ("Tnom", "20.0", false,
    QObject::tr ("Ambient temperature at which the parameters were determined")));


  // normal bipolar
  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  // thermal node
  Lines.append(new Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 17,-20, 23,QPen(Qt::darkBlue,2)));
  
  // arrow
  Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));

  // H
  Lines.append(new Line(-30,-30,-30,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-30,-27,-26,-27,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-26,-30,-26,-24,QPen(Qt::darkBlue,1)));
  // B
  Lines.append(new Line(-23,-30,-23,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-24,-30,-20,-30,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-23,-27,-20,-27,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-24,-24,-20,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-20,-30,-20,-27,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-20,-27,-20,-24,QPen(Qt::darkBlue,1)));
  // T
  Lines.append(new Line(-18,-30,-14,-30,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-16,-30,-16,-24,QPen(Qt::darkBlue,1)));

  Ports.append(new Port(  0,-30)); // collector
  Ports.append(new Port(-30,  0)); // base
  Ports.append(new Port(  0, 30)); // emitter
  Ports.append(new Port(-30, 20)); // thermal node

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

  tx = x2 + 4;
  ty = y1 + 4;
  Model = "HBT_X";
  Name  = "T";
}

Component * HBT_X::newOne()
{
  HBT_X * p = new HBT_X();
  p->recreate(0); 
  return p;
}

Element * HBT_X::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("FBH HBT");
  BitmapFile = (char *) "npn_therm";

  if(getNewOne) return new HBT_X();
  return 0;
}
