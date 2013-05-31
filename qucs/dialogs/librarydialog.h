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

#include <QRegExp>

#include <QList>
#include <QStringList>
#include <QTextStream>
#include <QDialog>
#include <QFile>
#include <QDir>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>

class QLabel;
class QucsApp;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QVBoxLayout;
class QTreeWidgetItem;
class QGroupBox;
class QRegExpValidator;


class LibraryDialog : public QDialog {
   Q_OBJECT
public:
  LibraryDialog(QucsApp*, QTreeWidgetItem*);  
 ~LibraryDialog();

private slots:
  void slotCreate();
  void slotNext();
  void slotSelectNone();
  void slotSelectAll();

private:
  void intoStream(QTextStream&, QString&, const char*);
  int intoFile(QString&, QString&,  QStringList&);

private:
  QVBoxLayout *all;   // the mother of all widgets
  QLabel *theLabel;
  QLineEdit *NameEdit;
  QTextEdit *ErrText;
  QGroupBox *Group;
  QPushButton *ButtCreate, *ButtCancel, *ButtSelectAll, *ButtSelectNone;
  //Q3PtrList<QCheckBox> BoxList;
  QList<QCheckBox *> BoxList;
  QStringList Descriptions;

  QucsApp *App;
  QFile LibFile;
  QDir LibDir;
  QRegExp Expr;
  QRegExpValidator *Validator;
};

#endif
