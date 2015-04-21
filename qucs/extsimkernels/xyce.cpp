/***************************************************************************
                               xyce.cpp
                             ----------------
    begin                : Fri Jan 16 2015
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


#include "xyce.h"
#include "components/equation.h"
#include "main.h"

Xyce::Xyce(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, parent)
{
    simulator_cmd = QucsSettings.XyceExecutable;
    simulator_parameters = "-a";
}

void Xyce::determineUsedSimulations()
{

    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
       if(pc->isSimulation) {
           QString sim_typ = pc->Model;
           if (sim_typ==".AC") simulationsQueue.append("ac");
           if (sim_typ==".TR") simulationsQueue.append("tran");
           if (sim_typ==".HB") simulationsQueue.append("hb");
           if ((sim_typ==".SW")&&
               (pc->Props.at(0)->Value.startsWith("DC"))) simulationsQueue.append("dc");
       }
    }
}

void Xyce::createNetlist(QTextStream &stream, int , QStringList &simulations,
                    QStringList &vars, QStringList &outputs)
{
    QString s;
    bool hasParSweep = false;

    if(!prepareSpiceNetlist(stream,true)) return; // Unable to perform spice simulation
    startNetlist(stream,true);

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
            QString var_pr = pc->getProbeVariable(true);
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

    //execute simulations

    QFileInfo inf(Sch->DocName);
    QString basenam = inf.baseName();

    QString nod,nods;
    nods.clear();
    foreach (nod,vars) {
        if (!nod.startsWith("I(")) {
            nods += QString("v(%1) ").arg(nod);
        } else {
            nods += nod + " ";
        }
    }
    QString sim = simulations.first();

    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) { // Xyce can run
       if(pc->isSimulation) {                        // only one simulations per time.
           QString sim_typ = pc->Model;              // Multiple simulations are forbidden.
           QString s = pc->getSpiceNetlist(true);
           if ((sim_typ==".AC")&&(sim=="ac")) stream<<s;
           if ((sim_typ==".TR")&&(sim=="tran")) stream<<s;
           if ((sim_typ==".HB")&&(sim=="hb")) stream<<s;
           if (sim_typ==".SW") {
               QString SwpSim = pc->Props.at(0)->Value;
               if (SwpSim.startsWith("DC")&&(sim=="dc")) stream<<s;
               else if (SwpSim.startsWith("AC")&&(sim=="ac")) {
                   stream<<s;
                   hasParSweep = true;
               } else if (SwpSim.startsWith("TR")&&(sim=="tran")) {
                   stream<<s;
                   hasParSweep = true;
               } if (SwpSim.startsWith("HB")&&(sim=="hb")) {
                   stream<<s;
                   hasParSweep = true;
               }
           }
           if ((sim_typ==".DC")) stream<<s;
       }
    }


    QString filename;
    if (hasParSweep) filename = QString("%1_%2_swp.txt").arg(basenam).arg(sim);
    else filename = QString("%1_%2.txt").arg(basenam).arg(sim);
    QString write_str;
    if (sim=="hb") {
        write_str = QString(".PRINT  %1 file=%2 %3\n").arg(sim).arg(filename).arg(nods);
    } else {
        write_str = QString(".PRINT  %1 format=raw file=%2 %3\n").arg(sim).arg(filename).arg(nods);
    }
    stream<<write_str;
    outputs.append(filename);

    stream<<".END\n";
}

void Xyce::slotSimulate()
{

    int num=0;
    netlistQueue.clear();
    output_files.clear();

    determineUsedSimulations();

    foreach(QString sim,simulationsQueue) {
        QStringList sim_lst;
        sim_lst.clear();
        sim_lst.append(sim);
        QString tmp_path = QDir::convertSeparators(workdir+"/spice4qucs."+sim+".cir");
        netlistQueue.append(tmp_path);
        QFile spice_file(tmp_path);
        if (spice_file.open(QFile::WriteOnly)) {
            QTextStream stream(&spice_file);
            createNetlist(stream,num,sim_lst,vars,output_files);
            spice_file.close();
        }
    }

    output.clear();
    nextSimulation();
    emit started();

}

void Xyce::SaveNetlist(QString filename)
{
    determineUsedSimulations();
    int num = 0;
    QFile spice_file(filename);
    if (spice_file.open(QFile::WriteOnly)) {
        QTextStream stream(&spice_file);
        createNetlist(stream,num,simulationsQueue,vars,output_files);
        spice_file.close();
    }
}

void Xyce::slotFinished()
{
    output += SimProcess->readAllStandardOutput();
    if (netlistQueue.isEmpty()) {
        emit finished();
        return;
    } else {
        nextSimulation();
    }
}

void Xyce::nextSimulation()
{
    if (!netlistQueue.isEmpty()) {
        QString file = netlistQueue.takeFirst();
        SimProcess->setWorkingDirectory(workdir);
        QString cmd = QString("%1 %2 %3").arg(simulator_cmd,simulator_parameters,file);
        SimProcess->start(cmd);
    } else {
        emit finished(); // nothing to simulate
    }
}
