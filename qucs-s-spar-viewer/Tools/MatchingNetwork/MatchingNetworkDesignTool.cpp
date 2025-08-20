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
  // Broadband checkbox
  Broadband_Checkbox = new QCheckBox("Broadband matching");
  MatchingNetworkDesignLayout->addWidget(Broadband_Checkbox, 0, 0);

  // Topology
  Topology_Label = new QLabel("Topology");
  Topology_Combo = new QComboBox();
  QStringList matching_methods;
  matching_methods.append(tr("L-section"));
 /* matching_methods.append(tr("Single stub"));
  matching_methods.append(tr("Double stub"));
  matching_methods.append(QString("%1 %2/4")
                              .arg(tr("Multistage "))
                              .arg(QString(QChar(0xBB, 0x03))));
  matching_methods.append(tr("Cascaded L-sections"));
  matching_methods.append(QString("%1/4 line").arg(QChar(0xBB, 0x03)));
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

  // Input impedance
  Zin_Label = new QLabel("Zin");
  ZinRSpinBox = new QDoubleSpinBox();
  ZinRSpinBox->setMinimum(0.5);
  ZinRSpinBox->setMaximum(10000);
  ZinRSpinBox->setSingleStep(0.5);
  ZinRSpinBox->setValue(50);
  Zin_J = new QLabel("+j");
  ZinISpinBox = new QDoubleSpinBox();
  ZinISpinBox->setMinimum(-10000);
  ZinISpinBox->setMaximum(10.00);
  ZinISpinBox->setSingleStep(0.5);
  ZinISpinBox->setValue(0);
  Ohm_Zin_Label = new QLabel(QChar(0xa9, 0x03));
  MatchingNetworkDesignLayout->addWidget(Zin_Label, 2, 0);
  MatchingNetworkDesignLayout->addWidget(ZinRSpinBox, 2, 1);
  MatchingNetworkDesignLayout->addWidget(Zin_J, 2, 2);
  MatchingNetworkDesignLayout->addWidget(ZinISpinBox, 2, 3);
  MatchingNetworkDesignLayout->addWidget(Ohm_Zin_Label, 2, 4);

  // Output impedance
  Zout_Label = new QLabel("Zout");
  ZoutRSpinBox = new QDoubleSpinBox();
  ZoutRSpinBox->setMinimum(0.5);
  ZoutRSpinBox->setMaximum(10000);
  ZoutRSpinBox->setSingleStep(0.5);
  ZoutRSpinBox->setValue(75);
  Zout_J = new QLabel("+j");
  ZoutISpinBox = new QDoubleSpinBox();
  ZoutISpinBox->setMinimum(-10000);
  ZoutISpinBox->setMaximum(10000);
  ZoutISpinBox->setSingleStep(0.5);
  ZoutISpinBox->setValue(0);
  Ohm_Zout_Label = new QLabel(QChar(0xa9, 0x03));
  MatchingNetworkDesignLayout->addWidget(Zout_Label, 3, 0);
  MatchingNetworkDesignLayout->addWidget(ZoutRSpinBox, 3, 1);
  MatchingNetworkDesignLayout->addWidget(Zout_J, 3, 2);
  MatchingNetworkDesignLayout->addWidget(ZoutISpinBox, 3, 3);
  MatchingNetworkDesignLayout->addWidget(Ohm_Zout_Label, 3, 4);

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
  MatchingNetworkDesignLayout->addWidget(FreqStart_Label, 4, 0);
  MatchingNetworkDesignLayout->addWidget(FreqStart_Spinbox, 4, 1);
  MatchingNetworkDesignLayout->addWidget(FreqStart_Scale_Combo, 4, 2);

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
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Label, 5, 0);
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Spinbox, 5, 1);
  MatchingNetworkDesignLayout->addWidget(FreqEnd_Scale_Combo, 5, 2);

  // Widgets to add a trace to plot
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit(" Match1");
  MatchingNetworkDesignLayout->addWidget(traceNameLabel, 6, 0);
  MatchingNetworkDesignLayout->addWidget(traceNameLineEdit, 6, 1);

  // Since it is more common to design narrowband matching networks than
  // broadband, the end-freq widgets are hidden
  FreqEnd_Label->hide();
  FreqEnd_Spinbox->hide();
  FreqEnd_Scale_Combo->hide();

  connect(Topology_Combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_TopoCombo_currentIndexChanged(int)));
  connect(ZinRSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(ZinISpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(ZoutRSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(ZoutISpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(FreqStart_Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(FreqStart_Scale_Combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(FreqEnd_Scale_Combo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(Solution1_RB, SIGNAL(clicked(bool)), this,
          SLOT(UpdateDesignParameters()));
  connect(Solution2_RB, SIGNAL(clicked(bool)), this,
          SLOT(UpdateDesignParameters()));

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
  delete Zin_J;
  delete ZinISpinBox;
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
  Specs.Zin = std::complex<double>(ZinRSpinBox->value(), ZinISpinBox->value());
  Specs.Zout =
      std::complex<double>(ZoutRSpinBox->value(), ZoutISpinBox->value());
  Specs.Topology = Topology_Combo->currentText();
  if (Solution1_RB->isChecked())
    Specs.Solution = 1;
  else
    Specs.Solution = 2;

  Specs.freqStart = FreqStart_Spinbox->value() *
                    getScaleFreq(FreqStart_Scale_Combo->currentIndex());
  Specs.freqEnd = FreqEnd_Spinbox->value() *
                  getScaleFreq(FreqEnd_Scale_Combo->currentIndex());

  if (Specs.Topology == "L-section") {
    Lsection *L = new Lsection(Specs);
    L->synthesize();
    SchContent = L->Schematic;
    delete L;
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
    Solution1_RB->show();
    Solution2_RB->show();
    ZinISpinBox->setEnabled(false); // The L-section method does not support
                                    // complex source impedance data
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
