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

    ngspice = new QProcess(this);
    ngspice->setProcessChannelMode(QProcess::MergedChannels);
    connect(ngspice,SIGNAL(finished(int)),this,SLOT(slotProcessNgSpiceOutput(int)));
    connect(this,SIGNAL(destroyed()),this,SLOT(killThemAll()));

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
    QStringList vars,sims;
    QString tmp_path = QDir::homePath()+"/.qucs/spice4qucs.cir";
    QFile spice_file(tmp_path);
    if (spice_file.open(QFile::WriteOnly)) {
        QTextStream stream(&spice_file);
        Sch->createSpiceNetlist(stream,num,sims,vars);
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
    ngspice->setWorkingDirectory(QDir::convertSeparators(QDir::homePath()+"/.qucs/"));
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
    editSimConsole->clear();
    editSimConsole->append(out);
}
