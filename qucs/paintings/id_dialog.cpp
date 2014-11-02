/***************************************************************************
                               id_dialog.cpp
                              ---------------
    begin                : Sat Oct 16 2004
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
#include <QtGui>
#include "id_dialog.h"
#include "id_text.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <Q3ListView>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>


ID_Dialog::ID_Dialog(ID_Text *idText_)
{
  idText = idText_;
  setCaption(tr("Edit Subcircuit Properties"));

  all = new QVBoxLayout;
  all->setSpacing(5);
  all->setMargin(5);

  QHBoxLayout *htop = new QHBoxLayout;
  htop->setSpacing(5);
  all->addLayout(htop);

  Expr.setPattern("[A-Za-z][A-Za-z0-9_]*");
  SubVal = new QRegExpValidator(Expr, this);
  Prefix = new QLineEdit(idText->Prefix);
  Prefix->setValidator(SubVal);

  htop->addWidget(new QLabel(tr("Prefix:")));
  htop->addWidget(Prefix);

  QGroupBox *ParamBox = new QGroupBox(tr("Parameters"));
  all->addWidget(ParamBox);
  QVBoxLayout *vbox_param = new QVBoxLayout;
  ParamBox->setLayout(vbox_param);

  ParamList = new Q3ListView();
  ParamList->addColumn(tr("display"));
  ParamList->addColumn(tr("Name"));
  ParamList->addColumn(tr("Default"));
  ParamList->addColumn(tr("Description"));
  ParamList->addColumn(tr("Type"));
  ParamList->setSorting(-1);   // no sorting
  vbox_param->addWidget(ParamList);

  SubParameter *pp;
  for(pp = idText->Parameter.last(); pp!=0; pp = idText->Parameter.prev())
    new Q3ListViewItem(ParamList,
      pp->display ? tr("yes") : tr("no"), pp->Name.section('=', 0,0),
      pp->Name.section('=', 1,1), pp->Description, pp->Type);

  connect(ParamList, SIGNAL(selectionChanged(Q3ListViewItem*)),
                     SLOT(slotEditParameter(Q3ListViewItem*)));

  showCheck = new QCheckBox(tr("display in schematic"));
  showCheck->setChecked(true);
  connect(showCheck, SIGNAL(toggled(bool)), SLOT(slotToggleShow(bool)));

  vbox_param->addWidget(showCheck);

  QGridLayout *paramEditLayout = new QGridLayout;
  vbox_param->addLayout(paramEditLayout);

  paramEditLayout->addWidget(new QLabel(tr("Name:")), 0, 0);
  paramEditLayout->addWidget(new QLabel(tr("Default Value:")), 1, 0);
  paramEditLayout->addWidget(new QLabel(tr("Description:")), 2, 0);
  paramEditLayout->addWidget(new QLabel(tr("Type:")), 3, 0);

  Expr.setPattern("[\\w_]+");
  NameVal = new QRegExpValidator(Expr, this);
  ParamNameEdit = new QLineEdit;
  ParamNameEdit->setValidator(NameVal);
  connect(ParamNameEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotNameChanged(const QString&)));

  Expr.setPattern("[^\"=]*");
  ValueVal = new QRegExpValidator(Expr, this);
  ValueEdit = new QLineEdit;
  ValueEdit->setValidator(ValueVal);
  connect(ValueEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotValueChanged(const QString&)));

  Expr.setPattern("[^\"=\\x005B\\x005D]*");
  DescrVal = new QRegExpValidator(Expr, this);
  DescriptionEdit = new QLineEdit;
  DescriptionEdit->setValidator(DescrVal);
  connect(DescriptionEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotDescrChanged(const QString&)));

  Expr.setPattern("[\\w_]+");
  TypeVal = new QRegExpValidator(Expr, this);
  TypeEdit = new QLineEdit;
  TypeEdit->setValidator(TypeVal);
  connect(TypeEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotTypeChanged(const QString&)));

  paramEditLayout->addWidget(ParamNameEdit, 0, 1);
  paramEditLayout->addWidget(ValueEdit, 1, 1);
  paramEditLayout->addWidget(DescriptionEdit, 2, 1);
  paramEditLayout->addWidget(TypeEdit, 3, 1);

  QPushButton *ButtAdd = new QPushButton(tr("Add"));
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotAddParameter()));
  QPushButton *ButtRemove = new QPushButton(tr("Remove"));
  connect(ButtRemove, SIGNAL(clicked()), SLOT(slotRemoveParameter()));

  QHBoxLayout *hbox_paramedit = new QHBoxLayout;
  vbox_param->addLayout(hbox_paramedit);
  hbox_paramedit->addStretch();
  hbox_paramedit->addWidget(ButtAdd);
  hbox_paramedit->addWidget(ButtRemove);

  QPushButton *ButtOK = new QPushButton(tr("OK"));
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotOk()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"));
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  QHBoxLayout *hbox_bottom = new QHBoxLayout;
  hbox_bottom->setSpacing(5);
  all->addLayout(hbox_bottom);
  hbox_bottom->addWidget(ButtOK);
  hbox_bottom->addWidget(ButtCancel);

  this->setLayout(all);
  resize(320, 350);
}

ID_Dialog::~ID_Dialog()
{
  delete all;
  delete SubVal;
  delete NameVal;
  delete ValueVal;
  delete DescrVal;
  delete TypeVal;
}

// -----------------------------------------------------------
void ID_Dialog::slotEditParameter(Q3ListViewItem *Item)
{
  if(Item == 0) {
    ParamList->clearSelection();
    ParamNameEdit->clear();
    showCheck->setChecked(true);
    ValueEdit->clear();
    DescriptionEdit->clear();
    TypeEdit->clear();
    return;
  }

  showCheck->setChecked(Item->text(0) == tr("yes"));
  ParamNameEdit->setText(Item->text(1));
  ValueEdit->setText(Item->text(2));
  DescriptionEdit->setText(Item->text(3));
  TypeEdit->setText(Item->text(4));
}

// -----------------------------------------------------------
void ID_Dialog::slotAddParameter()
{
  if(ParamNameEdit->text().isEmpty())
    return;

  if(ParamNameEdit->text() == "File") {
    QMessageBox::critical(this, tr("Error"),
       tr("Parameter must not be named \"File\"!"));
    return;
  }

  Q3ListViewItem *item, *lastItem=0;
  for(item = ParamList->firstChild(); item!=0; item = item->itemBelow()) {
    if(item->text(1) == ParamNameEdit->text()) {
      QMessageBox::critical(this, tr("Error"),
         tr("Parameter \"%1\" already in list!").arg(ParamNameEdit->text()));
      return;
    }
    lastItem = item;
  }


  new Q3ListViewItem(ParamList, lastItem,
      showCheck->isChecked() ? tr("yes") : tr("no"), ParamNameEdit->text(),
      ValueEdit->text(), DescriptionEdit->text(), TypeEdit->text());

  slotEditParameter(0);   // clear entry fields
  ParamList->clearSelection();
}

// -----------------------------------------------------------
void ID_Dialog::slotRemoveParameter()
{
  Q3ListViewItem *nextItem = 0;

  Q3ListViewItem *Item = ParamList->selectedItem();
  if(Item) {
    nextItem = Item->itemBelow();
    if(nextItem == 0)  nextItem = Item->itemAbove();
    ParamList->takeItem(Item);  // remove from ListView
    delete Item;                // delete item
  }

  slotEditParameter(nextItem);
}

// -----------------------------------------------------------
void ID_Dialog::slotToggleShow(bool On)
{
  Q3ListViewItem *Item = ParamList->selectedItem();
  if(Item == 0) return;

  Item->setText(0, On ? tr("yes") : tr("no"));
}

// -----------------------------------------------------------
void ID_Dialog::slotNameChanged(const QString& text)
{
  Q3ListViewItem *Item = ParamList->selectedItem();
  if(Item == 0) return;

  Item->setText(1, text);
}

// -----------------------------------------------------------
void ID_Dialog::slotValueChanged(const QString& text)
{
  Q3ListViewItem *Item = ParamList->selectedItem();
  if(Item == 0) return;

  Item->setText(2, text);
}

// -----------------------------------------------------------
void ID_Dialog::slotDescrChanged(const QString& text)
{
  Q3ListViewItem *Item = ParamList->selectedItem();
  if(Item == 0) return;

  Item->setText(3, text);
}

// -----------------------------------------------------------
void ID_Dialog::slotTypeChanged(const QString& text)
{
  Q3ListViewItem *Item = ParamList->selectedItem();
  if(Item == 0) return;

  Item->setText(4, text);
}

// -----------------------------------------------------------
void ID_Dialog::slotOk()
{
  bool changed = false;

  if(!Prefix->text().isEmpty())
    if(idText->Prefix != Prefix->text()) {
      idText->Prefix = Prefix->text();
      changed = true;
    }

  QString s;
  Q3ListViewItem *item;
  SubParameter *pp = idText->Parameter.first();
  for(item = ParamList->firstChild(); item != 0; item = item->itemBelow()) {
    s = item->text(1) + "=" + item->text(2);

    if(pp) {
      if(pp->display != (item->text(0) == tr("yes"))) {
        pp->display = (item->text(0) == tr("yes"));
        changed = true;
      }
      if(pp->Name != s) {
        pp->Name = s;
        changed = true;
      }
      if(pp->Description != item->text(3)) {
        pp->Description = item->text(3);
        changed = true;
      }
      if(pp->Type != item->text(4)) {
        pp->Type = item->text(4);
        changed = true;
      }
    }
    else {
      idText->Parameter.append(new SubParameter(
         (item->text(0) == tr("yes")) ? true : false, s, item->text(3),
	 item->text(4)));
      changed = true;
    }

    pp = idText->Parameter.next();
  }

  // if more properties than in ListView -> delete the rest
  if(pp) {
    pp = idText->Parameter.prev();
    idText->Parameter.last();
    while(pp != idText->Parameter.current())
      idText->Parameter.remove();
    changed = true;
  }

  if(changed)  accept();
  else  reject();
}
