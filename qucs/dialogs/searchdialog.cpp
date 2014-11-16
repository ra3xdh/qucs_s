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

#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>


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
void SearchDialog::searchText(bool fromCursor, int Offset)
{
  TextDoc *Doc = (TextDoc*)App->DocumentTab->currentPage();

  //FIXME : the find already searches from cursor
  int Line=0, Column=0, count=0, i;
  if(fromCursor) {
    Line = Doc->textCursor().blockNumber();
    Column = Doc->textCursor().columnNumber();
  }
  else if(ui->BackwardBox->isChecked())
    Line = Doc->document()->blockCount();

  if(!ui->BackwardBox->isChecked())
    Column += Offset;

  if(ui->SearchEdit->text().isEmpty())
    return;

  QFlag findFlags = 0;
  if (ui->CaseBox->isChecked()) {
    findFlags = QTextDocument::FindCaseSensitively;
  }
  if (ui->WordBox->isChecked()) {
    findFlags = findFlags | QTextDocument::FindWholeWords;
  }
  if (ui->BackwardBox->isChecked()) {
    findFlags = findFlags | QTextDocument::FindBackward;
  }

  while ( Doc->find(ui->SearchEdit->text(), findFlags)) {

      count++;
      if(ui->AskBox->isHidden())  // search only ?
        return;

      i = QMessageBox::Yes;
      if(ui->AskBox->isChecked()) {
        i = QMessageBox::information(this,
               tr("Replace..."), tr("Replace occurrence ?"),
               QMessageBox::Yes | QMessageBox::Default, QMessageBox::No,
               QMessageBox::Cancel | QMessageBox::Escape);
      }

      switch(i) {
        case QMessageBox::Yes:
                 Doc->insertPlainText(ui->ReplaceEdit->text());
                 Column += ui->ReplaceEdit->text().length();
                 break;
        case QMessageBox::No:
                 Column += ui->SearchEdit->text().length();
                 break;
        default: return;
      }
  }

  if(count == 0)
    QMessageBox::information(this, tr("Search..."),
                   tr("Search string not found!"));
  else
    if(!ui->AskBox->isHidden())  // replace ?
      if(!ui->AskBox->isChecked())  // only with that, "count" has correct number !!!
        QMessageBox::information(this, tr("Replace..."),
                     tr("Replaced %1 occurrences!").arg(count));
}

// ---------------------------------------------------------------------
void SearchDialog::slotSearch()
{
  accept();
  searchText(ui->PositionBox->isChecked(), 0);
}
