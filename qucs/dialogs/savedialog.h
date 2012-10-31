/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
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

#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmap.h>
#include <q3listview.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class QLabel;
class Q3ListView;
class Q3ListViewItem;
class QPushButton;
class QucsDoc;
class QucsApp;

class SaveDialog : public QDialog
{
   Q_OBJECT
   public:
      enum {
         AbortClosing = 0,
         DontSave,
         SaveSelected
      };
            
      SaveDialog(QWidget* p = 0, const char* n = 0, bool modal = true, Qt::WFlags fl = 0 );
      ~SaveDialog();
      void addUnsavedDoc(QucsDoc *doc);
      void setApp(QucsApp *a);
      bool isEmpty() const;
			     
   protected slots:
      void dontSaveClicked();
      void saveSelectedClicked();
      void languageChange();
      void reject();
      
   private:
      void initDialog();
      
      QMap<QucsDoc*,Q3CheckListItem*> unsavedDocs;
            
      QLabel* label;
      Q3ListView* filesView;
      QPushButton* abortClosingButton;
      QPushButton* dontSaveButton;
      QPushButton* saveSelectedButton;
      Q3VBoxLayout* SaveDialogLayout;
      Q3HBoxLayout* buttonsLayout;
      QSpacerItem* spacer;
      QucsApp *app;
};

#endif // SAVEDIALOG_H
