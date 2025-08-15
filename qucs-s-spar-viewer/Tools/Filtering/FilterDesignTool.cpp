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
  FilterImplementationCombo->addItem("Stepped impedance");
  FilterImplementationCombo->addItem("Quarter-wavelength");
  FilterImplementationCombo->addItem("Capacitively-coupled shunt resonators");
  FilterImplementationCombo->addItem("Coupled line bandpass");
  FilterImplementationCombo->addItem("End-coupled");
  FilterImplementationCombo->addItem("Semilumped");
  //FilterImplementationCombo->addItem("Coupled line SIR with harmonic rejection");
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
  DefaultFilterResponses.append("Legendre");
  DefaultFilterResponses.append("Bessel");
  DefaultFilterResponses.append("Gaussian");
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
  OrderSpinBox->setValue(3);
  OrderSpinBox->setMinimum(2);

  FilterDesignLayout->addWidget(new QLabel("Order"), 4, 0);
  FilterDesignLayout->addWidget(OrderSpinBox, 4, 1);

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
  RippleLabel = new QLabel("Ripple");
  RippledBLabel = new QLabel("dB");
  FilterDesignLayout->addWidget(RippleLabel, 7, 0);
  FilterDesignLayout->addWidget(RippleSpinbox, 7, 1);
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

         //***************  Minimum impedance achievable in the manufacturing process **************
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
  FilterDesignLayout->addWidget(MinimumZLabel, 10, 0);
  FilterDesignLayout->addWidget(MinimumZ_Spinbox, 10, 1);
  FilterDesignLayout->addWidget(MinimumZ_Unit_Label, 10, 2);

         //***************  Maximum impedance achievable in the manufacturing process **************
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
  FilterDesignLayout->addWidget(MaximumZLabel, 11, 0);
  FilterDesignLayout->addWidget(MaximumZ_Spinbox, 11, 1);
  FilterDesignLayout->addWidget(MaximumZ_Unit_Label, 11, 2);
  //************************* Semilumped implementation settings *********************
  SemiLumpedImplementationLabel = new QLabel("Semilumped settings");
  SemiLumpedImplementationCombo = new QComboBox();
  SemiLumpedImplementationCombo->addItem("Replace inductors and shunt capacitors");
  SemiLumpedImplementationCombo->addItem("Replace only inductors");
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();
  FilterDesignLayout->addWidget(SemiLumpedImplementationLabel, 12, 0);
  FilterDesignLayout->addWidget(SemiLumpedImplementationCombo, 12, 1);

         // Coupled line SIR BPF type
  ImpedanceRatio_Label = new QLabel(QString("Impedance ratio, K"));
  ImpedanceRatio_Spinbox = new QDoubleSpinBox();
  ImpedanceRatio_Spinbox->setMinimum(0.1);
  ImpedanceRatio_Spinbox->setMaximum(10);
  ImpedanceRatio_Spinbox->setValue(0.5);
  ImpedanceRatio_Spinbox->setSingleStep(0.1);

  FilterDesignLayout->addWidget(ImpedanceRatio_Label, 13, 0);
  FilterDesignLayout->addWidget(ImpedanceRatio_Spinbox, 13, 1);

         //************ Source impedance **********
  SourceImpedanceLineEdit = new QLineEdit("50");
  FilterDesignLayout->addWidget(new QLabel("ZS"), 14, 0);
  FilterDesignLayout->addWidget(SourceImpedanceLineEdit, 14, 1);
  FilterDesignLayout->addWidget(new QLabel(QChar(0xa9, 0x03)), 14, 2);

         // Widgets to add a trace to plot
  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Filter1");
  FilterDesignLayout->addWidget(traceNameLabel, 15, 0);
  FilterDesignLayout->addWidget(traceNameLineEdit, 15, 1);


  this->setLayout(FilterDesignLayout);

         // Connection functions for updating the network requirements and simulate in real time
  connect(FilterImplementationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ImplementationComboChanged(int)));
  connect(CLCRadioButton, SIGNAL(toggled(bool)), this, SLOT(ChangeRL_CLC_LCL_mode()));
  connect(FilterResponseTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ResposeComboChanged()));
  connect(EllipticType, SIGNAL(currentIndexChanged(int)), this, SLOT(EllipticTypeChanged()));
  connect(FilterClassCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(OrderSpinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(FCSpinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(FC_ScaleCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(BWSpinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(BW_ScaleCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(SourceImpedanceLineEdit, SIGNAL(textChanged(QString)), this, SLOT(UpdateDesignParameters()));
  connect(RippleSpinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(StopbandAttSpinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(EllipticType, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(DC_CouplingTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(MinimumZ_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(MaximumZ_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));
  connect(SemiLumpedImplementationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateDesignParameters()));
  connect(ImpedanceRatio_Spinbox, SIGNAL(valueChanged(double)), this, SLOT(UpdateDesignParameters()));

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

  int filter_type = FilterImplementationCombo->currentIndex();

  switch (filter_type) {
  case LC_LADDER:
    if (FilterResponseTypeCombo->currentText() == QString("Elliptic")){
      EllipticFilter *EF;
      EF = new EllipticFilter(Filter_SP);
      EF->synthesize();
      SchContent = EF->Schematic;
      delete EF;
    } else {
      CanonicalFilter *CF;
      CF = new CanonicalFilter(Filter_SP);
      CF->synthesize();
      SchContent = CF->Schematic;
      delete CF;
    }
    break;

  case LC_DIRECT_COUPLED:
    DirectCoupledFilters *DCF;
    DCF = new DirectCoupledFilters(Filter_SP);
    DCF->synthesize();
    SchContent = DCF->Schematic;
    delete DCF;
    break;

  case STEPPED_IMPEDANCE:
    SteppedImpedanceFilter *STIF;
    STIF = new SteppedImpedanceFilter(Filter_SP);
    STIF->synthesize();
    SchContent = STIF->Schematic;
    delete STIF;
    break;

  case QUARTERWAVE:
    QuarterWaveFilters *QWF;
    QWF = new QuarterWaveFilters(Filter_SP);
    QWF->synthesize();
    SchContent = QWF->Schematic;
    delete QWF;
    break;

  case CAPACITIVELY_COUPLED_RESONATORS:
    CapacitivelyCoupledShuntResonatorsFilter *CCSRF;
    CCSRF = new CapacitivelyCoupledShuntResonatorsFilter(Filter_SP);
    CCSRF->synthesize();
    SchContent = CCSRF->Schematic;
    delete CCSRF;
    break;

  case COUPLED_LINES:
    CoupledLineBandpassFilter *CLBPF;
    CLBPF = new CoupledLineBandpassFilter(Filter_SP);
    CLBPF->synthesize();
    SchContent = CLBPF->Schematic;
    delete CLBPF;
    break;

  case END_COUPLED:
    EndCoupled *ECF;
    ECF = new EndCoupled(Filter_SP);
    ECF->synthesize();
    SchContent = ECF->Schematic;
    delete ECF;
    break;

  case SEMILUMPED:
    if (!FilterResponseTypeCombo->currentText().compare("Elliptic")){
      EllipticFilter *SMLEF;
      SMLEF = new EllipticFilter(Filter_SP);
      SMLEF->setSemilumpedMode(true);
      SMLEF->synthesize();
      SchContent = SMLEF->Schematic;
      delete SMLEF;
    } else {
      // Canonical filter
      CanonicalFilter *CF;
      CF = new CanonicalFilter(Filter_SP);
      CF->setSemilumpedMode(true);
      CF->synthesize();
      SchContent = CF->Schematic;
      delete CF;
    }
    break;
  }


  QString FilterName = traceNameLineEdit->text();
  SchContent.Name = FilterName;
  emit updateSchematic(SchContent);
  emit updateSimulation(SchContent);

}

void FilterDesignTool::ResposeComboChanged() {
  QString Response = FilterResponseTypeCombo->currentText();
  bool ActivateCauer = !Response.compare("Elliptic");
  StopbandAttSpinbox->setVisible(ActivateCauer);
  StopbandAttLabel->setVisible(ActivateCauer);
  StopbandAttdBLabel->setVisible(ActivateCauer);

  EllipticType->setVisible(ActivateCauer);
  EllipticTypeLabel->setVisible(ActivateCauer);
  if (ActivateCauer) {
    CLCRadioButton->setText("Min L");
    LCLRadioButton->setText("Min C");
  } else {
    CLCRadioButton->setText("CLC");
    LCLRadioButton->setText("LCL");
  }

         // By default, don't limit the order.
  OrderSpinBox->setMinimum(1);
  OrderSpinBox->setMaximum(100);

         // Bessel, Gaussian and Legendre filter types are tabulated and the data is limited
  if (!Response.compare("Bessel")) {
    OrderSpinBox->setMinimum(2);
    OrderSpinBox->setMaximum(10);
    RippleLabel->hide();
    RippleSpinbox->hide();
  } else {
    if (!Response.compare("Gaussian")) {
      OrderSpinBox->setMinimum(3);
      OrderSpinBox->setMaximum(10);
      RippleLabel->hide();
      RippleSpinbox->hide();
    } else {
      if (!Response.compare("Legendre")) {
        OrderSpinBox->setMinimum(4);
        OrderSpinBox->setMaximum(10);
        RippleLabel->hide();
        RippleSpinbox->hide();
      } else {
        if (!Response.compare("Butterworth")) {
          RippleLabel->hide();
          RippleSpinbox->hide();
        } else {
          // Elliptic and Chebyshev
          RippleLabel->show();
          RippleSpinbox->show();
        }
      }
    }
  }


  UpdateDesignParameters();
}

// This function catches the events related to the changes in the filter specificatios
void FilterDesignTool::UpdateDesignParameters() {
  Filter_SP.Implementation = FilterImplementationCombo->currentText();

         // Filter response
  if (!FilterResponseTypeCombo->currentText().compare("Chebyshev"))
    Filter_SP.FilterResponse = Chebyshev;
  if (!FilterResponseTypeCombo->currentText().compare("Butterworth"))
    Filter_SP.FilterResponse = Butterworth;
  if (!FilterResponseTypeCombo->currentText().compare("Elliptic"))
    Filter_SP.FilterResponse = Elliptic;
  if (!FilterResponseTypeCombo->currentText().compare("Bessel"))
    Filter_SP.FilterResponse = Bessel;
  if (!FilterResponseTypeCombo->currentText().compare("Gaussian"))
    Filter_SP.FilterResponse = Gaussian;
  if (!FilterResponseTypeCombo->currentText().compare("Legendre"))
    Filter_SP.FilterResponse = Legendre;



         // Filter type
  if (!FilterClassCombo->currentText().compare("Lowpass"))
    Filter_SP.FilterType = Lowpass;
  if (!FilterClassCombo->currentText().compare("Highpass"))
    Filter_SP.FilterType = Highpass;
  if (!FilterClassCombo->currentText().compare("Bandpass"))
    Filter_SP.FilterType = Bandpass;
  if (!FilterClassCombo->currentText().compare("Bandstop"))
    Filter_SP.FilterType = Bandstop;

         // Coupling
  if (!DC_CouplingTypeCombo->currentText().compare("Capacitative coupled shunt resonators")) {
    Filter_SP.DC_Coupling = CapacitativeCoupledShuntResonators;
  }

  if (!DC_CouplingTypeCombo->currentText().compare("Inductive coupled series resonators")) {
    Filter_SP.DC_Coupling = InductiveCoupledSeriesResonators;
  }

         // Update user input
  if ((!FilterClassCombo->currentText().compare("Lowpass")) ||  (!FilterClassCombo->currentText().compare("Highpass"))) {
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

         // Update parameters
  Filter_SP.bw = BWSpinbox->value() * getScale(BW_ScaleCombobox->currentText());
  Filter_SP.fc = FCSpinbox->value() * getScale(FC_ScaleCombobox->currentText());
  Filter_SP.EllipticType = EllipticType->currentText();
  Filter_SP.isCLC = CLCRadioButton->isChecked();
  Filter_SP.ZS = SourceImpedanceLineEdit->text().toDouble();
  Filter_SP.Implementation = FilterImplementationCombo->currentText();
  Filter_SP.minZ = MinimumZ_Spinbox->value();
  Filter_SP.maxZ = MaximumZ_Spinbox->value();
  Filter_SP.ImpedanceRatio = ImpedanceRatio_Spinbox->value();
  Filter_SP.order = OrderSpinBox->value();
  Filter_SP.Ripple = RippleSpinbox->value();
  Filter_SP.as = StopbandAttSpinbox->value();
  Filter_SP.ZL = 50;



  if (SemiLumpedImplementationCombo->currentText() == "Replace inductors and shunt capacitors") {
    Filter_SP.SemiLumpedISettings = INDUCTORS_AND_SHUNT_CAPS;
  }
  if (SemiLumpedImplementationCombo->currentText() == "Replace only inductors") {
    Filter_SP.SemiLumpedISettings = ONLY_INDUCTORS;
  }

  synthesize();
}

// This function is called from ImplementationComboChanged() and sets the UI widgets for the LC ladder topologies.
void FilterDesignTool::setSettings_LC_Ladder(){
  // LC ladder filters
  DC_CouplingTypeCombo->hide();
  DC_CouplingLabel->hide();

         // Show CLC/LCL box
  CLCRadioButton->show();
  LCLRadioButton->show();

         // Hide minimum impedance
  MinimumZLabel->hide();
  MinimumZ_Spinbox->hide();
  MinimumZ_Unit_Label->hide();

         // Hide maximum impedance
  MaximumZLabel->hide();
  MaximumZ_Spinbox->hide();
  MaximumZ_Unit_Label->hide();

         // Hide semilumped type combobox
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();

         // All response types available: Lowpass, highpass, bandpass and notch. Elliptic types are available
  FilterClassCombo->clear();
  FilterClassCombo->addItem("Lowpass");
  FilterClassCombo->addItem("Highpass");
  FilterClassCombo->addItem("Bandpass");
  FilterClassCombo->addItem("Bandstop");


  FilterClassCombo->setEnabled(true);
  QString CurrentResponse = FilterResponseTypeCombo->currentText();
  FilterResponseTypeCombo->blockSignals(true);

         // Set default filter responses + Elliptic
  QStringList filter_response;
  filter_response = DefaultFilterResponses;
  filter_response.append("Elliptic");
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(filter_response);


  for (int i = 0; i < filter_response.length(); i++) {
    if (CurrentResponse == filter_response.at(i)) {
      FilterResponseTypeCombo->setCurrentIndex(i);
      break;
    }
  }
  FilterResponseTypeCombo->blockSignals(false);
}

// This function is called from ImplementationComboChanged() and sets the UI widgets for the LC direct coupled topologies.
void FilterDesignTool::setSettings_LC_Direct_Coupled(){
  DC_CouplingTypeCombo->show();
  DC_CouplingLabel->show();
  FilterResponseTypeCombo->blockSignals(true);

         // Hide CLC/LCL box
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
  FilterClassCombo->clear();
  FilterClassCombo->addItem("Bandpass");
  FilterClassCombo->setCurrentIndex(0);

         // Set default filter responses. Elliptic type is not implementable using the direct coupled topology
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterResponseTypeCombo->setCurrentIndex(0); // Chebyshev

         // Hide semilumped type combobox
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();

  FilterClassCombo->blockSignals(false);
  FilterResponseTypeCombo->blockSignals(false);
}

// This function is called from ImplementationComboChanged() and sets the UI widgets for the Stepped-Z LPF
void FilterDesignTool::setSettings_Stepped_Z_LPF(){
  FilterResponseTypeCombo->blockSignals(true);
  FilterResponseTypeCombo->clear();

         // Show CLC/LCL box
  CLCRadioButton->show();
  LCLRadioButton->show();

         // Hide impedance ratio
  ImpedanceRatio_Label->hide();
  ImpedanceRatio_Spinbox->hide();

         //Show minimum impedance
  MinimumZLabel->show();
  MinimumZ_Spinbox->show();
  MinimumZ_Unit_Label->show();

         // Show maximum impedance
  MaximumZLabel->show();
  MaximumZ_Spinbox->show();
  MaximumZ_Unit_Label->show();

         // Only bandpass filters can be implemented using direct coupled filters
  FilterClassCombo->clear();
  FilterClassCombo->addItem("Lowpass");
  FilterClassCombo->setCurrentIndex(0);

         // Hide semilumped type combobox
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();

         // Set default filter responses. Elliptic type is not implementable using the direct coupled topology
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterResponseTypeCombo->setCurrentIndex(0); // Chebyshev

  FilterClassCombo->setEnabled(false);
  FilterResponseTypeCombo->blockSignals(false);
}

// This function is called from ImplementationComboChanged() and sets the UI widgets for the QW BPF and BSF.
void FilterDesignTool::setSettings_Quarterwavelength_BPF_BSF(){
  FilterResponseTypeCombo->blockSignals(true);
  FilterClassCombo->blockSignals(true);

         // Hide CLC box
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

         // Load filter response types
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterResponseTypeCombo->setCurrentIndex(0); // Chebyshev

         // Only bandpass and notch types available
  FilterClassCombo->clear();
  FilterClassCombo->addItem("Bandpass");
  FilterClassCombo->addItem("Bandstop");
  FilterClassCombo->setCurrentIndex(0);

         // Hide semilumped type combobox
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();


  FilterResponseTypeCombo->blockSignals(false);
  FilterClassCombo->blockSignals(false);
}

// This function is called from ImplementationComboChanged() and sets the UI widgets for the end-coupled BPF
void FilterDesignTool::setSettings_EndCoupled_BPF(){
  FilterResponseTypeCombo->blockSignals(true);
  FilterClassCombo->blockSignals(true);

         // Show CLC/LCL box
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

         // Load default filter response types. No elliptic type
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterResponseTypeCombo->setCurrentIndex(0); // Chebyshev

         // Only bandpass
  FilterClassCombo->clear();
  FilterClassCombo->addItem("Bandpass");
  FilterClassCombo->setCurrentIndex(0);

         // Hide semilumped type combobox
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();

  FilterResponseTypeCombo->blockSignals(false);
  FilterClassCombo->blockSignals(false);
}



// This function is called from ImplementationComboChanged() and sets the UI widgets for the cap-coupled BPF
void FilterDesignTool::setSettings_CCoupledShuntResonators_BPF(){
  FilterResponseTypeCombo->blockSignals(true);
  FilterClassCombo->blockSignals(true);

         // Show CLC/LCL box
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

         // Load default filter response types. No elliptic type
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterResponseTypeCombo->setCurrentIndex(0); // Chebyshev

         // Only bandpass
  FilterClassCombo->clear();
  FilterClassCombo->addItem("Bandpass");
  FilterClassCombo->setCurrentIndex(0);

         // Hide semilumped type combobox
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();

  FilterResponseTypeCombo->blockSignals(false);
  FilterClassCombo->blockSignals(false);
}


// This function is called from ImplementationComboChanged() and sets the UI widgets for the semilumped filters
void FilterDesignTool::setSettings_Semilumped(){
  FilterResponseTypeCombo->blockSignals(true);
  FilterClassCombo->blockSignals(true);

  // Load default filter response types + Elliptic type
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterResponseTypeCombo->addItem("Elliptic");
  FilterResponseTypeCombo->setCurrentIndex(0); // Chebyshev

  if (!FilterResponseTypeCombo->currentText().compare("Elliptic")){
    // Semilumped Elliptic
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
  } else {
    // Semilumped Canonical

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

    FilterResponseTypeCombo->blockSignals(false);
    FilterClassCombo->blockSignals(false);
  }
}


// This function is called from ImplementationComboChanged() and sets the UI widgets for the side coupled BPF
void FilterDesignTool::setSettings_SideCoupled_BPF(){
  FilterResponseTypeCombo->blockSignals(true);
  FilterClassCombo->blockSignals(true);

         // Show CLC/LCL box
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

         // Load default filter response types. No elliptic type
  FilterResponseTypeCombo->clear();
  FilterResponseTypeCombo->addItems(DefaultFilterResponses);
  FilterResponseTypeCombo->setCurrentIndex(0); // Chebyshev

         // Only bandpass
  FilterClassCombo->clear();
  FilterClassCombo->addItem("Bandpass");
  FilterClassCombo->setCurrentIndex(0);

         // Hide semilumped type combobox
  SemiLumpedImplementationCombo->hide();
  SemiLumpedImplementationLabel->hide();

  FilterResponseTypeCombo->blockSignals(false);
  FilterClassCombo->blockSignals(false);
}


void FilterDesignTool::EllipticTypeChanged() {
  if (EllipticType->currentText() != "Type S") {
    OrderSpinBox->setMinimum(2);
  } else {
    OrderSpinBox->setMinimum(1);
  }
}

void FilterDesignTool::ChangeRL_CLC_LCL_mode() {
  UpdateDesignParameters();
  return;
}


QStringList FilterDesignTool::setItemsResponseTypeCombo() {
  QStringList data;

  data = DefaultFilterResponses;
  return data;
}

       // This function returns the scale of the argument
double FilterDesignTool::getScale(QString scale) {
  QStringView view(scale);
  static const QHash<QStringView, double> units {
      { u"GHz", 1e9 },
      { u"MHz", 1e6 },
      { u"kHz", 1e3 }
  };
  return units.value(view, 1.0);
}

       // The purpose of this function is to trigger a design from the main application
void FilterDesignTool::design() { UpdateDesignParameters(); }

       // This function updates the input combos according to the filter implementation
void FilterDesignTool::ImplementationComboChanged(int index) {

  switch (index) {
  case LC_LADDER:
    setSettings_LC_Ladder();
    break;

  case LC_DIRECT_COUPLED:
    setSettings_LC_Direct_Coupled();
    break;

  case QUARTERWAVE: // QW BPF and BSF
    setSettings_Quarterwavelength_BPF_BSF();
    break;

  case STEPPED_IMPEDANCE: // LPF
    setSettings_Stepped_Z_LPF();
    break;

  case END_COUPLED: // BPF
    setSettings_EndCoupled_BPF();
    break;

  case CAPACITIVELY_COUPLED_RESONATORS: // Capacitively-coupled shunt resonators
    setSettings_CCoupledShuntResonators_BPF();
    break;

  case SEMILUMPED:
    setSettings_Semilumped();
    break;

  case COUPLED_LINES: // Coupled line bandpass
    setSettings_SideCoupled_BPF();
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
