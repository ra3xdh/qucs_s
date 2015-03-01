/***************************************************************************
                               ngspice.cpp
                             ----------------
    begin                : Sat Jan 10 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "ngspice.h"
#include "components/iprobe.h"
#include "components/vprobe.h"
#include "components/equation.h"

Ngspice::Ngspice(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, parent)
{
    simulator_cmd = "ngspice";
    simulator_parameters = "";
}

// Reads ngspice simulation results and merges it in single Qucs dataset

void Ngspice::createNetlist(QTextStream &stream, int NumPorts,
                       QStringList &simulations, QStringList &vars, QStringList &outputs)
{
    QString s;
    if(!prepareSpiceNetlist(stream)) return; // Unable to perform spice simulation
    startNetlist(stream); // output .PARAM and components
    // determine which simulations are in use
    simulations.clear();
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
       if(pc->isSimulation) {
           s = pc->getSpiceNetlist();
           QString sim_typ = pc->Model;
           if (sim_typ==".AC") simulations.append("ac");
           if (sim_typ==".TR") simulations.append("tran");
           if ((sim_typ==".SW")&&
               (pc->Props.at(0)->Value.startsWith("DC"))) simulations.append("dc");
           stream<<s;
       }
    }

    // set variable names for named nodes and wires
    vars.clear();
    for(Node *pn = Sch->DocNodes.first(); pn != 0; pn = Sch->DocNodes.next()) {
      if(pn->Label != 0) {
          if (!vars.contains(pn->Label->Name)) {
              vars.append(pn->Label->Name);
          }
      }
    }
    for(Wire *pw = Sch->DocWires.first(); pw != 0; pw = Sch->DocWires.next()) {
      if(pw->Label != 0) {
          if (!vars.contains(pw->Label->Name)) {
              vars.append(pw->Label->Name);
          }
      }
    }
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->isProbe) {
            QString var_pr = pc->getProbeVariable();
            if (!vars.contains(var_pr)) {
                vars.append(var_pr);
            }
        }
        /*if (pc->isEquation) {
            Equation *eq = (Equation *)pc;
            QStringList vars_eq;
            eq->getDepVars(vars_eq);
            vars.append(vars_eq);
        }*/
    }
    vars.sort();
    qDebug()<<vars;

    stream<<".control\n"          //execute simulations
          <<"set filetype=ascii\n"
          <<"run\n";

    QStringList vars_eq;
    QStringList Eqns;
    Eqns.clear();
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->isEquation) {
            Equation *eq = (Equation *)pc;
            QString s_eq = eq->getEquations();
            stream<< s_eq;
            Eqns.append(s_eq.split("\n"));
            QStringList v1;
            eq->getDepVars(v1);
            vars_eq.append(v1);
        }
    }


    QFileInfo inf(Sch->DocName);
    QString basenam = inf.baseName();

    QString sim;                 // see results
    outputs.clear();
    foreach (sim,simulations) {
        QString nod;
        QString nods;
        nods.clear();
        foreach (nod,vars) {
            if (!nod.endsWith("#branch")) {
                nods += QString("%1.v(%2) ").arg(sim).arg(nod);
            } else {
                nods += QString("%1.%2 ").arg(sim).arg(nod);
            }
        }
        for (int i=0;i<Eqns.count();i++) {
            if (Eqns.at(i).contains(sim+".")) nods += " " + vars_eq.at(i);
        }
        QString filename = QString("%1_%2.txt").arg(basenam).arg(sim);
        QString write_str = QString("write %1 %2\n").arg(filename).arg(nods);
        stream<<write_str;
        outputs.append(filename);
    }

    stream<<"exit\n"
          <<".endc\n";

    stream<<".END\n";
    qDebug()<<outputs;
}

void Ngspice::slotSimulate()
{
    int num=0;
    sims.clear();
    vars.clear();
    QString tmp_path = QDir::convertSeparators(workdir+"/spice4qucs.cir");
    QFile spice_file(tmp_path);
    if (spice_file.open(QFile::WriteOnly)) {
        QTextStream stream(&spice_file);
        createNetlist(stream,num,sims,vars,output_files);
        spice_file.close();
    }
    qDebug()<<sims<<vars;

    //startNgSpice(tmp_path);
    SimProcess->setWorkingDirectory(workdir);
    QString cmd = QString("%1 %2 %3").arg(simulator_cmd,simulator_parameters,tmp_path);
    SimProcess->start(cmd);
    emit started();
}
