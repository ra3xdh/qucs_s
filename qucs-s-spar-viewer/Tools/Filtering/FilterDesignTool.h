/// @file FilterDesignTool.h
/// @brief GUI and control logic for RF filter design and synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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

/// @class FilterDesignTool
/// @brief Interactive filter design tool with real-time synthesis and visualization
///
/// Provides GUI for designing various RF filter topologies including:
/// - LC ladder filters (Butterworth, Chebyshev, Elliptic, Bessel, Gaussian, Legendre)
/// - Direct-coupled resonator filters
/// - Distributed element filters (stepped-impedance, quarter-wave, coupled-line)
/// - Semi-lumped implementations
///
/// Features automatic synthesis, parameter validation, and schematic generation.
class FilterDesignTool : public QWidget {
  Q_OBJECT
public:
  /// @brief Constructor initializes UI and connects signals
  /// @param parent Parent widget
  FilterDesignTool(QWidget* parent = nullptr);

  /// @brief Destructor cleans up UI elements
  ~FilterDesignTool();

  /// @brief Triggers filter design with current parameters
  void design() { UpdateDesignParameters(); }

private slots:
  /// @brief Updates filter design parameters and triggers synthesis
  void UpdateDesignParameters();

  /// @brief Handles filter response type changes (Butterworth, Chebyshev, etc.)
  void ResposeComboChanged();

  /// @brief Handles filter implementation type changes
  /// @param index Implementation combo box index
  void ImplementationComboChanged(int);

  /// @brief Handles elliptic filter type changes (Type A, B, C, S)
  void EllipticTypeChanged();

  /// @brief Opens dialog to adjust resonator component values
  /// @details Direct-coupled filters only
  void openResonatorValuesDialog();

  /// @brief Adjusts the tunable variables in the resonators (Direct-Coupled filters only)
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
  /// Quarter-wave line coupled      |      None
  void setAdjustableResonatorVariables_DirectCoupled();

private:
  /// @name UI Components - Filter Implementation
  /// @{
  QComboBox* FilterImplementationCombo;     ///< Filter topology selector
  QComboBox* TopologyCombo;                 ///< First element type (series/shunt)
  QLabel* TL_Implementation_Label;          ///< Transmission line type label
  QComboBox* TL_Implementation_Combo;       ///< Transmission line type (ideal/microstrip)
  /// @}

  /// @name UI Components - Filter Response
  /// @{
  QComboBox* FilterResponseTypeCombo;       ///< Response type (Butterworth, Chebyshev, etc.)
  QLabel* RippleLabel;                      ///< Ripple parameter label
  QDoubleSpinBox* RippleSpinbox;            ///< Passband ripple in dB
  QLabel* RippledBLabel;                    ///< Ripple unit label
  QLabel* StopbandAttLabel;                 ///< Stopband attenuation label
  QDoubleSpinBox* StopbandAttSpinbox;       ///< Stopband attenuation in dB
  QLabel* StopbandAttdBLabel;               ///< Stopband attenuation unit
  QComboBox* EllipticType;                  ///< Elliptic filter variant selector
  QLabel* EllipticTypeLabel;                ///< Elliptic type label
  QStringList DefaultFilterResponses;       ///< Available response types
  /// @}

  /// @name UI Components - Filter Class and Order
  /// @{
  QLabel* ClassLabel;                       ///< Filter class label
  QComboBox* FilterClassCombo;              ///< Filter class (LP/HP/BP/BS)
  QSpinBox* OrderSpinBox;                   ///< Filter order
  /// @}

  /// @name UI Components - Frequency Specifications
  /// @{
  QLabel* FC_Label;                         ///< Cutoff/center frequency label
  QDoubleSpinBox* FCSpinbox;                ///< Frequency value
  QComboBox* FC_ScaleCombobox;              ///< Frequency scale (GHz/MHz/kHz/Hz)
  QLabel* BW_Label;                         ///< Bandwidth label
  QDoubleSpinBox* BWSpinbox;                ///< Bandwidth value
  QComboBox* BW_ScaleCombobox;              ///< Bandwidth scale
  /// @}

 /// @name UI Components - Impedance Settings
 /// @{
  QLineEdit* SourceImpedanceLineEdit;       ///< Source impedance (typically 50Ω)
  QLabel* MinimumZLabel;                    ///< Minimum impedance label
  QDoubleSpinBox* MinimumZ_Spinbox;         ///< Minimum realizable impedance
  QLabel* MinimumZ_Unit_Label;              ///< Minimum Z unit
  QLabel* MaximumZLabel;                    ///< Maximum impedance label
  QDoubleSpinBox* MaximumZ_Spinbox;         ///< Maximum realizable impedance
  QLabel* MaximumZ_Unit_Label;              ///< Maximum Z unit
  QLabel* ImpedanceRatio_Label;             ///< SIR impedance ratio label
  QDoubleSpinBox* ImpedanceRatio_Spinbox;   ///< Impedance ratio K
  /// @}

  /// @name UI Components - Direct-Coupled Filters
  /// @{
  QComboBox* DC_CouplingTypeCombo;          ///< Coupling type selector
  QLabel* DC_CouplingLabel;                 ///< Coupling type label
  QPushButton* ResonatorValuesButton_DC;    ///< Open resonator adjustment dialog
  std::vector<double> resonatorValues;      ///< Resonator component values
  std::vector<QString> resonatorScaleValues; ///< Resonator component scales
  std::vector<QDoubleSpinBox*> ResonatorSpinboxes;   ///< Resonator value inputs
  std::vector<QComboBox*> ResonatorScaleComboboxes;  ///< Resonator scale selectors
  /// @}

  /// @name UI Components - Semi-Lumped Filters
  /// @{
  QLabel* SemiLumpedImplementationLabel;    ///< Semi-lumped mode label
  QComboBox* SemiLumpedImplementationCombo; ///< Semi-lumped options
  /// @}

  /// @name UI Components - Additional Labels
  /// @{
  QLabel *RLlabel, *RLlabelOhm, *PhaseErrorLabel, *PhaseErrorLabelDeg;
  /// @}

  /// @name UI Components - Trace Management
  /// @{
  QLabel* traceNameLabel;                   ///< Trace name label
  QLineEdit* traceNameLineEdit;             ///< Trace identifier for simulation
  /// @}

  /// @struct FilterSpecifications
  /// @brief Filter specifications structure
  struct FilterSpecifications Filter_SP;

  /// @brief Generated schematic content
  SchematicContent SchContent;

  /// @brief Microstrip substrate parameters
  MS_Substrate MS_Subs;

  /// @brief Populates response type combo box
  /// @return List of available filter responses
  QStringList setItemsResponseTypeCombo();

  /// @brief Gets frequency scale multiplier from combo box
  /// @param scale Scale string (GHz, MHz, kHz, Hz)
  /// @return Multiplier value (1e9, 1e6, 1e3, 1.0)
  double getScale(QString);

  /// @name UI Configuration Methods
  /// @{
  void setSettings_LC_Ladder();                    ///< Configure UI for LC ladder topology
  void setSettings_LC_Direct_Coupled();            ///< Configure UI for direct-coupled topology
  void setSettings_Stepped_Z_LPF();                ///< Configure UI for stepped-impedance LPF
  void setSettings_Quarterwavelength_BPF_BSF();    ///< Configure UI for quarter-wave filters
  void setSettings_EndCoupled_BPF();               ///< Configure UI for end-coupled BPF
  void setSettings_CCoupledShuntResonators_BPF();  ///< Configure UI for capacitively-coupled BPF
  void setSettings_Semilumped();                   ///< Configure UI for semi-lumped filters
  void setSettings_SideCoupled_BPF();              ///< Configure UI for side-coupled BPF
  /// @}

  /// @brief Checks the center frequency and the bandwidth selected by the user.
  /// If the relativa BW exceeds the limit (fixed by certain filter topology), the BW is adjuster
  /// \param max_rel_bw: Maximum admisible relative bandwidth
  void adjustRelativeBW(double max_rel_bw);

  /// @brief Direct-coupled filters only. Get a hint of a resonators' component value depending on the frequency
  /// \param freq: Frequency in Hz
  /// \param Component: "Inductor" or "Capacitor"
  /// \returns Hint of the component value to have a suitable resonator for a given frequency
  double getResonatorComponentValueHint(double freq, ComponentType component);

public:
  /// @brief Performs filter synthesis with current specifications
  void synthesize();

  /// @brief Sets microstrip substrate parameters
  /// @param MS_Subs Substrate properties (εr, h, tanδ, etc.)
  void set_MS_Subs(MS_Substrate SUBSTRATE) {
    MS_Subs = SUBSTRATE;
  }

signals:
  /// @brief Emitted when schematic is updated
  /// @param content New schematic content
  void updateSchematic(SchematicContent);

  /// @brief Emitted to trigger simulation update
  /// @param content Schematic content to simulate
  void updateSimulation(SchematicContent);
};

#endif // FILTERDESIGNTOOL_H
