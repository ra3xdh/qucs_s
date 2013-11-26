/***************************************************************************
                              arrowdialog.cpp
                             -----------------
    begin                : Fri Nov 28 2003
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

#include "arrowdialog.h"

//#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QColorDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QWidget>
#include <QHBoxLayout>


ArrowDialog::ArrowDialog(QWidget *parent, const char *name)
                                  : QDialog(parent, name)
{
  setCaption(tr("Edit Arrow Properties"));
  val100 = new QIntValidator(0, 100, this);

  all = new QGridLayout(this, 5,4,3,3);
  all->setMargin(3);



  all->addWidget(new QLabel(tr("Head Length: "), this), 0,0);
  HeadLength = new QLineEdit(this);
  HeadLength->setValidator(val100);
  HeadLength->setMaximumWidth(35);
  HeadLength->setText("10");
  all->addWidget(HeadLength, 0,1);

  all->addWidget(new QLabel(tr("      Head Width: "), this), 0,2);
  HeadWidth = new QLineEdit(this);
  HeadWidth->setValidator(val100);
  HeadWidth->setMaximumWidth(35);
  HeadWidth->setText("10");
  all->addWidget(HeadWidth, 0,3);


  all->addWidget(new QLabel(tr("Line color: "), this), 1,0);
  ColorButt = new QPushButton("    ",this);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
  all->addWidget(ColorButt, 1,1);

  all->addWidget(new QLabel(tr("      Line Width: "), this), 1,2);
  LineWidth = new QLineEdit(this);
  LineWidth->setValidator(val100);
  LineWidth->setMaximumWidth(35);
  LineWidth->setText("0");
  all->addWidget(LineWidth, 1,3);


  all->addWidget(new QLabel(tr("Line style: "), this), 2,0);
  StyleBox = new QComboBox(this);
  StyleBox->insertItem(tr("solid line"));
  StyleBox->insertItem(tr("dash line"));
  StyleBox->insertItem(tr("dot line"));
  StyleBox->insertItem(tr("dash dot line"));
  StyleBox->insertItem(tr("dash dot dot line"));
  connect(StyleBox, SIGNAL(activated(int)), SLOT(slotSetStyle(int)));
  LineStyle = Qt::SolidLine;
  all->addMultiCellWidget(StyleBox, 2,2,1,2);

  all->addWidget(new QLabel(tr("Arrow head: "), this), 3,0);
  ArrowStyleBox = new QComboBox(this);
  ArrowStyleBox->insertItem(tr("two lines"));
  ArrowStyleBox->insertItem(tr("filled"));
  all->addMultiCellWidget(ArrowStyleBox, 3,3,1,2);


  QWidget *h1 = new QWidget(this);
  QHBoxLayout *h1Layout = new QHBoxLayout();

  
  QPushButton *ButtOK = new QPushButton(tr("OK"));
  h1Layout->addWidget(ButtOK);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"));
  h1Layout->addWidget(ButtCancel);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
  
  h1->setLayout(h1Layout);
  all->addMultiCellWidget(h1, 4,4,0,3);

  ButtOK->setFocus();
}

ArrowDialog::~ArrowDialog()
{
  delete all;
  delete val100;
}

// --------------------------------------------------------------------------
void ArrowDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  if(c.isValid()) ColorButt->setPaletteBackgroundColor(c);
}

// --------------------------------------------------------------------------
void ArrowDialog::slotSetStyle(int index)
{
  switch(index) {
    case 0 : LineStyle = Qt::SolidLine;
             break;
    case 1 : LineStyle = Qt::DashLine;
             break;
    case 2 : LineStyle = Qt::DotLine;
             break;
    case 3 : LineStyle = Qt::DashDotLine;
             break;
    case 4 : LineStyle = Qt::DashDotDotLine;
             break;
    default: ;
  }
}

// --------------------------------------------------------------------------
void ArrowDialog::SetComboBox(Qt::PenStyle _Style)
{
  LineStyle = _Style;
  switch(_Style) {
    case Qt::SolidLine      : StyleBox->setCurrentItem(0);
                              break;
    case Qt::DashLine       : StyleBox->setCurrentItem(1);
                              break;
    case Qt::DotLine        : StyleBox->setCurrentItem(2);
                              break;
    case Qt::DashDotLine    : StyleBox->setCurrentItem(3);
                              break;
    case Qt::DashDotDotLine : StyleBox->setCurrentItem(4);
                              break;
    default: ;
  }
}
