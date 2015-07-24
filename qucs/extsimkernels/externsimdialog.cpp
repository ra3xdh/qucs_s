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

ExternSimDialog::ExternSimDialog(Schematic *sch,QWidget *parent) :
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

    buttonSimSettings = new QPushButton(tr("Settings"),this);
    connect(buttonSimSettings,SIGNAL(clicked()),this,SLOT(slotSimSettings()));

    buttonExit = new QPushButton(tr("Exit"),this);
    connect(buttonExit,SIGNAL(clicked()),this,SLOT(reject()));
    connect(buttonExit,SIGNAL(clicked()),ngspice,SLOT(killThemAll()));
    connect(buttonExit,SIGNAL(clicked()),xyce,SLOT(killThemAll()));

    lblSimulator = new QLabel(tr("Select external simulator:"));
    QGroupBox *grp1 = new QGroupBox(tr("Simulation console"),this);
    QVBoxLayout *vbl1 = new QVBoxLayout;

    cbxSimualor = new QComboBox(this);
    QStringList items;
    items<<"Ngspice"<<"Xyce (Serial)"<<"Xyce (Parallel)";
    cbxSimualor->addItems(items);
    connect(cbxSimualor,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSetSimulator()));

    editSimConsole = new QPlainTextEdit(this);
    QFont font;
    font.setFamily("monospace");
    font.setPointSize(10);
    editSimConsole->setFont(font);
    editSimConsole->setReadOnly(true);
    vbl1->addWidget(editSimConsole);
    grp1->setLayout(vbl1);

    simProgress = new QProgressBar(this);
    connect(ngspice,SIGNAL(progress(int)),simProgress,SLOT(setValue(int)));
    connect(xyce,SIGNAL(progress(int)),simProgress,SLOT(setValue(int)));

    QLabel *lbl_warn = new QLabel(this);
    lbl_warn->setAutoFillBackground(true);
    lbl_warn->setStyleSheet("background-color: #F0FFFF; border: 3px solid red ;");
    lbl_warn->setText("<HTML>"
                      "<CENTER>"
                      "<FONT color = #FF0000>"
                      "External simulator support is experimental and could be unstable. <BR>"
                      "Use it at your own risk! Future release are likely to be more stable. <BR>"
                      "See spice4qucs-help documentation at <BR>"
                      "http://qucs-help.readthedocs.org/en/spice4qucs/index.html/ <BR>"
                      "for more information and the current development status."
                      "</FONT>"
                      "</CENTER>"
                      "</HTML>");
    //lbl_warn->setBackgroundColor(Qt::white);

    QVBoxLayout *vl_top = new QVBoxLayout;
    vl_top->addWidget(lbl_warn);
    QHBoxLayout *hl2 = new QHBoxLayout;
    hl2->addWidget(lblSimulator);
    hl2->addWidget(cbxSimualor);
    hl2->addWidget(buttonSimSettings);
    vl_top->addLayout(hl2);
    vl_top->addWidget(grp1);
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
}

ExternSimDialog::~ExternSimDialog()
{
    ngspice->killThemAll();
}

void ExternSimDialog::slotSetSimulator()
{
    switch (cbxSimualor->currentIndex()) {
    case simNgspice: {
        xyce->setParallel(false);
        disconnect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        disconnect(xyce,SIGNAL(finished()),this,SLOT(slotProcessXyceOutput()));
        disconnect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessNgspiceOutput()));
        connect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        disconnect(buttonSimulate,SIGNAL(clicked()),xyce,SLOT(slotSimulate()));
        connect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()));
    }
        break;
    case simXyceSer: {
        xyce->setParallel(false);
        disconnect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        disconnect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessNgspiceOutput()));
        disconnect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(xyce,SIGNAL(finished()),this,SLOT(slotProcessXyceOutput()));
        connect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(buttonSimulate,SIGNAL(clicked()),xyce,SLOT(slotSimulate()));
        disconnect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()));
    }
        break;
    case simXycePar: {
#ifdef Q_OS_UNIX
        xyce->setParallel(true);
#else
        xyce->setParallel(false);
#endif
        disconnect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        disconnect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessNgspiceOutput()));
        disconnect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(xyce,SIGNAL(finished()),this,SLOT(slotProcessXyceOutput()));
        connect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        connect(buttonSimulate,SIGNAL(clicked()),xyce,SLOT(slotSimulate()));
        disconnect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()));
    }
        break;
    default: break;
    }
}


void ExternSimDialog::slotProcessNgspiceOutput()
{
    buttonSaveNetlist->setEnabled(true);
    buttonStopSim->setEnabled(false);
    cbxSimualor->setEnabled(true);
    QString out = ngspice->getOutput();
    if (out.contains("warning",Qt::CaseInsensitive)||
       (out.contains("error"),Qt::CaseInsensitive)) {
        emit warnings();
    } else emit success();
    //editSimConsole->clear();
    editSimConsole->insertPlainText(out);
    saveLog();
    // Set temporary safe output name
    QFileInfo inf(Sch->DocName);
    //QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_ngspice.dat";
    QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+".dat.ngspice";
    ngspice->convertToQucsData(qucs_dataset);
    emit simulated();
    wasSimulated = true;
}

void ExternSimDialog::slotProcessXyceOutput()
{
    buttonStopSim->setEnabled(false);
    cbxSimualor->setEnabled(true);
    QString out = xyce->getOutput();
    //editSimConsole->clear();
    editSimConsole->insertPlainText(out);
    if (out.contains("warning",Qt::CaseInsensitive)||
       (out.contains("error"),Qt::CaseInsensitive)) {
        emit warnings();
    } else emit success();
    saveLog();
    // Set temporary safe output name
    QFileInfo inf(Sch->DocName);
    // QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_xyce.dat";
    QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+".dat.xyce";
    xyce->convertToQucsData(qucs_dataset,true);
    emit simulated();
    wasSimulated = true;
}

void ExternSimDialog::slotNgspiceStarted()
{
    editSimConsole->clear();
    QString sim;
    switch (cbxSimualor->currentIndex()) {
    case simNgspice: sim = "Ngspice";
        break;
    case simXyceSer: sim = "Xyce (serial) ";
        break;
    case simXycePar: sim = "Xyce (parallel) ";
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
    default : msg = tr("Simualtor error!");
    }

    QMessageBox::critical(this,tr("Simulate with SPICE"),msg,QMessageBox::Ok);

    QString sim;
    switch (cbxSimualor->currentIndex()) {
    case simNgspice: sim = "Ngspice";
        break;
    case simXyceSer: sim = "Xyce (serial) ";
        break;
    case simXycePar: sim = "Xyce (parallel) ";
        break;
    default: sim = "Simulator "; // Some other simulators could be added ...
        break;
    }
    editSimConsole->insertPlainText(sim + tr(" error..."));
}

void ExternSimDialog::slotStart()
{
    buttonStopSim->setEnabled(true);
    cbxSimualor->setEnabled(false);
    buttonSaveNetlist->setEnabled(false);
}

void ExternSimDialog::slotStop()
{
    buttonStopSim->setEnabled(false);
    cbxSimualor->setEnabled(true);
    buttonSaveNetlist->setEnabled(true);
    ngspice->killThemAll();
}

void ExternSimDialog::slotSimSettings()
{
    SimSettingsDialog *SetDlg = new SimSettingsDialog(this);
    if (SetDlg->exec()) {
        ngspice->setSimulatorCmd(QucsSettings.NgspiceExecutable);
        xyce->setSimulatorCmd(QucsSettings.XyceExecutable);
        workdir = QucsSettings.S4Qworkdir;
        QFileInfo inf(workdir);
        if (!inf.exists()) {
            QDir dir;
            dir.mkpath(workdir);
        }
        ngspice->setWorkdir(QucsSettings.S4Qworkdir);
        xyce->setWorkdir(QucsSettings.S4Qworkdir);
    }
    delete SetDlg;
}

void ExternSimDialog::slotSaveNetlist()
{
    QFileInfo inf(Sch->DocName);
    QString filename = QFileDialog::getSaveFileName(this,tr("Save netlist"),inf.path()+QDir::separator()+"netlist.cir",
                       "All files (*)");
    if (filename.isEmpty()) return;

    switch (cbxSimualor->currentIndex()) {
    case simNgspice: {
        ngspice->SaveNetlist(filename);
        }
        break;
    case simXyceSer:
    case simXycePar: {
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


