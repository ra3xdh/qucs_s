/// @file PowerCombiningTool.h
/// @brief Widget for power combining network design and synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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


/// @class BagleyValidator
/// @brief Validator for Bagley topology ensuring odd number of outputs
class BagleyValidator : public QValidator {
  Q_OBJECT
public:
  /// @brief Constructor
  /// @param parent Parent object
  BagleyValidator(QObject* parent = 0) : QValidator(parent){}

  /// @brief Validate input to ensure odd numbers only
  /// @param input Input string to validate
  /// @param pos Current cursor position
  /// @return Validation state (Acceptable or Invalid)
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

/// @class PowerCombiningTool
/// @brief Widget for power combining network design and synthesis
class PowerCombiningTool : public QWidget {
  Q_OBJECT
public:
  /// @brief Class constructor
  /// @param parent Parent widget
  PowerCombiningTool(QWidget* parent = nullptr);

  /// @brief Class destructor
  ~PowerCombiningTool();

  /// @brief Trigger design update
  void design() { UpdateDesignParameters(); }

  /// @brief Get current schematic content
  /// @return Schematic content structure
  SchematicContent getSchematic() { return SchContent; }

private slots:
  /// @brief Update design parameters from UI controls
  void UpdateDesignParameters();

  /// @brief Handle topology combo box changes
  /// @param index New topology index
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

  /// @brief Get frequency scaling factor
  /// @return Scale multiplier (1, 1e3, 1e6, or 1e9)
  double getScaleFreq();
  QString netlist;

  /// @brief Power combiner specifications
  PowerCombinerParams Specs;
  SchematicContent SchContent; /// < Schematic representation

  // Transmission line implementation
  QLabel* TL_Implementation_Label;
  QComboBox* TL_Implementation_Combo;

  // Input validation
  QValidator* Bagley_Validator;

  // Add trace to simulate
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;

  /// @brief Synthesize network based on current parameters
  void synthesize();

  /// @brief Configure UI for Wilkinson topology
  void setSettings_Wilkinson();

  /// @brief Configure UI for multistage Wilkinson topology
  void setSettings_MultistageWilkinson();

  /// @brief Configure UI for T-junction topology
  void setSettings_T_Junction();

  /// @brief Configure UI for branchline topology
  void setSettings_Branchline();

  /// @brief Configure UI for double box branchline topology
  void setSettings_DoubleBoxBranchline();

  /// @brief Configure UI for Bagley topology
  void setSettings_Bagley();

  /// @brief Configure UI for Gysel topology
  void setSettings_Gysel();

  /// @brief Configure UI for Lim-Eom topology
  void setSettings_LimEom();

  /// @brief Configure UI for 3-way Wilkinson with improved isolation
  void setSettings_Wilkinson_3_Way_Improved_Isolation();

  /// @brief Configure UI for recombinant 3-way Wilkinson topology
  void setSettings_Recombinant_3_Way_Wilkinson();

  /// @brief Configure UI for travelling wave topology
  void setSettings_Travelling_Wave();

  /// @brief Configure UI for tree topology
  void setSettings_Tree();

  /// @brief Reset UI to default settings
  void setDefaultSettings();


signals:
  /// @brief Signal emitted when schematic needs update
  /// @param content Updated schematic content
  void updateSchematic(SchematicContent);

  /// @brief Signal emitted when simulation needs update
  /// @param content Updated schematic content
  void updateSimulation(SchematicContent);
};

#endif // POWERCOMBININGTOOL_H
