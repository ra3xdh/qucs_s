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
#include "misc.h"


/*!
  \file xyce.cpp
  \brief Implementation of the Xyce class
*/

/*!
 * \brief Xyce::Xyce Class constructor
 * \param sch_ Schematic that need to be simulated with Ngspice.
 * \param parent Parent object
 */
Xyce::Xyce(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, parent)
{
    simulator_cmd = QucsSettings.XyceExecutable;
    Nprocs = QucsSettings.NProcs;
    Noisesim = false;
}

/*!
 * \brief Xyce::determineUsedSimulations Determine simulation used
 *        in schematic and add them into simulationsQueue list
 */
void Xyce::determineUsedSimulations(QStringList *sim_lst)
{

    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
       if(pc->isSimulation) {
           QString sim_typ = pc->Model;
           if (sim_typ==".AC") simulationsQueue.append("ac");
           if (sim_typ==".NOISE") simulationsQueue.append("noise");
           if (sim_typ==".TR") simulationsQueue.append("tran");
           if (sim_typ==".HB") simulationsQueue.append("hb");
           if (sim_typ==".SP") simulationsQueue.append("sp");
           if (sim_typ==".SENS_XYCE") simulationsQueue.append("sens");
           if (sim_typ==".SENS_TR_XYCE") simulationsQueue.append("sens_tr");
           if (sim_typ==".XYCESCR") simulationsQueue.append(pc->Name); // May be >= XYCE scripts
           if ((sim_typ==".SW")&&
               (pc->Props.at(0)->Value.startsWith("DC"))) simulationsQueue.append("dc");
       }
    }

    if (sim_lst != NULL) {
        *sim_lst = simulationsQueue;
    }
}

/*!
 * \brief Xyce::createNetlist
 * \param[out] stream QTextStream that associated with spice netlist file
 * \param[in] simulations The list of simulations that need to included in netlist.
 * \param[out] vars The list of output variables and node names.
 * \param[out] outputs The list of spice output raw text files.
 */
void Xyce::createNetlist(QTextStream &stream, int , QStringList &simulations,
                    QStringList &vars, QStringList &outputs)
{
    QString s;
    bool hasParSweep = false;

    if(!prepareSpiceNetlist(stream)) return; // Unable to perform spice simulation
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

    if (DC_OP_only) {
        // Add all remaining nodes, because XYCE has no equivalent for PRINT ALL
        for(Node* pn = Sch->Nodes->first(); pn != 0; pn = Sch->Nodes->next()) {
            if ((!vars.contains(pn->Name))&&(pn->Name!="gnd")) {
                vars.append(pn->Name);
            }
        }
        // Add DC sources
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
             if ((pc->Model == "S4Q_V")||(pc->Model == "Vdc")) {
                 vars.append("I("+pc->Name+")");
             }
        }
    }

    vars.sort();

    //execute simulations

    QFileInfo inf(Sch->DocName);
    QString basenam = inf.baseName();

    QString nods;
    nods.clear();
    for (auto& nod :vars) {
        if (!nod.startsWith("I(")) {
            nods += QString("v(%1) ").arg(nod);
        } else {
            nods += nod + " ";
        }
    }

    if (DC_OP_only) {
        stream<<".OP\n";
        stream<<QString(".PRINT dc format=noindex file=spice4qucs.cir.dc_op_xyce %1\n").arg(nods);
        outputs.append("spice4qucs.cir.dc_op_xyce");
        return;
    }

    QString sim = simulations.first();
    QStringList spar_vars;
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) { // Xyce can run
       if(pc->isSimulation) {                        // only one simulations per time.
           QString sim_typ = pc->Model;              // Multiple simulations are forbidden.
           QString s = pc->getSpiceNetlist(true);
           if ((sim_typ==".AC")&&(sim=="ac")) stream<<s;
           if ((sim_typ==".NOISE")&&(sim=="noise")) stream<<s;
           if ((sim_typ==".SENS_XYCE")&&(sim=="sens")) stream<<s;
           if ((sim_typ==".SENS_TR_XYCE")&&(sim=="sens_tr")) stream<<s;
           if ((sim_typ==".SP")&&(sim=="sp")) {
               spar_vars = pc->getExtraVariables();
               stream<<s;
           }
           if (sim==pc->Name) stream<<s; // Xyce scripts
           if ((sim_typ==".TR")&&(sim=="tran")){
               stream<<s;
               Q3PtrList<Component> comps(Sch->DocComps); // find Fourier tran
               for(Component *pc1 = comps.first(); pc1 != 0; pc1 = comps.next()) {
                   if (pc1->Model==".FOURIER") {
                       if (pc1->Props.at(0)->Value==pc->Name) {
                           QString s1 = pc1->getSpiceNetlist(true);
                           outputs.append("spice4qucs.tran.cir.four0");
                           stream<<s1;
                       }
                   }
               }
           }
           if ((sim_typ==".HB")&&(sim=="hb")) stream<<s;
           if (sim_typ==".SW") {
               QString SwpSim = pc->Props.at(0)->Value;
               if (SwpSim.startsWith("DC")&&(sim=="dc")) stream<<s;
               else if (SwpSim.startsWith("AC")&&(sim=="ac")) {
                   stream<<s;
                   hasParSweep = true;
               } else if (SwpSim.startsWith("SP")&&(sim=="sp")) {
                   stream<<s;
                   hasParSweep = true;
               } else if (SwpSim.startsWith("TR")&&(sim=="tran")) {
                   stream<<s;
                   hasParSweep = true;
               } else if (SwpSim.startsWith("HB")&&(sim=="hb")) {
                   stream<<s;
                   hasParSweep = true;
               } else if (SwpSim.startsWith("SENS")&&(sim=="sens")) {
                   stream<<s;
                   hasParSweep = true;
               } else if (SwpSim.startsWith("TSENS")&&(sim=="sens_tr")) {
                   stream<<s;
                   hasParSweep = true;
               } else if (SwpSim.startsWith("SW")&&(sim=="dc")) {
                   for(Component *pc1 = Sch->DocComps.first(); pc1 != 0; pc1 = Sch->DocComps.next()) {
                       if ((pc1->Name==SwpSim)&&(pc1->Props.at(0)->Value.startsWith("DC"))) {
                           stream<<s;
                           hasParSweep = true;
                       }
                   }
               }
           }
           if ((sim_typ==".DC")) stream<<s;
       }
    }

    if (sim.startsWith("XYCESCR")) {
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            if (pc->isSimulation)
                if (sim == pc->Name)
                    outputs.append(pc->Props.at(2)->Value.split(';'));
        }
        stream<<".END\n";
        return;
    }

    QString filename;
    if (hasParSweep) filename = QString("%1_%2_swp.txt").arg(basenam).arg(sim);
    else filename = QString("%1_%2.txt").arg(basenam).arg(sim);
    filename.remove(QRegularExpression("\\s")); // XYCE don't support spaces and quotes
    QString write_str;
    if (sim=="hb") {
        // write_str = QString(".PRINT  %1 file=%2 %3\n").arg(sim).arg(filename).arg(nods);
        write_str = QString(".PRINT  %1 %2\n").arg(sim).arg(nods);
        outputs.append("spice4qucs.hb.cir.HB.FD.prn");
    } else if (sim=="noise") {
        filename += "_std";
        write_str = QString(".PRINT noise file=%1 inoise onoise\n").arg(filename);
        outputs.append(filename);
    } else if (sim=="sens") {
        write_str.clear();
        outputs.append("spice4qucs.sens.cir.SENS.prn");
    } else if (sim=="sens_tr") {
        write_str.clear();
        outputs.append("spice4qucs.sens_tr.cir.SENS.prn");
        outputs.append("spice4qucs.sens_tr.cir.TRADJ.prn");
    } else if (sim=="sp") {
        write_str = ".PRINT ac format=std file=spice4qucs_sparam.prn ";
        if (hasParSweep) {
            for (const auto &v: spar_vars) { // Bug in Xyce; cannot print Z-par if
                 // .STEP is activated; otherwise simulation error
                if ( !v.startsWith("z(")) write_str += QString("%1 ").arg(v);
            }
        } else {
            write_str += spar_vars.join(" ");
        }
        write_str += "\n";
        outputs.append("spice4qucs_sparam.prn");
    } else {
        write_str = QString(".PRINT  %1 format=raw file=%2 %3\n").arg(sim).arg(filename).arg(nods);
        outputs.append(filename);
    }
    stream<<write_str;

    stream<<".END\n";
}

/*!
 * \brief Xyce::slotSimulate Execute Xyce simulator and perform all
 *        simulations from the simulationQueue list
 */
void Xyce::slotSimulate()
{

    QStringList incompat;
    bool checker_error = false;
    if (!checkSchematic(incompat)) {
        QString s = incompat.join("; ");
        output.append("There were SPICE-incompatible components. Simulator cannot proceed.");
        output.append("Incompatible components are: " + s + "\n");
        checker_error = true;
    }

    if (!checkGround()) {
        output.append("No Ground found. Please add at least one ground!\n");
        checker_error = true;
    }

    if (!checkDCSimulation()) {
        output.append("Only DC simulation found in the schematic. It has no effect!"
                      " Add TRAN, AC, or Sweep simulation to proceed.\n");
        checker_error = true;
    }

    if (checker_error) {
        if (console != nullptr)
            console->insertPlainText(output);
        //emit finished();
        emit errors(QProcess::FailedToStart);
        return;
    }

    int num=0;
    netlistQueue.clear();
    output_files.clear();

    if (DC_OP_only) {
        simulationsQueue.append("dc");
    } else  determineUsedSimulations();

    QFile::remove(workdir+"spice4qucs.sens_tr.cir.SENS.prn");
    QFile::remove(workdir+"spice4qucs.sens_tr.cir.TRADJ.prn");

    for (const QString& sim : simulationsQueue) {
        QStringList sim_lst;
        sim_lst.clear();
        sim_lst.append(sim);
        QString tmp_path = QDir::toNativeSeparators(workdir+"/spice4qucs."+sim+".cir");
        netlistQueue.append(tmp_path);
        QFile spice_file(tmp_path);
        if (spice_file.open(QFile::WriteOnly)) {
            QTextStream stream(&spice_file);
            createNetlist(stream,num,sim_lst,vars,output_files);
            spice_file.close();
        }
    }

    output.clear();
    emit started();
    nextSimulation();

}

/*!
 * \brief Xyce::SaveNetlist Save netlist into specified file without
 *        execution of simulator.
 * \param[in] filename The name of file in which netlist is saved
 */
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

/*!
 * \brief Xyce::slotFinished Simulator finished handler. End simulation or
 *        execute the next simulation from queue.
 */
void Xyce::slotFinished()
{ 
    output += SimProcess->readAllStandardOutput();;

    if (Noisesim) {
        QFile logfile(workdir + QDir::separator() + "spice4qucs.noise_log");
        if (logfile.open(QIODevice::WriteOnly)) {
            QTextStream ts(&logfile);
            ts<<output;
            logfile.close();
        }
        Noisesim = false;
        output_files.append("spice4qucs.noise_log");
    }

    if (netlistQueue.isEmpty()) {
        emit finished();
        emit progress(100);
        return;
    } else {
        nextSimulation();
    }
}

bool Xyce::waitEndOfSimulation()
{
    bool ok = false;
    while (!netlistQueue.isEmpty()) {
        ok = SimProcess->waitForFinished(10000);
    }
    ok = SimProcess->waitForFinished(10000);
    return ok;
}

/*!
 * \brief Xyce::slotProcessOutput Process Xyce output and report progress.
 */
void Xyce::slotProcessOutput()
{
    //***** Percent complete: 85.4987 %
    QString s = SimProcess->readAllStandardOutput();
    if (s.contains("Percent complete:")) {
        int percent = round(s.section(' ',3,3,QString::SectionSkipEmpty).toFloat());
        emit progress(percent);
    }
    output += s;
    if (console != nullptr) {
        console->insertPlainText(s);
        console->moveCursor(QTextCursor::End);
    }
}

/*!
 * \brief Xyce::nextSimulation Execute the next simulation from queue.
 */
void Xyce::nextSimulation()
{
    if (!netlistQueue.isEmpty()) {
        QString file = netlistQueue.takeFirst();
        if (file.endsWith(".noise.cir")) Noisesim = true;
        SimProcess->setWorkingDirectory(workdir);
        QString cmd = QString("%1 %2 \"%3\"").arg(simulator_cmd,simulator_parameters,file);
        QStringList cmd_args = misc::parseCmdArgs(cmd);
        QString xyce_cmd = cmd_args.at(0);
        cmd_args.removeAt(0);
        SimProcess->start(xyce_cmd,cmd_args);
    } else {
        output += "No simulations!\n"
                  "Exiting...\n";
        emit progress(100);
        emit finished(); // nothing to simulate
    }
}

void Xyce::setParallel(bool par)
{
    if (par) {
        QString xyce_par = QucsSettings.XyceParExecutable;
        xyce_par.replace("%p",QString::number(QucsSettings.NProcs));
        simulator_cmd = xyce_par;
        simulator_parameters = simulator_parameters + QString(" -a ");
    } else {
        simulator_cmd = "\"" + QucsSettings.XyceExecutable + "\"";
        simulator_parameters = simulator_parameters + " -a ";
    }
}
