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
#include "components/subcircuit.h"
#include "spicecomponents/sp_spiceinit.h"
#include "spicecomponents/xsp_cmlib.h"
#include "main.h"
#include "misc.h"
#include "qucs.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*!
  \file ngspice.cpp
  \brief Implementation of the Ngspice class
*/

/*!
 * \brief Ngspice::Ngspice Class constructor
 * \param sch_ Schematic that need to be simulated with Ngspice.
 * \param parent Parent object
 */
Ngspice::Ngspice(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, parent)
{
    if (QFileInfo(QucsSettings.NgspiceExecutable).isRelative()) { // this check is related to MacOS
        simulator_cmd = QFileInfo(QucsSettings.BinDir + QucsSettings.NgspiceExecutable).absoluteFilePath();
    } else {
        simulator_cmd = QFileInfo(QucsSettings.NgspiceExecutable).absoluteFilePath();
    }
    if (!QFileInfo::exists(simulator_cmd)) {
        simulator_cmd = QucsSettings.NgspiceExecutable; //rely on $PATH
    }
    simulator_parameters = "";
    spinit_name = QDir::toNativeSeparators(QucsSettings.S4Qworkdir+"/.spiceinit");
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
    Q_UNUSED(simulations);

    if(!prepareSpiceNetlist(stream)) return; // Unable to perform spice simulation

    // include math. functions for inter-simulator compat.
    QString mathf_inc;
    bool found = findMathFuncInc(mathf_inc);
    // Let to simulate schematic without mathfunc.inc file
    if (found && QucsSettings.DefaultSimulator != spicecompat::simSpiceOpus)
        stream<<QString(".INCLUDE \"%1\"\n").arg(mathf_inc);

    startNetlist(stream); // output .PARAM and components

    if (DC_OP_only) {
        stream<<".control\n"  // Execute only DC OP analysis
              <<"set filetype=ascii\n" // Ignore all other simulations
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

    stream << "\n.control\n\n";          //execute simulations

    if (QucsMain != nullptr) { // if not run from CLI
        if (!QucsMain->ProjName.isEmpty()) {
            // always load osdi from the project directory
            QStringList osdi_ext;
            osdi_ext<<"*.osdi";
            QStringList osdi_files = QucsSettings.QucsWorkDir.entryList(osdi_ext,QDir::Files);
            for(const auto &file : osdi_files) {
                QString abs_file = QucsSettings.QucsWorkDir.absolutePath() +
                        QDir::separator() + file;
                stream<<QString("pre_osdi '%1'\n").arg(abs_file);
            }
        }
    }

    // determine which simulations are in use
    unsigned int dcSims = 0;
    unsigned int freqSims = 0;
    unsigned int timeSims = 0;
    unsigned int fourSims = 0;
    unsigned int pzSims = 0;

    outputs.clear();
    for ( unsigned int i = 0 ; i < Sch->DocComps.count() ; i++ ) {
        Component *pc = Sch->DocComps.at(i);
        if ( !pc->isSimulation ) continue;
        if ( pc->isActive != COMP_IS_ACTIVE ) continue;

        QString sim_typ = pc->Model;
        QString sim_name = pc->Name.toLower();
        QString spiceNetlist;

        bool hasParSWP = false;
        bool hasDblSWP = false;
        QString cnt_var;

        // Duplicate .PARAM in .control section. They may be used in euqations
        for ( unsigned int i = 0 ; i < Sch->DocComps.count() ; i++ ) {
            Component *pc1 = Sch->DocComps.at(i);
            if ( pc1->isActive != COMP_IS_ACTIVE ) continue;
            if ( pc1->Model == "Eqn" ) {
                spiceNetlist.append((reinterpret_cast<Equation *>(pc1))->getNgspiceScript());
            }
        }

        QString nods;
        for (const QString& nod : vars) {
            if ( nod.endsWith("#branch") )
                nods.append(QString("i(%1) ").arg(nod.section('#', 0, 0)));
            else
                nods.append(QString("v(%1) ").arg(nod));
        }

        for ( unsigned int i = 0 ; i < Sch->DocComps.count() ; i++ ) {
            Component *pc1 = Sch->DocComps.at(i);
            if ( !pc1->isSimulation ) continue;
            if ( pc1->isActive != COMP_IS_ACTIVE ) continue;
            QString sim_typ = pc1->Model;
            if ( sim_typ == ".SW" ) {
                QString SwpSim = pc1->Props.at(0)->Value.toLower();
                if ( SwpSim == sim_name ) {
                    cnt_var = (reinterpret_cast<Param_Sweep *>(pc1))->getCounterVar();
                    if ( !sim_name.startsWith("dc") ) {
                        spiceNetlist.append(getParentSWPscript(pc1, sim_name, true, hasDblSWP));
                        spiceNetlist.append(pc1->getNgspiceBeforeSim(sim_name));
                        hasParSWP = true;
                    }
                }
            }
        }

        if ( sim_typ == ".AC" ) {
            freqSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
        } else if ( sim_typ == ".TR" ) {
            timeSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            for ( unsigned int i = 0 ; i < Sch->DocComps.count() ; i++ ) {
                Component *pc1 = Sch->DocComps.at(i);
                if ( !pc1->isSimulation ) continue;
                if ( pc1->isActive != COMP_IS_ACTIVE ) continue;
                if ( pc1->Model == ".FOURIER" ) {
                    if ( pc1->Props.at(0)->Value.toLower() == sim_name ) {
                        fourSims++;
                        // Add it twice for THD
                        outputs.append("spice4qucs." + pc1->Name.toLower() + ".four");
                        outputs.append("spice4qucs." + pc1->Name.toLower() + ".four");
                        spiceNetlist.append(pc1->getSpiceNetlist());
                    }
                }
            }
        } else if ( sim_typ == ".CUSTOMSIM" ) {
            spiceNetlist.append(pc->getSpiceNetlist());
            nods = pc->Props.at(1)->Value;
            nods.replace(';', ' ');
            outputs.append(pc->Props.at(2)->Value.split(';', qucs::SkipEmptyParts));

            QRegularExpression ac_rx("^\\s*ac\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression sp_rx("^\\s*sp\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression noise_rx("^\\s*noise\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression disto_rx("^\\s*disto\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression fft_rx("^\\s*fft\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression four_rx("^\\s*(four|fourier)\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression dc_rx("^\\s*dc\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression op_rx("^\\s*op\\s*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression tran_rx("^\\s*tran\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression sens_ac_rx("^\\s*sens\\s.*ac\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression sens_dc_rx("^\\s*sens\\s.*", QRegularExpression::CaseInsensitiveOption);
            QRegularExpression pz_rx("^\\s*pz\\s.*", QRegularExpression::CaseInsensitiveOption);

            QStringList lines = pc->getSpiceNetlist().split('\n');
            for ( const QString& line : lines ) {
                if      ( ac_rx.match(line).hasMatch() )      freqSims++ ;
                else if ( sp_rx.match(line).hasMatch() )      freqSims++ ;
                else if ( noise_rx.match(line).hasMatch() )   freqSims++ ;
                else if ( disto_rx.match(line).hasMatch() )   freqSims++ ;
                else if ( fft_rx.match(line).hasMatch() )     freqSims++ ;
                else if ( four_rx.match(line).hasMatch() )    fourSims++ ;
                else if ( dc_rx.match(line).hasMatch() )      dcSims++ ;
                else if ( op_rx.match(line).hasMatch() )      dcSims++ ;
                else if ( tran_rx.match(line).hasMatch() )    timeSims++ ;
                else if ( sens_ac_rx.match(line).hasMatch() ) freqSims++ ;
                else if ( sens_dc_rx.match(line).hasMatch() ) dcSims++ ;
                else if ( pz_rx.match(line).hasMatch() )      pzSims++ ;
            }
        } else if ( sim_typ == ".DISTO" ) {
            freqSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            nods.clear();
        } else if ( sim_typ == ".NOISE" ) {
            freqSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            outputs.append("spice4qucs." + sim_name + ".cir.noise");
            if ( hasParSWP ) {  // Set necessary plot number to output Noise spectrum
                // each step of parameter sweep creates new couple of noise plots
                spiceNetlist.append(QString("let noise_%1 = 2*%1+1\n").arg(cnt_var));
                spiceNetlist.append(QString("setplot noise$&noise_%1\n").arg(cnt_var));
            } else {  // Set Noise1 plot to output noise spectrum
                spiceNetlist.append("setplot noise1\n");
            }
            nods = "inoise_spectrum onoise_spectrum";
        } else if ( sim_typ == ".PZ" ) {
            pzSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            QString out = "spice4qucs." + sim_name + ".cir.pz";
            // Add it twice for poles and zeros
            outputs.append(out);
            outputs.append(out);
        } else if ( sim_typ == ".SENS" ) {
            dcSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            outputs.append("spice4qucs." + sim_name + ".ngspice.sens.dc.prn");
        } else if ( sim_typ == ".SENS_AC" ) {
            freqSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            outputs.append("spice4qucs." + sim_name + ".sens.prn");
        } else if ( sim_typ == ".SP" ) {
            freqSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            nods.clear();
            nods.append(' ' + pc->getExtraVariables().join(' '));
        } else if ( sim_typ == ".FFT" ) {
            freqSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
            spiceNetlist.append(QString("linearize %1\n").arg(nods));
            spiceNetlist.append(QString("fft %1\n").arg(nods));
        } else if ( sim_typ == ".DC" ) {
            dcSims++;
            spiceNetlist.append(pc->getSpiceNetlist());
        } else if ( sim_typ == ".SW" ) {
            QString SwpSim = pc->Props.at(0)->Value.toLower();
            if ( SwpSim.startsWith("dc") ) {
                dcSims++;
                spiceNetlist.append(pc->getSpiceNetlist());
            } else
                continue;
        } else
            continue;

        if ( (sim_typ != ".PZ") && (sim_typ != ".SENS") && (sim_typ != ".SENS_AC") ) {
            QStringList dep_vars;
            for ( unsigned int i = 0 ; i < Sch->DocComps.count() ; i++ ) {
                Component *pc1 = Sch->DocComps.at(i);
                if ( pc1->isActive != COMP_IS_ACTIVE ) continue;
                if ( pc1->Model == "Eqn" || pc1->Model == "NutmegEq" )
                    spiceNetlist.append(pc1->getEquations(sim_name, dep_vars));
            }
            nods.append(' ' + dep_vars.join(' '));
        }

        if ( sim_typ == ".DC" ) {
            QString out = "spice4qucs." + sim_name + ".ngspice.dc.print";
            spiceNetlist.append(QString("print %1 > %2\n").arg(nods).arg(out));
            outputs.append(out);
        } else if ( (sim_typ != ".PZ") && (sim_typ != ".SENS") && (sim_typ != ".SENS_AC") ) {
            nods = nods.simplified();
            if ( !nods.isEmpty() ) {
                QString basenam = "spice4qucs";
                QString filename;
                if ( hasParSWP && hasDblSWP )
                    filename = QString("%1.%2._swp_swp.plot").arg(basenam).arg(sim_name);
                else if ( hasParSWP )
                    filename = QString("%1.%2._swp.plot").arg(basenam).arg(sim_name);
                else
                    filename = QString("%1.%2.plot").arg(basenam).arg(sim_name);
                filename.replace(' ', '_'); // Ngspice cannot understand spaces in filename
                spiceNetlist.append(QString("write %1 %2\n").arg(filename).arg(nods));
                outputs.append(filename);
            }
        }

        for ( unsigned int i = 0 ; i < Sch->DocComps.count() ; i++ ) {
            Component *pc1 = Sch->DocComps.at(i);
            if ( !pc1->isSimulation ) continue;
            if ( pc1->isActive != COMP_IS_ACTIVE ) continue;
            QString sim_typ = pc1->Model;
            if ( sim_typ == ".SW" ) {
                QString SwpSim = pc1->Props.at(0)->Value.toLower();
                if ( SwpSim == sim_name ) {
                    if ( !sim_name.startsWith("dc") ) {
                        spiceNetlist.append(pc1->getNgspiceAfterSim(sim_name));
                        spiceNetlist.append(getParentSWPscript(pc1, sim_name, false, hasDblSWP));
                    }
                }
            }
        }

        spiceNetlist.append("destroy all\n");
        spiceNetlist.append("reset\n\n");
        stream << spiceNetlist;
    }

    stream << "exit\n"
           << ".endc\n";
    stream << ".END\n";

    needsPrefix = ( (dcSims | freqSims | timeSims | fourSims | pzSims) > 1 );

    qDebug() << '\n'
             << "Simulations:\n"
             << "DC:        " << dcSims << '\n'
             << "Frequency: " << freqSims << '\n'
             << "Time:      " << timeSims << '\n'
             << "Fourier:   " << fourSims << '\n'
             << "Pole-Zero: " << pzSims << '\n'
             << '\n';
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
    QString swp = pc_swp->Name.toLower();
    for ( unsigned int i = 0 ; i < Sch->DocComps.count() ; i++ ) {
        Component *pc = Sch->DocComps.at(i);
        if ( !pc->isSimulation ) continue;
        if ( pc->isActive != COMP_IS_ACTIVE ) continue;
        if ( pc->Model == ".SW" ) {
            if ( pc->Props.at(0)->Value.toLower() == swp ) {
                if (before) {
                    hasDblSwp = true;
                    return pc->getNgspiceBeforeSim(sim, 1);
                } else {
                    hasDblSwp = true;
                    return pc->getNgspiceAfterSim(sim, 1);
                }
            }
        }
    }
    return QString("");
}

/*!
 * \brief Ngspice::slotSimulate Create netlist and execute Ngspice simulator. Netlist
 *        is saved at $HOME/.qucs/spice4qucs/spice4qucs.cir
 */
void Ngspice::slotSimulate()
{
    output.clear();

    QString mathf_inc; // drain
    if (!findMathFuncInc(mathf_inc)) {
        output.append("[Warning!] " + mathf_inc + " file not found!\n");
    }

    bool checker_error = false;
    QStringList incompat;
    if (!checkSchematic(incompat)) {
        QString s = incompat.join("; ");
        output.append("There were SPICE-incompatible components. Simulator cannot proceed.");
        output.append("Incompatible components are: " + s + "\n");
        checker_error = true;
    }

    if (!checkGround()) {
        output.append("No Ground found. Please add at least one ground!\n"
                      "Press Insert->Ground in the main menu and connect ground to one "
                      "of the schematic nodes.\n");
        checker_error = true;
    }

    if (!checkSimulations()) {
        output.append("No simulation found. Please add at least one simulation!\n"
                      "Navigate to the \"simulations\" group in the components panel (left)"
                      " and drag simulation to the schematic sheet. Then define its parameters.\n");
        checker_error = true;
    }

    if (!checkDCSimulation()) {
        output.append("Only DC simulation found in the schematic. It has no effect!"
                      " Add TRAN, AC, or Sweep simulation to proceed.\n");
        checker_error = true;
    }

    if (!checkNodeNames(incompat)) {
        QString s = incompat.join("; ");
        output.append("There were Nutmeg-incompatible node names. Simulator cannot proceed.\n");
        output.append("Incompatible node names are: " + s + "\n");
        checker_error = true;
    }

    if (checker_error) {
        if (console != nullptr)
            console->insertPlainText(output);
        //emit finished();
        emit errors(QProcess::FailedToStart);
        return;
    }

    QString netfile = "spice4qucs.cir";
    QString tmp_path = QDir::toNativeSeparators(workdir+QDir::separator()+netfile);
    SaveNetlist(tmp_path);

    removeAllSimulatorOutputs();

    /*XSPICE_CMbuilder *CMbuilder = new XSPICE_CMbuilder(Sch);
    CMbuilder->cleanSpiceinit();
    CMbuilder->createSpiceinit(collectSpiceinit(Sch));
    if (CMbuilder->needCompile()) {
        CMbuilder->cleanCModelTree();
        CMbuilder->createCModelTree(output);
        CMbuilder->compileCMlib(output);
    }
    delete CMbuilder;*/
    cleanSpiceinit();
    createSpiceinit(/*initial_spiceinit=*/collectSpiceinit(Sch));

    //startNgSpice(tmp_path);
    SimProcess->setWorkingDirectory(workdir);
    qDebug()<<workdir;
    QString cmd = QString("\"%1\" %2 %3").arg(simulator_cmd,simulator_parameters,netfile);
    QStringList cmd_args = misc::parseCmdArgs(cmd);
    QString ngsp_cmd = cmd_args.at(0);
    cmd_args.removeAt(0);
    SimProcess->start(ngsp_cmd,cmd_args);
    if (QucsMain)
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
 * \brief Ngspice::collectSpiceinit Collects user-specified .spiceinit data.
 * \param incompat
 * \return
 */
QString Ngspice::collectSpiceinit(Schematic* sch)
{
    QStringList collected_spiceinit;
    for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
        if (pc->Model == "SPICEINIT") {
            collected_spiceinit += ((SpiceSpiceinit*)pc)->getSpiceinit();
        } else if (pc->Model == "Sub") {
            Schematic *sub = new Schematic(0, ((Subcircuit *)pc)->getSubcircuitFile());
            if(!sub->loadDocument())      // load document if possible
            {
                delete sub;
                continue;
            }
            collected_spiceinit += collectSpiceinit(sub);
            delete sub;
	}
    }
    return collected_spiceinit.join("");
}

/*!
 * \brief Ngspice::findMathFuncInc Find the ngspice_mathfunc.inc file. This file
 *        contains math.functions definitions for Ngspice. It's need to let to simulate
 *        circuit if it is not found.
 * \param mathf_inc[out] The filename of include file
 * \return True if found. False otherwise
 */
bool Ngspice::findMathFuncInc(QString &mathf_inc)
{
    QDir qucs_root(QucsSettings.BinDir);
    qucs_root.cdUp();
    mathf_inc = QString("%1/share/" QUCS_NAME "/xspice_cmlib/include/ngspice_mathfunc.inc")
            .arg(qucs_root.absolutePath());
    return QFile::exists(mathf_inc);
}

/*!
 * \brief Ngspice::slotProcessOutput Process Ngspice output and report completion
 *        percentage.
 */
void Ngspice::slotProcessOutput()
{
    QString s = SimProcess->readAllStandardOutput();
    QRegularExpression percentage_pattern("^%\\d\\d*\\.\\d\\d.*$");
    if (percentage_pattern.match(s).hasMatch()) {
        int percent = round(s.mid(1,5).toFloat());
        emit progress(percent);
    }
    output += s;
    if (console != nullptr) {
        console->insertPlainText(s);
        console->moveCursor(QTextCursor::End);
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
    else
    {
        QString msg=QString("Tried to save netlist \nin %1\n(could not open for writing!)").arg(filename);
        QString final_msg=QString("%1\n This could be an error in the QSettings settings file\n(usually in ~/.config/qucs/qucs_s.conf)\nThe value for S4Q_workdir (default:/spice4qucs) needs to be writeable!\nFor a Simulation Simulation will raise error! (most likely S4Q_workdir does not exists)").arg(msg);
        QMessageBox::critical(nullptr,tr("Problem with SaveNetlist"),final_msg,QMessageBox::Ok);
    }
}

void Ngspice::setSimulatorCmd(QString cmd)
{
    if (cmd.contains(QRegularExpression("spiceopus(....|)$"))) {
        // spiceopus needs English locale to produce correct decimal point (dot symbol)
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.remove("LANG");
        env.insert("LANG","en_US");
        SimProcess->setProcessEnvironment(env);
        simulator_parameters = simulator_parameters + "-c";
    } else { // restore system environment
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        SimProcess->setProcessEnvironment(env);
    }

    simulator_cmd = cmd;
}

void Ngspice::setSimulatorParameters(QString parameters)
{
    simulator_parameters = parameters;
}

void Ngspice::cleanSpiceinit()
{
    QFileInfo inf(spinit_name);
    if (inf.exists()) QFile::remove(spinit_name);
}

void Ngspice::createSpiceinit(const QString &initial_spiceinit)
{
    if (initial_spiceinit.isEmpty()) return;
    QFile spinit(spinit_name);
    if (spinit.open(QIODevice::WriteOnly)) {
        QTextStream stream(&spinit);
        if (!initial_spiceinit.isEmpty()) {
          stream << initial_spiceinit << '\n';
        }
        spinit.close();
    }
}
