/***************************************************************************
                               displaydialog.cpp
                              -------------------
    begin                : Sat May 28 2005
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

#include "displaydialog.h"

//#include <QLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QVBoxLayout>



DisplayDialog::DisplayDialog(QWidget *parent, QString Text, QString SpiceText)
                     // QDialog(parent, 0, false, Qt::WDestructiveClose)
    : QDialog(parent)
{
  vLayout = new QVBoxLayout(this);
  vLayout->setContentsMargins(3,3,3,3);

  QGroupBox *Spice = new QGroupBox (tr("SPICE"), this);
  QVBoxLayout *SpiceLayout = new QVBoxLayout();

  QSpice = new QTextEdit(Spice);
  QSpice->setText(SpiceText);
  //QVHDLText->setTextFormat(Qt::PlainText);
  QSpice->setReadOnly(true);
  QSpice->setMinimumSize(200, 80);
  QSpice->setLineWrapMode(QTextEdit::NoWrap);

  SpiceLayout->addWidget(QSpice);
  Spice->setLayout(SpiceLayout);

  vLayout->addWidget(Spice);

  QGroupBox *Analog = new QGroupBox (tr("Qucs"), this);
  QVBoxLayout *AnalogLayout = new QVBoxLayout();

  QText = new QTextEdit();
  QText->setText(Text);
  //QText->setTextFormat(Qt::PlainText);
  QText->setReadOnly(true);
  QText->setMinimumSize(200, 80);
  QText->setLineWrapMode(QTextEdit::NoWrap);

  AnalogLayout->addWidget(QText);
  Analog->setLayout(AnalogLayout);

  vLayout->addWidget(Analog);

  QWidget *h = new QWidget(this);
  //h->setStretchFactor(new QWidget(h),5); // stretchable placeholder
  QHBoxLayout *hLayout = new QHBoxLayout();

  QPushButton *ButtonClose = new QPushButton(tr("Close"));
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();

  QWidget *dummy1 = new QWidget();
  hLayout->addWidget(dummy1);

  hLayout->addWidget(ButtonClose);

  QWidget *dummy2 = new QWidget();
  hLayout->addWidget(dummy2);
  hLayout->setStretchFactor(dummy1, 5);
  hLayout->setStretchFactor(dummy2, 5);
  h->setLayout(hLayout);

  vLayout->addWidget(h);

}

DisplayDialog::~DisplayDialog()
{
  delete vLayout;
}

// ************************************************************
void DisplayDialog::slotClose()
{
  accept();
}
