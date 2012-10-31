/***************************************************************************
                             packagedialog.h
                            -----------------
    begin                : Sun Jun 25 2006
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

#ifndef PACKAGEDIALOG_H
#define PACKAGEDIALOG_H

#include <qfile.h>
#include <qdialog.h>
#include <q3ptrlist.h>
#include <qstringlist.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <qdir.h>

class QucsApp;
class QString;
class QLineEdit;
class Q3TextEdit;
class QCheckBox;
class Q3VBoxLayout;
class QDataStream;
class QPushButton;
class Q3ListViewItem;
class Q3VButtonGroup;


class PackageDialog : public QDialog {
   Q_OBJECT
public:
  PackageDialog(QWidget*, bool);
 ~PackageDialog();

  void extractPackage();

private slots:
  void slotCreate();
  void slotBrowse();

private:
  int insertFile(const QString&, QFile&, QDataStream&);
  int insertDirectory(const QString&, QDataStream&);
  int insertLibraries(QDataStream&);

  int extractFile(QFile&, Q_UINT32, QDir&);
  int extractDirectory(QFile&, Q_UINT32, QDir&);
  int extractLibrary(QFile&, Q_UINT32);

  Q3VBoxLayout *all;   // the mother of all widgets
  QLineEdit *NameEdit;
  Q3TextEdit *MsgText;
  QCheckBox *LibraryCheck;
  Q3VButtonGroup *Group;
  Q3PtrList<QCheckBox> BoxList;

  QPushButton *ButtClose;
};

#endif
