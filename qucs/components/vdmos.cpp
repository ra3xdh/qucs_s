/***************************************************************************
                                jfet.cpp
                               ----------
    begin                : Fri Jun 4 2004
    copyright            : (C) 2003 by Michael Margraf
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

#include "vdmos.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


VDMOS::VDMOS() {
    Description = QObject::tr("VDMOS power transistor");
    Simulator = spicecompat::simNgspice;

    // this must be the first property in the list !!!
    Props.append(new Property("Type", "nchan", true,
                              QObject::tr("polarity") + " [nchan,pchan]"));
    Props.append(new Property("Mul", "1", true,
                              QObject::tr("Device multiplier")));
    Props.append(new Property("VtO", "0.0", true,
                             QObject::tr("Zero-bias threshold voltage (V)")));
    Props.append(new Property("Kp", "1.0", true,
                              QObject::tr("Transconductance parameter (A/V^2)")));
    Props.append(new Property("Phi", "0.6", false,
                              QObject::tr("Surface potential (V)")));
    Props.append(new Property("lambda", "0.0", true,
                              QObject::tr("Channel length modulation (1/V)")));
    Props.append(new Property("Theta", "0.0", false,
                              QObject::tr("Vgs influence on mobility (1/V)")));
    Props.append(new Property("Rd", "0.0", false,
                              QObject::tr("drain ohmic resistance (Ohm)")));
    Props.append(new Property("Rs", "0.0", false,
                              QObject::tr("source ohmic resistance (Ohm)")));
    Props.append(new Property("Rg", "0.0", false,
                              QObject::tr("gate ohmic resistance (Ohm)")));
    Props.append(new Property("Kf", "0.0", false,
                              QObject::tr("flicker noise coefficient")));
    Props.append(new Property("Af", "1.0", false,
                              QObject::tr("flicker noise exponent")));
    Props.append(new Property("RQ", "0.0", false,
                              QObject::tr("Quasi saturation resistance fitting parameter (Ohm)")));
    Props.append(new Property("VQ", "0.0", false,
                              QObject::tr("Quasi saturation voltage fitting parameter (V)")));
    Props.append(new Property("Mtriode", "1.0", false,
                              QObject::tr("Conductance multiplier in triode region")));
    Props.append(new Property("Subshift", "0.0", false,
                              QObject::tr("shift along gate voltage axis in the dual parameter subthreshold model (V)")));
    Props.append(new Property("Ksubthres", "0.1", false,
                              QObject::tr("slope in the single parameter subthreshold model (V)")));
    Props.append(new Property("BV", "1500", false,
                              QObject::tr("Vds breakdown voltage (V)")));
    Props.append(new Property("Ibv", "1.0e-10", false,
                              QObject::tr("Current at Vds=bv (A)")));
    Props.append(new Property("Nbv", "1.0", false,
                              QObject::tr("Vds breakdown emission coefficient")));
    Props.append(new Property("Rds", "1e7", false,
                              QObject::tr("Drain-source shunt resistance")));
    Props.append(new Property("Rb", "0.0", false,
                              QObject::tr("Body diode ohmic resistance")));
    Props.append(new Property("N", "1.0", false,
                              QObject::tr("Body diode emission coefficient")));
    Props.append(new Property("Tt", "0.0", false,
                              QObject::tr("Body diode transit time")));
    Props.append(new Property("Eg", "1.11", false,
                              QObject::tr("Body diode activation energy for temperature effect on Is")));
    Props.append(new Property("Xti", "3.0", false,
                              QObject::tr("Body diode saturation current temperature exponent")));
    Props.append(new Property("Is", "1e-14", false,
                              QObject::tr("Body diode saturation current")));
    Props.append(new Property("Vj", "0.8", false,
                              QObject::tr("Body diode junction potential")));
    Props.append(new Property("Fc", "0.5", false,
                              QObject::tr("Body diode coefficient for forward-bias depletion capacitance formula")));
    Props.append(new Property("CjO", "0.0", false,
                              QObject::tr("Zero-bias body diode junction capacitance (F)")));
    Props.append(new Property("M", "0.0", false,
                              QObject::tr("Body diode grading coefficient")));
    Props.append(new Property("Cgdmin", "0.0", false,
                              QObject::tr("Minimum non-linear G-D capacitance (F)")));
    Props.append(new Property("Cgdmax", "0.0", false,
                              QObject::tr("Maximum non-linear G-D capacitance (F)")));
    Props.append(new Property("A", "0.0", false,
                              QObject::tr("Non-linear Cgd capacitance parameter")));
    Props.append(new Property("Cgs", "0.0", false,
                              QObject::tr("Gate-source capacitance (F)")));
    Props.append(new Property("TcVth", "0.0", false,
                              QObject::tr("Linear Vth0 temperature coefficient")));
    Props.append(new Property("Mu", "0.0", false,
                              QObject::tr("Exponent of gain temperature dependency")));
    Props.append(new Property("Texp0", "0.0", false,
                              QObject::tr("Drain resistance rd0 temperature exponent")));
    Props.append(new Property("Texp1", "0.0", false,
                              QObject::tr("Drain resistance rd1 temperature exponent")));
    Props.append(new Property("Trd1", "0.0", false,
                              QObject::tr("Drain resistance linear temperature coefficient")));
    Props.append(new Property("Trd2", "0.0", false,
                              QObject::tr("Drain resistance quadratic temperature coefficient")));
    Props.append(new Property("Trg1", "0.0", false,
                              QObject::tr("Gate resistance linear temperature coefficient")));
    Props.append(new Property("Trg2", "0.0", false,
                              QObject::tr("Gate resistance quadratic temperature coefficient")));
    Props.append(new Property("Trs1", "0.0", false,
                              QObject::tr("Source resistance linear temperature coefficient")));
    Props.append(new Property("Trs2", "0.0", false,
                              QObject::tr("Source resistance quadratic temperature coefficient")));
    Props.append(new Property("Trb1", "0.0", false,
                              QObject::tr("Body resistance linear temperature coefficient")));
    Props.append(new Property("Trb2", "0.0", false,
                              QObject::tr("Body resistance quadratic temperature coefficient")));
    Props.append(new Property("TKSUBTHRES1", "0.0", false,
                              QObject::tr("Linear temperature coefficient of ksubthres")));
    Props.append(new Property("TKSUBTHRES2", "0.0", false,
                              QObject::tr("Quadratic temperature coefficient of ksubthres")));
    Props.append(new Property("Rthjc", "1.0", false,
                              QObject::tr("Thermal resistance junction-case")));
    Props.append(new Property("Cthj", "10e-6", false,
                              QObject::tr("Thermal capacitance")));
    Props.append(new Property("Rthca", "1000", false,
                              QObject::tr("Thermal resistance case-ambient (w/o heatsink)")));
    Props.append(new Property("Temp", "26.85", false,
                              QObject::tr("simulation temperature in degree Celsius")));
    Props.append(new Property("Tnom", "26.85", false,
                              QObject::tr("parameter measurement temperature")));
    Props.append(new Property("UseGlobTemp", "yes", false,
                              QObject::tr("Use global SPICE temperature")+" [yes,no]"));
    Props.append(new Property("Thermal", "off", true,
                              QObject::tr("Thermal model") + " [on,off]"));


    createSymbol();
    tx = x2 + 4;
    ty = y1 + 4;
    Model = "VDMOS";
    Name = "M";
    SpiceModel = "M";
    SpiceModelcards.append("VDMOS");
}

// -------------------------------------------------------
Component* VDMOS::newOne()
{
  VDMOS* p = new VDMOS();
  p->Props.front()->Value = Props.front()->Value;
  p->recreate();
  return p;
}

QString VDMOS::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    QString s = spicecompat::check_refdes(Name,SpiceModel);

    bool hasThermal = (getProperty("Thermal")->Value == "on");

    QList<int> pin_seq;
    if (hasThermal) {
      pin_seq<<1<<0<<2<<3<<4;
    } else {
      pin_seq<<1<<0<<2; // Pin sequence: DGS
    }
    // output all node names
    for (int pin : pin_seq) {
        QString nam = Ports.at(pin)->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }


    QStringList spice_incompat,spice_tr;
    spice_incompat<<"Type"<<"Thermal"<<"Mul"<<"UseGlobTemp";
    QString par_str = form_spice_param_list(spice_incompat,spice_tr);

    QString type = getProperty("Type")->Value;

    if (getProperty("UseGlobTemp")->Value == "yes") {
      s += QString(" VDMOS_%1 ").arg(Name);
    } else {
      s += QString(" VDMOS_%1 TEMP=%2").arg(Name).arg(getProperty("Temp")->Value);
    }

    if (hasThermal) s += " THERMAL";
    s += "\n";

        s += QString(".MODEL VDMOS_%1 VDMOS(%2 %3)\n").arg(Name).arg(type).arg(par_str);

    return s;
}
// -------------------------------------------------------
Element* VDMOS::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-VDMOS");
  BitmapFile = (char *) "nchan";

  if(getNewOne)  return new VDMOS();
  return 0;
}

// -------------------------------------------------------
Element* VDMOS::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("p-VDMOS");
  BitmapFile = (char *) "vdmos";

  if(getNewOne) {
    VDMOS* p = new VDMOS();
    p->Props.front()->Value = "pchan";
    p->recreate();
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void VDMOS::createSymbol()
{
  Lines.append(new qucs::Line(-14,-13,-14, 13,QPen(Qt::blue,2)));
  Lines.append(new qucs::Line(-30,  0,-14,  0,QPen(Qt::blue,2)));

  Lines.append(new qucs::Line(-10,-11,  0,-11,QPen(Qt::blue,2)));
  Lines.append(new qucs::Line(  0,-11,  0,-30,QPen(Qt::blue,2)));
  Lines.append(new qucs::Line(-10, 11,  0, 11,QPen(Qt::blue,2)));
  Lines.append(new qucs::Line(  0,  0,  0, 30,QPen(Qt::blue,2)));
  Lines.append(new qucs::Line(-10,  0,  0,  0,QPen(Qt::blue,2)));

  Lines.append(new qucs::Line(-10,-16,-10, -7,QPen(Qt::blue,2)));
  Lines.append(new qucs::Line(-10,  7,-10, 16,QPen(Qt::blue,2)));

  if(Props.first()->Value == "nchan") {
    Lines.append(new qucs::Line( -9,  0, -4, -5,QPen(Qt::blue,2)));
    Lines.append(new qucs::Line( -9,  0, -4,  5,QPen(Qt::blue,2)));
  }
  else {
    Lines.append(new qucs::Line( -1,  0, -6, -5,QPen(Qt::blue,2)));
    Lines.append(new qucs::Line( -1,  0, -6,  5,QPen(Qt::blue,2)));
  }

  if(Props.at(0)->Value == "nchan") {
    Lines.append(new qucs::Line(-10, -8,-10,  8,QPen(Qt::blue,2)));
  } else
    Lines.append(new qucs::Line(-10, -4,-10,  4,QPen(Qt::blue,2)));

  bool hasThermal = (getProperty("Thermal")->Value == "on");

  Texts.append(new Text(17,-15,"VDMOS",Qt::black,10.0,0.0,-1.0));
  if (hasThermal) {
    Texts.append(new Text(-25, -32, "Tj"));
    Texts.append(new Text(-45, -32, "Tc"));
  }

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));
  if (hasThermal) {
    Ports.append(new Port(  -20, -40));
    Ports.append(new Port(  -40, -40));
  }

  x1 = -45; y1 = -40;
  x2 =   4; y2 =  30;
}
