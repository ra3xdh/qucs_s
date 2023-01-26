/***************************************************************************
                               labeldialog.cpp
                              ----------------
    begin                : Thu Dec 09 2004
    copyright            : (C) 2004 by Michael Margraf
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
#include "labeldialog.h"
#include "../wirelabel.h"
#include "extsimkernels/spicecompat.h"
#include "main.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QGridLayout>
#include <QMessageBox>


LabelDialog::LabelDialog(WireLabel *pl, QWidget *parent)
                     : QDialog(parent) 
{
  setWindowTitle(tr("Insert Nodename"));

  pLabel = pl;
  gbox = new QGridLayout(this);

  // valid expression for LineEdit: alpha-numeric, but must start with
  // letter and never two "_" together
  Expr1.setPattern("[a-zA-Z]([0-9a-zA-Z]|_(?!_))+\\!{0,1}");
  Validator1 = new QRegularExpressionValidator(Expr1, this);

  QLabel *Label1 = new QLabel(tr("Enter the label:"));
  gbox->addWidget(Label1,0,0);

  NodeName = new QLineEdit();
  if(pLabel)  NodeName->setText(pLabel->Name);
  NodeName->setValidator(Validator1);
  gbox->addWidget(NodeName,1,0,1,3);

  Expr2.setPattern("[^\"=]+");    // valid expression for LineEdit
  Validator2 = new QRegularExpressionValidator(Expr2, this);

  Label2 = new QLabel(tr("Initial node voltage:"));
  gbox->addWidget(Label2,2,0);
  InitValue = new QLineEdit();
  if(pLabel)  InitValue->setText(pLabel->initValue);
  InitValue->setValidator(Validator2);
  gbox->addWidget(InitValue,2,1,1,2);

  ButtonMore = new QPushButton(tr("Less..."));
  gbox->addWidget(ButtonMore,3,0);
  ButtonOk = new QPushButton(tr("Ok"));
  gbox->addWidget(ButtonOk,3,1);
  ButtonCancel = new QPushButton(tr("Cancel"));
  gbox->addWidget(ButtonCancel,3,2);

  for(;;) {
    if(pLabel)  if(!pLabel->initValue.isEmpty())  break;
    Label2->hide();
    InitValue->hide();
    ButtonMore->setText(tr("More..."));
    break;
  }

  connect(ButtonMore, SIGNAL(clicked()), SLOT(slotExtend()));
  connect(ButtonOk, SIGNAL(clicked()), SLOT(slotOk()));
  connect(ButtonCancel, SIGNAL(clicked()), SLOT(slotCancel()));

  ButtonOk->setDefault(true);
  setFocusProxy(NodeName);
}

LabelDialog::~LabelDialog()
{
  delete gbox;
  delete Validator1;
  delete Validator2;
}

void LabelDialog::slotExtend()
{
  if(Label2->isHidden()) {
    Label2->setHidden(false);
    InitValue->setHidden(false);
    ButtonMore->setText(tr("Less..."));
  }
  else {
    Label2->hide();
    InitValue->hide();
    ButtonMore->setText(tr("More..."));
  }

}

void LabelDialog::slotCancel()
{
  done(0);
}

void LabelDialog::slotOk()
{
  if ((QucsSettings.DefaultSimulator == spicecompat::simNgspice)||
      (QucsSettings.DefaultSimulator == spicecompat::simSpiceOpus)) {
      QString nod = NodeName->text().trimmed();
      if (!spicecompat::check_nodename(nod)) {
          QMessageBox::warning(this,tr("SPICE checker"),
                               QString(tr("Node name \"%1\" is Nutmeg reserved keyword!\n"
                                          "Please select another node name!\n"
                                          "Node name will not be changed.")).arg(nod),
                               QMessageBox::Ok);
          slotCancel();
          return;
      }
  }

  NodeName->setText(NodeName->text().trimmed());
  InitValue->setText(InitValue->text().trimmed());

  bool changed = false;
  if(pLabel) {
    if(pLabel->Name != NodeName->text()) {
      pLabel->Name = NodeName->text();
      changed = true;
    }

    if(pLabel->initValue != InitValue->text()) {
      pLabel->initValue = InitValue->text();
      changed = true;
    }
  }

  if(changed) done(2);
  else done(1);
}
