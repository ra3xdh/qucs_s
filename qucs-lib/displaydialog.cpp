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

#include <qlayout.h>
#include <q3hbox.h>
#include <qpushbutton.h>
#include <q3textedit.h>
#include <q3vgroupbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>


DisplayDialog::DisplayDialog(QWidget *parent, bool helper)
                     : QDialog(parent, 0, false, Qt::WDestructiveClose)
{
  vLayout = new Q3VBoxLayout(this);
  vLayout->setMargin(3);

  if(helper) {
    Text = new Q3TextEdit(this);
    Text->setTextFormat(Qt::PlainText);
    Text->setReadOnly(true);
    Text->setMinimumSize(200, 100);
    vLayout->addWidget(Text);
  }
  else {
    Q3VGroupBox *Analog = new Q3VGroupBox (tr("Analogue"), this);
    Text = new Q3TextEdit(Analog);
    Text->setTextFormat(Qt::PlainText);
    Text->setReadOnly(true);
    Text->setMinimumSize(200, 80);
    vLayout->addWidget(Analog);

    Q3VGroupBox *VHDL = new Q3VGroupBox (tr("VHDL"), this);
    VHDLText = new Q3TextEdit(VHDL);
    VHDLText->setTextFormat(Qt::PlainText);
    VHDLText->setReadOnly(true);
    VHDLText->setMinimumSize(200, 80);
    vLayout->addWidget(VHDL);

    Q3VGroupBox *Verilog = new Q3VGroupBox (tr("Verilog"), this);
    VerilogText = new Q3TextEdit(Verilog);
    VerilogText->setTextFormat(Qt::PlainText);
    VerilogText->setReadOnly(true);
    VerilogText->setMinimumSize(200, 80);
    vLayout->addWidget(Verilog);
  }

  Q3HBox *h = new Q3HBox(this);
  vLayout->addWidget(h);

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder

  QPushButton *ButtonClose = new QPushButton(tr("Close"), h);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder
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
