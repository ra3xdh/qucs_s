/***************************************************************************
                              settingsdialog.h
                             ------------------
    begin                : Mon Oct 20 2003
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

class Schematic;
class QLineEdit;
class Q3TextEdit;
class QCheckBox;
class QComboBox;
class Q3VBoxLayout;
class QRegExpValidator;


class SettingsDialog : public QDialog  {
   Q_OBJECT
public:
  SettingsDialog(Schematic*);
 ~SettingsDialog();

private slots:
  void slotOK();
  void slotApply();

public:
  Schematic *Doc;

  QComboBox *Combo_Frame;
  Q3TextEdit *Input_Frame0;
  QLineEdit *Input_Frame1, *Input_Frame2, *Input_Frame3;
  QLineEdit *Input_DataSet, *Input_DataDisplay, *Input_Script;
  QLineEdit *Input_GridX, *Input_GridY;
  QCheckBox *Check_OpenDpl, *Check_GridOn, *Check_RunScript;

  Q3VBoxLayout *all;
  QRegExpValidator *valExpr;
};

#endif
