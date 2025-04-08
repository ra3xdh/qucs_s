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


ID_Dialog::ID_Dialog(ID_Text *idText_, QWidget *parent)
    : QDialog(parent)
{
  idText = idText_;
  setWindowTitle(tr("Edit Subcircuit Properties"));

  all = new QVBoxLayout;
  all->setSpacing(5);
  all->setContentsMargins(5,5,5,5);

  QHBoxLayout *htop = new QHBoxLayout;
  htop->setSpacing(5);
  all->addLayout(htop);

  Expr.setPattern("[A-Za-z][A-Za-z0-9_]*");
  SubVal = new QRegularExpressionValidator(Expr, this);
  Prefix = new QLineEdit(idText->prefix);
  Prefix->setValidator(SubVal);

  htop->addWidget(new QLabel(tr("Prefix:")));
  htop->addWidget(Prefix);

  QGroupBox *ParamBox = new QGroupBox(tr("Parameters"));
  all->addWidget(ParamBox);
  QVBoxLayout *vbox_param = new QVBoxLayout;
  ParamBox->setLayout(vbox_param);

  ParamTable = new QTableWidget();
  ParamTable->horizontalHeader()->setStretchLastSection(true);
  // set automatic resize so all content will be visible,
  //  horizontal scrollbar will appear if table becomes too large
  ParamTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ParamTable->horizontalHeader()->setSectionsClickable(false); // no action when clicking on the header
  ParamTable->verticalHeader()->hide();
  ParamTable->setColumnCount(5);
  ParamTable->setHorizontalHeaderLabels(
      QStringList() << tr("display") << tr("Name") << tr("Default") << tr("Description") << tr("Type"));
  ParamTable->setSortingEnabled(false); // no sorting
  ParamTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  vbox_param->addWidget(ParamTable);

  QTableWidgetItem *item;
  for (const auto& sub_param : idText->subParameters) {
    int row = ParamTable->rowCount();
    ParamTable->insertRow(row);
    item = new QTableWidgetItem((sub_param->display)? tr("yes") : tr("no"));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 0, item);
    item = new QTableWidgetItem(sub_param->name.section('=', 0, 0));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 1, item);
    item = new QTableWidgetItem(sub_param->name.section('=', 1, 1));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 2, item);
    item = new QTableWidgetItem(sub_param->description);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 3, item);
    item = new QTableWidgetItem(sub_param->type);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ParamTable->setItem(row, 4, item);
  }
  connect(ParamTable, SIGNAL(currentCellChanged(int, int, int, int)), SLOT(slotEditParameter()));

  showCheck = new QCheckBox(tr("display in schematic"));
  showCheck->setChecked(true);

  vbox_param->addWidget(showCheck);

  QGridLayout *paramEditLayout = new QGridLayout;
  vbox_param->addLayout(paramEditLayout);

  paramEditLayout->addWidget(new QLabel(tr("Name:")), 0, 0);
  paramEditLayout->addWidget(new QLabel(tr("Default Value:")), 1, 0);
  paramEditLayout->addWidget(new QLabel(tr("Description:")), 2, 0);
  paramEditLayout->addWidget(new QLabel(tr("Type:")), 3, 0);

  Expr.setPattern("[\\w_]+");
  NameVal = new QRegularExpressionValidator(Expr, this);
  ParamNameEdit = new QLineEdit;
  ParamNameEdit->setValidator(NameVal);

  Expr.setPattern("[^\"=]*");
  ValueVal = new QRegularExpressionValidator(Expr, this);
  ValueEdit = new QLineEdit;
  ValueEdit->setValidator(ValueVal);

  Expr.setPattern("[^\"=\\[\\]]*");
  DescrVal = new QRegularExpressionValidator(Expr, this);
  DescriptionEdit = new QLineEdit;
  DescriptionEdit->setValidator(DescrVal);

  Expr.setPattern("[\\w_]+");
  TypeVal = new QRegularExpressionValidator(Expr, this);
  TypeEdit = new QLineEdit;
  TypeEdit->setValidator(TypeVal);

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
  QPushButton *ButtApply = new QPushButton(tr("Apply"));
  connect(ButtApply, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"));
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  QHBoxLayout *hbox_bottom = new QHBoxLayout;
  hbox_bottom->setSpacing(5);
  all->addLayout(hbox_bottom);
  hbox_bottom->addWidget(ButtOK);
  hbox_bottom->addWidget(ButtApply);
  hbox_bottom->addWidget(ButtCancel);

  this->setLayout(all);
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


/*!
 * \brief ID_Dialog::slotEditParameter
 * Place data from selected table row in the edit fields.
 */
void ID_Dialog::slotEditParameter()
{
  int row = ParamTable->currentRow();
  if (row < 0 || row >= ParamTable->rowCount()) {
    return;
  }

  showCheck->setChecked(ParamTable->item(row, 0)->text() == tr("yes"));
  ParamNameEdit->setText(ParamTable->item(row, 1)->text());
  ValueEdit->setText(ParamTable->item(row, 2)->text());
  DescriptionEdit->setText(ParamTable->item(row, 3)->text());
  TypeEdit->setText(ParamTable->item(row, 4)->text());
}


/*!
 * \brief ID_Dialog::slotAddParameter
 * Add new set of parameters from edit fields into table.
 * Select added row.
 */
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
}


/*!
 * \brief ID_Dialog::slotRemoveParameter
 * Remove selected row from table.
 */
void ID_Dialog::slotRemoveParameter()
{
  int selectedrow = ParamTable->currentRow();
  ParamTable->removeRow(selectedrow);
  int nextRow = (selectedrow == ParamTable->rowCount())? selectedrow-1 : selectedrow;
  ParamTable->setCurrentCell(nextRow, 0);
}


/*!
 * \brief ID_Dialog::slotOk
 * Commit changes from dialog table to component properties.
 */
void ID_Dialog::slotOk()
{
  bool changed = false;

  if (!Prefix->text().isEmpty())
    if (idText->prefix != Prefix->text()) {
      idText->prefix = Prefix->text();
      changed = true;
    }

  std::vector<std::unique_ptr<SubParameter>> scratch;
  for (int row = 0; row < ParamTable->rowCount(); ++row) {
      bool display = ParamTable->item(row, 0)->text() == tr("yes");
      QString name(ParamTable->item(row, 1)->text() + "=" + ParamTable->item(row, 2)->text());
      QString desc(ParamTable->item(row, 3)->text());
      QString type(ParamTable->item(row, 4)->text());

      scratch.push_back(std::make_unique<SubParameter>(display, name, desc, type));
  }

  if (scratch.size() != idText->subParameters.size()) {
      changed = true;
  } else {
      for (std::size_t i = 0; !changed && i < scratch.size(); i++) {
        changed = changed
                || scratch[i]->display     != idText->subParameters[i]->display
                || scratch[i]->name        != idText->subParameters[i]->name
                || scratch[i]->description != idText->subParameters[i]->description
                || scratch[i]->type        != idText->subParameters[i]->type
                ;
      }
  }

  if (changed) {
      idText->subParameters.swap(scratch);
  }

  changed ? accept() : reject();
}


/*!
 * \brief ID_Dialog::slotApply
 * Apply data from edit fields to table. Clear edit fields.
 */
void ID_Dialog::slotApply()
{
  int selectedrow = ParamTable->currentRow();
  if (selectedrow<0) return; // Nothing selected

  QTableWidgetItem *item;
  item = ParamTable->item(selectedrow, 0);
  item->setText(showCheck->isChecked() ? tr("yes") : tr("no"));
  item = ParamTable->item(selectedrow, 1);
  item->setText(ParamNameEdit->text());
  item = ParamTable->item(selectedrow, 2);
  item->setText(ValueEdit->text());
  item = ParamTable->item(selectedrow, 3);
  item->setText(DescriptionEdit->text());
  item = ParamTable->item(selectedrow, 4);
  item->setText(TypeEdit->text());

}
