/***************************************************************************
                             librarydialog.h
                            -----------------
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

#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <qregexp.h>
#include <q3ptrlist.h>
#include <qstringlist.h>
#include <q3textstream.h>
#include <qdialog.h>
#include <qfile.h>
#include <qdir.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QLabel>

class QLabel;
class QucsApp;
class QLineEdit;
class Q3TextEdit;
class QPushButton;
class Q3VBoxLayout;
class Q3ListViewItem;
class Q3VButtonGroup;
class QRegExpValidator;


class LibraryDialog : public QDialog {
   Q_OBJECT
public:
  LibraryDialog(QucsApp*, Q3ListViewItem*);
 ~LibraryDialog();

private slots:
  void slotCreate();
  void slotNext();
  void slotSelectNone();
  void slotSelectAll();

private:
  void intoStream(Q3TextStream&, QString&, const char*);
  int intoFile(QString&, QString&,  QStringList&);

private:
  Q3VBoxLayout *all;   // the mother of all widgets
  QLabel *theLabel;
  QLineEdit *NameEdit;
  Q3TextEdit *ErrText;
  Q3VButtonGroup *Group;
  QPushButton *ButtCreate, *ButtCancel, *ButtSelectAll, *ButtSelectNone;
  Q3PtrList<QCheckBox> BoxList;
  QStringList Descriptions;

  QucsApp *App;
  QFile LibFile;
  QDir LibDir;
  QRegExp Expr;
  QRegExpValidator *Validator;
};

#endif
