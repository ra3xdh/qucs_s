/***************************************************************************
                                sp_sim.cpp
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
#include "sp_sim.h"
#include "main.h"
#include "misc.h"
#include "schematic.h"


SP_Sim::SP_Sim()
{
  isSimulation = true;
  Description = QObject::tr("S parameter simulation");

  QString s = Description;
  int a = s.indexOf(" ");
  int b = s.lastIndexOf(" ");
  if (a != -1 && b != -1) {
    if (a > (int) s.length() - b)  b = a;
  }
  if (b != -1) s[b] = '\n';

  Texts.append(new Text(0, 0, s.left(b), Qt::darkBlue, QucsSettings.largeFontSize));
  if (b != -1)
    Texts.append(new Text(0, 0, s.mid(b+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+121; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".SP";
  Name  = "SP";
  SpiceModel = ".SP";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Type", "lin", true,
	QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(new Property("Start", "1 GHz", true,
	QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "10 GHz", true,
	QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "19", true,
	QObject::tr("number of simulation steps")));
  Props.append(new Property("Noise", "no", false,
	QObject::tr("calculate noise parameters")+
	" [yes, no]"));
  Props.append(new Property("NoiseIP", "1", false,
	QObject::tr("input port for noise figure")));
  Props.append(new Property("NoiseOP", "2", false,
	QObject::tr("output port for noise figure")));
  Props.append(new Property("saveCVs", "no", false,
	QObject::tr("put characteristic values into dataset")+
	" [yes, no]"));
  Props.append(new Property("saveAll", "no", false,
	QObject::tr("save subcircuit characteristic values into dataset")+
	" [yes, no]"));
}

SP_Sim::~SP_Sim()
{
}

Component* SP_Sim::newOne()
{
  return new SP_Sim();
}

Element* SP_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("S-parameter simulation");
  BitmapFile = (char *) "sparameter";

  if(getNewOne)  return new SP_Sim();
  return 0;
}

void SP_Sim::recreate(Schematic*)
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

int SP_Sim::getSPortsNumber()
{
    int p_num = 0;
    if (containingSchematic != NULL) {
        auto comps = containingSchematic->DocComps;
        for(Component *pc = comps.first(); pc != 0; pc = comps.next()) {
            if (pc->Model == "Pac") p_num++;
        }
        return  p_num;
    } else {
        return p_num;
    }
}

QString SP_Sim::ngspice_netlist()
{
    QString s = "*Stub!\n";
    return s;
}

QString SP_Sim::xyce_netlist()
{
    QString s = ".AC ";
    QString unit;
    if (Props.at(0)->Value=="log") { // convert points number for spice compatibility
        double Np,Fstart,Fstop,fac = 1.0;
        misc::str2num(Props.at(3)->Value,Np,unit,fac); // Points number
        Np *= fac;
        misc::str2num(Props.at(1)->Value,Fstart,unit,fac);
        Fstart *= fac;
        misc::str2num(Props.at(2)->Value,Fstop,unit,fac);
        Fstop *= fac;
        double Nd = ceil(log10(Fstop/Fstart)); // number of decades
        double Npd = ceil(Np/Nd); // points per decade
        s += QString("DEC %1 ").arg(Npd);
    } else {  // no need conversion
        s += QString("LIN %1 ").arg(Props.at(3)->Value);
    }
    QString fstart = spicecompat::normalize_value(Props.at(1)->Value); // Start freq.
    QString fstop = spicecompat::normalize_value(Props.at(2)->Value); // Stop freq.
    s += QString("%1 %2 \n").arg(fstart).arg(fstop);
    s += ".LIN format=touchstone sparcalc=1\n"; // enable s-param
    int ports_num = getSPortsNumber();
    s += ".PRINT ac format=std file=spice4qucs_sparam.prn ";
    for (int i = 0; i < ports_num; i++) {
        for (int j = 0; j < ports_num; j++) {
            s += QString(" sdb(%1,%2) s(%1,%2) sp(%1,%2) y(%1,%2) z(%1,%2) ").arg(i+1).arg(j+1);
        }
    }
    s += "\n";
    return s;
}

QString SP_Sim::spice_netlist(bool isXyce)
{
    if (isXyce) {
        return xyce_netlist();
    } else {
        return ngspice_netlist();
    }
}
