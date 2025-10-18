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

#include "../../Schematic/Network.h"

#include "Wilkinson2Way.h"
#include "MultistageWilkinson.h"
#include "TJunction.h"
#include "Branchline.h"
#include "DoubleBoxBranchline.h"
#include "Bagley.h"
#include "Gysel.h"
#include "Lim_Eom.h"
#include "Wilkinson3way_ImprovedIsolation.h"
#include "Recombinant3WayWilkinson.h"


#define WILKINSON 0
#define MULTISTAGE_WILKINSON 1
#define T_JUNCTION 2
#define BRANCHLINE 3
#define DOUBLE_BOX_BRANCHLINE 4
#define BAGLEY 5
#define GYSEL 6
#define LIM_EOM 7
#define WILKINSON_3_WAY_IMPROVED_ISO 8
#define RECOMBINANT_3_WAY_WILKINSON 9
#define TRAVELLING_WAVE 10
#define TREE 11



class BagleyValidator : public QValidator {
  Q_OBJECT
public:
  BagleyValidator(QObject* parent = 0) : QValidator(parent){};
  virtual State validate(QString& input, int& /*pos*/) const {
    if (input.isEmpty()) {
      return Acceptable;
    }

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
  PowerCombiningTool(QWidget* parent = nullptr);
  ~PowerCombiningTool();
  void design();
  SchematicContent getSchematic();

private slots:
  void UpdateDesignParameters();
  void on_TopoCombo_currentIndexChanged(int);

private:
  QLabel *OhmLabel, *K1LabeldB, *NStagesLabel, *K1Label, *FreqLabel, *RefImp,
      *TopoLabel;
  QLabel *AlphaLabel, *AlphadBLabel, *UnitsLabel;
  QDoubleSpinBox *RefImpSpinbox, *FreqSpinbox, *AlphaSpinbox;
  QDoubleSpinBox *K1Spinbox, *K2Spinbox, *K3Spinbox;
  QSpinBox* NStagesSpinbox;
  QComboBox* BranchesCombo;
  QLabel* number_Output_Label;
  QComboBox *TopoCombo, *FreqScaleCombo, *UnitsCombo;
  double getScaleFreq();
  QString netlist;

  PowerCombinerParams Specs;
  SchematicContent SchContent; // Schematic representation

  // Transmission line implementation
  QLabel* TL_Implementation_Label;
  QComboBox* TL_Implementation_Combo;

  // Input validation
  QValidator* Bagley_Validator;

  // Add trace to simulate
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;

  void synthesize();

  // Functions for changing the default settings based on the topology
  void setSettings_Wilkinson();
  void setSettings_MultistageWilkinson();
  void setSettings_T_Junction();
  void setSettings_Branchline();
  void setSettings_DoubleBoxBranchline();
  void setSettings_Bagley();
  void setSettings_Gysel();
  void setSettings_LimEom();
  void setSettings_Wilkinson_3_Way_Improved_Isolation();
  void setSettings_Recombinant_3_Way_Wilkinson();
  void setSettings_Travelling_Wave();
  void setSettings_Tree();
  void setDefaultSettings();


signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // POWERCOMBININGTOOL_H
