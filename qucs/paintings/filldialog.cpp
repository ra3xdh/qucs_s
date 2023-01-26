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

#include <QLabel>
#include <QValidator>
#include <QColorDialog>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

#include "misc.h"

FillDialog::FillDialog(const QString& _Caption, bool show, QWidget *parent)
			: QDialog(parent)
{
  setWindowTitle(_Caption);

  all = new QVBoxLayout(this); // to provide the necessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp1 = new QGridLayout(Tab1);

  gp1->addWidget(new QLabel(tr("Line Width: "), Tab1), 0,0);
  val100 = new QIntValidator(0,100, this);
  LineWidth = new QLineEdit(Tab1);
  LineWidth->setValidator(val100);
  LineWidth->setMaximumWidth(35);
  LineWidth->setText("0");
  gp1->addWidget(LineWidth, 0,1);

  gp1->addWidget(new QLabel(tr("Line Color: "), Tab1), 1,0);
  ColorButt = new QPushButton("        ",Tab1);
  misc::setPickerColor(ColorButt,QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
  gp1->addWidget(ColorButt, 1,1);

  gp1->addWidget(new QLabel(tr("Line Style: "), Tab1), 2,0);
  StyleBox = new QComboBox(Tab1);
  StyleBox->addItem(tr("solid line"));
  StyleBox->addItem(tr("dash line"));
  StyleBox->addItem(tr("dot line"));
  StyleBox->addItem(tr("dash dot line"));
  StyleBox->addItem(tr("dash dot dot line"));
  gp1->addWidget(StyleBox, 2,1);


  t->addTab(Tab1, tr("Line Style"));

  // ...........................................................
if(show) {
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout(Tab2);

  CheckFilled = new QCheckBox(tr("enable filling"),Tab2);
  connect(CheckFilled, SIGNAL(toggled(bool)), SLOT(slotCheckFilled(bool)));
  gp2->addWidget(CheckFilled, 0, 0);


  FillLabel1 = new QLabel(tr("Fill Color: "), Tab2);
  gp2->addWidget(FillLabel1,1,0);
  FillColorButt = new QPushButton("        ", Tab2);
  misc::setPickerColor(FillColorButt,QColor(0,0,0));
  connect(FillColorButt, SIGNAL(clicked()), SLOT(slotSetFillColor()));
  gp2->addWidget(FillColorButt,1,1);

  FillLabel2 = new QLabel(tr("Fill Style: "), Tab2);
  gp2->addWidget(FillLabel2,2,0);
  FillStyleBox = new QComboBox(Tab2);
  FillStyleBox->addItem(tr("no filling"));
  FillStyleBox->addItem(tr("solid"));
  FillStyleBox->addItem(tr("dense 1 (densest)"));
  FillStyleBox->addItem(tr("dense 2"));
  FillStyleBox->addItem(tr("dense 3"));
  FillStyleBox->addItem(tr("dense 4"));
  FillStyleBox->addItem(tr("dense 5"));
  FillStyleBox->addItem(tr("dense 6"));
  FillStyleBox->addItem(tr("dense 7 (least dense)"));
  FillStyleBox->addItem(tr("horizontal line"));
  FillStyleBox->addItem(tr("vertical line"));
  FillStyleBox->addItem(tr("crossed lines"));
  FillStyleBox->addItem(tr("hatched backwards"));
  FillStyleBox->addItem(tr("hatched forwards"));
  FillStyleBox->addItem(tr("diagonal crossed"));
  gp2->addWidget(FillStyleBox,2,1);


  t->addTab(Tab2, tr("Filling Style"));
}
  // ...........................................................
  QWidget *Butts = new QWidget(this);
  QHBoxLayout *ButtsLayout = new QHBoxLayout();

  ButtsLayout->setSpacing(5);
  ButtsLayout->setContentsMargins(5,5,5,5);

  QPushButton *ButtOK = new QPushButton(tr("OK"));
  ButtsLayout->addWidget(ButtOK);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),Butts);
  ButtsLayout->addWidget(ButtCancel);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  Butts->setLayout(ButtsLayout);
  all->addWidget(Butts);


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
  QColor c = QColorDialog::getColor(misc::getWidgetBackgroundColor(ColorButt),this);
  if(c.isValid()) misc::setPickerColor(ColorButt,c);
}

// --------------------------------------------------------------------------
void FillDialog::slotSetFillColor()
{
  QColor c =
    QColorDialog::getColor(misc::getWidgetBackgroundColor(FillColorButt), this);
  if (c.isValid()) misc::setPickerColor(FillColorButt,c);
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
