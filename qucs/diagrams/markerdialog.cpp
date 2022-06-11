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
#include "markerdialog.h"
#include "diagram.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>


MarkerDialog::MarkerDialog(Marker *pm_, QWidget *parent)
                     : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("Edit Marker Properties"));
  pMarker = pm_;

  QGridLayout *g = new QGridLayout;

  Precision = new QLineEdit();
  Precision->setText(QString::number(pMarker->Precision));
  Precision->setValidator(new QIntValidator(0, 12, this));

  XPosition = new QLineEdit();
  if (pMarker->varPos().size() > 0) {
      XPosition->setText(QString::number(pMarker->varPos().at(0),
                         'g', pMarker->precision()));
  } else {
      XPosition->setText("0");
      XPosition->setEnabled(false);
  }
  XPosition->setValidator(new QDoubleValidator(this));

  g->addWidget(new QLabel(tr("Precision: ")), 0, 0);
  g->addWidget(Precision, 0, 1);

  NumberBox = new QComboBox();
  NumberBox->addItem(tr("real/imaginary"));
  NumberBox->addItem(tr("magnitude/angle (degree)"));
  NumberBox->addItem(tr("magnitude/angle (radian)"));
  NumberBox->setCurrentIndex(pMarker->numMode);

  g->addWidget(new QLabel(tr("Number Notation: ")), 1,0);
  g->addWidget(NumberBox, 1, 1);

  g->addWidget(new QLabel(tr("X-axis position:")), 2, 0);
  g->addWidget(XPosition, 2, 1);

  assert(pMarker->diag());
  if(pMarker->diag()->Name=="Smith") // BUG
  {
    //S parameter also displayed as Z, need Z0 here
		SourceImpedance = new QLineEdit();
  	SourceImpedance->setText(QString::number(pMarker->Z0));

        g->addWidget(new QLabel(tr("Z0: ")), 3,0);
        g->addWidget(SourceImpedance,3,1);
	}
  
  TransBox = new QCheckBox(tr("transparent"));
  TransBox->setChecked(pMarker->transparent);
  g->addWidget(TransBox,4,0);

  // first => activated by pressing RETURN
  QPushButton *ButtOK = new QPushButton(tr("OK"));
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotAcceptValues()));

  QPushButton *ButtCancel = new QPushButton(tr("Cancel"));
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  QHBoxLayout *b = new QHBoxLayout();
  b->setSpacing(5);
  b->addWidget(ButtOK);
  b->addWidget(ButtCancel);
  g->addLayout(b,5,0,1,2);

  this->setLayout(g);
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
  assert(pMarker->diag());
  if(pMarker->diag()->Name=="Smith") // BUG: need generic MarkerDialog.
	{
			double SrcImp = SourceImpedance->text().toDouble();
			if(SrcImp != pMarker->Z0)
			{
					pMarker->Z0 = SrcImp;
					changed = true;
			}
	}
  if(NumberBox->currentIndex() != pMarker->numMode) {
    pMarker->numMode = NumberBox->currentIndex();
    changed = true;
  }
  if(TransBox->isChecked() != pMarker->transparent) {
    pMarker->transparent = TransBox->isChecked();
    changed = true;
  }

  double xpos = XPosition->text().toDouble();
  if (xpos != pMarker->powFreq()) {
      pMarker->setPos(XPosition->text().toDouble());
      changed = true;
  }

  if(changed) {
    pMarker->createText();
    done(2);
  }
  else done(1);
}

// vim:ts=8:sw=2:noet
