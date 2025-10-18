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

#include "MatchingNetworkDesignTool.h"

MatchingNetworkDesignTool::MatchingNetworkDesignTool(QWidget *parent)
    : QWidget(parent) {
  QGridLayout *MatchingNetworkDesignLayout = new QGridLayout();

  // Checkbox to select 1-port or 2-port matching
  TwoPortCheckBox = new QCheckBox();
  TwoPortCheckBox->setText(QString("2-port matching"));
  TwoPortCheckBox->setChecked(false);
  MatchingNetworkDesignLayout->addWidget(TwoPortCheckBox, 0, 0);

  // Input matching network setup
  InputMatchingSetupWidget = new MatchingNetworkParametersWidget();
  MatchingNetworkDesignLayout->addWidget(InputMatchingSetupWidget, 1, 0, 1, 3);

  LoadSpecWidget = new LoadSpecificationWidget();
  MatchingNetworkDesignLayout->addWidget(LoadSpecWidget, 2, 0, 1, 5);

  // Output matching network setup
  OutputMatchingSetupWidget = new MatchingNetworkParametersWidget();
  OutputMatchingSetupWidget->setTitle("Output Matching Network Settings");
  MatchingNetworkDesignLayout->addWidget(OutputMatchingSetupWidget, 3, 0, 1, 3);
  OutputMatchingSetupWidget
      ->hide(); // By default, 1-port matching is selected, so hide this widget

  // Frequency range. Start
  f_match_Label = new QLabel("freq");
  f_match_Spinbox = new QDoubleSpinBox();
  f_match_Spinbox->setMinimum(1);
  f_match_Spinbox->setMaximum(1e6);
  f_match_Spinbox->setDecimals(0);
  f_match_Spinbox->setValue(1000);
  f_match_Spinbox->setSingleStep(1); // Step fixed to 1 Hz/kHz/MHz/GHz
  QStringList FreqScale;
  FreqScale << "GHz" << "MHz" << "kHz" << "Hz";
  f_match_Scale_Combo = new QComboBox();
  f_match_Scale_Combo->addItems(FreqScale);
  f_match_Scale_Combo->setCurrentIndex(1);
  MatchingNetworkDesignLayout->addWidget(f_match_Label, 4, 0);
  MatchingNetworkDesignLayout->addWidget(f_match_Spinbox, 4, 1);
  MatchingNetworkDesignLayout->addWidget(f_match_Scale_Combo, 4, 2);

  // Widgets to add a trace to plot
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Match1");
  MatchingNetworkDesignLayout->addWidget(traceNameLabel, 5, 0);
  MatchingNetworkDesignLayout->addWidget(traceNameLineEdit, 5, 1);

  // Connect signals from the input matching network setup widget
  connect(InputMatchingSetupWidget, SIGNAL(parametersChanged()), this,
          SLOT(UpdateDesignParameters()));
  connect(OutputMatchingSetupWidget, SIGNAL(parametersChanged()), this,
          SLOT(UpdateDesignParameters()));
  connect(TwoPortCheckBox, SIGNAL(stateChanged(int)), this,
          SLOT(AdjustOneTwoPortMatchingWidgetsVisibility()));
  connect(f_match_Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(f_match_Scale_Combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));

  connect(LoadSpecWidget, &LoadSpecificationWidget::impedanceChanged, this,
          &MatchingNetworkDesignTool::UpdateDesignParameters);
  connect(LoadSpecWidget,
          &LoadSpecificationWidget::reflectionCoefficientChanged, this,
          &MatchingNetworkDesignTool::UpdateDesignParameters);
  connect(LoadSpecWidget, &LoadSpecificationWidget::sParametersChanged, this,
          &MatchingNetworkDesignTool::UpdateDesignParameters);

  this->setLayout(MatchingNetworkDesignLayout);
}

MatchingNetworkDesignTool::~MatchingNetworkDesignTool() {
  delete f_match_Label;
  delete f_match_Scale_Combo;
  delete f_match_Spinbox;
  delete TwoPortCheckBox;
  delete InputMatchingSetupWidget;
  delete LoadSpecWidget;
  delete OutputMatchingSetupWidget;
  delete f_match_Label;
  delete f_match_Spinbox;
  delete f_match_Scale_Combo;
  delete traceNameLabel;
  delete traceNameLineEdit;
}

void MatchingNetworkDesignTool::AdjustOneTwoPortMatchingWidgetsVisibility() {
  if (TwoPortCheckBox->isChecked()) {
    // Two-ports matching
    OutputMatchingSetupWidget->show();
    InputMatchingSetupWidget->setTitle("Input Matching Network Settings");
    LoadSpecWidget->setTitle("Load Settings (2-ports)");
    traceNameLineEdit->setText("Match2p_1");
    LoadSpecWidget->setTwoPortMode(true);
  } else {
    // One-port matching
    OutputMatchingSetupWidget->hide();
    InputMatchingSetupWidget->setTitle("Matching Network Settings");
    LoadSpecWidget->setTitle("Load Settings (1-port)");
    traceNameLineEdit->setText("Match1");
    LoadSpecWidget->setTwoPortMode(false);
  }
  // Once visibility was adjusted, update the specifications and synthesize a
  // network
  UpdateDesignParameters();
}

void MatchingNetworkDesignTool::UpdateDesignParameters() {

  MatchingData Specs;
  // Get the input matching network setup parameters from the widget
  MatchingNetworkDesignParameters InputSpecs =
      InputMatchingSetupWidget->getDesignParameters();

  if (TwoPortCheckBox->isChecked()) {
    // 2-port mode
    MatchingNetworkDesignParameters OutputSpecs =
        OutputMatchingSetupWidget->getDesignParameters();

    // Pass the reference impedance of the input and output networks
    // This is required to calculate the impedances to match
    LoadSpecWidget->Z0_Port1 = InputSpecs.Z0;
    LoadSpecWidget->Z0_Port2 = OutputSpecs.Z0;

    // Get the impedances to match
    std::pair<std::complex<double>, std::complex<double>> ZL =
        LoadSpecWidget->getTwoPortMatchingImpedances();

    // Set the impedances to match for the input and output networks
    InputSpecs.ZL = ZL.first;
    OutputSpecs.ZL = ZL.second;

    // Set the input and output network data in the matching problem definition
    // (struct MatchingData)
    Specs.InputNetworkParameters = InputSpecs;
    Specs.OutputNetworkParameters = OutputSpecs;
    Specs.twoPortMode = true;

    // Get the S-parameters and add them to the match data. They are needed for
    // creating the SPAR block
    std::array<std::complex<double>, 4> sparams =
        LoadSpecWidget->getSParameters();
    Specs.sparams = sparams;

  } else {
    // 1-port mode - Get the load impedance
    InputSpecs.ZL = LoadSpecWidget->getLoadImpedance_At_Fmatch();
    QList<std::complex<double>> ZL_data = LoadSpecWidget->getZLdata();
    if (!ZL_data.isEmpty()) {
      // If available, load ZL vs frequency data
      InputSpecs.ZL_data = ZL_data;
      InputSpecs.freq = LoadSpecWidget->getFrequency();
      InputSpecs.sim_path = LoadSpecWidget->getSparFilePath();
    }

    Specs.twoPortMode = false;
    Specs.InputNetworkParameters = InputSpecs;
  }

  // Set match frequency
  Specs.f_match = f_match_Spinbox->value() *
                  getScaleFreq(f_match_Scale_Combo->currentIndex());

  LoadSpecWidget->setFmatch(Specs.f_match);

  // Design the matching network(s)
  MatchingNetworkDesigner *MatchDesigner = new MatchingNetworkDesigner(Specs);
  MatchDesigner->synthesize();
  SchContent = MatchDesigner->Schematic;

  // EMIT SIGNAL TO SIMULATE
  QString TraceName = traceNameLineEdit->text();
  SchContent.Name = TraceName;

  // Indicate the main tool the kind of circuit to adjust default traces (in
  // case no traces were selected)
  if (Specs.twoPortMode == true) {
    // Two-port matching: S21, S11 and S22
    SchContent.Type = QString("Matching-2-ports");
  } else {
    // One-port matching: S11
    SchContent.Type = QString("Matching-1-port");
  }

  emit updateSchematic(SchContent);
  emit updateSimulation(SchContent);
}

// The purpose of this function is to trigger a design from the main application
void MatchingNetworkDesignTool::design() { UpdateDesignParameters(); }

// This function scales the frequency according to 'FreqScaleCombo' combobox
double MatchingNetworkDesignTool::getScaleFreq(int index) {
  double exp = 1;
  switch (index) {
  case 0:
    exp = 9;
    break;
  case 1:
    exp = 6;
    break;
  case 2:
    exp = 3;
    break;
  case 3:
    exp = 1;
    break;
  }
  return pow(10, exp);
}
