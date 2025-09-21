/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
#ifndef POWERCOMBININGTOOL_H
#define POWERCOMBININGTOOL_H
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QValidator>
#include <QWidget>

#include "PowerCombinerDesigner.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"

class BagleyValidator : public QValidator {
  Q_OBJECT
public:
  BagleyValidator(QObject *parent = 0) : QValidator(parent){};
  virtual State validate(QString &input, int &/*pos*/) const {
    if (input.isEmpty())
      return Acceptable;

    bool b;
    int val = input.toInt(&b);

    if ((b == true) && (val % 2 != 0)) {
      return Acceptable;
    }
    return Invalid;
  }
};

class PowerCombiningTool : public QWidget {
  Q_OBJECT
public:
  PowerCombiningTool(QWidget *parent = nullptr);
  ~PowerCombiningTool();
  void design();
  SchematicContent getSchematic();

private slots:
  void UpdateDesignParameters();
  void on_TopoCombo_currentIndexChanged(int);

private:
  QLabel *OhmLabel, *K1LabeldB, *NStagesLabel, *K1Label, *FreqLabel, *RefImp, *TopoLabel;
  QLabel *AlphaLabel, *AlphadBLabel, *UnitsLabel;
  QDoubleSpinBox *RefImpSpinbox, *FreqSpinbox, *AlphaSpinbox;
  QDoubleSpinBox *K1Spinbox, *K2Spinbox, *K3Spinbox;
  QSpinBox *NStagesSpinbox;
  QComboBox *BranchesCombo;
  QComboBox *TopoCombo, *FreqScaleCombo, *UnitsCombo, *ImplementationCombobox;
  QGroupBox *ImplementationgroupBox;
  QRadioButton *IdealTLradioButton, *MicrostripradioButton, *LumpedElementsradioButton;

  double getScaleFreq();
  QString netlist;
  SchematicContent SchContent; // Schematic representation

         // Transmission line implementation
  QLabel *TL_Implementation_Label;
  QComboBox *TL_Implementation_Combo;

  // Input validation
  QValidator *Bagley_Validator;

  // Add trace to simulate
  QLabel *traceNameLabel;
  QLineEdit *traceNameLineEdit;

signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // POWERCOMBININGTOOL_H
