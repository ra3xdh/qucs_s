/***************************************************************************
                           simsettingsdialog.cpp
                             ----------------
    begin                : Tue Apr 21 2015
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

#include "simsettingsdialog.h"
#include "main.h"

SimSettingsDialog::SimSettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    lblSimulator = new QLabel(tr("Default simulator"));
    lblNgspice = new QLabel(tr("Ngspice executable location"));
    lblXyce = new QLabel(tr("Xyce executable location"));
    lblXycePar = new QLabel(tr("Xyce Parallel executable location (openMPI installed required)"));
    lblSpiceOpus = new QLabel(tr("SpiceOpus executable location"));
    lblQucsator = new QLabel(tr("Qucsator executable location"));
    lblNprocs = new QLabel(tr("Number of processors in a system:"));
    lblWorkdir = new QLabel(tr("Directory to store netlist and simulator output"));
    lblSimParam = new QLabel(tr("Extra simulator parameters"));

    cbxSimulator = new QComboBox(this);
    QStringList items;
    items<<"Ngspice"<<"Xyce (Serial)"<<"Xyce (Parallel)"<<"SpiceOpus"<<"Qucsator";
    cbxSimulator->addItems(items);
    qDebug()<<QucsSettings.DefaultSimulator;
    cbxSimulator->setCurrentIndex(QucsSettings.DefaultSimulator);
    if (QucsSettings.DefaultSimulator==spicecompat::simNotSpecified)
        cbxSimulator->setCurrentIndex(spicecompat::simQucsator);

    edtNgspice = new QLineEdit(QucsSettings.NgspiceExecutable);
    edtXyce = new QLineEdit(QucsSettings.XyceExecutable);
    edtXycePar = new QLineEdit(QucsSettings.XyceParExecutable);
    edtSpiceOpus = new QLineEdit(QucsSettings.SpiceOpusExecutable);
    edtQucsator = new QLineEdit(QucsSettings.Qucsator);
    spbNprocs = new QSpinBox(1,256,1,this);
    spbNprocs->setValue(QucsSettings.NProcs);
    edtWorkdir = new QLineEdit(QucsSettings.S4Qworkdir);
    edtSimParam = new QLineEdit(QucsSettings.SimParameters);

    btnOK = new QPushButton(tr("Apply changes"));
    connect(btnOK,SIGNAL(clicked()),this,SLOT(slotApply()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));

    btnSetNgspice = new QPushButton(tr("Select ..."));
    connect(btnSetNgspice,SIGNAL(clicked()),this,SLOT(slotSetNgspice()));
    btnSetXyce = new QPushButton(tr("Select ..."));
    connect(btnSetXyce,SIGNAL(clicked()),this,SLOT(slotSetXyce()));
    btnSetXycePar = new QPushButton(tr("Select ..."));
    connect(btnSetXycePar,SIGNAL(clicked()),this,SLOT(slotSetXycePar()));
    btnSetSpOpus = new QPushButton(tr("Select ..."));
    connect(btnSetSpOpus,SIGNAL(clicked()),this,SLOT(slotSetSpiceOpus()));
    btnSetQucsator = new QPushButton(tr("Select ..."));
    connect(btnSetQucsator,SIGNAL(clicked()),this,SLOT(slotSetQucsator()));
    btnSetWorkdir = new QPushButton(tr("Select ..."));
    connect(btnSetWorkdir,SIGNAL(clicked()),this,SLOT(slotSetWorkdir()));

    QVBoxLayout *top = new QVBoxLayout;

    QHBoxLayout *h8 = new QHBoxLayout;
    h8->addWidget(lblSimulator,1);
    h8->addWidget(cbxSimulator,3);
    top->addLayout(h8);

    QGroupBox *gbp1 = new QGroupBox(this);
    gbp1->setTitle(tr("SPICE settings"));
    QVBoxLayout *top2 = new QVBoxLayout;
    top2->addWidget(lblNgspice);
    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(edtNgspice,3);
    h1->addWidget(btnSetNgspice,1);
    top2->addLayout(h1);

    top2->addWidget(lblXyce);
    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(edtXyce,3);
    h2->addWidget(btnSetXyce,1);
    top2->addLayout(h2);

    top2->addWidget(lblXycePar);
    QHBoxLayout *h4 = new QHBoxLayout;
    h4->addWidget(edtXycePar,3);
    h4->addWidget(btnSetXycePar,1);
    top2->addLayout(h4);

    QHBoxLayout *h5 = new QHBoxLayout;
    h5->addWidget(lblNprocs);
    h5->addWidget(spbNprocs);
    top2->addLayout(h5);

    top2->addWidget(lblSpiceOpus);
    QHBoxLayout *h7 = new QHBoxLayout;
    h7->addWidget(edtSpiceOpus,3);
    h7->addWidget(btnSetSpOpus,1);
    top2->addLayout(h7);


    top2->addWidget(lblWorkdir);
    QHBoxLayout *h6 = new QHBoxLayout;
    h6->addWidget(edtWorkdir,3);
    h6->addWidget(btnSetWorkdir,1);
    top2->addLayout(h6);

    top2->addWidget(lblSimParam);
    QHBoxLayout *h10 = new QHBoxLayout;
    h10->addWidget(edtSimParam,4);
    top2->addLayout(h10);

    gbp1->setLayout(top2);
    top->addWidget(gbp1);

    QGroupBox *gbp2 = new QGroupBox;
    gbp2->setTitle(tr("Qucsator settings"));
    QVBoxLayout *top3 = new QVBoxLayout;
    top3->addWidget(lblQucsator);
    QHBoxLayout *h9 = new QHBoxLayout;
    h9->addWidget(edtQucsator,3);
    h9->addWidget(btnSetQucsator,1);
    top3->addLayout(h9);
    gbp2->setLayout(top3);

    top->addWidget(gbp2);

    QHBoxLayout *h3 = new QHBoxLayout;
    h3->addWidget(btnOK);
    h3->addWidget(btnCancel);
    h3->addStretch(2);
    top->addLayout(h3);

    this->setLayout(top);
    this->setFixedWidth(500);
    this->setWindowTitle(tr("Setup simulators executable location"));

#ifndef Q_OS_UNIX
    // Only Unix supports Xyce-parallel
    edtXycePar->setDisabled(true);
    lblXycePar->setDisabled(true);
    lblNprocs->setDisabled(true);
    spbNprocs->setDisabled(true);
#endif

}


void SimSettingsDialog::slotApply()
{
    QucsSettings.NgspiceExecutable = edtNgspice->text();
    QucsSettings.XyceExecutable = edtXyce->text();
    QucsSettings.XyceParExecutable = edtXycePar->text();
    QucsSettings.SpiceOpusExecutable = edtSpiceOpus->text();
    QucsSettings.Qucsator = edtQucsator->text();
    QucsSettings.NProcs = spbNprocs->value();
    QucsSettings.S4Qworkdir = edtWorkdir->text();
    QucsSettings.SimParameters = edtSimParam->text();
    if ((QucsSettings.DefaultSimulator != cbxSimulator->currentIndex())&&
        (QucsSettings.DefaultSimulator != spicecompat::simNotSpecified)) {
        QMessageBox::warning(this,tr("Simulator settings"),tr("Default simulator engine was changed!\n"
                                                              "Please restart Qucs to affect changes!"));
    }
    QucsSettings.DefaultSimulator = cbxSimulator->currentIndex();
    accept();
    saveApplSettings();
  }

void SimSettingsDialog::slotCancel()
{
    if (QucsSettings.DefaultSimulator == spicecompat::simNotSpecified)
    QucsSettings.DefaultSimulator = spicecompat::simQucsator;
    reject();
}

void SimSettingsDialog::slotSetNgspice()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select Ngspice executable location"),edtNgspice->text(),"All files (*)");
    if (!s.isEmpty()) {
        edtNgspice->setText(s);
    }
}

void SimSettingsDialog::slotSetXyce()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select Xyce executable location"),edtXyce->text(),"All files (*)");
    if (!s.isEmpty()) {
        edtXyce->setText(s);
    }
}

void SimSettingsDialog::slotSetXycePar()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select Xyce Parallel executable location"),edtXycePar->text(),"All files (*)");
    if (!s.isEmpty()) {
        if (s.endsWith("xmpirun")) s += " -np %p";
        edtXycePar->setText(s);
    }
}

void SimSettingsDialog::slotSetSpiceOpus()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select SpiceOpus executable location"),edtSpiceOpus->text(),"All files (*)");
    if (!s.isEmpty()) {
        edtSpiceOpus->setText(s);
    }
}

void SimSettingsDialog::slotSetQucsator()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select Qucsator executable location"),edtQucsator->text(),"All files (*)");
    if (!s.isEmpty()) {
        edtQucsator->setText(s);
    }
}

void SimSettingsDialog::slotSetWorkdir()
{
    QFileDialog dlg( this, tr("Select directory to store netlist and simulator output"), edtWorkdir->text() );
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::DirectoryOnly);
    if (dlg.exec()) {
        QString s = dlg.selectedFile();
        if (!s.isEmpty()) edtWorkdir->setText(s);
    }
}
