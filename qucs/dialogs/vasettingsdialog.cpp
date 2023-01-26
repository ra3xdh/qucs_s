/***************************************************************************
                           vasettingsdialog.cpp
                          ----------------------
    begin                : Sun Oct 26 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefa@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QFileDialog>
#include <QGridLayout>
#include <QRadioButton>

#include "vasettingsdialog.h"
#include "textdoc.h"
#include "main.h"


VASettingsDialog::VASettingsDialog (TextDoc * Doc_)
  : QDialog (Doc_)
{
  Doc = Doc_;
  setWindowTitle(tr("Document Settings"));

  QString Module = Doc->getModuleName ();

  Expr.setPattern("[0-9a-zA-Z /\\]+"); // valid expression for IconEdit
  Validator = new QRegularExpressionValidator (Expr, this);

  vLayout = new QVBoxLayout(this);
  
  QGroupBox * codeGroup = new QGroupBox (tr("Code Creation Settings"));
  vLayout->addWidget(codeGroup);
  QVBoxLayout *vbox = new QVBoxLayout();
  codeGroup->setLayout(vbox);
  
  QGridLayout * all = new QGridLayout ();
  vbox->addLayout(all);
  
  if (Doc->Icon.isEmpty ())
    Doc->Icon = Module + ".png";

  IconButt = new QLabel ();
  IconButt->setPixmap (QPixmap (Doc->Icon));
  all->addWidget (IconButt, 0, 0, 1, 1);

  IconEdit = new QLineEdit ();
  IconEdit->setValidator (Validator);
  IconEdit->setText (Doc->Icon);
  IconEdit->setCursorPosition (0);
  all->addWidget (IconEdit, 0, 1, 1, 3);
  
  BrowseButt = new QPushButton (tr("Browse"));
  connect (BrowseButt, SIGNAL (clicked()), SLOT (slotBrowse()));
  all->addWidget (BrowseButt, 0, 4, 1, 1);

  QLabel * l1 = new QLabel (tr("Output file:"));
  l1->setAlignment (Qt::AlignRight);
  all->addWidget (l1, 1, 0, 1, 1);
  OutputEdit = new QLineEdit ();
  OutputEdit->setText (Module + ".cpp");
  all->addWidget (OutputEdit, 1, 1, 1, 3);

  RecreateCheck = new QCheckBox (tr("Recreate"));
  all->addWidget (RecreateCheck, 1, 4, 1, 1);
  RecreateCheck->setChecked (Doc->recreate);

  if (Doc->ShortDesc.isEmpty ())
    Doc->ShortDesc = Module;

  QLabel * l2 = new QLabel (tr("Icon description:"));
  l2->setAlignment (Qt::AlignRight);
  all->addWidget (l2, 2, 0);
  ShortDescEdit = new QLineEdit ();
  ShortDescEdit->setText (Doc->ShortDesc);
  all->addWidget (ShortDescEdit, 2, 1, 1, 3);
  
  if (Doc->LongDesc.isEmpty ())
    Doc->LongDesc = Module + " verilog device";

  QLabel * l3 = new QLabel (tr("Description:"));
  l3->setAlignment (Qt::AlignRight);
  all->addWidget (l3, 3, 0);
  LongDescEdit = new QLineEdit ();
  LongDescEdit->setText (Doc->LongDesc);
  all->addWidget (LongDescEdit, 3, 1, 1, 3);

  toggleGroupDev = new QButtonGroup ();
  QRadioButton * nonRadio =
    new QRadioButton (tr("unspecified device"));
  QRadioButton * bjtRadio = 
    new QRadioButton (tr("NPN/PNP polarity"));
  QRadioButton * mosRadio =
    new QRadioButton (tr("NMOS/PMOS polarity"));
  toggleGroupDev->addButton(nonRadio, 0);
  toggleGroupDev->addButton(bjtRadio, DEV_BJT);
  toggleGroupDev->addButton(mosRadio, DEV_MOS);
  if (Doc->devtype & DEV_BJT)
    bjtRadio->setChecked (true);
  else if (Doc->devtype & DEV_MOS)
    mosRadio->setChecked (true);
  else
    nonRadio->setChecked (true);
  all->addWidget (nonRadio, 4, 4, 0, 1);
  all->addWidget (bjtRadio, 4, 2);
  all->addWidget (mosRadio, 4, 4, 3, 4);

  toggleGroupTyp = new QButtonGroup ();
  QRadioButton * anaRadio = 
    new QRadioButton (tr("analog only"));
  QRadioButton * digRadio =
    new QRadioButton (tr("digital only"));
  QRadioButton * allRadio =
    new QRadioButton (tr("both"));
  toggleGroupTyp->addButton(digRadio, DEV_DIG);
  toggleGroupTyp->addButton(anaRadio, DEV_ANA);
  toggleGroupTyp->addButton(allRadio, DEV_ALL);
  if ((Doc->devtype & DEV_ALL) == DEV_ALL)
    allRadio->setChecked (true);
  else if (Doc->devtype & DEV_ANA)
    anaRadio->setChecked (true);
  else
    digRadio->setChecked (true);
  all->addWidget (anaRadio, 5, 5, 0, 1);
  all->addWidget (allRadio, 5, 2);
  all->addWidget (digRadio, 5, 5, 3, 4);

  QHBoxLayout * Buttons = new QHBoxLayout ();
  vbox->addLayout(Buttons);
  QPushButton * ButtonOk = new QPushButton (tr("Ok"));
  Buttons->addWidget(ButtonOk);
  QPushButton * ButtonCancel = new QPushButton (tr("Cancel"));
  Buttons->addWidget(ButtonCancel);
  connect (ButtonOk, SIGNAL(clicked()), SLOT(slotOk()));
  connect (ButtonCancel, SIGNAL(clicked()), SLOT(reject()));
  ButtonOk->setDefault(true);
  
}

VASettingsDialog::~VASettingsDialog ()
{
  delete Validator;
}

void VASettingsDialog::slotOk ()
{
  bool changed = false;

  if (Doc->Icon != IconEdit->text ()) {
    Doc->Icon = IconEdit->text ();
    changed = true;
  }
  if (Doc->ShortDesc != ShortDescEdit->text ()) {
    Doc->ShortDesc = ShortDescEdit->text ();
    changed = true;
  }
  if (Doc->LongDesc != LongDescEdit->text ()) {
    Doc->LongDesc = LongDescEdit->text ();
    changed = true;
  }
  if (Doc->DataSet != OutputEdit->text ()) {
    Doc->DataSet = OutputEdit->text ();
    changed = true;
  }
  if (Doc->recreate != RecreateCheck->isChecked ()) {
    Doc->recreate = RecreateCheck->isChecked ();
    changed = true;
  }
  if ((Doc->devtype & DEV_MASK_TYP) != toggleGroupTyp->checkedId()) {
    Doc->devtype &= ~DEV_MASK_TYP;
    Doc->devtype |= toggleGroupTyp->checkedId();
    changed = true;
  }
  if ((Doc->devtype & DEV_MASK_DEV) != toggleGroupDev->checkedId()) {
    Doc->devtype &= ~DEV_MASK_DEV;
    Doc->devtype |= toggleGroupDev->checkedId();
    changed = true;
  }

  if (changed) {
    Doc->SetChanged = true;
    Doc->slotSetChanged ();
  }
  accept ();
}

void VASettingsDialog::slotBrowse ()
{
  QString s = QFileDialog::getOpenFileName (
     this, tr("Enter an Icon File Name"),
     lastDir.isEmpty () ? QString (".") : lastDir,
     tr("PNG files")+" (*.png);;"+
     tr("Any file")+" (*)"
     );

  if (!s.isEmpty ()) {
    QFileInfo Info (s);
    lastDir = Info.absolutePath();  // remember last directory
    IconEdit->setText (s);
    IconButt->setPixmap (QPixmap (s));
  }
}
