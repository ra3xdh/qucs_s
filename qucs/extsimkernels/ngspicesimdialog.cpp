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

    ngspice = new Ngspice(sch,this);

    connect(ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
    connect(ngspice,SIGNAL(finished()),this,SLOT(slotProcessNgSpiceOutput()));
    connect(ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError()));

    buttonSimulate = new QPushButton(tr("Simulate"),this);
    connect(buttonSimulate,SIGNAL(clicked()),ngspice,SLOT(slotSimulate()));
    connect(buttonSimulate,SIGNAL(clicked()),this,SLOT(slotStart()));
    buttonStopSim = new QPushButton(tr("Stop"),this);
    connect(buttonStopSim,SIGNAL(clicked()),this,SLOT(reject()));
    connect(buttonStopSim,SIGNAL(clicked()),ngspice,SLOT(killThemAll()));
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
    ngspice->killThemAll();
}



void NgspiceSimDialog::slotProcessNgSpiceOutput()
{
    buttonStopSim->setEnabled(false);
    QString out = ngspice->getOutput();
    //editSimConsole->clear();
    editSimConsole->append(out);
    // Set temporary safe output name
    QFileInfo inf(Sch->DocName);
    QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_ngspice.dat";
    ngspice->convertToQucsData(qucs_dataset);
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

void NgspiceSimDialog::slotStart()
{
    buttonStopSim->setEnabled(true);
}

void NgspiceSimDialog::slotStop()
{
    buttonStopSim->setEnabled(false);
    ngspice->killThemAll();
}




