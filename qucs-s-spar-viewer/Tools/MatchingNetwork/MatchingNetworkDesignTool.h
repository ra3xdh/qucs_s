/// @file MatchingNetworkDesignTool.h
/// @brief GUI for the matching network synthesis tool (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef MATCHINGNETWORKDESIGNTOOL_H
#define MATCHINGNETWORKDESIGNTOOL_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"

#include "LoadSpecificationWidget.h"
#include "MatchingNetworkDesigner.h" // Class to create the matching network
#include "MatchingNetworkParametersWidget.h" // Custom widget for the network setup data entry

/// @class MatchingNetworkDesignTool
/// @brief Main widget for matching network design
class MatchingNetworkDesignTool : public QWidget {
  Q_OBJECT
public:
  /// @brief Class constructor
  /// @param parent Parent widget
  MatchingNetworkDesignTool(QWidget* parent = nullptr);

  /// @brief Class destructor
  ~MatchingNetworkDesignTool();

  /// @brief Trigger a design from the main application
  void design();

private slots:
  /// @brief Update design parameters and synthesize network
  void UpdateDesignParameters();

  /// @brief Adjust widget visibility based on 1-port or 2-port mode
  void AdjustOneTwoPortMatchingWidgetsVisibility();

private: 
  /// @brief Get frequency scale multiplier
  /// @param index Scale index (0=GHz, 1=MHz, 2=kHz, 3=Hz)
  /// @return Scale multiplier
  double getScaleFreq(int);

  QCheckBox* TwoPortCheckBox; ///< 1-port or 2-port matching selection

  MatchingNetworkParametersWidget* InputMatchingSetupWidget; ///< Input matching network parameters
  MatchingNetworkParametersWidget* OutputMatchingSetupWidget; ///< Output matching network parameters
  LoadSpecificationWidget* LoadSpecWidget; ///< Load specification widget

  QLabel *Zout_Label, *Ohm_Zout_Label, *Zout_J;
  QDoubleSpinBox *ZoutISpinBox, *ZoutRSpinBox;

  QLabel* f_match_Label; ///< Matching frequency label

  /// @name Two-port network widgets
  /// @{
  QGroupBox* two_port_GroupBox;
  QCheckBox* enter_S2P_file_CheckBox;
  QPushButton* browse_S2P_Button;
  QLabel* s2p_filename_Label;
  /// @}

  QLabel* input_format_Label;
  QComboBox* input_format_Combo;

  /// @name S-parameter labels
  /// @{
  QLabel *S11_Label, *S12_Label, *S21_Label, *S22_Label;
  QLabel *S11_Separator_Label, *S12_Separator_Label, *S21_Separator_Label,
      *S22_Separator_Label;
  /// @}

  /// @name S-parameter spinboxes
  /// @{
  QDoubleSpinBox *S11_A_SpinBox, *S11_B_SpinBox;
  QDoubleSpinBox *S12_A_SpinBox, *S12_B_SpinBox;
  QDoubleSpinBox *S21_A_SpinBox, *S21_B_SpinBox;
  QDoubleSpinBox *S22_A_SpinBox, *S22_B_SpinBox;
  /// @}

  QDoubleSpinBox *f_match_Spinbox, *FreqEnd_Spinbox;
  QComboBox *f_match_Scale_Combo, *FreqEnd_Scale_Combo,
      *StubTermination_ComboBox;

  SchematicContent SchContent; ///< Schematic representation

  /// @name Trace widgets
  /// @{
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;
  /// @}

signals:
  /// @brief Emitted when schematic needs to be updated
  /// @param content Schematic content
  void updateSchematic(SchematicContent);

  /// @brief Emitted when simulation needs to be updated
  /// @param content Schematic content
  void updateSimulation(SchematicContent);
};

#endif // MATCHINGNETWORKDESIGNTOOL_H
