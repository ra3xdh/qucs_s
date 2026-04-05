/***************************************************************************
                                tr_sim.cpp
                               ------------
    begin                : Sat Aug 23 2003
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
#include "tr_sim.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


TR_Sim::TR_Sim()
{
  isSimulation = true;
  Description = QObject::tr("transient simulation");
  initSymbol(Description);
  Model = ".TR";
  Name  = "TR";
  SpiceModel = ".TRAN";

  Property::Builder bld;

  bld.visible().simulator(spicecompat::simAll);
  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(bld.property("Type", "lin",
	QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(bld.property("Start", "0",
	QObject::tr("start time in seconds")));
  Props.append(bld.property("Stop", "1 ms",
	QObject::tr("stop time in seconds")));
  Props.append(bld.hidden().property("Points", "200",
	QObject::tr("number of simulation time steps")));

  bld.hidden().simulator(spicecompat::simQucsator);
  Props.append(bld.property("IntegrationMethod", "Trapezoidal",
	QObject::tr("integration method")+
	" [Euler, Trapezoidal, Gear, AdamsMoulton]"));
  Props.append(bld.property("Order", "2",
	QObject::tr("order of integration method")+" (1-6)"));
  Props.append(bld.property("InitialStep", "1 ns",
	QObject::tr("initial step size in seconds")));
  Props.append(bld.property("MinStep", "1e-16",
	QObject::tr("minimum step size in seconds")));
  Props.append(bld.property("MaxIter", "150",
	QObject::tr("maximum number of iterations until error")));
  Props.append(bld.property("reltol", "0.001",
	QObject::tr("relative tolerance for convergence")));
  Props.append(bld.property("abstol", "1 pA",
	QObject::tr("absolute tolerance for currents")));
  Props.append(bld.property("vntol", "1 uV",
	QObject::tr("absolute tolerance for voltages")));
  Props.append(bld.property("Temp", "26.85",
	QObject::tr("simulation temperature in degree Celsius")));
  Props.append(bld.property("LTEreltol", "1e-3",
	QObject::tr("relative tolerance of local truncation error")));
  Props.append(bld.property("LTEabstol", "1e-6",
	QObject::tr("absolute tolerance of local truncation error")));
  Props.append(bld.property("LTEfactor", "1",
	QObject::tr("overestimation of local truncation error")));
  Props.append(bld.property("Solver", "CroutLU",
	QObject::tr("method for solving the circuit matrix")+
	" [CroutLU, DoolittleLU, HouseholderQR, HouseholderLQ, GolubSVD]"));
  Props.append(bld.property("relaxTSR", "no",
	QObject::tr("relax time step raster")+" [no, yes]"));

  bld.hidden().simulator(spicecompat::simAll);
  Props.append(bld.property("initialDC", "yes",
    QObject::tr("perform initial DC (set \"no\" to activate UIC)")+" [yes, no]"));
  Props.append(bld.property("MaxStep", "0",
	QObject::tr("maximum step size in seconds")));
}

TR_Sim::~TR_Sim()
{
}

Component* TR_Sim::newOne()
{
  return new TR_Sim();
}

Element* TR_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Transient simulation");
  BitmapFile = (char *) "tran";

  if(getNewOne)  return new TR_Sim();
  return 0;
}

QString TR_Sim::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    QString s = SpiceModel;
    QString unit;
    double Tstart,Tstop,Npoints,Tstep,fac;

    misc::str2num(Props.at(1)->Value,Tstart,unit,fac); // Calculate Time Step
    Tstart *= fac;
    misc::str2num(Props.at(2)->Value,Tstop,unit,fac);
    Tstop *= fac;
    Npoints = Props.at(3)->Value.toDouble();
    Tstep = (Tstop-Tstart)/(Npoints-1);

    if (dialect == spicecompat::SPICEXyce) {
      // Force fixed output interval equal to Tstep so that parameter sweeps
      // produce the same number of time points for every step.
      // Without this Xyce uses adaptive internal timesteps giving unequal point counts.
      // This makes Qucs-S unable to load the data as it assumes the same time vector
      // for all iterations.
      s += QStringLiteral(" %1 %2 %3 %4").arg(Tstep).arg(Tstop).arg(Tstart).arg(Tstep);
    } else {
      s += QStringLiteral(" %1 %2 %3 ").arg(Tstep).arg(Tstop).arg(Tstart);
      QString max_step = spicecompat::normalize_value(getProperty("MaxStep")->Value);
      if (max_step!="0") s+= max_step;
}

    if (Props.at(18)->Value == "no") s += " UIC";

    s += "\n";
    if (dialect != spicecompat::SPICEXyce) s.remove(0,1);
    return s.toLower();
}

void TR_Sim::recreate()
{
  if((Props.at(0)->Value == "list") || (Props.at(0)->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    Props.at(1)->Name = "Symbol";
    Props.at(1)->display = false;
    Props.at(2)->Name = "Symbol";
    Props.at(2)->display = false;
    Props.at(3)->Name = "Values";
  }
  else {
    Props.at(1)->Name = "Start";
    Props.at(2)->Name = "Stop";
    Props.at(3)->Name = "Points";
  }
}
