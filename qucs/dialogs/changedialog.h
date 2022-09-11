/***************************************************************************
                              changedialog.h
                             ----------------
    begin                : Fri Jul 22 2005
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

#ifndef CHANGEDIALOG_H
#define CHANGEDIALOG_H

#include <QDialog>
#include <QRegExp>
#include <QGridLayout>

class Schematic;
class QLineEdit;
class QComboBox;
class QGridLayout;
class QRegExpValidator;


class ChangeDialog : public QDialog {
   Q_OBJECT
public:
  ChangeDialog(Schematic*);
 ~ChangeDialog();

private slots:
  void slotButtReplace();

private:
  bool matches(const QString&);

  Schematic *Doc;
  QGridLayout *all;   // the mother of all widgets
  QRegExpValidator  *Validator, *ValRestrict;
  QRegExp     Expr;
  QLineEdit   *CompNameEdit, *NewValueEdit;
  QComboBox   *CompTypeEdit, *PropNameEdit;
};

#endif
