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
    lblNgspice = new QLabel(tr("Ngspice executable location"));
    lblXyce = new QLabel(tr("Xyce executable location"));

    edtNgspice = new QLineEdit(QucsSettings.NgspiceExecutable);
    edtXyce = new QLineEdit(QucsSettings.XyceExecutable);

    btnOK = new QPushButton(tr("Apply changes"));
    connect(btnOK,SIGNAL(clicked()),this,SLOT(slotApply()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));

    btnSetNgspice = new QPushButton(tr("Select ..."));
    connect(btnSetNgspice,SIGNAL(clicked()),this,SLOT(slotSetNgspice()));
    btnSetXyce = new QPushButton(tr("Select.."));
    connect(btnSetXyce,SIGNAL(clicked()),this,SLOT(slotSetXyce()));

    QVBoxLayout *top = new QVBoxLayout;
    top->addWidget(lblNgspice);
    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(edtNgspice,3);
    h1->addWidget(btnSetNgspice,1);
    top->addLayout(h1);

    top->addWidget(lblXyce);
    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(edtXyce,3);
    h2->addWidget(btnSetXyce,1);
    top->addLayout(h2);

    QHBoxLayout *h3 = new QHBoxLayout;
    h3->addWidget(btnOK);
    h3->addWidget(btnCancel);
    h3->addStretch(2);
    top->addLayout(h3);

    this->setLayout(top);
    this->setFixedWidth(500);

}


void SimSettingsDialog::slotApply()
{
    QucsSettings.NgspiceExecutable = edtNgspice->text();
    QucsSettings.XyceExecutable = edtXyce->text();
    accept();
}

void SimSettingsDialog::slotSetNgspice()
{
    QFileDialog dialog(this,tr("Select Ngspice executable location"),edtNgspice->text(),"All files (*)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec()) {
        edtNgspice->setText(dialog.selectedFile());
    }
}

void SimSettingsDialog::slotSetXyce()
{
    QFileDialog dialog(this,tr("Select Xyce executable location"),edtXyce->text(),"All files (*)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec()) {
        edtXyce->setText(dialog.selectedFile());
    }
}
