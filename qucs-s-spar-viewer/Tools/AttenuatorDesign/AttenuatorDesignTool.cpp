/// @file AttenuatorDesignTool.cpp
/// @brief GUI for the attenuator design tool (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "AttenuatorDesignTool.h"

AttenuatorDesignTool::AttenuatorDesignTool(QWidget *parent) : QWidget(parent) {
  QGridLayout *AttenuatorDesignLayout = new QGridLayout();

  int layout_row =
      0; // Row index. This is useful to add a new line on the layout without
         // the need of modifying manually all the widgets.

  // Topology
  Topology_Label = new QLabel("Topology");
  Topology_Combo = new QComboBox();
  Topology_Combo->addItem("Pi");
  Topology_Combo->addItem("Tee");
  Topology_Combo->addItem("Bridged Tee");
  Topology_Combo->addItem("Reflection Attenuator");
  Topology_Combo->addItem("Quarter-wave series");
  Topology_Combo->addItem("Quarter-wave shunt");
  Topology_Combo->addItem("L-pad 1st series");
  Topology_Combo->addItem("L-pad 1st shunt");
  Topology_Combo->addItem("Rseries");
  Topology_Combo->addItem("Rshunt");
  AttenuatorDesignLayout->addWidget(Topology_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(Topology_Combo, layout_row, 1);

  // Transmission line implementation
  layout_row++;
  TL_Implementation_Label = new QLabel(QString("TLIN implementation"));
  TL_Implementation_Combo = new QComboBox();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Stripline");
  TL_Implementation_Combo->addItem("Lumped");
  AttenuatorDesignLayout->addWidget(TL_Implementation_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(TL_Implementation_Combo, layout_row, 1);

  // Attenuation
  layout_row++;
  Attenuation_Label = new QLabel("Attenuation");
  AttenuationSpinBox = new QDoubleSpinBox();
  AttenuationSpinBox->setMinimum(0.5);
  AttenuationSpinBox->setMaximum(60);
  AttenuationSpinBox->setSingleStep(0.5);
  AttenuationSpinBox->setValue(15);
  dBLabelAtt = new QLabel("dB");
  AttenuatorDesignLayout->addWidget(Attenuation_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(AttenuationSpinBox, layout_row, 1);
  AttenuatorDesignLayout->addWidget(dBLabelAtt, layout_row, 2);

  // Input impedance
  layout_row++;
  Zin_Label = new QLabel("Zin");
  ZinSpinBox = new QDoubleSpinBox();
  ZinSpinBox->setMinimum(0.5);
  ZinSpinBox->setMaximum(1000);
  ZinSpinBox->setSingleStep(0.5);
  ZinSpinBox->setValue(50);
  Ohm_Zin_Label = new QLabel(QChar(0xa9, 0x03));
  AttenuatorDesignLayout->addWidget(Zin_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(ZinSpinBox, layout_row, 1);
  AttenuatorDesignLayout->addWidget(Ohm_Zin_Label, layout_row, 2);

  // Output impedance
  layout_row++;
  Zout_Label = new QLabel("Zout");
  ZoutSpinBox = new QDoubleSpinBox();
  ZoutSpinBox->setMinimum(0.5);
  ZoutSpinBox->setMaximum(1000);
  ZoutSpinBox->setSingleStep(0.5);
  ZoutSpinBox->setValue(50);
  Ohm_Zout_Label = new QLabel(QChar(0xa9, 0x03));
  AttenuatorDesignLayout->addWidget(Zout_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(ZoutSpinBox, layout_row, 1);
  AttenuatorDesignLayout->addWidget(Ohm_Zout_Label, layout_row, 2);

  // Input power
  layout_row++;
  Pin_Label = new QLabel("Input power");
  Pin_SpinBox = new QDoubleSpinBox();
  Pin_SpinBox->setMinimum(-150);   // dBm
  Pin_SpinBox->setMaximum(200);    // dBm
  Pin_SpinBox->setSingleStep(0.1); // dB
  Pin_SpinBox->setValue(30);       // dBm
  Pin_units_Combo = new QComboBox();
  QStringList power_units;
  power_units
      << "mW" << "W" << "dBm"
      << QString("dB%1V [75%2]").arg(QChar(0xbc, 0x03), QChar(0xa9, 0x03))
      << QString("dB%1V [50%2]").arg(QChar(0xbc, 0x03), QChar(0xa9, 0x03))
      << QString("dBmV [75%1]").arg(QChar(0xa9, 0x03))
      << QString("dBmV [50%1]").arg(QChar(0xa9, 0x03));
  Pin_units_Combo->addItems(power_units);
  Pin_units_Combo->setCurrentIndex(2); // dBm
  AttenuatorDesignLayout->addWidget(Pin_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(Pin_SpinBox, layout_row, 1);
  AttenuatorDesignLayout->addWidget(Pin_units_Combo, layout_row, 2);

  // Frequency. Tuned attenuators only
  layout_row++;
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

  AttenuatorDesignLayout->addWidget(freqLabel, layout_row, 0);
  AttenuatorDesignLayout->addWidget(freqSpinBox, layout_row, 1);
  AttenuatorDesignLayout->addWidget(FreqScaleCombo, layout_row, 2);

  // Power dissipation
  layout_row++;
  Pdiss_R1_Label = new QLabel("Pdiss. R1");
  Pdiss_R1_Lineedit = new QLineEdit();
  Pdiss_R1_Lineedit->setReadOnly(true);
  R1_Pdiss_Units_Combo = new QComboBox();
  R1_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R1_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R1_Lineedit, layout_row, 1);
  AttenuatorDesignLayout->addWidget(R1_Pdiss_Units_Combo, layout_row, 2);

  layout_row++;
  Pdiss_R2_Label = new QLabel("Pdiss. R2");
  Pdiss_R2_Lineedit = new QLineEdit();
  Pdiss_R2_Lineedit->setReadOnly(true);
  R2_Pdiss_Units_Combo = new QComboBox();
  R2_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R2_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R2_Lineedit, layout_row, 1);
  AttenuatorDesignLayout->addWidget(R2_Pdiss_Units_Combo, layout_row, 2);

  layout_row++;
  Pdiss_R3_Label = new QLabel("Pdiss. R3");
  Pdiss_R3_Lineedit = new QLineEdit();
  Pdiss_R3_Lineedit->setReadOnly(true);
  R3_Pdiss_Units_Combo = new QComboBox();
  R3_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R3_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R3_Lineedit, layout_row, 1);
  AttenuatorDesignLayout->addWidget(R3_Pdiss_Units_Combo, layout_row, 2);

  layout_row++;
  Pdiss_R4_Label = new QLabel("Pdiss. R4");
  Pdiss_R4_Lineedit = new QLineEdit();
  Pdiss_R4_Lineedit->setReadOnly(true);
  R4_Pdiss_Units_Combo = new QComboBox();
  R4_Pdiss_Units_Combo->addItems(power_units);
  AttenuatorDesignLayout->addWidget(Pdiss_R4_Label, layout_row, 0);
  AttenuatorDesignLayout->addWidget(Pdiss_R4_Lineedit, layout_row, 1);
  AttenuatorDesignLayout->addWidget(R4_Pdiss_Units_Combo, layout_row, 2);

  // Widgets to add a trace to plot
  layout_row++;
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Attenuator1");
  AttenuatorDesignLayout->addWidget(traceNameLabel, layout_row, 0);
  AttenuatorDesignLayout->addWidget(traceNameLineEdit, layout_row, 1);

  connect(Topology_Combo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::on_TopoCombo_currentIndexChanged);
  connect(TL_Implementation_Combo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(AttenuationSpinBox, &QDoubleSpinBox::valueChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(ZinSpinBox, &QDoubleSpinBox::valueChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(ZoutSpinBox, &QDoubleSpinBox::valueChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(freqSpinBox, &QDoubleSpinBox::valueChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(Pin_SpinBox, &QDoubleSpinBox::valueChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(FreqScaleCombo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(Pin_units_Combo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::UpdateDesignParameters);
  connect(R1_Pdiss_Units_Combo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::UpdatePowerDissipationData);
  connect(R2_Pdiss_Units_Combo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::UpdatePowerDissipationData);
  connect(R3_Pdiss_Units_Combo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::UpdatePowerDissipationData);
  connect(R4_Pdiss_Units_Combo, &QComboBox::currentIndexChanged, this,
          &AttenuatorDesignTool::UpdatePowerDissipationData);

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
}

void AttenuatorDesignTool::UpdateDesignParameters() {
  Specs.Attenuation = AttenuationSpinBox->value();
  Specs.Frequency = getFreq();
  Specs.Zin = ZinSpinBox->value();
  Specs.Zout = ZoutSpinBox->value();
  Specs.Topology = Topology_Combo->currentText();
  Specs.Pin = getPowerW(Pin_SpinBox->value(), Pin_units_Combo->currentIndex());

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

  // EMIT SIGNAL TO SIMULATE
  QString TraceName = traceNameLineEdit->text();
  SchContent.Name = TraceName;
  SchContent.Type = QString(
      "Attenuator"); // Indicate the main tool the kind of circuit to adjust
                     // default traces (in case no traces were selected)
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

    // Hide TLIN implementation widgets
    TL_Implementation_Label->hide();
    TL_Implementation_Combo->hide();

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

    break;

  case 2: // Bridged Tee
    // Show Pdiss for R3
    Pdiss_R3_Label->show();
    Pdiss_R3_Lineedit->show();
    R3_Pdiss_Units_Combo->show();

    // Hide TLIN implementation widgets
    TL_Implementation_Label->hide();
    TL_Implementation_Combo->hide();

    // Show R4
    Pdiss_R4_Label->show();
    Pdiss_R4_Lineedit->show();
    R4_Pdiss_Units_Combo->show();

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

    // Hide TLIN implementation widgets
    TL_Implementation_Label->hide();
    TL_Implementation_Combo->hide();

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

    break;

  case 4: // Quarter-wave series
  case 5: // Quarter-wave shunt

    // Replace Zin by Z0
    Zin_Label->setText(QString("Z0"));

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

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
    break;

  case 6:
  case 7: // Lpads

    // Replace Zin by Z0
    Zin_Label->setText(QString("Z0"));

    // Hide TLIN implementation widgets
    TL_Implementation_Label->hide();
    TL_Implementation_Combo->hide();

    // Hide Pdiss for R3
    Pdiss_R3_Label->hide();
    Pdiss_R3_Lineedit->hide();
    R3_Pdiss_Units_Combo->hide();

    // Hide Pdiss for R4
    Pdiss_R4_Label->hide();
    Pdiss_R4_Lineedit->hide();
    R4_Pdiss_Units_Combo->hide();

    // Show Zout
    Zout_Label->show();
    ZoutSpinBox->show();
    Ohm_Zout_Label->show();

    // Hide frequency input
    freqLabel->hide();
    freqSpinBox->hide();
    FreqScaleCombo->hide();
    break;

  case 8: // Rseries
  case 9: // Rshunt

    // Replace Zin by Z0
    Zin_Label->setText(QString("Z0"));

    // Hide TLIN implementation widgets
    TL_Implementation_Label->hide();
    TL_Implementation_Combo->hide();

    // Hide Pdiss for R2
    Pdiss_R2_Label->hide();
    Pdiss_R2_Lineedit->hide();
    R2_Pdiss_Units_Combo->hide();

    // Hide Pdiss for R3
    Pdiss_R3_Label->hide();
    Pdiss_R3_Lineedit->hide();
    R3_Pdiss_Units_Combo->hide();

    // Hide Pdiss for R4
    Pdiss_R4_Label->hide();
    Pdiss_R4_Lineedit->hide();
    R4_Pdiss_Units_Combo->hide();

    // Show Zout
    Zout_Label->show();
    ZoutSpinBox->show();
    Ohm_Zout_Label->show();

    // Hide frequency input
    freqLabel->hide();
    freqSpinBox->hide();
    FreqScaleCombo->hide();
    break;

  default:
    break;
  }
  UpdateDesignParameters();
}
void AttenuatorDesignTool::synthesize() {
  int attenuator_type = Topology_Combo->currentIndex();

  switch (attenuator_type) {
  case PI_ATTENUATOR:
    PiAttenuator *PI_AT;
    PI_AT = new PiAttenuator(Specs);
    PI_AT->synthesize();
    SchContent = PI_AT->Schematic;
    Pdiss = PI_AT->getPowerDissipation();
    delete PI_AT;
    break;

  case TEE_ATTENUATOR:
    TeeAttenuator *TEE_AT;
    TEE_AT = new TeeAttenuator(Specs);
    TEE_AT->synthesize();
    SchContent = TEE_AT->Schematic;
    Pdiss = TEE_AT->getPowerDissipation();
    delete TEE_AT;
    break;

  case BRIDGED_TEE:
    BridgedTeeAttenuator *BT_AT;
    BT_AT = new BridgedTeeAttenuator(Specs);
    BT_AT->synthesize();
    SchContent = BT_AT->Schematic;
    Pdiss = BT_AT->getPowerDissipation();
    delete BT_AT;
    break;

  case REFLECTION_ATTENUATOR:
    ReflectionAttenuator *RFAT;
    RFAT = new ReflectionAttenuator(Specs);
    RFAT->synthesize();
    SchContent = RFAT->Schematic;
    Pdiss = RFAT->getPowerDissipation();
    break;

  case QW_SERIES:
    QW_SeriesAttenuator *QW_Series;
    QW_Series = new QW_SeriesAttenuator(Specs);
    QW_Series->synthesize();
    SchContent = QW_Series->Schematic;
    Pdiss = QW_Series->getPowerDissipation();
    delete QW_Series;
    break;

  case QW_SHUNT:
    QW_ShuntAttenuator *QW_Shunt;
    QW_Shunt = new QW_ShuntAttenuator(Specs);
    QW_Shunt->synthesize();
    SchContent = QW_Shunt->Schematic;
    Pdiss = QW_Shunt->getPowerDissipation();
    delete QW_Shunt;
    break;

  case LPAD_1ST_SERIES:
    LPadFirstSeries *LP_1st_Series;
    LP_1st_Series = new LPadFirstSeries(Specs);
    LP_1st_Series->synthesize();
    SchContent = LP_1st_Series->Schematic;
    Pdiss = LP_1st_Series->getPowerDissipation();
    delete LP_1st_Series;
    break;

  case LPAD_1ST_SHUNT:
    LPadFirstShunt *LP_1st_Shunt;
    LP_1st_Shunt = new LPadFirstShunt(Specs);
    LP_1st_Shunt->synthesize();
    SchContent = LP_1st_Shunt->Schematic;
    Pdiss = LP_1st_Shunt->getPowerDissipation();
    delete LP_1st_Shunt;
    break;

  case RSERIES:
    RSeriesAttenuator *RSeries;
    RSeries = new RSeriesAttenuator(Specs);
    RSeries->synthesize();
    SchContent = RSeries->Schematic;
    Pdiss = RSeries->getPowerDissipation();
    delete RSeries;
    break;

  case RSHUNT:
    RShuntAttenuator *RShunt;
    RShunt = new RShuntAttenuator(Specs);
    RShunt->synthesize();
    SchContent = RShunt->Schematic;
    Pdiss = RShunt->getPowerDissipation();
    delete RShunt;
    break;
  }

  // Once having the power dissipation data, update the corresponding widgets
  UpdatePowerDissipationData();
}

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
  return Pin;
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
  if (Pdiss.contains("R1")) {
    double powerW = Pdiss["R1"];
    int unitIndex = R1_Pdiss_Units_Combo->currentIndex();
    double convertedPower = ConvertPowerFromW(powerW, unitIndex);
    Pdiss_R1_Lineedit->setText(QString::number(convertedPower));
  } else {
    Pdiss_R1_Lineedit->clear();
  }

  // Update R2
  if (Pdiss.contains("R2")) {
    double powerW = Pdiss["R2"];
    int unitIndex = R2_Pdiss_Units_Combo->currentIndex();
    double convertedPower = ConvertPowerFromW(powerW, unitIndex);
    Pdiss_R2_Lineedit->setText(QString::number(convertedPower));
  } else {
    Pdiss_R2_Lineedit->clear();
  }

  // Update R3
  if (Pdiss.contains("R3")) {
    double powerW = Pdiss["R3"];
    int unitIndex = R3_Pdiss_Units_Combo->currentIndex();
    double convertedPower = ConvertPowerFromW(powerW, unitIndex);
    Pdiss_R3_Lineedit->setText(QString::number(convertedPower));
  } else {
    Pdiss_R3_Lineedit->clear();
  }

  // Update R4
  if (Pdiss.contains("R4")) {
    double powerW = Pdiss["R4"];
    int unitIndex = R4_Pdiss_Units_Combo->currentIndex();
    double convertedPower = ConvertPowerFromW(powerW, unitIndex);
    Pdiss_R4_Lineedit->setText(QString::number(convertedPower));
  } else {
    Pdiss_R4_Lineedit->clear();
  }
}
