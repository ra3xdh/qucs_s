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

#include <qlayout.h>
#include <q3hbox.h>
#include <q3hgroupbox.h>
#include <qpushbutton.h>
#include <q3vbox.h>
#include <qlabel.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include "optionsdialog.h"
#include "qucstrans.h"

extern struct TransUnit TransUnits[];

OptionsDialog::OptionsDialog(QWidget *parent)
                     : QDialog(parent, 0, false, Qt::WDestructiveClose)
{
  setCaption("QucsTranscalc "+tr("Options"));

  // --------  create dialog widgets  ------------
  vLayout = new Q3VBoxLayout(this);
  vLayout->setMargin(3);
  vLayout->setSpacing(3);

  Q3HGroupBox * h = new Q3HGroupBox(tr("Units"), this);
  vLayout->addWidget(h);
  Q3VBox * l = new Q3VBox(h);
  l->setSpacing(3);
  QLabel * lfr = new QLabel(tr("Frequency"),l);
  lfr->setAlignment (Qt::AlignRight);
  QLabel * lle = new QLabel(tr("Length"),l);
  lle->setAlignment (Qt::AlignRight);
  QLabel * lre = new QLabel(tr("Resistance"),l);
  lre->setAlignment (Qt::AlignRight);
  QLabel * lan = new QLabel(tr("Angle"),l);
  lan->setAlignment (Qt::AlignRight);
  Q3VBox * r = new Q3VBox(h);
  r->setSpacing(3);
  for (int j = 0; j < 4; j++) {
    units[j] = new QComboBox(r);
    for (int i = 0; TransUnits[j].units[i] != NULL; i++)
      units[j]->insertItem (TransUnits[j].units[i]);
  }
  units[0]->setCurrentItem (QucsSettings.freq_unit);
  units[1]->setCurrentItem (QucsSettings.length_unit);
  units[2]->setCurrentItem (QucsSettings.res_unit);
  units[3]->setCurrentItem (QucsSettings.ang_unit);
  
  Q3HBox * h2 = new Q3HBox(this);
  vLayout->addWidget(h2);

  QPushButton *ButtonSave = new QPushButton(tr("Save as Default"), h2);
  connect(ButtonSave, SIGNAL(clicked()), SLOT(slotSave()));

  QPushButton *ButtonClose = new QPushButton(tr("Dismiss"), h2);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();
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
  QucsSettings.freq_unit = units[0]->currentItem();
  QucsSettings.length_unit = units[1]->currentItem();
  QucsSettings.res_unit = units[2]->currentItem();
  QucsSettings.ang_unit = units[3]->currentItem();
  accept();
}
