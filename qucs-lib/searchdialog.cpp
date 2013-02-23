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

#include "searchdialog.h"
#include "qucslib.h"

#include <qlayout.h>
#include <q3hbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdir.h>
#include <qfile.h>
#include <q3listbox.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3VBoxLayout>


SearchDialog::SearchDialog(QucsLib *parent)
                     : QDialog(parent, 0, false, Qt::WDestructiveClose)
{
  ParentDialog = parent;

  all = new Q3VBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(5);

  all->addWidget(
    new QLabel(tr("The search result contains all components whose\n"
                  "name contains the search string. All libraries\n"
                  "are included in the search."), this) );

  Q3HBox *h1 = new Q3HBox(this);
  all->addWidget(h1);

  new QLabel(tr("Search string:"), h1);
  SearchEdit = new QLineEdit(h1);
  connect(SearchEdit, SIGNAL(returnPressed()), SLOT(slotSearch()));

  Q3HBox *h2 = new Q3HBox(this);
  all->addWidget(h2);

  h2->setStretchFactor(new QWidget(h2), 5); // stretchable placeholder

  QPushButton *ButtonSearch = new QPushButton(tr("Search"), h2);
  connect(ButtonSearch, SIGNAL(clicked()), SLOT(slotSearch()));
  QPushButton *ButtonClose = new QPushButton(tr("Close"), h2);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonSearch->setFocus();

  SearchEdit->setFocus();
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
  QStringList LibFiles = LibDir.entryList("*.lib", QDir::Files, QDir::Name);

  QFile File;
  Q3TextStream ReadWhole;
  QString LibraryString, LibName, CompName;
  QStringList::iterator it;
  int Start, End, NameStart, NameEnd;
  for(it = LibFiles.begin(); it != LibFiles.end(); it++) { // all library files
    File.setName(QucsSettings.LibDir + (*it));
    if(!File.open(QIODevice::ReadOnly))  continue;

    ReadWhole.setDevice(&File);
    LibraryString = ReadWhole.read();
    File.close();

    Start = LibraryString.find("<Qucs Library ");
    if(Start < 0)  continue;
    End = LibraryString.find('>', Start);
    if(End < 0)  continue;
    LibName = LibraryString.mid(Start, End-Start).section('"', 1, 1);

    // check all components of the current library
    while((Start=LibraryString.find("\n<Component ", Start)) > 0) {
      Start++;
      NameStart = Start + 11;
      NameEnd = LibraryString.find('>', NameStart);
      if(NameEnd < 0)  continue;
      CompName = LibraryString.mid(NameStart, NameEnd-NameStart);

      End = LibraryString.find("\n</Component>", NameEnd);
      if(End < 0)  continue;
      End += 13;

      // does search criterion match ?
      if(CompName.find(SearchEdit->text()) >= 0) {
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
    if(ParentDialog->Library->text(End-1) != tr("Search result"))
      ParentDialog->Library->insertItem(tr("Search result"));
    ParentDialog->Library->setCurrentItem(End);
    reject();
  }
  else  accept();
}
