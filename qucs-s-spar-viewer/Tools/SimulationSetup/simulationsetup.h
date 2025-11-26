/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

class SimulationSetup : public QWidget {
  Q_OBJECT
public:
  SimulationSetup(QWidget* parent = nullptr);
  ~SimulationSetup();

  // Frequency sweep methods
  double getFstart();
  QString getFstart_as_Text();
  double getFstop();
  QString getFstop_as_Text();
  int getNpoints();

  // Substrate properties methods
  TransmissionLineType getTransmissionLineType();
  double getSubstrateThickness();
  double getSubstratePermittivity();
  double getSubstrateLossTangent();
  double getConductorThickness();
  double getConductorConductivity();
  double getGroundPlaneThickness(); // For stripline

  // Interfacing functions
  MS_Substrate get_MS_Substrate();

private:
  // Tab widget
  QTabWidget* tabWidget;

  // Frequency sweep widgets
  QDoubleSpinBox *fstartSpinBox, *fstopSpinBox;
  QComboBox *fstartScaleComboBox, *fstopScaleComboBox;
  QSpinBox* npointsSpinBox;

  // Substrate properties widgets
  QComboBox* transmissionLineComboBox;
  QDoubleSpinBox* substrateThicknessSpinBox;
  QDoubleSpinBox* substratePermittivitySpinBox;
  QDoubleSpinBox* substrateLossTangentSpinBox;
  QDoubleSpinBox* conductorThicknessSpinBox;
  QDoubleSpinBox* conductorConductivitySpinBox;
  QDoubleSpinBox* groundPlaneThicknessSpinBox;
  QLabel* imageLabel;

  // Helper methods
  QWidget* createFrequencySweepTab();
  QWidget* createSubstratePropertiesTab();
  void updateImageDisplay();

  // Substrate data
  MS_Substrate MS_Subs;

private slots:
  void updateSubstrateDefinition();
  void onTransmissionLineTypeChanged();

signals:
  void updateSimulation();
  void updateSubstrate();
};

#endif // SIMULATIONSETUP_H
