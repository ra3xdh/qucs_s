/***************************************************************************
                            optimizedialog.h
                           ------------------
    begin                : Sun Jul 30 2006
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

#ifndef OPTIMIZEDIALOG_H
#define OPTIMIZEDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

class Schematic;
class Optimize_Sim;
class QTableWidget;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QVBoxLayout;
class QRegExpValidator;
class QDoubleValidator;
class QIntValidator;


class OptimizeDialog : public QDialog  {
Q_OBJECT
public:
  OptimizeDialog(Optimize_Sim*, Schematic*);
 ~OptimizeDialog();

private slots:
  void slotOK();
  void slotApply();
  void slotCancel();
  void slotAddVariable();
  void slotDeleteVariable();
  void slotAddGoal();
  void slotDeleteGoal();
  void slotEditGoal();
  void slotEditVariable();
  void slotChangeVarName(const QString&);
  void slotChangeVarActive(bool);
  void slotChangeVarInit(const QString&);
  void slotChangeVarMin(const QString&);
  void slotChangeVarMax(const QString&);
  void slotChangeVarType(const QString&);
  void slotChangeGoalName(const QString&);
  void slotChangeGoalNum(const QString&);
  void slotChangeGoalType(const QString&);
  void slotCreateEqn();
  void slotSetPrecision(const QPoint&);

private:
  void createASCOFiles();

public:
  Optimize_Sim *Comp;
  Schematic *Doc;
  bool changed;
  int numPrec;

  QVBoxLayout *all;
  QLineEdit *NameEdit, *VarNameEdit,
            *VarInitEdit, *VarMinEdit, *VarMaxEdit,
            *IterEdit, *RefreshEdit, *ParentsEdit, *ConstEdit, *CrossEdit,
            *SeedEdit, *CostVarEdit, *CostObjEdit, *CostConEdit,
            *GoalNameEdit, *GoalNumEdit;
  QCheckBox *VarActiveCheck;
  QComboBox *SimEdit, *GoalTypeCombo, *MethodCombo, *VarTypeCombo;
  QTableWidget *VarTable, *GoalTable;

  QRegularExpression Expr;
  QRegularExpressionValidator *Validator;
  QDoubleValidator *numVal;
  QIntValidator *intVal;
};

#endif
