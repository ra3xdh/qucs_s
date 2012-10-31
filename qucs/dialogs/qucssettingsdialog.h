/***************************************************************************
                           qucssettingsdialog.h
                          ----------------------
    begin                : Sun May 23 2004
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef QUCSSETTINGSDIALOG_H
#define QUCSSETTINGSDIALOG_H

#include "qucs.h"

#include <qdialog.h>
#include <qfont.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

class Q3ListView;
class Q3ListViewItem;
class QLineEdit;
class QCheckBox;
class Q3VBoxLayout;
class QPushButton;
class QComboBox;
class QIntValidator;
class QRegExpValidator;


class QucsSettingsDialog : public QDialog  {
   Q_OBJECT
public:
  QucsSettingsDialog(QucsApp *parent=0, const char *name=0);
 ~QucsSettingsDialog();

private slots:
  void slotOK();
  void slotApply();
  void slotFontDialog();
  void slotBGColorDialog();
  void slotDefaultValues();
  void slotAdd();
  void slotRemove();
  void slotEditSuffix(Q3ListViewItem*);
  void slotColorComment();
  void slotColorString();
  void slotColorInteger();
  void slotColorReal();
  void slotColorCharacter();
  void slotColorDataType();
  void slotColorAttribute();
  void slotColorDirective();
  void slotColorTask();

public:
  QucsApp *App;

  QFont Font;
  QCheckBox *checkWiring;
  QComboBox *LanguageCombo;
  QPushButton *FontButton, *BGColorButton;
  QLineEdit *undoNumEdit, *editorEdit, *Input_Suffix, *Input_Program;
  Q3ListView *List_Suffix;
  QPushButton *ColorComment, *ColorString, *ColorInteger,
       *ColorReal, *ColorCharacter, *ColorDataType, *ColorAttribute,
       *ColorDirective, *ColorTask;

  Q3VBoxLayout *all;
  QIntValidator *val200;
  QRegExp Expr;
  QRegExpValidator *Validator;
};

#endif
