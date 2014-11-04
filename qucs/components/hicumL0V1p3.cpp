/*
 * hicumL0V1p3.cpp - device implementations for hicumL0V1p3 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "node.h"
#include "hicumL0V1p3.h"

hicumL0V1p3::hicumL0V1p3()
{
  Description = QObject::tr ("HICUM Level 0 v1.3 verilog device");

  Props.append (new Property ("Type", "npn", true,
    QObject::tr ("polarity") + " [npn, pnp]"));
  Props.append (new Property ("is", "1.0e-16", false,
    QObject::tr ("(Modified) saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("it_mod", "0", false,
    QObject::tr ("Flag for using third order solution for transfer current")));
  Props.append (new Property ("mcf", "1.00", false,
    QObject::tr ("Non-ideality coefficient of forward collector current")));
  Props.append (new Property ("mcr", "1.00", false,
    QObject::tr ("Non-ideality coefficient of reverse collector current")));
  Props.append (new Property ("vef", "1.0e6", false,
    QObject::tr ("forward Early voltage (normalization volt.)")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("ver", "1.0e6", false,
    QObject::tr ("reverse Early voltage (normalization volt.)")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("aver", "0.0", false,
    QObject::tr ("bias dependence for reverse Early voltage")));
  Props.append (new Property ("iqf", "1.0e6", false,
    QObject::tr ("forward d.c. high-injection roll-off current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("fiqf", "0", false,
    QObject::tr ("flag for turning on base related critical current")));
  Props.append (new Property ("iqr", "1.0e6", false,
    QObject::tr ("inverse d.c. high-injection roll-off current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("iqfh", "1.0e6", false,
    QObject::tr ("high-injection correction current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("tfh", "0.0", false,
    QObject::tr ("high-injection correction factor")));
  Props.append (new Property ("ahq", "0", false,
    QObject::tr ("Smoothing factor for the d.c. injection width")));
  Props.append (new Property ("ibes", "1e-18", false,
    QObject::tr ("BE saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mbe", "1.0", false,
    QObject::tr ("BE non-ideality factor")));
  Props.append (new Property ("ires", "0.0", false,
    QObject::tr ("BE recombination saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mre", "2.0", false,
    QObject::tr ("BE recombination non-ideality factor")));
  Props.append (new Property ("ibcs", "0.0", false,
    QObject::tr ("BC saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("mbc", "1.0", false,
    QObject::tr ("BC non-ideality factor")));
  Props.append (new Property ("cje0", "1.0e-20", false,
    QObject::tr ("Zero-bias BE depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vde", "0.9", false,
    QObject::tr ("BE built-in voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("ze", "0.5", false,
    QObject::tr ("BE exponent factor")));
  Props.append (new Property ("aje", "2.5", false,
    QObject::tr ("Ratio of maximum to zero-bias value")));
  Props.append (new Property ("vdedc", "0.9", false,
    QObject::tr ("BE charge built-in voltage for d.c. transfer current")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zedc", "0.5", false,
    QObject::tr ("charge BE exponent factor for d.c. transfer current")));
  Props.append (new Property ("ajedc", "2.5", false,
    QObject::tr ("BE capacitance ratio (maximum to zero-bias value) for d.c. transfer current")));
  Props.append (new Property ("t0", "0.0", false,
    QObject::tr ("low current transit time at Vbici=0")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("dt0h", "0.0", false,
    QObject::tr ("Base width modulation contribution")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("tbvl", "0.0", false,
    QObject::tr ("SCR width modulation contribution")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("tef0", "0.0", false,
    QObject::tr ("Storage time in neutral emitter")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("gte", "1.0", false,
    QObject::tr ("Exponent factor for emitter transit time")));
  Props.append (new Property ("thcs", "0.0", false,
    QObject::tr ("Saturation time at high current densities")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("ahc", "0.1", false,
    QObject::tr ("Smoothing factor for current dependence")));
  Props.append (new Property ("tr", "0.0", false,
    QObject::tr ("Storage time at inverse operation")
    +" ("+QObject::tr ("s")+")"));
  Props.append (new Property ("rci0", "150", false,
    QObject::tr ("Low-field collector resistance under emitter")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("vlim", "0.5", false,
    QObject::tr ("Voltage dividing ohmic and satur.region")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vpt", "100", false,
    QObject::tr ("Punch-through voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vces", "0.1", false,
    QObject::tr ("Saturation voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("cjci0", "1.0e-20", false,
    QObject::tr ("Total zero-bias BC depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vdci", "0.7", false,
    QObject::tr ("BC built-in voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zci", "0.333", false,
    QObject::tr ("BC exponent factor")));
  Props.append (new Property ("vptci", "100", false,
    QObject::tr ("Punch-through voltage of BC junction")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("cjcx0", "1.0e-20", false,
    QObject::tr ("Zero-bias external BC depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vdcx", "0.7", false,
    QObject::tr ("External BC built-in voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zcx", "0.333", false,
    QObject::tr ("External BC exponent factor")));
  Props.append (new Property ("vptcx", "100", false,
    QObject::tr ("Punch-through voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("fbc", "1.0", false,
    QObject::tr ("Split factor = Cjci0/Cjc0")));
  Props.append (new Property ("rbi0", "0.0", false,
    QObject::tr ("Internal base resistance at zero-bias")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("vr0e", "2.5", false,
    QObject::tr ("forward Early voltage (normalization volt.)")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vr0c", "1.0e6", false,
    QObject::tr ("forward Early voltage (normalization volt.)")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("fgeo", "0.656", false,
    QObject::tr ("Geometry factor")));
  Props.append (new Property ("rbx", "0.0", false,
    QObject::tr ("External base series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("rcx", "0.0", false,
    QObject::tr ("Emitter series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("re", "0.0", false,
    QObject::tr ("External collector series resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("itss", "0.0", false,
    QObject::tr ("Substrate transistor transfer saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("msf", "1.0", false,
    QObject::tr ("Substrate transistor transfer current non-ideality factor")));
  Props.append (new Property ("iscs", "0.0", false,
    QObject::tr ("SC saturation current")
    +" ("+QObject::tr ("A")+")"));
  Props.append (new Property ("msc", "1.0", false,
    QObject::tr ("SC non-ideality factor")));
  Props.append (new Property ("cjs0", "1.0e-20", false,
    QObject::tr ("Zero-bias SC depletion capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("vds", "0.3", false,
    QObject::tr ("SC built-in voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("zs", "0.3", false,
    QObject::tr ("External SC exponent factor")));
  Props.append (new Property ("vpts", "100", false,
    QObject::tr ("SC punch-through voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("cbcpar", "0.0", false,
    QObject::tr ("Collector-base isolation (overlap) capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("cbepar", "0.0", false,
    QObject::tr ("Emitter-base oxide capacitance")
    +" ("+QObject::tr ("F")+")"));
  Props.append (new Property ("eavl", "0.0", false,
    QObject::tr ("Exponent factor")));
  Props.append (new Property ("kavl", "0.0", false,
    QObject::tr ("Prefactor")));
  Props.append (new Property ("kf", "0.0", false,
    QObject::tr ("flicker noise coefficient")
    +" ("+QObject::tr ("M^(1-AF)")+")"));
  Props.append (new Property ("af", "2.0", false,
    QObject::tr ("flicker noise exponent factor")));
  Props.append (new Property ("vgb", "1.2", false,
    QObject::tr ("Bandgap-voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vge", "1.17", false,
    QObject::tr ("Effective emitter bandgap-voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vgc", "1.17", false,
    QObject::tr ("Effective collector bandgap-voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("vgs", "1.17", false,
    QObject::tr ("Effective substrate bandgap-voltage")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("f1vg", "-1.02377e-4", false,
    QObject::tr ("Coefficient K1 in T-dependent bandgap equation")
    +" ("+QObject::tr ("V/K")+")"));
  Props.append (new Property ("f2vg", "4.3215e-4", false,
    QObject::tr ("Coefficient K2 in T-dependent bandgap equation")
    +" ("+QObject::tr ("V/K")+")"));
  Props.append (new Property ("alt0", "0.0", false,
    QObject::tr ("Frist-order TC of tf0")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("kt0", "0.0", false,
    QObject::tr ("Second-order TC of tf0")
    +" ("+QObject::tr ("1/K^2")+")"));
  Props.append (new Property ("zetact", "3.0", false,
    QObject::tr ("Exponent coefficient in transfer current temperature dependence")));
  Props.append (new Property ("zetabet", "3.5", false,
    QObject::tr ("Exponent coefficient in BE junction current temperature dependence")));
  Props.append (new Property ("zetaci", "0.0", false,
    QObject::tr ("TC of epi-collector diffusivity")));
  Props.append (new Property ("alvs", "0.0", false,
    QObject::tr ("Relative TC of satur.drift velocity")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("alces", "0.0", false,
    QObject::tr ("Relative TC of vces")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("zetarbi", "0.0", false,
    QObject::tr ("TC of internal base resistance")));
  Props.append (new Property ("zetarbx", "0.0", false,
    QObject::tr ("TC of external base resistance")));
  Props.append (new Property ("zetarcx", "0.0", false,
    QObject::tr ("TC of external collector resistance")));
  Props.append (new Property ("zetare", "0.0", false,
    QObject::tr ("TC of emitter resistances")));
  Props.append (new Property ("zetaiqf", "0.0", false,
    QObject::tr ("TC of iqf")));
  Props.append (new Property ("alkav", "0.0", false,
    QObject::tr ("TC of avalanche prefactor")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("aleav", "0.0", false,
    QObject::tr ("TC of avalanche exponential factor")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("zetarth", "0.0", false,
    QObject::tr ("Exponent factor for temperature dependent thermal resistance")));
  Props.append (new Property ("tef_temp", "1", false,
    QObject::tr ("Flag for turning temperature dependence of tef0 on and off")));
  Props.append (new Property ("zetaver", "-1.0", false,
    QObject::tr ("TC of Reverse Early voltage")));
  Props.append (new Property ("zetavgbe", "1.0", false,
    QObject::tr ("TC of AVER")));
  Props.append (new Property ("dvgbe", "0.0", false,
    QObject::tr ("Bandgap difference between base and BE-junction")));
  Props.append (new Property ("aliqfh", "0", false,
    QObject::tr ("Frist-order TC of iqfh")
    +" ("+QObject::tr ("1/K")+")"));
  Props.append (new Property ("kiqfh", "0", false,
    QObject::tr ("Second-order TC of iqfh")
    +" ("+QObject::tr ("1/K^2")+")"));
  Props.append (new Property ("flsh", "0", false,
    QObject::tr ("Flag for self-heating calculation")));
  Props.append (new Property ("rth", "0.0", false,
    QObject::tr ("Thermal resistance")
    +" ("+QObject::tr ("K/W")+")"));
  Props.append (new Property ("cth", "0.0", false,
    QObject::tr ("Thermal capacitance")
    +" ("+QObject::tr ("Ws/K")+")"));
  Props.append (new Property ("tnom", "27", false,
    QObject::tr ("Temperature for which parameters are valid")
    +" ("+QObject::tr ("C")+")"));
  Props.append (new Property ("dt", "0.0", false,
    QObject::tr ("Temperature change for particular transistor")
    +" ("+QObject::tr ("K")+")"));
  Props.append (new Property ("Temp", "27", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "hicumL0V1p3";
  Name  = "T";
}

Component * hicumL0V1p3::newOne()
{
  hicumL0V1p3 * p = new hicumL0V1p3();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * hicumL0V1p3::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("npn HICUM L0 v1.3");
  BitmapFile = (char *) "pnpsub_therm";

  if(getNewOne) return new hicumL0V1p3();
  return 0;
}

Element * hicumL0V1p3::info_pnp(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("pnp HICUM L0 v1.3");
  BitmapFile = (char *) "pnpsub_therm";

  if(getNewOne)
  {
    hicumL0V1p3* p = new hicumL0V1p3();
    p->Props.getFirst()->Value = "pnp";
    p->recreate(0);
    return p;
  }
  return 0;
}

void hicumL0V1p3::createSymbol()
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
  if(Props.getFirst()->Value == "npn") {
    Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
  } else {
    Lines.append(new Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
  }

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

QString hicumL0V1p3::netlist()
{
  QString s = "hicumL0V1p3:"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names

  // output type npn/pnp property
  Property *p2 = Props.first();
  if(p2->Value == "npn")
    s += " npn=\"1\"";
  else
    s += " pnp=\"1\"";

  // output all remaining properties
  for(p2 = Props.next(); p2 != 0; p2 = Props.next())
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + '\n';
}
