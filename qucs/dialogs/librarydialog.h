/*
 * librarydialog.h - declaration of dialog to create library
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include <QList>
#include <QStringList>
#include <QTextStream>
#include <QDialog>
#include <QFile>
#include <QDir>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>

class QLabel;
class QLineEdit;
class QTextEdit;
class QPlainTextEdit;
class QPushButton;
class QVBoxLayout;
class QTreeWidgetItem;
class QGroupBox;
class QRegExpValidator;
class QStackedWidget;
//class QStringList;
class QListWidget;


class LibraryDialog : public QDialog {
   Q_OBJECT
public:
  LibraryDialog(QWidget *);
 ~LibraryDialog();

  void fillSchematicList(QStringList);

private slots:
  void slotCreateNext();
  void slotSave();
  void slotSelectNone();
  void slotSelectAll();
  void slotCheckDescrChanged(int);
  void slotPrevDescr();
  void slotNextDescr();
  void slotUpdateDescription();

private:
  void intoStream(QTextStream&, QString&, const char*);
  int intoFile(QString&, QString&,  QStringList&);

private:
  int curDescr;
  QVBoxLayout *all;   // the mother of all widgets
  QVBoxLayout *subcirListLayout;
  QStackedWidget *stackedWidgets;
  QLabel *theLabel;
  QLabel *checkedCktName;
  QLabel *libSaveName;
  QLineEdit *NameEdit;
  QPlainTextEdit *ErrText;
  QTextEdit *textDescr;
  QGroupBox *Group;
  QPushButton *ButtCreateNext, *ButtCancel, *ButtSelectAll, *ButtSelectNone;
  QPushButton *prevButt, *nextButt;
  QPushButton *createButt;
  QListWidget *subcirFileList;
  //QList<QCheckBox *> BoxList;
  QStringList SelectedNames;
  QStringList Descriptions;
  QCheckBox *checkDescr;

  QFile LibFile;
  QDir LibDir;
  QRegularExpression Expr;
  QRegularExpressionValidator *Validator;
};

#endif
