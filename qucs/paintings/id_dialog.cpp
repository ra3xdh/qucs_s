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
#include "id_dialog.h"
#include "id_text.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>


ID_Dialog::ID_Dialog(ID_Text *idText_)
{
  idText = idText_;
  setWindowTitle(tr("Edit Subcircuit Properties"));

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

  ParamTable = new QTableWidget();
  ParamTable->horizontalHeader()->setStretchLastSection(true);
  ParamTable->verticalHeader()->hide();
  ParamTable->setColumnCount(5);
  ParamTable->setHorizontalHeaderLabels(
      QStringList() << tr("display") << tr("Name") << tr("Default") << tr("Description") << tr("Type"));
  ParamTable->setSortingEnabled(false); // no sorting
  ParamTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  vbox_param->addWidget(ParamTable);

  QTableWidgetItem *item;
  QList<SubParameter *>::const_iterator it;
  for(it = idText->Parameter.constBegin(); it != idText->Parameter.constEnd(); it++) {
    int row = ParamTable->rowCount();
    ParamTable->insertRow(row);
    item = new QTableWidgetItem(((*it)->display)? tr("yes") : tr("no"));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 0, item);
    item = new QTableWidgetItem((*it)->Name.section('=', 0, 0));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 1, item);
    item = new QTableWidgetItem((*it)->Name.section('=', 1, 1));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 2, item);
    item = new QTableWidgetItem((*it)->Description);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 3, item);
    item = new QTableWidgetItem((*it)->Type);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 4, item);
  }
  connect(ParamTable, SIGNAL(currentCellChanged(int, int, int, int)), SLOT(slotEditParameter()));

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
  resize(320, 400);
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
void ID_Dialog::slotEditParameter()
{
  int row = ParamTable->currentRow();
  if (row < 0 || row >= ParamTable->rowCount()) {
    ParamTable->clearSelection();
    ParamNameEdit->clear();
    showCheck->setChecked(true);
    ValueEdit->clear();
    DescriptionEdit->clear();
    TypeEdit->clear();
    return;
  }

  showCheck->setChecked(ParamTable->item(row, 0)->text() == tr("yes"));
  ParamNameEdit->setText(ParamTable->item(row, 1)->text());
  ValueEdit->setText(ParamTable->item(row, 2)->text());
  DescriptionEdit->setText(ParamTable->item(row, 3)->text());
  TypeEdit->setText(ParamTable->item(row, 4)->text());
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
  
  int row;
  for (row = 0; row < ParamTable->rowCount(); ++row) {
    if(ParamTable->item(row, 1)->text() == ParamNameEdit->text()) {
      QMessageBox::critical(this, tr("Error"),
         tr("Parameter \"%1\" already in list!").arg(ParamNameEdit->text()));
      return;
    }
  }

  row = ParamTable->rowCount();
  ParamTable->insertRow(row);

  QTableWidgetItem *item;
  item = new QTableWidgetItem((showCheck->isChecked())? tr("yes") : tr("no"));
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  ParamTable->setItem(row, 0, item);
  item = new QTableWidgetItem(ParamNameEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  ParamTable->setItem(row, 1, item);
  item = new QTableWidgetItem(ValueEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  ParamTable->setItem(row, 2, item);
  item = new QTableWidgetItem(DescriptionEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  ParamTable->setItem(row, 3, item);
  item = new QTableWidgetItem(TypeEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  ParamTable->setItem(row, 4, item);

  ParamTable->setCurrentCell(row, 0);
  slotEditParameter();   // clear entry fields
  ParamTable->clearSelection();
}

// -----------------------------------------------------------
void ID_Dialog::slotRemoveParameter()
{
  int selectedrow = ParamTable->currentRow();
  ParamTable->removeRow(selectedrow);
  int nextRow = (selectedrow == ParamTable->rowCount())? selectedrow-1 : selectedrow;
  ParamTable->setCurrentCell(nextRow, 0);
  slotEditParameter();
}

// -----------------------------------------------------------
void ID_Dialog::slotToggleShow(bool On)
{
  int selectedrow = ParamTable->currentRow();
  QTableWidgetItem *item = ParamTable->item(selectedrow, 0);
  if (item) {
    item->setText(On ? tr("yes") : tr("no"));
  }
}

// -----------------------------------------------------------
void ID_Dialog::slotNameChanged(const QString& text)
{
  int selectedrow = ParamTable->currentRow();
  QTableWidgetItem *item = ParamTable->item(selectedrow, 1);
  if (item) {
    item->setText(text);
  }
}

// -----------------------------------------------------------
void ID_Dialog::slotValueChanged(const QString& text)
{
  int selectedrow = ParamTable->currentRow();
  QTableWidgetItem *item = ParamTable->item(selectedrow, 2);
  if (item) {
    item->setText(text);
  }
}

// -----------------------------------------------------------
void ID_Dialog::slotDescrChanged(const QString& text)
{
  int selectedrow = ParamTable->currentRow();
  QTableWidgetItem *item = ParamTable->item(selectedrow, 3);
  if (item) {
    item->setText(text);
  }
}

// -----------------------------------------------------------
void ID_Dialog::slotTypeChanged(const QString& text)
{
  int selectedrow = ParamTable->currentRow();
  QTableWidgetItem *item = ParamTable->item(selectedrow, 4);
  if (item) {
    item->setText(text);
  }
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

  QList<SubParameter *> scratch;
  for (int row = 0; row < ParamTable->rowCount(); ++row) {
      bool display = ParamTable->item(row, 0)->text() == tr("yes");
      // s = "name=defaultval"
      QString s(ParamTable->item(row, 1)->text() + "=" + ParamTable->item(row, 2)->text()),
              desc(ParamTable->item(row, 3)->text()),
              type(ParamTable->item(row, 4)->text());

      scratch.append(new SubParameter(display, s, desc, type));
  }

  if (scratch.size()!=idText->Parameter.size()) {
      changed = true;
  } else {
      QList<SubParameter *>::const_iterator A(scratch.begin()),
                                            end(scratch.end()),
                                            B(idText->Parameter.begin());
      for(;A!=end; ++A, ++B) {
          if((*A)->display!=(*B)->display || (*A)->Name!=(*B)->Name ||
             (*A)->Description!=(*B)->Description || (*A)->Type!=(*B)->Type) {
              changed = true;
              break;
          }
      }
  }

  if(changed)
      idText->Parameter.swap(scratch);

  foreach(SubParameter *p, scratch) {
      delete p;
  }

  if(changed)  accept();
  else  reject();
}
