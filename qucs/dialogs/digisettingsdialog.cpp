/***************************************************************************
                           digisettingsdialog.cpp
                          ------------------------
    begin                : Sat Apr 01 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "digisettingsdialog.h"
#include "textdoc.h"
#include "misc.h"


DigiSettingsDialog::DigiSettingsDialog(TextDoc *Doc_)
                  : QDialog(Doc_) 
{
  Doc = Doc_;
  setWindowTitle(tr("Document Settings"));

  Expr.setPattern("[0-9][0-9a-zA-Z ]+"); // valid expression for LineEdit
  Validator = new QRegularExpressionValidator(Expr, this);

  QVBoxLayout *all = new QVBoxLayout(this);
  all->setContentsMargins(5,5,5,5);

  QGroupBox *setGroup = new QGroupBox(tr("Digital Simulation Settings"));
  all->addWidget(setGroup);
 
  QVBoxLayout *group = new QVBoxLayout();
  setGroup->setLayout(group);
   
  QButtonGroup *toggleGroup = new QButtonGroup();
  simRadio = new QRadioButton(tr("Simulation"));
  group->addWidget(simRadio);
  simRadio->setChecked(Doc->simulation);

  QHBoxLayout *hb1 = new QHBoxLayout();
  hb1->setSpacing(5);
  TimeLabel = new QLabel(tr("Duration of Simulation:"));
  hb1->addWidget(TimeLabel);
  Doc->loadSimulationTime(SimTime);
  TimeEdit = new QLineEdit();
  hb1->addWidget(TimeEdit);
  TimeEdit->setValidator(Validator);
  TimeEdit->setText(SimTime);
  group->addLayout(hb1);

  QRadioButton *comRadio = new QRadioButton(tr("Precompile Module"));
  group->addWidget(comRadio);
  toggleGroup->addButton(simRadio);
  toggleGroup->addButton(comRadio);
  connect(toggleGroup, SIGNAL(buttonClicked(int)), SLOT(slotChangeMode(int)));

  QHBoxLayout *hb3 = new QHBoxLayout();
  hb3->setSpacing(5);
  NameLabel = new QLabel(tr("Library Name:"));
  hb3->addWidget(NameLabel);
  NameEdit = new QLineEdit();
  hb3->addWidget(NameEdit);
  NameEdit->setText(Doc->Library);
  group->addLayout(hb3);

  group->addSpacing(15);
  
  QHBoxLayout *hb2 = new QHBoxLayout();
  hb2->setSpacing(5);
  LibLabel = new QLabel(tr("Libraries:"));
  hb2->addWidget(LibLabel);
  LibEdit = new QLineEdit();
  hb2->addWidget(LibEdit);
  LibEdit->setText(Doc->Libraries);
  group->addLayout(hb2);

  all->addSpacing(5);
  all->addStretch();
  QHBoxLayout *Buttons = new QHBoxLayout();
  QPushButton *ButtonOk = new QPushButton(tr("Ok"));
  QPushButton *ButtonCancel = new QPushButton(tr("Cancel"));
  Buttons->addWidget(ButtonOk);
  Buttons->addWidget(ButtonCancel);
  connect(ButtonOk, SIGNAL(clicked()), SLOT(slotOk()));
  connect(ButtonCancel, SIGNAL(clicked()), SLOT(reject()));
  all->addLayout(Buttons);

  simRadio->setChecked(Doc->simulation);
  Doc->SimOpenDpl = Doc->simulation ? true : false;
  comRadio->setChecked(!Doc->simulation);
  slotChangeMode(!Doc->simulation);

  ButtonOk->setDefault(true);
  if(Doc->simulation) {
    setFocusProxy(TimeEdit);
    TimeEdit->setFocus();
  }
  else {
    setFocusProxy(NameEdit);
    NameEdit->setFocus();
  }
}

DigiSettingsDialog::~DigiSettingsDialog()
{
  delete Validator;
}

void DigiSettingsDialog::slotOk()
{
  bool changed = false;
  if(SimTime != TimeEdit->text()) {
    if(simRadio->isChecked()) {
      QString s = TimeEdit->text();
      if(!misc::VHDL_Time(s, tr("Document Settings"))) {
	QMessageBox::critical(this, tr("Error"), s.mid(1));
	reject();
	return;
      } else {
	Doc->SimTime = s;
	changed = true;
      }
    }
  }
  if(Doc->Libraries != LibEdit->text()) {
    QStringList lst = LibEdit->text().split(' ');
    Doc->Libraries = lst.join(" ");
    changed = true;
  }
  if(Doc->simulation != simRadio->isChecked()) {
    Doc->simulation = simRadio->isChecked();
    Doc->SimOpenDpl = Doc->simulation ? true : false;
    changed = true;
  }
  if(Doc->Library != NameEdit->text()) {
    QString lib = NameEdit->text().trimmed();
    Doc->Library = lib;
    changed = true;
  }

  if(changed) {
    Doc->SetChanged = true;
    Doc->slotSetChanged();
  }
  accept();
}

void DigiSettingsDialog::slotChangeMode(int idx)
{
  switch(idx) {
  case 0:
    TimeEdit->setEnabled(true);
    TimeEdit->setFocus();
    TimeLabel->setEnabled(true);
    NameEdit->setEnabled(false);
    NameLabel->setEnabled(false);
    break;
  case 1:
    TimeEdit->setEnabled(false);
    TimeLabel->setEnabled(false);
    NameEdit->setEnabled(true);
    NameLabel->setEnabled(true);
    break;
  }
}
