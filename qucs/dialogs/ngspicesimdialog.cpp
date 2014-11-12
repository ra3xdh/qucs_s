/***************************************************************************
                           ngspicesimdialog.cpp
                             ----------------
    begin                : Sun Nov 9 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
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

#include "ngspicesimdialog.h"

NgspiceSimDialog::NgspiceSimDialog(Schematic *sch,QWidget *parent) :
    QDialog(parent)
{
    Sch = sch;

    workdir = QDir::convertSeparators(QDir::homePath()+"/.qucs/spice4qucs");
    QFileInfo inf(workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(workdir);
    }

    ngspice = new QProcess(this);
    ngspice->setProcessChannelMode(QProcess::MergedChannels);
    connect(ngspice,SIGNAL(finished(int)),this,SLOT(slotProcessNgSpiceOutput(int)));
    connect(this,SIGNAL(destroyed()),this,SLOT(killThemAll()));
    connect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
    connect(ngspice,SIGNAL(error(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));

    buttonSimulate = new QPushButton(tr("Simulate"),this);
    connect(buttonSimulate,SIGNAL(clicked()),this,SLOT(slotSimulate()));
    buttonStopSim = new QPushButton(tr("Stop"),this);
    connect(buttonStopSim,SIGNAL(clicked()),this,SLOT(reject()));
    connect(buttonStopSim,SIGNAL(clicked()),this,SLOT(killThemAll()));
    buttonStopSim->setEnabled(false);

    QGroupBox *grp1 = new QGroupBox(tr("Simulation console"),this);
    QVBoxLayout *vbl1 = new QVBoxLayout;
    editSimConsole = new QTextEdit(this);
    editSimConsole->setReadOnly(true);
    vbl1->addWidget(editSimConsole);
    grp1->setLayout(vbl1);

    QVBoxLayout *vl_top = new QVBoxLayout;
    vl_top->addWidget(grp1);
    QHBoxLayout *hl1 = new QHBoxLayout;
    hl1->addWidget(buttonSimulate);
    hl1->addWidget(buttonStopSim);
    vl_top->addLayout(hl1);
    this->setLayout(vl_top);
}

NgspiceSimDialog::~NgspiceSimDialog()
{
    killThemAll();
}

void NgspiceSimDialog::slotSimulate()
{
    int num=0;
    sims.clear();
    vars.clear();
    QString tmp_path = QDir::convertSeparators(workdir+"/spice4qucs.cir");
    QFile spice_file(tmp_path);
    if (spice_file.open(QFile::WriteOnly)) {
        QTextStream stream(&spice_file);
        createSpiceNetlist(stream,num,sims,vars,output_files);
        spice_file.close();
    }
    qDebug()<<sims<<vars;

    startNgSpice(tmp_path);
    /*QString old_dir = QDir::currentPath(); // Execute ngspice
    QDir::setCurrent(QDir::homePath()+"/.qucs");
    QProcess::execute("ngspice "+tmp_path);
    QDir::setCurrent(old_dir);*/
}

void NgspiceSimDialog::startNgSpice(QString netlist)
{
    buttonStopSim->setEnabled(true);
    ngspice->setWorkingDirectory(workdir);
    ngspice->start("ngspice "+ netlist);
}

void NgspiceSimDialog::killThemAll()
{
    buttonStopSim->setEnabled(false);
    if (ngspice->state()!=QProcess::NotRunning) {
        ngspice->kill();
    }
}

void NgspiceSimDialog::slotProcessNgSpiceOutput(int exitCode)
{
    buttonStopSim->setEnabled(false);
    QString out = ngspice->readAllStandardOutput();
    //editSimConsole->clear();
    editSimConsole->append(out);
    // Set temporary safe output name
    QFileInfo inf(Sch->DocName);
    QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_ngspice.dat";
    convertToQucsData(qucs_dataset);
}

void NgspiceSimDialog::slotNgspiceStarted()
{
    editSimConsole->clear();
    editSimConsole->append(tr("ngspice started...\n"));
}

void NgspiceSimDialog::slotNgspiceStartError()
{
    editSimConsole->append(tr("ngspice error..."));
}

// Reads ngspice simulation results and merges it in single Qucs dataset
void NgspiceSimDialog::convertToQucsData(const QString &qucs_dataset)
{
    QFile dataset(qucs_dataset);
    if (dataset.open(QFile::WriteOnly)) {
        QTextStream ds_stream(&dataset);

        ds_stream<<"<Qucs Dataset "PACKAGE_VERSION">\n";

        QString sim,indep;
        QStringList indep_vars;

        QList< QList<double> > sim_points;
        QStringList var_list;

        QString ngspice_output_filename;
        foreach(ngspice_output_filename,output_files) { // For every simulation convert results to Qucs dataset
            parseNgSpiceSimOutput(workdir+QDir::separator()+ngspice_output_filename,sim_points,var_list);
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
                    ds_stream<<QString::number(sim_point.at(i),'e',12)<<endl;
                }
                ds_stream<<"</dep>\n";
            }

        }

        /*foreach(sim,sims) { // extract independent vars from simulations list
            if (sim=="tran") {
                indep_vars<<"time";
            } else if (sim=="ac") {
                indep_vars<<"frequency";
            }
            ds_stream<<QString("<indep %1>\n").arg(indep_vars.last());
            QList<double> sim_point;
            foreach (sim_point,sim_points) {
                ds_stream<<QString::number(sim_point.at(0),'e',12)<<endl;
            }

            ds_stream<<"</indep>\n";
        }*/

        /*foreach (indep,indep_vars) { // form dependent vars
            QString var;
            foreach (var,vars) {
                ds_stream<<QString("<dep %1.Vt %2>\n").arg(var).arg(indep);
                ds_stream<<"</dep>\n";
            }
        }*/

        dataset.close();
    }
}


void NgspiceSimDialog::createSpiceNetlist(QTextStream& stream, int NumPorts,QStringList& simulations, QStringList& vars,
                                      QStringList &outputs)
{
    if(!Sch->prepareSpiceNetlist(stream)) return; // Unable to perform ngspice simulation

    QString s;
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
      if(Sch->isAnalog &&
         !(pc->isSimulation) &&
         !(pc->isProbe)) {
        s = pc->getSpiceNetlist();
        stream<<s;
      }
    }

    // determine which simulations are in use
    simulations.clear();
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
       if(pc->isSimulation) {
           s = pc->getSpiceNetlist();
           QString sim_typ = pc->Model;
           if (sim_typ==".AC") simulations.append("ac");
           if (sim_typ==".TR") simulations.append("tran");
           if (sim_typ==".DC") simulations.append("dc");
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
    vars.sort();
    qDebug()<<vars;

    stream<<".control\n"          //execute simulations
          <<"set filetype=ascii\n"
          <<"run\n";

    QFileInfo inf(Sch->DocName);
    QString basenam = inf.baseName();

    QString sim;                 // see results
    outputs.clear();
    foreach (sim,simulations) {
        QString nod;
        QString nods;
        nods.clear();
        foreach (nod,vars) {
            nods += QString("%1.v(%2) ").arg(sim).arg(nod);
        }
        QString filename = QString("%1_%2.txt").arg(basenam).arg(sim);
        QString write_str = QString("write %1 %2\n").arg(filename).arg(nods);
        stream<<write_str;
        outputs.append(filename);
        stream<<endl;
    }

    stream<<"exit\n"
          <<".endc\n";

    stream<<".END\n";

}

void NgspiceSimDialog::parseNgSpiceSimOutput(QString ngspice_file,QList< QList<double> > &sim_points,QStringList &var_list)
{

    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        bool start_values_sec = false;
        int NumVars=0; // Number of dep. and indep.variables
        while (!ngsp_data.atEnd()) {
            QRegExp sep("[ \t]");
            QString lin = ngsp_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.contains("No. Variables")) {
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
                double indep_val = lin.split(sep,QString::SkipEmptyParts).at(1).toDouble();
                sim_point.append(indep_val);
                for (int i=0;i<vars.count();i++) {
                    double dep_val = ngsp_data.readLine().remove(sep).toDouble();
                    sim_point.append(dep_val);
                }
                sim_points.append(sim_point);
            }
        }
        ofile.close();
    }
}
