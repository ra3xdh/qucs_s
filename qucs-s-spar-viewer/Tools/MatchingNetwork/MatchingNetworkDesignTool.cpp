/***************************************************************************
                                MatchingNetworkDesignTool.cpp
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
#include "MatchingNetworkDesignTool.h"

MatchingNetworkDesignTool::MatchingNetworkDesignTool(QWidget *parent): QWidget(parent) {
  QGridLayout *MatchingNetworkDesignLayout = new QGridLayout();

  // Topology
  Topology_Label = new QLabel("Topology");
  Topology_Combo = new QComboBox();
  QStringList matching_methods;
  matching_methods.append(tr("L-section"));
  matching_methods.append(tr("Single stub"));
  matching_methods.append(tr("Double stub"));
  matching_methods.append(QString("%1 %2/4").arg(tr("Multisection ")).arg(QString(QChar(0xBB, 0x03))));
  matching_methods.append(tr("Cascaded L-sections"));
  matching_methods.append(QString("%1/8 + %1/4 line").arg(QChar(0xBB, 0x03)));
 /* matching_methods.append(QString("%1/4 line").arg(QChar(0xBB, 0x03)));
  matching_methods.append(QString("%1/8 + %1/4 line").arg(QChar(0xBB, 0x03)));
  matching_methods.append(QString("%1-type").arg(QChar(0xC0, 0x03)));
  matching_methods.append(tr("Tee-Type"));
  matching_methods.append(tr("Tapped C transformer"));
  matching_methods.append(tr("Tapped L transformer"));
  matching_methods.append(tr("Double tapped resonator"));
  matching_methods.append(tr("Single tuned transformer"));
  matching_methods.append(tr("Parallel double-tuned transformer"));
  matching_methods.append(tr("Series double-tuned transformer"));*/
  Topology_Combo->addItems(matching_methods);
  MatchingNetworkDesignLayout->addWidget(Topology_Label, 1, 0);
  MatchingNetworkDesignLayout->addWidget(Topology_Combo, 1, 1);

  // Solution number. In some matching techniques there are two possible
  // solutions
  QWidget *SolutionWidget = new QWidget();
  QHBoxLayout *SolutionLayout = new QHBoxLayout();
  Solution1_RB = new QRadioButton("Solution 1");
  Solution2_RB = new QRadioButton("Solution 2");
  Solution1_RB->setChecked(true);
  SolutionLayout->addWidget(Solution1_RB);
  SolutionLayout->addWidget(Solution2_RB);
  SolutionWidget->setLayout(SolutionLayout);
  MatchingNetworkDesignLayout->addWidget(SolutionWidget, 1, 2);

  // Stub termination (only for single and double stub matching)
  StubTermination_Label = new QLabel(QString("Stub Termination"));
  MatchingNetworkDesignLayout->addWidget(StubTermination_Label, 2, 0);

  StubTermination_ComboBox = new QComboBox();
  StubTermination_ComboBox->addItem(QString("Open circuit"));
  StubTermination_ComboBox->addItem(QString("Short circuit"));
  MatchingNetworkDesignLayout->addWidget(StubTermination_ComboBox, 2, 1);


  // Weighting settings (multisection lambda/4 transformers)
  Weighting_GroupBox = new QGroupBox(tr("Weighting"));
  QGridLayout *WeightingLayout = new QGridLayout();

         // Combobox: Binomial or Chebyshev
  QLabel *WeightingMethodLabel = new QLabel(tr("Method"));
  Weighting_Combo = new QComboBox();
  Weighting_Combo->addItem(tr("Binomial"));
  Weighting_Combo->addItem(tr("Chebyshev"));
  WeightingLayout->addWidget(WeightingMethodLabel, 0, 0);
  WeightingLayout->addWidget(Weighting_Combo, 0, 1);

         // Ripple parameter (only for Chebyshev)
  Ripple_Label = new QLabel(tr("Ripple"));
  Ripple_SpinBox = new QDoubleSpinBox();
  Ripple_SpinBox->setRange(0.001, 1.0);       // typical range (linear)
  Ripple_SpinBox->setSingleStep(0.01);
  Ripple_SpinBox->setDecimals(3);
  Ripple_SpinBox->setValue(0.05);             // default 5% ripple
  WeightingLayout->addWidget(Ripple_Label, 1, 0);
  WeightingLayout->addWidget(Ripple_SpinBox, 1, 1);
  Weighting_GroupBox->setLayout(WeightingLayout);
  MatchingNetworkDesignLayout->addWidget(Weighting_GroupBox, 3, 0, 1, 3);

         // Hide ripple controls if Binomial selected
  Ripple_Label->setVisible(false);
  Ripple_SpinBox->setVisible(false);

         // Number of sections (integer).
  Sections_Label = new QLabel(tr("Sections"));
  Sections_SpinBox = new QSpinBox();
  Sections_SpinBox->setRange(2, 10);           // reasonable range
  Sections_SpinBox->setValue(3);               // default 3 sections
  MatchingNetworkDesignLayout->addWidget(Sections_Label, 4, 0);
  MatchingNetworkDesignLayout->addWidget(Sections_SpinBox, 4, 1);


  // Input impedance
  Zin_Label = new QLabel("ZS");
  ZinRSpinBox = new QDoubleSpinBox();
  ZinRSpinBox->setMinimum(0.5);
  ZinRSpinBox->setMaximum(10000);
  ZinRSpinBox->setSingleStep(0.5);
  ZinRSpinBox->setValue(50);
  ZinRSpinBox->setDecimals(1);
  Ohm_Zin_Label = new QLabel(QChar(0xa9, 0x03));
  MatchingNetworkDesignLayout->addWidget(Zin_Label, 5, 0);
  MatchingNetworkDesignLayout->addWidget(ZinRSpinBox, 5, 1);
  MatchingNetworkDesignLayout->addWidget(Ohm_Zin_Label, 5, 2);

  // Output impedance
  Zout_Label = new QLabel("ZL");
  ZoutRSpinBox = new QDoubleSpinBox();
  ZoutRSpinBox->setMinimum(0.5);
  ZoutRSpinBox->setMaximum(10000);
  ZoutRSpinBox->setSingleStep(0.5);
  ZoutRSpinBox->setValue(75);
  ZoutRSpinBox->setDecimals(1);
  Zout_J = new QLabel("+j");
  ZoutISpinBox = new QDoubleSpinBox();
  ZoutISpinBox->setMinimum(-10000);
  ZoutISpinBox->setMaximum(10000);
  ZoutISpinBox->setSingleStep(0.5);
  ZoutISpinBox->setValue(0);
  ZoutISpinBox->setDecimals(1);
  Ohm_Zout_Label = new QLabel(QChar(0xa9, 0x03));
  MatchingNetworkDesignLayout->addWidget(Zout_Label, 6, 0);
  MatchingNetworkDesignLayout->addWidget(ZoutRSpinBox, 6, 1);
  MatchingNetworkDesignLayout->addWidget(Zout_J, 6, 2);
  MatchingNetworkDesignLayout->addWidget(ZoutISpinBox, 6, 3);
  MatchingNetworkDesignLayout->addWidget(Ohm_Zout_Label, 6, 4);

  // Frequency range. Start
  FreqStart_Label = new QLabel("freq");
  FreqStart_Spinbox = new QDoubleSpinBox();
  FreqStart_Spinbox->setMinimum(1);
  FreqStart_Spinbox->setMaximum(1e6);
  FreqStart_Spinbox->setDecimals(0);
  FreqStart_Spinbox->setValue(1000);
  FreqStart_Spinbox->setSingleStep(1); // Step fixed to 1 Hz/kHz/MHz/GHz
  QStringList FreqScale;
  FreqScale << "GHz"
            << "MHz"
            << "kHz"
            << "Hz";
  FreqStart_Scale_Combo = new QComboBox();
  FreqStart_Scale_Combo->addItems(FreqScale);
  FreqStart_Scale_Combo->setCurrentIndex(1);
  MatchingNetworkDesignLayout->addWidget(FreqStart_Label, 7, 0);
  MatchingNetworkDesignLayout->addWidget(FreqStart_Spinbox, 7, 1);
  MatchingNetworkDesignLayout->addWidget(FreqStart_Scale_Combo, 7, 2);

  // Frequency range. End
  FreqEnd_Label = new QLabel("Freq. stop");
  FreqEnd_Spinbox = new QDoubleSpinBox();
  FreqEnd_Spinbox->setMinimum(1);
  FreqEnd_Spinbox->setMaximum(1e6);
  FreqEnd_Spinbox->setDecimals(0);
  FreqEnd_Spinbox->setValue(1000);
  FreqEnd_Spinbox->setSingleStep(1); // Step fixed to 1 Hz/kHz/MHz/GHz
  FreqEnd_Scale_Combo = new QComboBox();
  FreqEnd_Scale_Combo->addItems(FreqScale);
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Label, 8, 0);
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Spinbox, 8, 1);
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Scale_Combo, 8, 2);

  // Widgets to add a trace to plot
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Match1");
  MatchingNetworkDesignLayout->addWidget(traceNameLabel, 9, 0);
  MatchingNetworkDesignLayout->addWidget(traceNameLineEdit, 9, 1);

  // Since it is more common to design narrowband matching networks than
  // broadband, the end-freq widgets are hidden
  FreqEnd_Label->hide();
  FreqEnd_Spinbox->hide();
  FreqEnd_Scale_Combo->hide();

  connect(Topology_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_TopoCombo_currentIndexChanged(int)));
  connect(StubTermination_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(ZinRSpinBox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(ZoutRSpinBox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(ZoutISpinBox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqStart_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqStart_Scale_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Scale_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(Solution1_RB, SIGNAL(clicked(bool)), this, SLOT(UpdateDesignParameters()));
  connect(Solution2_RB, SIGNAL(clicked(bool)), this, SLOT(UpdateDesignParameters()));
  connect(Weighting_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(Ripple_SpinBox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(Sections_SpinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateDesignParameters()));

  this->setLayout(MatchingNetworkDesignLayout);

  on_TopoCombo_currentIndexChanged(0);
}

MatchingNetworkDesignTool::~MatchingNetworkDesignTool() {
  delete Broadband_Checkbox;
  delete Topology_Label;
  delete Topology_Combo;
  delete Solution1_RB;
  delete Solution2_RB;
  delete Zin_Label;
  delete ZinRSpinBox;
  delete Ohm_Zin_Label;
  delete Ohm_Zout_Label;
  delete ZoutRSpinBox;
  delete Zout_J;
  delete ZoutISpinBox;
  delete Zout_Label;
  delete FreqStart_Label;
  delete FreqStart_Scale_Combo;
  delete FreqStart_Spinbox;
  delete FreqEnd_Label;
  delete FreqEnd_Scale_Combo;
  delete FreqEnd_Spinbox;
}

void MatchingNetworkDesignTool::UpdateDesignParameters() {
  MatchingNetworkDesignParameters Specs;
  Specs.Zin = std::complex<double>(ZinRSpinBox->value(), 0);
  Specs.Zout =
      std::complex<double>(ZoutRSpinBox->value(), ZoutISpinBox->value());
  Specs.Topology = Topology_Combo->currentText();

  if (Solution1_RB->isChecked()) {
    Specs.Solution = 1;
  } else {
    Specs.Solution = 2;
  }

  Specs.freqStart = FreqStart_Spinbox->value() *
                    getScaleFreq(FreqStart_Scale_Combo->currentIndex());
  Specs.freqEnd = FreqEnd_Spinbox->value() *
                  getScaleFreq(FreqEnd_Scale_Combo->currentIndex());

  // Single/double stub matching
  Specs.OpenShort = StubTermination_ComboBox->currentIndex(); // 0: Open circuit stub; 1: Short-circuited stub

  // Multisection lambda/4 transformers
  Specs.NSections = Sections_SpinBox->value();
  Specs.Weigthing = Weighting_Combo->currentText(); // Chebyshev or binomial
  Specs.gamma_MAX = Ripple_SpinBox->value(); // Ripple of the Chebyshev taper


  int topology = Topology_Combo->currentIndex();
  switch (topology){
    case 0: {// L-section
      Lsection *L = new Lsection(Specs);
      L->synthesize();
      SchContent = L->Schematic;
      delete L;
      break;
    }
    case 1: { // Single-stub
      SingleStub *SSM = new SingleStub(Specs);
      SSM->synthesize();
      SchContent = SSM->Schematic;
      delete SSM;
      break;
    }
    case 2: { // Double-stub
      DoubleStub *DSM = new DoubleStub(Specs);
      DSM->synthesize();
      SchContent = DSM->Schematic;
      delete DSM;
      break;
    }
    case 3: { // Multisection lambda/4
      MultisectionQuarterWave *MSL4 = new MultisectionQuarterWave(Specs);
      MSL4->synthesize();
      SchContent = MSL4->Schematic;
      delete MSL4;
      break;
    }
    case 4: { // Cascaded LC sections
      CascadedLCSections *CLCM = new CascadedLCSections(Specs);
      CLCM->synthesize();
      SchContent = CLCM->Schematic;
      delete CLCM;
      break;
    }
    case 5: { //lambda/8 + lambda/4
      Lambda8Lambda4 *L8L4 = new Lambda8Lambda4(Specs);
      L8L4->synthesize();
      SchContent = L8L4->Schematic;
      delete L8L4;
      break;
    }
  }


  // EMIT SIGNAL TO SIMULATE
  QString TraceName = traceNameLineEdit->text();
  SchContent.Name = TraceName;
  SchContent.Type = QString("Matching"); // Indicate the main tool the kind of circuit to adjust default traces (in case no traces were selected)
  emit updateSchematic(SchContent);
  emit updateSimulation(SchContent);
}

// This function is triggered by a change in the current selected combo item. It
// determines the visibility of the UI components
void MatchingNetworkDesignTool::on_TopoCombo_currentIndexChanged(int index) {
  switch (index) {
  case 0: // Lsection

    // Show imaginary part of the load impedance
    Zout_J->setText(QChar(0xa9, 0x03));// Put "+j" text in 3rd position. It is changed if topologies for real ZL were selected
    Ohm_Zout_Label->show();// Show Omega symbol in the 5th position
    ZoutISpinBox->show();

    // Show Lsection matching solutions
    Solution1_RB->show();
    Solution2_RB->show();

    // Hide number of sections
    Sections_Label->hide();
    Sections_SpinBox->hide();

    // Hide open circuit termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

    // Hide lambda/4 weighting
    Weighting_GroupBox->hide();
    break;

  case 1: // Single-stub matching
  case 2: // Double-stub matching

    // Show imaginary part of the load impedance
    Zout_J->setText(QChar(0xa9, 0x03));// Put "+j" text in 3rd position. It is changed if topologies for real ZL were selected
    Ohm_Zout_Label->show();// Show Omega symbol in the 5th position
    ZoutISpinBox->show();

    // Hide Lsection matching solutions
    Solution1_RB->hide();
    Solution2_RB->hide();

    // Hide number of sections
    Sections_Label->hide();
    Sections_SpinBox->hide();

    // Hide open circuit termination options
    StubTermination_Label->show();
    StubTermination_ComboBox->show();

    // Hide lambda/4 weighting
    Weighting_GroupBox->hide();
    break;

  case 3: // Multisection lambda/4 transformer

    // Hide imaginary part of the load impedance
    Zout_J->setText(QChar(0xa9, 0x03));// Put Omega symbol in 3rd position
    Ohm_Zout_Label->hide();// Hide Omega symbol in the 5th position
    ZoutISpinBox->hide();

    // Hide Lsection matching solutions
    Solution1_RB->hide();
    Solution2_RB->hide();

    // Show number of sections
    Sections_Label->show();
    Sections_SpinBox->show();

    // Hide open circuit termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

    // Show lambda/4 weighting
    Weighting_GroupBox->show();
    break;

  case 4: // Cascaded L-sections transformer

           // Hide imaginary part of the load impedance (real only topology)
    Zout_J->setText(QChar(0xa9, 0x03));// Put Omega symbol in 3rd position
    Ohm_Zout_Label->hide();// Hide Omega symbol in the 5th position
    ZoutISpinBox->hide();

           // Show Lsection matching solutions
    Solution1_RB->show();
    Solution2_RB->show();

    // Show number of sections
    Sections_Label->show();
    Sections_SpinBox->show();

           // Hide open circuit termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

           // Hide lambda/4 weighting
    Weighting_GroupBox->hide();
    break;

  case 5: // lambda/4 + lambda/4
    // Show imaginary part of the load impedance
    Zout_J->setText(QChar(0xa9, 0x03));// Put "+j" text in 3rd position. It is changed if topologies for real ZL were selected
    Ohm_Zout_Label->show();// Show Omega symbol in the 5th position

    // Hide Lsection matching solutions
    Solution1_RB->hide();
    Solution2_RB->hide();

           // Hide number of sections
    Sections_Label->hide();
    Sections_SpinBox->hide();

           // Hide open circuit termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

           // Hide lambda/4 weighting
    Weighting_GroupBox->hide();

    break;

  default:
    break;
  }
  UpdateDesignParameters();
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
