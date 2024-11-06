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

class BoundLineEdit;
class ParamWidget;

// struct SweepUISettingType;
struct QStringPair;

class ComponentDialog : public QDialog {
   Q_OBJECT
public:
  ComponentDialog(Component*, Schematic*);
 ~ComponentDialog();

private slots:
  void slotOKButton();
  void slotApplyButton();
  // void slotCancelButton();

  void slotSelectProperty(QTableWidgetItem *item);
  void slotApplyState(int State);
  void slotBrowseFile();
  void slotEditFile();
  void slotApplyChange(int idx);
  void slotApplyProperty();
  void slotApplyPropName();

  void slotButtAdd();
  void slotButtRem();

  void slotButtUp();
  void slotButtDown();

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
  void slotHHeaderClicked(int headerIdx);
  void slotFillFromSpice();

  void slotUpdateSweepWidgets(const QString& sweepType, QGridLayout* sweepPageLayout);

protected slots:
    // void reject();
    bool eventFilter(QObject *obj, QEvent *event);

private:
  QValidator  *Validator, *ValRestrict, *Validator2,
              *ValName;
  QRegularExpression     Expr;
  QIntValidator *ValInteger;
  
  QTableWidget  *propertyTable;
  Component   *originalComponent;
  Schematic   *document;
  Component   editComponent;
  
  // Maps to contain specialisations for different sweep types. Note: the arrangement
  // has been chosen to have the smallest number of map elements.
  // TODO: Make these constant once settled, see https://stackoverflow.com/questions/2636303/how-to-initialize-a-private-static-const-map-in-c
  QMap<QString, QList<QStringPair> > sweepTypeSpecialLabels;
  QMap<QString, QStringList> sweepTypeEnabledParams;
  QMap<QString, QStringList> sweepTypeHiddenParams;
  
  // Maps to store references to edit widgets.
  QMap<QString, ParamWidget*> sweepParam;

  BoundLineEdit* mpNameLineEdit;

  QLineEdit   *edit, *NameEdit, *CompNameEdit;
  QComboBox   *ComboEdit;
  QLabel      *Name, *Description;
  QPushButton *BrowseButt, *EditButt, *ButtAdd, *ButtRem;
  QPushButton *ButtUp, *ButtDown;
  QPushButton *ButtFillFromSpice;
  QCheckBox   *disp;

  bool        changed;
  int         tx_Dist, ty_Dist;   // remember the text position
  bool        setAllVisible; // used for toggling visibility of properties
 
  QLabel    *textType;
  QLabel    *textSim, *textParam, *textValues, *textStart, *textStop,
            *textStep, *textNumber;
  QLineEdit *editParam, *editValues, *editStart, *editStop,
            *editStep, *editNumber;
  QCheckBox *checkSim, *checkParam, *checkValues, *checkStart, *checkStop,
            *checkNumber, *checkType, *showName;
  QComboBox *comboSim, *comboType;

  QStringList getSimulationList();

  void updateCompPropsList(void);
  void updateSimProperty(const QString& propertyWidget, const QString& value);
  void updateSimWidgets(const QString& simType);

  void fillPropsFromTable();
  void recreatePropsFromTable();
  bool propChanged(Property *pp, const QString &value, const bool display);
  void updateProperty(Property *pp, const QString &value, const bool display);

  enum CELL_TYPE { LabelCell = 1000, TextEditCell, ComboBoxCell, CheckBoxCell};
};

// Helper structures to manage specialisation of sweep UI presentation.
// enum RELATION { StepToNumber, NumberToStep };

// struct RelatedParameterType
// {
//   RelatedParameterType(const QString& a, RELATION b)
//   : relatedParameter(a), relation(b) {}

//   QString relatedParameter;
//   RELATION relation; 
// };

// struct SweepUISettingType
// {
//   SweepUISettingType(const QString& a, const QString& b, const QStringList& c, bool d, bool e, QList<RelatedParameterType> f = {}) 
//   : label(a), value(b), options(c), enabled(d), visible(e), relatedParameters(f) {}
//   QString label;
//   QString value;
//   QStringList options;
//   bool enabled;
//   bool visible;
//   QList<RelatedParameterType> relatedParameters;
// };
// typedef QList<SweepUISettingType> SweepSettingUIList;

struct QStringPair
{
  QStringPair(const QString& b, const QString& a)
  : first(a), second(b) {}

  QString first;
  QString second;
};


#endif
