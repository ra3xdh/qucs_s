/***************************************************************************
                               searchdialog.cpp
                              ------------------
    begin                : Sat Jun 11 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDir>
#include <QFile>
#include <QTextStream>


#include "searchdialog.h"
#include "qucslib.h"



SearchDialog::SearchDialog(QucsLib *parent)
                     //: QDialog(parent, 0, false, Qt::WDestructiveClose)
    : QDialog(parent)
{
  ParentDialog = parent;

  all = new QVBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(5);

  all->addWidget(
    new QLabel(tr("The search result contains all components whose\n"
                  "name contains the search string. All libraries\n"
                  "are included in the search."), this) );

  QLabel *SearchString = new QLabel(tr("Search string:"));
  SearchEdit = new QLineEdit();
  connect(SearchEdit, SIGNAL(returnPressed()), SLOT(slotSearch()));

  QGroupBox *h1 = new QGroupBox(this);
  QHBoxLayout *h1Layout = new QHBoxLayout();
  h1Layout->addWidget(SearchString);
  h1Layout->addWidget(SearchEdit);
  h1->setLayout(h1Layout);
  

  QPushButton *ButtonSearch = new QPushButton(tr("Search"));
  connect(ButtonSearch, SIGNAL(clicked()), SLOT(slotSearch()));
  QPushButton *ButtonClose = new QPushButton(tr("Close"));
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonSearch->setFocus();

  SearchEdit->setFocus();
  
  QGroupBox *h2 = new QGroupBox(this);
  QHBoxLayout *h2Layout = new QHBoxLayout();
  h2Layout->addWidget(ButtonSearch);
  h2Layout->addWidget(ButtonClose);
  h2->setLayout(h2Layout);
  
  all->addWidget(h1);
  all->addWidget(h2);
  
}

SearchDialog::~SearchDialog()
{
  delete all;
}

// ************************************************************
void SearchDialog::slotClose()
{
  reject();
}

// ************************************************************
void SearchDialog::slotSearch()
{
  if(SearchEdit->text().isEmpty()) {
    reject();
    return;
  }

  bool findComponent = false;
  QDir LibDir(QucsSettings.LibDir);
  QStringList LibFiles = LibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

  QFile File;
  QTextStream ReadWhole;
  QString LibraryString, LibName, CompName;
  QStringList::iterator it;
  int Start, End, NameStart, NameEnd;
  for(it = LibFiles.begin(); it != LibFiles.end(); it++) { // all library files
    File.setFileName(QucsSettings.LibDir + (*it));
    if(!File.open(QIODevice::ReadOnly))  continue;

    ReadWhole.setDevice(&File);
    LibraryString = ReadWhole.readAll();
    File.close();

    Start = LibraryString.indexOf("<Qucs Library ");
    if(Start < 0)  continue;
    End = LibraryString.indexOf('>', Start);
    if(End < 0)  continue;
    LibName = LibraryString.mid(Start, End-Start).section('"', 1, 1);

    // check all components of the current library
    while((Start=LibraryString.indexOf("\n<Component ", Start)) > 0) {
      Start++;
      NameStart = Start + 11;
      NameEnd = LibraryString.indexOf('>', NameStart);
      if(NameEnd < 0)  continue;
      CompName = LibraryString.mid(NameStart, NameEnd-NameStart);

      End = LibraryString.indexOf("\n</Component>", NameEnd);
      if(End < 0)  continue;
      End += 13;

      // does search criterion match ?
      if(CompName.indexOf(SearchEdit->text()) >= 0) {
        if(!findComponent) {
          ParentDialog->DefaultSymbol = "";
          ParentDialog->CompList->clear();
          ParentDialog->LibraryComps.clear();
        }
        findComponent = true;
        //ParentDialog->CompList->insertItem(CompName);
	ParentDialog->CompList->addItem(CompName);
        ParentDialog->LibraryComps.append(
			LibName+'\n'+LibraryString.mid(Start, End-Start));
      }
      Start = End;
    }
  }

  if(findComponent) {
    End = ParentDialog->Library->count();
    if(ParentDialog->Library->itemText(End-1) != tr("Search result"))
      ParentDialog->Library->addItem(tr("Search result"));
    ParentDialog->Library->setCurrentIndex(End);
    reject();
  }
  else  accept();
}
