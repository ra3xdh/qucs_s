/// @file DirectCoupledFilterAdjustResonatorValuesWindow.cpp
/// @brief Window to define the values of the resonator components in
/// direct-coupled filters
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "../FilterDesignTool.h"

///
/// \internal The resonators have two degrees of freedom: the value of the
/// inductance and the value of the capacitance. This window allows the user to
/// set a specific value to one of these variables.
///
/// Depending on the topology, the quantity set by the user will be modified by
/// the design equations to absorb the negative inductance or capacitance of the
/// inverters
///
void FilterDesignTool::openResonatorValuesDialog() {
  // The filter order widget may vary before the program reaches this point and
  // the size of "resonatorValues" and "resonatorScaleValues" may not be updated
  // at this point.

  // Thus, it is needed to ensure their size is correct.In case
  // it isn't, it is needed to resize them.
  unsigned int N = OrderSpinBox->value(); // Number of resonators.

  // Identify the adjustable variable, depending on the direct-coupled
  // topology
  int index = DC_CouplingTypeCombo->currentIndex();
  ComponentType ResonatorTunableComponent = ComponentType::Inductor;

  switch (index) {
  case 0: // C-coupled shunt resonators (Adjust L)
    ResonatorTunableComponent = ComponentType::Inductor;
    break;
  case 1: // L-coupled shunt resonators (Adjust C)
    ResonatorTunableComponent = ComponentType::Capacitor;
    break;
  case 2: // L-coupled series resonators (Adjust C)
    ResonatorTunableComponent = ComponentType::Capacitor;
    break;
  case 3: // C-coupled series resonators (Adjust L)
    ResonatorTunableComponent = ComponentType::Inductor;
    break;
  }

  // Ensure resonatorValues vector has correct size
  // In case the order of the filter doesn't match the number of resonator,
  // rebuilt the user interface
  if (resonatorValues.size() != N) {
    double res_val = 10;
    QString res_scale_val = QString("nH");

    // Initialize resonator values. These variables (resonatorValues and
    // resonatorScaleValues) are stored in the FilterDesignTool class. It's
    // needed they to be there to launch DC-filter synthesis without the need to
    // open this dialog
    resonatorValues.clear();
    resonatorScaleValues.clear();

    // Resize according to the number of resonators
    resonatorValues.resize(N);
    resonatorScaleValues.resize(N);

    // Get the center frequency, directly from the widgets
    double freq =
        FCSpinbox->value() * getScale(FC_ScaleCombobox->currentText());

    // Initialize resonator adjustable values. This depends on the frequency
    res_val = getResonatorComponentValueHint(freq, ResonatorTunableComponent);

    // Scale factor
    if (ResonatorTunableComponent == ComponentType::Inductor) {
      // Scale factor
      res_scale_val = QString("nH");
    } else {
      res_scale_val = QString("pF");
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

  // Create dialog
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Adjust Resonator");
  dialog->setFixedWidth(400); // Width enough to see the entire title
  QGridLayout *layout = new QGridLayout(dialog);

  // Add tunable component switch at the top
  QWidget *switchWidget = new QWidget();
  QHBoxLayout *switchLayout = new QHBoxLayout(switchWidget);
  switchLayout->setContentsMargins(0, 0, 0, 0);

  QLabel *switchLabel = new QLabel();

  if (ResonatorTunableComponent == ComponentType::Inductor) {
    // Inductor
    switchLabel->setText("Adjust resonators' inductance");
  } else {
    // Capacitor
    switchLabel->setText("Adjust resonators' capacitance");
  }

  switchLayout->addWidget(switchLabel);
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

  // Create spinboxes for each resonator (starting at row 2)
  QString label;
  if (ResonatorTunableComponent == ComponentType::Inductor) {
    // Inductor
    label = QString("L");
  } else {
    // Capacitor
    label = QString("C");
  }
  for (unsigned int i = 0; i < N; i++) {
    layout->addWidget(new QLabel(QString("%1%2").arg(label).arg(i + 1)), i + 2,
                      0);

    QDoubleSpinBox *ResonatorSpinbox = new QDoubleSpinBox();
    ResonatorSpinbox->setDecimals(1);
    ResonatorSpinbox->setMinimum(0.01);
    ResonatorSpinbox->setMaximum(100000);
    ResonatorSpinbox->setSingleStep(0.5);

    // Create scale combobox
    QComboBox *scaleCombo = new QComboBox();
    if (ResonatorTunableComponent == ComponentType::Inductor) {
      scaleCombo->addItem("mH"); // [0]
      scaleCombo->addItem("µH"); // [1]
      scaleCombo->addItem("nH"); // [2]
      scaleCombo->addItem("pH"); // [3]
    } else {
      // Capacitor
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
    connect(ResonatorSpinbox, &QDoubleSpinBox::valueChanged, this,
            [this, i](double value) {
              resonatorValues[i] = value;
              UpdateDesignParameters();
            });

    // Connect scale change to trigger design update
    connect(scaleCombo, &QComboBox::currentIndexChanged, this,
            [this, i, scaleCombo]() {
              resonatorScaleValues[i] = scaleCombo->currentText();
              UpdateDesignParameters();
            });

    ResonatorSpinboxes.push_back(ResonatorSpinbox);
    ResonatorScaleComboboxes.push_back(scaleCombo);

    layout->addWidget(ResonatorSpinbox, i + 2, 1);
    layout->addWidget(scaleCombo, i + 2, 2);
  }

  // Add OK and Cancel buttons
  QPushButton *okButton = new QPushButton("OK");
  QPushButton *cancelButton = new QPushButton("Cancel");

  layout->addWidget(okButton, N + 2, 0);
  layout->addWidget(cancelButton, N + 2, 1);

  connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);
  connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);

  // Show dialog and process result
  if (dialog->exec() == QDialog::Accepted) {
    // Iterate over the widgets and store their value in the private variable,
    // so that value is now default
    for (unsigned int i = 0; i < N; i++) {
      resonatorValues[i] = ResonatorSpinboxes[i]->value();
      resonatorScaleValues[i] = ResonatorScaleComboboxes[i]->currentText();
    }

    // Values are already updated through UpdateDesignParameters
    // Just trigger final update to ensure everything is synchronized
    UpdateDesignParameters();
  } else {
    // Restore original values on cancel
    resonatorValues = originalResonatorValues;
    resonatorScaleValues = originalResonatorScaleValues;
    UpdateDesignParameters();
  }

  delete dialog;
}

double
FilterDesignTool::getResonatorComponentValueHint(double freq,
                                                 ComponentType component) {
  double res_val;
  if (component == ComponentType::Inductor) {
    // Inductor
    if (freq > 1.5e9) {
      res_val = 1; // 1 nH
    } else if (freq > 7e8) {
      res_val = 10; // 5 nH
    } else if (freq > 4e8) {
      res_val = 20; // 10 nH
    } else if (freq > 2e8) {
      res_val = 50; // 50 nH
    } else {
      res_val = 100; // 100 nH
    }
    return res_val;

  } else {
    // Capacitor
    if (freq > 1e9) {
      res_val = 1; // 1 pF
    } else if (freq > 7e8) {
      res_val = 5; // 5 pF
    } else if (freq > 4e8) {
      res_val = 10; // 10 pF
    } else if (freq > 2e8) {
      res_val = 20; // 20 pF
    } else {
      res_val = 50; // 50 pF
    }
    return res_val;
  }
}
