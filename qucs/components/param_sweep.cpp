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
#include "schematic.h"
#include "misc.h"

Param_Sweep::Param_Sweep()
{
  Description = QObject::tr("Parameter sweep");

  QString  s = Description;
  initSymbol(Description);
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
  if((Props.at(1)->Value == "list") || (Props.at(1)->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    Props.at(2)->Name = "Symbol";
    Props.at(2)->display = false;
    Props.at(3)->Name = "Symbol";
    Props.at(3)->display = false;
    Props.at(4)->Name = "Values";
  }
  else {
    Props.at(2)->Name = "Start";
    Props.at(3)->Name = "Stop";
    Props.at(4)->Name = "Points";
  }
}

QString Param_Sweep::getNgspiceBeforeSim(QString sim, int lvl)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s,unit;
    QStringList parameter_list = getProperty("Param")->Value.split( this->param_split_str );
    QStringList::const_iterator constListIterator;
    QString type = getProperty("Type")->Value;
    QString step_var = parameter_list.begin()->toLower();// use first element name as variable name
    step_var.remove(QRegularExpression("[\\.\\[\\]@:]"));

    s = "option interp\n";
    s += QString("let number_%1 = 0\n").arg(step_var);
    if (lvl==0) s += QString("echo \"STEP %1.%2\" > spice4qucs.%3.cir.res\n").arg(sim).arg(step_var).arg(sim);
    else s += QString("echo \"STEP %1.%2\" > spice4qucs.%3.cir.res%4\n").arg(sim).arg(step_var).arg(sim).arg(lvl);

    s += QString("foreach  %1_act ").arg(step_var);

    if((type == "list") || (type == "const")) {
        QString list_str = getProperty("Values")->Value;
        list_str.remove(0,1); // remove  [ ]
        list_str.chop(1);
        QStringList List = list_str.split(";");
        for(int i = 0; i < List.length(); i++) {
            s += QString("%1 ").arg(List[i]);
        }
    } else {
        double start,stop,step,fac,points,ostart,ostop;
        misc::str2num(getProperty("Start")->Value,ostart,unit,fac);
        ostart *= fac;
        misc::str2num(getProperty("Stop")->Value,ostop,unit,fac);
        ostop *= fac;
        misc::str2num(getProperty("Points")->Value,points,unit,fac);
        points *= fac;

        start = std::min(ostart,ostop);
        stop = std::max(ostart,ostop);

        if(type == "lin") {
            step = (stop-start)/(points-1);
            while ( points > 0 ) {
                s += QString("%1 ").arg(start);
                start += step;
                points -= 1;
            }
        } else {
            start = log10(start);
            stop = log10(stop);
            step = (stop - start)/(points - 1);

            while ( points > 0 ) {
                s += QString("%1 ").arg(pow(10, start));
                start += step;
                points -= 1;
            }
        }
    }
    s += "\n"; // newline after step listing
    QString nline_char('\n');
    for(constListIterator=parameter_list.begin(); constListIterator!=parameter_list.end();++constListIterator)
    {
        QString par = *constListIterator;
        bool compfound = false;
        bool temper_sweep = false;

        Schematic *sch = getSchematic();
        Component *pc = sch->getComponentByName(getProperty("Param")->Value);
        if (pc != NULL) compfound = true;
        else compfound = false;

        if (step_var == "temp" || step_var == "temper") temper_sweep = true;

        if (temper_sweep) {
          s += QString("option temp = $%1_act%2").arg(step_var).arg(nline_char);
        } else if (compfound) {
          s += QString("alter %1 = $%2_act%3").arg(par).arg(step_var).arg(nline_char);
        } else {
          s += QString("alterparam %1 = $%2_act%3reset%3").arg(par).arg(step_var).arg(nline_char);
        }
    }
    return s;
}

QString Param_Sweep::getNgspiceAfterSim(QString sim, int lvl)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    QStringList parameter_list = getProperty("Param")->Value.split( this->param_split_str );
    QString par = parameter_list.begin()->toLower();
    QString type = getProperty("Type")->Value;
    par.remove(QRegularExpression("[\\.\\[\\]@:]"));

    s = "set appendwrite\n";

    if (lvl==0) s += QString("echo \"$&number_%1  $%2_act\" >> spice4qucs.%3.cir.res\n").arg(par).arg(par).arg(sim);
    else s += QString("echo \"$&number_%1\" $%1_act >> spice4qucs.%2.cir.res%3\n").arg(par).arg(sim).arg(lvl);
    s += QString("let number_%1 = number_%1 + 1\n").arg(par);

    s += "end\n";
    s += "unset appendwrite\n";
    return s;
}

QString Param_Sweep::getCounterVar()
{
    QString par = getProperty("Param")->Value;
    par.remove(QRegularExpression("[\\.\\[\\]@:]"));
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
        step = (stop-start)/(points-1);
    }

    if (Props.at(0)->Value.toLower().startsWith("dc")) {
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

