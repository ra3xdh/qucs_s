/***************************************************************************
                                FilterDesignTool.cpp
                                ----------

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
#include "FilterDesignTool.h"

FilterDesignTool::FilterDesignTool(QWidget *parent): QWidget(parent) {

  //****************************** BUILD THE UI **************************
  QGridLayout *FilterDesignLayout = new QGridLayout();

         //********** Filter Implementation ***********
  FilterImplementationCombo = new QComboBox();
  FilterImplementationCombo->addItem("LC Ladder");
  FilterImplementationCombo->addItem("LC Direct Coupled");
  FilterImplementationCombo->addItem("Quarter-wavelength");
  FilterImplementationCombo->addItem("Stepped impedance");
  FilterImplementationCombo->addItem("End-coupled");
  FilterImplementationCombo->addItem("Capacitively-coupled shunt resonators");
  FilterImplementationCombo->addItem("Semilumped Elliptic");
  FilterImplementationCombo->addItem("Semilumped Canonical");
  FilterImplementationCombo->addItem("Coupled line bandpass");
  FilterImplementationCombo->addItem(
      "Coupled line SIR with harmonic rejection");
  FilterDesignLayout->addWidget(new QLabel("Implementation"), 0, 0);
  FilterDesignLayout->addWidget(FilterImplementationCombo, 0, 1);
  //******** Tee or Pi (LC ladder only) ********
  QWidget *TeePiWidget = new QWidget();
  QHBoxLayout *TeePiLayout = new QHBoxLayout();
  CLCRadioButton = new QRadioButton("CLC");
  LCLRadioButton = new QRadioButton("LCL");
  CLCRadioButton->setChecked(true);
  TeePiLayout->addWidget(CLCRadioButton);
  TeePiLayout->addWidget(LCLRadioButton);
  TeePiWidget->setLayout(TeePiLayout);
  FilterDesignLayout->addWidget(TeePiWidget, 0, 2);
  //************ Response type **************
  FilterResponseTypeCombo = new QComboBox();
  DefaultFilterResponses.append("Chebyshev");
  DefaultFilterResponses.append("Butterworth");
  DefaultFilterResponses.append("Elliptic");
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterDesignLayout->addWidget(new QLabel("Response"), 1, 0);
  FilterDesignLayout->addWidget(FilterResponseTypeCombo, 1, 1);
  //********** Direct coupled filters - Coupling type *****
  DC_CouplingTypeCombo = new QComboBox();
  DC_CouplingTypeCombo->addItem("Capacitative coupled shunt resonators");
  DC_CouplingTypeCombo->addItem("Inductive coupled series resonators");
  DC_CouplingLabel = new QLabel("Coupling");
  FilterDesignLayout->addWidget(DC_CouplingLabel, 2, 0);
  FilterDesignLayout->addWidget(DC_CouplingTypeCombo, 2, 1);
  DC_CouplingTypeCombo->hide();
  DC_CouplingLabel->hide();
  //************ Filter class ****************
  FilterClassCombo = new QComboBox();
  FilterClassCombo->addItem("Lowpass");
  FilterClassCombo->addItem("Highpass");
  FilterClassCombo->addItem("Bandpass");
  FilterClassCombo->addItem("Bandstop");
  FilterDesignLayout->addWidget(new QLabel("Class"), 3, 0);
  FilterDesignLayout->addWidget(FilterClassCombo, 3, 1);
  //*************** Order *******************
  OrderSpinBox = new QSpinBox();
  OrderSpinBox->setValue(2);
  OrderSpinBox->setMinimum(1);
  OrderCombobox = new QComboBox(); // Combobox option for Zverev mode. This is
                                   // filled up in the corresponding slot
  OrderCombobox->hide();
  FilterDesignLayout->addWidget(new QLabel("Order"), 4, 0);
  FilterDesignLayout->addWidget(OrderSpinBox, 4, 1);
  FilterDesignLayout->addWidget(OrderCombobox, 4, 1);
  //******* Cutoff freq (Lowpass and Highpass) *********
  //****** Central freq (Bandpass and Bandstop) *********
  FCSpinbox = new QDoubleSpinBox();
  FCSpinbox->setMinimum(1);
  FCSpinbox->setMaximum(1e6);
  FCSpinbox->setDecimals(0);
  FCSpinbox->setValue(1000);
  FCSpinbox->setSingleStep(1); // Step fixed to 1 Hz/kHz/MHz/GHz
  FC_ScaleCombobox = new QComboBox();
  FC_ScaleCombobox->addItem("GHz");
  FC_ScaleCombobox->addItem("MHz");
  FC_ScaleCombobox->addItem("kHz");
  FC_ScaleCombobox->addItem("Hz");
  FC_ScaleCombobox->setCurrentIndex(1);
  FilterDesignLayout->addWidget(new QLabel("Cutoff freq"), 5, 0);
  FilterDesignLayout->addWidget(FCSpinbox, 5, 1);
  FilterDesignLayout->addWidget(FC_ScaleCombobox, 5, 2);
  //************* Bandwidth ***********
  BWSpinbox = new QDoubleSpinBox();
  BWSpinbox->setMinimum(1);
  BWSpinbox->setMaximum(1e6);
  BWSpinbox->setDecimals(0);
  BWSpinbox->setValue(1000);
  BWSpinbox->setSingleStep(1); // Step fixed to 1 Hz/kHz/MHz/GHz
  BW_ScaleCombobox = new QComboBox();
  BW_ScaleCombobox->addItem("GHz");
  BW_ScaleCombobox->addItem("MHz");
  BW_ScaleCombobox->addItem("kHz");
  BW_ScaleCombobox->addItem("Hz");
  BW_ScaleCombobox->setCurrentIndex(1);
  FilterDesignLayout->addWidget(new QLabel("Bandwidth"), 6, 0);
  FilterDesignLayout->addWidget(BWSpinbox, 6, 1);
  FilterDesignLayout->addWidget(BW_ScaleCombobox, 6, 2);
  //************** Ripple ****************
  RippleSpinbox = new QDoubleSpinBox();
  RippleSpinbox->setMinimum(0.01);
  RippleSpinbox->setMaximum(2);
  RippleSpinbox->setValue(0.01);
  RippleSpinbox->setSingleStep(0.01); // Step fixed to 0.01dB
  RippleCombobox = new QComboBox();
  RippleCombobox->hide();
  RippleLabel = new QLabel("Ripple");
  RippledBLabel = new QLabel("dB");
  FilterDesignLayout->addWidget(RippleLabel, 7, 0);
  FilterDesignLayout->addWidget(RippleSpinbox, 7, 1);
  FilterDesignLayout->addWidget(RippleCombobox, 7, 1);
  FilterDesignLayout->addWidget(RippledBLabel, 7, 2);
  //************** Stopband attenuation ****************
  StopbandAttSpinbox = new QDoubleSpinBox();
  StopbandAttSpinbox->setMinimum(5);
  StopbandAttSpinbox->setMaximum(150);
  StopbandAttSpinbox->setValue(30);
  StopbandAttSpinbox->setSingleStep(0.1); // Step fixed to 0.1dB
  StopbandAttLabel = new QLabel("Stopband att");
  StopbandAttdBLabel = new QLabel("dB");
  FilterDesignLayout->addWidget(StopbandAttLabel, 8, 0);
  FilterDesignLayout->addWidget(StopbandAttSpinbox, 8, 1);
  FilterDesignLayout->addWidget(StopbandAttdBLabel, 8, 2);
  StopbandAttSpinbox->setVisible(false);
  StopbandAttLabel->setVisible(false);
  StopbandAttdBLabel->setVisible(false);
  //************** Elliptic type ****************
  EllipticTypeLabel = new QLabel("Elliptic type");
  EllipticType = new QComboBox();
  EllipticType->addItem("Type A");
  EllipticType->addItem("Type B");
  EllipticType->addItem("Type C");
  EllipticType->addItem("Type S");
  EllipticTypeLabel->setVisible(false);
  EllipticType->setVisible(false);
  FilterDesignLayout->addWidget(EllipticTypeLabel, 9, 0);
  FilterDesignLayout->addWidget(EllipticType, 9, 1);
  //************* Load resistance (Zverev mode only) ***********
  RLCombobox = new QComboBox();
  RLlabel = new QLabel("Load");
  RLlabelOhm = new QLabel(QChar(0xa9, 0x03));
  RLCombobox->hide();
  RLlabel->hide();
  RLlabelOhm->hide();
  FilterDesignLayout->addWidget(RLlabel, 9, 0);
  FilterDesignLayout->addWidget(RLCombobox, 9, 1);
  FilterDesignLayout->addWidget(RLlabelOhm, 9, 2);
  //**************** Phase error (Zverev mode only) ***********
  PhaseErrorLabel = new QLabel("Phase error");
  PhaseErrorCombobox = new QComboBox();
  PhaseErrorLabelDeg = new QLabel("deg");
  PhaseErrorLabel->hide();
  PhaseErrorCombobox->hide();
  PhaseErrorLabelDeg->hide();
  FilterDesignLayout->addWidget(PhaseErrorLabel, 10, 0);
  FilterDesignLayout->addWidget(PhaseErrorCombobox, 10, 1);
  FilterDesignLayout->addWidget(PhaseErrorLabelDeg, 10, 2);
  //***************  Minimum impedance achievable in the manufacturing process
  //**************
  MinimumZLabel = new QLabel("Minimum Z");
  MinimumZ_Spinbox = new QDoubleSpinBox();
  MinimumZ_Spinbox->setMinimum(1);
  MinimumZ_Spinbox->setMaximum(1000);
  MinimumZ_Spinbox->setValue(10);
  MinimumZ_Spinbox->setSingleStep(10);
  MinimumZ_Unit_Label = new QLabel(QChar(0xa9, 0x03));
  MinimumZLabel->hide();
  MinimumZ_Spinbox->hide();
  MinimumZ_Unit_Label->hide();
  FilterDesignLayout->addWidget(MinimumZLabel, 11, 0);
  FilterDesignLayout->addWidget(MinimumZ_Spinbox, 11, 1);
  FilterDesignLayout->addWidget(MinimumZ_Unit_Label, 11, 2);
  //***************  Maximum impedance achievable in the manufacturing process
  //**************
  MaximumZLabel = new QLabel("Maximum Z");
  MaximumZ_Spinbox = new QDoubleSpinBox();
  MaximumZ_Spinbox->setMinimum(1);
  MaximumZ_Spinbox->setMaximum(1000);
  MaximumZ_Spinbox->setValue(400);
  MaximumZ_Spinbox->setSingleStep(10);
  MaximumZ_Unit_Label = new QLabel(QChar(0xa9, 0x03));
  MaximumZLabel->hide();
  MaximumZ_Spinbox->hide();
  MaximumZ_Unit_Label->hide();
  FilterDesignLayout->addWidget(MaximumZLabel, 12, 0);
  FilterDesignLayout->addWidget(MaximumZ_Spinbox, 12, 1);
  FilterDesignLayout->addWidget(MaximumZ_Unit_Label, 12, 2);
  //************************* Semilumped implementation settings
  //*********************
  SemiLumpedImplementationLabel = new QLabel("Semilumped settings");
  SemiLumpedImplementationCombo = new QComboBox();
  SemiLumpedImplementationCombo->addItem(
      "Replace inductors and shunt capacitors");
  SemiLumpedImplementationCombo->addItem("Replace only inductors");
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();
  FilterDesignLayout->addWidget(SemiLumpedImplementationLabel, 13, 0);
  FilterDesignLayout->addWidget(SemiLumpedImplementationCombo, 13, 1);

         // Coupled line SIR BPF type
  ImpedanceRatio_Label = new QLabel(QString("Impedance ratio, K"));
  ImpedanceRatio_Spinbox = new QDoubleSpinBox();
  ImpedanceRatio_Spinbox->setMinimum(0.1);
  ImpedanceRatio_Spinbox->setMaximum(10);
  ImpedanceRatio_Spinbox->setValue(0.5);
  ImpedanceRatio_Spinbox->setSingleStep(0.1);

  FilterDesignLayout->addWidget(ImpedanceRatio_Label, 14, 0);
  FilterDesignLayout->addWidget(ImpedanceRatio_Spinbox, 14, 1);

         //************ Source impedance **********
  SourceImpedanceLineEdit = new QLineEdit("50");
  FilterDesignLayout->addWidget(new QLabel("ZS"), 15, 0);
  FilterDesignLayout->addWidget(SourceImpedanceLineEdit, 15, 1);
  FilterDesignLayout->addWidget(new QLabel(QChar(0xa9, 0x03)), 15, 2);
  this->setLayout(FilterDesignLayout);

         // Connection functions for updating the network requirements and simulate on
         // the fly
  connect(FilterImplementationCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(ImplementationComboChanged(int)));
  connect(CLCRadioButton, SIGNAL(toggled(bool)), this,
          SLOT(ChangeRL_CLC_LCL_mode()));
  // connect(LCLRadioButton, SIGNAL(toggled(bool)), this,
  // SLOT(ChangeRL_CLC_LCL_mode()));
  connect(FilterResponseTypeCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(ResposeComboChanged()));
  connect(EllipticType, SIGNAL(currentIndexChanged(int)), this,
          SLOT(EllipticTypeChanged()));
  connect(FilterClassCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(OrderSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(FCSpinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(FC_ScaleCombobox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(BWSpinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(BW_ScaleCombobox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(SourceImpedanceLineEdit, SIGNAL(textChanged(QString)), this,
          SLOT(UpdateDesignParameters()));
  connect(RippleSpinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(StopbandAttSpinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(EllipticType, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(DC_CouplingTypeCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(RLCombobox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateRipple(int)));
  connect(MinimumZ_Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(MaximumZ_Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));
  connect(SemiLumpedImplementationCombo, SIGNAL(currentIndexChanged(int)), this,
          SLOT(UpdateDesignParameters()));
  connect(ImpedanceRatio_Spinbox, SIGNAL(valueChanged(double)), this,
          SLOT(UpdateDesignParameters()));

  ImplementationComboChanged(LC_LADDER);
}

FilterDesignTool::~FilterDesignTool() {
  delete FilterClassCombo;
  delete FilterResponseTypeCombo;
  delete FilterImplementationCombo;
  delete FC_ScaleCombobox;
  delete BW_ScaleCombobox;
  delete EllipticType;
  delete FCSpinbox;
  delete BWSpinbox;
  delete RippleSpinbox;
  delete StopbandAttSpinbox;
  delete OrderSpinBox;
  delete SourceImpedanceLineEdit;
  delete CLCRadioButton;
  delete LCLRadioButton;
  delete StopbandAttLabel;
  delete StopbandAttdBLabel;
  delete EllipticTypeLabel;
  delete MinimumZLabel;
  delete MinimumZ_Spinbox;
  delete MinimumZ_Unit_Label;
  delete MaximumZLabel;
  delete MaximumZ_Spinbox;
  delete MaximumZ_Unit_Label;
}

void FilterDesignTool::synthesize() {
  // Recalculate network

  CanonicalFilter *CF;
  CF = new CanonicalFilter(Filter_SP);
  CF->synthesize();
  SchContent = CF->Schematic;
  if (FilterClassCombo->currentText() ==
      "Bandstop") { // The bandstop configuration contains a shunt resonator
                    // which cannot be handled with the internal simulator
    SchContent.setDescription(QString("NOT LADDER"));
  } else {
    SchContent.setDescription(QString(""));
  }
  delete CF;

}

QString FilterDesignTool::getQucsNetlist() {
  return SchContent.getQucsNetlist();
}

SchematicContent FilterDesignTool::getSchematic() { return SchContent; }

void FilterDesignTool::ResposeComboChanged() {
  bool ActivateCauer =
      !FilterResponseTypeCombo->currentText().compare("Elliptic");
  StopbandAttSpinbox->setVisible(ActivateCauer);
  StopbandAttLabel->setVisible(ActivateCauer);
  StopbandAttdBLabel->setVisible(ActivateCauer);

  EllipticType->setVisible(ActivateCauer);
  EllipticTypeLabel->setVisible(ActivateCauer);
  OrderSpinBox->setMinimum(1);
  if (ActivateCauer) {
    CLCRadioButton->setText("Min L");
    LCLRadioButton->setText("Min C");
  } else {
    CLCRadioButton->setText("CLC");
    LCLRadioButton->setText("LCL");
  }

  UpdateDesignParameters();
}

// This function catches the events related to the changes in the filter
// specificatios
void FilterDesignTool::UpdateDesignParameters() {
  Filter_SP.Implementation = FilterImplementationCombo->currentText();
  //************************** Set filter response
  //*********************************
  if (!FilterResponseTypeCombo->currentText().compare("Chebyshev"))
    Filter_SP.FilterResponse = Chebyshev;
  if (!FilterResponseTypeCombo->currentText().compare("Butterworth"))
    Filter_SP.FilterResponse = Butterworth;
  if (!FilterResponseTypeCombo->currentText().compare("Legendre"))
    Filter_SP.FilterResponse = Legendre;
  if (!FilterResponseTypeCombo->currentText().compare("Elliptic"))
    Filter_SP.FilterResponse = Elliptic;
  if (!FilterResponseTypeCombo->currentText().compare("Bessel"))
    Filter_SP.FilterResponse = Bessel;
  if (!FilterResponseTypeCombo->currentText().compare("Gegenbauer"))
    Filter_SP.FilterResponse = Gegenbauer;
  if (!FilterResponseTypeCombo->currentText().compare("LinearPhase"))
    Filter_SP.FilterResponse = LinearPhaseEqError;

         //**************************** Set filter type
         //**************************************
  if (!FilterClassCombo->currentText().compare("Lowpass"))
    Filter_SP.FilterType = Lowpass;
  if (!FilterClassCombo->currentText().compare("Highpass"))
    Filter_SP.FilterType = Highpass;
  if (!FilterClassCombo->currentText().compare("Bandpass"))
    Filter_SP.FilterType = Bandpass;
  if (!FilterClassCombo->currentText().compare("Bandstop"))
    Filter_SP.FilterType = Bandstop;

         //**************************** Set coupling
         //********************************************
  if (!DC_CouplingTypeCombo->currentText().compare(
          "Capacitative coupled shunt resonators"))
    Filter_SP.DC_Coupling = CapacitativeCoupledShuntResonators;
  if (!DC_CouplingTypeCombo->currentText().compare(
          "Inductive coupled series resonators"))
    Filter_SP.DC_Coupling = InductiveCoupledSeriesResonators;

         // Update user input
  if ((!FilterClassCombo->currentText().compare("Lowpass")) ||
      (!FilterClassCombo->currentText().compare("Highpass"))) {
    BWSpinbox->setEnabled(false);
    BW_ScaleCombobox->setEnabled(false);
  } else {
    BWSpinbox->setEnabled(true);
    BW_ScaleCombobox->setEnabled(true);
    if (Filter_SP.bw >= Filter_SP.fc) {
      BWSpinbox->blockSignals(true);
      BWSpinbox->setValue(0.1 * FCSpinbox->value()); // 10% BW
      BW_ScaleCombobox->setCurrentIndex(FC_ScaleCombobox->currentIndex());
      BWSpinbox->blockSignals(false);
    }
  }

  if ((Filter_SP.FilterType == Lowpass) || (Filter_SP.FilterType == Highpass)) {
    SPAR_Settings.fstart = Filter_SP.fc * 0.5;
    SPAR_Settings.fstop = Filter_SP.fc * 1.5;
    SPAR_Settings.n_points = 200;
  } else {
    SPAR_Settings.fstart = (Filter_SP.fc - Filter_SP.bw / 2) * 0.5;
    SPAR_Settings.fstop = (Filter_SP.fc - Filter_SP.bw / 2) * 1.5;
    SPAR_Settings.n_points = 200;
  }

  if (Filter_SP.Implementation == "LC Direct Coupled") {
    DC_CouplingTypeCombo->show();
    DC_CouplingLabel->show();
    FilterResponseTypeCombo->blockSignals(true);
    CLCRadioButton->hide();
    LCLRadioButton->hide();
    QString CurrentResponse = FilterResponseTypeCombo->currentText();
    FilterResponseTypeCombo->clear();
    QStringList data = setItemsResponseTypeCombo();
    data.removeAt(data.indexOf("Elliptic"));
    data.removeAt(data.indexOf("Cauer"));
    FilterResponseTypeCombo->addItems(data);
    for (int i = 0; i < data.length(); i++) {
      if (CurrentResponse == data.at(i)) {
        FilterResponseTypeCombo->setCurrentIndex(i);
        break;
      }
    }
    FilterClassCombo->blockSignals(false);
    FilterResponseTypeCombo->blockSignals(false);
  } else {
    DC_CouplingTypeCombo->hide();
    DC_CouplingLabel->hide();
    CLCRadioButton->show();
    LCLRadioButton->show();
    FilterClassCombo->setEnabled(true);
    QString CurrentResponse = FilterResponseTypeCombo->currentText();
    FilterResponseTypeCombo->blockSignals(true);
    QStringList data = setItemsResponseTypeCombo();
    FilterResponseTypeCombo->clear();
    FilterResponseTypeCombo->addItems(data);
    for (int i = 0; i < data.length(); i++) {
      if (CurrentResponse == data.at(i)) {
        FilterResponseTypeCombo->setCurrentIndex(i);
        break;
      }
    }
    FilterResponseTypeCombo->blockSignals(false);
  }

         // Update parameters
  Filter_SP.bw = BWSpinbox->value() * getScale(BW_ScaleCombobox->currentText());
  Filter_SP.fc = FCSpinbox->value() * getScale(FC_ScaleCombobox->currentText());
  Filter_SP.isCLC = CLCRadioButton->isChecked();
  Filter_SP.ZS = SourceImpedanceLineEdit->text().toDouble();
  Filter_SP.Implementation = FilterImplementationCombo->currentText();
  Filter_SP.minZ = MinimumZ_Spinbox->value();
  Filter_SP.maxZ = MaximumZ_Spinbox->value();
  Filter_SP.ImpedanceRatio = ImpedanceRatio_Spinbox->value();

  if (SemiLumpedImplementationCombo->currentText() ==
      "Replace inductors and shunt capacitors")
    Filter_SP.SemiLumpedISettings = INDUCTORS_AND_SHUNT_CAPS;
  if (SemiLumpedImplementationCombo->currentText() == "Replace only inductors")
    Filter_SP.SemiLumpedISettings = ONLY_INDUCTORS;

 // The data comes from comboboxes rather than spinboxes
  Filter_SP.order = 3;//OrderCombobox->currentText().toInt();
  Filter_SP.Ripple = 0.01;//RippleCombobox->currentText().toDouble();
    bool ok;
  Filter_SP.ZL = 50;//RLCombobox->currentText().toDouble(&ok);
    if (!ok)
      Filter_SP.ZL = 1e8 * Filter_SP.ZS; // It is a single ended filter


  synthesize();
  // EMIT SIGNAL TO FORCE SIMULATION
  emit simulateNetwork(SchContent);
}

void FilterDesignTool::EllipticTypeChanged() {
  if (EllipticType->currentText() != "Type S") {
    OrderSpinBox->setMinimum(2);
  } else {
    OrderSpinBox->setMinimum(1);
  }
}

void FilterDesignTool::SwitchZverevTablesMode(bool ZverevMode) {
  if (FilterResponseTypeCombo->currentText() == "Elliptic")
    return; // Zverev mode is only available for canonical responses
  if (ZverevMode) {
    QString aux = FilterResponseTypeCombo->currentText();
    QStringList data = setItemsResponseTypeCombo();
    FilterResponseTypeCombo->blockSignals(true);
    FilterResponseTypeCombo->clear();
    FilterResponseTypeCombo->addItems(data);
    FilterResponseTypeCombo->blockSignals(false);
    // Find the index of the current response type and select it for the Zverev
    // mode
    for (int i = 0; i < FilterResponseTypeCombo->count(); i++) {
      if (FilterResponseTypeCombo->itemText(i) == aux) {
        FilterResponseTypeCombo->setCurrentIndex(i);
        break;
      }
    }
    ResposeComboChanged(); // Fill the user input widgets with the parameters of
                           // the Zverev tables

    OrderSpinBox->hide();
    OrderCombobox->show();
    RLlabel->show();
    RLlabelOhm->show();
    RLCombobox->show();
    RippleCombobox->show();
    RippleSpinbox->hide();
    FilterResponseTypeCombo->blockSignals(false);
  } else { // Conventional mode. Restore the default filter responses
    OrderSpinBox->show();
    OrderCombobox->hide();
    OrderCombobox->clear();
    RLlabel->hide();
    RLlabelOhm->hide();
    RLCombobox->hide();
    RippleCombobox->hide();
    RippleSpinbox->show();

    QString aux =
        FilterResponseTypeCombo
            ->currentText(); // Last response selected in the Zverev mode
    FilterResponseTypeCombo->clear();
    FilterResponseTypeCombo->addItems(
        DefaultFilterResponses); // Fill the combo with the default option
    for (int i = 0; i < FilterResponseTypeCombo->count();
         i++) { // Find the index of the current response type and select it for
                // the conventional mode
      if (FilterResponseTypeCombo->itemText(i) == aux) {
        FilterResponseTypeCombo->setCurrentIndex(i);
        break;
      }
    }
    ResposeComboChanged();
    // Update order and ripple
    OrderSpinBox->setValue(OrderCombobox->currentText().toInt());
    RippleSpinbox->setValue(RippleCombobox->currentText().toDouble());
  }
  UpdateDesignParameters();
}

void FilterDesignTool::ChangeRL_CLC_LCL_mode() {
  RLCombobox->blockSignals(true);
  RLCombobox->setInsertPolicy(QComboBox::InsertAfterCurrent);
  if (UseZverevTablesCheckBox->isChecked()) {
    bool mode = CLCRadioButton->isChecked();
    double ZS = SourceImpedanceLineEdit->text().toDouble();
    for (int i = 0; i < RLCombobox->count(); i++) {
      double aux = RLCombobox->itemText(i).toDouble();

      if (mode) { // CLC mode selected, then previously it was LCL
        aux /= ZS;
        aux = 1 / aux;
        aux *= ZS;
      } else {     // LCL mode selected, then previously it was CLC
        aux /= ZS; // Now aux = gi or aux = 1/gi
        aux = ZS / aux;
      }
      RLCombobox->insertItem(i, QString("%1").arg(aux));
      RLCombobox->removeItem(i + 1);
    }
  }
  for (int i = 0; i < RLCombobox->count(); i++)
    qDebug() << QString("%1").arg(RLCombobox->itemText(i).toDouble());
  qDebug() << "Current index: " << RLCombobox->currentIndex() << " = "
           << RLCombobox->currentText().toDouble();
  RLCombobox->blockSignals(false);
  UpdateDesignParameters();
  return;
}

void FilterDesignTool::UpdateRipple(int refresh = 0) {
  if (OrderCombobox->currentText().isEmpty())
    return;
  double Ripple_CurrentVal = RippleCombobox->currentText().toDouble();
  double min_dist_err = 1e6;
  int index = 0, selected_index = 0;
  QStringList data;
  QString table = FilterResponseTypeCombo->currentText()
                      .trimmed(); // Name of the table without whitespaces

  data.clear();
  QString query_str;


    RippleCombobox->clear();

  if (refresh > -1)
    UpdateDesignParameters();
}



QStringList FilterDesignTool::setItemsResponseTypeCombo() {
  QStringList data;

    data = DefaultFilterResponses;
  return data;
}

// This function returns the scale of the argument
double FilterDesignTool::getScale(QString scale) {
  if (!scale.compare("GHz"))
    return 1e9;
  if (!scale.compare("MHz"))
    return 1e6;
  if (!scale.compare("kHz"))
    return 1e3;
   else
    return 1;
}

// The purpose of this function is to trigger a design from the main application
void FilterDesignTool::design() { UpdateDesignParameters(); }

// This function updates the input combos according to the filter implementation
void FilterDesignTool::ImplementationComboChanged(int index) {

  switch (index) {
  case LC_LADDER: // LC Ladder
    // Show LC box
    CLCRadioButton->show();
    LCLRadioButton->show();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Hide minimum impedance
    MinimumZLabel->hide();
    MinimumZ_Spinbox->hide();
    MinimumZ_Unit_Label->hide();

           // Hide maximum impedance
    MaximumZLabel->hide();
    MaximumZ_Spinbox->hide();
    MaximumZ_Unit_Label->hide();

           // All response types available: Lowpass, highpass, bandpass and notch
           // Elliptic types are available
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Lowpass");
    FilterClassCombo->addItem("Highpass");
    FilterClassCombo->addItem("Bandpass");
    FilterClassCombo->addItem("Bandstop");

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  case LC_DIRECT_COUPLED: // LC Direct Coupled
    // Hide LC box
    CLCRadioButton->hide();
    LCLRadioButton->hide();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Hide minimum impedance
    MinimumZLabel->hide();
    MinimumZ_Spinbox->hide();
    MinimumZ_Unit_Label->hide();

           // Hide maximum impedance
    MaximumZLabel->hide();
    MaximumZ_Spinbox->hide();
    MaximumZ_Unit_Label->hide();

           // Only bandpass filters can be implemented using direct coupled filters
           // The elliptic response cannot be implemented too
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Bandpass");
    FilterClassCombo->setCurrentIndex(0);

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  case QUARTERWAVE: // QW bandpass and bandstop
    // Hide LC box
    CLCRadioButton->hide();
    LCLRadioButton->hide();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Hide minimum impedance
    MinimumZLabel->hide();
    MinimumZ_Spinbox->hide();
    MinimumZ_Unit_Label->hide();

           // Hide maximum impedance
    MaximumZLabel->hide();
    MaximumZ_Spinbox->hide();
    MaximumZ_Unit_Label->hide();

           // Only bandpass and notch types available
           // Of course, the elliptic type cannot be implemented this way.
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Bandpass");
    FilterClassCombo->addItem("Bandstop");
    FilterClassCombo->setCurrentIndex(0);

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  case STEPPED_IMPEDANCE: // Stepped impedance LPF
    // Hide LC box
    CLCRadioButton->hide();
    LCLRadioButton->hide();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Show minimum impedance
    MinimumZLabel->show();
    MinimumZ_Spinbox->show();
    MinimumZ_Unit_Label->show();

           // Show maximum impedance
    MaximumZLabel->show();
    MaximumZ_Spinbox->show();
    MaximumZ_Unit_Label->show();

           // Only bandpass filters can be implemented using direct coupled filters
           // The elliptic response cannot be implemented too
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Lowpass");
    FilterClassCombo->setCurrentIndex(0);

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  case END_COUPLED: // End-coupled bandpass
    // Hide LC box
    CLCRadioButton->hide();
    LCLRadioButton->hide();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Hide minimum impedance
    MinimumZLabel->hide();
    MinimumZ_Spinbox->hide();
    MinimumZ_Unit_Label->hide();

           // Hide maximum impedance
    MaximumZLabel->hide();
    MaximumZ_Spinbox->hide();
    MaximumZ_Unit_Label->hide();

           // Only bandpass type available
           // Of course, the elliptic type cannot be implemented this way.
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Bandpass");
    FilterClassCombo->setCurrentIndex(0);

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  case CAPACITIVELY_COUPLED_RESONATORS: // Capacitively-coupled shunt resonators
    // Hide LC box
    CLCRadioButton->hide();
    LCLRadioButton->hide();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Hide minimum impedance
    MinimumZLabel->hide();
    MinimumZ_Spinbox->hide();
    MinimumZ_Unit_Label->hide();

           // Hide maximum impedance
    MaximumZLabel->hide();
    MaximumZ_Spinbox->hide();
    MaximumZ_Unit_Label->hide();

           // Only bandpass type available
           // Of course, the elliptic type cannot be implemented this way.
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Bandpass");
    FilterClassCombo->setCurrentIndex(0);

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  case SEMILUMPED_ELLIPTIC: // Semilumped Elliptic
    // Show LC box
    CLCRadioButton->show();
    LCLRadioButton->show();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Show minimum impedance
    MinimumZLabel->show();
    MinimumZ_Spinbox->show();
    MinimumZ_Unit_Label->show();

           // Show maximum impedance
    MaximumZLabel->show();
    MaximumZ_Spinbox->show();
    MaximumZ_Unit_Label->show();

           // Only lowpass and highpass
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Lowpass");
    FilterClassCombo->addItem("Highpass");

           // Show semilumped type combobox
    SemiLumpedImplementationCombo->show();
    SemiLumpedImplementationLabel->show();
    break;
  case SEMILUMPED_CANONICAL: // Semilumped Canonical
    // Show LC box
    CLCRadioButton->show();
    LCLRadioButton->show();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Only lowpass and highpass
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Lowpass");
    FilterClassCombo->addItem("Highpass");

           // Show semilumped type combobox
    SemiLumpedImplementationCombo->show();
    SemiLumpedImplementationLabel->show();
    break;
  case COUPLED_LINE: // Coupled line bandpass
    // Hide LC box
    CLCRadioButton->hide();
    LCLRadioButton->hide();

           // Hide impedance ratio
    ImpedanceRatio_Label->hide();
    ImpedanceRatio_Spinbox->hide();

           // Show minimum impedance
    MinimumZLabel->show();
    MinimumZ_Spinbox->show();
    MinimumZ_Unit_Label->show();

           // Show maximum impedance
    MaximumZLabel->show();
    MaximumZ_Spinbox->show();
    MaximumZ_Unit_Label->show();

           // Only bandpass type available
           // Of course, the elliptic type cannot be implemented this way.
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Bandpass");
    FilterClassCombo->setCurrentIndex(0);

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  case COUPLED_LINE_SIR: // Coupled line with harmonic rejection
    // Hide LC box
    CLCRadioButton->hide();
    LCLRadioButton->hide();

           // Show impedance ratio
    ImpedanceRatio_Label->show();
    ImpedanceRatio_Spinbox->show();

           // Show minimum impedance
    MinimumZLabel->show();
    MinimumZ_Spinbox->show();
    MinimumZ_Unit_Label->show();

           // Show maximum impedance
    MaximumZLabel->show();
    MaximumZ_Spinbox->show();
    MaximumZ_Unit_Label->show();

           // Only bandpass type available
           // Of course, the elliptic type cannot be implemented this way.
    FilterClassCombo->clear();
    FilterClassCombo->addItem("Bandpass");
    FilterClassCombo->setCurrentIndex(0);

           // Hide semilumped type combobox
    SemiLumpedImplementationCombo->hide();
    SemiLumpedImplementationLabel->hide();
    break;
  default:
    break;
  }
  UpdateDesignParameters();
}
