/***************************************************************************
                              librarydialog.cpp
                             -------------------
    begin                : Sun Jun 04 2006
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "librarydialog.h"
#include "qucslib.h"

#include <QFile>
#include <QLabel>
#include <QLineedit>
#include <QValidator>
#include <QMessagebox>
#include <QPushbutton>
//#include <q3scrollview.h> //-->QAbstractScrollArea and QScrollArea
#include <QRadiobutton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QGroupBox>


LibraryDialog::LibraryDialog(QWidget *App_) : QDialog(App_) //?, 0, TRUE, Qt::WDestructiveClose)
{
  setWindowTitle(tr("Manage User Libraries"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);

  // ...........................................................
  QVBoxLayout *all = new QVBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(6);

  Group = new QGroupBox(tr("Choose library:"), this);
  all->addWidget(Group);
/* TODO insert the group of radio buttons into an scrollable area
  Q3ScrollView *Dia_Scroll = new Q3ScrollView(Group);
  Dia_Scroll->setMargin(5);
  Dia_Box = new Q3VBox(Dia_Scroll->viewport());
  Dia_Scroll->addChild(Dia_Box);
*/
  QHBoxLayout *h1 = new QHBoxLayout();
  theLabel = new QLabel(tr("New Name:"));
  NameEdit = new QLineEdit();
  NameEdit->setValidator(Validator);
  h1->addWidget(theLabel);
  h1->addWidget(NameEdit);
  all->addLayout(h1);

  // ...........................................................
  QHBoxLayout *h2 = new QHBoxLayout();
  ButtDelete = new QPushButton(tr("Delete"));
  connect(ButtDelete, SIGNAL(clicked()), SLOT(slotDelete()));
  ButtRename = new QPushButton(tr("Rename"));
  connect(ButtRename, SIGNAL(clicked()), SLOT(slotRename()));
  ButtClose = new QPushButton(tr("Close"));
  connect(ButtClose, SIGNAL(clicked()), SLOT(reject()));
  ButtClose->setDefault(true);
  h2->addWidget(ButtDelete);
  h2->addWidget(ButtRename);
  h2->addWidget(ButtClose);
  all->addLayout(h2);

  // ...........................................................
  // insert all user libraries as radio buttons
  QStringList LibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

  QVBoxLayout *vbox = new QVBoxLayout;
  //previousLib = 0;

  QButtonGroup *radiogroup = new QButtonGroup(this); //for exclusive check

  // inserts all user libs as named RadioButton into a list
  //qDebug() << "Starting";
  QStringList::iterator it;
  for(it = LibFiles.begin(); it != LibFiles.end(); it++) {
    //qDebug() << *it;
    radioButton.append(new QRadioButton((*it).left((*it).length()-4)));
  }

  // populate button group and vertical layout
  for(int i = 0; i < LibFiles.count(); i++){
    //qDebug() << radioButton[i]->text();
    radiogroup->addButton(radioButton[i]);
    vbox->addWidget(radioButton[i]);
  }

  Group->setLayout(vbox);

/*

  QColor theColor;
  QAbstractButton *rButton = toggleGroup->find(0); //?indexOf
  if(rButton)
    theColor = rButton->paletteBackgroundColor();
  else {
    ButtDelete->setEnabled(false);
    ButtRename->setEnabled(false);
    theColor =
       (new QLabel(tr("No user library!"), Dia_Box))->paletteBackgroundColor();
  }
  Dia_Scroll->viewport()->setPaletteBackgroundColor(theColor);
*/
}

LibraryDialog::~LibraryDialog()
{
  delete Validator;
}

// ---------------------------------------------------------------
// Renames selected user library.
void LibraryDialog::slotRename()
{
  if(NameEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please insert a new library name!"));
    return;
  }

  // pick the clicked radio
  QRadioButton *rButton =0;
  for(int i = 0; i < radioButton.count(); i++){
    if (radioButton[i]->isChecked()){
      rButton = radioButton[i];
      break;
    }
  }

  if(rButton == 0) {
    QMessageBox::critical(this, tr("Error"), tr("Please choose a library!"));
    return;
  }

  QString oldName = UserLibDir.absoluteFilePath(rButton->text());
  QString newName = UserLibDir.absoluteFilePath(NameEdit->text());
  qDebug() << oldName + ".lib";
  qDebug() << newName + ".lib";

  QFile NewLibFile(QucsSettings.LibDir + NameEdit->text() + ".lib");
  if(NewLibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A system library with this name already exists!"));
    return;
  }

  NewLibFile.setFileName(newName + ".lib");
  if(NewLibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A library with this name already exists!"));
    return;
  }

  QDir LibSubdir(oldName);
  if(LibSubdir.exists()) {
    if(!LibSubdir.rename(oldName, newName)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot rename library subdirectory!"));
    }
  }

  QFile LibFile(oldName + ".lib");
  if(!LibFile.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot open library!"));
    return;
  }

  QByteArray FileContent = LibFile.readAll();
//  LibFile.close();

  // rename library name within file
  char *p, *Name;
  char *Config = FileContent.data();
  for(;;) {
    p = strstr(Config, "<Qucs Library ");
    if(p == 0) break;
    Name = strstr(p, " \"");
    if(Name == 0) break;
    Name += 2;
    p = strstr(Name, "\">");
    if(p == 0) break;

    if(!NewLibFile.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(this, tr("Error"), tr("No permission to modify library!"));
      return;
    }
    int count = 0;
    count += NewLibFile.write(Config, Name-Config);
    count += NewLibFile.write(NameEdit->text().toLatin1(), NameEdit->text().length());
    count += NewLibFile.write(p, FileContent.count() - (p-Config) );
    NewLibFile.close();
    count -= FileContent.count() + NameEdit->text().length() - (p-Name);
    if(count != 0) {
      QMessageBox::critical(this, tr("Error"), tr("Writing new library not successful!"));
      return;
    }

    if(!LibFile.remove()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot delete old library."));
      //toggleGroup->insert(new QRadioButton(NameEdit->text(), Dia_Box));
      NameEdit->clear();
      return;
    }

    rButton->setText(NameEdit->text());
    NameEdit->clear();
    return;
  }

  QMessageBox::critical(this, tr("Error"), tr("Library file is corrupt!"));
}

// ---------------------------------------------------------------
// Deletes the selected user library.
void LibraryDialog::slotDelete()
{
  // pick the clicked radio
  QRadioButton *rButton =0;
  for(int i = 0; i < radioButton.count(); i++){
    if (radioButton[i]->isChecked()){
      rButton = radioButton[i];
      break;
    }
  }

  if(rButton == 0) {
    QMessageBox::critical(this, tr("Error"), tr("Please choose a library!"));
    return;
  }

  QString oldName = UserLibDir.absoluteFilePath(rButton->text());
  qDebug() << oldName +".lib";
  QFile LibFile(oldName + ".lib");
  if(!LibFile.remove()) {
    QMessageBox::critical(this, tr("Error"),
                 tr("No permission to delete library \"%1\".").arg(rButton->text()));
    return;
  }

  QDir LibSubdir(oldName);
  if(LibSubdir.exists()) {
    QStringList DirFiles;
    QStringList::iterator it;
    DirFiles = LibSubdir.entryList(QStringList("*"), QDir::Files, QDir::Name);
    for(it = DirFiles.begin(); it != DirFiles.end(); it++)
      LibSubdir.remove(*it);
    if(!LibSubdir.rmdir(oldName)) {
      QMessageBox::critical(this, tr("Error"),
        tr("No permission to delete library subdirectory \"%1\".").
			    arg(rButton->text()));
      return;
    }
  }

  delete rButton;
}
