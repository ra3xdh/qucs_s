/***************************************************************************
                             vasettingsdialog.h
                            --------------------
    begin                : Sun Oct 26 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefa@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VASETTINGSDIALOG_H
#define VASETTINGSDIALOG_H

#include <qdialog.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QLabel>

class TextDoc;
class QLineEdit;
class QPushButton;
class QRegExpValidator;
class QLabel;
class QCheckBox;
class Q3ButtonGroup;

class VASettingsDialog : public QDialog  {
Q_OBJECT
public:
  VASettingsDialog (TextDoc *);
 ~VASettingsDialog ();

  QLineEdit * IconEdit, * OutputEdit, * NameEdit, * ShortDescEdit,
    * LongDescEdit;
  QPushButton * BrowseButt;
  QLabel * IconButt;
  QCheckBox * RecreateCheck;
  Q3ButtonGroup * toggleGroupDev, * toggleGroupTyp;

private slots:
  void slotOk ();
  void slotBrowse ();

private:
  TextDoc * Doc;
  QRegExp Expr;
  QRegExpValidator * Validator;
};

#endif
