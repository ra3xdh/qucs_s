/***************************************************************************
                           abstractspicekernel.h
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




#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "abstractspicekernel.h"
#include "misc.h"
#include "main.h"
#include "../paintings/id_text.h"
#include "dialogs/sweepdialog.h"
#include "components/subcircuit.h"


#include <QPlainTextEdit>
#include <algorithm>

/*!
  \file abstractspicekernel.cpp
  \brief Implementation of the AbstractSpiceKernel class
*/


/*!
 * \brief AbstractSpiceKernel::AbstractSpiceKernel class constructor
 * \param schematic Schematic that should be simulated with Spice-compatible
 *        simulator
 * \param parent Parent object
 */
AbstractSpiceKernel::AbstractSpiceKernel(Schematic *schematic, QObject *parent) :
    QObject(parent),
    a_workdir(),
    a_simulator_cmd(),
    a_simulator_parameters(),
    a_output(),
    a_simProcess(new QProcess(this)),
    a_console(nullptr),
    a_sims(),
    a_vars(),
    a_output_files(),
    a_DC_OP_only(schematic->getShowBias() == 0 ? true : false),
    a_needsPrefix(false),
    a_schematic(schematic),
    a_parseFourTHD(false),
    a_parsePZzeros(false)
{
    if (!checkDCSimulation()) { // Run Show bias mode automatically
        a_DC_OP_only = true;      // If schematic contains DC simulation only
        a_schematic->setShowBias(0);
    }

    a_workdir = QucsSettings.S4Qworkdir;
    QFileInfo inf(a_workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(a_workdir);
    }

    a_simProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(a_simProcess,SIGNAL(finished(int)),this,SLOT(slotFinished()));
    connect(a_simProcess,SIGNAL(readyRead()),this,SLOT(slotProcessOutput()));
    connect(a_simProcess,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(slotErrors(QProcess::ProcessError)));
    connect(this,SIGNAL(destroyed()),this,SLOT(killThemAll()));

}


AbstractSpiceKernel::~AbstractSpiceKernel()
{
    killThemAll();
}

void AbstractSpiceKernel::killThemAll()
{
    if (a_simProcess->state()!=QProcess::NotRunning) {
        a_simProcess->kill();
    }
}

/*!
 * \brief AbstractSpiceKernel::prepareSpiceNetlist Fill components nodes
 *        with approate node numbers
 * \param stream QTextStream that associated with spice netlist file
 * \param xyce Default is false. Should be set in true if netlist is
 *        prepared for Xyce simulator. For Ngspice should be false.
 * \return Returns true if success, false if netlist preparation fails
 */
bool AbstractSpiceKernel::prepareSpiceNetlist(QTextStream &stream, bool isSubckt)
{
    QStringList collect;
    QPlainTextEdit *err = new QPlainTextEdit;
    if (a_schematic->prepareNetlist(stream,collect,err)==-10) { // Broken netlist
        a_output.append(err->toPlainText());
        delete err;
        return false;
    }
    delete err;
    if (isSubckt) a_schematic->clearSignals();
    else a_schematic->clearSignalsAndFileList(); // for proper build of subckts
    return true; // TODO: Add feature to determine ability of spice simulation
}


/*!
 * \brief AbstractSpiceKernel::checkSchematic Check SPICE-compatibility of
 *        all components.
 * \param incompat[out] QStringList filled by incompatible components names
 * \return true --- All components are SPICE-compatible; false --- otherwise
 */
bool AbstractSpiceKernel::checkSchematic(QStringList &incompat)
{
    incompat.clear();
    for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
        if ((!pc->isEquation)&&!(pc->isProbe)) {
        if(pc->ComponentName == QString("Ground")) continue; // Skip GND
        if ((pc->SpiceModel.isEmpty() && pc->Netlists["Ngspice"].isEmpty()) && pc->isActive) incompat.append(pc->Name);
        }
    }

    return incompat.isEmpty();
}

/*!
 * \brief AbstractSpiceKernel::checkGround Check if schematic contain at least one ground.
 * \return True if ground found, false otherwise
 */
bool AbstractSpiceKernel::checkGround()
{
    bool r = false;
    for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
        if (pc->Name=="Ground") {
            r = true;
            break;
        }
    }
    return r;
}

bool AbstractSpiceKernel::checkSimulations()
{
    if (a_DC_OP_only) return true;
    bool r = false;
    for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
        if (pc->isSimulation) {
            r = true;
            break;
        }
    }
    return r;
}

bool AbstractSpiceKernel::checkDCSimulation()
{
    return true;  // DC OP is now saved in the dataset

    //if (a_DC_OP_only) return true;
    //bool r = false;
    //for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
    //    if (!pc->isActive) continue;
    //    if (pc->isSimulation && pc->Model != ".DC") {
    //        r = true;
    //        break;
    //    }
    //}
    //return r;
}

/*!
 * \brief AbstractSpiceKernel::startNetlist Outputs .PARAM , .GLOABAL_PARAM,
 *        and .OPTIONS sections to netlist. These sections are placed on schematic
 *        directly or converted form Equation components. Then outputs common
 *        components to netlist.
 * \param stream QTextStream that associated with spice netlist file
 * \param xyce Default is false. Should be set in true if netlist is
 *        prepared for Xyce simulator. For Ngspice should be false.
 */
void AbstractSpiceKernel::startNetlist(QTextStream &stream, spicecompat::SpiceDialect dialect)
{
        QString s;

        // User-defined functions
        for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
            if ((pc->SpiceModel==".FUNC")||
                (pc->SpiceModel=="INCLSCR")) {
                s = pc->getExpression();
                stream<<s;
            }
        }

        // create .IC from wire labels
        QStringList wire_labels;
        for(Wire *pw = a_schematic->a_DocWires.first(); pw != 0; pw = a_schematic->a_DocWires.next()) {
            if (pw->Label != nullptr) {
                QString label = pw->Label->Name;
                if (!wire_labels.contains(label)) wire_labels.append(label);
                else continue;
                QString ic = pw->Label->initValue;
                if (!ic.isEmpty()) {
                    QString ic_str = QStringLiteral(".IC v(%1)=%2\n").arg(label).arg(ic);
                    stream<<ic_str;
                }
            }
        }
        for(Node *pn = a_schematic->a_DocNodes.first(); pn != 0; pn = a_schematic->a_DocNodes.next()) {
            Conductor *pw = (Conductor*) pn;
            if (pw->Label != nullptr) {
                QString label = pw->Label->Name;
                if (!wire_labels.contains(label)) wire_labels.append(label);
                else continue;
                QString ic = pw->Label->initValue;
                if (!ic.isEmpty()) {
                    QString ic_str = QStringLiteral(".IC v(%1)=%2\n").arg(label).arg(ic);
                    stream<<ic_str;
                }
            }
        }

        // Parameters, Initial conditions, Options
        for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
            if (pc->isEquation) {
                s = pc->getExpression(dialect);
                stream<<s;
            }
        }

        // Components
        for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
          if(a_schematic->getIsAnalog() &&
             !(pc->isSimulation) &&
             !(pc->isEquation)) {
            //s = pc->getSpiceNetlist(dialect);
            s = pc->getNetlist();
            stream<<s;
          }
        }

        // Modelcards
        for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
            if (pc->SpiceModel==".MODEL") {
                s = pc->getSpiceModel();
                stream<<s;
            }
        }
}

/*!
 * \brief AbstractSpiceKernel::createNetlist Writes netlist in stream TextStream.
 *        This is overloaded method. Should be reimplemted for Ngspice and Xyce.
 */
void AbstractSpiceKernel::createNetlist(QTextStream&, int ,QStringList&,
                                        QStringList&, QStringList &)
{

}

/*!
 * \brief AbstractSpiceKernel::createSubNetlsit Output Netlist with
 *        Subcircuit header .SUBCKT
 * \param stream QTextStream that associated with spice netlist file
 * \param xyce Default is false. Should be set in true if netlist is
 *        prepared for Xyce simulator. For Ngspice should be false.
 */
void AbstractSpiceKernel::createSubNetlsit(QTextStream &stream, bool lib)
{
    QString header;
    QString f = misc::properFileName(a_schematic->getDocName());
    header = QStringLiteral(".SUBCKT %1 ").arg(misc::properName(f));

    QList< QPair<int,QString> > ports;
    if(!prepareSpiceNetlist(stream,true)) {
        emit finished();
        emit errors(QProcess::FailedToStart);
        return;
    } // Unable to perform spice simulation
    for(Component *pc = a_schematic->a_DocComps.first(); pc != 0; pc = a_schematic->a_DocComps.next()) {
        if (pc->Model=="Port") {
            ports.append(qMakePair(pc->Props.first()->Value.toInt(),
                                   pc->Ports.first()->Connection->Name));
        }
    }
    std::sort(ports.begin(), ports.end());
    if (lib) header += " gnd "; // Ground node forwarding for Library
    for (const auto& pp : ports) {
        header += pp.second + " ";
    }

    Painting *pai;
    for(pai = a_schematic->a_SymbolPaints.first(); pai != 0; pai = a_schematic->a_SymbolPaints.next())
      if(pai->Name == ".ID ") {
        ID_Text *pid = (ID_Text*)pai;
        QList<SubParameter *>::const_iterator it;
        for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
            header += (*it)->Name + " "; // keep 'Name' unchanged
          //(*tstream) << " " << s.replace("=", "=\"") << '"';
        }
        break;
      }

    header += "\n";
    if (lib) stream<<"\n";
    stream<<header;

    const spicecompat::SpiceDialect dialect(
            QucsSettings.DefaultSimulator == spicecompat::simXyce ? spicecompat::SPICEXyce : spicecompat::SPICEDefault);
    startNetlist(stream, dialect);
    stream<<".ENDS\n";
}

/*!
 * \brief AbstractSpiceKernel::slotSimulate Executes simulator
 */
void AbstractSpiceKernel::slotSimulate()
{

}



/*!
 * \brief AbstractSpiceKernel::parseNgSpiceSimOutput This method parses text raw spice
 *        output. Extracts a simulation points array and variables names and types (Real
 *        or Complex) from output.
 * \param ngspice_file Spice output file name
 * \param sim_points 2D array in which simulation points should be extracted
 * \param var_list This list is filled by simulation variables. There is a list of dependent
 *        and independent variables. An independent variable is the first in list.
 * \param isComplex Type of variables. True if complex. False if real.
 */
void AbstractSpiceKernel::parseNgSpiceSimOutput(QString ngspice_file, QList< QList<double> > &sim_points,
                                                QStringList &var_list, bool &isComplex,
                                                QStringList &digital_vars, QList<int> &dig_vars_dims)
{
    isComplex = false;
    bool isBinary = false;
    int NumPoints = 0;
    int bin_offset = 0;
    QByteArray content;

    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        //QTextStream ts(&ofile);
        content = ofile.readAll();
        ofile.close();
    }

    QTextStream ngsp_data(&content);
    sim_points.clear();
    bool start_values_sec = false;
    int NumVars=0; // Number of dep. and indep.variables
    while (!ngsp_data.atEnd()) { // Parse header;
        QRegularExpression sep("[ \t,]");
        QString lin = ngsp_data.readLine();
        if (lin.isEmpty()) continue;
        if (lin.contains("Flags")&&lin.contains("complex")) { // output consists of
            isComplex = true; // complex numbers
            continue;         // maybe ac_analysis
        }
        if (lin.contains("No. Variables")) {  // get number of variables
            NumVars=lin.section(sep,2,2,QString::SectionSkipEmpty).toInt();
            continue;
        }
        if (lin.contains("No. Points:")) {  // get number of variables
            NumPoints=lin.section(sep,2,2,QString::SectionSkipEmpty).toInt();
            continue;
        }
        if (lin=="Variables:") {
            var_list.clear();
            QString indep_var = ngsp_data.readLine().section(sep,1,1,QString::SectionSkipEmpty);
            var_list.append(indep_var);

            for (int i=1;i<NumVars;i++) {
                lin = ngsp_data.readLine();
                QString dep_var = lin.section(sep,1,1,QString::SectionSkipEmpty);
                var_list.append(dep_var);
                if (lin.contains("dims=")) {
                  digital_vars.append(dep_var); // XSPICE digital node
                  QString tail = lin.section("dims=",1,1,QString::SectionSkipEmpty);
                  dig_vars_dims.append(tail.toInt());
                }
            }
            continue;
        }
        if (lin=="Values:") {
            start_values_sec = true;
            continue;
        }
        if (lin=="Binary:") {
            isBinary = true;
            bin_offset = ngsp_data.pos();
        }

        if (isBinary) {
            QDataStream dbl(content);
            dbl.setByteOrder(QDataStream::LittleEndian);
            dbl.device()->seek(bin_offset);
            extractBinSamples(dbl, sim_points, NumPoints, NumVars, isComplex);
            break;
        }

        if (start_values_sec) {
            extractASCIISamples(lin,ngsp_data,sim_points,NumVars,isComplex);
        }
    }
}


/*!
 * \brief AbstractSpiceKernel::parseHBOutput Parse Xyce Harmonic balance (HB) simulation output.
 * \param ngspice_file Spice output file name
 * \param sim_points 2D array in which simulation points should be extracted
 * \param var_list This list is filled by simulation variables. There is a list of dependent
 *        variables. Independent hbfrequency variable is always the first in this list.
 * \param hasParSweep[out] Set to true if dataset contains parameter sweep output.
 */
void AbstractSpiceKernel::parseHBOutput(QString ngspice_file, QList<QList<double> > &sim_points,
                                        QStringList &var_list, bool &hasParSweep)
{
    var_list.clear();
    sim_points.clear();
    hasParSweep = false;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream hb_data(&ofile);
        var_list.append("hbfrequency");
        while (!hb_data.atEnd()) {
            QString lin = hb_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.contains("Parameter Sweep")) {
                hasParSweep = true;
                continue;
            }
            if (lin.startsWith("Index")) { // CSV heading
                    QStringList vars1 = lin.split(" ",Qt::SkipEmptyParts);
                    vars1.removeFirst();
                    vars1.removeFirst();
                    QStringList norm_vars;
                    for (const QString& v : vars1) { // Normalize variables
                        QString nv = v;
                        nv.remove(0,3).chop(1); // extract variable between "Re|Im(" and ")"
                        if (!norm_vars.contains(nv))
                            norm_vars.append(nv);
                    }
                    var_list.append(norm_vars);
            }
            if ((lin.contains(QRegularExpression("\\d*\\.\\d+[+-]*[eE]*[\\d]*")))) { // CSV dataline
                QStringList vals = lin.split(" ",Qt::SkipEmptyParts);
                QList <double> sim_point;
                sim_point.clear();
                for (int i=1;i<vals.count();i++) {
                    sim_point.append(vals.at(i).toDouble());
                }
                sim_points.append(sim_point);
            }
        }
        ofile.close();
    }
}

/*!
 * \brief AbstractSpiceKernel::parseFourierOutput Parse output of fourier simulation.
 * \param ngspice_file[in] Spice output file name
 * \param sim_points[out] 2D array in which simulation points should be extracted
 * \param var_list[out] This list is filled by simulation variables. There is a list of dependent
 *        and independent variables. An independent variable is the first in list.
 */
void AbstractSpiceKernel::parseFourierOutput(QString ngspice_file, QList<QList<double> > &sim_points,
                                             QStringList &var_list)
{
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {

        QTextStream ngsp_data(&ofile);
        QList<double> sim_point;
        int Nharm; // number of harmonics
        bool firstgroup = false;
        QRegularExpression sep("[ \t,]");
        QRegularExpression thd_rx("(?<=THD:).*(?=%)");
        sim_points.clear();
        var_list.clear();

        if ( a_parseFourTHD ) {
            var_list.append("");
            sim_point.append(0.0);
        } else
            var_list.append("fourierfreq");

        while (!ngsp_data.atEnd()) {
            QString lin = ngsp_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.contains("Fourier analysis for")) {
                QStringList tokens = lin.split(sep,Qt::SkipEmptyParts);
                QString var; // TODO chech
                for (const QString& var1 : tokens) {
                    if (var1.contains('(')&&var1.contains(')')) {
                        var = var1;
                        break;
                    }
                }

                if (var.endsWith(':')) var.chop(1);
                if ( a_parseFourTHD )
                    var_list.append("thd_%("+var+")");
                else {
                    var_list.append("magnitude("+var+")");
                    var_list.append("phase("+var+")");
                    var_list.append("norm(mag("+var+"))");
                    var_list.append("norm(phase("+var+"))");
                }
                continue;
            }
            if (lin.contains("No. Harmonics:")) {
                if ( a_parseFourTHD ) {
                    sim_point.append(thd_rx.match(lin).captured(0).toDouble());
                    continue;
                }
                QString ss = lin.section(sep,2,2,QString::SectionSkipEmpty);
                if (ss.endsWith(',')) ss.chop(1);
                Nharm = ss.toInt();
                while (!ngsp_data.readLine().contains(QRegularExpression("Harmonic\\s+Frequency")));
                if (QucsSettings.DefaultSimulator != spicecompat::simXyce) lin = ngsp_data.readLine(); // dummy line
                for (int i=0;i<Nharm;i++) {
                    lin = ngsp_data.readLine();
                    if (!firstgroup) {
                        sim_point.clear();
                        sim_point.append(lin.section(sep,1,1,QString::SectionSkipEmpty).toDouble()); // freq
                        sim_point.append(lin.section(sep,2,2,QString::SectionSkipEmpty).toDouble()); // magnitude
                        sim_point.append(lin.section(sep,3,3,QString::SectionSkipEmpty).toDouble()); // phase
                        sim_point.append(lin.section(sep,4,4,QString::SectionSkipEmpty).toDouble()); // normalized magnitude
                        sim_point.append(lin.section(sep,5,5,QString::SectionSkipEmpty).toDouble()); // normalized phase
                        sim_points.append(sim_point);
                    } else {
                        sim_points[i].append(lin.section(sep,2,2,QString::SectionSkipEmpty).toDouble()); // magnitude
                        sim_points[i].append(lin.section(sep,3,3,QString::SectionSkipEmpty).toDouble()); // phase
                        sim_points[i].append(lin.section(sep,4,4,QString::SectionSkipEmpty).toDouble()); // normalized magnitude
                        sim_points[i].append(lin.section(sep,5,5,QString::SectionSkipEmpty).toDouble()); // normalized phase
                    }
                }
                firstgroup = true;
            }
        }
        if ( a_parseFourTHD )
            sim_points.append(sim_point);
        a_parseFourTHD = !a_parseFourTHD;
        ofile.close();
    }
}

/*!
 * \brief AbstractSpiceKernel::parseNoiseOutput Parse output of .NOISE simulation.
 * \param[in] ngspice_file Spice output file name
 * \param[out] sim_points 2D array in which simulation points should be extracted. All simulation
 *        points from all sweep variable steps are extracted in a single array
 * \param[out] var_list This list is filled by simulation variables. There is a list of dependent
 *        and independent variables. An independent variable is the first in list.
 * \param[out] ParSwp Set to true if there was parameter sweep
 */
void AbstractSpiceKernel::parseNoiseOutput(QString ngspice_file, QList<QList<double> > &sim_points,
                                           QStringList &var_list, bool &ParSwp)
{
    var_list.clear();
    var_list.append(""); // dummy indep var
    var_list.append("inoise_total");
    var_list.append("onoise_total");

    ParSwp = false;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        sim_points.clear();
        int cnt = 0;
        while (!ngsp_data.atEnd()) {
            QString line = ngsp_data.readLine();
            if (line.contains('=')) {
                QList <double> sim_point;
                sim_point.append(0.0);
                sim_point.append(line.section('=',1,1).toDouble());
                line = ngsp_data.readLine();
                sim_point.append(line.section('=',1,1).toDouble());
                sim_points.append(sim_point);
                cnt++;
            }
        }
        if (cnt>1) ParSwp = true;
        ofile.close();
    }
}

void AbstractSpiceKernel::parsePZOutput(QString ngspice_file, QList<QList<double> > &sim_points,
                                        QStringList &var_list, bool &ParSwp)
{
    //static bool zeros = false; // first run --- poles; second run --- zeros
                        // because poles and zeros vectors have unequal dimension
    QString var;
    if (a_parsePZzeros) var = "zero";
    else var = "pole";

    var_list.clear();
    sim_points.clear();
    ParSwp = false;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        QStringList lines = ngsp_data.readAll().split("\n");

        if (lines.count("PZ analysis")>1) ParSwp = true;

        for (const QString& lin : lines) {  // Extract poles
            if (lin.contains(var + "(")) {
                if (!var_list.contains(var)) {
                    var_list.append(var+"_number");
                    var_list.append(var);
                }
                QList <double> sim_point;
                sim_point.append(lin.section('(',1,1).section(')',0,0).toDouble());
                QString right = lin.section("=",1,1);
                sim_point.append(right.section(",",0,0).toDouble());
                sim_point.append(right.section(",",1,1).toDouble());
                sim_points.append(sim_point);
            }
        }
        a_parsePZzeros = !a_parsePZzeros;
        ofile.close();
    }
}

/*!
 * \brief AbstractSpiceKernel::parseSENSOutput Parse output after DC sensitivity analysis.
 * \param[in] ngspice_file Spice output file name
 * \param[out] sim_points 2D array in which simulation points should be extracted. All simulation
 *        points from all sweep variable steps are extracted in a single array
 * \param[out] var_list This list is filled by simulation variables. There is a list of dependent
 *        and independent variables. An independent variable is the first in list.
 */
void AbstractSpiceKernel::parseSENSOutput(QString ngspice_file, QList<QList<double> > &sim_points,
                                          QStringList &var_list)
{
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        QStringList lines = ngsp_data.readAll().split("\n");
        // Extract variables
        int cnt = 0;
        for (auto lin=lines.begin(); lin != lines.end(); lin++) {
            if (lin->contains("Sens analysis")) cnt++;
            if ( lin->contains('=')) {
                QString var = (*lin).section("=",0,0).trimmed();
                var_list.append(var);
            }
            if (cnt>=2) break;
        }

        // Extract values
        QList <double> sim_point;
        cnt = 0;
        for (auto lin=lines.begin(); lin != lines.end(); lin++) {
            if (lin->contains('=')) {
                double val = (*lin).section("=",1,1).trimmed().toDouble();
                sim_point.append(val);
                cnt++;
            }
            if (cnt >= var_list.count()) {
                sim_points.append(sim_point);
                sim_point.clear();
                cnt = 0;
            }
        }
        ofile.close();
    }
}

/*!
 * \brief AbstractSpiceKernel::parseDC_OPoutput Parse DC OP simulation result and setup
 *        schematic node names to show DC bias
 * \param ngspice_file[in] DC OP results test file
 */
void AbstractSpiceKernel::parseDC_OPoutput(QString ngspice_file)
{
    QHash<QString,double> NodeVals;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        QStringList lines = ngsp_data.readAll().split("\n");
        for (const QString& lin : lines) {
            if (lin.contains('=')) {
                QString nod = lin.section('=',0,0).remove(' ');
                double val = lin.section('=',1,1).toDouble();
                NodeVals.insert(nod,val);
            }
        }
        ofile.close();
    }

    // Update Node labels on schematic
    SweepDialog *swpdlg = new SweepDialog(a_schematic,&NodeVals);
    delete swpdlg;

    a_schematic->setShowBias(1);
}

/*!
 * \brief AbstractSpiceKernel::parseDC_OPoutputXY Parse DC OP simulation result for XYCE
 *        and setup schematic node names to show DC bias
 * \param ngspice_file[in] DC OP results test file
 */
void AbstractSpiceKernel::parseDC_OPoutputXY(QString xyce_file)
{
    QHash<QString,double> NodeVals;
    QFile ofile(xyce_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        QStringList lines = ngsp_data.readAll().split("\n");
        if (lines.count()>=2) {
            QStringList nods = lines.at(0).split(QRegularExpression("\\s"), Qt::SkipEmptyParts);
            QStringList vals = lines.at(1).split(QRegularExpression("\\s"), Qt::SkipEmptyParts);
            QStringList::iterator n,v;
            for(n = nods.begin(),v = vals.begin();n!=nods.end()||v!=vals.end();n++,v++) {
                if ((*n).startsWith("I(")) {
                    (*n).remove(0,2).chop(1);
                    (*n) += "#branch";  // Ngspice compatible
                } else (*n).remove(0,2).chop(1); // Remove (I|V), starting ( and closing )
                NodeVals.insert((*n).toLower(),(*v).toDouble());
            }
        }
        ofile.close();
    }

    // Update Node labels on schematic
    SweepDialog *swpdlg = new SweepDialog(a_schematic,&NodeVals);
    delete swpdlg;

    a_schematic->setShowBias(1);
}

/*!
 * \brief AbstractSpiceKernel::parseSTEPOutput This method parses text raw spice
 *        output from Parameter sweep analysis. Can parse data that uses appedwrite.
 *        Extracts a simulation points array and variables names and types (Real
 *        or Complex) from output.
 * \param ngspice_file Spice output file name
 * \param sim_points 2D array in which simulation points should be extracted. All simulation
 *        points from all sweep variable steps are extracted in a single array
 * \param var_list This list is filled by simulation variables. There is a list of dependent
 *        and independent variables. An independent variable is the first in list.
 * \param isComplex Type of variables. True if complex. False if real.
 */
void AbstractSpiceKernel::parseSTEPOutput(QString ngspice_file,
                     QList< QList<double> > &sim_points,
                     QStringList &var_list, bool &isComplex)
{
    isComplex = false;
    bool isBinary = false;
    int bin_offset = 0;
    QByteArray content;

    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        content = ofile.readAll();
        ofile.close();
    }

    QTextStream ngsp_data(&content);
    sim_points.clear();
    bool start_values_sec = false;
    bool header_parsed = false;
    int NumVars=0; // Number of dep. and indep.variables
    int NumPoints=0; // Number of simulation points
    while (!ngsp_data.atEnd()) {
        QRegularExpression sep("[ \t,]");
        QString lin = ngsp_data.readLine();
        if (lin.isEmpty()) continue;
        if (lin.contains("Plotname:")&&  // skip operating point
            (lin.contains("DC operating point"))) {
            for(bool t = false; !t; t = (ngsp_data.readLine().startsWith("Plotname:")));
        }
        if (!header_parsed) {
            if (lin.contains("Flags")&&lin.contains("complex")) { // output consists of
                isComplex = true; // complex numbers
                continue;         // maybe ac_analysis
            }
            if (lin.contains("No. Variables")) {  // get number of variables
                NumVars=lin.section(sep,2,2,QString::SectionSkipEmpty).toInt();
                continue;
            }
            if (lin.contains("No. Points:")) {  // get number of variables
                NumPoints=lin.section(sep,2,2,QString::SectionSkipEmpty).toInt();
                continue;
            }
            if (lin=="Variables:") {
                var_list.clear();
                QString indep_var = ngsp_data.readLine().section(sep,1,1,QString::SectionSkipEmpty);
                var_list.append(indep_var);

                for (int i=1;i<NumVars;i++) {
                    lin = ngsp_data.readLine();
                    QString dep_var = lin.section(sep,1,1,QString::SectionSkipEmpty);
                    var_list.append(dep_var);
                }
                header_parsed = true;
                continue;
            }
        }

        if (lin=="Values:") {
            start_values_sec = true;
            continue;
        }
        if (lin=="Binary:") {
            isBinary = true;
            bin_offset = ngsp_data.pos();
        }

        if (isBinary) {
            QDataStream dbl(content);
            dbl.setByteOrder(QDataStream::LittleEndian);
            dbl.device()->seek(bin_offset);
            extractBinSamples(dbl,sim_points,NumPoints,NumVars,isComplex);
            int pos = dbl.device()->pos();
            ngsp_data.seek(pos);
            isBinary = false;
            continue;
        }


        if (start_values_sec) {
            if (!extractASCIISamples(lin,ngsp_data,sim_points,NumVars,isComplex)) continue;
        }

    }
}

/*!
 * \brief AbstractSpiceKernel::parsePrnOutput Parse scalar print output.
 * \param[in] ngspice_file Spice output file name
 * \param[out] sim_points 2D array in which simulation points should be extracted. All simulation
 *        points from all sweep variable steps are extracted in a single array
 * \param[out] var_list This list is filled by simulation variables. There is a list of dependent
 *        and independent variables. An independent variable is the first in list.
 */
void AbstractSpiceKernel::parsePrnOutput(const QString &ngspice_file,
                                         QList<QList<double> > &sim_points,
                                         QStringList &var_list,
                                         bool isComplex) {
    QChar eq_sep = '=';
    QChar reim_sep = ',';
    QList <double> sim_point;

    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        var_list.clear();
        sim_points.clear();
        var_list.append("");  // dummy indep var
        sim_point.append(0.0);
        QTextStream data(&ofile);
        while (!data.atEnd()) {
            QString line = data.readLine();
            if (line.contains(eq_sep)) {
                QString var = line.section(eq_sep, 0, 0).trimmed();
                if (!var.startsWith('_')) {
                    QString val = line.section(eq_sep, 1, 1).trimmed();
                    double re = val.section(reim_sep, 0, 0).toDouble();
                    double im = val.section(reim_sep, 1, 1).toDouble();
                    var_list.append(var);
                    sim_point.append(re);
                    if ( isComplex )
                        sim_point.append(im);
                }
            }
        }
        sim_points.append(sim_point);
        ofile.close();
    }
}

void AbstractSpiceKernel::extractBinSamples(QDataStream &dbl, QList<QList<double> > &sim_points,
                                            int NumPoints, int NumVars, bool isComplex)
{
    int cnt = NumPoints;
    while (cnt>0) {
        QList<double> sim_point;
        double re,im;
        dbl>>re; // Indep. variable
        sim_point.append(re);
        if (isComplex) dbl>>im; // drop Im part of indep.var
        for (int i=1;i<NumVars;i++) { // first variable is independent
            if (isComplex) {
                dbl>>re; // Re
                dbl>>im; // Im
                sim_point.append(re);
                sim_point.append(im);
            } else {
                dbl>>re;
                sim_point.append(re); // Re
            }
        }
        sim_points.append(sim_point);
        cnt--;
    }
}

bool AbstractSpiceKernel::extractASCIISamples(QString &lin, QTextStream &ngsp_data,
                                              QList<QList<double> > &sim_points, int NumVars, bool isComplex)
{
    QRegularExpression sep("[ \t,]");
    QList<double> sim_point;
    bool ok = false;
    QRegularExpression dataline_patter("^ *[0-9]+[ \t]+.*");
    if (!dataline_patter.match(lin).hasMatch()) return false;
    double indep_val = lin.section(sep,1,1,QString::SectionSkipEmpty).toDouble(&ok);
    //double indep_val = lin.split(sep,QString::SkipEmptyParts).at(1).toDouble(&ok); // only real indep vars
    if (!ok) return false;
    sim_point.append(indep_val);
    for (int i=0;i<NumVars;i++) {
        if (isComplex) {
            QStringList lst = ngsp_data.readLine().split(sep, Qt::SkipEmptyParts);
            if (lst.count()==2) {
                double re_dep_val = lst.at(0).toDouble();  // for complex sim results
                double im_dep_val = lst.at(1).toDouble();  // imaginary part follows
                sim_point.append(re_dep_val);              // real part
                sim_point.append(im_dep_val);
            }
        } else {
            double dep_val = ngsp_data.readLine().remove(sep).toDouble();
            sim_point.append(dep_val);
        }
    }
    sim_points.append(sim_point);
    return true;
}

/*!
 * \brief AbstractSpiceKernel::parseXYCESTDOutput
 * \param std_file[in] XYCE STD output file name
 * \param sim_points[out] 2D array in which simulation points should be extracted
 * \param var_list[out] This list is filled by simulation variables. There is a list of dependent
 *        and independent variables. An independent variable is the first in list.
 * \param isComplex[out] Type of variables. True if complex. False if real.
 */
void AbstractSpiceKernel::parseXYCESTDOutput(QString std_file, QList<QList<double> > &sim_points,
                                             QStringList &var_list, bool &isComplex, bool &hasParSweep)
{
    isComplex = false;
    QString content;

    QFile ofile(std_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ts(&ofile);
        content = ts.readAll();
        ofile.close();
    }

    QTextStream ngsp_data(&content);
    sim_points.clear();
    var_list.clear();
    QStringList complex_var_list;
    QList<int> complex_var_idx;
    while (!ngsp_data.atEnd()) { // Parse header;
        QString lin = ngsp_data.readLine();
        if (lin.isEmpty()) continue;
        if (lin.contains("Parameter Sweep")) {
            hasParSweep = true;
            continue;
        }
        if (lin.startsWith("End of ")) continue;
        if (lin.startsWith("Index ",Qt::CaseInsensitive)) {
            var_list = lin.split(" ", Qt::SkipEmptyParts);
            var_list.removeFirst(); // Drop Index
            for(int i = 0; i < var_list.count()-1; i++) {
                QString var_re = var_list.at(i);
                QString var_im = var_list.at(i+1);
                if (var_re.startsWith("Re(") &&
                    var_im.startsWith("Im(")) {
                    QString var = var_re;
                    var.remove(0,3);
                    var.chop(1);
                    complex_var_list.append(var);
                    complex_var_idx.append(i+1);
                    isComplex = true;
                }
            }
            continue;
        } else {
            QStringList val_lst = lin.split(" ", Qt::SkipEmptyParts);
            QList<double> sim_point;
            for (int i = 1; i <= var_list.count(); i++ ) {
                if (isComplex && i != 1) {
                    sim_point.append(val_lst.at(i).toDouble()); // Re and Im
                    sim_point.append(0.0);                      // real vars
                } else {
                    sim_point.append(val_lst.at(i).toDouble());
                }
            }
            if (isComplex) { // reassemble complex variables
                for (int j = 0; j < complex_var_list.count(); j++) {
                    int idx = complex_var_idx[j];
                    sim_point.append(val_lst.at(idx).toDouble());
                    sim_point.append(val_lst.at(idx+1).toDouble());
                }
            }
            //sim_point.removeFirst(); // Index
            sim_points.append(sim_point);
        }
    }
    if (isComplex) {
        var_list.append(complex_var_list);
    }
}


/*!
 * \brief AbstractSpiceKernel::parseXYCENoiseLog
 * \param logfile
 * \param sim_points
 * \param var_list
 */
void AbstractSpiceKernel::parseXYCENoiseLog(QString logfile, QList<QList<double> > &sim_points,
                                            QStringList &var_list)
{
    var_list.clear();
    var_list.append(""); // dummy indep var
    var_list.append("ONOISE_TOTAL");
    var_list.append("INOISE_TOTAL");
    QString content;
    QList <double> sim_point;
    sim_point.append(0.0);

    QFile ofile(logfile);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ts(&ofile);
        content = ts.readAll();
        ofile.close();
    }

    QTextStream data(&content);
    sim_points.clear();
    while (!data.atEnd()) { // Parse header;
        QString lin = data.readLine();
        if (lin.startsWith("Total Output Noise")) {
            double val = lin.section('=',1,1).toDouble();
            sim_point.append(val);
        }
        if (lin.startsWith("Total Input Noise")) {
            double val = lin.section('=',1,1).toDouble();
            sim_point.append(val);
        }
    }
    sim_points.append(sim_point);
}

/*!
 * \brief AbstractSpiceKernel::parseResFile Extract sweep variable name and
 *        values from Ngspice or Xyce *.res output
 * \param resfile A name of a *.res file
 * \param var QString in which war is stored
 * \param values String list in which values are extracted
 */
void AbstractSpiceKernel::parseResFile(QString resfile, QString &var, QStringList &values)
{
    var.clear();
    values.clear();

    QFile ofile(resfile);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream swp_data(&ofile);
        QRegularExpression point_pattern("^\\s*[0-9]+ .*");
        QRegularExpression var_pattern("^STEP\\s+.*");
        QRegularExpression sep("\\s");
        while (!swp_data.atEnd()) {
            QString lin = swp_data.readLine();
            if (var_pattern.match(lin).hasMatch()) {
                var = lin.split(sep, Qt::SkipEmptyParts).last();
            }
            if (point_pattern.match(lin).hasMatch()) {
                values.append(lin.split(sep, Qt::SkipEmptyParts).last());
            }
        }
        ofile.close();
    }
}

/*!
 * \brief AbstractSpiceKernel::checkRawOutupt Determine Ngspice Raw output contains
 *        parameter sweep or is XYCE STD output.
 * \param ngspice_file[in] Raw output file name
 * \param values[out] Numbers of parameter sweep steps
 * \return true if parameter sweep presents, false otherwise
 */
int AbstractSpiceKernel::checkRawOutupt(QString ngspice_file, QStringList &values)
{
    values.clear();

    QFile ofile(ngspice_file);
    int plots_cnt = 0;
    int zeroindex_cnt = 0;
    int prnln_cnt = 0;
    bool isXyce = false;
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        QRegularExpression prnln_rx("^[A-Za-z].*\\s=\\s-?\\d.\\d+[Ee][+-]\\d+");
        QRegularExpression rx("^0\\s+[0-9].*"); // Zero index pattern
        while (!ngsp_data.atEnd()) {
            QString lin = ngsp_data.readLine();
            if (lin.startsWith("Plotname: ")) {
                plots_cnt++;
                values.append(QString::number(plots_cnt));
            }
            if (lin.startsWith("End of Xyce(TM)")) isXyce = true;
            if (rx.match(lin).hasMatch()) {
                zeroindex_cnt++;
                values.append(QString::number(zeroindex_cnt));
            }
            if (prnln_rx.match(lin).hasMatch())
                prnln_cnt++;
        }
        ofile.close();
    }
    int filetype = Unknown;
    if (plots_cnt>1) filetype = spiceRawSwp;
    else if ((plots_cnt == 0)&&(isXyce)){
        if (zeroindex_cnt>1) filetype = xyceSTDswp;
        else filetype = xyceSTD;
    } else filetype = spiceRaw;
    if ( (plots_cnt == 0) && (zeroindex_cnt == 0) && (prnln_cnt > 0) )
        filetype = spicePrn;
    return filetype;
}

/*!
 * \brief AbstractSpiceKernel::convertToQucsData Put data extracted from spice raw
 *        text output files (given in outputs_files property) into single XML
 *        Qucs Dataset.
 * \param qucs_dataset A file name of Qucs Dataset to create
 * \param xyce True if Xyce simulator was used.
 */
void AbstractSpiceKernel::convertToQucsData(const QString &qucs_dataset)
{
    if (a_DC_OP_only) { // Don't touch existing datasets when only DC was simulated
        // It's need to show DC bias on schematic only
        for (const QString& outputfile : a_output_files) {
            QString full_outfile = a_workdir+QDir::separator()+outputfile;
            if (outputfile.endsWith(".dc_op")) {
                parseDC_OPoutput(full_outfile);
            } else if (outputfile.endsWith(".dc_op_xyce")) {
                parseDC_OPoutputXY(full_outfile); }
        }
        return;
    }

    // Merge all outputs in a single Qucs dataset otherwise
    QString ds_str;
    QTextStream ds_stream(&ds_str);

    ds_stream<<"<Qucs Dataset " PACKAGE_VERSION ">\n";

    QString sim,indep;
    QStringList indep_vars;

    for (const QString& ngspice_output_filename : a_output_files) { // For every simulation convert results to Qucs dataset
        QList< QList<double> > sim_points;
        QStringList var_list, digital_vars;
        QString swp_var,swp_var2;
        QStringList swp_var_val,swp_var2_val;
        bool isComplex = false;
        bool hasParSweep = false;
        bool hasDblParSweep = false;
        QList<int> dig_vars_dims;

        QString dataset_prefix;
        bool isCustomPrefix = false;
        if ( ngspice_output_filename.startsWith("spice4qucs.") ) {
            dataset_prefix = ngspice_output_filename.section('.', 1, 1).toLower();
        } else {
            QRegularExpression dataset_prefix_rx("(?<=#).*?(?=#)");
            dataset_prefix = dataset_prefix_rx.match(ngspice_output_filename).captured(0).toLower();
            isCustomPrefix = !dataset_prefix.isEmpty();
        }
        QRegularExpression four_rx(".*\\.four[0-9]+$");
        QString full_outfile = a_workdir+QDir::separator()+ngspice_output_filename;
        if (ngspice_output_filename.endsWith("HB.FD.prn")) {
            //parseHBOutput(full_outfile,sim_points,var_list,hasParSweep);
            //isComplex = true;
            parseXYCESTDOutput(full_outfile,sim_points,var_list,isComplex,hasParSweep);
            if (hasParSweep) {
                QString res_file = QDir::toNativeSeparators(a_workdir + QDir::separator()
                                                        + "spice4qucs.hb.cir.res");
                parseResFile(res_file,swp_var,swp_var_val);
            }
        } else if (ngspice_output_filename.endsWith(".four") ||
                   four_rx.match(ngspice_output_filename).hasMatch()) {
            isComplex=false;
            parseFourierOutput(full_outfile,sim_points,var_list);
        } else if (ngspice_output_filename.endsWith(".ngspice.sens.dc.prn")) {
            isComplex = false;
            parseSENSOutput(full_outfile,sim_points,var_list);
        } else if (ngspice_output_filename.endsWith(".txt_std")) {
            parseXYCESTDOutput(full_outfile,sim_points,var_list,isComplex,hasParSweep);
        } else if (ngspice_output_filename.endsWith(".noise_log")) {
            isComplex = false;
            parseXYCENoiseLog(full_outfile,sim_points,var_list);
        } else if (ngspice_output_filename.endsWith(".noise")) {
            isComplex = false;
            parseNoiseOutput(full_outfile,sim_points,var_list,hasParSweep);
            if (hasParSweep) {
                QString res_file = QDir::toNativeSeparators(a_workdir + QDir::separator()
                                                        + "spice4qucs." + dataset_prefix + ".cir.res");
                parseResFile(res_file,swp_var,swp_var_val);
            }
        } else if (ngspice_output_filename.endsWith(".pz")) {
            isComplex = true;
            parsePZOutput(full_outfile,sim_points,var_list,hasParSweep);
            if (hasParSweep) {
                QString res_file = QDir::toNativeSeparators(a_workdir + QDir::separator()
                                                        + "spice4qucs." + dataset_prefix + ".cir.res");
                parseResFile(res_file,swp_var,swp_var_val);
            }
        } else if (ngspice_output_filename.endsWith(".SENS.prn")) {
            QStringList vals;
            int type = checkRawOutupt(full_outfile,vals);
            parseXYCESTDOutput(full_outfile,sim_points,var_list,isComplex,hasParSweep);
            if (type == xyceSTDswp) {
                hasParSweep = true;
                QString res_file = QDir::toNativeSeparators(a_workdir + QDir::separator()
                                                        + "spice4qucs.sens.cir.res");
                parseResFile(res_file,swp_var,swp_var_val);
            }
        } else if (ngspice_output_filename.endsWith("_swp.plot")) {
            hasParSweep = true;
            if (ngspice_output_filename.endsWith("_swp_swp.plot")) { // 2-var parameter sweep
                hasDblParSweep = true;
                QString res2_file = QDir::toNativeSeparators(a_workdir + QDir::separator()
                                                            + "spice4qucs." + dataset_prefix + ".cir.res1");
                parseResFile(res2_file,swp_var2,swp_var2_val);
            }

            QString res_file = QDir::toNativeSeparators(a_workdir + QDir::separator()
                                                    + "spice4qucs." + dataset_prefix + ".cir.res");
            parseResFile(res_file,swp_var,swp_var_val);

            parseSTEPOutput(full_outfile,sim_points,var_list,isComplex);
        } else {
            int OutType = checkRawOutupt(full_outfile,swp_var_val);
            bool hasSwp = false;
            switch (OutType) {
            case spiceRawSwp:
                hasParSweep = true;
                swp_var = "Number";
                parseSTEPOutput(full_outfile,sim_points,var_list,isComplex);
                break;
            case spiceRaw:
                parseNgSpiceSimOutput(full_outfile, sim_points, var_list, isComplex, digital_vars, dig_vars_dims);
                break;
            case xyceSTD:
                parseXYCESTDOutput(full_outfile,sim_points,var_list,isComplex,hasSwp);
                break;
            case xyceSTDswp:
                hasParSweep = true;
                swp_var = "Number";
                parseXYCESTDOutput(full_outfile,sim_points,var_list,isComplex,hasSwp);
                break;
            case spicePrn:
                isComplex = true;
                parsePrnOutput(full_outfile, sim_points, var_list, isComplex);
                break;
            default: break;
            }
        }
        if (var_list.isEmpty()) continue; // nothing to convert
        normalizeVarsNames(var_list, dataset_prefix, isCustomPrefix);
        digital_vars.prepend(var_list.first());
        normalizeVarsNames(digital_vars, dataset_prefix, isCustomPrefix);

        QString indep = var_list.first();
        //QList<double> sim_point;


        if (hasParSweep) {
            int indep_cnt;
            if (swp_var_val.isEmpty()) continue;
            if (hasDblParSweep&&swp_var2_val.isEmpty()) continue;
            if (hasDblParSweep) indep_cnt =  sim_points.count()/(swp_var_val.count()*swp_var2_val.count());
            else indep_cnt = sim_points.count()/swp_var_val.count();
            if (!indep.isEmpty()) {
                ds_stream<<QStringLiteral("<indep %1 %2>\n").arg(indep).arg(indep_cnt); // output indep var: TODO: parameter sweep
                for (int i=0;i<indep_cnt;i++) {
                    ds_stream<<QString::number(sim_points.at(i).at(0),'e',12)<<"\n";
                }
                ds_stream<<"</indep>\n";
            }

            ds_stream<<QStringLiteral("<indep %1 %2>\n").arg(swp_var).arg(swp_var_val.count());
            for (const QString& val : swp_var_val) {
                ds_stream<<val<<"\n";
            }
            ds_stream<<"</indep>\n";
            if (indep.isEmpty()) indep = swp_var;
            else indep += " " + swp_var;
            if (hasDblParSweep) {
                ds_stream<<QStringLiteral("<indep %1 %2>\n").arg(swp_var2).arg(swp_var2_val.count());
                for (const QString& val : swp_var2_val) {
                    ds_stream<<val<<"\n";
                }
                ds_stream<<"</indep>\n";
                indep += " " + swp_var2;
            }
        } else if (!indep.isEmpty()) {
            ds_stream<<QStringLiteral("<indep %1 %2>\n").arg(indep).arg(sim_points.count()); // output indep var: TODO: parameter sweep
            for (auto& sim_point : sim_points) {
                ds_stream<<QString::number(sim_point.at(0),'e',12)<<"\n";
            }
            ds_stream<<"</indep>\n";
        }

        int dig_var_idx = 0;
        for(int i=1;i<var_list.count();i++) { // output dep var
            bool is_digital_var = false;
            bool digital_indep = false;
            if (indep.isEmpty()) {
              ds_stream<<QStringLiteral("<indep %1 %2>\n").arg(var_list.at(i)).arg(sim_points.count());
            } else {
              QString var = var_list.at(i);
              is_digital_var = digital_vars.contains(var);
              if (is_digital_var && !var.endsWith("_steps")) { // XSPICE digital node
                // requires another X-variable; not time
                QString var2 = var + "_steps";
                var2.remove("v(");
                var2.remove("i(");
                var2.remove(")");
                if (hasParSweep) {
                  var2 += " " + swp_var;
                  if (hasDblParSweep) var += " " + swp_var2;
                }
                ds_stream<<QStringLiteral("<dep %1 %2>\n").arg(var).arg(var2);
              } else if (is_digital_var && var.endsWith("_steps") && // indep XSPICE digital var
                         !var.contains("(") && !var.contains(")")) {
                digital_indep = true;
                ds_stream<<QStringLiteral("<indep %1 %2>\n").arg(var).arg(dig_vars_dims.at(dig_var_idx));
              } else {
                ds_stream<<QStringLiteral("<dep %1 %2>\n").arg(var_list.at(i)).arg(indep);
              }
            }
            int count = 0;
            for (auto& sim_point : sim_points) {
                if (is_digital_var && count > dig_vars_dims.at(dig_var_idx)) break;
                if (isComplex) {
                    double re=sim_point.at(2*(i-1)+1);
                    double im = sim_point.at(2*i);
                    QString s;
                    s += QString::number(re,'e',12);
                    if (im<0) s += "-j";
                    else s += "+j";
                    s += QString::number(fabs(im),'e',12) + "\n";
                    ds_stream<<s;
                } else {
                    ds_stream<<QString::number(sim_point.at(i),'e',12)<<"\n";
                }
                count++;
            }
            if (indep.isEmpty() || digital_indep) {
              ds_stream<<"</indep>\n";
            } else {
              ds_stream<<"</dep>\n";
            }
            if (is_digital_var) dig_var_idx++;
        }
    }

    QFile dataset(qucs_dataset);
    if (dataset.open(QFile::WriteOnly)) {
        QTextStream ts(&dataset);
        ts<<ds_str;
        dataset.close();
    } else {
        QFileInfo inf(qucs_dataset);
        QMessageBox::warning(nullptr, tr("Simulate"),
                             tr("Failed to create dataset file ") + qucs_dataset + "\n"
                             + tr("Check write permission of the directory ") + inf.path());
    }
#ifdef NDEBUG
    removeAllSimulatorOutputs();
#endif
}

/*!
 * \brief AbstractSpiceKernel::removeAllSimulatorOutputs Clean temporary simulator
 *        datasets.
 */
void AbstractSpiceKernel::removeAllSimulatorOutputs()
{
    for (const QString& output_filename : a_output_files) {
        QString full_outfile = a_workdir+QDir::separator()+output_filename;
        QFile::remove(full_outfile);
    }
    QDir dir(a_workdir);
    dir.setNameFilters(QStringList() << "*.cir.res*");
    dir.setFilter(QDir::Files);
    foreach(QString file, dir.entryList())
        dir.remove(file);
}

/*!
 * \brief AbstractSpiceKernel::normalizeVarsNames Convert spice-style variable names to
 *        Qucs style and add simualation type prefix (i.e. AC, TRAN, DC). Conversion
 *        for harmonic balance variable and current probes variables are supported.
 * \param var_list This list contains variable names that need normalization.
 */
void AbstractSpiceKernel::normalizeVarsNames(QStringList &var_list, const QString &dataset_prefix, bool isCustom)
{
    QString prefix="";
    QString iprefix="";
    QString indep = var_list.first();
    bool HB = false;
    indep = indep.toLower();
    if (indep=="time") {
        prefix = "tran.";
        iprefix = "i(tran.";
    } else if (indep=="frequency") {
        prefix = "ac.";
        iprefix = "i(ac.";
    } else if (indep=="hbfrequency") {
        HB = true;
    }

    for(auto & it : var_list) { // For subcircuit nodes output i.e. v(X1:n1)
        it.replace(":","_");         // colon symbol is reserved in Qucs as dataset specifier
    }

    QStringList::iterator it=var_list.begin();

    QRegularExpression iprobe_pattern("^[Vv][Pp][Rr][0-9]+.*");
    QRegularExpression ivprobe_pattern("^[Vv][Pp][Rr][0-9]+.*");
    QRegularExpression ivprobe_pattern_ngspice("^(ac\\.|tran\\.)[Vv][Pp][Rr][0-9]+.*");

    for (it++;it!=var_list.end();it++) {
        if ((!(it->startsWith(prefix)||it->startsWith(iprefix)))||(HB)) {
            if (HB) {
                QString suffix;
                if ((*it).startsWith('I')) suffix = ".Ib";
                else suffix = ".Vb";
                auto idx = it->indexOf('(');
                auto cnt = it->size();
                *it = it->right(cnt-idx-1);
                it->remove(')');
                *it += suffix;

                if (iprobe_pattern.match(*it).hasMatch()) (*it).remove(0,1);
            } else {
                *it = prefix + *it;
            }
        }
        QStringList lst = it->split('(');
        if (lst.count()>1) {

            if (ivprobe_pattern.match(lst.at(1)).hasMatch()) {
                lst[1].remove(0,1);
                *it = lst.join("(");
            } else if (ivprobe_pattern_ngspice.match(lst.at(1)).hasMatch()) {
                lst[1].replace(".v",".",Qt::CaseInsensitive);
                *it = lst.join("(");
            }
        }
    }

    if ( a_needsPrefix || isCustom )
        if ( !dataset_prefix.isEmpty() ) {
            for ( it = var_list.begin() ; it != var_list.end() ; ++it)
                if ( !(*it).isEmpty() )
                    (*it).prepend(dataset_prefix + ".");
        }
}

/*!
 * \brief AbstractSpiceKernel::slotErrors Simulator errors handler
 * \param err
 */
void AbstractSpiceKernel::slotErrors(QProcess::ProcessError err)
{
    emit errors(err);
}

/*!
 * \brief AbstractSpiceKernel::slotFinished Simulation process normal finish handler
 */
void AbstractSpiceKernel::slotFinished()
{
    //a_output.clear();
    a_output += a_simProcess->readAllStandardOutput();
    emit finished();
    emit progress(100);
}

/*!
 * \brief AbstractSpiceKernel::slotProcessOutput Process a_simProcess output and report progress
 */
void AbstractSpiceKernel::slotProcessOutput()
{

}

/*!
 * \brief AbstractSpiceKernel::getOutput Get sdtout and stderr output of simulation
 *        process.
 * \return Simulation process output
 */
QString AbstractSpiceKernel::getOutput()
{
    return a_output;
}

/*!
 * \brief AbstractSpiceKernel::setSimulatorCmd Set simulator executable location
 * \param cmd Simulator executable absolute path. For example /usr/bin/ngspice
 */
void AbstractSpiceKernel::setSimulatorCmd(QString cmd)
{
    a_simulator_cmd = cmd;
}


/*!
 * \brief AbstractSpiceKernel::setSimulatorParameters Set simulator parameters
 * \param cmd Simulator executable absolute path. For example -plugin lib/Xyce_ADMS_Plugin.so
 */
void AbstractSpiceKernel::setSimulatorParameters(QString parameters)
{
    a_simulator_parameters = parameters;
}

/*!
 * \brief AbstractSpiceKernel::setWorkdir Set simulator working directory path
 *        to store netlist and temp data.
 * \param path[in] New working directory path
 */
void AbstractSpiceKernel::setWorkdir(QString path)
{
    a_workdir = path;
    QFileInfo inf(a_workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(a_workdir);
    }
}

/*!
 * \brief AbstractSpiceKernel::SaveNetlist Save netlist to file. Reimplemented
 *        in Ngspice and Xyce classes.
 */
void AbstractSpiceKernel::SaveNetlist(QString)
{

}

bool AbstractSpiceKernel::waitEndOfSimulation()
{
    return a_simProcess->waitForFinished(10000);
}

QString AbstractSpiceKernel::collectSpiceLibs(Schematic* sch)
{
  QStringList collected_spicelib;
  for(Component *pc = sch->a_DocComps.first(); pc != 0; pc = sch->a_DocComps.next()) {
    if (pc->Model == "Sub") {
      Schematic *sub = new Schematic(0, ((Subcircuit *)pc)->getSubcircuitFile());
      if(!sub->loadDocument())      // load document if possible
      {
        delete sub;
        continue;
      }
      QString libstr = collectSpiceLibs(sub);
      if (!collected_spicelib.contains(libstr)) {
        collected_spicelib.append(libstr);
      }
      delete sub;
    } else {
      QString libstr = pc->getSpiceLibrary();
      if (!collected_spicelib.contains(libstr)) {
        collected_spicelib.append(libstr);
      }
    }
  }
  return collected_spicelib.join("");
}


QStringList AbstractSpiceKernel::collectSpiceLibraryFiles(Schematic *sch)
{
  QStringList collected_spicelib;
  for(Component *pc = sch->a_DocComps.first(); pc != 0; pc = sch->a_DocComps.next()) {
    QStringList new_libs;
    if (pc->Model == "Sub") {
      Schematic *sub = new Schematic(nullptr, ((Subcircuit *)pc)->getSubcircuitFile());
      if(!sub->loadDocument())      // load document if possible
      {
        delete sub;
        continue;
      }
      new_libs = collectSpiceLibraryFiles(sub);
      delete sub;
    } else {
      new_libs = pc->getSpiceLibraryFiles();
    }
    for (const auto&lib: new_libs) {
      if (!collected_spicelib.contains(lib)) {
        collected_spicelib.append(lib);
      }
    }
  }
  return collected_spicelib;
}

