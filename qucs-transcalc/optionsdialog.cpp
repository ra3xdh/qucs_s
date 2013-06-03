/***************************************************************************
                              optionsdialog.cpp
                             --------------------
    begin                : Sun Apr 03 2005
    copyright            : (C) 2005 by Stefan Jahn
    email                : stefan@lkcc.org
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

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "optionsdialog.h"
#include "qucstrans.h"

extern struct TransUnit TransUnits[];

OptionsDialog::OptionsDialog(QWidget *parent)
                     : QDialog(parent) //, 0, false, Qt::WDestructiveClose)
{
  setWindowTitle("QucsTranscalc "+tr("Options"));

  // --------  create dialog widgets  ------------
  vLayout = new QVBoxLayout(this);
  vLayout->setMargin(3);
  vLayout->setSpacing(3);

  QGroupBox * unitsGroup = new QGroupBox(tr("Units"));
  QVBoxLayout * l = new QVBoxLayout();
  l->setSpacing(3);
  QLabel * lfr = new QLabel(tr("Frequency"));
  lfr->setAlignment (Qt::AlignRight);
  l->addWidget(lfr);
  QLabel * lle = new QLabel(tr("Length"));
  lle->setAlignment (Qt::AlignRight);
  l->addWidget(lle);
  QLabel * lre = new QLabel(tr("Resistance"));
  lre->setAlignment (Qt::AlignRight);
  l->addWidget(lre);
  QLabel * lan = new QLabel(tr("Angle"));
  lan->setAlignment (Qt::AlignRight);
  l->addWidget(lan);
  QVBoxLayout * r = new QVBoxLayout();
  r->setSpacing(3);
  for (int j = 0; j < 4; j++) {
    units[j] = new QComboBox(); //?r);
    r->addWidget(units[j]);
    for (int i = 0; TransUnits[j].units[i] != NULL; i++)
      units[j]->addItem(TransUnits[j].units[i]);
  }
  units[0]->setCurrentIndex(QucsSettings.freq_unit);
  units[1]->setCurrentIndex(QucsSettings.length_unit);
  units[2]->setCurrentIndex(QucsSettings.res_unit);
  units[3]->setCurrentIndex(QucsSettings.ang_unit);
  
  QHBoxLayout * h1 = new QHBoxLayout();
  h1->addLayout(l);
  h1->addLayout(r);
  unitsGroup->setLayout(h1);

  vLayout->addWidget(unitsGroup);

  QHBoxLayout * h2 = new QHBoxLayout();

  QPushButton *ButtonSave = new QPushButton(tr("Save as Default"));
  connect(ButtonSave, SIGNAL(clicked()), SLOT(slotSave()));

  QPushButton *ButtonClose = new QPushButton(tr("Dismiss"));
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();

  h2->addWidget(ButtonSave);
  h2->addWidget(ButtonClose);
  vLayout->addLayout(h2);
}

OptionsDialog::~OptionsDialog()
{
  delete vLayout;
}

void OptionsDialog::slotClose()
{
  accept();
}

void OptionsDialog::slotSave()
{
  QucsSettings.freq_unit = units[0]->currentIndex();
  QucsSettings.length_unit = units[1]->currentIndex();
  QucsSettings.res_unit = units[2]->currentIndex();
  QucsSettings.ang_unit = units[3]->currentIndex();
  accept();
}
