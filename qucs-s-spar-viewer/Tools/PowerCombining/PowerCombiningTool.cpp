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

#include "PowerCombiningTool.h"

PowerCombiningTool::PowerCombiningTool(QWidget *parent) : QWidget(parent) {
  QGridLayout *PowerCombinerDesignLayout = new QGridLayout();

  int layout_row =
      0; // Row index. This is useful to add a new line on the layout without
         // the need of modifying manually all the widgets.

  // Topology
  TopoLabel = new QLabel("Topology");
  TopoCombo = new QComboBox();
  TopoCombo->addItem("Wilkinson");
  TopoCombo->addItem("Multistage Wilkinson");
  TopoCombo->addItem("T-junction");
  TopoCombo->addItem("Branchline");
  TopoCombo->addItem("Double box branchline");
  TopoCombo->addItem("Bagley");
  TopoCombo->addItem("Gysel");
  TopoCombo->addItem("Lim-Eom");
  TopoCombo->addItem("3 Way Wilkinson Improved Isolation");
  TopoCombo->addItem("Recombinant 3 Way Wilkinson");
  TopoCombo->addItem("Travelling Wave");
  TopoCombo->addItem("Tree");
  PowerCombinerDesignLayout->addWidget(TopoLabel, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(TopoCombo, layout_row, 1);

  // Transmission line implementation
  layout_row++;
  TL_Implementation_Label = new QLabel(QString("TLIN implementation"));
  TL_Implementation_Combo = new QComboBox();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Stripline");
  TL_Implementation_Combo->addItem("Lumped");
  PowerCombinerDesignLayout->addWidget(TL_Implementation_Label, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(TL_Implementation_Combo, layout_row, 1);

  // Number of outputs
  layout_row++;
  BranchesCombo = new QComboBox();
  BranchesCombo->addItem("2");
  BranchesCombo->hide();

  number_Output_Label = new QLabel("Number of outputs");
  PowerCombinerDesignLayout->addWidget(number_Output_Label, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(BranchesCombo, layout_row, 1);
  number_Output_Label->hide();

  // Ref impedance
  layout_row++;
  RefImp = new QLabel("Z0");
  RefImpSpinbox = new QDoubleSpinBox();
  RefImpSpinbox->setMinimum(1);
  RefImpSpinbox->setValue(50);
  RefImpSpinbox->setSingleStep(1); // 1 Ohm step
  OhmLabel = new QLabel(QChar(0xa9, 0x03));
  OhmLabel->setFixedWidth(15);
  PowerCombinerDesignLayout->addWidget(RefImp, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(RefImpSpinbox, layout_row, 1);
  PowerCombinerDesignLayout->addWidget(OhmLabel, layout_row, 2);

  // Frequency
  layout_row++;
  FreqLabel = new QLabel("Frequency");
  FreqSpinbox = new QDoubleSpinBox();
  FreqSpinbox->setMinimum(1);
  FreqSpinbox->setMaximum(1e6);
  FreqSpinbox->setDecimals(0);
  FreqSpinbox->setValue(1000);
  FreqSpinbox->setSingleStep(1); // Step fixed to 1 Hz/kHz/MHz/GHz
  FreqScaleCombo = new QComboBox();
  FreqScaleCombo->addItem("GHz");
  FreqScaleCombo->addItem("MHz");
  FreqScaleCombo->addItem("kHz");
  FreqScaleCombo->addItem("Hz");
  FreqScaleCombo->setCurrentIndex(1); // MHz
  PowerCombinerDesignLayout->addWidget(FreqLabel, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(FreqSpinbox, layout_row, 1);
  PowerCombinerDesignLayout->addWidget(FreqScaleCombo, layout_row, 2);

  // Output power ratio
  layout_row++;
  K1Label = new QLabel("Output Power ratio");

  QHBoxLayout *hbox = new QHBoxLayout();
  K1Spinbox = new QDoubleSpinBox();
  K1Spinbox->setValue(0); // Equal split ratio
  K1Spinbox->setMinimum(-20);
  K1Spinbox->setMaximum(20);
  K1Spinbox->setSingleStep(0.1); // 0.1dB step

  K2Spinbox = new QDoubleSpinBox();
  K2Spinbox->setValue(0); // Equal split ratio
  K2Spinbox->setMinimum(-20);
  K2Spinbox->setMaximum(20);
  K2Spinbox->setSingleStep(0.1); // 0.1dB step
  K2Spinbox->setVisible(false);

  K3Spinbox = new QDoubleSpinBox();
  K3Spinbox->setValue(0); // Equal split ratio
  K3Spinbox->setMinimum(-20);
  K3Spinbox->setMaximum(20);
  K3Spinbox->setSingleStep(0.1); // 0.1dB step
  K3Spinbox->setVisible(false);

  hbox->addWidget(K1Spinbox);
  hbox->addWidget(K2Spinbox);
  hbox->addWidget(K3Spinbox);

  K1LabeldB = new QLabel("dB");
  K1LabeldB->setFixedWidth(20);
  PowerCombinerDesignLayout->addWidget(K1Label, layout_row, 0);
  PowerCombinerDesignLayout->addLayout(hbox, layout_row, 1);
  PowerCombinerDesignLayout->addWidget(K1LabeldB, layout_row, 2);

  // Number of stages
  layout_row++;
  NStagesSpinbox = new QSpinBox();
  NStagesSpinbox->setValue(2);
  NStagesSpinbox->setMinimum(2);
  NStagesLabel = new QLabel("Number of stages");
  PowerCombinerDesignLayout->addWidget(NStagesLabel, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(NStagesSpinbox, layout_row, 1);
  NStagesLabel->hide();
  NStagesSpinbox->hide();

  // Ideal transmission line attenuation coeffient
  layout_row++;
  AlphaLabel = new QLabel("Attenuation coefficient");
  AlphaSpinbox = new QDoubleSpinBox();
  AlphaSpinbox->setValue(0);        // Initial value: 0 dB/m
  AlphaSpinbox->setSingleStep(0.1); // 0.1dB
  AlphadBLabel = new QLabel("dB/m");
  AlphadBLabel->setVisible(false);
  AlphaLabel->setVisible(false);
  AlphaSpinbox->setVisible(false);
  PowerCombinerDesignLayout->addWidget(AlphaLabel, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(AlphaSpinbox, layout_row, 1);
  PowerCombinerDesignLayout->addWidget(AlphadBLabel, layout_row, 2);

  // Units
  layout_row++;
  UnitsLabel = new QLabel("Length unit");
  UnitsCombo = new QComboBox();
  UnitsCombo->addItem("mm");
  UnitsCombo->addItem("mil");
  UnitsCombo->addItem("um");
  UnitsCombo->addItem("nm");
  UnitsCombo->addItem("inch");
  UnitsCombo->addItem("ft");
  UnitsCombo->addItem("m");
  PowerCombinerDesignLayout->addWidget(UnitsLabel, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(UnitsCombo, layout_row, 1);

  // Widgets to add a trace to plot
  layout_row++;
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("PowComb1");
  PowerCombinerDesignLayout->addWidget(traceNameLabel, layout_row, 0);
  PowerCombinerDesignLayout->addWidget(traceNameLineEdit, layout_row, 1);

  this->setLayout(PowerCombinerDesignLayout);

  // Make connection between widgets and handler functions to update the design
  // in real time
  connect(TL_Implementation_Combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(BranchesCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(RefImpSpinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(FreqSpinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(FreqScaleCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(K1Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(K2Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(K2Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(NStagesSpinbox, SIGNAL(valueChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(AlphaSpinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(UnitsCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(TopoCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_TopoCombo_currentIndexChanged(int)));

  Bagley_Validator = new BagleyValidator(this);
}

PowerCombiningTool::~PowerCombiningTool() {
  delete TopoLabel;
  delete TopoCombo;
  delete TL_Implementation_Label;
  delete TL_Implementation_Combo;
  delete BranchesCombo;
  delete number_Output_Label;
  delete RefImp;
  delete RefImpSpinbox;
  delete OhmLabel;
  delete FreqLabel;
  delete FreqSpinbox;
  delete FreqScaleCombo;
  delete K1Label;
  delete K1Spinbox;
  delete K2Spinbox;
  delete K3Spinbox;
  delete K1LabeldB;
  delete NStagesSpinbox;
  delete NStagesLabel;
  delete AlphaLabel;
  delete AlphaSpinbox;
  delete AlphadBLabel;
  delete UnitsLabel;
  delete UnitsCombo;
  delete traceNameLabel;
  delete traceNameLineEdit;
  delete Bagley_Validator;
}

void PowerCombiningTool::UpdateDesignParameters() {
  PowerCombinerParams Specs;
  Specs.Type = TopoCombo->currentText();
  Specs.Noutputs = BranchesCombo->currentText().toInt();
  Specs.OutputRatio.push_back(pow(10, K1Spinbox->value() / 20.));
  if (Specs.Type == "Lim-Eom") { // Supports arbitrary 3 way split ratio
    Specs.OutputRatio.push_back(pow(10, K2Spinbox->value() / 20.));
    Specs.OutputRatio.push_back(pow(10, K3Spinbox->value() / 20.));
  }
  Specs.alpha = AlphaSpinbox->value();
  Specs.units = UnitsCombo->currentText();
  Specs.Nstages = NStagesSpinbox->value();
  Specs.freq = FreqSpinbox->value() * getScaleFreq();
  Specs.Z0 = RefImpSpinbox->value();

  ////////////////////////////////////////////////////////////////////////////
  // Transmission line implementation
  static const QMap<QString, TransmissionLineType> tlMap{
      {"Ideal", TransmissionLineType::Ideal},
      {"Microstrip", TransmissionLineType::MLIN},
      {"Stripline", TransmissionLineType::SLIN},
      {"Lumped", TransmissionLineType::Lumped}};

  const QString tlKey = TL_Implementation_Combo->currentText();
  if (tlMap.contains(tlKey)) {
    Specs.TL_implementation = tlMap.value(tlKey);
  }
  ////////////////////////////////////////////////////////////////////////////

  PowerCombinerDesigner *PowCombD = new PowerCombinerDesigner(Specs);
  PowCombD->synthesize();
  SchContent = PowCombD->getSchematic();
  delete PowCombD;

  QString TraceName = traceNameLineEdit->text();
  SchContent.Name = TraceName;
  SchContent.Type = QString(
      "Power Combiner"); // Indicate the main tool the kind of circuit to adjust
                         // default traces (in case no traces were selected)
  emit updateSchematic(SchContent);
  emit updateSimulation(SchContent);
}

//---------------------------------------------------------------------------
// This function scales the frequency according to 'FreqScaleCombo' combobox
double PowerCombiningTool::getScaleFreq() {
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
  return pow(10, exp);
}

//------------------------------------------------------------------
// This function changes the window according to the selected topology
void PowerCombiningTool::on_TopoCombo_currentIndexChanged(int index) {
  // Change settings
  if ((index == 0) | (index == 2) || (index == 3)) // Wilkinson, Tee, Branchline
  {
    // Block signals before adjusting parameters
    BranchesCombo->blockSignals(true);

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    K1Spinbox->setVisible(true);
    K1Label->setVisible(true);
    K1LabeldB->setVisible(true);
    BranchesCombo->clear();
    BranchesCombo->addItem("2");
    BranchesCombo->hide();
    number_Output_Label->hide();

    // Unblock signals after adjusting parameters
    BranchesCombo->blockSignals(false);

  } else { // The rest of power combiners do not support unequal power ratio
    K1Spinbox->setVisible(false);
    K1Label->setVisible(false);
    K1LabeldB->setVisible(false);
  }

  if ((index == 3) || (index == 4) ||
      (index == 6)) // Branchline, double-box branchline and Gysel
  {                 // The Gysel power combiner has only two output branches
    // Block signals before adjusting parameters
    BranchesCombo->blockSignals(true);

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    BranchesCombo->clear();
    BranchesCombo->addItem("2");
    BranchesCombo->hide();
    number_Output_Label->hide();

    // Unblock signals after adjusting parameters
    BranchesCombo->blockSignals(false);
  }

  if (TopoCombo->currentText() ==
      "Lim-Eom") { // The Lim-Eom combiner can handle arbitrary split ratios for
                   // the three outputs
    K1Label->setVisible(true);
    K1LabeldB->setVisible(true);
    K1Spinbox->setVisible(true);
    K2Spinbox->setVisible(true);
    K3Spinbox->setVisible(true);

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

  } else {
    K2Spinbox->setVisible(false);
    K3Spinbox->setVisible(false);
  }

  if (index == 1) // Multistage Wilkinson. So far, it is not possible to
                  // implement more than 7 stages
  {
    // Block signals before adjusting parameters
    NStagesSpinbox->blockSignals(true);
    BranchesCombo->blockSignals(true);

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    NStagesSpinbox->setMinimum(2);
    NStagesSpinbox->setMaximum(7);
    NStagesSpinbox->setValue(2);
    NStagesLabel->setVisible(true);
    NStagesSpinbox->setVisible(true);
    BranchesCombo->clear();
    BranchesCombo->addItem("2"); // 2 outputs only
    BranchesCombo->hide();
    number_Output_Label->hide();

    // Unblock signals after adjusting parameters
    NStagesSpinbox->blockSignals(false);
    BranchesCombo->blockSignals(false);

  } else // There are no more multistage combiners implemented
  {
    NStagesLabel->setVisible(false);
    NStagesSpinbox->setVisible(false);
  }
  if (index == 5) // Bagley
  {
    // Block signals before adjusting parameters
    BranchesCombo->blockSignals(true);

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    BranchesCombo->clear();
    BranchesCombo->addItem("3");
    BranchesCombo->addItem("5");
    BranchesCombo->addItem("7");
    BranchesCombo->show();
    number_Output_Label->show();
    BranchesCombo->setValidator(Bagley_Validator);

    // Unblock signals after adjusting parameters
    BranchesCombo->blockSignals(false);

  } else {
    BranchesCombo->setValidator(NULL);
  }

  if (index == 7) // Travelling wave
  {
    // Block signals before adjusting parameters
    BranchesCombo->blockSignals(true);

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    BranchesCombo->clear();
    BranchesCombo->addItem("3");
    BranchesCombo->addItem("4");
    BranchesCombo->addItem("5");
    BranchesCombo->addItem("6");
    BranchesCombo->show();
    number_Output_Label->show();

    // Unblock signals after adjusting parameters
    BranchesCombo->blockSignals(false);
  }
  if (index == 8) // Tree
  {
    // Block signals before adjusting parameters
    BranchesCombo->blockSignals(true);

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    BranchesCombo->clear();
    BranchesCombo->addItem("4");
    BranchesCombo->addItem("8");
    BranchesCombo->addItem("16");
    BranchesCombo->show();
    number_Output_Label->show();

    // Unblock signals after adjusting parameters
    BranchesCombo->blockSignals(false);
  }

  UpdateDesignParameters();
}

// The purpose of this function is to trigger a design from the main application
void PowerCombiningTool::design() { UpdateDesignParameters(); }

SchematicContent PowerCombiningTool::getSchematic() { return SchContent; }
