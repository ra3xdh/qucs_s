/***************************************************************************
                             settingsdialog.cpp
                            --------------------
    begin                : Mon Oct 20 2003
    copyright            : (C) 2003, 2004 by Michael Margraf
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

#include "settingsdialog.h"

#include "node.h"
#include "qucs.h"
#include "mnemo.h"
#include "schematic.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qvalidator.h>
#include <qpushbutton.h>


SettingsDialog::SettingsDialog(Schematic *Doc_)
                : QDialog(Doc_, 0, TRUE, Qt::WDestructiveClose)
{
  Doc = Doc_;
  setCaption(tr("Edit File Properties"));

  all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
<<<<<<< HEAD
  QGridLayout *gp = new QGridLayout(Tab1,4,2,5,5);

  QLabel *l1 = new QLabel(tr("Data Display:"), Tab1);
  gp->addWidget(l1,0,0);
  Input_DataDisplay = new QLineEdit(Tab1);
  gp->addWidget(Input_DataDisplay,0,1);

  QLabel *l2 = new QLabel(tr("Data Set:"), Tab1);
  gp->addWidget(l2,1,0);
  Input_DataSet = new QLineEdit(Tab1);
  gp->addWidget(Input_DataSet,1,1);
=======
  QGridLayout *gp = new QGridLayout(Tab1,6,2,5,5);

  QLabel *l1 = new QLabel(tr("Data Display:"), Tab1);
  gp->addWidget(l1,1,0);
  Input_DataDisplay = new QLineEdit(Tab1);
  gp->addWidget(Input_DataDisplay,1,1);

  QLabel *l2 = new QLabel(tr("Data Set:"), Tab1);
  gp->addWidget(l2,0,0);
  Input_DataSet = new QLineEdit(Tab1);
  gp->addWidget(Input_DataSet,0,1);
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d

  Check_OpenDpl = new QCheckBox(tr("open data display after simulation"),
				Tab1);
  gp->addMultiCellWidget(Check_OpenDpl,2,2,0,1);

<<<<<<< HEAD
=======
  QLabel *l20 = new QLabel(tr("Octave Script:"), Tab1);
  gp->addWidget(l20,3,0);
  Input_Script = new QLineEdit(Tab1);
  gp->addWidget(Input_Script,3,1);

  Check_RunScript = new QCheckBox(tr("run script after simulation"),
				Tab1);
  gp->addMultiCellWidget(Check_RunScript,4,4,0,1);

>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d
  t->addTab(Tab1, tr("Simulation"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout(Tab2,4,2,5,5);
  Check_GridOn = new QCheckBox(tr("show Grid"),Tab2);
  gp2->addMultiCellWidget(Check_GridOn,0,0,0,1);

  valExpr = new QRegExpValidator(QRegExp("[1-9]\\d{0,2}"), this);

  QLabel *l3 = new QLabel(tr("horizontal Grid:"), Tab2);
  gp2->addWidget(l3,1,0);
  Input_GridX = new QLineEdit(Tab2);
  Input_GridX->setValidator(valExpr);
  gp2->addWidget(Input_GridX,1,1);

  QLabel *l4 = new QLabel(tr("vertical Grid:"), Tab2);
  gp2->addWidget(l4,2,0);
  Input_GridY = new QLineEdit(Tab2);
  Input_GridY->setValidator(valExpr);
  gp2->addWidget(Input_GridY,2,1);

  t->addTab(Tab2, tr("Grid"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  QGridLayout *gp3 = new QGridLayout(Tab3,5,2,5,5);
  Combo_Frame = new QComboBox(Tab3);
  Combo_Frame->insertItem(tr("no Frame"));
  Combo_Frame->insertItem(tr("DIN A5 landscape"));
  Combo_Frame->insertItem(tr("DIN A5 portrait"));
  Combo_Frame->insertItem(tr("DIN A4 landscape"));
  Combo_Frame->insertItem(tr("DIN A4 portrait"));
  Combo_Frame->insertItem(tr("DIN A3 landscape"));
  Combo_Frame->insertItem(tr("DIN A3 portrait"));
  Combo_Frame->insertItem(tr("Letter landscape"));
  Combo_Frame->insertItem(tr("Letter portrait"));
  gp3->addMultiCellWidget(Combo_Frame,0,0,0,1);

  Input_Frame0 = new QTextEdit(Tab3);
  Input_Frame0->setTextFormat(Qt::PlainText);
  Input_Frame0->setWordWrap(QTextEdit::NoWrap);
  gp3->addMultiCellWidget(Input_Frame0,1,2,0,1);

  Input_Frame1 = new QLineEdit(Tab3);
  gp3->addMultiCellWidget(Input_Frame1,3,3,0,1);

  Input_Frame2 = new QLineEdit(Tab3);
  gp3->addWidget(Input_Frame2,4,0);
  Input_Frame3 = new QLineEdit(Tab3);
  gp3->addWidget(Input_Frame3,4,1);

  t->addTab(Tab3, tr("Frame"));

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBox *Butts = new QHBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"), Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"), Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));

  OkButt->setDefault(true);

  // ...........................................................
  // fill the fields with the QucsDoc-Properties

  Input_DataSet->setText(Doc->DataSet);
  Input_DataDisplay->setText(Doc->DataDisplay);
<<<<<<< HEAD
  Check_OpenDpl->setChecked(Doc->SimOpenDpl);
=======
  Input_Script->setText(Doc->Script);
  Check_OpenDpl->setChecked(Doc->SimOpenDpl);
  Check_RunScript->setChecked(Doc->SimRunScript);
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d
  Check_GridOn->setChecked(Doc->GridOn);
  Input_GridX->setText(QString::number(Doc->GridX));
  Input_GridY->setText(QString::number(Doc->GridY));
  Combo_Frame->setCurrentItem(Doc->showFrame);

  QString Text_;
  decode_String(Text_ = Doc->Frame_Text0);
  Input_Frame0->setText(Text_);
  decode_String(Text_ = Doc->Frame_Text1);
  Input_Frame1->setText(Text_);
  decode_String(Text_ = Doc->Frame_Text2);
  Input_Frame2->setText(Text_);
  decode_String(Text_ = Doc->Frame_Text3);
  Input_Frame3->setText(Text_);

  resize(250, 200);
}

SettingsDialog::~SettingsDialog()
{
  delete all;
  delete valExpr;
}

// -----------------------------------------------------------
void SettingsDialog::slotOK()
{
  slotApply();
  accept();
}

// -----------------------------------------------------------
void SettingsDialog::slotApply()
{
  bool changed = false;
  
  if(Doc->DataSet != Input_DataSet->text()) {
    Doc->DataSet = Input_DataSet->text();
    changed = true;
  }

  if(Doc->DataDisplay != Input_DataDisplay->text()) {
    Doc->DataDisplay = Input_DataDisplay->text();
    changed = true;
  }

<<<<<<< HEAD
=======
  if(Doc->Script != Input_Script->text()) {
    Doc->Script = Input_Script->text();
    changed = true;
  }

>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d
  if(Doc->SimOpenDpl != Check_OpenDpl->isChecked()) {
    Doc->SimOpenDpl = Check_OpenDpl->isChecked();
    changed = true;
  }

<<<<<<< HEAD
=======
  if(Doc->SimRunScript != Check_RunScript->isChecked()) {
    Doc->SimRunScript = Check_RunScript->isChecked();
    changed = true;
  }

>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d
  if(Doc->GridOn != Check_GridOn->isChecked()) {
    Doc->GridOn = Check_GridOn->isChecked();
    changed = true;
  }

  if(Doc->GridX != Input_GridX->text()) {
    Doc->GridX = Input_GridX->text().toInt();
    changed = true;
  }

  if(Doc->GridY != Input_GridY->text()) {
    Doc->GridY = Input_GridY->text().toInt();
    changed = true;
  }

  if(Doc->showFrame != Combo_Frame->currentItem()) {
    Doc->showFrame = Combo_Frame->currentItem();
    changed = true;
  }

  QString t;
  encode_String(Input_Frame0->text(), t);
  if(Doc->Frame_Text0 != t) {
    Doc->Frame_Text0 = t;
    changed = true;
  }

  encode_String(Input_Frame1->text(), t);
  if(Doc->Frame_Text1 != t) {
    Doc->Frame_Text1 = t;
    changed = true;
  }

  encode_String(Input_Frame2->text(), t);
  if(Doc->Frame_Text2 != t) {
    Doc->Frame_Text2 = t;
    changed = true;
  }

  encode_String(Input_Frame3->text(), t);
  if(Doc->Frame_Text3 != t) {
    Doc->Frame_Text3 = t;
    changed = true;
  }

  if(changed) {
    Doc->setChanged(true);
    Doc->viewport()->repaint();
  }
}
