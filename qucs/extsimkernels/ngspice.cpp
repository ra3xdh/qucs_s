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
#include "components/param_sweep.h"

Ngspice::Ngspice(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, parent)
{
    simulator_cmd = "ngspice";
    simulator_parameters = "";
}

// Reads ngspice simulation results and merges it in single Qucs dataset

void Ngspice::createNetlist(QTextStream &stream, int ,
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
           if (sim_typ==".CUSTOMSIM") simulations.append("custom");
           if ((sim_typ==".SW")&&
               (pc->Props.at(0)->Value.startsWith("DC"))) simulations.append("dc");
           // stream<<s;
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
    }
    vars.sort();
    qDebug()<<vars;

    stream<<".control\n"          //execute simulations
          <<"set filetype=ascii\n";

    QString sim;
    outputs.clear();

    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->Model=="Eqn") {
            Equation *eq = (Equation *)pc;
            stream<<eq->getNgspiceScript();
        }
    }

    foreach(sim, simulations) {

        bool hasParSWP = false;

        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            QString sim_typ = pc->Model;
            if (sim_typ==".SW") {
                QString SwpSim = pc->Props.at(0)->Value;
                Param_Sweep *ParSWP = (Param_Sweep *)pc;
                QString s = ParSWP->getNgspiceBeforeSim();
                if (SwpSim.startsWith("AC")&&(sim=="ac")) {
                    stream<<s;
                    hasParSWP = true;
                } else if (SwpSim.startsWith("TR")&&(sim=="tran")) {
                    stream<<s;
                    hasParSWP = true;
                } else if (SwpSim.startsWith("SW")&&(sim=="dc")) {
                    for(Component *pc1 = Sch->DocComps.first(); pc1 != 0; pc1 = Sch->DocComps.next()) {
                        if ((pc1->Name==SwpSim)&&(pc1->Props.at(0)->Value.startsWith("DC"))) {
                            stream<<s;
                            hasParSWP = true;
                        }
                    }
                }
            }
        }


        QString custom_vars;
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
           if(pc->isSimulation) {
               QString sim_typ = pc->Model;
               QString s = pc->getSpiceNetlist();
               if ((sim_typ==".AC")&&(sim=="ac")) stream<<s;
               if ((sim_typ==".TR")&&(sim=="tran")) stream<<s;
               if ((sim_typ==".CUSTOMSIM")&&(sim=="custom")) {
                   stream<<s;
                   custom_vars = pc->Props.at(1)->Value;
                   custom_vars.replace(";"," ");
               }
               if (sim_typ==".SW") {
                   QString SwpSim = pc->Props.at(0)->Value;
                   if (SwpSim.startsWith("DC")&&(sim=="dc")) stream<<s;
               }
           }
        }


        QFileInfo inf(Sch->DocName);
        QString basenam = inf.baseName();

        if (sim=="custom") {
            QString filename = basenam + "_custom.txt";
            outputs.append(filename);
            QString write_str = QString("write %1 %2\n").arg(filename).arg(custom_vars);
            stream<<write_str;
            continue;
        }


        QStringList vars_eq;
        vars_eq.clear();
        QStringList Eqns;
        Eqns.clear();
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            if (pc->Model == "Eqn") {
                Equation *eq = (Equation *)pc;
                QStringList v1;
                QString s_eq = eq->getEquations(sim,v1);
                stream<< s_eq;
                Eqns.append(s_eq.split("\n"));
                vars_eq.append(v1);
            }
        }

        QString nod;
        QString nods;
        nods.clear();
        foreach (nod,vars) {
            if (!nod.endsWith("#branch")) {
                nods += QString("v(%1) ").arg(nod);
            } else {
                nods += QString("%1 ").arg(nod);
            }
        }
        for (QStringList::iterator it = vars_eq.begin();it != vars_eq.end(); it++) {
            nods += " " + *it;
        }

        QString filename;
        if (hasParSWP) filename = QString("%1_%2_swp.txt").arg(basenam).arg(sim);
        else filename = QString("%1_%2.txt").arg(basenam).arg(sim);

        QString write_str = QString("write %1 %2\n").arg(filename).arg(nods);
        stream<<write_str;
        outputs.append(filename);

        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            QString sim_typ = pc->Model;
            if (sim_typ==".SW") {
                Param_Sweep *ParSWP = (Param_Sweep *)pc;
                QString s = ParSWP->getNgspiceAfterSim();
                QString SwpSim = pc->Props.at(0)->Value;
                if (SwpSim.startsWith("AC")&&(sim=="ac")) stream<<s;
                else if (SwpSim.startsWith("TR")&&(sim=="tran")) stream<<s;
                else if (SwpSim.startsWith("SW")&&(sim=="dc")) {
                    for(Component *pc1 = Sch->DocComps.first(); pc1 != 0; pc1 = Sch->DocComps.next()) {
                        if ((pc1->Name==SwpSim)&&(pc1->Props.at(0)->Value.startsWith("DC"))) {
                             stream<<s;
                        }
                    }
               }
            }
        }

        stream<<"destroy all\n";
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
