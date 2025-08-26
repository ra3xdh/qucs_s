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

  LoadSpecWidget = new LoadSpecificationWidget();
  MatchingNetworkDesignLayout->addWidget(LoadSpecWidget, 2, 0, 1, 5);

  // Frequency range. Start
  FreqStart_Label = new QLabel("freq");
  FreqStart_Spinbox = new QDoubleSpinBox();
  FreqStart_Spinbox->setMinimum(1);
  FreqStart_Spinbox->setMaximum(1e6);
  FreqStart_Spinbox->setDecimals(0);
  FreqStart_Spinbox->setValue(1000);
  FreqStart_Spinbox->setSingleStep(1); // Step fixed to 1 Hz/kHz/MHz/GHz
  QStringList FreqScale;
  FreqScale << "GHz" << "MHz" << "kHz" << "Hz";
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

  // Widgets to add a trace to plot
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Match1");
  MatchingNetworkDesignLayout->addWidget(traceNameLabel, 5, 0);
  MatchingNetworkDesignLayout->addWidget(traceNameLineEdit, 5, 1);

  // Since it is more common to design narrowband matching networks than
  // broadband, the end-freq widgets are hidden
  FreqEnd_Label->hide();
  FreqEnd_Spinbox->hide();
  FreqEnd_Scale_Combo->hide();


  // Connect signals from the input matching network setup widget
  connect(InputMatchingSetupWidget, SIGNAL(parametersChanged()), this, SLOT(parametersChanged()));
  connect(InputMatchingSetupWidget, SIGNAL(parametersChanged()), this, SLOT(UpdateDesignParameters()));
  connect(FreqStart_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FreqStart_Scale_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Scale_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));

  connect(LoadSpecWidget, &LoadSpecificationWidget::impedanceChanged, this, &MatchingNetworkDesignTool::UpdateDesignParameters);
  connect(LoadSpecWidget, &LoadSpecificationWidget::reflectionCoefficientChanged, this, &MatchingNetworkDesignTool::UpdateDesignParameters);
  connect(LoadSpecWidget, &LoadSpecificationWidget::sParametersChanged, this, &MatchingNetworkDesignTool::UpdateDesignParameters);

  this->setLayout(MatchingNetworkDesignLayout);
}

MatchingNetworkDesignTool::~MatchingNetworkDesignTool() {
  delete Broadband_Checkbox;
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


  if (TwoPortCheckBox->isChecked()) {
    // Two-port mode - use S-parameters
    Specs.S11 = LoadSpecWidget->getS11();
    Specs.S12 = LoadSpecWidget->getS12();
    Specs.S21 = LoadSpecWidget->getS21();
    Specs.S22 = LoadSpecWidget->getS22();
    Specs.twoPortMode = true;
  } else {
    // One-port mode - use load impedance
    Specs.Zout = LoadSpecWidget->getLoadImpedance();
    Specs.twoPortMode = false;
  }


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
