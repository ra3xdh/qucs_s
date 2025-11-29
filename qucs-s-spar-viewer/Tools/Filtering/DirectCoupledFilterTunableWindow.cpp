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

#include "FilterDesignTool.h"

///////////////////////////////////////////////////////////////////
// Direct-coupled filters: Tunable elements window
void FilterDesignTool::openResonatorValuesDialog() {
  // Number of resonators
  unsigned int N = OrderSpinBox->value();

  // Use the stored tunable component preference (independent of topology)
  // If not set, default to "Inductor"
  QString tunableComponent = tunableComponent_DC_Filters;

  // Ensure resonatorValues vector has correct size
  if (resonatorValues.size() != N) {
    double res_val = 10;
    QString res_scale_val = QString("nH");

    // Initialize resonator values
    resonatorValues.clear();
    resonatorScaleValues.clear();

    // Resize according to the number of resonators
    resonatorValues.resize(N);
    resonatorScaleValues.resize(N);

    // Determine frequency
    double freq =
        FCSpinbox->value() * getScale(FC_ScaleCombobox->currentText());
    // Initialize resonator adjustable values
    if (tunableComponent == "Inductor") {
      // Adjustable element: Inductor
      if (freq > 1e9) {
        res_val = 1; // 1 nH
      } else if ((freq > 7e8) && (freq < 1e9)) {
        res_val = 5; // 5 nH
      } else if ((freq > 4e8) && (freq < 7e9)) {
        res_val = 10; // 10 nH
      } else {
        res_val = 50; // 50 nH
      }
      // Scale factor
      res_scale_val = QString("nH");
    } else {
      // Adjustable element: Capacitor
      res_val = 10;                  // 10 pF
      res_scale_val = QString("pF"); // Scale factor
    }

    // Set the resonator and the scale values for each resonator
    for (unsigned int i = 0; i < N; i++) {
      resonatorValues[i] = res_val;
      resonatorScaleValues[i] = res_scale_val;
    }
  }

  // Save original values in case user cancels
  std::vector<double> originalResonatorValues = resonatorValues;
  std::vector<QString> originalResonatorScaleValues = resonatorScaleValues;
  QString originalTunableComponent = tunableComponent;

  // Create dialog
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Adjust Resonator");
  QGridLayout *layout = new QGridLayout(dialog);

  // Add tunable component switch at the top
  QWidget *switchWidget = new QWidget();
  QHBoxLayout *switchLayout = new QHBoxLayout(switchWidget);
  switchLayout->setContentsMargins(0, 0, 0, 0);

  QLabel *switchLabel = new QLabel("Tunable Component:");
  QComboBox *tunableSwitch = new QComboBox();
  tunableSwitch->addItem("Inductor");
  tunableSwitch->addItem("Capacitor");
  tunableSwitch->setCurrentIndex(tunableComponent == "Inductor" ? 0 : 1);
  tunableSwitch->setMinimumWidth(120);

  switchLayout->addWidget(switchLabel);
  switchLayout->addWidget(tunableSwitch);
  switchLayout->addStretch();

  layout->addWidget(switchWidget, 0, 0, 1, 3);

  // Add separator line
  QFrame *line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  layout->addWidget(line, 1, 0, 1, 3);

  // Clear previous spinboxes and scale combos
  ResonatorSpinboxes.clear();
  ResonatorScaleComboboxes.clear();

  // Lambda to update all widgets based on tunable component selection
  auto updateWidgets = [&]() {
    tunableComponent = tunableSwitch->currentText();
    bool isInductor = (tunableComponent == "Inductor");
    QString label = isInductor ? "L" : "C";

    // Update each resonator widget
    for (unsigned int i = 0; i < N; i++) {
      // Update label
      QLabel *labelWidget =
          qobject_cast<QLabel *>(layout->itemAtPosition(i + 2, 0)->widget());
      if (labelWidget) {
        labelWidget->setText(QString("%1%2").arg(label).arg(i + 1));
      }

      // Update scale combobox options
      QComboBox *scaleCombo = ResonatorScaleComboboxes[i];
      scaleCombo->blockSignals(true);
      scaleCombo->clear();

      if (isInductor) {
        scaleCombo->addItem("mH");      // [0]
        scaleCombo->addItem("µH");      // [1]
        scaleCombo->addItem("nH");      // [2]
        scaleCombo->addItem("pH");      // [3]
        scaleCombo->setCurrentIndex(2); // Default to nH
        resonatorScaleValues[i] = "nH";
      } else {
        scaleCombo->addItem("µF");      // [0]
        scaleCombo->addItem("nF");      // [1]
        scaleCombo->addItem("pF");      // [2]
        scaleCombo->addItem("fF");      // [3]
        scaleCombo->setCurrentIndex(2); // Default to pF
        resonatorScaleValues[i] = "pF";
      }

      scaleCombo->blockSignals(false);

      // Update default value
      QDoubleSpinBox *spinbox = ResonatorSpinboxes[i];
      spinbox->blockSignals(true);
      resonatorValues[i] = 10.0;
      spinbox->setValue(10.0);
      spinbox->blockSignals(false);
    }

    UpdateDesignParameters();
  };

  // Create spinboxes for each resonator (starting at row 2)
  for (unsigned int i = 0; i < N; i++) {
    bool isInductor = (tunableComponent == "Inductor");
    QString label = isInductor ? "L" : "C";
    layout->addWidget(new QLabel(QString("%1%2").arg(label).arg(i + 1)), i + 2,
                      0);

    QDoubleSpinBox *ResonatorSpinbox = new QDoubleSpinBox();
    ResonatorSpinbox->setDecimals(1);
    ResonatorSpinbox->setMinimum(0.01);
    ResonatorSpinbox->setMaximum(100000);
    ResonatorSpinbox->setSingleStep(0.5);

    // Create scale combobox
    QComboBox *scaleCombo = new QComboBox();
    if (isInductor) {
      scaleCombo->addItem("mH"); // [0]
      scaleCombo->addItem("µH"); // [1]
      scaleCombo->addItem("nH"); // [2]
      scaleCombo->addItem("pH"); // [3]
    } else {
      scaleCombo->addItem("µF"); // [0]
      scaleCombo->addItem("nF"); // [1]
      scaleCombo->addItem("pF"); // [2]
      scaleCombo->addItem("fF"); // [3]
    }

    // Set scale, based on a QString
    int index = scaleCombo->findText(resonatorScaleValues[i]);
    if (index != -1) {
      scaleCombo->setCurrentIndex(index);
    } else {
      // Default to nH or pF
      scaleCombo->setCurrentIndex(2);
    }

    // Set initial value
    ResonatorSpinbox->setValue(resonatorValues[i]);

    // Connect value change to trigger design update
    connect(ResonatorSpinbox, SIGNAL(valueChanged(double)), this,
            SLOT(UpdateDesignParameters()));

    // Connect scale change to trigger design update
    connect(scaleCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(UpdateDesignParameters()));

    ResonatorSpinboxes.push_back(ResonatorSpinbox);
    ResonatorScaleComboboxes.push_back(scaleCombo);

    layout->addWidget(ResonatorSpinbox, i + 2, 1);
    layout->addWidget(scaleCombo, i + 2, 2);
  }

  // Connect tunable component switch
  connect(tunableSwitch, QOverload<int>::of(&QComboBox::currentIndexChanged),
          updateWidgets);

  // Add OK and Cancel buttons
  QPushButton *okButton = new QPushButton("OK");
  QPushButton *cancelButton = new QPushButton("Cancel");

  layout->addWidget(okButton, N + 2, 0);
  layout->addWidget(cancelButton, N + 2, 1);

  connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);
  connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);

  // Show dialog and process result
  if (dialog->exec() == QDialog::Accepted) {
    // Save the user's tunable component preference
    tunableComponent_DC_Filters = tunableComponent;

    // Values are already updated through UpdateDesignParameters
    // Just trigger final update to ensure everything is synchronized
    UpdateDesignParameters();
  } else {
    // Restore original values on cancel
    resonatorValues = originalResonatorValues;
    resonatorScaleValues = originalResonatorScaleValues;
    tunableComponent_DC_Filters = originalTunableComponent;
    UpdateDesignParameters();
  }

  delete dialog;
}
