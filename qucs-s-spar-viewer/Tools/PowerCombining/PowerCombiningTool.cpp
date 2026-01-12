/// @file PowerCombiningTool.cpp
/// @brief Widget for power combining network design and synthesis
/// (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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
  //  TopoCombo->addItem("Travelling Wave");
  //  TopoCombo->addItem("Tree");
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
  connect(TL_Implementation_Combo, &QComboBox::currentIndexChanged, this,
          [this](int) { UpdateDesignParameters(); });

  connect(BranchesCombo, &QComboBox::currentIndexChanged, this,
          [this](int) { UpdateDesignParameters(); });

  connect(RefImpSpinbox, &QDoubleSpinBox::valueChanged, this,
          [this](double) { UpdateDesignParameters(); });

  connect(FreqSpinbox, &QDoubleSpinBox::valueChanged, this,
          [this](double) { UpdateDesignParameters(); });

  connect(FreqScaleCombo, &QComboBox::currentIndexChanged, this,
          [this](int) { UpdateDesignParameters(); });

  connect(K1Spinbox, &QDoubleSpinBox::valueChanged, this,
          [this](double) { UpdateDesignParameters(); });

  connect(K2Spinbox, &QDoubleSpinBox::valueChanged, this,
          [this](double) { UpdateDesignParameters(); });

  connect(NStagesSpinbox, &QSpinBox::valueChanged, this,
          [this](int) { UpdateDesignParameters(); });

  connect(AlphaSpinbox, &QDoubleSpinBox::valueChanged, this,
          [this](double) { UpdateDesignParameters(); });

  connect(UnitsCombo, &QComboBox::currentIndexChanged, this,
          [this](int) { UpdateDesignParameters(); });

  connect(TopoCombo, &QComboBox::currentIndexChanged, this,
          &PowerCombiningTool::on_TopoCombo_currentIndexChanged);

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

  Specs.Type = TopoCombo->currentText();
  Specs.Noutputs = BranchesCombo->currentText().toInt();

  Specs.OutputRatio.clear();
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

  synthesize();
}

void PowerCombiningTool::synthesize() {
  // Recalculate network

  int topology = TopoCombo->currentIndex();

  switch (topology) {

  case WILKINSON:
    Wilkinson2Way *WK;
    WK = new Wilkinson2Way(Specs);
    WK->synthesize();
    SchContent = WK->Schematic;
    delete WK;
    break;

  case MULTISTAGE_WILKINSON:
    MultistageWilkinson *MSWK;
    MSWK = new MultistageWilkinson(Specs);
    MSWK->synthesize();
    SchContent = MSWK->Schematic;
    delete MSWK;
    break;

  case T_JUNCTION:
    TJunction *TJ;
    TJ = new TJunction(Specs);
    TJ->synthesize();
    SchContent = TJ->Schematic;
    delete TJ;
    break;

  case BRANCHLINE:
    Branchline *BR;
    BR = new Branchline(Specs);
    BR->synthesize();
    SchContent = BR->Schematic;
    delete BR;
    break;

  case DOUBLE_BOX_BRANCHLINE:
    DoubleBoxBranchline *DBBR;
    DBBR = new DoubleBoxBranchline(Specs);
    DBBR->synthesize();
    SchContent = DBBR->Schematic;
    delete DBBR;
    break;

  case BAGLEY:
    Bagley *BG;
    BG = new Bagley(Specs);
    BG->synthesize();
    SchContent = BG->Schematic;
    delete BG;
    break;

  case GYSEL:
    Gysel *GS;
    GS = new Gysel(Specs);
    GS->synthesize();
    SchContent = GS->Schematic;
    break;

  case LIM_EOM:
    Lim_Eom *LE;
    LE = new Lim_Eom(Specs);
    LE->synthesize();
    SchContent = LE->Schematic;
    break;

  case WILKINSON_3_WAY_IMPROVED_ISO:
    Wilkinson3Way_ImprovedIsolation *W3WII;
    W3WII = new Wilkinson3Way_ImprovedIsolation(Specs);
    W3WII->synthesize();
    SchContent = W3WII->Schematic;
    break;

  case RECOMBINANT_3_WAY_WILKINSON:
    Recombinant3WayWilkinson *RWK;
    RWK = new Recombinant3WayWilkinson(Specs);
    RWK->synthesize();
    SchContent = RWK->Schematic;
    break;
  }

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

  switch (index) {
  case WILKINSON:
    setSettings_Wilkinson();
    break;
  case MULTISTAGE_WILKINSON:
    setSettings_MultistageWilkinson();
    break;
  case T_JUNCTION:
    setSettings_T_Junction();
    break;
  case BRANCHLINE:
    setSettings_Branchline();
    break;
  case DOUBLE_BOX_BRANCHLINE:
    setSettings_DoubleBoxBranchline();
    break;
  case BAGLEY:
    setSettings_Bagley();
    break;
  case GYSEL:
    setSettings_Gysel();
    break;
  case LIM_EOM:
    setSettings_LimEom();
    break;
  case WILKINSON_3_WAY_IMPROVED_ISO:
    setSettings_Wilkinson_3_Way_Improved_Isolation();
    break;
  case RECOMBINANT_3_WAY_WILKINSON:
    setSettings_Recombinant_3_Way_Wilkinson();
    break;
  case TRAVELLING_WAVE:
    setSettings_Travelling_Wave();
    break;
  case TREE:
    setSettings_Tree();
    break;
  }

  UpdateDesignParameters();
}

void PowerCombiningTool::setSettings_Wilkinson() {

  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  // Enable power split ration
  K1Spinbox->setVisible(true);
  K1Label->setVisible(true);
  K1LabeldB->setVisible(true);

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  TL_Implementation_Combo->addItem("Lumped");

  // Set 2 branches only
  BranchesCombo->clear();
  BranchesCombo->addItem("2");
  BranchesCombo->hide();

  // Hide number of outputs
  number_Output_Label->hide();

  // Hide number of stages
  NStagesLabel->setVisible(false);
  NStagesSpinbox->setVisible(false);

  // Unblock signals after adjusting parameters
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_MultistageWilkinson() {
  // Block signals before adjusting parameters
  NStagesSpinbox->blockSignals(true);
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  TL_Implementation_Combo->addItem("Lumped");

  // Set number of stages and enable visibility
  NStagesSpinbox->setMinimum(2);
  NStagesSpinbox->setMaximum(7);
  NStagesSpinbox->setValue(2);
  NStagesLabel->setVisible(true);
  NStagesSpinbox->setVisible(true);

  // Set the number of output branches to 2 and disable visibility
  BranchesCombo->clear();
  BranchesCombo->addItem("2"); // 2 outputs only
  BranchesCombo->hide();
  number_Output_Label->hide();

  // Hide power split ratio
  K1Spinbox->setVisible(false);
  K1Label->setVisible(false);
  K1LabeldB->setVisible(false);

  // Unblock signals after adjusting parameters
  NStagesSpinbox->blockSignals(false);
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_T_Junction() {

  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");

  // Show power split ratio
  K1Spinbox->setVisible(true);
  K1Label->setVisible(true);
  K1LabeldB->setVisible(true);

  // Force the output branches to 2
  BranchesCombo->clear();
  BranchesCombo->addItem("2");
  BranchesCombo->hide();
  number_Output_Label->hide();

  // Unblock signals after adjusting parameters
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}
void PowerCombiningTool::setSettings_Branchline() {

  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");

  // Enable power split ratio
  K1Spinbox->setVisible(true);
  K1Label->setVisible(true);
  K1LabeldB->setVisible(true);

  // Set the number of branches to 2
  BranchesCombo->clear();
  BranchesCombo->addItem("2");
  BranchesCombo->hide();
  number_Output_Label->hide();

  // Unblock signals after adjusting parameters Unblock signals after adjusting
  // parameters
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}
void PowerCombiningTool::setSettings_DoubleBoxBranchline() {

  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");

  // Set the number of branches to 2
  BranchesCombo->clear();
  BranchesCombo->addItem("2");
  BranchesCombo->hide();
  number_Output_Label->hide();

  // Hide power split ratio
  K1Spinbox->setVisible(false);
  K1Label->setVisible(false);
  K1LabeldB->setVisible(false);

  // Unblock signals after adjusting parameters Unblock signals after adjusting
  // parameters
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_Bagley() {
  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Hide stages control
  NStagesLabel->setVisible(false);
  NStagesSpinbox->setVisible(false);

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");

  // Set possible number of branches
  BranchesCombo->clear();
  BranchesCombo->addItem("3");
  BranchesCombo->addItem("5");
  BranchesCombo->addItem("7");
  BranchesCombo->show();
  number_Output_Label->show();
  BranchesCombo->setValidator(Bagley_Validator);

  // Hide power split ratio
  K1Spinbox->setVisible(false);
  K1Label->setVisible(false);
  K1LabeldB->setVisible(false);

  // Unblock signals after adjusting parameters
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_Gysel() {

  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Hide stages control
  NStagesLabel->setVisible(false);
  NStagesSpinbox->setVisible(false);

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");

  // Set the number of output branches to 2
  BranchesCombo->clear();
  BranchesCombo->addItem("2");
  BranchesCombo->hide();
  number_Output_Label->hide();

  // Hide power split ratio
  K1Spinbox->setVisible(false);
  K1Label->setVisible(false);
  K1LabeldB->setVisible(false);

  // Unblock signals after adjusting parameters Unblock signals after adjusting
  // parameters
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_LimEom() {

  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);
  TL_Implementation_Combo->blockSignals(true);

  K1Label->setVisible(true);
  K1LabeldB->setVisible(true);
  K1Spinbox->setVisible(true);
  K2Spinbox->setVisible(true);
  K3Spinbox->setVisible(true);

  // Hide number of outputs label
  number_Output_Label->hide();

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Hide stages control
  NStagesLabel->setVisible(false);
  NStagesSpinbox->setVisible(false);

  // Adjust available transmission line implementations
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");

  // Unblock signals after adjusting parameters Unblock signals after adjusting
  // parameters
  BranchesCombo->blockSignals(false);
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_Wilkinson_3_Way_Improved_Isolation() {
  setDefaultSettings();

  // Adjust available transmission line implementations
  TL_Implementation_Combo->blockSignals(true);
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_Recombinant_3_Way_Wilkinson() {
  setDefaultSettings();

  // Adjust available transmission line implementations
  TL_Implementation_Combo->blockSignals(true);
  TL_Implementation_Combo->clear();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Lumped");
  TL_Implementation_Combo->blockSignals(false);
}

void PowerCombiningTool::setSettings_Travelling_Wave() {
  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Adjust possible number of branches
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

void PowerCombiningTool::setSettings_Tree() {
  // Block signals before adjusting parameters
  BranchesCombo->blockSignals(true);

  // Show TLIN implementation widgets
  TL_Implementation_Label->show();
  TL_Implementation_Combo->show();

  // Adjust possible number of branches
  BranchesCombo->clear();
  BranchesCombo->addItem("4");
  BranchesCombo->addItem("8");
  BranchesCombo->addItem("16");
  BranchesCombo->show();
  number_Output_Label->show();

  // Unblock signals after adjusting parameters
  BranchesCombo->blockSignals(false);
}

void PowerCombiningTool::setDefaultSettings() {
  // Hide power ratio controls
  K1Spinbox->setVisible(false);
  K1Label->setVisible(false);
  K1LabeldB->setVisible(false);
  K2Spinbox->setVisible(false);
  K3Spinbox->setVisible(false);

  // Hide stages control
  NStagesLabel->setVisible(false);
  NStagesSpinbox->setVisible(false);

  // Reset branches combo
  BranchesCombo->blockSignals(true);
  BranchesCombo->clear();
  BranchesCombo->hide();
  number_Output_Label->hide();
  BranchesCombo->setValidator(NULL);
  BranchesCombo->blockSignals(false);
}
