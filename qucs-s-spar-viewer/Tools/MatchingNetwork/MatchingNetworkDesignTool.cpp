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

  // Output matching network setup
  OutputMatchingSetupWidget = new MatchingNetworkParametersWidget();
  MatchingNetworkDesignLayout->addWidget(OutputMatchingSetupWidget, 3, 0, 1, 3);
  OutputMatchingSetupWidget->hide();// By default, 1-port matching is selected, so hide this widget

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
  connect(InputMatchingSetupWidget, SIGNAL(parametersChanged()), this, SLOT(UpdateDesignParameters()));
  connect(OutputMatchingSetupWidget, SIGNAL(parametersChanged()), this, SLOT(UpdateDesignParameters()));
  connect(TwoPortCheckBox, SIGNAL(stateChanged(int)), this, SLOT(AdjustOneTwoPortMatchingWidgetsVisibility()));
  connect(f_match_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(f_match_Scale_Combo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));

  connect(LoadSpecWidget, &LoadSpecificationWidget::impedanceChanged, this, &MatchingNetworkDesignTool::UpdateDesignParameters);
  connect(LoadSpecWidget, &LoadSpecificationWidget::reflectionCoefficientChanged, this, &MatchingNetworkDesignTool::UpdateDesignParameters);
  connect(LoadSpecWidget, &LoadSpecificationWidget::sParametersChanged, this, &MatchingNetworkDesignTool::UpdateDesignParameters);

  this->setLayout(MatchingNetworkDesignLayout);
}

MatchingNetworkDesignTool::~MatchingNetworkDesignTool() {
  delete f_match_Label;
  delete f_match_Scale_Combo;
  delete f_match_Spinbox;
}

void MatchingNetworkDesignTool::AdjustOneTwoPortMatchingWidgetsVisibility(){
  if (TwoPortCheckBox->isChecked()) {
    // Two-ports matching
    OutputMatchingSetupWidget->show();
  } else {
    // One-port matching
    OutputMatchingSetupWidget->hide();
  }
  // Once visibility was adjusted, update the specifications and synthesize a network
  UpdateDesignParameters();
}

void MatchingNetworkDesignTool::UpdateDesignParameters() {

  MatchingData Specs;
  // Get the input matching network setup parameters from the widget
  MatchingNetworkDesignParameters InputSpecs = InputMatchingSetupWidget->getDesignParameters();

  if (TwoPortCheckBox->isChecked()) {
    // 2-port mode - Get the S-parameters
    /*Specs.S11 = LoadSpecWidget->getS11();
    Specs.S12 = LoadSpecWidget->getS12();
    Specs.S21 = LoadSpecWidget->getS21();
    Specs.S22 = LoadSpecWidget->getS22();
    Specs.twoPortMode = true;
*/
    MatchingNetworkDesignParameters OutputSpecs = OutputMatchingSetupWidget->getDesignParameters();
    Specs.OutputNetworkParameters = OutputSpecs;

  } else {
    // 1-port mode - Get the load impedance
    InputSpecs.ZL = LoadSpecWidget->getLoadImpedance();
    Specs.twoPortMode = false;
    Specs.InputNetworkParameters = InputSpecs;
  }

  Specs.f_match = f_match_Spinbox->value() *
                    getScaleFreq(f_match_Scale_Combo->currentIndex());


  MatchingNetworkDesigner *MatchDesigner = new MatchingNetworkDesigner(Specs);
  MatchDesigner->synthesize();
  SchContent = MatchDesigner->Schematic;

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
