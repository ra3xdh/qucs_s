/// @file simulationsetup.h
/// @brief UI component to set the simulation settings (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-laterng with this program.  If not, see
/// <https://www.gnu.org/licenses/>.

#ifndef SIMULATIONSETUP_H
#define SIMULATIONSETUP_H

#include "../../Misc/general.h"
#include "../../Schematic/infoclasses.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

// Needed for having the substrate structures for each transmission line
// implementation
#include "../TransmissionLineSynthesis/Microstrip.h"

/// @class SimulationSetup
/// @brief UI component to set the simulation settings
/// The widget consists of two tabs:
/// 1) Frequency Sweep – start/stop frequency and number of points.
/// 2) Substrate Properties – transmission‑line type, substrate geometry, etc
class SimulationSetup : public QWidget {
    Q_OBJECT
  public:
    /// @brief Clas constructor
    /// @param parent Parent widget
    SimulationSetup(QWidget* parent = nullptr);

    /// @brief Class destructor
    ~SimulationSetup() {}

    /// @name Frequency sweep methods
    /// @{
    /// @brief Get the start frequency in Hz.
    /// @return Start frequency (Hz).
    double getFstart();

    /// @brief Get the start frequency as a formatted string.
    /// @return Textual representation, e.g. “10 MHz”.
    QString getFstart_as_Text();

    /// @brief Get the stop frequency in Hz.
    /// @return Stop frequency (Hz).
    double getFstop();

    /// @brief Get the stop frequency as a formatted string.
    /// @return Textual representation, e.g. “2 GHz”.
    QString getFstop_as_Text();

    /// @brief Get the number of frequency points.
    /// @return Number of points.
    int getNpoints() { return npointsSpinBox->value(); }
    /// }@

    /// @name Substrate properties methods
    /// @{
    ///
    /// @brief Get the selected transmission‑line type.
    /// @return TransmissionLineType::MLIN for microstrip,
    ///         TransmissionLineType::SLIN for stripline.
    TransmissionLineType getTransmissionLineType();

    /// @brief Substrate thickness (mm).
    double getSubstrateThickness(){
      return substrateThicknessSpinBox->value(); // in mm
    }

    /// @brief Relative permittivity (εᵣ)
    double getSubstratePermittivity(){
      return substratePermittivitySpinBox->value();
    }

    /// @brief Loss tangent (tan δ)
    double getSubstrateLossTangent(){
      return substrateLossTangentSpinBox->value();
    }

    /// @brief Conductor thickness (µm)
    double getConductorThickness(){
      return conductorThicknessSpinBox->value();
    }

    /// @brief Conductor conductivity (S/m)
    double getConductorConductivity() {
      return conductorConductivitySpinBox->value();
    }

    /// @brief Ground‑plane thickness (µm).
    /// @note Stripline only
    double getGroundPlaneThickness(){
      return groundPlaneThicknessSpinBox->value();
    }
    /// }@

    /// @brief Retrieve the populated @c MS_Substrate structure.
    /// @return Substrate description compatible with the synthesis code.
    MS_Substrate get_MS_Substrate() { return MS_Subs; }

  private:
    // Tab widget
    QTabWidget       *tabWidget;

    /// @name Frequency‑sweep widgets
    /// @{
    QDoubleSpinBox   *fstartSpinBox;
    QDoubleSpinBox   *fstopSpinBox;
    QComboBox        *fstartScaleComboBox;
    QComboBox        *fstopScaleComboBox;
    QSpinBox         *npointsSpinBox;
    /// }@

    /// @name Substrate‑property widgets
    /// @{
    QComboBox        *transmissionLineComboBox;
    QDoubleSpinBox   *substrateThicknessSpinBox;
    QDoubleSpinBox   *substratePermittivitySpinBox;
    QDoubleSpinBox   *substrateLossTangentSpinBox;
    QDoubleSpinBox   *conductorThicknessSpinBox;
    QDoubleSpinBox   *conductorConductivitySpinBox;
    QDoubleSpinBox   *groundPlaneThicknessSpinBox;
    QLabel           *imageLabel;
    /// }@

    /// @name Helper methods
    /// @{
    QWidget* createFrequencySweepTab();
    QWidget* createSubstratePropertiesTab();
    void updateImageDisplay();
    /// }@

    // Substrate data
    MS_Substrate MS_Subs; ///< Holds the current substrate definition

  private slots:
    /// @brief Refresh the stored substrate data after a UI change.
    void updateSubstrateDefinition();

    /// @brief Refresh the frequency sweep settings
    void updateFrequencySweep() {emit updateSimulation();}

    /// @brief React to a change in transmission‑line type (enable/disable controls)
    void onTransmissionLineTypeChanged();

  signals:
    /// @brief Emitted when any simulation parameter changes.
    void updateSimulation();

    /// @brief Emitted specifically when substrate data changes
    void updateSubstrate();
};

#endif // SIMULATIONSETUP_H
