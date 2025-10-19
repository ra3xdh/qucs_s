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

#include "simulationsetup.h"

SimulationSetup::SimulationSetup(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout();

  // Create tab widget
  tabWidget = new QTabWidget();

  // Add tabs
  tabWidget->addTab(createFrequencySweepTab(), "Frequency Sweep");
  tabWidget->addTab(createSubstratePropertiesTab(), "Substrate Properties");

  mainLayout->addWidget(tabWidget);
  mainLayout->setAlignment(Qt::AlignTop);
  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  this->setLayout(mainLayout);

  // Connect all widgets to trigger simulation updates
  connect(fstartSpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
  connect(fstartScaleComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(update()));
  connect(fstopSpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
  connect(fstopScaleComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(update()));
  connect(npointsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(update()));

  connect(transmissionLineComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onTransmissionLineTypeChanged()));
  connect(substrateThicknessSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(update()));
  connect(substratePermittivitySpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(update()));
  connect(substrateLossTangentSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(update()));
  connect(conductorThicknessSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(update()));
  connect(conductorConductivitySpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(update()));
  connect(groundPlaneThicknessSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(update()));
}

SimulationSetup::~SimulationSetup() {}

QWidget *SimulationSetup::createFrequencySweepTab() {
  QWidget *frequencyWidget = new QWidget();
  QGridLayout *frequencyLayout = new QGridLayout();

  // Start frequency
  QLabel *fstartLabel = new QLabel("Start freq");
  fstartSpinBox = new QDoubleSpinBox();
  fstartSpinBox->setMinimum(0);
  fstartSpinBox->setMaximum(1e7);
  fstartSpinBox->setValue(10);

  fstartScaleComboBox = new QComboBox();
  fstartScaleComboBox->addItem("GHz");
  fstartScaleComboBox->addItem("MHz");
  fstartScaleComboBox->addItem("kHz");
  fstartScaleComboBox->addItem("Hz");
  fstartScaleComboBox->setCurrentIndex(1);

  frequencyLayout->addWidget(fstartLabel, 0, 0);
  frequencyLayout->addWidget(fstartSpinBox, 0, 1);
  frequencyLayout->addWidget(fstartScaleComboBox, 0, 2);

  // Stop frequency
  QLabel *fstopLabel = new QLabel("Stop freq");
  fstopSpinBox = new QDoubleSpinBox();
  fstopSpinBox->setMinimum(0);
  fstopSpinBox->setMaximum(1e7);
  fstopSpinBox->setValue(2000);

  fstopScaleComboBox = new QComboBox();
  fstopScaleComboBox->addItem("GHz");
  fstopScaleComboBox->addItem("MHz");
  fstopScaleComboBox->addItem("kHz");
  fstopScaleComboBox->addItem("Hz");
  fstopScaleComboBox->setCurrentIndex(1);

  frequencyLayout->addWidget(fstopLabel, 1, 0);
  frequencyLayout->addWidget(fstopSpinBox, 1, 1);
  frequencyLayout->addWidget(fstopScaleComboBox, 1, 2);

  // Number of points
  QLabel *npointsLabel = new QLabel("Number of points");
  npointsSpinBox = new QSpinBox();
  npointsSpinBox->setMinimum(10);
  npointsSpinBox->setMaximum(1e6);
  npointsSpinBox->setValue(200);

  frequencyLayout->addWidget(npointsLabel, 2, 0);
  frequencyLayout->addWidget(npointsSpinBox, 2, 1);

  // Add stretch to push everything to the top
  frequencyLayout->setRowStretch(3, 1);

  frequencyWidget->setLayout(frequencyLayout);
  return frequencyWidget;
}

QWidget *SimulationSetup::createSubstratePropertiesTab() {
  QWidget *substrateWidget = new QWidget();
  substrateWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  QVBoxLayout *substrateLayout = new QVBoxLayout();

  // Substrate parameters
  QGroupBox *parametersGroupBox = new QGroupBox("Substrate Parameters");
  QGridLayout *parametersLayout = new QGridLayout();

  // Transmission line type selection
  QLabel *transmissionLineLabel = new QLabel("Transmission Line Type");
  transmissionLineComboBox = new QComboBox();
  transmissionLineComboBox->addItem("Microstrip");
  transmissionLineComboBox->addItem("Stripline");
  transmissionLineComboBox->setCurrentIndex(0); // Default to Microstrip

  parametersLayout->addWidget(transmissionLineLabel, 0, 0);
  parametersLayout->addWidget(transmissionLineComboBox, 0, 1);

  // Image display area (small, in same frame)
  imageLabel = new QLabel();
  imageLabel->setFixedSize(250, 200);
  imageLabel->setStyleSheet(
      "QLabel { border: 1px solid gray; background-color: white; }");
  imageLabel->setAlignment(Qt::AlignCenter);
  imageLabel->setScaledContents(true);

  parametersLayout->addWidget(imageLabel, 0, 2, 7, 1); // Span 2 rows

  // Substrate thickness
  QLabel *thicknessLabel = new QLabel("Substrate thickness (H) (mm)");
  substrateThicknessSpinBox = new QDoubleSpinBox();
  substrateThicknessSpinBox->setMinimum(0.1);
  substrateThicknessSpinBox->setMaximum(20);
  substrateThicknessSpinBox->setDecimals(3);
  substrateThicknessSpinBox->setSingleStep(0.1);
  substrateThicknessSpinBox->setValue(0.508); // 20 mils

  parametersLayout->addWidget(thicknessLabel, 1, 0);
  parametersLayout->addWidget(substrateThicknessSpinBox, 1, 1);

  // Relative permittivity
  QLabel *permittivityLabel = new QLabel("Relative permittivity (εᵣ)");
  substratePermittivitySpinBox = new QDoubleSpinBox();
  substratePermittivitySpinBox->setMinimum(1.0);
  substratePermittivitySpinBox->setMaximum(20.0);
  substratePermittivitySpinBox->setDecimals(2);
  substratePermittivitySpinBox->setValue(3.55); // RO4003C
  substratePermittivitySpinBox->setSingleStep(0.5);

  parametersLayout->addWidget(permittivityLabel, 2, 0);
  parametersLayout->addWidget(substratePermittivitySpinBox, 2, 1);

  // Loss tangent
  QLabel *lossTangentLabel = new QLabel("Loss tangent (tan δ)");
  substrateLossTangentSpinBox = new QDoubleSpinBox();
  substrateLossTangentSpinBox->setMinimum(0.0);
  substrateLossTangentSpinBox->setMaximum(1.0);
  substrateLossTangentSpinBox->setDecimals(4);
  substrateLossTangentSpinBox->setValue(0.0027); // RO4003C
  substrateLossTangentSpinBox->setSingleStep(0.0001);

  parametersLayout->addWidget(lossTangentLabel, 3, 0);
  parametersLayout->addWidget(substrateLossTangentSpinBox, 3, 1);

  // Conductor thickness
  QLabel *conductorThicknessLabel = new QLabel("Conductor thickness (T) (μm)");
  conductorThicknessSpinBox = new QDoubleSpinBox();
  conductorThicknessSpinBox->setMinimum(0.1);
  conductorThicknessSpinBox->setMaximum(1000.0);
  conductorThicknessSpinBox->setDecimals(1);
  conductorThicknessSpinBox->setValue(35.0);
  conductorThicknessSpinBox->setSingleStep(1);

  parametersLayout->addWidget(conductorThicknessLabel, 4, 0);
  parametersLayout->addWidget(conductorThicknessSpinBox, 4, 1);

  // Conductor conductivity
  QLabel *conductorConductivityLabel =
      new QLabel("Conductor conductivity (S/m)");
  conductorConductivitySpinBox = new QDoubleSpinBox();
  conductorConductivitySpinBox->setMinimum(1e6);
  conductorConductivitySpinBox->setMaximum(1e8);
  conductorConductivitySpinBox->setValue(5.8e7);
  conductorConductivitySpinBox->setDecimals(0);
  conductorConductivitySpinBox->setSingleStep(1e6);

  parametersLayout->addWidget(conductorConductivityLabel, 5, 0);
  parametersLayout->addWidget(conductorConductivitySpinBox, 5, 1);

  // Ground plane thickness (for stripline)
  QLabel *groundPlaneThicknessLabel = new QLabel("Ground plane thickness (μm)");
  groundPlaneThicknessSpinBox = new QDoubleSpinBox();
  groundPlaneThicknessSpinBox->setMinimum(0.1);
  groundPlaneThicknessSpinBox->setMaximum(1000.0);
  groundPlaneThicknessSpinBox->setValue(35.0);
  groundPlaneThicknessSpinBox->setDecimals(1);
  groundPlaneThicknessSpinBox->setEnabled(
      false); // Initially disabled for microstrip

  parametersLayout->addWidget(groundPlaneThicknessLabel, 6, 0);
  parametersLayout->addWidget(groundPlaneThicknessSpinBox, 6, 1);

  parametersGroupBox->setLayout(parametersLayout);
  substrateLayout->addWidget(parametersGroupBox);
  substrateLayout->setAlignment(Qt::AlignTop);

  // Connect widgets to the signal to update the substrate definition in the
  // tools
  connect(substrateThicknessSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(updateSubstrateDefinition()));
  connect(substratePermittivitySpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(updateSubstrateDefinition()));
  connect(substrateLossTangentSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(updateSubstrateDefinition()));
  connect(conductorThicknessSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(updateSubstrateDefinition()));
  connect(conductorConductivitySpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(updateSubstrateDefinition()));
  connect(groundPlaneThicknessSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(updateSubstrateDefinition()));

  // Initial image update
  updateImageDisplay();

  substrateWidget->setLayout(substrateLayout);
  return substrateWidget;
}

void SimulationSetup::updateImageDisplay() {
  // Load and display the appropriate cross-section image
  QString imagePath;
  if (transmissionLineComboBox->currentIndex() == 0) { // Microstrip
    imagePath = ":/Tools/SimulationSetup/bitmaps/microstrip.png";
  } else { // Stripline
    imagePath = ":/Tools/SimulationSetup/bitmaps/stripline.png";
  }

  QPixmap pixmap(imagePath);
  if (!pixmap.isNull()) {
    // Scale the image to fit the label while maintaining aspect ratio
    imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
    imageLabel->setText("");
  } else {
    // Fallback if image loading fails
    if (transmissionLineComboBox->currentIndex() == 0) {
      imageLabel->setText("Microstrip\nCross-section");
    } else {
      imageLabel->setText("Stripline\nCross-section");
    }
  }
}

double SimulationSetup::getFstart() {
  double freq = fstartSpinBox->value();
  QString unit = fstartScaleComboBox->currentText();
  double scale = getFreqScale(unit);
  freq /= scale;
  return freq;
}

QString SimulationSetup::getFstart_as_Text() {
  double freq = fstartSpinBox->value();
  QString unit = fstartScaleComboBox->currentText();
  QString text = QString("%1 %2").arg(freq).arg(unit);
  return text;
}

double SimulationSetup::getFstop() {
  double freq = fstopSpinBox->value();
  QString unit = fstopScaleComboBox->currentText();
  double scale = getFreqScale(unit);
  freq /= scale;
  return freq;
}

QString SimulationSetup::getFstop_as_Text() {
  double freq = fstopSpinBox->value();
  QString unit = fstopScaleComboBox->currentText();
  QString text = QString("%1 %2").arg(freq).arg(unit);
  return text;
}

int SimulationSetup::getNpoints() { return npointsSpinBox->value(); }

TransmissionLineType SimulationSetup::getTransmissionLineType() {
  return transmissionLineComboBox->currentIndex() == 0
             ? TransmissionLineType::MLIN
             : TransmissionLineType::SLIN;
}

double SimulationSetup::getSubstrateThickness() {
  return substrateThicknessSpinBox->value(); // in mm
}

double SimulationSetup::getSubstratePermittivity() {
  return substratePermittivitySpinBox->value();
}

double SimulationSetup::getSubstrateLossTangent() {
  return substrateLossTangentSpinBox->value();
}

double SimulationSetup::getConductorThickness() {
  return conductorThicknessSpinBox->value(); // in μm
}

double SimulationSetup::getConductorConductivity() {
  return conductorConductivitySpinBox->value(); // in S/m
}

double SimulationSetup::getGroundPlaneThickness() {
  return groundPlaneThicknessSpinBox->value(); // in μm
}

void SimulationSetup::updateSubstrateDefinition() {

  // Save stack up data
  if (transmissionLineComboBox->currentText() == QString("Microstrip")) {
    // Microstrip substrate
    MS_Subs.er = substratePermittivitySpinBox->value();
    MS_Subs.height = substrateThicknessSpinBox->value() *
                     1e-3; // Input in mm -> Result in meters
    MS_Subs.tand = substrateLossTangentSpinBox->value();

    // Metal properties
    MS_Subs.MetalConductivity = conductorConductivitySpinBox->value();
    MS_Subs.MetalThickness = conductorThicknessSpinBox->value() *
                             1e-6; // Input in um -> Result in meters
  }

  emit updateSubstrate();
}

void SimulationSetup::onTransmissionLineTypeChanged() {
  // Enable/disable ground plane thickness based on transmission line type
  bool isStripline = (transmissionLineComboBox->currentIndex() == 1);
  groundPlaneThicknessSpinBox->setEnabled(isStripline);

  // Update the image display
  updateImageDisplay();

  // Trigger simulation update
  update();
}

MS_Substrate SimulationSetup::get_MS_Substrate() { return MS_Subs; }
