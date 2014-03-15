/***************************************************************************
                              searchdialog.cpp
                             ------------------
    begin                : Sat Apr 01 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "searchdialog.h"
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


SearchDialog::SearchDialog(QucsApp *App_)
            : QDialog(App_, 0, true)
{
  App = App_;

  QVBoxLayout *all = new QVBoxLayout(this);
  all->setMargin(5);

  QGroupBox *g1 = new QGroupBox(tr("Text to search for"));
  QVBoxLayout *vbox1 = new QVBoxLayout;
  SearchEdit = new QLineEdit();
  vbox1->addWidget(SearchEdit);
  g1->setLayout(vbox1);

  all->addWidget(g1);

  ReplaceGroup = new QGroupBox(tr("Text to replace with"));
  QVBoxLayout *vbox2 = new QVBoxLayout;
  ReplaceEdit = new QLineEdit();
  vbox2->addWidget(ReplaceEdit);
  ReplaceGroup->setLayout(vbox2);

  all->addWidget(ReplaceGroup);

  AskBox = new QCheckBox(tr("Ask before replacing"));
  all->addWidget(AskBox);

  PositionBox = new QCheckBox(tr("From cursor position"));
  all->addWidget(PositionBox);
  CaseBox = new QCheckBox(tr("Case sensitive"));
  all->addWidget(CaseBox);
  WordBox = new QCheckBox(tr("Whole words only"));
  all->addWidget(WordBox);
  BackwardBox = new QCheckBox(tr("Search backwards"));
  all->addWidget(BackwardBox);

  QHBoxLayout *Buttons = new QHBoxLayout();
  all->addLayout(Buttons);
  QPushButton *ButtonSearch = new QPushButton(tr("Search"));
  Buttons->addWidget(ButtonSearch);
  QPushButton *ButtonCancel = new QPushButton(tr("Cancel"));
  Buttons->addWidget(ButtonCancel);
  connect(ButtonSearch, SIGNAL(clicked()), SLOT(slotSearch()));
  connect(ButtonCancel, SIGNAL(clicked()), SLOT(reject()));

  ButtonSearch->setDefault(true);
}

SearchDialog::~SearchDialog()
{
}

// ---------------------------------------------------------------------
void SearchDialog::initSearch(bool replace)
{
  if(replace) {
    setWindowTitle(tr("Replace Text"));
    AskBox->setHidden(false);
    ReplaceGroup->setHidden(false);
  }
  else {
    setWindowTitle(tr("Search Text"));
    AskBox->hide();
    ReplaceGroup->hide();
  }

  TextDoc *Doc = (TextDoc*)App->DocumentTab->currentPage();
  ReplaceEdit->clear();
  SearchEdit->setText(Doc->textCursor().selectedText());
  SearchEdit->selectAll();

  SearchEdit->setFocus();
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
  else if(BackwardBox->isChecked())
    Line = Doc->document()->blockCount();

  if(!BackwardBox->isChecked())
    Column += Offset;

  if(SearchEdit->text().isEmpty())
    return;

  QFlag findFlags = 0;
  if (CaseBox->isChecked())
    findFlags = QTextDocument::FindCaseSensitively;
  if (WordBox->isChecked())
    findFlags = findFlags | QTextDocument::FindWholeWords;
  if (BackwardBox->isChecked())
    findFlags = findFlags | QTextDocument::FindBackward;

  while ( Doc->find(SearchEdit->text(), findFlags)) {

      count++;
      if(AskBox->isHidden())  // search only ?
        return;

      i = QMessageBox::Yes;
      if(AskBox->isChecked()) {
        i = QMessageBox::information(this,
               tr("Replace..."), tr("Replace occurrence ?"),
               QMessageBox::Yes | QMessageBox::Default, QMessageBox::No,
               QMessageBox::Cancel | QMessageBox::Escape);
      }

      switch(i) {
        case QMessageBox::Yes:
                 Doc->insertPlainText(ReplaceEdit->text());
                 Column += ReplaceEdit->text().length();
                 break;
        case QMessageBox::No:
                 Column += SearchEdit->text().length();
                 break;
        default: return;
      }
  }

  if(count == 0)
    QMessageBox::information(this, tr("Search..."),
                   tr("Search string not found!"));
  else
    if(!AskBox->isHidden())  // replace ?
      if(!AskBox->isChecked())  // only with that, "count" has correct number !!!
        QMessageBox::information(this, tr("Replace..."),
                     tr("Replaced %1 occurrences!").arg(count));
}

// ---------------------------------------------------------------------
void SearchDialog::slotSearch()
{
  accept();
  searchText(PositionBox->isChecked(), 0);
}
