/***************************************************************************
                           abstractspicekernel.cpp
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

#include "externsimdialog.h"
#include "simsettingsdialog.h"
#include "main.h"

ExternSimDialog::ExternSimDialog(Schematic *sch, QWidget *parent, bool netlist_mode) :
    QDialog(parent)
{
    Sch = sch;
    wasSimulated = false;

    workdir = QucsSettings.S4Qworkdir;
    QFileInfo inf(workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(workdir);
    }

    ngspice = new Ngspice(sch,this);
    xyce = new Xyce(sch,this);


    buttonSimulate = new QPushButton(tr("Simulate"),this);
    connect(buttonSimulate,SIGNAL(clicked()),this,SLOT(slotStart()));

    buttonStopSim = new QPushButton(tr("Stop"),this);
    connect(buttonStopSim,SIGNAL(clicked()),ngspice,SLOT(killThemAll()));
    connect(buttonStopSim,SIGNAL(clicked()),xyce,SLOT(killThemAll()));
    buttonStopSim->setEnabled(false);

    buttonSaveNetlist = new QPushButton(tr("Save netlist"),this);
    connect(buttonSaveNetlist,SIGNAL(clicked()),this,SLOT(slotSaveNetlist()));

    buttonExit = new QPushButton(tr("Exit"),this);
    connect(buttonExit,SIGNAL(clicked()),this,SLOT(reject()));
    connect(buttonExit,SIGNAL(clicked()),ngspice,SLOT(killThemAll()));
    connect(buttonExit,SIGNAL(clicked()),xyce,SLOT(killThemAll()));

    QGroupBox *grp_1 = new QGroupBox(tr("Simulation console"),this);
    QVBoxLayout *vbl1 = new QVBoxLayout;

    editSimConsole = new QPlainTextEdit(this);
    QFont font;
    font.setFamily("monospace");
    font.setPointSize(10);
    editSimConsole->setFont(font);
    editSimConsole->setReadOnly(true);
    vbl1->addWidget(editSimConsole);
    grp_1->setLayout(vbl1);
    ngspice->setConsole(editSimConsole);
    xyce->setConsole(editSimConsole);

    simProgress = new QProgressBar(this);
    connect(ngspice,SIGNAL(progress(int)),simProgress,SLOT(setValue(int)));
    connect(xyce,SIGNAL(progress(int)),simProgress,SLOT(setValue(int)));

    QVBoxLayout *vl_top = new QVBoxLayout;
    vl_top->addWidget(grp_1);
    vl_top->addWidget(simProgress);
    QHBoxLayout *hl1 = new QHBoxLayout;
    hl1->addWidget(buttonSimulate);
    hl1->addWidget(buttonStopSim);
    hl1->addWidget(buttonSaveNetlist);
    hl1->addWidget(buttonExit);
    vl_top->addLayout(hl1);
    this->setLayout(vl_top);
    this->setWindowTitle(tr("Simulate with external simulator"));

    slotSetSimulator();
    if (!netlist_mode)
        buttonSimulate->click(); // Start simulation

}

ExternSimDialog::~ExternSimDialog()
{
    ngspice->killThemAll();
}

void ExternSimDialog::slotSetSimulator()
{
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice: {
        xyce->setParallel(false);
        connect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessOutput()));
        connect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()));
        QString cmd;
        if (QFileInfo(QucsSettings.NgspiceExecutable).isRelative()) { // this check is related to MacOS
            cmd = QFileInfo(QucsSettings.BinDir + QucsSettings.NgspiceExecutable).absoluteFilePath();
        } else {
            cmd = QFileInfo(QucsSettings.NgspiceExecutable).absoluteFilePath();
        }
        if (QFileInfo::exists(cmd)) {
            ngspice->setSimulatorCmd(cmd);
        } else {
            ngspice->setSimulatorCmd(QucsSettings.NgspiceExecutable); //rely on $PATH
        }
        ngspice->setSimulatorParameters(QucsSettings.SimParameters);
    }
        break;
    case spicecompat::simXyceSer: {
        xyce->setParallel(false);
        connect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(xyce,SIGNAL(finished()),this,SLOT(slotProcessOutput()));
        connect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(buttonSimulate,SIGNAL(clicked()),xyce,SLOT(slotSimulate()));
        xyce->setSimulatorParameters(QucsSettings.SimParameters);
    }
        break;
    case spicecompat::simXycePar: {
#ifdef Q_OS_UNIX
        xyce->setParallel(true);
#else
        xyce->setParallel(false);
#endif
        connect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(xyce,SIGNAL(finished()),this,SLOT(slotProcessOutput()));
        connect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(buttonSimulate,SIGNAL(clicked()),xyce,SLOT(slotSimulate()));
        xyce->setSimulatorParameters(QucsSettings.SimParameters);
    }
        break;
    case spicecompat::simSpiceOpus: {
        xyce->setParallel(false);
        connect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()),Qt::UniqueConnection);
        connect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessOutput()),Qt::UniqueConnection);
        connect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)),Qt::UniqueConnection);
        connect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()),Qt::UniqueConnection);
        ngspice->setSimulatorCmd(QucsSettings.SpiceOpusExecutable);
        ngspice->setSimulatorParameters(QucsSettings.SimParameters);
    }
        break;
    default: break;
    }
}


void ExternSimDialog::slotProcessOutput()
{
    buttonSaveNetlist->setEnabled(true);
    buttonStopSim->setEnabled(false);
    QString out;

    // Set temporary safe output name

    QString ext;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice:
        ext = ".dat.ngspice";
        out = ngspice->getOutput();
        break;
    case spicecompat::simXycePar:
    case spicecompat::simXyceSer:
        ext = ".dat.xyce";
        out = xyce->getOutput();
        break;
    case spicecompat::simSpiceOpus:
        out = ngspice->getOutput();
        ext = ".dat.spopus";
        break;
    default:
        out = "dummy";
        ext = ".dat";
        break;
    }

    if (out.contains("warning",Qt::CaseInsensitive)||
        out.contains("error",Qt::CaseInsensitive)) {
        emit warnings();
    } else emit success();
    //editSimConsole->clear();
    /*editSimConsole->insertPlainText(out);
    editSimConsole->moveCursor(QTextCursor::End);*/
    saveLog();
    editSimConsole->insertPlainText("Simulation finished\n");

    QFileInfo inf(Sch->DocName);
    //QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_ngspice.dat";
    QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+ext;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice:
    case spicecompat::simSpiceOpus:
        ngspice->convertToQucsData(qucs_dataset);
        break;
    case spicecompat::simXycePar:
    case spicecompat::simXyceSer:
        xyce->convertToQucsData(qucs_dataset);
        break;
    default:break;
    }
    emit simulated();
    wasSimulated = true;
    if (Sch->showBias>0) this->close();
}


void ExternSimDialog::slotNgspiceStarted()
{
    editSimConsole->clear();
    QString sim;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice: sim = "Ngspice";
        break;
    case spicecompat::simXyceSer: sim = "Xyce (serial) ";
        break;
    case spicecompat::simXycePar: sim = "Xyce (parallel) ";
        break;
    default: sim = "Simulator "; // Some other simulators could be added ...
        break;
    }
    editSimConsole->insertPlainText(sim + tr(" started...\n"));
}

void ExternSimDialog::slotNgspiceStartError(QProcess::ProcessError err)
{
    QString msg;
    switch (err) {
    case QProcess::FailedToStart : msg = tr("Failed to start simulator!");
        break;
    case QProcess::Crashed : msg = tr("Simulator crashed!");
        break;
    default : msg = tr("Simulator error!");
    }

    QMessageBox::critical(this,tr("Simulate with SPICE"),msg,QMessageBox::Ok);

    QString sim;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice: sim = "Ngspice";
        break;
    case spicecompat::simXyceSer: sim = "Xyce (serial) ";
        break;
    case spicecompat::simXycePar: sim = "Xyce (parallel) ";
        break;
    default: sim = "Simulator "; // Some other simulators could be added ...
        break;
    }
    editSimConsole->insertPlainText(sim + tr(" error..."));
}

void ExternSimDialog::slotStart()
{
    buttonStopSim->setEnabled(true);
    buttonSaveNetlist->setEnabled(false);
}

void ExternSimDialog::slotStop()
{
    buttonStopSim->setEnabled(false);
    buttonSaveNetlist->setEnabled(true);
    ngspice->killThemAll();
}

void ExternSimDialog::slotSaveNetlist()
{
    QFileInfo inf(Sch->DocName);
    QString filename = QFileDialog::getSaveFileName(this,tr("Save netlist"),inf.path()+QDir::separator()+"netlist.cir",
                       "All files (*)");
    if (filename.isEmpty()) return;

    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice:
    case spicecompat::simSpiceOpus: {
        ngspice->SaveNetlist(filename);
        }
        break;
    case spicecompat::simXyceSer:
    case spicecompat::simXycePar: {
        xyce->SaveNetlist(filename);
        }
        break;
    default: break;
    }

    if (!QFile::exists(filename)) {
      QMessageBox::critical(0, QObject::tr("Save netlist"),
          QObject::tr("Disk write error!"), QMessageBox::Ok);
    }
}

void ExternSimDialog::saveLog()
{
    QString filename = QucsSettings.QucsHomeDir.filePath("log.txt");
    QFile log(filename);
    if (log.open(QIODevice::WriteOnly)) {
        QTextStream ts_log(&log);
        ts_log<<editSimConsole->toPlainText();
        log.flush();
        log.close();
    }
}


