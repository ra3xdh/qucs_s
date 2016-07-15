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
#include "xspice_cmbuilder.h"
#include "components/iprobe.h"
#include "components/vprobe.h"
#include "components/equation.h"
#include "components/param_sweep.h"
#include "spicecomponents/xsp_cmlib.h"
#include "main.h"

/*!
  \file ngspice.cpp
  \brief Implementation of the Ngspice class
*/

/*!
 * \brief Ngspice::Ngspice Class constructor
 * \param sch_ Schematic that need to be simualted with Ngspice.
 * \param parent Parent object
 */
Ngspice::Ngspice(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, parent)
{
    simulator_cmd = QucsSettings.NgspiceExecutable;
    simulator_parameters = "";
}

/*!
 * \brief Ngspice::createNetlist Output Ngspice-style netlist to text stream.
 *        Netlist contains sections necessary for Ngspice.
 * \param[out] stream QTextStream that associated with spice netlist file
 * \param[out] simulations The list of simulations used by schematic.
 * \param[out] vars The list of output variables and node names.
 * \param[out] outputs The list of spice output raw text files.
 */
void Ngspice::createNetlist(QTextStream &stream, int ,
                       QStringList &simulations, QStringList &vars, QStringList &outputs)
{
    QString s;
    if(!prepareSpiceNetlist(stream)) return; // Unable to perform spice simulation
    startNetlist(stream); // output .PARAM and components

    if (DC_OP_only) {
        stream<<".control\n"  // Execute only DC OP analysis
              <<"set filetype=ascii\n" // Ingnore all other simulations
              <<"op\n"
              <<"print all > spice4qucs.cir.dc_op\n"
              <<"destroy all\n"
              <<"quit\n"
              <<".endc\n"
              <<".end\n";
        outputs.clear();
        outputs.append("spice4qucs.cir.dc_op");
        return;
    }

    // determine which simulations are in use
    simulations.clear();
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
       if(pc->isSimulation) {
           s = pc->getSpiceNetlist();
           QString sim_typ = pc->Model;
           if (sim_typ==".AC") simulations.append("ac");
           if (sim_typ==".TR") simulations.append("tran");
           if (sim_typ==".CUSTOMSIM") simulations.append("custom");
           if (sim_typ==".DISTO") simulations.append("disto");
           if (sim_typ==".NOISE") simulations.append("noise");
           if (sim_typ==".PZ") simulations.append("pz");
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

    stream<<".control\n"          //execute simulations
          <<"echo \"\" > spice4qucs.cir.noise\n"
          <<"echo \"\" > spice4qucs.cir.pz\n";

    QString sim;
    outputs.clear();

    foreach(sim, simulations) {

        bool hasParSWP = false;
        bool hasDblSWP = false;
        QString cnt_var;

        // Duplicate .PARAM in .control section. They may be used in euqations
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            if (pc->Model=="Eqn") {
                Equation *eq = (Equation *)pc;
                stream<<eq->getNgspiceScript();
            }
        }

        // Hack: Such indexation method is needed to avoid entering infinite loop
        // in some cases of recursive access to Sch->DocComps list
        for(unsigned int i=0;i<Sch->DocComps.count();i++) {
            Component *pc = Sch->DocComps.at(i);
            QString sim_typ = pc->Model;
            if (sim_typ==".SW") {
                QString SwpSim = pc->Props.at(0)->Value;
                QString s = pc->getNgspiceBeforeSim(sim);
                cnt_var = ((Param_Sweep *)pc)->getCounterVar();
                if (SwpSim.startsWith("AC")&&(sim=="ac")) {
                    QString s2 = getParentSWPscript(pc,sim,true,hasDblSWP);
                    stream<<(s2+s);
                    hasParSWP = true;
                } else if (SwpSim.startsWith("DISTO")&&(sim=="disto")) {
                    QString s2 = getParentSWPscript(pc,sim,true,hasDblSWP);
                    stream<<(s2+s);
                    hasParSWP = true;
                } else if (SwpSim.startsWith("NOISE")&&(sim=="noise")) {
                    QString s2 = getParentSWPscript(pc,sim,true,hasDblSWP);
                    stream<<(s2+s);
                    hasParSWP = true;
                } else if (SwpSim.startsWith("PZ")&&(sim=="pz")) {
                    QString s2 = getParentSWPscript(pc,sim,true,hasDblSWP);
                    stream<<(s2+s);
                    hasParSWP = true;
                } if (SwpSim.startsWith("TR")&&(sim=="tran")) {
                    QString s2 = getParentSWPscript(pc,sim,true,hasDblSWP);
                    stream<<(s2+s);
                    hasParSWP = true;
                } else if (SwpSim.startsWith("SW")&&(sim=="dc")) {
                    for(Component *pc1 = Sch->DocComps.first(); pc1 != 0; pc1 = Sch->DocComps.next()) {
                        if ((pc1->Name==SwpSim)&&(pc1->Props.at(0)->Value.startsWith("DC"))) {
                            QString s2 = getParentSWPscript(pc,sim,true,hasDblSWP);
                            stream<<(s2+s);
                            hasParSWP = true;
                        }
                    }
                }
            }
        }


        QString custom_vars;
        // Hack: Such indexation method is needed to avoid entering infinite loop
        // in some cases of recursive access to Sch->DocComps list
        for(unsigned int i=0;i<Sch->DocComps.count();i++) {
           Component *pc = Sch->DocComps.at(i);
           if(pc->isSimulation) {
               QString sim_typ = pc->Model;
               QString s = pc->getSpiceNetlist();
               if ((sim_typ==".AC")&&(sim=="ac")) stream<<s;
               if ((sim_typ==".DISTO")&&(sim=="disto")) stream<<s;
               if ((sim_typ==".NOISE")&&(sim=="noise")) {
                   outputs.append("spice4qucs.cir.noise");
                   stream<<s;
               } if ((sim_typ==".PZ")&&(sim=="pz")) {
                   outputs.append("spice4qucs.cir.pz"); // Add it twice for poles and zeros
                   outputs.append("spice4qucs.cir.pz");
                   stream<<s;
               } if ((sim_typ==".TR")&&(sim=="tran")) {
                   stream<<s;
                   Q3PtrList<Component> comps(Sch->DocComps); // find Fourier tran
                   for(Component *pc1 = comps.first(); pc1 != 0; pc1 = comps.next()) {
                       if (pc1->Model==".FOURIER") {
                           if (pc1->Props.at(0)->Value==pc->Name) {
                               QString s1 = pc1->getSpiceNetlist();
                               outputs.append("spice4qucs.four");
                               stream<<s1;
                           }
                       }
                   }
               }
               if ((sim_typ==".CUSTOMSIM")&&(sim=="custom")) {
                   stream<<s;
                   custom_vars = pc->Props.at(1)->Value;
                   custom_vars.replace(";"," ");
                   QString cust_out = pc->Props.at(2)->Value;
                   outputs.append(cust_out.split(';',QString::SkipEmptyParts));
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
            QString ss = custom_vars;
            if (!ss.remove(' ').isEmpty()) { // if there was no variables
                QString filename = basenam + "_custom.txt";
                outputs.append(filename);
                QString write_str = QString("write %1 %2\n").arg(filename).arg(custom_vars);
                stream<<write_str;
                stream<<"destroy all\n";
                stream<<"reset\n";
            }
            continue;
        }


        QStringList vars_eq;
        vars_eq.clear();
        QStringList Eqns;
        Eqns.clear();
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            if ((pc->Model == "Eqn")||(pc->Model== "NutmegEq")) {
                QStringList v1;
                QString s_eq = pc->getEquations(sim,v1);
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

        if (sim=="noise") {
            if (hasParSWP) {  // Set necessary plot number to output Noise spectrum
                // each step of parameter sweep creates new couple of noise plots
                stream<<QString("let noise_%1 = 2*%1+1\n").arg(cnt_var);
                stream<<QString("setplot noise$&noise_%1\n").arg(cnt_var);
            } else {  // Set Noise1 plot to output noise spectrum
                stream<<"setplot noise1\n";
            }
            nods = " inoise_spectrum onoise_spectrum";
        }

        if (sim!="pz") {
            QString filename;
            if (hasParSWP&&hasDblSWP) filename = QString("%1_%2_swp_swp.txt").arg(basenam).arg(sim);
            else if (hasParSWP) filename = QString("%1_%2_swp.txt").arg(basenam).arg(sim);
            else filename = QString("%1_%2.txt").arg(basenam).arg(sim);
            filename.replace(' ','_'); // Ngspice cannot understand spaces in filename

            QString write_str = QString("write %1 %2\n").arg(filename).arg(nods);
            stream<<write_str;
            outputs.append(filename);
        }


        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            QString sim_typ = pc->Model;
            if (sim_typ==".SW") {
                QString s = pc->getNgspiceAfterSim(sim);
                QString SwpSim = pc->Props.at(0)->Value;
                bool b; // value drain
                if (SwpSim.startsWith("AC")&&(sim=="ac")) {
                    s += getParentSWPscript(pc,sim,false,b);
                    stream<<s;
                } else if (SwpSim.startsWith("DISTO")&&(sim=="disto")) {
                    s += getParentSWPscript(pc,sim,false,b);
                    stream<<s;
                } else if (SwpSim.startsWith("NOISE")&&(sim=="noise")) {
                    s += getParentSWPscript(pc,sim,false,b);
                    stream<<s;
                } else if (SwpSim.startsWith("PZ")&&(sim=="pz")) {
                    s += getParentSWPscript(pc,sim,false,b);
                    stream<<s;
                } else if (SwpSim.startsWith("TR")&&(sim=="tran")) {
                    s += getParentSWPscript(pc,sim,false,b);
                    stream<<s;
                } else if (SwpSim.startsWith("SW")&&(sim=="dc")) {
                    for(Component *pc1 = Sch->DocComps.first(); pc1 != 0; pc1 = Sch->DocComps.next()) {
                        if ((pc1->Name==SwpSim)&&(pc1->Props.at(0)->Value.startsWith("DC"))) {
                             s += getParentSWPscript(pc,sim,false,b);
                             stream<<s;
                        }
                    }
               }
            }
        }

        stream<<"destroy all\n";
        stream<<"reset\n\n";
    }

    stream<<"exit\n"
          <<".endc\n";

    stream<<".END\n";
}

/*!
 * \brief Ngspice::getParentSWPscript
 * \param pc_swp
 * \param sim
 * \param before
 * \return
 */
QString Ngspice::getParentSWPscript(Component *pc_swp, QString sim, bool before, bool &hasDblSwp)
{
    hasDblSwp = false;
    QString swp = pc_swp->Name;
    if (swp.startsWith("DC")) return QString("");
    Q3PtrList<Component> Comps(Sch->DocComps);
    if (Comps.count()>0) {
        for (unsigned int i=0;i<Comps.count();i++) {
            Component *pc2 = Comps.at(i);
            if (pc2->Model.startsWith(".SW")) {
                if (pc2->Props.at(0)->Value==swp) {
                    if (before) {
                        hasDblSwp = true;
                        return pc2->getNgspiceBeforeSim(sim,1);
                    } else {
                        hasDblSwp = true;
                        return pc2->getNgspiceAfterSim(sim,1);
                    }
                }
            }
        }
    }
    return QString("");
}

/*!
 * \brief Ngspice::slotSimulate Create netlist and execute Ngspice simualtor. Netlist
 *        is saved at $HOME/.qucs/spice4qucs/spice4qucs.cir
 */
void Ngspice::slotSimulate()
{
    output.clear();
    QStringList incompat;
    if (!checkSchematic(incompat)) {
        QString s = incompat.join("; ");
        output.append("There were SPICE-incompatible components. Simulator cannot proceed.");
        output.append("Incompatible components are: " + s + "\n");
        emit finished();
        emit errors(QProcess::FailedToStart);
        return;
    }

    if (!checkNodeNames(incompat)) {
        QString s = incompat.join("; ");
        output.append("There were Nutmeg-incompatible node names. Simulator cannot proceed.\n");
        output.append("Incompatible node names are: " + s + "\n");
        emit finished();
        emit errors(QProcess::FailedToStart);
        return;
    }

    QString netfile = "spice4qucs.cir";
    QString tmp_path = QDir::convertSeparators(workdir+QDir::separator()+netfile);
    SaveNetlist(tmp_path);

    removeAllSimulatorOutputs();

    XSPICE_CMbuilder *CMbuilder = new XSPICE_CMbuilder(Sch);
    CMbuilder->cleanSpiceinit();
    CMbuilder->createSpiceinit();
    if (CMbuilder->needCompile()) {
        CMbuilder->cleanCModelTree();
        CMbuilder->createCModelTree(output);
        CMbuilder->compileCMlib(output);
    }
    delete CMbuilder;

    //startNgSpice(tmp_path);
    SimProcess->setWorkingDirectory(workdir);
    qDebug()<<workdir;
    QString cmd = QString("\"%1\" %2 %3").arg(simulator_cmd,simulator_parameters,netfile);
    SimProcess->start(cmd);
    emit started();
}

/*!
 * \brief Ngspice::checkNodeNames Check schematic node names on reserved Nutmeg keywords.
 * \param incompat
 * \return
 */
bool Ngspice::checkNodeNames(QStringList &incompat)
{
    bool result = true;
    for(Node *pn = Sch->DocNodes.first(); pn != 0; pn = Sch->DocNodes.next()) {
      if(pn->Label != 0) {
          if (!spicecompat::check_nodename(pn->Label->Name)) {
              incompat.append(pn->Label->Name);
              result = false;
          }
      }
    }
    for(Wire *pw = Sch->DocWires.first(); pw != 0; pw = Sch->DocWires.next()) {
      if(pw->Label != 0) {
          if (!spicecompat::check_nodename(pw->Label->Name)) {
              incompat.append(pw->Label->Name);
              result = false;
          }
      }
    }
    return result;
}

/*!
 * \brief Ngspice::slotProcessOutput Process Ngspice output and report completetion
 *        percentage.
 */
void Ngspice::slotProcessOutput()
{
    QString s = SimProcess->readAllStandardOutput();
    QRegExp percentage_pattern("^%\\d\\d*\\.\\d\\d.*$");
    if (percentage_pattern.exactMatch(s)) {
        int percent = round(s.mid(1,5).toFloat());
        emit progress(percent);
    } else {
        s.remove(QRegExp("%\\d\\d*\\.\\d\\d")); // Remove percentage from logs
        s.remove(QRegExp("\010+")); // Large amount of datar from percentage reports
                                    // can freeze QTextEdit for over 100k simulation points
        output += s;
    }
}

/*!
 * \brief Ngspice::SaveNetlist Create netlist and save it to file without execution
 *        of simulator.
 * \param[in] filename Absolute path to netlist
 */
void Ngspice::SaveNetlist(QString filename)
{
    int num=0;
    sims.clear();
    vars.clear();

    QFile spice_file(filename);
    if (spice_file.open(QFile::WriteOnly)) {
        QTextStream stream(&spice_file);
        createNetlist(stream,num,sims,vars,output_files);
        spice_file.close();
    }
}

void Ngspice::setSimulatorCmd(QString cmd)
{
    if (cmd.contains(QRegExp("spiceopus(....|)$"))) {
        // spiceopus needs English locale to produce correct decimal point (dot symbol)
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.remove("LANG");
        env.insert("LANG","en_US");
        SimProcess->setProcessEnvironment(env);
        simulator_parameters="-c";
    } else { // restore system environment
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        SimProcess->setProcessEnvironment(env);
        simulator_parameters="";
    }

    simulator_cmd = cmd;
}

