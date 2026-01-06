/// @file AttenuatorDesignTool.h
/// @brief GUI for the RF attenuator design tool (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef ATTENUATORDESIGNTOOL_H
#define ATTENUATORDESIGNTOOL_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QWidget>

#include "../../Schematic/Network.h"

#include "BridgedTeeAttenuator.h"
#include "LPadFirstSeries.h"
#include "LPadFirstShunt.h"
#include "PiAttenuator.h"
#include "QW_SeriesAttenuator.h"
#include "QW_ShuntAttenuator.h"
#include "RSeriesAttenuator.h"
#include "ReflectionAttenuator.h"
#include "RshuntAttenuator.h"
#include "TeeAttenuator.h"


#define PI_ATTENUATOR 0          ///< Pi topology
#define TEE_ATTENUATOR 1         ///< Tee topology
#define BRIDGED_TEE 2            ///< Bridged-Tee topology
#define REFLECTION_ATTENUATOR 3  ///< Reflection topology
#define QW_SERIES 4              ///< Quarter-wave series topology
#define QW_SHUNT 5               ///< Quarter-wave shunt topology
#define LPAD_1ST_SERIES 6        ///< L-pad first series topology
#define LPAD_1ST_SHUNT 7         ///< L-pad first shunt topology
#define RSERIES 8                ///< Series resistor topology
#define RSHUNT 9                 ///< Shunt resistor topology

/// @class AttenuatorDesignTool
/// @brief GUI tool for RF attenuator design tool
class AttenuatorDesignTool : public QWidget {
  Q_OBJECT
public:
  /// @brief Class constructor
  /// @param parent Parent widget
  AttenuatorDesignTool(QWidget* parent = nullptr);

  /// @brief Class destructor
  ~AttenuatorDesignTool();

  /// @brief Trigger design process
  void design() { UpdateDesignParameters(); }

  /// @brief Synthesize attenuator circuit based on selected topology
  void synthesize();

private slots:
  /// @brief Update design parameters from GUI and trigger synthesis
  void UpdateDesignParameters();

  /// @brief Update power dissipation displays with unit conversion
  void UpdatePowerDissipationData();

  /// @brief Handle topology selection changes
  /// @param index Selected topology index
  void on_TopoCombo_currentIndexChanged(int);

private:
  /// @name Topology widgets
  ///@{
  QLabel* Topology_Label;
  QComboBox* Topology_Combo;
  ///@}

  /// @name Attenuation widgets
  ///@{
  QLabel* Attenuation_Label;
  QLabel* dBLabelAtt;
  QDoubleSpinBox* AttenuationSpinBox;
  ///@}

  /// @name Input impedance widgets
  ///@{
  QLabel* Zin_Label;
  QLabel* Ohm_Zin_Label;
  QDoubleSpinBox* ZinSpinBox;
  ///@}

  /// @name Output impedance widgets
  ///@{
  QLabel* Zout_Label;
  QLabel* Ohm_Zout_Label;
  QDoubleSpinBox* ZoutSpinBox;
  ///@}

  /// @name Input power widgets
  ///@{
  QLabel* Pin_Label;
  QDoubleSpinBox* Pin_SpinBox;
  QComboBox* Pin_units_Combo;
  ///@}

  /// @name Frequency widgets
  ///@{
  QLabel* freqLabel;
  QDoubleSpinBox* freqSpinBox;
  QComboBox* FreqScaleCombo;
  ///@}

  /// @name Power dissipation widgets
  ///@{
  QLabel* Pdiss_R1_Label;
  QLineEdit* Pdiss_R1_Lineedit;
  QComboBox* R1_Pdiss_Units_Combo;

  QLabel* Pdiss_R2_Label;
  QLineEdit* Pdiss_R2_Lineedit;
  QComboBox* R2_Pdiss_Units_Combo;

  QLabel* Pdiss_R3_Label;
  QLineEdit* Pdiss_R3_Lineedit;
  QComboBox* R3_Pdiss_Units_Combo;

  QLabel* Pdiss_R4_Label;
  QLineEdit* Pdiss_R4_Lineedit;
  QComboBox* R4_Pdiss_Units_Combo;
  ///@}

  /// @brief Get frequency in Hz from spinbox and scale combo
  double getFreq();

  /// @brief Convert power to Watts
  /// @param Pin Power value
  /// @param index Unit index (0=mW, 1=W, 2=dBm, 3-6=dBµV/dBmV variants)
  double getPowerW(double, unsigned int);

  /// @brief Convert power from Watts to selected units
  /// @param Pin Power in Watts
  /// @param index Unit index
  double ConvertPowerFromW(double, unsigned int);

  // Transmission line implementation
  QLabel* TL_Implementation_Label;
  QComboBox* TL_Implementation_Combo;

  // Add trace to simulate
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;

  AttenuatorDesignParameters Specs; ///< Design specifications

  /////////////////////////////////////////////////////////////////////////////////////
  // This info is calculated in the synthesis functions and then passed to the tool class
  SchematicContent SchContent; ///< Generated schematic
  QMap<QString, double> Pdiss; ///< Power dissipation per resistor (Watts)

signals:
  /// @brief Emitted when schematic needs updating
  void updateSchematic(SchematicContent);

  /// @brief Emitted when simulation needs updating
  void updateSimulation(SchematicContent);
};

#endif // ATTENUATORDESIGNTOOL_H
