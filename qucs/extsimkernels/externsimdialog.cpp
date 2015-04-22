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

    workdir = QDir::convertSeparators(QDir::homePath()+"/.qucs/spice4qucs");
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
    connect(buttonStopSim,SIGNAL(clicked()),this,SLOT(reject()));
    connect(buttonStopSim,SIGNAL(clicked()),ngspice,SLOT(killThemAll()));
    buttonStopSim->setEnabled(false);

    buttonSaveNetlist = new QPushButton(tr("Save netlsit"),this);
    connect(buttonSaveNetlist,SIGNAL(clicked()),this,SLOT(slotSaveNetlist()));

    buttonSimSettings = new QPushButton(tr("Settings"),this);
    connect(buttonSimSettings,SIGNAL(clicked()),this,SLOT(slotSimSettings()));

    lblSimulator = new QLabel(tr("Select external simulator:"));
    QGroupBox *grp1 = new QGroupBox(tr("Simulation console"),this);
    QVBoxLayout *vbl1 = new QVBoxLayout;

    cbxSimualor = new QComboBox(this);
    QStringList items;
    items<<"Ngspice"<<"Xyce (Serial)"<<"Xyce (Parallel)";
    cbxSimualor->addItems(items);
    connect(cbxSimualor,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSetSimulator()));

    editSimConsole = new QTextEdit(this);
    editSimConsole->setReadOnly(true);
    vbl1->addWidget(editSimConsole);
    grp1->setLayout(vbl1);

    QVBoxLayout *vl_top = new QVBoxLayout;
    QHBoxLayout *hl2 = new QHBoxLayout;
    hl2->addWidget(lblSimulator);
    hl2->addWidget(cbxSimualor);
    hl2->addWidget(buttonSimSettings);
    vl_top->addLayout(hl2);
    vl_top->addWidget(grp1);
    QHBoxLayout *hl1 = new QHBoxLayout;
    hl1->addWidget(buttonSimulate);
    hl1->addWidget(buttonStopSim);
    hl1->addWidget(buttonSaveNetlist);
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
        disconnect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        disconnect(xyce,SIGNAL(finished()),this,SLOT(slotProcessXyceOutput()));
        disconnect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));
        connect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessNgspiceOutput()));
        connect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));
        disconnect(buttonSimulate,SIGNAL(clicked()),xyce,SLOT(slotSimulate()));
        connect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()));
    }
        break;
    case simXyceSer: {
        disconnect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        disconnect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessNgspiceOutput()));
        disconnect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));
        connect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(xyce,SIGNAL(finished()),this,SLOT(slotProcessXyceOutput()));
        connect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));
        connect(buttonSimulate,SIGNAL(clicked()),xyce,SLOT(slotSimulate()));
        disconnect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()));
    }
        break;
    case simXycePar: {
        disconnect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        disconnect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessNgspiceOutput()));
        disconnect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));
        connect(xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(xyce,SIGNAL(finished()),this,SLOT(slotProcessXyceOutput()));
        connect(xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));
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
    //editSimConsole->clear();
    editSimConsole->append(out);
    // Set temporary safe output name
    QFileInfo inf(Sch->DocName);
    QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_ngspice.dat";
    ngspice->convertToQucsData(qucs_dataset);
}

void ExternSimDialog::slotProcessXyceOutput()
{
    buttonStopSim->setEnabled(false);
    cbxSimualor->setEnabled(true);
    QString out = xyce->getOutput();
    //editSimConsole->clear();
    editSimConsole->append(out);
    // Set temporary safe output name
    QFileInfo inf(Sch->DocName);
    QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_xyce.dat";
    xyce->convertToQucsData(qucs_dataset,true);
}

void ExternSimDialog::slotNgspiceStarted()
{
    editSimConsole->clear();
    editSimConsole->append(tr("ngspice started...\n"));
}

void ExternSimDialog::slotNgspiceStartError()
{
    editSimConsole->append(tr("ngspice error..."));
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
    }
    delete SetDlg;
}

void ExternSimDialog::slotSaveNetlist()
{
    QString filename;
    QFileDialog dialog(this,tr("Save netlist"),QDir::homeDirPath()+QDir::separator()+"netlist.cir",
                       "All files (*)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec()) {
        filename = dialog.selectedFile();
    } else return;

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


