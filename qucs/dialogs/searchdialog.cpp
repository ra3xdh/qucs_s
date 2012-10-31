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

#include <q3hbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <q3vgroupbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>


SearchDialog::SearchDialog(QucsApp *App_)
            : QDialog(App_, 0, true)
{
  App = App_;

  Q3VBoxLayout *all = new Q3VBoxLayout(this);
  all->setMargin(5);

  Q3VGroupBox *g1 = new Q3VGroupBox(tr("Text to search for"), this);
  all->addWidget(g1);
  SearchEdit = new QLineEdit(g1);

  ReplaceGroup = new Q3VGroupBox(tr("Text to replace with"), this);
  all->addWidget(ReplaceGroup);
  ReplaceEdit = new QLineEdit(ReplaceGroup);

  AskBox = new QCheckBox(tr("Ask before replacing"), this);
  all->addWidget(AskBox);

  PositionBox = new QCheckBox(tr("From cursor position"), this);
  all->addWidget(PositionBox);
  CaseBox = new QCheckBox(tr("Case sensitive"), this);
  all->addWidget(CaseBox);
  WordBox = new QCheckBox(tr("Whole words only"), this);
  all->addWidget(WordBox);
  BackwardBox = new QCheckBox(tr("Search backwards"), this);
  all->addWidget(BackwardBox);

  Q3HBox *Buttons = new Q3HBox(this);
  all->addWidget(Buttons);
  QPushButton *ButtonSearch = new QPushButton(tr("Search"), Buttons);
  connect(ButtonSearch, SIGNAL(clicked()), SLOT(slotSearch()));
  connect(new QPushButton(tr("Cancel"), Buttons),
          SIGNAL(clicked()), SLOT(reject()));

  ButtonSearch->setDefault(true);
}

SearchDialog::~SearchDialog()
{
}

// ---------------------------------------------------------------------
void SearchDialog::initSearch(bool replace)
{
  if(replace) {
    setCaption(tr("Replace Text"));
    AskBox->setHidden(false);
    ReplaceGroup->setHidden(false);
  }
  else {
    setCaption(tr("Search Text"));
    AskBox->hide();
    ReplaceGroup->hide();
  }

  TextDoc *Doc = (TextDoc*)App->DocumentTab->currentPage();
  ReplaceEdit->clear();
  SearchEdit->setText(Doc->selectedText());
  SearchEdit->selectAll();

  SearchEdit->setFocus();
  exec();
}

// ---------------------------------------------------------------------
void SearchDialog::searchText(bool fromCursor, int Offset)
{
  TextDoc *Doc = (TextDoc*)App->DocumentTab->currentPage();

  int Line=0, Column=0, count=0, i;
  if(fromCursor)
    Doc->getCursorPosition(&Line, &Column);
  else if(BackwardBox->isChecked())
    Line = Doc->paragraphs();

  if(!BackwardBox->isChecked())
    Column += Offset;

  if(SearchEdit->text().isEmpty())
    return;
  while(Doc->find(SearchEdit->text(), CaseBox->isChecked(),
         WordBox->isChecked(), !BackwardBox->isChecked(), &Line, &Column)) {

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
               Doc->insert(ReplaceEdit->text());
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
