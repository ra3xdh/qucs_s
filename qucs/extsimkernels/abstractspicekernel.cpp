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
#include <QPlainTextEdit>

/*!
  \file abstractspicekernel.cpp
  \brief Implementation of the AbstractSpiceKernel class
*/


/*!
 * \brief AbstractSpiceKernel::AbstractSpiceKernel class constructor
 * \param sch_ Schematic that schould be simualted with Spice-comaptibele
 *        simulator
 * \param parent Parent object
 */
AbstractSpiceKernel::AbstractSpiceKernel(Schematic *sch_, QObject *parent) :
    QObject(parent)
{
    Sch = sch_;

    if (Sch->showBias == 0) DC_OP_only = true;
    else DC_OP_only = false;

    workdir = QucsSettings.S4Qworkdir;
    QFileInfo inf(workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(workdir);
    }

    SimProcess = new QProcess(this);
    SimProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(SimProcess,SIGNAL(finished(int)),this,SLOT(slotFinished()));
    connect(SimProcess,SIGNAL(readyRead()),this,SLOT(slotProcessOutput()));
    connect(SimProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(slotErrors(QProcess::ProcessError)));
    connect(this,SIGNAL(destroyed()),this,SLOT(killThemAll()));

}


AbstractSpiceKernel::~AbstractSpiceKernel()
{
    killThemAll();
}

void AbstractSpiceKernel::killThemAll()
{
    if (SimProcess->state()!=QProcess::NotRunning) {
        SimProcess->kill();
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
bool AbstractSpiceKernel::prepareSpiceNetlist(QTextStream &stream)
{
    QStringList collect;
    QPlainTextEdit *err = new QPlainTextEdit;
    if (Sch->prepareNetlist(stream,collect,err)==-10) { // Broken netlist
        output.append(err->toPlainText());
        delete err;
        return false;
    }
    delete err;
    Sch->clearSignalsAndFileList(); // for proper build of subckts
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
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if ((!pc->isEquation)&&!(pc->isProbe)) {
            if (pc->SpiceModel.isEmpty() && pc->isActive) incompat.append(pc->Name);
        }
    }

    return incompat.isEmpty();
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
void AbstractSpiceKernel::startNetlist(QTextStream &stream, bool xyce)
{
        QString s;

        // Include Directives
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            if ((pc->SpiceModel==".INCLUDE")||
                (pc->Model=="SpLib")) {
                s = pc->getSpiceModel();
                stream<<s;
            }
        }

        // Parameters, Initial conditions, Options
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            if (pc->isEquation) {
                s = pc->getExpression(xyce);
                stream<<s;
            }
        }

        // Components
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
          if(Sch->isAnalog &&
             !(pc->isSimulation) &&
             !(pc->isEquation)) {
            s = pc->getSpiceNetlist(xyce);
            stream<<s;
          }
        }

        // Modelcards
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
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
    QString f = misc::properFileName(Sch->DocName);
    header = QString(".SUBCKT %1 ").arg(misc::properName(f));

    QList< QPair<int,QString> > ports;
    if(!prepareSpiceNetlist(stream)) {
        emit finished();
        emit errors(QProcess::FailedToStart);
        return;
    } // Unable to perform spice simulation
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->Model=="Port") {
            ports.append(qMakePair(pc->Props.first()->Value.toInt(),
                                   pc->Ports.first()->Connection->Name));
        }
    }
    qSort(ports);
    QPair<int,QString> pp;
    if (lib) header += " gnd "; // Ground node forwarding for Library
    foreach(pp,ports) {
        header += pp.second + " ";
    }

    Painting *pi;
    for(pi = Sch->SymbolPaints.first(); pi != 0; pi = Sch->SymbolPaints.next())
      if(pi->Name == ".ID ") {
        ID_Text *pid = (ID_Text*)pi;
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
    bool xyce = false;
    if ((QucsSettings.DefaultSimulator == spicecompat::simXyceSer)||
        (QucsSettings.DefaultSimulator == spicecompat::simXycePar)) xyce = true;
    startNetlist(stream,xyce);
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
 * \param var_list This list is filled by simualtion variables. There is a list of dependent
 *        and independent varibales. An independent variable is the first in list.
 * \param isComplex Type of variables. True if complex. False if real.
 */
void AbstractSpiceKernel::parseNgSpiceSimOutput(QString ngspice_file,QList< QList<double> > &sim_points,QStringList &var_list, bool &isComplex)
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
        QRegExp sep("[ \t,]");
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
 * \param var_list This list is filled by simualtion variables. There is a list of dependent
 *        varibales. Independent hbfrequency variable is always the first in this list.
 */
void AbstractSpiceKernel::parseHBOutput(QString ngspice_file,
                                        QList<QList<double> > &sim_points, QStringList &var_list)
{
    var_list.clear();
    sim_points.clear();
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream hb_data(&ofile);
        var_list.append("hbfrequency");
        while (!hb_data.atEnd()) {
            QString lin = hb_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.startsWith("Index")) { // CSV heading
                    QStringList vars1 = lin.split(" ",QString::SkipEmptyParts);
                    vars1.removeFirst();
                    vars1.removeFirst();
                    QStringList norm_vars;
                    foreach(QString v, vars1) { // Normalize variables
                        QString nv = v;
                        nv.remove(0,3).chop(1); // extract variable between "Re|Im(" and ")"
                        if (!norm_vars.contains(nv))
                            norm_vars.append(nv);
                    }
                    var_list.append(norm_vars);
            }
            if ((lin.contains(QRegExp("\\d*\\.\\d+[+-]*[eE]*[\\d]*")))) { // CSV dataline
                QStringList vals = lin.split(" ",QString::SkipEmptyParts);
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
 * \param var_list[out] This list is filled by simualtion variables. There is a list of dependent
 *        and independent varibales. An independent variable is the first in list.
 */
void AbstractSpiceKernel::parseFourierOutput(QString ngspice_file, QList<QList<double> > &sim_points,
                                             QStringList &var_list, bool xyce)
{
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        sim_points.clear();
        var_list.clear();
        var_list.append("fourierfreq");
        int Nharm; // number of harmonics
        bool firstgroup = false;
        while (!ngsp_data.atEnd()) {
            QRegExp sep("[ \t,]");
            QString lin = ngsp_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.contains("Fourier analysis for")) {
                QString var = lin.split(sep,QString::SkipEmptyParts).last();
                if (var.endsWith(':')) var.chop(1);
                var_list.append("magnitude("+var+")");
                var_list.append("phase("+var+")");
                var_list.append("norm(mag("+var+"))");
                var_list.append("norm(phase("+var+"))");
                continue;
            }
            if (lin.contains("No. Harmonics:")) {
                QString ss = lin.section(sep,2,2,QString::SectionSkipEmpty);
                if (ss.endsWith(',')) ss.chop(1);
                Nharm = ss.toInt();
                while (!ngsp_data.readLine().contains(QRegExp("Harmonic\\s+Frequency")));
                if (!xyce) lin = ngsp_data.readLine(); // dummy line
                for (int i=0;i<Nharm;i++) {
                    lin = ngsp_data.readLine();
                    if (!firstgroup) {
                        QList<double> sim_point;
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
        ofile.close();
    }
}

/*!
 * \brief AbstractSpiceKernel::parseNoiseOutput Parse output of .NOISE simulation.
 * \param[in] ngspice_file Spice output file name
 * \param[out] sim_points 2D array in which simulation points should be extracted. All simulation
 *        points from all sweep variable steps are extracted in a single array
 * \param[out] var_list This list is filled by simualtion variables. There is a list of dependent
 *        and independent varibales. An independent variable is the first in list.
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
    static bool zeros = false; // first run --- poles; second run --- zeros
                        // because poles and zeros vectors have unequal dimension
    QString var;
    if (zeros) var = "zero";
    else var = "pole";

    var_list.clear();
    sim_points.clear();
    ParSwp = false;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        QStringList lines = ngsp_data.readAll().split("\n");

        if (lines.count("PZ analysis")>1) ParSwp = true;

        foreach (QString lin, lines) {  // Extract poles
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
        zeros = !zeros;
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
        foreach (QString lin,lines) {
            if (lin.contains('=')) {
                QString nod = lin.section('=',0,0).remove(' ');
                double val = lin.section('=',1,1).toDouble();
                NodeVals.insert(nod,val);
            }
        }
        ofile.close();
    }

    // Update Node labels on schematic
    SweepDialog *swpdlg = new SweepDialog(Sch,&NodeVals);
    delete swpdlg;

    Sch->showBias = 1;
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
            QStringList nods = lines.at(0).split(QRegExp("\\s"),QString::SkipEmptyParts);
            QStringList vals = lines.at(1).split(QRegExp("\\s"),QString::SkipEmptyParts);
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
    SweepDialog *swpdlg = new SweepDialog(Sch,&NodeVals);
    delete swpdlg;

    Sch->showBias = 1;
}

/*!
 * \brief AbstractSpiceKernel::parseSTEPOutput This method parses text raw spice
 *        output from Parameter sweep analysis. Can parse data that uses appedwrite.
 *        Extracts a simulation points array and variables names and types (Real
 *        or Complex) from output.
 * \param ngspice_file Spice output file name
 * \param sim_points 2D array in which simulation points should be extracted. All simulation
 *        points from all sweep variable steps are extracted in a single array
 * \param var_list This list is filled by simualtion variables. There is a list of dependent
 *        and independent varibales. An independent variable is the first in list.
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
        QRegExp sep("[ \t,]");
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
    QRegExp sep("[ \t,]");
    QList<double> sim_point;
    bool ok = false;
    QRegExp dataline_patter("^ *[0-9]+[ \t]+.*");
    if (!dataline_patter.exactMatch(lin)) return false;
    double indep_val = lin.section(sep,1,1,QString::SectionSkipEmpty).toDouble(&ok);
    //double indep_val = lin.split(sep,QString::SkipEmptyParts).at(1).toDouble(&ok); // only real indep vars
    if (!ok) return false;
    sim_point.append(indep_val);
    for (int i=0;i<NumVars;i++) {
        if (isComplex) {
            QStringList lst = ngsp_data.readLine().split(sep,QString::SkipEmptyParts);
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

void AbstractSpiceKernel::parseHBSTEPOutput(QString , QList<QList<double> >&,
                                            QStringList&, bool&)
{

}

/*!
 * \brief AbstractSpiceKernel::parseXYCESTDOutput
 * \param std_file[in] XYCE STD output file name
 * \param sim_points[out] 2D array in which simulation points should be extracted
 * \param var_list[out] This list is filled by simualtion variables. There is a list of dependent
 *        and independent varibales. An independent variable is the first in list.
 * \param isComplex[out] Type of variables. True if complex. False if real.
 */
void AbstractSpiceKernel::parseXYCESTDOutput(QString std_file, QList<QList<double> > &sim_points,
                                             QStringList &var_list, bool &isComplex)
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
    while (!ngsp_data.atEnd()) { // Parse header;
        QString lin = ngsp_data.readLine();
        if (lin.isEmpty()) continue;
        if (lin.startsWith("End of ")) continue;
        if (lin.startsWith("Index ",Qt::CaseInsensitive)) {
            var_list = lin.split(" ",QString::SkipEmptyParts);
            var_list.removeFirst(); // Drop Index
            continue;
        } else {
            QStringList val_lst = lin.split(" ",QString::SkipEmptyParts);
            QList<double> sim_point;
            foreach (QString val, val_lst) {
                sim_point.append(val.toDouble());
            }
            sim_point.removeFirst(); // Index
            sim_points.append(sim_point);
        }
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
        while (!swp_data.atEnd()) {
            QRegExp point_pattern("^\\s*[0-9]+ .*");
            QRegExp var_pattern("^STEP\\s+.*");
            QRegExp sep("\\s");
            QString lin = swp_data.readLine();
            if (var_pattern.exactMatch(lin)) {
                var = lin.split(sep,QString::SkipEmptyParts).last();
            }
            if (point_pattern.exactMatch(lin)) {
                values.append(lin.split(sep,QString::SkipEmptyParts).last());
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
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        while (!ngsp_data.atEnd()) {
            QString lin = ngsp_data.readLine();
            if (lin.startsWith("Plotname: ")) {
                plots_cnt++;
                values.append(QString::number(plots_cnt));
            }
        }
        ofile.close();
    }
    if (plots_cnt>1) return spiceRawSwp;
    else if (plots_cnt == 0)return xyceSTD;
    else return spiceRaw;
}

/*!
 * \brief AbstractSpiceKernel::convertToQucsData Put data extracted from spice raw
 *        text output files (given in outputs_files property) into single XML
 *        Qucs Dataset.
 * \param qucs_dataset A file name of Qucs Dataset to create
 * \param xyce True if Xyce simulator was used.
 */
void AbstractSpiceKernel::convertToQucsData(const QString &qucs_dataset, bool xyce)
{
    if (DC_OP_only) { // Don't touch existing datasets when only DC was simulated
        // It's need to show DC bias on schematic only
        foreach(QString output,output_files) {
            QString full_outfile = workdir+QDir::separator()+output;
            if (output.endsWith(".dc_op")) parseDC_OPoutput(full_outfile);
            else if (output.endsWith(".dc_op_xyce")) parseDC_OPoutputXY(full_outfile);
        }
        return;
    }

    // Merge all outputs in a single Qucs dataset otherwise
    QString ds_str;
    QTextStream ds_stream(&ds_str);

    ds_stream<<"<Qucs Dataset " PACKAGE_VERSION ">\n";

    QString sim,indep;
    QStringList indep_vars;

    QString swp_var,swp_var2;
    QStringList swp_var_val,swp_var2_val;
    swp_var.clear();
    swp_var2.clear();
    swp_var_val.clear();
    swp_var2_val.clear();

    QList< QList<double> > sim_points;
    QStringList var_list;
    bool isComplex = false;
    bool hasParSweep = false;
    bool hasDblParSweep = false;

    QString ngspice_output_filename;
    foreach(ngspice_output_filename,output_files) { // For every simulation convert results to Qucs dataset
        QString full_outfile = workdir+QDir::separator()+ngspice_output_filename;
        if (ngspice_output_filename.endsWith("HB.FD.prn")) {
            parseHBOutput(full_outfile,sim_points,var_list);
            isComplex = true;
        } else if (ngspice_output_filename.endsWith(".four")) {
            isComplex=false;
            parseFourierOutput(full_outfile,sim_points,var_list,xyce);
        } else if (ngspice_output_filename.endsWith(".txt_std")) {
            parseXYCESTDOutput(full_outfile,sim_points,var_list,isComplex);
        } else if (ngspice_output_filename.endsWith(".noise_log")) {
            isComplex = false;
            parseXYCENoiseLog(full_outfile,sim_points,var_list);
        } else if (ngspice_output_filename.endsWith(".noise")) {
            isComplex = false;
            parseNoiseOutput(full_outfile,sim_points,var_list,hasParSweep);
            if (hasParSweep) {
                QString res_file = QDir::convertSeparators(workdir + QDir::separator()
                                                        + "spice4qucs.noise.cir.res");
                parseResFile(res_file,swp_var,swp_var_val);
            }
        } else if (ngspice_output_filename.endsWith(".pz")) {
            isComplex = true;
            parsePZOutput(full_outfile,sim_points,var_list,hasParSweep);
            if (hasParSweep) {
                QString res_file = QDir::convertSeparators(workdir + QDir::separator()
                                                        + "spice4qucs.pz.cir.res");
                parseResFile(res_file,swp_var,swp_var_val);
            }
        } else if (ngspice_output_filename.endsWith("_swp.txt")) {
            hasParSweep = true;
            QString simstr = full_outfile;
            simstr.remove("_swp.txt");
            if (ngspice_output_filename.endsWith("_swp_swp.txt")) { // 2-var parameter sweep
                hasDblParSweep = true;
                simstr.chop(4);
                simstr = simstr.split('_').last();
                QString res2_file = QDir::convertSeparators(workdir + QDir::separator()
                                                            + "spice4qucs." + simstr + ".cir.res1");
                parseResFile(res2_file,swp_var2,swp_var2_val);
            } else {
                simstr = simstr.split('_').last();
            }

            QString res_file = QDir::convertSeparators(workdir + QDir::separator()
                                                    + "spice4qucs." + simstr + ".cir.res");
            parseResFile(res_file,swp_var,swp_var_val);

            if (ngspice_output_filename.endsWith("_hb_swp.txt")) {
                parseHBSTEPOutput(full_outfile,sim_points,var_list,isComplex);
            } else {
                parseSTEPOutput(full_outfile,sim_points,var_list,isComplex);
            }


        } else {
            int OutType = checkRawOutupt(full_outfile,swp_var_val);
            switch (OutType) {
            case spiceRawSwp:
                hasParSweep = true;
                swp_var = "Number";
                parseSTEPOutput(full_outfile,sim_points,var_list,isComplex);
                break;
            case spiceRaw:
                parseNgSpiceSimOutput(full_outfile,sim_points,var_list,isComplex);
                break;
            case xyceSTD:
                parseXYCESTDOutput(full_outfile,sim_points,var_list,isComplex);
                break;
            default: break;
            }
        }
        if (var_list.isEmpty()) continue; // notning to convert
        normalizeVarsNames(var_list);

        QString indep = var_list.first();
        QList<double> sim_point;


        if (hasParSweep) {
            int indep_cnt;
            if (hasDblParSweep) indep_cnt =  sim_points.count()/(swp_var_val.count()*swp_var2_val.count());
            else indep_cnt = sim_points.count()/swp_var_val.count();
            if (!indep.isEmpty()) {
                ds_stream<<QString("<indep %1 %2>\n").arg(indep).arg(indep_cnt); // output indep var: TODO: parameter sweep
                for (int i=0;i<indep_cnt;i++) {
                    ds_stream<<QString::number(sim_points.at(i).at(0),'e',12)<<endl;
                }
                ds_stream<<"</indep>\n";
            }

            ds_stream<<QString("<indep %1 %2>\n").arg(swp_var).arg(swp_var_val.count());
            foreach (QString val,swp_var_val) {
                ds_stream<<val<<endl;
            }
            ds_stream<<"</indep>\n";
            if (indep.isEmpty()) indep = swp_var;
            else indep += " " + swp_var;
            if (hasDblParSweep) {
                ds_stream<<QString("<indep %1 %2>\n").arg(swp_var2).arg(swp_var2_val.count());
                foreach (QString val,swp_var2_val) {
                    ds_stream<<val<<endl;
                }
                ds_stream<<"</indep>\n";
                indep += " " + swp_var2;
            }
        } else if (!indep.isEmpty()) {
            ds_stream<<QString("<indep %1 %2>\n").arg(indep).arg(sim_points.count()); // output indep var: TODO: parameter sweep
            foreach (sim_point,sim_points) {
                ds_stream<<QString::number(sim_point.at(0),'e',12)<<endl;
            }
            ds_stream<<"</indep>\n";
        }

        for(int i=1;i<var_list.count();i++) { // output dep var
            if (indep.isEmpty()) ds_stream<<QString("<indep %1 %2>\n").arg(var_list.at(i)).arg(sim_points.count());
            else ds_stream<<QString("<dep %1 %2>\n").arg(var_list.at(i)).arg(indep);
            foreach (sim_point,sim_points) {
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
                    ds_stream<<QString::number(sim_point.at(i),'e',12)<<endl;
                }
            }
            if (indep.isEmpty()) ds_stream<<"</indep>\n";
            else ds_stream<<"</dep>\n";
        }
        hasParSweep = false;
    }

    QFile dataset(qucs_dataset);
    if (dataset.open(QFile::WriteOnly)) {
        QTextStream ts(&dataset);
        ts<<ds_str;
        dataset.close();
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
    foreach(QString output_filename,output_files) {
        QString full_outfile = workdir+QDir::separator()+output_filename;
        QFile::remove(full_outfile);
    }
}

/*!
 * \brief AbstractSpiceKernel::normalizeVarsNames Convert spice-style variable names to
 *        Qucs style and add simualation type prefix (i.e. AC, TRAN, DC). Conversion
 *        for harmonic balance variable and current probes variables are supported.
 * \param var_list This list contains variable names that need normalization.
 */
void AbstractSpiceKernel::normalizeVarsNames(QStringList &var_list)
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

    QStringList::iterator it=var_list.begin();

    for (it++;it!=var_list.end();it++) {
        if ((!(it->startsWith(prefix)||it->startsWith(iprefix)))||(HB)) {
            if (HB) {
                int idx = it->indexOf('(');  // Add .Vb suffix for correct display
                int cnt = it->count();
                *it = it->right(cnt-idx-1); // Only node.Vb is displayed correctly
                it->remove(')');
                *it += ".Vb";
            } else {
                *it = prefix + *it;
            }
        }
        QStringList lst = it->split('(');
        if (lst.count()>1) {
            QRegExp ivprobe_pattern("^[Vv][Pp][Rr][0-9]+.*");
            QRegExp ivprobe_pattern_ngspice("^(ac\\.|tran\\.)[Vv][Pp][Rr][0-9]+.*");
            if (ivprobe_pattern.exactMatch(lst.at(1))) {
                lst[1].remove(0,1);
                *it = lst.join("(");
            } else if (ivprobe_pattern_ngspice.exactMatch(lst.at(1))) {
                lst[1].replace(".v",".",Qt::CaseInsensitive);
                *it = lst.join("(");
            }
        }
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
    //output.clear();
    output += SimProcess->readAllStandardOutput();
    emit finished();
    emit progress(100);
}

/*!
 * \brief AbstractSpiceKernel::slotProcessOutput Process SimProcess output and report progress
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
    return output;
}

/*!
 * \brief AbstractSpiceKernel::setSimulatorCmd Set simualtor executable location
 * \param cmd Simulator executable absolute path. For example /usr/bin/ngspice
 */
void AbstractSpiceKernel::setSimulatorCmd(QString cmd)
{
    simulator_cmd = cmd;
}

/*!
 * \brief AbstractSpiceKernel::setWorkdir Set simulator working directory path
 *        to store netlist and temp data.
 * \param path[in] New working directory path
 */
void AbstractSpiceKernel::setWorkdir(QString path)
{
    workdir = path;
    QFileInfo inf(workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(workdir);
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
    return SimProcess->waitForFinished(10000);
}

