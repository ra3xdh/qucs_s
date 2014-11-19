/*
 * searchdialog.h - implementation of search dialog
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "searchdialog.h"
#include "ui_searchdialog.h"

#include "textdoc.h"
#include "qucs.h"

SearchDialog::SearchDialog(QucsApp *App_) : 
    QDialog(App_, 0, true),
    ui(new Ui::SearchDialog)
{
  App = App_;
  ui->setupUi(this);

  connect(ui->ButtonSearch, SIGNAL(clicked()), SLOT(slotSearch()));
}

SearchDialog::~SearchDialog()
{
  delete ui;
}

// ---------------------------------------------------------------------
void SearchDialog::initSearch(bool replace)
{
  if(replace) {
    setWindowTitle(tr("Replace Text"));
    ui->AskBox->setHidden(false);
    ui->ReplaceGroup->setHidden(false);
  }
  else {
    setWindowTitle(tr("Search Text"));
    ui->AskBox->hide();
    ui->ReplaceGroup->hide();
  }

  TextDoc *Doc = (TextDoc*)App->DocumentTab->currentPage();
  ui->ReplaceEdit->clear();
  ui->SearchEdit->setText(Doc->textCursor().selectedText());
  ui->SearchEdit->selectAll();

  ui->SearchEdit->setFocus();
  layout()->setSizeConstraint(QLayout::SetFixedSize);
  exec();
}

// ---------------------------------------------------------------------
void SearchDialog::slotSearch()
{
  if(ui->SearchEdit->text().isEmpty()) {
    return;
  }

  if(ui->AskBox->isHidden()) { //search
    emit search(
        ui->SearchEdit->text(), ui->CaseBox->isChecked(), ui->WordBox->isChecked(),
        ui->BackwardBox->isChecked());
  } else {
    emit replace(
        ui->SearchEdit->text(), ui->ReplaceEdit->text(),
        ui->AskBox->isChecked(), ui->CaseBox->isChecked(),
        ui->WordBox->isChecked(), ui->BackwardBox->isChecked());
  }
}
