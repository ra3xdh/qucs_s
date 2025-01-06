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
#include "settings.h"

SimSettingsDialog::SimSettingsDialog(QWidget *parent) :
    QDialog(parent),
    a_lblXyce(new QLabel(tr("Xyce executable location"))),
    a_lblNgspice(new QLabel(tr("Ngspice executable location"))),
    a_lblSpiceOpus(new QLabel(tr("SpiceOpus executable location"))),
    //a_lblXycePar(new QLabel(tr("Xyce Parallel executable location (openMPI installed required)"))),
    //a_lblNprocs(new QLabel(tr("Number of processors in a system:"))),
    a_lblQucsator(new QLabel(tr("Qucsator executable location"))),
    //a_lblSimulator(new QLabel(tr("Default simulator"))),
    a_lblNgspiceSimParam(new QLabel(tr("Ngspice CLI parameters"))),
    a_lblXyceSimParam(new QLabel(tr("Xyce CLI parameters"))),
    a_lblSpopusSimParam(new QLabel(tr("SpiceOpus CLI parameters"))),
    a_lblCompatMode(new QLabel(tr("Ngspice compatibility mode"))),
    a_cbxCompatMode(new QComboBox),
    //a_cbxSimulator(new QComboBox(this)),
    a_edtNgspice(new QLineEdit(QucsSettings.NgspiceExecutable)),
    a_edtSpiceOpus(new QLineEdit(QucsSettings.SpiceOpusExecutable)),
    a_edtXyce(new QLineEdit(QucsSettings.XyceExecutable)),
    //a_edtXycePar(new QLineEdit(QucsSettings.XyceParExecutable),
    a_edtQucsator(new QLineEdit(QucsSettings.Qucsator)),
    //a_spbNprocs(new QSpinBox(this)),
    a_edtNgspiceSimParam(new QLineEdit()),
    a_edtXyceSimParam(new QLineEdit()),
    a_edtSpopusSimParam(new QLineEdit()),
    a_btnOK(new QPushButton(tr("Apply changes"))),
    a_btnCancel(new QPushButton(tr("Cancel"))),
    a_btnSetNgspice(new QPushButton(tr("Select ..."))),
    a_btnSetSpOpus(new QPushButton(tr("Select ..."))),
    a_btnSetXyce(new QPushButton(tr("Select ..."))),
    //a_btnSetXycePar(new QPushButton(tr("Select ..."))),
    a_btnSetQucsator(new QPushButton(tr("Select ...")))
{
//    a_cbxSimulator->addItem("Ngspice", 1);
//    a_cbxSimulator->addItem("Xyce", 2);
//    a_cbxSimulator->addItem("SpiceOpus", 4);
//    a_cbxSimulator->addItem("Qucsator", 8);
    //QStringList items;
    //items<<"Ngspice"<<"Xyce (Serial)"<<"Xyce (Parallel)"<<"SpiceOpus"<<"Qucsator";
    //a_cbxSimulator->addItems(items);
    qDebug()<<QucsSettings.DefaultSimulator;
//    a_cbxSimulator->setCurrentIndex(QucsSettings.DefaultSimulator);
//    if (QucsSettings.DefaultSimulator==spicecompat::simNotSpecified)
//        a_cbxSimulator->setCurrentIndex(spicecompat::simNgspice);

//    a_spbNprocs->setMinimum(1);
//    a_spbNprocs->setMaximum(256);
//    a_spbNprocs->setValue(1);
//    a_spbNprocs->setValue(QucsSettings.NProcs);

    a_edtNgspiceSimParam->setText(_settings::Get().item<QString>("NgspiceParams"));
    a_edtXyceSimParam->setText(_settings::Get().item<QString>("XyceParams"));
    a_edtSpopusSimParam->setText(_settings::Get().item<QString>("SpopusParams"));


    connect(a_btnOK,SIGNAL(clicked()),this,SLOT(slotApply()));
    connect(a_btnCancel,SIGNAL(clicked()),this,SLOT(reject()));

    connect(a_btnSetNgspice,SIGNAL(clicked()),this,SLOT(slotSetNgspice()));
    connect(a_btnSetXyce,SIGNAL(clicked()),this,SLOT(slotSetXyce()));
    //connect(a_btnSetXycePar,SIGNAL(clicked()),this,SLOT(slotSetXycePar()));
    connect(a_btnSetSpOpus,SIGNAL(clicked()),this,SLOT(slotSetSpiceOpus()));
    connect(a_btnSetQucsator,SIGNAL(clicked()),this,SLOT(slotSetQucsator()));

    QStringList lst_modes;
    lst_modes<<"Default"<<"LTspice"<<"HSPICE"<<"Spice3";
    a_cbxCompatMode->addItems(lst_modes);
    auto compat_mode = _settings::Get().item<int>("NgspiceCompatMode");
    a_cbxCompatMode->setCurrentIndex(compat_mode);

    QVBoxLayout *top = new QVBoxLayout;

//    QHBoxLayout *h8 = new QHBoxLayout;
//    h8->addWidget(a_lblSimulator,1);
//    h8->addWidget(a_cbxSimulator,3);
//    top->addLayout(h8);

    QGroupBox *gbp1 = new QGroupBox(this);
    gbp1->setTitle(tr("SPICE settings"));
    QVBoxLayout *top2 = new QVBoxLayout;
    top2->addWidget(a_lblNgspice);
    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(a_edtNgspice,3);
    h1->addWidget(a_btnSetNgspice,1);
    top2->addLayout(h1);

    QHBoxLayout *h4 = new QHBoxLayout;
    h4->addWidget(a_lblCompatMode);
    h4->addWidget(a_cbxCompatMode);
    top2->addLayout(h4);
    top2->addWidget(a_lblNgspiceSimParam);
    top2->addWidget(a_edtNgspiceSimParam);

    top2->addWidget(a_lblXyce);
    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(a_edtXyce,3);
    h2->addWidget(a_btnSetXyce,1);
    top2->addLayout(h2);
    top2->addWidget(a_lblXyceSimParam);
    top2->addWidget(a_edtXyceSimParam);

//    top2->addWidget(a_lblXycePar);
//    QHBoxLayout *h4 = new QHBoxLayout;
//    h4->addWidget(a_edtXycePar,3);
//    h4->addWidget(a_btnSetXycePar,1);
//    top2->addLayout(h4);
//
//    QHBoxLayout *h5 = new QHBoxLayout;
//    h5->addWidget(a_lblNprocs);
//    h5->addWidget(a_spbNprocs);
//    top2->addLayout(h5);

    top2->addWidget(a_lblSpiceOpus);
    QHBoxLayout *h7 = new QHBoxLayout;
    h7->addWidget(a_edtSpiceOpus,3);
    h7->addWidget(a_btnSetSpOpus,1);
    top2->addLayout(h7);
    top2->addWidget(a_lblSpopusSimParam);
    top2->addWidget(a_edtSpopusSimParam);

    gbp1->setLayout(top2);
    top->addWidget(gbp1);

    QGroupBox *gbp2 = new QGroupBox;
    gbp2->setTitle(tr("Qucsator settings"));
    QVBoxLayout *top3 = new QVBoxLayout;
    top3->addWidget(a_lblQucsator);
    QHBoxLayout *h9 = new QHBoxLayout;
    h9->addWidget(a_edtQucsator,3);
    h9->addWidget(a_btnSetQucsator,1);
    top3->addLayout(h9);
    gbp2->setLayout(top3);

    top->addWidget(gbp2);

    QHBoxLayout *h3 = new QHBoxLayout;
    h3->addWidget(a_btnOK);
    h3->addWidget(a_btnCancel);
    h3->addStretch(2);
    top->addLayout(h3);

    this->setLayout(top);
    this->setFixedWidth(500);
    this->setWindowTitle(tr("Setup simulators executable location"));

#ifndef Q_OS_UNIX
    // Only Unix supports Xyce-parallel
//    a_edtXycePar->setDisabled(true);
//    a_lblXycePar->setDisabled(true);
//    a_lblNprocs->setDisabled(true);
//    a_spbNprocs->setDisabled(true);
#endif

}


void SimSettingsDialog::slotApply()
{
    QucsSettings.NgspiceExecutable = a_edtNgspice->text();
    QucsSettings.XyceExecutable = a_edtXyce->text();
    //QucsSettings.XyceParExecutable = a_edtXycePar->text();
    QucsSettings.SpiceOpusExecutable = a_edtSpiceOpus->text();
    QucsSettings.Qucsator = a_edtQucsator->text();
    //QucsSettings.NProcs = a_spbNprocs->value();
//    if ((QucsSettings.DefaultSimulator != a_cbxSimulator->currentIndex())&&
//        (QucsSettings.DefaultSimulator != spicecompat::simNotSpecified)) {
//        QMessageBox::warning(this,tr("Simulator settings"),tr("Default simulator engine was changed!\n"
//                                                              "Please restart Qucs to affect changes!"));
//    }
//    QucsSettings.DefaultSimulator = a_cbxSimulator->currentIndex();
    settingsManager& qs = _settings::Get();
    qs.setItem<int>("NgspiceCompatMode", a_cbxCompatMode->currentIndex());
    qs.setItem<QString>("NgspiceParams", a_edtNgspiceSimParam->text());
    qs.setItem<QString>("XyceParams", a_edtXyceSimParam->text());
    qs.setItem<QString>("SpopusParams", a_edtSpopusSimParam->text());
    accept();
    saveApplSettings();
  }

void SimSettingsDialog::slotCancel()
{
    //if (QucsSettings.DefaultSimulator == spicecompat::simNotSpecified)
    //QucsSettings.DefaultSimulator = spicecompat::simQucsator;
    reject();
}

void SimSettingsDialog::slotSetNgspice()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select Ngspice executable location"),a_edtNgspice->text(),"All files (*)");
    if (!s.isEmpty()) {
        a_edtNgspice->setText(s);
    }
}

void SimSettingsDialog::slotSetXyce()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select Xyce executable location"),a_edtXyce->text(),"All files (*)");
    if (!s.isEmpty()) {
        a_edtXyce->setText(s);
    }
}

void SimSettingsDialog::slotSetXycePar() // TODO ZERGUD
{
//    QString s = QFileDialog::getOpenFileName(this,tr("Select Xyce Parallel executable location"),a_edtXycePar->text(),"All files (*)");
//    if (!s.isEmpty()) {
//        if (s.endsWith("xmpirun")) s += " -np %p";
//        a_edtXycePar->setText(s);
//    }
}

void SimSettingsDialog::slotSetSpiceOpus()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select SpiceOpus executable location"),a_edtSpiceOpus->text(),"All files (*)");
    if (!s.isEmpty()) {
        a_edtSpiceOpus->setText(s);
    }
}

void SimSettingsDialog::slotSetQucsator()
{
    QString s = QFileDialog::getOpenFileName(this,tr("Select Qucsator executable location"),a_edtQucsator->text(),"All files (*)");
    if (!s.isEmpty()) {
        a_edtQucsator->setText(s);
    }
}
