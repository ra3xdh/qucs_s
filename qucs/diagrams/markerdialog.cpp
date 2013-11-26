/***************************************************************************
                          markerdialog.cpp  -  description
                             -------------------
    begin                : Wed April 21 2004
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
#include <QtGui>
#include "markerdialog.h"

#include <QLayout>
#include <QLabel>
#include <Q3HBox>
#include <QPushButton>
#include <QValidator>
//Added by qt3to4:
#include <Q3GridLayout>
#include "diagram.h"


MarkerDialog::MarkerDialog(Marker *pm_, QWidget *parent)
                     : QDialog(parent, 0, FALSE, Qt::WDestructiveClose)
{
  setCaption(tr("Edit Marker Properties"));
  pMarker = pm_;

  Q3GridLayout *g = new Q3GridLayout(this, 5,2,5,5);

  g->addWidget(new QLabel(tr("Precision: "), this), 0,0);
  Precision = new QLineEdit(this);
  Precision->setText(QString::number(pMarker->Precision));
  Precision->setValidator(new QIntValidator(0, 12, this));
  g->addWidget(Precision, 0, 1);
	if(pMarker->Diag->Name=="Smith") //S parameter also displayed as Z, need Z0 here
  {
		g->addWidget(new QLabel(tr("Z0: "), this), 2,0);
		SourceImpedance = new QLineEdit(this);
  	SourceImpedance->setText(QString::number(pMarker->Z0));
		g->addWidget(SourceImpedance,2,1);
	}
  g->addWidget(new QLabel(tr("Number Notation: "), this), 1,0);
  NumberBox = new QComboBox(this);
  NumberBox->insertItem(tr("real/imaginary"));
  NumberBox->insertItem(tr("magnitude/angle (degree)"));
  NumberBox->insertItem(tr("magnitude/angle (radian)"));
  NumberBox->setCurrentItem(pMarker->numMode);
  g->addWidget(NumberBox, 1, 1);
  
  TransBox = new QCheckBox(tr("transparent"), this);
  TransBox->setChecked(pMarker->transparent);
  g->addMultiCellWidget(TransBox,3,3,0,1);

  

  Q3HBox *b = new Q3HBox(this);
  b->setSpacing(5);
  g->addMultiCellWidget(b,4,4,0,1);

  // first => activated by pressing RETURN
  QPushButton *ButtOK = new QPushButton(tr("OK"),b);
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotAcceptValues()));

  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),b);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
}

MarkerDialog::~MarkerDialog()
{
}

// ----------------------------------------------------------
void MarkerDialog::slotAcceptValues()
{
  bool changed = false;
  int tmp = Precision->text().toInt();
  if(tmp != pMarker->Precision) {
    pMarker->Precision = tmp;
    changed = true;
  }
	if(pMarker->Diag->Name=="Smith")
	{
			double SrcImp = SourceImpedance->text().toDouble();
			if(SrcImp != pMarker->Z0)
			{
					pMarker->Z0 = SrcImp;
					changed = true;
			}
	}
  if(NumberBox->currentItem() != pMarker->numMode) {
    pMarker->numMode = NumberBox->currentItem();
    changed = true;
  }
  if(TransBox->isChecked() != pMarker->transparent) {
    pMarker->transparent = TransBox->isChecked();
    changed = true;
  }

  if(changed) {
    pMarker->createText();
    done(2);
  }
  else done(1);
}
