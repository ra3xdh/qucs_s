/***************************************************************************
                              param_sweep.cpp
                             -----------------
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
#include "param_sweep.h"
#include "main.h"
#include "qucs.h"
#include "schematic.h"
#include "misc.h"


Param_Sweep::Param_Sweep()
{
  Description = QObject::tr("Parameter sweep");

  QString  s = Description;
  int a = s.findRev(" ");
  if (a != -1) s[a] = '\n';    // break line

  Texts.append(new Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+104; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".SW";
  Name  = "SW";
  isSimulation = true;

  // The index of the first 6 properties must not changed. Used in recreate().
  Props.append(new Property("Sim", "", true,
		QObject::tr("simulation to perform parameter sweep on")));
  Props.append(new Property("Type", "lin", true,
		QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(new Property("Param", "R1", true,
		QObject::tr("parameter to sweep")));
  Props.append(new Property("Start", "5 Ohm", true,
		QObject::tr("start value for sweep")));
  Props.append(new Property("Stop", "50 Ohm", true,
		QObject::tr("stop value for sweep")));
  Props.append(new Property("Points", "20", true,
		QObject::tr("number of simulation steps")));
}

Param_Sweep::~Param_Sweep()
{
}

Component* Param_Sweep::newOne()
{
  return new Param_Sweep();
}

Element* Param_Sweep::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Parameter sweep");
  BitmapFile = (char *) "sweep";

  if(getNewOne)  return new Param_Sweep();
  return 0;
}

void Param_Sweep::recreate(Schematic*)
{
  Property *pp = Props.at(1);
  Props.next();
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

QString Param_Sweep::getNgspiceBeforeSim()
{
    QString s,unit;
    QString par = getProperty("Param")->Value;
    QString step_var = par;
    step_var.remove('.');

    double start,stop,step,fac,points;
    misc::str2num(getProperty("Start")->Value,start,unit,fac);
    start *= fac;
    misc::str2num(getProperty("Stop")->Value,stop,unit,fac);
    stop *= fac;
    misc::str2num(getProperty("Points")->Value,points,unit,fac);
    points *= fac;
    step = (stop-start)/points;
    s = QString("let start_%1 = %2\n").arg(step_var).arg(start);
    s += QString("let stop_%1 = %2\n").arg(step_var).arg(stop);
    s += QString("let %1_act=start_%1\n").arg(step_var);
    s += QString("let delta = %1\n").arg(step);
    s += "let number = 0\n";
    s += QString("echo \"STEP %1\" > spice4qucs.cir.res\n").arg(step_var);
    s += QString("while %1_act le stop_%1\n").arg(step_var);

    bool modelsweep = false; // Find component and its modelstring
    QStringList par_lst = par.split('.',QString::SkipEmptyParts);
    QString mod,mod_par;
    if (par_lst.count()>1) {
        mod_par = par_lst.at(1);
        Schematic *sch = (Schematic *) QucsMain->DocumentTab->currentPage();
        Component *pc = sch->getComponentByName(par_lst.at(0));
        if (pc != NULL) {
            mod = pc->getSpiceNetlist().section('\n',1,1,QString::SectionSkipEmpty)
                                       .section(' ',1,1,QString::SectionSkipEmpty);
            modelsweep = true;
        }
    }


    if (modelsweep) { // Model parameter sweep
        s += QString("altermod %1 %2 = %3_act\n").arg(mod).arg(mod_par).arg(step_var);
    } else {
        s += QString("alter %1 = %2_act\n").arg(par).arg(step_var);
    }
    return s;
}

QString Param_Sweep::getNgspiceAfterSim()
{
    QString s;
    QString par = getProperty("Param")->Value;
    par.remove('.');
    s = "set appendwrite\n";
    s += QString("echo \"$&number\" \"$&%1_act\" >> spice4qucs.cir.res\n").arg(par);
    s += QString("let %1_act = %1_act + delta\n").arg(par);
    s += "let number = number +1\n";
    s += "end\n";
    s += "unset appendwrite\n";
    return s;
}

QString Param_Sweep::spice_netlist(bool isXyce)
{
    double start,stop,step,fac,points;
    QString unit;
    misc::str2num(getProperty("Start")->Value,start,unit,fac);
    start *= fac;
    misc::str2num(getProperty("Stop")->Value,stop,unit,fac);
    stop *= fac;
    misc::str2num(getProperty("Points")->Value,points,unit,fac);
    points *= fac;
    step = (stop-start)/points;

    QString s;
    if (Props.at(0)->Value.startsWith("DC")) {
        QString src = getProperty("Param")->Value;
        s = QString("DC %1 %2 %3 %4\n").arg(src).arg(start).arg(stop).arg(step);
        if (isXyce) s.prepend('.');
    } else if (isXyce) {
        QString var = getProperty("Param")->Value;
        s = QString(".STEP %1 %2 %3 %4\n").arg(var).arg(start).arg(stop).arg(step);
    } else {
        s = "";
    }
    return s;
}
