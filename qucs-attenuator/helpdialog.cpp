/***************************************************************************
                               helpdialog.cpp
                             ------------------
    begin                : Fri Mar 04 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "helpdialog.h"

#include <QLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>


HelpDialog::HelpDialog(QWidget *parent)
                     : QDialog(parent)//, 0, false, Qt::WDestructiveClose)
{
  setWindowTitle(tr("Qucs Attenuator Help"));


  // --------  set help text into dialog  ------------
  QString s(tr("QucsAttenuator is an attenuator synthesis program. "
	       "To create a attenuator, simply enter all "
	       "the input parameters and press the calculation button. "
	       "Immediatly, the "
	       "schematic of the attenuator is calculated and "
	       "put into the clipboard. Now go to Qucs, "
	       "open an schematic and press "
	       "CTRL-V (paste from clipboard). The attenuator "
	       "schematic can now be inserted. "
	       "Have lots of fun!"));


  // --------  create dialog widgets  ------------
  resize(250, 230);

  vLayout = new QVBoxLayout();

  Text = new QTextEdit(s, this);
  //Text->setTextFormat(Qt::PlainText);
  Text->setReadOnly(true);
  //Text->setWordWrap(QTextEdit::NoWrap);
  Text->setMinimumSize(200,200);
  vLayout->addWidget(Text);

  QHBoxLayout *h = new QHBoxLayout();
  h->addLayout(vLayout);

  h->addStretch(5);

  QPushButton *ButtonClose = new QPushButton(tr("Close"));
  h->addWidget(ButtonClose);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();

  h->addStretch(5);

  setLayout(h);
}

HelpDialog::~HelpDialog()
{
  delete vLayout;
}

// ************************************************************
void HelpDialog::slotClose()
{
  accept();
}
