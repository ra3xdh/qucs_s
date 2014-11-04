/***************************************************************************
 * Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>        *
 *                                                                         *
 * Modified from SaveDialog and LibraryDialog                              *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#include <QVariant>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDebug>
#include <QFileDialog>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QGroupBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>

#include "loaddialog.h"
#include "qucs.h"
#include "qucsdoc.h"
#include "components/components.h"

LoadDialog::LoadDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
   : QDialog( parent, name, modal, fl )
{
   if ( !name )
      setWindowTitle( tr( "Load Verilog-A symbols" ) );
   app = 0l;
//   initDialog();
}

LoadDialog::~LoadDialog()
{
}

void LoadDialog::setApp(QucsApp *a)
{
   app = a;
}

void LoadDialog::initDialog()
{
   QVBoxLayout *all = new QVBoxLayout(this);
   all->setMargin(5);
   all->setSpacing(6);

   // hold group of files / group icon and checkboxes
   QHBoxLayout *hGroups = new QHBoxLayout();
   all->addLayout(hGroups);

   // ---
   QGroupBox *group1 = new QGroupBox( tr( "Choose Verilog-A symbol files:" ) );
   hGroups->addWidget(group1);

   QScrollArea *scrollArea = new QScrollArea(group1);
   scrollArea->setWidgetResizable(true);

   fileView = new QListWidget(this);

   scrollArea->setWidget(fileView);

   QVBoxLayout *areaLayout = new QVBoxLayout();
   areaLayout->addWidget(scrollArea);
   group1->setLayout(areaLayout);


   // ...........................................................
   QGridLayout *gridButts = new QGridLayout();
   all->addLayout(gridButts);

   ButtSelectAll = new QPushButton(tr("Select All"));
   gridButts->addWidget(ButtSelectAll, 0, 0);
   connect(ButtSelectAll, SIGNAL(clicked()), SLOT(slotSelectAll()));
   ButtSelectNone = new QPushButton(tr("Deselect All"));
   gridButts->addWidget(ButtSelectNone, 0, 1);
   connect(ButtSelectNone, SIGNAL(clicked()), SLOT(slotSelectNone()));
   // ...........................................................
   ButtCancel = new QPushButton(tr("Cancel"));
   gridButts->addWidget(ButtCancel, 1, 0);
   connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
   ButtOk = new QPushButton(tr("Ok"));
   gridButts->addWidget(ButtOk, 1, 1);
   connect(ButtOk, SIGNAL(clicked()), SLOT(loadSelected()));
   ButtOk->setDefault(true);


   QVBoxLayout *iconLayout = new QVBoxLayout();

   QGroupBox *group2 = new QGroupBox( );

   QVBoxLayout *group2Layout = new QVBoxLayout();

   iconPixmap = new QLabel();
   iconPixmap->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);
   iconPixmap->setAlignment(Qt::AlignCenter);
   group2Layout->addWidget(iconPixmap);

   group2->setLayout(group2Layout);
   iconLayout->addWidget(group2);

   ButtChangeIcon = new QPushButton(tr("Change Icon"));
   iconLayout->addWidget(ButtChangeIcon);
   hGroups->addLayout(iconLayout);
   connect(ButtChangeIcon,SIGNAL(clicked()),this,SLOT(slotChangeIcon()));

   // group checkboxes
   QGroupBox *group3 = new QGroupBox();
   QVBoxLayout *group3Layout = new QVBoxLayout();
   group3->setLayout(group3Layout);
   iconLayout->addWidget(group3);

   //
   QCheckBox *autoLoadSelCheck = new QCheckBox(tr("auto-load selected"));
   autoLoadSelCheck->setToolTip(
               tr("Load the selected symbols when opening the project."));

   autoLoadSelCheck->setDisabled(true); //disabled for now

   group3Layout->addWidget(autoLoadSelCheck);

   /*
   QCheckBox *autoLoadAllwaysCheck = new QCheckBox(tr("auto-load all"));
   autoLoadAllwaysCheck->setToolTip(
               tr("Load all symbols."));
   group3Layout->addWidget(autoLoadAllwaysCheck);
   */

   connect(fileView, SIGNAL(itemPressed(QListWidgetItem*)),
           this, SLOT(slotSymbolFileClicked(QListWidgetItem*)));

//   qDebug() << "files " << symbolFiles;

   for(int i=0; i < symbolFiles.size(); i++){
       QListWidgetItem *item = new QListWidgetItem(symbolFiles.at(i), fileView);
     item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
     item->setCheckState(Qt::Checked);

     //set first as selected, one need to be selected to assign bitmap
     fileView->setCurrentRow(0);
   }

   // update icon
   this->slotSymbolFileClicked(fileView->currentItem());

   fileView->installEventFilter(this);
   fileView->setFocus();

}

void LoadDialog::slotSelectAll()
{
    for(int i = 0; i < fileView->count(); ++i)
    {
        QListWidgetItem* item = fileView->item(i);
        item->setCheckState(Qt::Checked);
//        qDebug() << "select" << item->text();
    }
}

void LoadDialog::slotSelectNone()
{
    for(int i = 0; i < fileView->count(); ++i)
    {
        QListWidgetItem* item = fileView->item(i);
        item->setCheckState(Qt::Unchecked);
//        qDebug() << "unselect" << item->text();
    }
}

void LoadDialog::slotSymbolFileClicked(QListWidgetItem* item)
{
//  qDebug() << "pressed" << item->text();
 // get bitmap, try to plot
 // similar to QucsApp::slotSetCompView
  QString JSON = projDir.filePath(item->text());

//  qDebug() << "read " << JSON;

  // Just need path to bitmap, do not create an object
  QString Name, vaBitmap;
  Component * c = (Component *)
          vacomponent::info (Name, vaBitmap, false, JSON);
  if (c) delete c;

//  qDebug() << "slotSymbolFileClicked" << Name << vaBitmap;

  // check if icon exists, fall back to default
  QString iconPath = QString(projDir.absFilePath(vaBitmap+".png"));
  QFile iconFile(iconPath);

  if(iconFile.exists())
  {
    // load bitmap defined on the JSON symbol file
    iconPixmap->setPixmap(QPixmap(iconPath));
  }
  else
  {
    QMessageBox::information(this, tr("Info"),
                 tr("Icon not found:\n %1.png").arg(vaBitmap));
    // default icon
    iconPixmap->setPixmap(QPixmap(":/bitmaps/editdelete.png"));
  }
}

void LoadDialog::reject()
{
    done(AbortClosing);
}

//
void LoadDialog::loadSelected()
{
  // build list vaComponentds
  // hand it down to main app

  selectedComponents.clear();

  for(int i = 0; i < fileView->count(); ++i)
  {
    QListWidgetItem* item = fileView->item(i);

    if (item->checkState() == Qt::Checked){
        QString key = item->text().split("_symbol.json").at(0);
        QString value = projDir.absoluteFilePath(item->text());

        qDebug() << "key" << key;
        qDebug() << "file " << value;

        selectedComponents[key] = value;
    }
  }

  accept();
}


/*
 * Browse for icon image
 * Save image path to JSON symbol file
 */
void LoadDialog::slotChangeIcon()
{
//  qDebug() << "slotChangeIcon";
  QString iconFileName =
          QFileDialog::getOpenFileName(this,
                                        tr("Open File"),
                                        QString(projDir.absolutePath()),
                                        tr("Icon image (*.png)"));

  QString newIcon =  QFileInfo(iconFileName).baseName();
//  qDebug() << "icon "<< newIcon;

  QString filename = fileView->currentItem()->text();
  filename = projDir.absoluteFilePath(filename);
//  qDebug() << "for " <<  filename;

  // open json
  // change property
  // save&close
  // Try to open the JSON file, can use QScriptEngine for this?
  //
  QFile file(filename);
  QByteArray ba;
  ba.clear();
  if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
    QMessageBox::critical(this, tr("Error"),
                          tr("File not found: %1").arg(filename));
  }
  else {
    QTextStream in(&file);
    while ( !in.atEnd() )
    {
      QString line = in.readLine();
      if (line.contains("BitmapFile")){
          QString change =
                  QString("  \"BitmapFile\" : \"%1\",").arg(newIcon);
          ba.append(change+"\n");
      }
      else{
          ba.append(line+"\n");
      }
    }
  }

  // write back to the same file, clear it first
  file.resize(0);
  file.write(ba);
  file.close();

  // update icon
  this->slotSymbolFileClicked(fileView->currentItem());
}

bool LoadDialog::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
//    qDebug() << "type" << keyEvent->key() << fileView->count();
    if (keyEvent->key() == Qt::Key_Up) {

        fileView->setCurrentRow(std::max(0, fileView->currentRow()-1));
        this->slotSymbolFileClicked(fileView->currentItem());
        return true;
    }
    if (keyEvent->key() == Qt::Key_Down) {
        fileView->setCurrentRow(std::min(fileView->count()-1, fileView->currentRow()+1));
        this->slotSymbolFileClicked(fileView->currentItem());
        return true;
    }
  }
  return false;
}
