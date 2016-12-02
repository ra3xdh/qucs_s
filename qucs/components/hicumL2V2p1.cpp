/*
 * hicumL2V2p1.cpp - device implementations for hicumL2V2p1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "hicumL2V2p1.h"

hicumL2V2p1::hicumL2V2p1()
{
  Description = QObject::tr ("HICUM Level 2 v2.1 verilog device");

  Props.append (new Property ("c10", "1.516E-31", false,
    QObject::tr ("GICCR constant")));
  Props.append (new Property ("qp0", "5.939E-15", false,
    QObject::tr ("Zero-bias hole charge")));
  Props.append (new Property ("ich", "1.0E11", false,
    QObject::tr ("High-current correction for 2D and 3D effects")));
  Props.append (new Property ("hfe", "1.0", false,
    QObject::tr ("Emitter minority charge weighting factor in HBTs")));
  Props.append (new Property ("hfc", "0.03999", false,
    QObject::tr ("Collector minority charge weighting factor in HBTs")));
  Props.append (new Property ("hjei", "0.435", false,
    QObject::tr ("B-E depletion charge weighting factor in HBTs")));
  Props.append (new Property ("hjci", "0.09477", false,
    QObject::tr ("B-C depletion charge weighting factor in HBTs")));
  Props.append (new Property ("ibeis", "3.47E-20", false,
    QObject::tr ("Internal B-E saturation current")));
  Props.append (new Property ("mbei", "1.025", false,
    QObject::tr ("Internal B-E current ideality factor")));
  Props.append (new Property ("ireis", "390E-12", false,
    QObject::tr ("Internal B-E recombination saturation current")));
  Props.append (new Property ("mrei", "3", false,
    QObject::tr ("Internal B-E recombination current ideality factor")));
  Props.append (new Property ("ibeps", "4.18321E-21", false,
    QObject::tr ("Peripheral B-E saturation current")));
  Props.append (new Property ("mbep", "1.045", false,
    QObject::tr ("Peripheral B-E current ideality factor")));
  Props.append (new Property ("ireps", "1.02846E-14", false,
    QObject::tr ("Peripheral B-E recombination saturation current")));
  Props.append (new Property ("mrep", "3", false,
    QObject::tr ("Peripheral B-E recombination current ideality factor")));
  Props.append (new Property ("mcf", "1.0", false,
    QObject::tr ("Non-ideality factor for III-V HBTs")));
  Props.append (new Property ("ibcis", "3.02613E-18", false,
    QObject::tr ("Internal B-C saturation current")));
  Props.append (new Property ("mbci", "1.0", false,
    QObject::tr ("Internal B-C current ideality factor")));
  Props.append (new Property ("ibcxs", "4.576E-29", false,
    QObject::tr ("External B-C saturation current")));
  Props.append (new Property ("mbcx", "1.0", false,
    QObject::tr ("External B-C current ideality factor")));
  Props.append (new Property ("ibets", "0.0", false,
    QObject::tr ("B-E tunneling saturation current")));
  Props.append (new Property ("abet", "36.74", false,
    QObject::tr ("Exponent factor for tunneling current")));
  Props.append (new Property ("favl", "14.97", false,
    QObject::tr ("Avalanche current factor")));
  Props.append (new Property ("qavl", "7.2407E-14", false,
    QObject::tr ("Exponent factor for avalanche current")));
  Props.append (new Property ("alfav", "0.0", false,
    QObject::tr ("Relative TC for FAVL")));
  Props.append (new Property ("alqav", "0.0", false,
    QObject::tr ("Relative TC for QAVL")));
  Props.append (new Property ("rbi0", "7.9", false,
    QObject::tr ("Zero bias internal base resistance")));
  Props.append (new Property ("rbx", "13.15", false,
    QObject::tr ("External base series resistance")));
  Props.append (new Property ("fgeo", "0.724", false,
    QObject::tr ("Factor for geometry dependence of emitter current crowding")));
  Props.append (new Property ("fdqr0", "0", false,
    QObject::tr ("Correction factor for modulation by B-E and B-C space charge layer")));
  Props.append (new Property ("fcrbi", "0.0", false,
    QObject::tr ("Ratio of HF shunt to total internal capacitance (lateral NQS effect)")));
  Props.append (new Property ("fqi", "1.0", false,
    QObject::tr ("Ration of internal to total minority charge")));
  Props.append (new Property ("re", "9.77", false,
    QObject::tr ("Emitter series resistance")));
  Props.append (new Property ("rcx", "10", false,
    QObject::tr ("External collector series resistance")));
  Props.append (new Property ("itss", "2.81242E-19", false,
    QObject::tr ("Substrate transistor transfer saturation current")));
  Props.append (new Property ("msf", "1.0", false,
    QObject::tr ("Forward ideality factor of substrate transfer current")));
  Props.append (new Property ("iscs", "7.6376E-17", false,
    QObject::tr ("C-S diode saturation current")));
  Props.append (new Property ("msc", "1.0", false,
    QObject::tr ("Ideality factor of C-S diode current")));
  Props.append (new Property ("tsf", "1.733E-8", false,
    QObject::tr ("Transit time for forward operation of substrate transistor")));
  Props.append (new Property ("rsu", "800", false,
    QObject::tr ("Substrate series resistance")));
  Props.append (new Property ("csu", "1.778E-14", false,
    QObject::tr ("Substrate shunt capacitance")));
  Props.append (new Property ("cjei0", "5.24382E-14", false,
    QObject::tr ("Internal B-E zero-bias depletion capacitance")));
  Props.append (new Property ("vdei", "0.9956", false,
    QObject::tr ("Internal B-E built-in potential")));
  Props.append (new Property ("zei", "0.4", false,
    QObject::tr ("Internal B-E grading coefficient")));
  Props.append (new Property ("aljei", "2.5", false,
    QObject::tr ("Ratio of maximum to zero-bias value of internal B-E capacitance")));
  Props.append (new Property ("cjep0", "0", false,
    QObject::tr ("Peripheral B-E zero-bias depletion capacitance")));
  Props.append (new Property ("vdep", "1", false,
    QObject::tr ("Peripheral B-E built-in potential")));
  Props.append (new Property ("zep", "0.01", false,
    QObject::tr ("Peripheral B-E grading coefficient")));
  Props.append (new Property ("aljep", "2.5", false,
    QObject::tr ("Ratio of maximum to zero-bias value of peripheral B-E capacitance")));
  Props.append (new Property ("cjci0", "4.46887E-15", false,
    QObject::tr ("Internal B-C zero-bias depletion capacitance")));
  Props.append (new Property ("vdci", "0.7", false,
    QObject::tr ("Internal B-C built-in potential")));
  Props.append (new Property ("zci", "0.38", false,
    QObject::tr ("Internal B-C grading coefficient")));
  Props.append (new Property ("vptci", "100", false,
    QObject::tr ("Internal B-C punch-through voltage")));
  Props.append (new Property ("cjcx0", "1.55709E-14", false,
    QObject::tr ("External B-C zero-bias depletion capacitance")));
  Props.append (new Property ("vdcx", "0.733", false,
    QObject::tr ("External B-C built-in potential")));
  Props.append (new Property ("zcx", "0.34", false,
    QObject::tr ("External B-C grading coefficient")));
  Props.append (new Property ("vptcx", "100", false,
    QObject::tr ("External B-C punch-through voltage")));
  Props.append (new Property ("fbc", "0.3487", false,
    QObject::tr ("Partitioning factor of parasitic B-C capacitance")));
  Props.append (new Property ("cjs0", "17.68E-15", false,
    QObject::tr ("C-S zero-bias depletion capacitance")));
  Props.append (new Property ("vds", "0.621625", false,
    QObject::tr ("C-S built-in potential")));
  Props.append (new Property ("zs", "0.122136", false,
    QObject::tr ("C-S grading coefficient")));
  Props.append (new Property ("vpts", "1000", false,
    QObject::tr ("C-S punch-through voltage")));
  Props.append (new Property ("t0", "1.28E-12", false,
    QObject::tr ("Low current forward transit time at VBC=0V")));
  Props.append (new Property ("dt0h", "260E-15", false,
    QObject::tr ("Time constant for base and B-C space charge layer width modulation")));
  Props.append (new Property ("tbvl", "2.0E-13", false,
    QObject::tr ("Time constant for modelling carrier jam at low VCE")));
  Props.append (new Property ("tef0", "0.0", false,
    QObject::tr ("Neutral emitter storage time")));
  Props.append (new Property ("gtfe", "1.0", false,
    QObject::tr ("Exponent factor for current dependence of neutral emitter storage time")));
  Props.append (new Property ("thcs", "46E-15", false,
    QObject::tr ("Saturation time constant at high current densities")));
  Props.append (new Property ("alhc", "0.08913", false,
    QObject::tr ("Smoothing factor for current dependence of base and collector transit time")));
  Props.append (new Property ("fthc", "0.8778", false,
    QObject::tr ("Partitioning factor for base and collector portion")));
  Props.append (new Property ("rci0", "50.4277", false,
    QObject::tr ("Internal collector resistance at low electric field")));
  Props.append (new Property ("vlim", "0.9", false,
    QObject::tr ("Voltage separating ohmic and saturation velocity regime")));
  Props.append (new Property ("vces", "0.01", false,
    QObject::tr ("Internal C-E saturation voltage")));
  Props.append (new Property ("vpt", "10", false,
    QObject::tr ("Collector punch-through voltage")));
  Props.append (new Property ("tr", "1.0E-11", false,
    QObject::tr ("Storage time for inverse operation")));
  Props.append (new Property ("ceox", "1.71992E-15", false,
    QObject::tr ("Total parasitic B-E capacitance")));
  Props.append (new Property ("ccox", "4.9E-15", false,
    QObject::tr ("Total parasitic B-C capacitance")));
  Props.append (new Property ("alqf", "0.1288", false,
    QObject::tr ("Factor for additional delay time of minority charge")));
  Props.append (new Property ("alit", "1.0", false,
    QObject::tr ("Factor for additional delay time of transfer current")));
  Props.append (new Property ("kf", "2.83667E-9", false,
    QObject::tr ("Flicker noise coefficient")));
  Props.append (new Property ("af", "2.0", false,
    QObject::tr ("Flicker noise exponent factor")));
  Props.append (new Property ("krbi", "1.0", false,
    QObject::tr ("Noise factor for internal base resistance")));
  Props.append (new Property ("latb", "10.479", false,
    QObject::tr ("Scaling factor for collector minority charge in direction of emitter width")));
  Props.append (new Property ("latl", "0.300012", false,
    QObject::tr ("Scaling factor for collector minority charge in direction of emitter length")));
  Props.append (new Property ("vgb", "1.112", false,
    QObject::tr ("Bandgap voltage extrapolated to 0 K")));
  Props.append (new Property ("alt0", "0.0017580", false,
    QObject::tr ("First order relative TC of parameter T0")));
  Props.append (new Property ("kt0", "4.07E-6", false,
    QObject::tr ("Second order relative TC of parameter T0")));
  Props.append (new Property ("zetaci", "0.7", false,
    QObject::tr ("Temperature exponent for RCI0")));
  Props.append (new Property ("zetacx", "1.0", false,
    QObject::tr ("Temperature exponent of mobility in substrate transistor transit time")));
  Props.append (new Property ("alvs", "0.001", false,
    QObject::tr ("Relative TC of saturation drift velocity")));
  Props.append (new Property ("alces", "0.000125", false,
    QObject::tr ("Relative TC of VCES")));
  Props.append (new Property ("zetarbi", "0.0", false,
    QObject::tr ("Temperature exponent of internal base resistance")));
  Props.append (new Property ("zetarbx", "0.2", false,
    QObject::tr ("Temperature exponent of external base resistance")));
  Props.append (new Property ("zetarcx", "0.21", false,
    QObject::tr ("Temperature exponent of external collector resistance")));
  Props.append (new Property ("zetare", "0.7", false,
    QObject::tr ("Temperature exponent of emitter resistance")));
  Props.append (new Property ("alb", "0.007", false,
    QObject::tr ("Relative TC of forward current gain for V2.1 model")));
  Props.append (new Property ("rth", "1293.95", false,
    QObject::tr ("Thermal resistance")));
  Props.append (new Property ("cth", "7.22203E-11", false,
    QObject::tr ("Thermal capacitance")));
  Props.append (new Property ("tnom", "27.0", false,
    QObject::tr ("Temperature at which parameters are specified")));
  Props.append (new Property ("dt", "0.0", false,
    QObject::tr ("Temperature change w.r.t. chip temperature for particular transistor")));
  Props.append (new Property ("Temp", "27", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "hicumL2V2p1";
  Name  = "T";
}

Component * hicumL2V2p1::newOne()
{
  hicumL2V2p1 * p = new hicumL2V2p1();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * hicumL2V2p1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("HICUM L2 v2.1");
  BitmapFile = (char *) "npnsub_therm";

  if(getNewOne) return new hicumL2V2p1();
  return 0;
}

void hicumL2V2p1::createSymbol()
{
  // normal bipolar
  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  // substrate node
  Lines.append(new Line(  9,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  9, -7,  9,  7,QPen(Qt::darkBlue,3)));

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
  // I
  Lines.append(new Line(-24,-30,-24,-24,QPen(Qt::darkBlue,1)));
  // C
  Lines.append(new Line(-22,-30,-22,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-22,-30,-19,-30,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-22,-24,-19,-24,QPen(Qt::darkBlue,1)));
  // U
  Lines.append(new Line(-17,-30,-17,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-14,-30,-14,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-17,-24,-14,-24,QPen(Qt::darkBlue,1)));
  // M
  Lines.append(new Line(-12,-30,-12,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line( -8,-30, -8,-24,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-12,-30,-10,-28,QPen(Qt::darkBlue,1)));
  Lines.append(new Line( -8,-30,-10,-28,QPen(Qt::darkBlue,1)));

  Ports.append(new Port(  0,-30)); // collector
  Ports.append(new Port(-30,  0)); // base
  Ports.append(new Port(  0, 30)); // emitter
  Ports.append(new Port( 30,  0)); // substrate
  Ports.append(new Port(-30, 20)); // thermal node

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;
}
