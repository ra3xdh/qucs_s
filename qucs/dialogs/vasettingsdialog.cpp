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

#include <q3hbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <q3vgroupbox.h>
#include <qstring.h>
#include <qstringlist.h>
#include <q3button.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qradiobutton.h>
#include <qfileinfo.h>
#include <q3filedialog.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include "vasettingsdialog.h"
#include "textdoc.h"
#include "main.h"


VASettingsDialog::VASettingsDialog (TextDoc * Doc_)
  : QDialog (Doc_, 0, true, Qt::WDestructiveClose)
{
  Doc = Doc_;
  setCaption (tr("Document Settings"));

  QString Module = Doc->getModuleName ();

  Expr.setPattern("[0-9a-zA-Z /\\]+"); // valid expression for IconEdit
  Validator = new QRegExpValidator (Expr, this);

  Q3GridLayout * box = new Q3GridLayout (this, 1, 1, 5);

  Q3VGroupBox * setGroup = new Q3VGroupBox (tr("Code Creation Settings"), this);
  box->addWidget (setGroup, 0, 0);
  QWidget * f = new QWidget (setGroup);
  Q3GridLayout * all = new Q3GridLayout (f, 7, 5, 5);

  if (Doc->Icon.isEmpty ())
    Doc->Icon = Module + ".png";

  IconButt = new QLabel (f);
  IconButt->setPixmap (QPixmap (Doc->Icon));
  all->addWidget (IconButt, 0, 0);

  IconEdit = new QLineEdit (f);
  IconEdit->setValidator (Validator);
  IconEdit->setText (Doc->Icon);
  IconEdit->setCursorPosition (0);
  all->addMultiCellWidget (IconEdit, 0, 0, 1, 3);
  
  BrowseButt = new QPushButton (tr("Browse"), f);
  connect (BrowseButt, SIGNAL (clicked()), SLOT (slotBrowse()));
  all->addWidget (BrowseButt, 0, 4);

  QLabel * l1 = new QLabel (tr("Output file:"), f);
  l1->setAlignment (Qt::AlignRight);
  all->addWidget (l1, 1, 0);
  OutputEdit = new QLineEdit (f);
  OutputEdit->setText (Module + ".cpp");
  all->addMultiCellWidget (OutputEdit, 1, 1, 1, 3);

  RecreateCheck = new QCheckBox (tr("Recreate"), f);
  all->addWidget (RecreateCheck, 1, 4);
  RecreateCheck->setChecked (Doc->recreate);

  if (Doc->ShortDesc.isEmpty ())
    Doc->ShortDesc = Module;

  QLabel * l2 = new QLabel (tr("Icon description:"), f);
  l2->setAlignment (Qt::AlignRight);
  all->addWidget (l2, 2, 0);
  ShortDescEdit = new QLineEdit (f);
  ShortDescEdit->setText (Doc->ShortDesc);
  all->addMultiCellWidget (ShortDescEdit, 2, 2, 1, 4);
  
  if (Doc->LongDesc.isEmpty ())
    Doc->LongDesc = Module + " verilog device";

  QLabel * l3 = new QLabel (tr("Description:"), f);
  l3->setAlignment (Qt::AlignRight);
  all->addWidget (l3, 3, 0);
  LongDescEdit = new QLineEdit (f);
  LongDescEdit->setText (Doc->LongDesc);
  all->addMultiCellWidget (LongDescEdit, 3, 3, 1, 4);

  toggleGroupDev = new Q3ButtonGroup ();
  QRadioButton * nonRadio =
    new QRadioButton (tr("unspecified device"), f);
  QRadioButton * bjtRadio = 
    new QRadioButton (tr("NPN/PNP polarity"), f);
  QRadioButton * mosRadio =
    new QRadioButton (tr("NMOS/PMOS polarity"), f);
  toggleGroupDev->insert (nonRadio, 0);
  toggleGroupDev->insert (bjtRadio, DEV_BJT);
  toggleGroupDev->insert (mosRadio, DEV_MOS);
  if (Doc->devtype & DEV_BJT)
    bjtRadio->setChecked (true);
  else if (Doc->devtype & DEV_MOS)
    mosRadio->setChecked (true);
  else
    nonRadio->setChecked (true);
  all->addMultiCellWidget (nonRadio, 4, 4, 0, 1);
  all->addWidget (bjtRadio, 4, 2);
  all->addMultiCellWidget (mosRadio, 4, 4, 3, 4);

  toggleGroupTyp = new Q3ButtonGroup ();
  QRadioButton * anaRadio = 
    new QRadioButton (tr("analog only"), f);
  QRadioButton * digRadio =
    new QRadioButton (tr("digital only"), f);
  QRadioButton * allRadio =
    new QRadioButton (tr("both"), f);
  toggleGroupTyp->insert (digRadio, DEV_DIG);
  toggleGroupTyp->insert (anaRadio, DEV_ANA);
  toggleGroupTyp->insert (allRadio, DEV_ALL);
  if ((Doc->devtype & DEV_ALL) == DEV_ALL)
    allRadio->setChecked (true);
  else if (Doc->devtype & DEV_ANA)
    anaRadio->setChecked (true);
  else
    digRadio->setChecked (true);
  all->addMultiCellWidget (anaRadio, 5, 5, 0, 1);
  all->addWidget (allRadio, 5, 2);
  all->addMultiCellWidget (digRadio, 5, 5, 3, 4);

  Q3HBox * Buttons = new Q3HBox (f);
  all->addMultiCellWidget (Buttons, 6, 6, 0, 4);
  QPushButton * ButtonOk = new QPushButton (tr("Ok"), Buttons);
  QPushButton * ButtonCancel = new QPushButton (tr("Cancel"), Buttons);
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
  if ((Doc->devtype & DEV_MASK_TYP) != toggleGroupTyp->selectedId ()) {
    Doc->devtype &= ~DEV_MASK_TYP;
    Doc->devtype |= toggleGroupTyp->selectedId ();
    changed = true;
  }
  if ((Doc->devtype & DEV_MASK_DEV) != toggleGroupDev->selectedId ()) {
    Doc->devtype &= ~DEV_MASK_DEV;
    Doc->devtype |= toggleGroupDev->selectedId ();
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
  QString s = Q3FileDialog::getOpenFileName (
     lastDir.isEmpty () ? QString (".") : lastDir,
     tr("PNG files")+" (*.png);;"+
     tr("Any file")+" (*)",
     this, 0, tr("Enter an Icon File Name"));

  if (!s.isEmpty ()) {
    QFileInfo Info (s);
    lastDir = Info.dirPath (true);  // remember last directory
    IconEdit->setText (s);
    IconButt->setPixmap (QPixmap (s));
  }
}
