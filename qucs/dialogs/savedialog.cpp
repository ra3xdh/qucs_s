/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>                        *
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

#include "savedialog.h"
#include "qucs.h"
#include "qucsdoc.h"

#include <QVariant>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QGroupBox>
#include <QListWidgetItem>

SaveDialog::SaveDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
   : QDialog( parent, name, modal, fl ),unsavedDocs()
{
   if ( !name )
      setWindowTitle( tr( "Save the modified files" ) );
   app = 0l;
   initDialog();
}

SaveDialog::~SaveDialog()
{
}

void SaveDialog::setApp(QucsApp *a)
{
   app = a;
}

void SaveDialog::initDialog()
{
   setSizeGripEnabled( FALSE );
   SaveDialogLayout = new QVBoxLayout( this, 11, 6, "SaveDialogLayout"); 

   label = new QLabel( tr( "Select files to be saved" ) );
   SaveDialogLayout->addWidget( label );

   QGroupBox *group = new QGroupBox( tr( "Modified Files" ) );
   QVBoxLayout *checkBoxLayout = new QVBoxLayout();
   group->setLayout(checkBoxLayout);
   
   fileView = new QListWidget(this);
   checkBoxLayout->addWidget(fileView);
   SaveDialogLayout->addWidget(group);
   
   buttonsLayout = new QHBoxLayout( 0, 0, 6, "buttonsLayout"); 

   abortClosingButton = new QPushButton( tr( "Abort Closing" ) );
   buttonsLayout->addWidget( abortClosingButton );
   spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
   buttonsLayout->addItem( spacer );

   dontSaveButton = new QPushButton( tr( "Don't Save" ) );
   buttonsLayout->addWidget( dontSaveButton );

   saveSelectedButton = new QPushButton( tr( "Save Selected" ) );
   saveSelectedButton->setDefault(true);
   buttonsLayout->addWidget( saveSelectedButton );
   SaveDialogLayout->addLayout( buttonsLayout );
   languageChange();
   resize( QSize(500, 300).expandedTo(minimumSizeHint()) );
   //clearWState( Qt::WA_WState_Polished );
   setAttribute(Qt::WA_WState_Polished, false);

   connect(abortClosingButton,SIGNAL(clicked()),this,SLOT(reject()));
   connect(dontSaveButton,SIGNAL(clicked()),this,SLOT(dontSaveClicked()));
   connect(saveSelectedButton,SIGNAL(clicked()),this,SLOT(saveSelectedClicked()));
}

void SaveDialog::addUnsavedDoc(QucsDoc *doc)
{
   QString text = (doc->DocName).isEmpty() ? tr("Untitled") : doc->DocName;

   QListWidgetItem *item = new QListWidgetItem(text, fileView);
   item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
   item->setCheckState(Qt::Checked);
   
   unsavedDocs.insert(doc, item);
}

void SaveDialog::dontSaveClicked()
{
   done(DontSave);
}

void SaveDialog::saveSelectedClicked()
{   
   QList<QucsDoc*> unsavable;
   QMap<QucsDoc*,QListWidgetItem*>::iterator it(unsavedDocs.begin());
   for ( ; it != unsavedDocs.end(); ++it)
   {
      if ( it.value()->checkState() == Qt::Checked )
      {
         QucsDoc *doc = static_cast<QucsDoc*>(it.key());
         if(app->saveFile(doc) == false)
            unsavable.append(doc);
         else
            unsavedDocs.remove(it);
      }
   }
   if(unsavable.isEmpty())
       done(SaveSelected);
}

void SaveDialog::reject()
{
   done(AbortClosing);
}

bool SaveDialog::isEmpty() const
{
   return unsavedDocs.isEmpty();
}
