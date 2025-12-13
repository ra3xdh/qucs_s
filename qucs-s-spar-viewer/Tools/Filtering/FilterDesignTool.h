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

#ifndef FILTERDESIGNTOOL_H
#define FILTERDESIGNTOOL_H
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include "../../Schematic/Network.h"
#include "../../Misc/general.h" // Get scale function

#include "CanonicalFilter.h"
#include "CapacitivelyCoupledShuntResonatorsFilter.h"
#include "CoupledLineBandpassFilter.h"
#include "./DirectCoupledFilters/DirectCoupledFilters.h"
#include "EllipticFilter.h"
#include "EndCoupled.h"
#include "QuarterWaveFilters.h"
#include "SteppedImpedanceFilter.h"

/*#include "Filtering/CoupledLineHarmonicRejectionSIRBandpassFilter.h"*/

#define LC_LADDER 0
#define LC_DIRECT_COUPLED 1
#define STEPPED_IMPEDANCE 2
#define QUARTERWAVE 3
#define CAPACITIVELY_COUPLED_RESONATORS 4
#define COUPLED_LINES 5
#define END_COUPLED 6
#define SEMILUMPED 7

#define COUPLED_LINE_SIR 9

class FilterDesignTool : public QWidget {
  Q_OBJECT
public:
  FilterDesignTool(QWidget* parent = nullptr);
  ~FilterDesignTool();
  void design();

private slots:
  void UpdateDesignParameters();
  void ResposeComboChanged();
  void ImplementationComboChanged(int);
  void EllipticTypeChanged();

  ///
  /// \brief Opens a dialog to adjust the resonator's variables in direct-coupled filters
  ///
  void openResonatorValuesDialog();

  ///
  /// \brief Adjusts the tunable variables in the resonators (Direct-Coupled filters only)
  /// This function is called when the type of Direct-Coupled filter is changed.
  /// The synthesis procedure of Direct-Coupled filters (see Matthaei's book) allows de user
  /// to specify custom values for the inductance/capacitance of the resonators. This depends
  /// on the topology
  ///
  /// Topology                       |  Tunable element
  /// C-coupled shunt resonators     |       L
  /// L-coupled shunt resonators     |       C
  /// C-coupled series resonators    |       L
  /// L-coupled series resonators    |       C
  /// Magnetic-coupled               |      None
  /// Quarter-wave line coupled      |      Nobe
  ///
  void setAdjustableResonatorVariables_DirectCoupled();

private:
  // ************************** FILTER DESIGN ***************************
  QWidget* SetupFilterDesignGUI();
  QComboBox *FilterResponseTypeCombo,
      *FilterImplementationCombo, *FC_ScaleCombobox, *BW_ScaleCombobox,
      *SemiLumpedImplementationCombo;
  QComboBox *EllipticType;
  QComboBox  *DC_CouplingTypeCombo;
  QDoubleSpinBox *FCSpinbox, *BWSpinbox, *RippleSpinbox, *StopbandAttSpinbox,
      *MinimumZ_Spinbox, *MaximumZ_Spinbox, *ImpedanceRatio_Spinbox;
  QSpinBox* OrderSpinBox;

  QLineEdit* SourceImpedanceLineEdit;

  QComboBox* TopologyCombo; // 1st series or 1st shunt configuration

  QLabel *StopbandAttLabel, *StopbandAttdBLabel, *EllipticTypeLabel,
      *RippleLabel, *RippledBLabel, *DC_CouplingLabel, *MaximumZ_Unit_Label,
      *MinimumZ_Unit_Label, *SemiLumpedImplementationLabel,
      *ImpedanceRatio_Label;

  QLabel * ClassLabel;
  QComboBox *FilterClassCombo;

  QLabel *RLlabel, *RLlabelOhm, *PhaseErrorLabel, *PhaseErrorLabelDeg,
      *MinimumZLabel, *MaximumZLabel, *FC_Label, *BW_Label; // Zverev mode
  QStringList DefaultFilterResponses;

  // Transmission line implementation
  QLabel* TL_Implementation_Label;
  QComboBox* TL_Implementation_Combo;

  // Filter specs
  struct FilterSpecifications Filter_SP;
  SchematicContent SchContent;

  // Direct-coupled filters only. Adjust resonator elements
  QPushButton* ResonatorValuesButton_DC;
  std::vector<double> resonatorValues;
  std::vector<QString> resonatorScaleValues;

  // Widgets for the resonator values in Direct-Coupled filters
  std::vector<QDoubleSpinBox *> ResonatorSpinboxes; // Value
  std::vector<QComboBox*> ResonatorScaleComboboxes; // Scale


  // Substrate
  MS_Substrate MS_Subs;

  QStringList setItemsResponseTypeCombo();
  double getScale(QString); // Gets the scale from the comboboxes

  // Add trace to simulate
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;

  // Set UI settings depending on filter topology
  void setSettings_LC_Ladder();
  void setSettings_LC_Direct_Coupled();
  void setSettings_Stepped_Z_LPF();
  void setSettings_Quarterwavelength_BPF_BSF();
  void setSettings_EndCoupled_BPF();
  void setSettings_CCoupledShuntResonators_BPF();
  void setSettings_Semilumped();
  void setSettings_SideCoupled_BPF();

  ///
  /// \brief Checks the center frequency and the bandwidth selected by the user.
  /// If the relativa BW exceeds the limit (fixed by certain filter topology), the BW is adjuster
  /// \param max_rel_bw: Maximum admisible relative bandwidth
  ///
  void adjustRelativeBW(double max_rel_bw);

  ///
  /// \brief Direct-coupled filters only. Get a hint of a resonators' component value depending on the frequency
  /// \param freq: Frequency in Hz
  /// \param Component: "Inductor" or "Capacitor"
  /// \returns Hint of the component value to have a suitable resonator for a given frequency
  double getResonatorComponentValueHint(double freq, ComponentType component);

public:
  void synthesize();
  void set_MS_Subs(MS_Substrate MS_Subs);

signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // FILTERDESIGNTOOL_H
