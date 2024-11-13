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

#include <QDialog>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextEdit>

class Schematic;

class QValidator;
class QIntValidator;
class QRegExpValidator;
class QTableWidget;
class QTableWidgetItem;
class QLabel;
class QLineEdit;
class QCheckBox;
class QRegExp;
class QComboBox;
class QPushButton;
class QGridLayout;

class ParamWidget;
class ParamLineEdit;
class ParamCombo;

class ComponentDialog : public QDialog 
{
Q_OBJECT
public:
  ComponentDialog(Component*, Schematic*);
 ~ComponentDialog();

private slots:
  void slotOKButton();
  void slotApplyButton();

  void slotBrowseFile(QLineEdit* lineEdit);
  // void slotEditFile();

  void slotFillFromSpice();

private:
  QIntValidator* intVal;
  QValidator* compNameVal;
  QValidator* nameVal;
  QValidator* paramVal;
  
  QTableWidget* propertyTable;
  QTextEdit* eqnEditor;
  QComboBox* eqnSimCombo = nullptr;
  QCheckBox* eqnExportCheck = nullptr;

  Component* component;
  Schematic* document;
  
  // Maps to contain specialisations for different sweep types. Note: the arrangement
  // has been chosen to have the smallest number of map elements.
  // TODO: Make these constant once settled, see https://stackoverflow.com/questions/2636303/how-to-initialize-a-private-static-const-map-in-c
  QHash<QPair<QString, QString>, QString> sweepTypeSpecialLabels;
  QHash<QString, QStringList> sweepTypeEnabledParams;
  QHash<QString, QStringList> paramsHiddenBySim;
  
  // TODO: It would be better for simulations with sweeps to have a flag saying so.
  bool isEquation;
  bool hasSweep;
  bool hasFile = false;
  QStringList sweepProperties;
  
  // Maps to store references to edit widgets.
  QMap<QString, ParamWidget*> sweepParamWidget;

  ParamLineEdit* componentNameWidget;
  int tx_Dist, ty_Dist;   // remember the text position
 
  QStringList getSimulationList();

  void updateSweepProperty(const QString& propertyWidget, const QString& value);
  void updateSweepWidgets(const QString& simType);
  void updatePropertyTable();
  void updateEqnEditor();
  void updateEqnSimulation(const QString& propertyWidget, const QString& value) { (void)propertyWidget; (void)value; };

  void writeSweepProperties();
  void writePropertyTable();
  void writeEquation();

  void simpleEditEqn(QLineEdit* lineEdit);

  enum CELL_TYPE { LabelCell = 1000, TextEditCell, ComboBoxCell, CompoundCell, CheckBoxCell };
};

class EqnHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
  EqnHighlighter(const QString& keywordSet, QTextDocument* parent);

protected:
  void highlightBlock(const QString &text) override;

private:
  struct HighlightingRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
  };
  QList<HighlightingRule> highlightingRules;

  QTextCharFormat keywordFormat;
  QTextCharFormat quotationFormat;
  QTextCharFormat functionFormat;
};

class SimpleEqnDialog : public QDialog
{
Q_OBJECT

public:
  SimpleEqnDialog(QString& string, QWidget* parent);

private slots:
  void slotOkButton();

private:
  QString& mText;
  QTextEdit* mEditor; 
};

#endif
