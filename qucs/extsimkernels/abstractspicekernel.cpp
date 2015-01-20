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



AbstractSpiceKernel::AbstractSpiceKernel(Schematic *sch_, QObject *parent) :
    QObject(parent)
{
    Sch = sch_;

    workdir = QDir::convertSeparators(QDir::homePath()+"/.qucs/spice4qucs");
    QFileInfo inf(workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(workdir);
    }

    SimProcess = new QProcess(this);
    SimProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(SimProcess,SIGNAL(finished(int)),this,SLOT(slotFinished()));
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


void AbstractSpiceKernel::createNetlist(QTextStream& stream, int NumPorts,QStringList& simulations,
                                        QStringList& vars, QStringList &outputs)
{

}

void AbstractSpiceKernel::slotSimulate()
{

}




void AbstractSpiceKernel::parseNgSpiceSimOutput(QString ngspice_file,QList< QList<double> > &sim_points,QStringList &var_list, bool &isComplex)
{
    isComplex = false;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        sim_points.clear();
        bool start_values_sec = false;
        int NumVars=0; // Number of dep. and indep.variables
        while (!ngsp_data.atEnd()) {
            QRegExp sep("[ \t,]");
            QString lin = ngsp_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.contains("Flags")&&lin.contains("complex")) { // output consists of
                isComplex = true; // complex numbers
                continue;         // maybe ac_analysis
            }
            if (lin.contains("No. Variables")) {  // get number of variables
                NumVars=lin.split(sep,QString::SkipEmptyParts).at(2).toInt();
                qDebug()<<NumVars;
                continue;
            }
            if (lin=="Variables:") {
                var_list.clear();
                QString indep_var = ngsp_data.readLine().split(sep,QString::SkipEmptyParts).at(1); // Achtung!!! Vorsicht!!!
                var_list.append(indep_var);

                for (int i=1;i<NumVars;i++) {
                    lin = ngsp_data.readLine();
                    QString dep_var = lin.split(sep,QString::SkipEmptyParts).at(1);
                    qDebug()<<dep_var;
                    var_list.append(dep_var);
                }
                qDebug()<<var_list;
                continue;
            }
            if (lin=="Values:") {
                start_values_sec = true;
                continue;
            }
            if (start_values_sec) {
                QList<double> sim_point;
                double indep_val = lin.split(sep,QString::SkipEmptyParts).at(1).toDouble(); // only real indep vars
                sim_point.append(indep_val);
                for (int i=0;i<NumVars;i++) {
                    if (isComplex) {
                        QStringList lst = ngsp_data.readLine().split(sep,QString::SkipEmptyParts);
                        qDebug()<<lst;
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
            }
        }
        ofile.close();
    }
}

void AbstractSpiceKernel::parseHBOutput(QString ngspice_file,
                                        QList<QList<double> > &sim_points, QStringList &var_list)
{
    var_list.clear();
    sim_points.clear();
    int NumVars=0;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream hb_data(&ofile);
        var_list.append("hbfrequency");
        bool first_head = true;
        while (!hb_data.atEnd()) {
            QRegExp sep("[ \t,]");
            QRegExp heading("^[a-zA-Z_][a-zA-Z0-9]{1,},");
            QRegExp dataline("^[0-9]{1,},");
            QString lin = hb_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.startsWith("Index")) { // CSV heading
                if (first_head) {
                    QStringList vars1 = lin.split(" ",QString::SkipEmptyParts);
                    vars1.removeFirst();
                    vars1.removeFirst();
                    var_list.append(vars1);
                    NumVars = var_list.count();
                    qDebug()<<vars1;
                }
                first_head = false;
            }
            if ((lin.contains(QRegExp("\\d*\\.\\d+[+-]*[eE]*[\\d]*")))&&(!first_head)) { // CSV dataline
                QStringList vals = lin.split(" ",QString::SkipEmptyParts);
                QList <double> sim_point;
                sim_point.clear();
                for (int i=NumVars+1;i<vals.count();i++) {
                    sim_point.append(vals.at(i).toDouble());
                }
                sim_points.append(sim_point);
                qDebug()<<sim_point;
            }
        }
        ofile.close();
    }
}

void AbstractSpiceKernel::parseSTEPOutput(QString ngspice_file,
                     QList< QList<double> > &sim_points,
                     QStringList &var_list, bool &isComplex)
{

}

void AbstractSpiceKernel::convertToQucsData(const QString &qucs_dataset)
{
    QFile dataset(qucs_dataset);
    if (dataset.open(QFile::WriteOnly)) {
        QTextStream ds_stream(&dataset);

        ds_stream<<"<Qucs Dataset " PACKAGE_VERSION ">\n";

        QString sim,indep;
        QStringList indep_vars;

        QList< QList<double> > sim_points;
        QStringList var_list;
        bool isComplex = false;

        QString ngspice_output_filename;
        foreach(ngspice_output_filename,output_files) { // For every simulation convert results to Qucs dataset
            if (ngspice_output_filename.endsWith("_hb.txt")) {
                parseHBOutput(workdir+QDir::separator()+ngspice_output_filename,sim_points,var_list);
                isComplex = true;
            } else {
                parseNgSpiceSimOutput(workdir+QDir::separator()+ngspice_output_filename,sim_points,var_list,isComplex);
            }
            if (var_list.isEmpty()) continue; // notning to convert
            normalizeVarsNames(var_list);
            QString indep = var_list.first();
            ds_stream<<QString("<indep %1 %2>\n").arg(indep).arg(sim_points.count()); // output indep var: TODO: parameter sweep
            QList<double> sim_point;
            foreach (sim_point,sim_points) {
                ds_stream<<QString::number(sim_point.at(0),'e',12)<<endl;
            }
            ds_stream<<"</indep>\n";
            for(int i=1;i<var_list.count();i++) { // output dep var
                ds_stream<<QString("<dep %1 %2>\n").arg(var_list.at(i)).arg(indep);
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
                ds_stream<<"</dep>\n";
            }

        }

        dataset.close();
    }
}

// Normalize Ngspice and Xyce variables naming
void AbstractSpiceKernel::normalizeVarsNames(QStringList &var_list)
{
    QString prefix="";
    QString indep = var_list.first();
    bool HB = false;
    qDebug()<<"norm:"<<indep;
    indep = indep.toLower();
    if (indep=="time") {
        prefix = "tran.";
    } else if (indep=="frequency") {
        prefix = "ac.";
    } else if (indep=="hbfrequency") {
        HB = true;
    }

    if (var_list.count()>1) {
        for (int i=1;i<var_list.count();i++) {
            if ((!var_list[i].startsWith(prefix))||(HB)) {
                if (HB) {
                    int idx = var_list[i].indexOf('(');  // Add .Vb suffix for correct display
                    int cnt = var_list[i].count();
                    var_list[i] = var_list[i].right(cnt-idx-1); // Only node.Vb is displayed correctly
                    var_list[i].remove(')');
                    var_list[i] += ".Vb";
                } else {
                    var_list[i] = prefix + var_list[i];
                }
            }
        }
    }
}

void AbstractSpiceKernel::slotErrors(QProcess::ProcessError err)
{
    emit errors(err);
}

void AbstractSpiceKernel::slotFinished()
{
    output.clear();
    output = SimProcess->readAllStandardOutput();
    emit finished();
}

QString AbstractSpiceKernel::getOutput()
{
    return output;
}
