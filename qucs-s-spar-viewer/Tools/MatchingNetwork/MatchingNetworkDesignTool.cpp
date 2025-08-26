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

  // Checkbox to select 1-port or 2-port matching
  TwoPortCheckBox = new QCheckBox();
  TwoPortCheckBox->setText(QString("2-port matching"));
  TwoPortCheckBox->setChecked(false);
  MatchingNetworkDesignLayout->addWidget(TwoPortCheckBox, 0, 0);

  // Input matching network setup
  InputMatchingSetupWidget = new MatchingNetworkParametersWidget();
  MatchingNetworkDesignLayout->addWidget(InputMatchingSetupWidget, 1, 0, 1, 3);

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
  MatchingNetworkDesignLayout->addWidget(Zout_Label, 2, 0);
  MatchingNetworkDesignLayout->addWidget(ZoutRSpinBox, 2, 1);
  MatchingNetworkDesignLayout->addWidget(Zout_J, 2, 2);
  MatchingNetworkDesignLayout->addWidget(ZoutISpinBox, 2, 3);
  MatchingNetworkDesignLayout->addWidget(Ohm_Zout_Label, 2, 4);

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
  MatchingNetworkDesignLayout->addWidget(FreqStart_Label, 3, 0);
  MatchingNetworkDesignLayout->addWidget(FreqStart_Spinbox, 3, 1);
  MatchingNetworkDesignLayout->addWidget(FreqStart_Scale_Combo, 3, 2);

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
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Label, 4, 0);
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Spinbox, 4, 1);
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Scale_Combo, 4, 2);


  // Two-port data entry widgets
  two_port_GroupBox = new QGroupBox();
  QGridLayout *TwoPortLayout = new QGridLayout();

  enter_S2P_file_CheckBox = new QCheckBox();
  enter_S2P_file_CheckBox->setText(QString("Enter s2p data file"));
  browse_S2P_Button = new QPushButton(QString("Browse"));
  s2p_filename_Label = new QLabel(QString(""));

  // S2P entry mode
  TwoPortLayout->addWidget(enter_S2P_file_CheckBox, 0, 0);
  TwoPortLayout->addWidget(browse_S2P_Button, 0, 1);
  TwoPortLayout->addWidget(s2p_filename_Label, 0, 2);

  // Input format: re/im or mag/ang selection
  input_format_Label = new QLabel(QString("Complex format"));
  input_format_Combo = new QComboBox();
  input_format_Combo->addItem(QString("Real / Imaginary"));
  input_format_Combo->addItem(QString("Magnitude / Angle"));

  TwoPortLayout->addWidget(input_format_Label, 1, 0);
  TwoPortLayout->addWidget(input_format_Combo, 1, 1);

  // Manual entry s2p

  S11_Label = new QLabel(QString("S11"));
  S11_A_SpinBox = new QDoubleSpinBox();
  S11_Separator_Label = new QLabel(QString("+j"));
  S11_B_SpinBox = new QDoubleSpinBox();

  TwoPortLayout->addWidget(S11_Label, 2, 0);
  TwoPortLayout->addWidget(S11_A_SpinBox, 2, 1);
  TwoPortLayout->addWidget(S11_Separator_Label, 2, 2);
  TwoPortLayout->addWidget(S11_B_SpinBox, 2, 3);

  two_port_GroupBox->setLayout(TwoPortLayout);

  MatchingNetworkDesignLayout->addWidget(two_port_GroupBox, 5, 1, 0, 7);


  // Widgets to add a trace to plot
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Match1");
  MatchingNetworkDesignLayout->addWidget(traceNameLabel, 6, 0);
  MatchingNetworkDesignLayout->addWidget(traceNameLineEdit, 6, 1);

  // Since it is more common to design narrowband matching networks than
  // broadband, the end-freq widgets are hidden
  FreqEnd_Label->hide();
  FreqEnd_Spinbox->hide();
  FreqEnd_Scale_Combo->hide();


  // Connect signals from the input matching network setup widget
  connect(InputMatchingSetupWidget, SIGNAL(parametersChanged()), this, SLOT(parametersChanged()));
  connect(InputMatchingSetupWidget, SIGNAL(parametersChanged()), this, SLOT(UpdateDesignParameters()));

 // connect(TwoPortCheckBox, SIGNAL(stateChanged(int)), this, SLOT(One_TwoPort_Matching_Checkbox(int)));
  connect(ZoutRSpinBox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(ZoutISpinBox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqStart_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqStart_Scale_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Scale_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));

  this->setLayout(MatchingNetworkDesignLayout);
}

MatchingNetworkDesignTool::~MatchingNetworkDesignTool() {
  delete Broadband_Checkbox;
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

  // Get the input matching network setup parameters from the widget
  MatchingNetworkDesignParameters Specs = InputMatchingSetupWidget->getDesignParameters();


  // Get the load impedance data
  Specs.Zout = std::complex<double>(ZoutRSpinBox->value(), ZoutISpinBox->value());


  Specs.freqStart = FreqStart_Spinbox->value() *
                    getScaleFreq(FreqStart_Scale_Combo->currentIndex());
  Specs.freqEnd = FreqEnd_Spinbox->value() *
                  getScaleFreq(FreqEnd_Scale_Combo->currentIndex());


  switch (Specs.Topology){
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
