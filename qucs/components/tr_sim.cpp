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
#include "main.h"


TR_Sim::TR_Sim()
{
  isSimulation = true;
  Description = QObject::tr("transient simulation");

  QString  s = Description;
  int a = s.indexOf(" ");
  if (a != -1) s[a] = '\n';

  Texts.append(new Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+104; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".TR";
  Name  = "TR";
  SpiceModel = ".TRAN";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Type", "lin", true,
	QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(new Property("Start", "0", true,
	QObject::tr("start time in seconds")));
  Props.append(new Property("Stop", "1 ms", true,
	QObject::tr("stop time in seconds")));
  Props.append(new Property("Points", "11", false,
	QObject::tr("number of simulation time steps")));
  Props.append(new Property("IntegrationMethod", "Trapezoidal", false,
	QObject::tr("integration method")+
	" [Euler, Trapezoidal, Gear, AdamsMoulton]"));
  Props.append(new Property("Order", "2", false,
	QObject::tr("order of integration method")+" (1-6)"));
  Props.append(new Property("InitialStep", "1 ns", false,
	QObject::tr("initial step size in seconds")));
  Props.append(new Property("MinStep", "1e-16", false,
	QObject::tr("minimum step size in seconds")));
  Props.append(new Property("MaxIter", "150", false,
	QObject::tr("maximum number of iterations until error")));
  Props.append(new Property("reltol", "0.001", false,
	QObject::tr("relative tolerance for convergence")));
  Props.append(new Property("abstol", "1 pA", false,
	QObject::tr("absolute tolerance for currents")));
  Props.append(new Property("vntol", "1 uV", false,
	QObject::tr("absolute tolerance for voltages")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("LTEreltol", "1e-3", false,
	QObject::tr("relative tolerance of local truncation error")));
  Props.append(new Property("LTEabstol", "1e-6", false,
	QObject::tr("absolute tolerance of local truncation error")));
  Props.append(new Property("LTEfactor", "1", false,
	QObject::tr("overestimation of local truncation error")));
  Props.append(new Property("Solver", "CroutLU", false,
	QObject::tr("method for solving the circuit matrix")+
	" [CroutLU, DoolittleLU, HouseholderQR, HouseholderLQ, GolubSVD]"));
  Props.append(new Property("relaxTSR", "no", false,
	QObject::tr("relax time step raster")+" [no, yes]"));
  Props.append(new Property("initialDC", "yes", false,
	QObject::tr("perform an initial DC analysis")+" [yes, no]"));
  Props.append(new Property("MaxStep", "0", false,
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

QString TR_Sim::spice_netlist()
{
    QString s = SpiceModel;
    QString unit;
    double Tstart,Tstop,Npoints,Tstep,fac;

    str2num(Props.at(1)->Value,Tstart,unit,fac); // Calculate Time Step
    Tstart *= fac;
    str2num(Props.at(2)->Value,Tstop,unit,fac);
    Tstop *= fac;
    Npoints = Props.at(3)->Value.toDouble();
    Tstep = (Tstop-Tstart)/Npoints;

    s += QString(" %1 %2 %3\n").arg(Tstep).arg(Tstop).arg(Tstart);
    return s;
}

void TR_Sim::recreate(Schematic*)
{
  Property *pp = Props.first();
  if((pp->Value == "list") || (pp->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    Props.next()->Name = "Values";
  }
  else {
    Props.next()->Name = "Start";
    Props.next()->Name = "Stop";
    Props.next()->Name = "Points";
  }
}
