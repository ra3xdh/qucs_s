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


#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>


HelpDialog::HelpDialog(QWidget *parent)
                     : QDialog(parent)
{
  setWindowTitle("Qucs Active Filter Help");


  // --------  set help text into dialog  ------------
  QString s(tr("QucsActiveFilter is a active filter synthesis program. "
           "Butterworth, Chebyshev, Inverse Chebyshev, Cauer, Bessel"
           " and User defined transfer function are supported."
	       "To create a filter, simply enter all "
	       "parameters and press the big button at the "
	       "bottom of the main window. Immediatly, the "
	       "schematic of the filter is calculated and "
	       "put into the clipboard. Now go to Qucs, "
	       "open an empty schematic and press "
	       "CTRL-V (paste from clipboard). The filter "
	       "schematic can now be inserted and "
	       " simulated. Have lots of fun!"));


  // --------  create dialog widgets  ------------
  resize(250, 230);

  vLayout = new QVBoxLayout();

  Text = new QTextEdit(s, this);
  //Text->setTextFormat(Qt::PlainText);
  Text->setReadOnly(true);
  //Text->setWordWrap(QTextEdit::NoWrap);
  Text->setMinimumSize(200,200);
  vLayout->addWidget(Text);

  QVBoxLayout *h = new QVBoxLayout();
  h->addLayout(vLayout);

  h->addStretch(5);

  QPushButton *ButtonClose = new QPushButton(tr("Close"));
  h->addWidget(ButtonClose);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(accept()));
  ButtonClose->setFocus();

  h->addStretch(5);

  setLayout(h);
}

HelpDialog::~HelpDialog()
{
  delete vLayout;
}

