/***************************************************************************
                          componentdialog.h  -  description
                             -------------------
    begin                : Tue Sep 9 2003
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

#ifndef COMPONENTDIALOG_H
#define COMPONENTDIALOG_H

#include "component.h"

#include <QtGui>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QRegExp>
#include <QComboBox>
#include <QPushButton>
#include <QDir>
#include <QVBoxLayout>

class Schematic;
class QValidator;
class QIntValidator;
class QRegExpValidator;


class ComponentDialog : public QDialog {
   Q_OBJECT
public:
  ComponentDialog(Component*, Schematic*);
 ~ComponentDialog();

private slots:
  void slotButtOK();
  void slotButtCancel();
  void slotSelectProperty(QTableWidgetItem *item);
  void slotApplyInput();
  void slotApplyState(int State);
  void slotBrowseFile();
  void slotEditFile();
  void slotApplyChange(const QString& Text);
  void slotApplyProperty();
  void slotApplyPropName();

  void slotButtAdd();
  void slotButtRem();

  void slotSimTypeChange(int);
  void slotNumberChanged(const QString&);
  void slotStepChanged(const QString&);

  void slotParamEntered();
  void slotSimEntered(int);
  void slotValuesEntered();
  void slotStartEntered();
  void slotStopEntered();
  void slotStepEntered();
  void slotNumberEntered();

protected slots:
    void reject();

private:
  QVBoxLayout *all;   // the mother of all widgets
  QValidator  *Validator, *ValRestrict, *Validator2;
  QRegExp     Expr;
  QIntValidator *ValInteger;
  QTableWidget  *prop;
  QLineEdit   *edit, *NameEdit, *CompNameEdit;
  QComboBox   *ComboEdit;
  QLabel      *Name, *Description;
  QPushButton *BrowseButt, *EditButt, *ButtAdd, *ButtRem;
  QCheckBox   *disp;
  Component   *Comp;
  Schematic   *Doc;
  bool        changed;
  int         tx_Dist, ty_Dist;   // remember the text position

  QLabel    *textType;
  QLabel    *textSim, *textParam, *textValues, *textStart, *textStop,
            *textStep, *textNumber;
  QLineEdit *editParam, *editValues, *editStart, *editStop,
            *editStep, *editNumber;
  QCheckBox *checkSim, *checkParam, *checkValues, *checkStart, *checkStop,
            *checkNumber, *checkType, *showName;
  QComboBox *comboSim, *comboType;
};

#endif
