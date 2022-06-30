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
  int a = s.lastIndexOf(" ");
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
  SpiceModel = "*";
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
  Props.append(new Property("SweepModel","false",false,
                            "[true,false]"));
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

QString Param_Sweep::getNgspiceBeforeSim(QString sim, int lvl)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s,unit;
    QString par = getProperty("Param")->Value.toLower();
    QString type = getProperty("Type")->Value;
    QString step_var = par;
    step_var.remove(QRegExp("[\\.\\[\\]@:]"));

    s = QString("let number_%1 = 0\n").arg(step_var);
    if (lvl==0) s += QString("echo \"STEP %1.%2\" > spice4qucs.%3.cir.res\n").arg(sim).arg(step_var).arg(sim);
    else s += QString("echo \"STEP %1.%2\" > spice4qucs.%3.cir.res%4\n").arg(sim).arg(step_var).arg(sim).arg(lvl);

    s += QString("foreach  %1_act ").arg(step_var);

    if((type == "list") || (type == "const")) {
        QStringList List;
        List = getProperty("Values")->Value.split(";");

        for(int i = 0; i < List.length(); i++) {
            List[i].remove(QRegExp("[A-Z a-z [\\] s/' '//g]"));
            s += QString("%1 ").arg(List[i]);
        }
    } else {
        double start,stop,step,fac,points;
        misc::str2num(getProperty("Start")->Value,start,unit,fac);
        start *= fac;
        misc::str2num(getProperty("Stop")->Value,stop,unit,fac);
        stop *= fac;
        misc::str2num(getProperty("Points")->Value,points,unit,fac);
        points *= fac;

        if(type == "lin") {
            step = (stop-start)/points;
            for (; start <= stop; start += step) {
                s += QString("%1 ").arg(start);
            }
        } else {
            start = log10(start);
            stop = log10(stop);
            step = (stop - start)/points;

            for(; start <= stop; start += step) {
                s += QString("%1 ").arg(pow(10, start));
            }

            if (start - step < stop) {
                s += QString("%1 ").arg(pow(10, stop));
            }
        }
    }
    s += "\n";

    bool modelsweep = false; // Find component and its modelstring 
    QString mod,mod_par;

    if (!par.contains('@')) {
        QStringList par_lst = par.split('.',qucs::SkipEmptyParts);
        if (par_lst.count()>1) {
            mod_par = par_lst.at(1);
            // Schematic *sch = (Schematic *) QucsMain->DocumentTab->currentPage();
            Schematic *sch = getSchematic();
            Component *pc = sch->getComponentByName(par_lst.at(0));
            if (pc != NULL) {
                mod = pc->getSpiceNetlist().section('\n',1,1,QString::SectionSkipEmpty)
                                           .section(' ',1,1,QString::SectionSkipEmpty);
                if (!mod.isEmpty()) modelsweep = true;
            }
        }
    }

    if (modelsweep) { // Model parameter sweep
        s += QString("altermod %1 %2 = $%3_act\n").arg(mod).arg(mod_par).arg(step_var);
    } else {
        QString mswp = getProperty("SweepModel")->Value;
        if (mswp == "true")
            s += QString("altermod %1 = $%2_act\n").arg(par).arg(step_var);
        else s += QString("alter %1 = $%2_act\n").arg(par).arg(step_var);
    }
    return s;
}

QString Param_Sweep::getNgspiceAfterSim(QString sim, int lvl)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    QString par = getProperty("Param")->Value.toLower();
    QString type = getProperty("Type")->Value;
    par.remove(QRegExp("[\\.\\[\\]@:]"));

    s = "set appendwrite\n";

    if (lvl==0) s += QString("echo \"$&number_%1  $%2_act\">> spice4qucs.%3.cir.res\n").arg(par).arg(par).arg(sim);
    else s += QString("echo \"$&number_%1\" $%1_act >> spice4qucs.%2.cir.res%3\n").arg(par).arg(sim).arg(lvl);
    s += QString("let number_%1 = number_%1 + 1\n").arg(par);

    s += "end\n";
    s += "unset appendwrite\n";
    return s;
}

QString Param_Sweep::getCounterVar()
{
    QString par = getProperty("Param")->Value;
    par.remove(QRegExp("[\\.\\[\\]@:]"));
    QString s = QString("number_%1").arg(par);
    return s;
}

QString Param_Sweep::spice_netlist(bool isXyce)
{
    double start,stop,step,fac,points;
    QString unit;
    QString s;

    if(getProperty("Type")->Value=="list") { // List STEP variance Xyce-only
        if(isXyce) {
            QString var = getProperty("Param")->Value;
            QString list = getProperty("Values")->Value;
            list.remove('[').remove(']');
            list = list.split(';').join(" ");
            s = QString(".STEP %1 LIST %2\n").arg(var).arg(list);
            return s.toLower();
        }
    }
    if(getProperty("Type")->Value!="list" && getProperty("Type")->Value!="const"){
        misc::str2num(getProperty("Start")->Value,start,unit,fac);
        start *= fac;
        misc::str2num(getProperty("Stop")->Value,stop,unit,fac);
        stop *= fac;
        misc::str2num(getProperty("Points")->Value,points,unit,fac);
        points *= fac;
        step = (stop-start)/points;
    }

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
    return s.toLower();
}

// -------------------------------------------------------
QString Param_Sweep::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  for(unsigned int i=0; i <= Props.count()-2; i++)
    if(Props.at(i)->Name != "Symbol")
      s += " "+Props.at(i)->Name+"=\""+Props.at(i)->Value+"\"";

  return s + '\n';
}
