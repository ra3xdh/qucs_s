/*
 * hicumL2V2p23.cpp - device implementations for hicumL2V2p23 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "hicumL2V2p23.h"

hicumL2V2p23::hicumL2V2p23()
{
  Description = QObject::tr ("HICUM Level 2 v2.23 verilog device");

  Props.append (new Property ("c10", "2.0E-30", false,
    QObject::tr ("GICCR constant")
    +" ("+QObject::tr ("A^2s")+")"));
  Props.append (new Property ("qp0", "2.0E-14", false,
    QObject::tr ("Zero-bias hole charge")
    +" ("+QObject::tr ("Coul")+")"));
  Props.append (new Property ("ich", "0.0", false,
    QObject::tr ("High-current correction for 2D and 3D effects")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("hfe", "1.0", false,
    QObject::tr ("Emitter minority charge weighting factor in HBTs")));
  Props.append (new Property ("hfc", "1.0", false,
    QObject::tr ("Collector minority charge weighting factor in HBTs")));
  Props.append (new Property ("hjei", "1.0", false,
    QObject::tr ("B-E depletion charge weighting factor in HBTs")));
  Props.append (new Property ("hjci", "1.0", false,
    QObject::tr ("B-C depletion charge weighting factor in HBTs")));
  Props.append (new Property ("ibeis", "1.0E-18", false,
    QObject::tr ("Internal B-E saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mbei", "1.0", false,
    QObject::tr ("Internal B-E current ideality factor")));
  Props.append (new Property ("ireis", "0.0", false,
    QObject::tr ("Internal B-E recombination saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mrei", "2.0", false,
    QObject::tr ("Internal B-E recombination current ideality factor")));
  Props.append (new Property ("ibeps", "0.0", false,
    QObject::tr ("Peripheral B-E saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mbep", "1.0", false,
    QObject::tr ("Peripheral B-E current ideality factor")));
  Props.append (new Property ("ireps", "0.0", false,
    QObject::tr ("Peripheral B-E recombination saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mrep", "2.0", false,
    QObject::tr ("Peripheral B-E recombination current ideality factor")));
  Props.append (new Property ("mcf", "1.0", false,
    QObject::tr ("Non-ideality factor for III-V HBTs")));
  Props.append (new Property ("tbhrec", "0.0", false,
    QObject::tr ("Base current recombination time constant at B-C barrier for high forward injection")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("ibcis", "1.0E-16", false,
    QObject::tr ("Internal B-C saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mbci", "1.0", false,
    QObject::tr ("Internal B-C current ideality factor")));
  Props.append (new Property ("ibcxs", "0.0", false,
    QObject::tr ("External B-C saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mbcx", "1.0", false,
    QObject::tr ("External B-C current ideality factor")));
  Props.append (new Property ("ibets", "0.0", false,
    QObject::tr ("B-E tunneling saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("abet", "40", false,
    QObject::tr ("Exponent factor for tunneling current")));
  Props.append (new Property ("tunode", "1", false,
    QObject::tr ("Specifies the base node connection for the tunneling current")));
  Props.append (new Property ("favl", "0.0", false,
    QObject::tr ("Avalanche current factor")
    +" ("+QObject::tr ("1/V")+")"));
  Props.append (new Property ("qavl", "0.0", false,
    QObject::tr ("Exponent factor for avalanche current")
    +" ("+QObject::tr ("Coul")+")"));
  Props.append (new Property ("alfav", "0.0", false,
    QObject::tr ("Relative TC for FAVL")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("alqav", "0.0", false,
    QObject::tr ("Relative TC for QAVL")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("rbi0", "0.0", false,
    QObject::tr ("Zero bias internal base resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("rbx", "0.0", false,
    QObject::tr ("External base series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("fgeo", "0.6557", false,
    QObject::tr ("Factor for geometry dependence of emitter current crowding")));
  Props.append (new Property ("fdqr0", "0.0", false,
    QObject::tr ("Correction factor for modulation by B-E and B-C space charge layer")));
  Props.append (new Property ("fcrbi", "0.0", false,
    QObject::tr ("Ratio of HF shunt to total internal capacitance (lateral NQS effect)")));
  Props.append (new Property ("fqi", "1.0", false,
    QObject::tr ("Ration of internal to total minority charge")));
  Props.append (new Property ("re", "0.0", false,
    QObject::tr ("Emitter series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("rcx", "0.0", false,
    QObject::tr ("External collector series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("itss", "0.0", false,
    QObject::tr ("Substrate transistor transfer saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("msf", "1.0", false,
    QObject::tr ("Forward ideality factor of substrate transfer current")));
  Props.append (new Property ("iscs", "0.0", false,
    QObject::tr ("C-S diode saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("msc", "1.0", false,
    QObject::tr ("Ideality factor of C-S diode current")));
  Props.append (new Property ("tsf", "0.0", false,
    QObject::tr ("Transit time for forward operation of substrate transistor")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("rsu", "0.0", false,
    QObject::tr ("Substrate series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("csu", "0.0", false,
    QObject::tr ("Substrate shunt capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("cjei0", "1.0E-20", false,
    QObject::tr ("Internal B-E zero-bias depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vdei", "0.9", false,
    QObject::tr ("Internal B-E built-in potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zei", "0.5", false,
    QObject::tr ("Internal B-E grading coefficient")));
  Props.append (new Property ("ajei", "2.5", false,
    QObject::tr ("Ratio of maximum to zero-bias value of internal B-E capacitance")));
  Props.append (new Property ("cjep0", "1.0E-20", false,
    QObject::tr ("Peripheral B-E zero-bias depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vdep", "0.9", false,
    QObject::tr ("Peripheral B-E built-in potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zep", "0.5", false,
    QObject::tr ("Peripheral B-E grading coefficient")));
  Props.append (new Property ("ajep", "2.5", false,
    QObject::tr ("Ratio of maximum to zero-bias value of peripheral B-E capacitance")));
  Props.append (new Property ("cjci0", "1.0E-20", false,
    QObject::tr ("Internal B-C zero-bias depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vdci", "0.7", false,
    QObject::tr ("Internal B-C built-in potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zci", "0.4", false,
    QObject::tr ("Internal B-C grading coefficient")));
  Props.append (new Property ("vptci", "100", false,
    QObject::tr ("Internal B-C punch-through voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("cjcx0", "1.0E-20", false,
    QObject::tr ("External B-C zero-bias depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vdcx", "0.7", false,
    QObject::tr ("External B-C built-in potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zcx", "0.4", false,
    QObject::tr ("External B-C grading coefficient")));
  Props.append (new Property ("vptcx", "100", false,
    QObject::tr ("External B-C punch-through voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("fbcpar", "0.0", false,
    QObject::tr ("Partitioning factor of parasitic B-C cap")));
  Props.append (new Property ("fbepar", "1.0", false,
    QObject::tr ("Partitioning factor of parasitic B-E cap")));
  Props.append (new Property ("cjs0", "0.0", false,
    QObject::tr ("C-S zero-bias depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vds", "0.6", false,
    QObject::tr ("C-S built-in potential")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zs", "0.5", false,
    QObject::tr ("C-S grading coefficient")));
  Props.append (new Property ("vpts", "100", false,
    QObject::tr ("C-S punch-through voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("t0", "0.0", false,
    QObject::tr ("Low current forward transit time at VBC=0V")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("dt0h", "0.0", false,
    QObject::tr ("Time constant for base and B-C space charge layer width modulation")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("tbvl", "0.0", false,
    QObject::tr ("Time constant for modelling carrier jam at low VCE")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("tef0", "0.0", false,
    QObject::tr ("Neutral emitter storage time")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("gtfe", "1.0", false,
    QObject::tr ("Exponent factor for current dependence of neutral emitter storage time")));
  Props.append (new Property ("thcs", "0.0", false,
    QObject::tr ("Saturation time constant at high current densities")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("ahc", "0.1", false,
    QObject::tr ("Smoothing factor for current dependence of base and collector transit time")));
  Props.append (new Property ("fthc", "0.0", false,
    QObject::tr ("Partitioning factor for base and collector portion")));
  Props.append (new Property ("rci0", "150", false,
    QObject::tr ("Internal collector resistance at low electric field")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("vlim", "0.5", false,
    QObject::tr ("Voltage separating ohmic and saturation velocity regime")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vces", "0.1", false,
    QObject::tr ("Internal C-E saturation voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vpt", "0.0", false,
    QObject::tr ("Collector punch-through voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("tr", "0.0", false,
    QObject::tr ("Storage time for inverse operation")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("cbepar", "0.0", false,
    QObject::tr ("Total parasitic B-E capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("cbcpar", "0.0", false,
    QObject::tr ("Total parasitic B-C capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("alqf", "0.0", false,
    QObject::tr ("Factor for additional delay time of minority charge")));
  Props.append (new Property ("alit", "0.0", false,
    QObject::tr ("Factor for additional delay time of transfer current")));
  Props.append (new Property ("flnqs", "0", false,
    QObject::tr ("Flag for turning on and off of vertical NQS effect")));
  Props.append (new Property ("kf", "0.0", false,
    QObject::tr ("Flicker noise coefficient")));
  Props.append (new Property ("af", "2.0", false,
    QObject::tr ("Flicker noise exponent factor")));
  Props.append (new Property ("cfbe", "-1", false,
    QObject::tr ("Flag for determining where to tag the flicker noise source")));
  Props.append (new Property ("latb", "0.0", false,
    QObject::tr ("Scaling factor for collector minority charge in direction of emitter width")));
  Props.append (new Property ("latl", "0.0", false,
    QObject::tr ("Scaling factor for collector minority charge in direction of emitter length")));
  Props.append (new Property ("vgb", "1.17", false,
    QObject::tr ("Bandgap voltage extrapolated to 0 K")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("alt0", "0.0", false,
    QObject::tr ("First order relative TC of parameter T0")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("kt0", "0.0", false,
    QObject::tr ("Second order relative TC of parameter T0")));
  Props.append (new Property ("zetaci", "0.0", false,
    QObject::tr ("Temperature exponent for RCI0")));
  Props.append (new Property ("alvs", "0.0", false,
    QObject::tr ("Relative TC of saturation drift velocity")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("alces", "0.0", false,
    QObject::tr ("Relative TC of VCES")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("zetarbi", "0.0", false,
    QObject::tr ("Temperature exponent of internal base resistance")));
  Props.append (new Property ("zetarbx", "0.0", false,
    QObject::tr ("Temperature exponent of external base resistance")));
  Props.append (new Property ("zetarcx", "0.0", false,
    QObject::tr ("Temperature exponent of external collector resistance")));
  Props.append (new Property ("zetare", "0.0", false,
    QObject::tr ("Temperature exponent of emitter resistance")));
  Props.append (new Property ("zetacx", "1.0", false,
    QObject::tr ("Temperature exponent of mobility in substrate transistor transit time")));
  Props.append (new Property ("vge", "1.17", false,
    QObject::tr ("Effective emitter bandgap voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vgc", "1.17", false,
    QObject::tr ("Effective collector bandgap voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vgs", "1.17", false,
    QObject::tr ("Effective substrate bandgap voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("f1vg", "-1.02377e-4", false,
    QObject::tr ("Coefficient K1 in T-dependent band-gap equation")));
  Props.append (new Property ("f2vg", "4.3215e-4", false,
    QObject::tr ("Coefficient K2 in T-dependent band-gap equation")));
  Props.append (new Property ("zetact", "3.0", false,
    QObject::tr ("Exponent coefficient in transfer current temperature dependence")));
  Props.append (new Property ("zetabet", "3.5", false,
    QObject::tr ("Exponent coefficient in B-E junction current temperature dependence")));
  Props.append (new Property ("alb", "0.0", false,
    QObject::tr ("Relative TC of forward current gain for V2.1 model")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("flsh", "0", false,
    QObject::tr ("Flag for turning on and off self-heating effect")));
  Props.append (new Property ("rth", "0.0", false,
    QObject::tr ("Thermal resistance")
    +" ("+QObject::tr ("K/W")+")"));
  Props.append (new Property ("cth", "0.0", false,
    QObject::tr ("Thermal capacitance")
    +" ("+QObject::tr ("J/W")+")"));
  Props.append (new Property ("flcomp", "0.0", false,
    QObject::tr ("Flag for compatibility with v2.1 model (0=v2.1)")));
  Props.append (new Property ("tnom", "27.0", false,
    QObject::tr ("Temperature at which parameters are specified")
    +" ("+QObject::tr ("C")+")"));
  Props.append (new Property ("dt", "0.0", false,
    QObject::tr ("Temperature change w.r.t. chip temperature for particular transistor")
    +" ("+QObject::tr ("K")+")"));
  Props.append (new Property ("Temp", "27", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "hicumL2V2p23";
  Name  = "T";
}

Component * hicumL2V2p23::newOne()
{
  hicumL2V2p23 * p = new hicumL2V2p23();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * hicumL2V2p23::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("HICUM L2 v2.23");
  BitmapFile = (char *) "npnsub_therm";

  if(getNewOne) return new hicumL2V2p23();
  return 0;
}

void hicumL2V2p23::createSymbol()
{
  // normal bipolar
  Lines.append(new qucs::Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  // substrate node
  Lines.append(new qucs::Line(  9,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  9, -7,  9,  7,QPen(Qt::darkBlue,3)));

  // thermal node
  Lines.append(new qucs::Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20, 17,-20, 23,QPen(Qt::darkBlue,2)));  

  // arrow
  Lines.append(new qucs::Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));

  // H
  Lines.append(new qucs::Line(-30,-30,-30,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-30,-27,-26,-27,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-26,-30,-26,-24,QPen(Qt::darkBlue,1)));
  // I
  Lines.append(new qucs::Line(-24,-30,-24,-24,QPen(Qt::darkBlue,1)));
  // C
  Lines.append(new qucs::Line(-22,-30,-22,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-22,-30,-19,-30,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-22,-24,-19,-24,QPen(Qt::darkBlue,1)));
  // U
  Lines.append(new qucs::Line(-17,-30,-17,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-14,-30,-14,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-17,-24,-14,-24,QPen(Qt::darkBlue,1)));
  // M
  Lines.append(new qucs::Line(-12,-30,-12,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( -8,-30, -8,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-12,-30,-10,-28,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( -8,-30,-10,-28,QPen(Qt::darkBlue,1)));

  // terminal definitions
  Ports.append(new Port(  0,-30)); // collector
  Ports.append(new Port(-30,  0)); // base
  Ports.append(new Port(  0, 30)); // emitter
  Ports.append(new Port( 30,  0)); // substrate
  Ports.append(new Port(-30, 20)); // thermal node

  // relative boundings
  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;
}
