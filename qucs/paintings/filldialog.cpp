/***************************************************************************
                          filldialog.cpp  -  description
                             -------------------
    begin                : Thu May 20 2004
    copyright            : (C) 2003 by Michael Margraf
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

#include "filldialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <q3hbox.h>
#include <qvalidator.h>
#include <qcolordialog.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>


FillDialog::FillDialog(const QString& _Caption, bool show, QWidget *parent)
			: QDialog(parent)
{
  setCaption(_Caption);

  all = new Q3VBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  Q3GridLayout *gp1 = new Q3GridLayout(Tab1,3,2,5,5);

  gp1->addWidget(new QLabel(tr("Line Width: "), Tab1), 0,0);
  val100 = new QIntValidator(0,100, this);
  LineWidth = new QLineEdit(Tab1);
  LineWidth->setValidator(val100);
  LineWidth->setMaximumWidth(35);
  LineWidth->setText("0");
  gp1->addWidget(LineWidth, 0,1);

  gp1->addWidget(new QLabel(tr("Line Color: "), Tab1), 1,0);
  ColorButt = new QPushButton("        ",Tab1);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
  gp1->addWidget(ColorButt, 1,1);

  gp1->addWidget(new QLabel(tr("Line Style: "), Tab1), 2,0);
  StyleBox = new QComboBox(Tab1);
  StyleBox->insertItem(tr("solid line"));
  StyleBox->insertItem(tr("dash line"));
  StyleBox->insertItem(tr("dot line"));
  StyleBox->insertItem(tr("dash dot line"));
  StyleBox->insertItem(tr("dash dot dot line"));
  gp1->addWidget(StyleBox, 2,1);


  t->addTab(Tab1, tr("Line Style"));

  // ...........................................................
if(show) {
  QWidget *Tab2 = new QWidget(t);
  Q3GridLayout *gp2 = new Q3GridLayout(Tab2,3,2,5,5);

  CheckFilled = new QCheckBox(tr("enable filling"),Tab2);
  connect(CheckFilled, SIGNAL(toggled(bool)), SLOT(slotCheckFilled(bool)));
  gp2->addMultiCellWidget(CheckFilled, 0,0,0,1);


  FillLabel1 = new QLabel(tr("Fill Color: "), Tab2);
  gp2->addWidget(FillLabel1, 1,0);
  FillColorButt = new QPushButton("        ", Tab2);
  FillColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(FillColorButt, SIGNAL(clicked()), SLOT(slotSetFillColor()));
  gp2->addWidget(FillColorButt, 1,1);

  FillLabel2 = new QLabel(tr("Fill Style: "), Tab2);
  gp2->addWidget(FillLabel2, 2,0);
  FillStyleBox = new QComboBox(Tab2);
  FillStyleBox->insertItem(tr("no filling"));
  FillStyleBox->insertItem(tr("solid"));
  FillStyleBox->insertItem(tr("dense 1 (densest)"));
  FillStyleBox->insertItem(tr("dense 2"));
  FillStyleBox->insertItem(tr("dense 3"));
  FillStyleBox->insertItem(tr("dense 4"));
  FillStyleBox->insertItem(tr("dense 5"));
  FillStyleBox->insertItem(tr("dense 6"));
  FillStyleBox->insertItem(tr("dense 7 (least dense)"));
  FillStyleBox->insertItem(tr("horizontal line"));
  FillStyleBox->insertItem(tr("vertical line"));
  FillStyleBox->insertItem(tr("crossed lines"));
  FillStyleBox->insertItem(tr("hatched backwards"));
  FillStyleBox->insertItem(tr("hatched forwards"));
  FillStyleBox->insertItem(tr("diagonal crossed"));
  gp2->addWidget(FillStyleBox, 2,1);


  t->addTab(Tab2, tr("Filling Style"));
}
  // ...........................................................
  Q3HBox *Butts = new Q3HBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *ButtOK = new QPushButton(tr("OK"),Butts);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),Butts);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));


  ButtOK->setDefault(true);
//  ButtOK->setFocus();
}

FillDialog::~FillDialog()
{
  delete all;
  delete val100;
}

// --------------------------------------------------------------------------
void FillDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  if(c.isValid()) ColorButt->setPaletteBackgroundColor(c);
}

// --------------------------------------------------------------------------
void FillDialog::slotSetFillColor()
{
  QColor c =
    QColorDialog::getColor(FillColorButt->paletteBackgroundColor(), this);
  FillColorButt->setPaletteBackgroundColor(c);
}

// --------------------------------------------------------------------------
void FillDialog::slotCheckFilled(bool on)
{
  if(on) {
    FillLabel1->setEnabled(true);
    FillColorButt->setEnabled(true);
    FillLabel2->setEnabled(true);
    FillStyleBox->setEnabled(true);
  }
  else {
    FillLabel1->setEnabled(false);
    FillColorButt->setEnabled(false);
    FillLabel2->setEnabled(false);
    FillStyleBox->setEnabled(false);
  }
}
