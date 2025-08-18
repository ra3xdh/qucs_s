/***************************************************************************
                                AttenuatorDesignTool.cpp
                                ----------
    copyright            :  QUCS team
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#include "AttenuatorDesignTool.h"

AttenuatorDesignTool::AttenuatorDesignTool(QWidget *parent): QWidget(parent) {
  QGridLayout *AttenuatorDesignLayout = new QGridLayout();
  // Topology
  Topology_Label = new QLabel("Topology");
  Topology_Combo = new QComboBox();
  Topology_Combo->addItem("Pi");
  Topology_Combo->addItem("Tee");
  Topology_Combo->addItem("Bridged Tee");
  Topology_Combo->addItem("Reflection Attenuator");
  Topology_Combo->addItem("Quarter-wave series");
  Topology_Combo->addItem("Quarter-wave shunt");
  AttenuatorDesignLayout->addWidget(Topology_Label, 0, 0);
  AttenuatorDesignLayout->addWidget(Topology_Combo, 0, 1);

  // Attenuation
  Attenuation_Label = new QLabel("Attenuation");
  AttenuationSpinBox = new QDoubleSpinBox();
  AttenuationSpinBox->setMinimum(0.5);
  AttenuationSpinBox->setMaximum(60);
  AttenuationSpinBox->setSingleStep(0.5);
  AttenuationSpinBox->setValue(15);
  dBLabelAtt = new QLabel("dB");
  AttenuatorDesignLayout->addWidget(Attenuation_Label, 1, 0);
  AttenuatorDesignLayout->addWidget(AttenuationSpinBox, 1, 1);
  AttenuatorDesignLayout->addWidget(dBLabelAtt, 1, 2);

  // Input impedance
  Zin_Label = new QLabel("Zin");
  ZinSpinBox = new QDoubleSpinBox();
  ZinSpinBox->setMinimum(0.5);
  ZinSpinBox->setMaximum(1000);
  ZinSpinBox->setSingleStep(0.5);
  ZinSpinBox->setValue(50);
  Ohm_Zin_Label = new QLabel(QChar(0xa9, 0x03));
  AttenuatorDesignLayout->addWidget(Zin_Label, 2, 0);
  AttenuatorDesignLayout->addWidget(ZinSpinBox, 2, 1);
  AttenuatorDesignLayout->addWidget(Ohm_Zin_Label, 2, 2);

  // Output impedance
  Zout_Label = new QLabel("Zout");
  ZoutSpinBox = new QDoubleSpinBox();
  ZoutSpinBox->setMinimum(0.5);
  ZoutSpinBox->setMaximum(1000);
  ZoutSpinBox->setSingleStep(0.5);
  ZoutSpinBox->setValue(50);
  Ohm_Zout_Label = new QLabel(QChar(0xa9, 0x03));
  AttenuatorDesignLayout->addWidget(Zout_Label, 3, 0);
  AttenuatorDesignLayout->addWidget(ZoutSpinBox, 3, 1);
  AttenuatorDesignLayout->addWidget(Ohm_Zout_Label, 3, 2);

  // Input power
  Pin_Label = new QLabel("Input power");
  Pin_SpinBox = new QDoubleSpinBox();
  Pin_SpinBox->setMinimum(-150);   // dBm
  Pin_SpinBox->setMaximum(200);    // dBm
  Pin_SpinBox->setSingleStep(0.1); // dB
  Pin_SpinBox->setValue(30);       // dBm
  Pin_units_Combo = new QComboBox();
  QStringList power_units;
  power_units
      << "mW"
      << "W"
      << "dBm"
      << QString("dB%1V [75%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03))
      << QString("dB%1V [50%2]").arg(QChar(0xbc, 0x03)).arg(QChar(0xa9, 0x03))
      << QString("dBmV [75%1]").arg(QChar(0xa9, 0x03))
      << QString("dBmV [50%1]").arg(QChar(0xa9, 0x03));
  Pin_units_Combo->addItems(power_units);
  Pin_units_Combo->setCurrentIndex(2); // dBm
  AttenuatorDesignLayout->addWidget(Pin_Label, 4, 0);
  AttenuatorDesignLayout->addWidget(Pin_SpinBox, 4, 1);
  AttenuatorDesignLayout->addWidget(Pin_units_Combo, 4, 2);

  freqLabel = new QLabel("Frequency");
  freqSpinBox = new QDoubleSpinBox();
  freqSpinBox->setMinimum(0.5);
  freqSpinBox->setMaximum(10000);
  freqSpinBox->setValue(1000);
  freqSpinBox->setSingleStep(1);
  FreqScaleCombo = new QComboBox();
  FreqScaleCombo->addItem("GHz");
  FreqScaleCombo->addItem("MHz");
  FreqScaleCombo->addItem("kHz");
  FreqScaleCombo->addItem("Hz");
  FreqScaleCombo->setCurrentIndex(1);

  AttenuatorDesignLayout->addWidget(freqLabel, 5, 0);
  AttenuatorDesignLayout->addWidget(freqSpinBox, 5, 1);
  AttenuatorDesignLayout->addWidget(FreqScaleCombo, 5, 2);

  LumpedImplementationCheckbox = new QCheckBox("Use lumped components");
  LumpedImplementationCheckbox->setChecked(false);
  AttenuatorDesignLayout->addWidget(LumpedImplementationCheckbox, 6, 0);

  // Power dissipation
  Pdiss_R1_Label = new QLabel("Pdiss. R1");
  Pdiss_R1_Lineedit = new QLineEdit();
  Pdiss_R1_Lineedit->setReadOnly(true);
  R1_Pdiss_Units_Combo = new QComboBox();
  R1_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R1_Label, 7, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R1_Lineedit, 7, 1);
  AttenuatorDesignLayout->addWidget(R1_Pdiss_Units_Combo, 7, 2);

  Pdiss_R2_Label = new QLabel("Pdiss. R2");
  Pdiss_R2_Lineedit = new QLineEdit();
  Pdiss_R2_Lineedit->setReadOnly(true);
  R2_Pdiss_Units_Combo = new QComboBox();
  R2_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R2_Label, 8, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R2_Lineedit, 8, 1);
  AttenuatorDesignLayout->addWidget(R2_Pdiss_Units_Combo, 8, 2);

  Pdiss_R3_Label = new QLabel("Pdiss. R3");
  Pdiss_R3_Lineedit = new QLineEdit();
  Pdiss_R3_Lineedit->setReadOnly(true);
  R3_Pdiss_Units_Combo = new QComboBox();
  R3_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R3_Label, 9, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R3_Lineedit, 9, 1);
  AttenuatorDesignLayout->addWidget(R3_Pdiss_Units_Combo, 9, 2);

  Pdiss_R4_Label = new QLabel("Pdiss. R4");
  Pdiss_R4_Lineedit = new QLineEdit();
  Pdiss_R4_Lineedit->setReadOnly(true);
  R4_Pdiss_Units_Combo = new QComboBox();
  R4_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R4_Label, 10, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R4_Lineedit, 10, 1);
  AttenuatorDesignLayout->addWidget(R4_Pdiss_Units_Combo, 10, 2);

  // Widgets to add a trace to plot
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Attenuator1");
  AttenuatorDesignLayout->addWidget(traceNameLabel, 11, 0);
  AttenuatorDesignLayout->addWidget(traceNameLineEdit, 11, 1);

  connect(Topology_Combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_TopoCombo_currentIndexChanged(int)));
  connect(AttenuationSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(ZinSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(ZoutSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(freqSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(FreqScaleCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(Pin_SpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(Pin_units_Combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(LumpedImplementationCheckbox, SIGNAL(stateChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(R1_Pdiss_Units_Combo, SIGNAL(currentIndexChanged(QString)), this,
          SLOT(UpdatePowerDissipationData()));
  connect(R2_Pdiss_Units_Combo, SIGNAL(currentIndexChanged(QString)), this,
          SLOT(UpdatePowerDissipationData()));
  connect(R3_Pdiss_Units_Combo, SIGNAL(currentIndexChanged(QString)), this,
          SLOT(UpdatePowerDissipationData()));
  connect(R4_Pdiss_Units_Combo, SIGNAL(currentIndexChanged(QString)), this,
          SLOT(UpdatePowerDissipationData()));
  this->setLayout(AttenuatorDesignLayout);

  on_TopoCombo_currentIndexChanged(0);
}

AttenuatorDesignTool::~AttenuatorDesignTool() {
  delete Attenuation_Label;
  delete AttenuationSpinBox;
  delete dBLabelAtt;
  delete Topology_Label;
  delete Topology_Combo;
  delete Zin_Label;
  delete ZinSpinBox;
  delete Ohm_Zin_Label;
  delete Ohm_Zout_Label;
  delete ZoutSpinBox;
  delete Zout_Label;
  delete Pin_Label;
  delete Pin_SpinBox;
  delete Pin_units_Combo;
  delete Pdiss_R1_Label;
  delete Pdiss_R1_Lineedit;
  delete R1_Pdiss_Units_Combo;
  delete Pdiss_R2_Label;
  delete Pdiss_R2_Lineedit;
  delete R2_Pdiss_Units_Combo;
  delete Pdiss_R3_Label;
  delete Pdiss_R3_Lineedit;
  delete R3_Pdiss_Units_Combo;
  delete Pdiss_R4_Label;
  delete Pdiss_R4_Lineedit;
  delete R4_Pdiss_Units_Combo;
  delete freqLabel;
  delete freqSpinBox;
  delete FreqScaleCombo;
  delete LumpedImplementationCheckbox;
}

void AttenuatorDesignTool::UpdateDesignParameters() {
  AttenuatorDesignParameters Specs;
  Specs.Attenuation = AttenuationSpinBox->value();
  Specs.Frequency = getFreq();
  Specs.Zin = ZinSpinBox->value();
  Specs.Zout = ZoutSpinBox->value();
  Specs.Topology = Topology_Combo->currentText();
  Specs.Pin = getPowerW(Pin_SpinBox->value(), Pin_units_Combo->currentIndex());
  Specs.Lumped_TL = LumpedImplementationCheckbox->isChecked();

  AttenuatorDesigner *AttDesigner = new AttenuatorDesigner(Specs);
  AttDesigner->synthesize();
  SchContent = AttDesigner->Schematic;

  // Update power dissipation data
  setPdiss(AttDesigner->Pdiss);
  UpdatePowerDissipationData();
  delete AttDesigner;

  // EMIT SIGNAL TO SIMULATE
  QString TraceName = traceNameLineEdit->text();
  SchContent.Name = TraceName;
  SchContent.Type = QString("Attenuator"); // Indicate the main tool the kind of circuit to adjust default traces (in case no traces were selected)
  emit updateSchematic(SchContent);
  emit updateSimulation(SchContent);
}

// This function is triggered by a change in the current selected combo item
void AttenuatorDesignTool::on_TopoCombo_currentIndexChanged(int index) {
  switch (index) {
  case 0: // Pi attenuator
  case 1: // Tee attenuator
    Zin_Label->setText(QString("Zin"));
    // Show Zout input
    Zout_Label->show();
    ZoutSpinBox->show();
    Ohm_Zout_Label->show();

    // Hide frequency input
    freqLabel->hide();
    freqSpinBox->hide();
    FreqScaleCombo->hide();

    // Hide Pdiss for R4
    Pdiss_R4_Label->hide();
    Pdiss_R4_Lineedit->hide();
    R4_Pdiss_Units_Combo->hide();

    // Show Pdiss for R3
    Pdiss_R3_Label->show();
    Pdiss_R3_Lineedit->show();
    R3_Pdiss_Units_Combo->show();

    // Hide lumped component checkbox
    LumpedImplementationCheckbox->hide();
    break;

  case 2: // Bridged Tee
    // Show Pdiss for R3
    Pdiss_R3_Label->show();
    Pdiss_R3_Lineedit->show();
    R3_Pdiss_Units_Combo->show();

    // Show R4
    Pdiss_R4_Label->show();
    Pdiss_R4_Lineedit->show();
    R4_Pdiss_Units_Combo->show();

    // Hide lumped component checkbox
    LumpedImplementationCheckbox->hide();

    // Hide Zout
    Zout_Label->hide();
    ZoutSpinBox->hide();
    Ohm_Zout_Label->hide();

    // Replace Zin by Z0
    Zin_Label->setText(QString("Z0"));

    // Hide frequency input
    freqLabel->hide();
    freqSpinBox->hide();
    FreqScaleCombo->hide();
    break;

  case 3: // Reflection attenuator
    // Hide Pdiss for R3
    Pdiss_R3_Label->hide();
    Pdiss_R3_Lineedit->hide();
    R3_Pdiss_Units_Combo->hide();

    // Hide Pdiss for R4
    Pdiss_R4_Label->hide();
    Pdiss_R4_Lineedit->hide();
    R4_Pdiss_Units_Combo->hide();

    // Replace Zin by Z0
    Zin_Label->setText(QString("Z0"));

    // Hide Zout
    Zout_Label->hide();
    ZoutSpinBox->hide();
    Ohm_Zout_Label->hide();

    // Hide frequency input
    freqLabel->hide();
    freqSpinBox->hide();
    FreqScaleCombo->hide();

    // Hide lumped component checkbox
    LumpedImplementationCheckbox->hide();
    break;

  case 4: // Quarter-wave series
  case 5: // Quarter-wave shunt
    // Show lumped component checkbox
    LumpedImplementationCheckbox->show();

    // Replace Zin by Z0
    Zin_Label->setText(QString("Z0"));

    // Hide Zout
    Zout_Label->hide();
    ZoutSpinBox->hide();
    Ohm_Zout_Label->hide();

    // Shows Pdiss for R3
    Pdiss_R3_Label->show();
    Pdiss_R3_Lineedit->show();
    R3_Pdiss_Units_Combo->show();

    // Hide Pdiss for R4
    Pdiss_R4_Label->hide();
    Pdiss_R4_Lineedit->hide();
    R4_Pdiss_Units_Combo->hide();

    // Show frequency
    freqLabel->show();
    freqSpinBox->show();
    FreqScaleCombo->show();
  default:
    break;
  }
  UpdateDesignParameters();
}

// The purpose of this function is to trigger a design from the main application
void AttenuatorDesignTool::design() { UpdateDesignParameters(); }

// Given a power data in mW, dBm or dBuV[Z0], this function returns the power
// data in Watts
double AttenuatorDesignTool::getPowerW(double Pin, unsigned int index) {
  switch (index) {
  default:
  case 0:             // mW
    Pin = Pin * 1e-3; // mW -> W
    break;
  case 1: // W
    break;
  case 2:                            // dBm
    Pin = pow(10, 0.1 * (Pin - 30)); // dBm -> W
    break;
  case 3:                                 // dBuV 75Ohm
    Pin = pow(10, (0.1 * Pin - 12)) / 75; // dBuV [75Ohm] -> W
    break;
  case 4:                                 // dBuV 50Ohm
    Pin = pow(10, (0.1 * Pin - 12)) / 50; // dBuV [50Ohm] -> W
    break;
  case 5:                                // dBmV 75Ohm
    Pin = pow(10, (0.1 * Pin - 6)) / 75; // dBmV [75Ohm] -> W
    break;
  case 6:                                // dBmV 50Ohm
    Pin = pow(10, (0.1 * Pin - 6)) / 50; // dBmV [50Ohm] -> W
    break;
  }
}

// Given a power data in W, this function converts the power
// to mW, dBm or dBuV[Z0]
double AttenuatorDesignTool::ConvertPowerFromW(double Pin, unsigned int index) {
  switch (index) {
  default:
  case 0:            // mW
    Pin = Pin * 1e3; // mW -> W
    break;
  case 1: // W
    break;
  case 2:                       // dBm
    Pin = 10 * log10(Pin) + 30; // W -> dBm
    break;
  case 3:                           // dBuV 75Ohm
    Pin = 10 * log10(Pin) + 138.75; // W -> dBuV [75Ohm]
    break;
  case 4:                           // dBuV 50Ohm
    Pin = 10 * log10(Pin) + 136.99; // W -> dBuV [50Ohm]
    break;
  case 5:                          // dBmV 75Ohm
    Pin = 10 * log10(Pin) + 78.75; // W -> dBmV [75Ohm]
    break;
  case 6:                          // dBmV 50Ohm
    Pin = 10 * log10(Pin) + 76.99; // W -> dBmV [50Ohm]
    break;
  }
  return Pin;
}

// This function scales the frequency according to 'FreqScaleCombo' combobox
double AttenuatorDesignTool::getFreq() {
  double exp = 1;
  switch (FreqScaleCombo->currentIndex()) {
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
  return freqSpinBox->value() * pow(10, exp);
}

void AttenuatorDesignTool::UpdatePowerDissipationData() {
  // Update R1
  Pdiss_R1_Lineedit->setText(QString("%1").arg(
      ConvertPowerFromW(Pdiss.R1, R1_Pdiss_Units_Combo->currentIndex())));

  // Update R2
  Pdiss_R2_Lineedit->setText(QString("%1").arg(
      ConvertPowerFromW(Pdiss.R2, R2_Pdiss_Units_Combo->currentIndex())));

  // Update R3
  Pdiss_R3_Lineedit->setText(QString("%1").arg(
      ConvertPowerFromW(Pdiss.R3, R3_Pdiss_Units_Combo->currentIndex())));

  // Update R4
  Pdiss_R4_Lineedit->setText(QString("%1").arg(
      ConvertPowerFromW(Pdiss.R4, R4_Pdiss_Units_Combo->currentIndex())));
}

void AttenuatorDesignTool::setPdiss(struct PdissAtt Pdiss_) { Pdiss = Pdiss_; }
