/***************************************************************************
                               matchdialog.cpp
                              -----------------
    begin                : Fri Jul 22 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    copyright            : (C) 2012, 2013, 2016 by Qucs Team (see AUTHORS file)

-----------------------------------------------------------------------------
    Update (2017)        : New impedance matching techniques: Single matching,
                           double stub matching, real to real broadband transformers, 
                           cascaded L-sections and lambda/8 + lambda/4 matching

                          Andres Martinez-Mera <andresmartinezmera@gmail.com>
                          Claudio Girardi      <claudio.girardi@virgilio.it>
------------------------------------------------------------------------------

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../../qucs-filter/material_props.h"
#include "main.h"
#include "matchdialog.h"
#include "misc.h"
#include "qucs.h"

#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

MatchDialog::MatchDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("Create Matching Circuit"));
  DoubleVal = new QDoubleValidator(this);
  DoubleVal->setLocale(QLocale::C);

  all = new QHBoxLayout(this);
//  all->setSizeConstraint(QLayout::SetFixedSize);

  /* The main frame was divided into two vertical layouts. The first one, on the
     left side, is much the old matching tool whereas the other layout was
     included specifically for microstrip synthesis.
  */
  QVBoxLayout *matchFrame = new QVBoxLayout();   // Matching circuit design panel
  QVBoxLayout *micro_layout = new QVBoxLayout(); // Substrate properties
  all->addLayout(matchFrame);
  all->addLayout(micro_layout);

  MethodBox = new QGroupBox(tr("Implementation"));
  matchFrame->addWidget(MethodBox);
  MethodLayout = new QVBoxLayout();

  SubstrateBox = new QGroupBox(tr("Microstrip Substrate"));
  QGridLayout *hsubs = new QGridLayout();

  SubstrateBox->setVisible(false);
  micro_layout->addWidget(SubstrateBox);
  SubstrateBox->setLayout(hsubs);
  RelPermLabel = new QLabel(tr("Relative Permitivity"));
  RelPermCombo = new QComboBox();
  RelPermCombo->setEditable(true);
  const char **p = List_er;
  while (*(++p))
    RelPermCombo->addItem(*p); // The dielectric coeff combobox is filled with
                               // the materials taken from
                               // "../../qucs-filter/material_props.h"
  RelPermCombo->lineEdit()->setText("9.8");
  hsubs->addWidget(RelPermLabel, 0, 0);
  hsubs->addWidget(RelPermCombo, 0, 1, 1, 2);

  subsHLabel = new QLabel(tr("Substrate height"));
  SubHeightEdit = new QLineEdit("1.0");
  SubsHScale = new QLabel("mm");
  hsubs->addWidget(subsHLabel, 1, 0);
  hsubs->addWidget(SubHeightEdit, 1, 1);
  hsubs->addWidget(SubsHScale, 1, 2);

  // Thickness
  thicknessLabel = new QLabel(tr("Metal thickness"));
  thicknessEdit = new QLineEdit("12.5");
  ThicknessScale = new QLabel("um");
  hsubs->addWidget(thicknessLabel, 2, 0);
  hsubs->addWidget(thicknessEdit, 2, 1);
  hsubs->addWidget(ThicknessScale, 2, 2);

  // Minimum width
  minWLabel = new QLabel(tr("Minimum width"));
  minWEdit = new QLineEdit("0.4");
  minWScale = new QLabel("mm");
  hsubs->addWidget(minWLabel, 3, 0);
  hsubs->addWidget(minWEdit, 3, 1);
  hsubs->addWidget(minWScale, 3, 2);

  // Maximum width
  maxWLabel = new QLabel(tr("Maximum width"));
  maxWEdit = new QLineEdit("5.0");
  maxWScale = new QLabel("mm");
  hsubs->addWidget(maxWLabel, 4, 0);
  hsubs->addWidget(maxWEdit, 4, 1);
  hsubs->addWidget(maxWScale, 4, 2);

  // tan(delta)
  tanDLabel = new QLabel(tr("tanD"));
  tanDEdit = new QLineEdit("0.0125");
  hsubs->addWidget(tanDLabel, 5, 0);
  hsubs->addWidget(tanDEdit, 5, 1);

  // Resistivity
  ResistivityLabel = new QLabel(tr("Resistivity"));
  ResistivityEdit = new QLineEdit("2.43902e-08");
  hsubs->addWidget(ResistivityLabel, 6, 0);
  hsubs->addWidget(ResistivityEdit, 6, 1);

  QHBoxLayout *h4 = new QHBoxLayout();
  h4->setSpacing(3);
  TopoLabel = new QLabel(tr("Method"));
  h4->addWidget(TopoLabel);

  // Matching network topology
  TopoCombo = new QComboBox();
  TopoCombo->setFixedWidth(220);
  TopoCombo->addItem(tr("L-section"));
  TopoCombo->addItem(tr("Single stub"));
  TopoCombo->addItem(tr("Double stub"));
  QString str = tr("Multistage ") + QString(QChar(0xBB, 0x03)) + "/4";
  TopoCombo->addItem(str);
  TopoCombo->addItem("Cascaded L-sections");
  str = QString(QChar(0xBB, 0x03)) + "/8 +" + QString(QChar(0xBB, 0x03)) +
        "/4 transformer";
  TopoCombo->addItem(str);

  h4->addWidget(TopoCombo);
  connect(TopoCombo, SIGNAL(activated(int)), SLOT(slotChangeMode_TopoCombo()));
  h4->addStretch(5);
  MethodLayout->addLayout(h4);

  // When the stub implementation is selected, it is possible to select either
  // the open or the short circuit solution
  QHBoxLayout *h5 = new QHBoxLayout();
  OpenRadioButton = new QRadioButton(tr("Open stub"), this);
  h5->addWidget(OpenRadioButton);
  OpenRadioButton->setChecked(true);
  OpenRadioButton->setVisible(false);

  ShortRadioButton = new QRadioButton(tr("Short circuit stub"), this);
  h5->addWidget(ShortRadioButton);
  ShortRadioButton->setChecked(false);
  ShortRadioButton->setVisible(false);
  MethodLayout->addLayout(h5);

  // Number of sections of the cascaded lambda/4 implementation
  QHBoxLayout *h6 = new QHBoxLayout();
  OrderLabel = new QLabel(tr("Number of sections"));
  OrderEdit = new QLineEdit("3");
  h6->addWidget(OrderLabel);
  h6->addWidget(OrderEdit);
  OrderLabel->setVisible(false);
  OrderEdit->setVisible(false);
  OrderEdit->setFixedWidth(50);
  OrderEdit->setAlignment(Qt::AlignLeft);
  h6->setAlignment(Qt::AlignLeft);

  // Weighting: Binomial or Chebyshev
  QHBoxLayout *h7 = new QHBoxLayout();
  Weighting_groupBox = new QGroupBox(tr("Weighting"));
  QHBoxLayout *h7_box = new QHBoxLayout();
  QVBoxLayout *v7_box = new QVBoxLayout();
  BinRadio = new QRadioButton(tr("Binomial"), this);
  ChebyRadio = new QRadioButton(tr("Chebyshev"), this);
  BinRadio->setChecked(true);
  h7_box->addWidget(BinRadio);
  h7_box->addWidget(ChebyRadio);

  // Maximum ripple
  QHBoxLayout *h8 = new QHBoxLayout();
  maxRippleLabel = new QLabel(tr("Maximum ripple"));
  h8->addWidget(maxRippleLabel);
  MaxRippleEdit = new QLineEdit("0.05");
  h8->addWidget(MaxRippleEdit);
  MaxRippleEdit->setFixedWidth(50);
  MaxRippleEdit->setAlignment(Qt::AlignLeft);
  h8->setAlignment(Qt::AlignLeft);

  QGridLayout *OptLayout = new QGridLayout();
  v7_box->addLayout(h7_box);
  v7_box->addLayout(h8);
  Weighting_groupBox->setLayout(v7_box);
  Weighting_groupBox->setVisible(false);
  h7->addWidget(Weighting_groupBox);
  MethodLayout->addLayout(h7);
  MethodLayout->addLayout(h6);
  MethodBox->setLayout(MethodLayout);
  BalancedCheck = new QCheckBox(tr("Use balanced stubs"));
  BalancedCheck->setEnabled(false);
  TwoCheck = new QCheckBox(tr("Calculate two-port matching"));
  TwoCheck->setChecked(true);
  AddSPBlock =
      new QCheckBox(tr("Add S-Parameter simulation")); // The user can choose
                                                       // whether add a S-param
                                                       // simulation or not
  AddSPBlock->setChecked(false);
  MicrostripCheck = new QCheckBox(tr("Synthesize microstrip lines"));
  MicrostripCheck->setEnabled(false);
  MicrostripCheck->setChecked(false);

  OptLayout->addWidget(BalancedCheck, 0, 0);
  OptLayout->addWidget(TwoCheck, 0, 1);
  OptLayout->addWidget(AddSPBlock, 1, 0);
  OptLayout->addWidget(MicrostripCheck, 1, 1);
  matchFrame->addLayout(OptLayout);

  connect(TwoCheck, SIGNAL(toggled(bool)), SLOT(slotSetTwoPort(bool)));
  connect(MicrostripCheck, SIGNAL(toggled(bool)),
          SLOT(slotSetMicrostripCheck()));
  connect(ChebyRadio, SIGNAL(toggled(bool)), SLOT(slotChebyCheck()));

  // ...........................................................
  QGroupBox *ImpBox = new QGroupBox(tr("Reference Impedance"));
  matchFrame->addWidget(ImpBox);
  QHBoxLayout *ImpLayout = new QHBoxLayout();
  Port1Label = new QLabel(tr("Port 1"));
  Ref1Edit = new QLineEdit("50");
  Ref1Edit->setMaximumWidth(75);
  Ref1Edit->setValidator(DoubleVal);
  Ohm1Label = new QLabel(QString(QChar(0xA9, 0x03)));
  connect(Ref1Edit, SIGNAL(textChanged(const QString &)),
          SLOT(slotImpedanceChanged(const QString &)));
  Port2Label = new QLabel(tr("Port 2"));
  Ref2Edit = new QLineEdit("50");
  Ref2Edit->setMaximumWidth(75);
  Ref2Edit->setValidator(DoubleVal);
  Ohm2Label = new QLabel(QString(QChar(0xA9, 0x03)));
  ImpLayout->addWidget(Port1Label);
  ImpLayout->addWidget(Ref1Edit);
  ImpLayout->addWidget(Ohm1Label);
  ImpLayout->addSpacing(50);
  ImpLayout->addWidget(Port2Label);
  ImpLayout->addWidget(Ref2Edit);
  ImpLayout->addWidget(Ohm2Label);
  ImpBox->setLayout(ImpLayout);

  // ...........................................................
  QGroupBox *SParBox = new QGroupBox(tr("S Parameter"));
  matchFrame->addWidget(SParBox);
  QVBoxLayout *SParLayout = new QVBoxLayout();
  SParBox->setLayout(SParLayout);

  QHBoxLayout *h1 = new QHBoxLayout();
  h1->setSpacing(3);
  FormatLabel = new QLabel(tr("Input format"));
  h1->addWidget(FormatLabel);
  FormatCombo = new QComboBox();
  h1->addWidget(FormatCombo);
  FormatCombo->setFixedWidth(140);
  FormatCombo->addItem(tr("Real/Imag"));
  FormatCombo->addItem(tr("mag/deg"));
  connect(FormatCombo, SIGNAL(activated(int)), SLOT(slotChangeMode(int)));
  h1->addStretch(5);
  SParLayout->addLayout(h1);

  QHBoxLayout *h3 = new QHBoxLayout();
  h3->setSpacing(3);
  QVBoxLayout *VBox1 = new QVBoxLayout();
  h3->addLayout(VBox1);
  S11Label = new QLabel(tr("S11"));
  S21Label = new QLabel(tr("S21"));
  VBox1->addWidget(S11Label);
  VBox1->addWidget(S21Label);
  QVBoxLayout *VBox2 = new QVBoxLayout();
  h3->addLayout(VBox2);
  S11magEdit = new QLineEdit("0.5");
  S11magEdit->setValidator(DoubleVal);
  S11magEdit->setMaximumWidth(75);
  S21magEdit = new QLineEdit("0.5");
  S21magEdit->setValidator(DoubleVal);
  S21magEdit->setMaximumWidth(75);

  VBox2->addWidget(S11magEdit);
  VBox2->addWidget(S21magEdit);
  QVBoxLayout *VBox3 = new QVBoxLayout();
  h3->addLayout(VBox3);
  S11sLabel = new QLabel("+j");
  S21sLabel = new QLabel("+j");
  VBox3->addWidget(S11sLabel);
  VBox3->addWidget(S21sLabel);
  QVBoxLayout *VBox4 = new QVBoxLayout();
  h3->addLayout(VBox4);
  S11degEdit = new QLineEdit("0");
  S11degEdit->setValidator(DoubleVal);
  S11degEdit->setMaximumWidth(75);
  S21degEdit = new QLineEdit("0");
  S21degEdit->setValidator(DoubleVal);
  S21degEdit->setMaximumWidth(75);
  VBox4->addWidget(S11degEdit);
  VBox4->addWidget(S21degEdit);
  QVBoxLayout *VBox5 = new QVBoxLayout();
  h3->addLayout(VBox5);
  S11uLabel = new QLabel(" ");
  S21uLabel = new QLabel(" ");
  VBox5->addWidget(S11uLabel);
  VBox5->addWidget(S21uLabel);
  h3->addStretch(5);
  QVBoxLayout *VBox6 = new QVBoxLayout();
  h3->addLayout(VBox6);
  S12Label = new QLabel(tr("S12"));
  S22Label = new QLabel(tr("S22"));
  VBox6->addWidget(S12Label);
  VBox6->addWidget(S22Label);
  QVBoxLayout *VBox7 = new QVBoxLayout();
  h3->addLayout(VBox7);
  S12magEdit = new QLineEdit("0");
  S12magEdit->setValidator(DoubleVal);
  S12magEdit->setMaximumWidth(75);
  S22magEdit = new QLineEdit("0.5");
  S22magEdit->setValidator(DoubleVal);
  S22magEdit->setMaximumWidth(75);
  VBox7->addWidget(S12magEdit);
  VBox7->addWidget(S22magEdit);
  QVBoxLayout *VBox8 = new QVBoxLayout();
  h3->addLayout(VBox8);
  S12sLabel = new QLabel("+j");
  S22sLabel = new QLabel("+j");
  VBox8->addWidget(S12sLabel);
  VBox8->addWidget(S22sLabel);
  QVBoxLayout *VBox9 = new QVBoxLayout();
  h3->addLayout(VBox9);
  S12degEdit = new QLineEdit("0");
  S12degEdit->setMaximumWidth(75);
  S12degEdit->setValidator(DoubleVal);
  S22degEdit = new QLineEdit("0");
  S22degEdit->setMaximumWidth(75);
  S22degEdit->setValidator(DoubleVal);
  VBox9->addWidget(S12degEdit);
  VBox9->addWidget(S22degEdit);
  QVBoxLayout *VBox0 = new QVBoxLayout();
  h3->addLayout(VBox0);
  S12uLabel = new QLabel(" ");
  S22uLabel = new QLabel(" ");
  VBox0->addWidget(S12uLabel);
  VBox0->addWidget(S22uLabel);
  SParLayout->addLayout(h3);

  // set tab order to a more natural mode
  setTabOrder(S11magEdit, S11degEdit);
  setTabOrder(S11degEdit, S12magEdit);
  setTabOrder(S12magEdit, S12degEdit);
  setTabOrder(S12degEdit, S21magEdit);
  setTabOrder(S21magEdit, S21degEdit);
  setTabOrder(S21degEdit, S22magEdit);
  setTabOrder(S22magEdit, S22degEdit);

  connect(S21magEdit, SIGNAL(textChanged(const QString &)),
          SLOT(slotImpedanceChanged(const QString &)));
  connect(S21degEdit, SIGNAL(textChanged(const QString &)),
          SLOT(slotImpedanceChanged(const QString &)));
  connect(S11magEdit, SIGNAL(textChanged(const QString &)),
          SLOT(slotReflexionChanged(const QString &)));
  connect(S11degEdit, SIGNAL(textChanged(const QString &)),
          SLOT(slotReflexionChanged(const QString &)));

  QHBoxLayout *h2 = new QHBoxLayout();
  h2->setSpacing(3);
  FrequencyLabel = new QLabel(tr("Frequency:"));
  FrequencyEdit = new QLineEdit();
  FrequencyEdit->setValidator(DoubleVal);
  FrequencyEdit->setMaximumWidth(75);
  h2->addWidget(FrequencyLabel);
  h2->addWidget(FrequencyEdit);
  UnitCombo = new QComboBox();
  UnitCombo->addItem("Hz");
  UnitCombo->addItem("kHz");
  UnitCombo->addItem("MHz");
  UnitCombo->addItem("GHz");
  UnitCombo->setFixedWidth(100);
  h2->addWidget(UnitCombo);
  h2->addStretch(5);
  SParLayout->addLayout(h2);

  // ...........................................................
  QHBoxLayout *h0 = new QHBoxLayout();
  h0->setSpacing(5);
  matchFrame->addLayout(h0);
  h0->addStretch(5);
  QPushButton *buttCreate = new QPushButton(tr("Create"));
  QPushButton *buttCancel = new QPushButton(tr("Cancel"));
  h0->addWidget(buttCreate);
  h0->addWidget(buttCancel);
  connect(buttCreate, SIGNAL(clicked()), SLOT(slotButtCreate()));
  connect(buttCancel, SIGNAL(clicked()), SLOT(reject()));

  slotReflexionChanged(""); // calculate impedance
  setFrequency(1e9);        // set 1GHz
}

MatchDialog::~MatchDialog() {
  delete all;
  delete DoubleVal;
}

// -----------------------------------------------------------------------
void MatchDialog::setFrequency(double Freq_) {
  int Expo = int(log10(Freq_) / 3.0);
  if (Expo < 0)
    Expo = 0;
  else if (Expo > 3)
    Expo = 3;
  UnitCombo->setCurrentIndex(Expo);
  Freq_ /= pow(10.0, double(3 * Expo));
  FrequencyEdit->setText(QString::number(Freq_));
}

//------------------------------------------------------
// This function sets the visibility of the microstrip synthesis panel. The
// substrate properties are visible when the microstrip implementation is
// selected.
void MatchDialog::slotSetMicrostripCheck() {
  if (MicrostripCheck->isChecked()) {
    SubstrateBox->setVisible(true);
    resize(650, 100);
  } else {
    SubstrateBox->setVisible(false);
    setMaximumSize(500, 100);
  }
}

//---------------------------------------------------------
// When the Chebyshev weighting is selected, the window show the MaxRipple
// textbox and label so as to let the user to enter this parameter. It makes no
// sense to show it otherwise since when using the binomial weighting, the
// maximum ripple is given by the number of sections
void MatchDialog::slotChebyCheck() {
  if (ChebyRadio->isChecked()) {
    MaxRippleEdit->setVisible(true);
    maxRippleLabel->setVisible(true);
  } else {
    MaxRippleEdit->setVisible(false);
    maxRippleLabel->setVisible(false);
  }
}

// Set visibility of LineEdits and Labels associated with two-port matching
void MatchDialog::set2PortWidgetsVisible(bool visible) {
  S12magEdit->setVisible(visible);
  S22magEdit->setVisible(visible);
  S12degEdit->setVisible(visible);
  S22degEdit->setVisible(visible);
  S12Label->setVisible(visible);
  S22Label->setVisible(visible);
  S12sLabel->setVisible(visible);
  S22sLabel->setVisible(visible);
  S12degEdit->setVisible(visible);
  S22degEdit->setVisible(visible);
  S12uLabel->setVisible(visible);
  S22uLabel->setVisible(visible);
  Port2Label->setVisible(visible);
  Ref2Edit->setVisible(visible);
  Ohm2Label->setVisible(visible);
}
// -----------------------------------------------------------------------
// Is called when the checkbox for two-port matching changes.
void MatchDialog::slotSetTwoPort(bool on) {
  if (on) { // two-port matching ?
    S11Label->setText(tr("S11"));
    S21Label->setText(tr("S21"));
    // restore the previous S21 values
    setS21LineEdits(tmpS21mag, tmpS21deg);
    set2PortWidgetsVisible(true);
  } else {
    S11Label->setText(tr("Reflexion Coefficient"));
    S21Label->setText(tr("Impedance (Ohms)"));
    set2PortWidgetsVisible(false);
    // save S21 values, as these will be overwritten with the impedance value
    tmpS21mag = S21magEdit->text().toDouble();
    tmpS21deg = S21degEdit->text().toDouble();
    slotReflexionChanged(""); // calculate impedance
  }
}
//------------------------------------------------------------------------
// This function is called when a new topology is selected. According to the
// index selected, it makes visible (or invisible) certain window components.
void MatchDialog::slotChangeMode_TopoCombo() {
  if ((TopoCombo->currentIndex() == 1) ||
      (TopoCombo->currentIndex() == 2)) // Single/Double stub selected
  {
    ShortRadioButton->setVisible(true);
    OpenRadioButton->setVisible(true);
    OpenRadioButton->setChecked(true);
    BalancedCheck->setEnabled(true);
  } else {
    ShortRadioButton->setVisible(false);
    OpenRadioButton->setVisible(false);
    BalancedCheck->setEnabled(false);
  }
  if ((TopoCombo->currentIndex() != 0) &&
      (TopoCombo->currentIndex() != 4)) // Obviously, the microstrip
                                        // implementation panel cannot be used
                                        // when LC is selected
  {
    MicrostripCheck->setEnabled(true);
  } else {
    MicrostripCheck->setEnabled(false);
  }

  if (TopoCombo->currentIndex() == 3)
    Weighting_groupBox->setVisible(true); // Cascaded lambda/4 sections selected
  else
    Weighting_groupBox->setVisible(false);

  if ((TopoCombo->currentIndex() == 4) ||
      (TopoCombo->currentIndex() == 3)) // Cascaded LC sections selected
  {
    OrderLabel->setVisible(true);
    OrderEdit->setVisible(true);
  } else {
    OrderLabel->setVisible(false);
    OrderEdit->setVisible(false);
  }
}

// -----------------------------------------------------------------------
// Is called when the combobox changes between mag/deg and real/imag.
void MatchDialog::slotChangeMode(int Index) {
  if (Index) { // polar ?
    S11sLabel->setText("/");
    S12sLabel->setText("/");
    S21sLabel->setText("/");
    S22sLabel->setText("/");
    S11uLabel->setText(QString::fromUtf8("°"));
    S12uLabel->setText(QString::fromUtf8("°"));
    S21uLabel->setText(QString::fromUtf8("°"));
    S22uLabel->setText(QString::fromUtf8("°"));

    double Real = S11magEdit->text().toDouble();
    double Imag = S11degEdit->text().toDouble();
    c2p(Real, Imag);
    setS11LineEdits(Real, Imag);

    Real = S12magEdit->text().toDouble();
    Imag = S12degEdit->text().toDouble();
    c2p(Real, Imag);
    setS12LineEdits(Real, Imag);

    Real = S21magEdit->text().toDouble();
    Imag = S21degEdit->text().toDouble();
    c2p(Real, Imag);
    setS21LineEdits(Real, Imag);
    // convert also temp entries for future use
    c2p(tmpS21mag, tmpS21deg);

    Real = S22magEdit->text().toDouble();
    Imag = S22degEdit->text().toDouble();
    c2p(Real, Imag);
    setS22LineEdits(Real, Imag);
  } else { // cartesian
    S11sLabel->setText("+j");
    S12sLabel->setText("+j");
    S21sLabel->setText("+j");
    S22sLabel->setText("+j");
    S11uLabel->setText(" ");
    S12uLabel->setText(" ");
    S21uLabel->setText(" ");
    S22uLabel->setText(" ");

    double Mag = S11magEdit->text().toDouble();
    double Phase = S11degEdit->text().toDouble();
    p2c(Mag, Phase);
    setS11LineEdits(Mag, Phase);

    Mag = S12magEdit->text().toDouble();
    Phase = S12degEdit->text().toDouble();
    p2c(Mag, Phase);
    setS12LineEdits(Mag, Phase);

    Mag = S21magEdit->text().toDouble();
    Phase = S21degEdit->text().toDouble();
    p2c(Mag, Phase);
    setS21LineEdits(Mag, Phase);
    // convert also temp entries for future use
    p2c(tmpS21mag, tmpS21deg);

    Mag = S22magEdit->text().toDouble();
    Phase = S22degEdit->text().toDouble();
    p2c(Mag, Phase);
    setS22LineEdits(Mag, Phase);
  }
}

// -----------------------------------------------------------------------
// Is called if the user changed the impedance. -> The reflexion
// coefficient is calculated.
void MatchDialog::slotImpedanceChanged(const QString &) {
  if (TwoCheck->isChecked())
    return;

  double Z0 = Ref1Edit->text().toDouble();
  double Real = S21magEdit->text().toDouble();
  double Imag = S21degEdit->text().toDouble();

  if (FormatCombo->currentIndex()) { // entries in polar format
    p2c(Real, Imag);
    z2r(Real, Imag, Z0);
    c2p(Real, Imag);
  } else {
    z2r(Real, Imag, Z0);
  }

  setS11LineEdits(Real, Imag);
}

// -----------------------------------------------------------------------
// Is called if the user changed the reflexion coefficient. -> The impedance
// is calculated.
void MatchDialog::slotReflexionChanged(const QString &) {
  if (TwoCheck->isChecked())
    return;

  double Z0 = Ref1Edit->text().toDouble();
  double Real = S11magEdit->text().toDouble();
  double Imag = S11degEdit->text().toDouble();

  if (FormatCombo->currentIndex()) { // entries in polar format
    p2c(Real, Imag);
    r2z(Real, Imag, Z0);
    c2p(Real, Imag);
  } else {
    r2z(Real, Imag, Z0);
  }
  setS21LineEdits(Real, Imag);
}

void MatchDialog::setS11LineEdits(double Real, double Imag) {
  S11magEdit->blockSignals(true); // do not call slot for "textChanged"
  S11magEdit->setText(QString::number(Real));
  S11magEdit->blockSignals(false);
  S11degEdit->blockSignals(true); // do not call slot for "textChanged"
  S11degEdit->setText(QString::number(Imag));
  S11degEdit->blockSignals(false);
}

void MatchDialog::setS12LineEdits(double Real, double Imag) {
  S12magEdit->setText(QString::number(Real));
  S12degEdit->setText(QString::number(Imag));
}

void MatchDialog::setS21LineEdits(double Real, double Imag) {
  S21magEdit->blockSignals(true); // do not call slot for "textChanged"
  S21magEdit->setText(QString::number(Real));
  S21magEdit->blockSignals(false);
  S21degEdit->blockSignals(true); // do not call slot for "textChanged"
  S21degEdit->setText(QString::number(Imag));
  S21degEdit->blockSignals(false);
}

void MatchDialog::setS22LineEdits(double Real, double Imag) {
  S22magEdit->setText(QString::number(Real));
  S22degEdit->setText(QString::number(Imag));
}

// -----------------------------------------------------------------------
// Is called if the "Create"-button is pressed.
void MatchDialog::slotButtCreate() {
  double Z1 = Ref1Edit->text().toDouble(); // Port 1 impedance
  double Z2 = Ref2Edit->text().toDouble(); // Port 2 impedance
  double Freq = FrequencyEdit->text().toDouble() *
                pow(10.0, 3.0 * UnitCombo->currentIndex());

  // S matrix
  double S11real = S11magEdit->text().toDouble();
  double S11imag = S11degEdit->text().toDouble();
  double S12real = S12magEdit->text().toDouble();
  double S12imag = S12degEdit->text().toDouble();
  double S21real = S21magEdit->text().toDouble();
  double S21imag = S21degEdit->text().toDouble();
  double S22real = S22magEdit->text().toDouble();
  double S22imag = S22degEdit->text().toDouble();
  if (FormatCombo->currentIndex()) { // are they polar ?
    p2c(S11real, S11imag);
    p2c(S12real, S12imag);
    p2c(S21real, S21imag);
    p2c(S22real, S22imag);
  }

  bool BalancedStubs = BalancedCheck->isChecked();
  bool micro_syn = MicrostripCheck->isChecked(); // Microstrip implementation?
  bool SP_block = AddSPBlock->isChecked();       // Add S-parameter block?
  bool open_short =
      OpenRadioButton
          ->isChecked(); // Open stub or short circuit stub configuration
  int order = OrderEdit->text().toInt() +
              1; // Order of the multisection lambda/4 matching
  bool success = true;
  double gamma_MAX =
      MaxRippleEdit->text()
          .toDouble(); // Maximum ripple (Chebyshev weighting only)

  tSubstrate Substrate;
  if (micro_syn) // In case microstrip implementation is selected. This reads
                 // the substrate properties given by the user
  {
    Substrate.er = RelPermCombo->currentText().section("  ", 0, 0).toDouble();
    Substrate.height = SubHeightEdit->text().toDouble() / 1e3;
    Substrate.thickness = thicknessEdit->text().toDouble() / 1e6;
    Substrate.tand = tanDEdit->text().toDouble();
    Substrate.resistivity = ResistivityEdit->text().toDouble();
    Substrate.roughness = 0.0;
    Substrate.minWidth = minWEdit->text().toDouble() / 1e3;
    Substrate.maxWidth = maxWEdit->text().toDouble() / 1e3;
  }

  if (TwoCheck->isChecked()) { // two-port matching ?
    // determinant of S-parameter matrix
    double DetReal = S11real * S22real - S11imag * S22imag - S12real * S21real +
                     S12imag * S21imag;
    double DetImag = S11real * S22imag + S11imag * S22real - S12real * S21imag -
                     S12imag * S21real;
    success =
        calc2PortMatch(S11real, S11imag, S22real, S22imag, DetReal, DetImag, Z1,
                       Z2, Freq, micro_syn, SP_block, open_short, Substrate,
                       order, gamma_MAX, BalancedStubs);
  } else {
    success = calcMatchingCircuit(S11real, S11imag, Z1, Freq, micro_syn,
                                  SP_block, open_short, Substrate, order,
                                  gamma_MAX, BalancedStubs);
  }

  QucsMain->slotEditPaste(success);
  accept();
}

// -----------------------------------------------------------------------
// transform real/imag into mag/deg (cartesian to polar)
void MatchDialog::c2p(double &Real, double &Imag) {
  double Real_ = Real;
  Real = sqrt(Real * Real + Imag * Imag); // magnitude
  Imag = 180.0 / pi * atan2(Imag, Real_); // phase in degree
}

// -----------------------------------------------------------------------
// transform mag/deg into real/imag (polar to cartesian)
void MatchDialog::p2c(double &Real, double &Imag) {
  double Real_ = Real;
  Real = Real_ * cos(Imag * pi / 180.0); // real part
  Imag = Real_ * sin(Imag * pi / 180.0); // imaginary part
}

// -----------------------------------------------------------------------
// transform reflexion coefficient into impedance
void MatchDialog::r2z(double &Real, double &Imag, double Z0) {
  double tmp = Z0 / ((1.0 - Real) * (1.0 - Real) + Imag * Imag);
  Real = (1.0 - Real * Real - Imag * Imag) * tmp;
  Imag *= 2.0 * tmp;
}

// -----------------------------------------------------------------------
// transform impedance into reflexion coefficient
void MatchDialog::z2r(double &Real, double &Imag, double Z0) {
  double tmp = (Real + Z0) * (Real + Z0) + Imag * Imag;
  Real = (Real * Real + Imag * Imag - Z0 * Z0) / tmp;
  Imag *= 2.0 * Z0 / tmp;
}

// -----------------------------------------------------------------------
// This function calculates a LC matching section
QString MatchDialog::calcMatchingLC(double r_real, double r_imag, double Z0,
                                    double Freq) {
  double Zreal = r_real, Zimag = r_imag;
  r2z(Zreal, Zimag, Z0);

  if (Zreal < 0.0) {
    if (Zreal < -1e-13) {
      QMessageBox::critical(
          0, tr("Error"),
          tr("Real part of impedance must be greater zero,\nbut is %1 !")
              .arg(Zreal));
      return QString(""); // matching not possible
    }

    // In high-Q circuits, Zreal often becomes somewhat about -1e-16
    // because of numerical inaccuracy.
    Zreal = 0.0;
  }

  double X1, X2, Omega = 2.0 * pi * Freq;
  QString Str;
  if (r_real < 0) {
    // ...................................................
    // first serial than parallel component (possible if Zreal <= Z0)
    Str = "sp";
    X1 = sqrt(Zreal * (Z0 - Zreal));
    if (Zimag < 0.0)
      X1 *= -1.0; // always use shortest matching path
    X1 -= Zimag;

    // parallel component
    X2 = (Zimag + X1) / (Zreal * Zreal + (Zimag + X1) * (Zimag + X1));
  } else {

    Str = "ps";
    X1 = Zreal + Zimag * Zimag / Zreal - Z0;
    // ...................................................
    // first parallel than serial component (possible if X >= 0.0)
    X1 = sqrt(Z0 * X1);
    if (Zimag > 0.0)
      X1 *= -1.0; // always use shortest matching path

    // parallel component
    X2 = Zimag / (Zreal * Zreal + Zimag * Zimag) + X1 / (Z0 * Z0 + X1 * X1);
  }

  // Circuit topology and values
  QString laddercode = "", series_element, shunt_element;
  // serial component
  if (X1 < 0.0) // capacitance ?
    series_element = QString("CS:%1;").arg(-1.0 / Omega / X1);
  else // inductance
    series_element = QString("LS:%1;").arg(X1 / Omega);

  // parallel component
  if (X2 < 0.0) // inductance ?
    shunt_element = QString("LP:%1;").arg(-1.0 / Omega / X2);
  else // capacitance
    shunt_element = QString("CP:%1;").arg(X2 / Omega);

  (r_real < 0) ? laddercode = shunt_element + series_element
               : laddercode = series_element + shunt_element;
  return laddercode;
}

// -----------------------------------------------------------------------
// This function calls the specific matching network function so as to get the
// desired matching topology Returns true if the synthesis worked as expected or
// false if it wasn't
bool MatchDialog::calcMatchingCircuit(double S11real, double S11imag, double Z0,
                                      double Freq, bool micro_syn,
                                      bool SP_Block, bool open_short,
                                      tSubstrate Substrate, int order,
                                      double gamma_MAX, bool BalancedStubs) {
  QString laddercode;
  switch (TopoCombo->currentIndex()) {
  case 0: // LC
    laddercode = calcMatchingLC(S11real, S11imag, Z0,
                                Freq); // It calculates the LC matching circuit.
    break;
  case 1: // Single stub
    laddercode =
        calcSingleStub(S11real, S11imag, Z0, Freq, open_short, BalancedStubs);
    break;
  case 2: // Double stub
    laddercode =
        calcDoubleStub(S11real, S11imag, Z0, Freq, open_short, BalancedStubs);
    break;
  case 3: // Quarter wave cascaded sections
    (BinRadio->isChecked())
        ? laddercode = calcBinomialLines(S11real, S11imag, Z0, order, Freq)
        : laddercode =
              calcChebyLines(S11real, S11imag, Z0, gamma_MAX, order, Freq);
    break;
  case 4: // Cascaded LC sections
    laddercode =
        calcMatchingCascadedLCSections(S11real, S11imag, Z0, Freq, order - 1);
    break;
  case 5: // Lambda/8 + Lambda/4 impedance transformer
          // Reference: Inder J. Bahl. "Fundamentals of RF and microwave
          // transistor amplifiers". John Wiley and Sons. 2009. Pages 159-160
    laddercode = calcMatchingLambda8Lambda4(S11real, S11imag, Z0, Freq);
    break;
  }

  if (laddercode.isEmpty())
    return false;

  double RL = S11real, XL = S11imag;
  QString wirestr = "";
  QString componentstr = "";
  QString paintingstr = "";
  int x_pos = 0;

  r2z(RL, XL, Z0);

  if (SP_Block) {
    laddercode.append("S2P:Freq;");
    laddercode.prepend(QString("P1:%1;").arg(Z0));
    laddercode.append(QString("ZL:%1#%2;").arg(RL).arg(XL));
  } else // Add tags instead of a S-param simulation
  {
    laddercode.prepend("LBL:Port 1;");
    laddercode.append("LBL:Port 2;");
  }
  SchematicParser(laddercode, x_pos, Freq, Substrate, micro_syn);
  return true; // The schematic was successfully created
}

// -----------------------------------------------------------------------
// Fundamental calculations for concurrent 2-port matching.
QString MatchDialog::calcBiMatch(double S11real, double S11imag, double S22real,
                                 double S22imag, double DetReal, double DetImag,
                                 double Z0, double Freq, bool open_short,
                                 double gamma_MAX, int order,
                                 bool BalancedStubs) {
  double B = 1.0 + S11real * S11real + S11imag * S11imag - S22real * S22real -
             S22imag * S22imag - DetReal * DetReal - DetImag * DetImag;
  double Creal = S11real - S22real * DetReal - S22imag * DetImag;
  double Cimag = S22real * DetImag - S11imag - S22imag * DetReal;
  double Cmag = 2.0 * (Creal * Creal + Cimag * Cimag);
  Creal /= Cmag;
  Cimag /= Cmag;

  double Rreal = B * B - 2.0 * Cmag;
  double Rimag;
  if (Rreal < 0.0) {
    Rimag = Cimag * B - Creal * sqrt(-Rreal);
    Rreal = Creal * B + Cimag * sqrt(-Rreal);
  } else {
    Rreal = B - sqrt(Rreal);
    Rimag = Cimag * Rreal;
    Rreal *= Creal;
  }

  QString laddercode;

  switch (TopoCombo->currentIndex()) // Matches both the input and the output
                                     // port to external sources (typically, 50
                                     // Ohms)
  {
  case 0: // LC
    laddercode = calcMatchingLC(Rreal, -Rimag, Z0, Freq);
    break;
  case 1: // Single stub
    laddercode =
        calcSingleStub(Rreal, -Rimag, Z0, Freq, open_short, BalancedStubs);
    break;
  case 2: // Double stub
    laddercode =
        calcDoubleStub(Rreal, -Rimag, Z0, Freq, open_short, BalancedStubs);
    break;
  case 3: // Quarter wave cascaded sections
    (BinRadio->isChecked())
        ? laddercode = calcBinomialLines(Rreal, -Rimag, Z0, order, Freq)
        : laddercode =
              calcChebyLines(Rreal, -Rimag, Z0, gamma_MAX, order, Freq);
    break;
  case 4: // Cascaded LC sections
    laddercode = calcMatchingCascadedLCSections(Rreal, -Rimag, Z0, Freq, order);
    break;
  case 5: // Lambda/8 + Lambda/4 transformer
    laddercode = calcMatchingLambda8Lambda4(Rreal, -Rimag, Z0, Freq);
    break;
  }

  return laddercode;
}

// -----------------------------------------------------------------------
// This function calculates both the input and the output matching networks. In
// this sense, it calls 'calcBiMatch' function to get the input/output
// impedance. Then it synthesize the network according to the user choice
bool MatchDialog::calc2PortMatch(double S11real, double S11imag, double S22real,
                                 double S22imag, double DetReal, double DetImag,
                                 double Z1, double Z2, double Freq,
                                 bool microsyn, bool SP_Block, bool open_short,
                                 tSubstrate Substrate, int order,
                                 double gamma_MAX, bool BalancedStubs) {
  // Input port network
  // The result is a string which gives the structure of the matching network
  QString InputLadderCode =
      calcBiMatch(S11real, S11imag, S22real, S22imag, DetReal, DetImag, Z1,
                  Freq, open_short, gamma_MAX, order, BalancedStubs);
  if (InputLadderCode.isEmpty())
    return false; // Synthesis error

  // Output port network
  QString OutputLadderCode =
      calcBiMatch(S22real, S22imag, S11real, S11imag, DetReal, DetImag, Z2,
                  Freq, open_short, gamma_MAX, order, BalancedStubs);
  if (OutputLadderCode.isEmpty())
    return false; // Synthesis error
  else
    OutputLadderCode = flipLadderCode(OutputLadderCode);

  if (SP_Block) {
    InputLadderCode.prepend(QString("S2P:%1;").arg(
        Freq)); // Add the s-param simulation to the circuit code
    InputLadderCode.prepend(QString("P1:%1;").arg(Z1)); // Port 1
    OutputLadderCode.append(QString("P2:%1;").arg(Z2)); // Port 2
  } else { // Use labels instead of the S-param simulation
    InputLadderCode.prepend(QString("LBL:Port 1;")); // Port 1
    OutputLadderCode.append(QString("LBL:Port 2;")); // Port 2
  }
  QString laddercode = InputLadderCode + QString("DEV:0") + OutputLadderCode;
  int x_pos = 0;
  SchematicParser(laddercode, x_pos, Freq, Substrate, microsyn);

  return true;
}

// This function flips the ladder structure so as to correctly process the
// output matching network
QString MatchDialog::flipLadderCode(QString laddercode) {
  QStringList strlist = laddercode.split(";"), auxlist;
  std::reverse(strlist.begin(), strlist.end());

  QString flipped_laddercode = "";
  // Build the flipped ladder code
  for (int i = 0; i < strlist.length(); i++) {
    flipped_laddercode += strlist.at(i) + ";";
  }
  return flipped_laddercode;
}

// FUNCTIONS FOR THE MICROSTRIP LINE SYNTHESIS. JUST COPIED FROM THE QUCS-FILTER
// TOOL
/////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_ERROR 1e-7
void calcMicrostrip(tSubstrate *substrate, double width, double freq,
                    double &er_eff, double &zl) {
  double a, b;
  double h = substrate->height;
  double t = substrate->thickness;
  double er = substrate->er;
  double Wh = width / h;
  t /= h;

  // quasi-static models by Hammerstad

  double w1 = Wh;
  if (t > 1e-100) { // width correction due to metal thickness?
    a = coth(sqrt(6.517 * Wh));
    b = t / pi * log(1.0 + 10.873127 / t / a / a);
    w1 += b;
    Wh += 0.5 * b * (1.0 + sech(sqrt(er - 1.0)));
  }

  // relative effective permittivity
  a = Wh * Wh;
  b = a * a;
  er_eff = -0.564 * pow((er - 0.9) / (er + 3.0), 0.053);
  er_eff *= 1.0 + log((b + a / 2704.0) / (b + 0.432)) / 49.0 +
            log(1.0 + a * Wh / 5929.741) / 18.7;
  er_eff = (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * pow(1.0 + 10.0 / Wh, er_eff);

  // characteristic impedance
  zl = 6.0 + 0.2831853 * exp(-pow(30.666 / Wh, 0.7528));
  zl = Z_FIELD / 2.0 / pi * log(zl / Wh + sqrt(1.0 + 4.0 / Wh / Wh));

  // characteristic impedance (same again for "w1")
  a = 6.0 + 0.2831853 * exp(-pow(30.666 / w1, 0.7528));
  a = Z_FIELD / 2.0 / pi * log(a / w1 + sqrt(1.0 + 4.0 / w1 / w1));

  a /= zl;
  zl /= sqrt(er_eff);
  er_eff *= a * a;

  // dispersion models by Kirschning

  freq *= h / 1e6; // normalize frequency into GHz*mm

  // relative effective permittivity
  a = 0.0363 * exp(-4.6 * Wh) * (1.0 - exp(-pow(freq / 38.7, 4.97)));
  a *= 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));
  a = pow((0.1844 + a) * freq, 1.5763);
  a *= 0.27488 + Wh * (0.6315 + 0.525 / pow(1.0 + 0.0157 * freq, 20.0)) -
       0.065683 * exp(-8.7513 * Wh);
  a *= 0.33622 * (1.0 - exp(-0.03442 * er));
  double er_freq = er - (er - er_eff) / (1.0 + a);

  // characteristic impedance
  a = -0.03891 * pow(er, 1.4);
  b = -0.267 * pow(Wh, 7.0);
  double R7 = 1.206 - 0.3144 * exp(a) * (1.0 - exp(b));

  a = 0.016 + pow(0.0514 * er, 4.524);
  b = pow(freq / 28.843, 12.0);
  a = 5.086 * a * b / (0.3838 + 0.386 * a) / (1.0 + 1.2992 * b);
  b = -22.2 * pow(Wh, 1.92);
  a *= exp(b);
  b = pow(er - 1.0, 6.0);
  double R9 = a * b / (1.0 + 10.0 * b);

  a = 4.766 * exp(-3.228 * pow(Wh, 0.641)); // = R3
  a = 1.0 + 1.275 * (1.0 - exp(-0.004625 * a * pow(er, 1.674) *
                               pow(freq / 18.365, 2.745))); // = R8

  b = 0.9408 * pow(er_freq, a) - 0.9603; // = R13
  b /= (0.9408 - R9) * pow(er_eff, a) - 0.9603;
  R9 = b; // = R13 / R14

  a = 0.00044 * pow(er, 2.136) + 0.0184; // = R10
  a *= 0.707 * pow(freq / 12.3, 1.097);  // = R15
  a = exp(-0.026 * pow(freq, 1.15656) - a);
  b = pow(freq / 19.47, 6.0);
  b /= 1.0 + 0.0962 * b;                                            // = R11
  b = 1.0 + 0.0503 * er * er * b * (1.0 - exp(-pow(Wh / 15, 6.0))); // = R16
  R7 *= (1.0 - 1.1241 * a / b / (1.0 + 0.00245 * Wh * Wh));         // = R17

  zl *= pow(R9, R7);
  er_eff = er_freq;
}

// -------------------------------------------------------------------
// Calculates the width 'width' and the relative effective permittivity 'er_eff'
// of a microstrip line. It uses an iterative search algorithm because
// synthesis equations doesn't exist.
void MatchDialog::getMicrostrip(double Z0, double freq, tSubstrate *substrate,
                                double &width, double &er_eff) {
  int iteration = 0; // iteration counter
  double Z0_current, Z0_result, increment;

  width = 1e-3; // start with 1mm

  do {
    // compute line parameters
    calcMicrostrip(substrate, width, freq, er_eff, Z0_current);

    if (fabs(Z0 - Z0_current) < MAX_ERROR)
      break; // wanted value was found

    increment = width / 100.0;
    width += increment;

    // compute line parameters
    calcMicrostrip(substrate, width, freq, er_eff, Z0_result);

    // Newton iteration: w(n+1) = w(n) - f(w(n))/f'(w(n))
    //   with f(w(n))  = Z0_current - Z0
    //   and  f'(w(n)) = (Z0_result - Z0_current) / increment
    width -= (Z0_current - Z0) / (Z0_result - Z0_current) * increment;
    if (width <= 0.0)
      width = increment;

    iteration++;
  } while (iteration < 150);
}
////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
// Calculates a matching network according to the stub+line method
// Reference: 'Microwave Engineering'. David Pozar. John Wiley and Sons. 4th
// Edition. Pg 234-241
QString MatchDialog::calcSingleStub(double r_real, double r_imag, double Z0,
                                    double Freq, bool open_short,
                                    bool BalancedStubs) {
  double t = 0, t1 = 0, t2 = 0;
  double dl, dl1, dl2, B;
  double B1, B2, d, lstub, ll;
  double lambda = SPEED_OF_LIGHT / (Freq);
  double RL = r_real, XL = r_imag;
  r2z(RL, XL, Z0);

  // Stub+line method formulas
  if (RL == Z0) {
    t = -XL / (2 * Z0);
    (t < 0) ? dl = (pi + atan(t)) / (2 * pi) : dl = (atan(t)) / (2 * pi);
    B = (RL * RL * t - (Z0 - XL * t) * (Z0 * t + XL)) /
        (Z0 * (RL * RL + (Z0 * t + XL) * (Z0 * t + XL)));
  } else {
    t1 = (XL + sqrt(((RL / Z0) * fabs((Z0 - RL) * (Z0 - RL) + XL * XL)))) /
         (RL - Z0);
    (t1 < 0) ? dl1 = (pi + atan(t1)) / (2 * pi) : dl1 = (atan(t1)) / (2 * pi);
    B1 = (RL * RL * t1 - (Z0 - XL * t1) * (Z0 * t1 + XL)) /
         (Z0 * (RL * RL + (Z0 * t1 + XL) * (Z0 * t1 + XL)));

    t2 = (XL - sqrt((RL * fabs((Z0 - RL) * (Z0 - RL) + XL * XL)) / (Z0))) /
         (RL - Z0);
    (t2 < 0) ? dl2 = (pi + atan(t2)) / (2 * pi) : dl2 = (atan(t2)) / (2 * pi);
    B2 = (RL * RL * t2 - (Z0 - XL * t2) * (Z0 * t2 + XL)) /
         (Z0 * (RL * RL + (Z0 * t2 + XL) * (Z0 * t2 + XL)));
  }

  if (t != 0) {
    d = dl * lambda;
    (open_short) ? ll = -(atan(B * Z0)) / (2 * pi)
                 : ll = (atan(1. / (B * Z0))) / (2 * pi);
    if ((open_short) && (ll < 0))
      ll += 0.5;
    if ((!open_short) && (ll > 0.5))
      ll -= 0.5;
    lstub = ll * lambda;
  }

  if (t1 != 0) {
    d = dl1 * lambda;
    (open_short) ? ll = -(atan(B1 * Z0)) / (2 * pi)
                 : ll = (atan(1. / (1. * B1 * Z0))) / (2 * pi);
    if ((open_short) && (ll < 0))
      ll += 0.5;
    if ((!open_short) && (ll > 0.5))
      ll -= 0.5;
    lstub = ll * lambda;

  } else {
    if (t2 != 0) {
      d = dl2 * lambda;
      (open_short) ? ll = -(atan(B2 * Z0)) / (2 * pi)
                   : ll = (atan(1. / (1. * B2 * Z0))) / (2 * pi);
      if ((open_short) && (ll < 0))
        ll += 0.5;
      if ((!open_short) && (ll > 0.5))
        ll -= 0.5;
      lstub = ll * lambda;
    }
  }

  if (BalancedStubs) { // Balanced stub implementation
    double K;
    (open_short) ? K = 0.5 : K = 2;
    lstub = (lambda / (2 * pi)) * atan(K * tan((2 * pi * lstub) / lambda));
    if (lstub < 0)
      lstub += 0.5 * lambda;
  }

  // String code
  QString laddercode;
  if ((open_short) && (!BalancedStubs))
    laddercode = QString("OL:%1#%2;TL:%1#%3;")
                     .arg(Z0)
                     .arg(lstub)
                     .arg(d); // Line + Open stub
  if ((open_short) && (BalancedStubs))
    laddercode = QString("OU:%1#%2;OL:%1#%2;TL:%1#%3;")
                     .arg(Z0)
                     .arg(lstub)
                     .arg(d); // Open circuit balanced stubs
  if ((!open_short) && (!BalancedStubs))
    laddercode = QString("SL:%1#%2;TL:%1#%3;")
                     .arg(Z0)
                     .arg(lstub)
                     .arg(d); // Line + Short circuited stub
  if ((!open_short) && (BalancedStubs))
    laddercode = QString("SU:%1#%2;SL:%1#%2;TL:%1#%3;")
                     .arg(Z0)
                     .arg(lstub)
                     .arg(d); // Short circuited balanced stubs

  return laddercode;
}

//--------------------------------------------------------------------------------
// Calculates a matching network according to the stub+line+stub method
// Reference: 'Microwave Engineering', David Pozar. John Wiley and Sons. 4th
// Edition. Pg 241-245
QString MatchDialog::calcDoubleStub(double r_real, double r_imag, double Z0,
                                    double Freq, bool open_short,
                                    bool BalancedStubs) {
  double RL = r_real, XL = r_imag;
  r2z(RL, XL, Z0);
  double Y0 = 1. / Z0;
  double GL = (1 / ((RL * RL) + (XL * XL))) * RL;
  double BL = -(1 / ((RL * RL) + (XL * XL))) * XL;
  double lambda = SPEED_OF_LIGHT / Freq;
  double beta = (2 * pi) / lambda;
  double d = lambda / 8;
  double t = tan(beta * d);
  double ll1, ll2;

  // Double stub method formulas
  if (GL > Y0 * ((1 + t * t) / (2 * t * t))) // Not every load can be match
                                             // using the double stub technique.
  {
    QMessageBox::warning(0, tr("Error"),
                tr("It is not possible to match this load using the double stub method"));
    return QString("");
  }

  // Stubs susceptance
  double B11 = -BL + (Y0 + sqrt((1 + t * t) * GL * Y0 - GL * GL * t * t)) /
                         (t); // 1st solution
  // double B12 = -BL + (Y0 - sqrt((1+t*t)*GL*Y0 - GL*GL*t*t))/(t); // 2nd
  // solution

  double B21 =
      ((Y0 * sqrt((1 + t * t) * GL * Y0 - GL * GL * t * t)) + GL * Y0) /
      (GL * t); // 1st solution
  // double B22 = ((-Y0*sqrt((1+t*t)*GL*Y0 - GL*GL*t*t)) + GL*Y0)/(GL*t);// 2nd
  // solution

  // Open circuit solution
  (open_short) ? ll1 = (atan(B11 * Z0)) / (2 * pi)
               : ll1 = -(atan(1. / (1. * B11 * Z0))) / (2 * pi);
  (open_short) ? ll2 = (atan(B21 * Z0)) / (2 * pi)
               : ll2 = -(atan(1. / (1. * B21 * Z0))) / (2 * pi);

  if (ll1 < 0)
    ll1 += 0.5;
  if (ll2 < 0)
    ll2 += 0.5;
  if ((!open_short) && (ll1 > 0.5))
    ll1 -= 0.5;
  if ((!open_short) && (ll2 > 0.5))
    ll2 -= 0.5;

  double lstub1 = ll1 * lambda, lstub2 = ll2 * lambda;

  if (BalancedStubs) { // Balanced stub implementation
    double K;
    (open_short) ? K = 0.5 : K = 2;
    lstub1 = (lambda / (2 * pi)) * atan(K * tan((2 * pi * lstub1) / lambda));
    lstub2 = (lambda / (2 * pi)) * atan(K * tan((2 * pi * lstub2) / lambda));
    if (lstub1 < 0)
      lstub1 += 0.5 * lambda;
    if (lstub2 < 0)
      lstub2 += 0.5 * lambda;
  }

  QString laddercode;
  if ((open_short) && (BalancedStubs))
    laddercode = QString("OU:%1#%2;OL:%1#%2;TL:%1#%3;OU:%1#%4;OL:%1#%4;")
                     .arg(Z0)
                     .arg(lstub2)
                     .arg(d)
                     .arg(lstub1);
  if ((open_short) && (!BalancedStubs))
    laddercode = QString("OL:%1#%2;TL:%1#%3;OL:%1#%4;")
                     .arg(Z0)
                     .arg(lstub2)
                     .arg(d)
                     .arg(lstub1);
  if ((!open_short) && (BalancedStubs))
    laddercode = QString("SU:%1#%2;SL:%1#%2;TL:%1#%3;SU:%1#%4;SL:%1#%4;")
                     .arg(Z0)
                     .arg(lstub2)
                     .arg(d)
                     .arg(lstub1);
  if ((!open_short) && (!BalancedStubs))
    laddercode = QString("SL:%1#%2;TL:%1#%3;SL:%1#%4;")
                     .arg(Z0)
                     .arg(lstub2)
                     .arg(d)
                     .arg(lstub1);

  return laddercode;
}

// -----------------------------------------------------------------------------------------------
// Binomial coefficients using the multiplicative formula (more efficient than
// the recursive one)
int BinomialCoeffs(int n, int k) {
  double coeff = 1;
  for (int i = 1; i <= k; i++) {
    coeff *= (n + (1 - i)) / (1.0 * i);
  }
  return (int)coeff;
}

//-----------------------------------------------------------------------------------
// This function calculates a multistage lambda/4 matching using binomial
// weigthing Reference: 'Microwave Engineering'. David Pozar. John Wiley and
// Sons. 4th Edition. Pg 252-256
QString MatchDialog::calcBinomialLines(double r_real, double r_imag, double Z0,
                                       int order, double Freq) {
  double RL = r_real, XL = r_imag;
  r2z(RL, XL, Z0);
  if (RL == 0) {
    QMessageBox::warning(
        0, QObject::tr("Error"),
        QObject::tr("The load has not resistive part. It cannot be matched "
                    "using the quarter wavelength method"));
    return NULL;
  }
  if (XL != 0) {
    QMessageBox::warning(0, QObject::tr("Warning"),
                         QObject::tr("Reactive loads cannot be matched. Only "
                                     "the real part will be matched"));
  }
  double l4 = SPEED_OF_LIGHT / (4. * Freq);
  double Ci, Zi, Zaux = Z0;
  QString laddercode;
  for (int i = 1; i < order; i++) {
    Ci = BinomialCoeffs(order - 1, i - 1);
    Zi = exp(log(Zaux) + (Ci / pow(2, order - 1)) * log(RL / Z0));
    Zaux = Zi;
    laddercode += QString("TL:%1#%2;").arg(Zi).arg(l4);
  }
  return laddercode;
}

//-----------------------------------------------------------------------------------
// This function calculates a multistage lambda/4 matching using the Chebyshev
// weigthing. Reference 'Microwave Engineering'. David Pozar. John Wiley and
// Sons. 4th Edition. Pg 256-261
QString MatchDialog::calcChebyLines(double r_real, double r_imag, double Z0,
                                    double gamma, int order, double Freq) {
  int N = order - 1; // Number of sections
  if (N > 7)         // So far, it is only available Chebyshev weighting up to 7
             // sections. Probably, it makes no sense to use a higher number of
             // sections because of the losses
  {
    QMessageBox::warning(
        0, QObject::tr("Error"),
        QObject::tr("Chebyshev weighting for N>7 is not available"));
    return QString("");
  }
  QString laddercode;
  double RL = r_real, XL = r_imag;
  r2z(RL, XL,
      Z0); // Calculation of the load impedance given the reflection coeffient
  double sec_theta_m; // =
                      // cosh((1/(1.*N))*acosh((1/gamma)*fabs((RL-Z0)/(Z0+RL)))
                      // );
  // double sec_theta_m = cosh((1/(1.*N))*acosh(fabs(log(RL/Z0)/(2*gamma))) );
  (fabs(log(RL / Z0) / (2 * gamma)) < 1)
      ? sec_theta_m = 0
      : sec_theta_m =
            cosh((1 / (1. * N)) * acosh(fabs(log(RL / Z0) / (2 * gamma))));

  double w[N];

  switch (N) // The weights are calculated by equating the reflection coeffient
             // formula to the N-th Chebyshev polinomial
  {
  case 1:
    w[0] = sec_theta_m;
    break;
  case 2:
    w[0] = sec_theta_m * sec_theta_m;
    w[1] = 2 * (sec_theta_m * sec_theta_m - 1);
    break;
  case 3:
    w[0] = pow(sec_theta_m, 3);
    w[1] = 3 * (pow(sec_theta_m, 3) - sec_theta_m);
    w[2] = w[1];
    break;
  case 4:
    w[0] = pow(sec_theta_m, 4);
    w[1] = 4 * sec_theta_m * sec_theta_m * (sec_theta_m * sec_theta_m - 1);
    w[2] = 2 * (1 - 4 * sec_theta_m * sec_theta_m + 3 * pow(sec_theta_m, 4));
    w[3] = w[1];
    break;
  case 5:
    w[0] = pow(sec_theta_m, 5);
    w[1] = 5 * (pow(sec_theta_m, 5) - pow(sec_theta_m, 3));
    w[2] =
        10 * pow(sec_theta_m, 5) - 15 * pow(sec_theta_m, 3) + 5 * sec_theta_m;
    w[3] = w[2];
    w[4] = w[1];
    break;
  case 6:
    w[0] = pow(sec_theta_m, 6);
    w[1] = 6 * pow(sec_theta_m, 4) * (sec_theta_m * sec_theta_m - 1);
    w[2] = 15 * pow(sec_theta_m, 6) - 24 * pow(sec_theta_m, 4) +
           9 * sec_theta_m * sec_theta_m;
    w[3] = 2 * (10 * pow(sec_theta_m, 6) - 18 * pow(sec_theta_m, 4) +
                9 * sec_theta_m * sec_theta_m - 1);
    w[4] = w[2];
    w[5] = w[1];
    break;
  case 7:
    w[0] = pow(sec_theta_m, 7);
    w[1] = 7 * pow(sec_theta_m, 5) * (sec_theta_m * sec_theta_m - 1);
    w[2] = 21 * pow(sec_theta_m, 7) - 35 * pow(sec_theta_m, 5) +
           14 * pow(sec_theta_m, 3);
    w[3] = 35 * pow(sec_theta_m, 7) - 70 * pow(sec_theta_m, 5) +
           42 * pow(sec_theta_m, 3) - 7 * sec_theta_m;
    w[4] = w[3];
    w[5] = w[2];
    w[6] = w[1];
    break;
  }
  double l4 = SPEED_OF_LIGHT / (4. * Freq);
  double Zaux = Z0, Zi;
  for (int i = 0; i < N; i++) {
    (RL < Z0) ? Zi = exp(log(Zaux) - gamma * w[i])
              : Zi = exp(log(Zaux) + gamma * w[i]); // When RL<Z0, Z_{i}<Z_{i-1}
    Zaux = Zi;
    laddercode += QString("TL:%1#%2;").arg(Zi).arg(l4);
  }
  return laddercode;
}

//--------------------------------------------------------------------------
// It calculates a cascaded LC matching network (only real impedances)
// Reference: Inder J. Bahl. "Fundamentals of RF and microwave transistor
// amplifiers". John Wiley and Sons. 2009. Pages 169 - 170
QString MatchDialog::calcMatchingCascadedLCSections(double r_real,
                                                    double r_imag, double Z0,
                                                    double Freq, int N) {
  double RL = r_real, XL = r_imag, RS = Z0;
  double w = 2 * pi * Freq, Q, C, L;
  r2z(RL, XL, Z0);
  double Raux, R, R1, R2;
  QString s = "";

  if (RL == 0) {
    QMessageBox::warning(
        0, QObject::tr("Error"),
        QObject::tr("The load is reactive. It cannot be matched "
                    "using the quarter wavelength method"));
    return NULL;
  }
  if (XL != 0) {
    QMessageBox::warning(0, QObject::tr("Warning"),
                         QObject::tr("Reactive loads cannot be matched. Only "
                                     "the real part will be matched"));
  }

  if (RL > RS)//The design equations were tailored for the RS > RL case. In case of RL > RS, the ports impedance are swapped
    R1 = RL, R2 = RS;
  else
    R1 = RS, R2 = RL;

  Raux = R1;
  for (int i = 0; i < N - 1; i++) {
    R = pow(R1, 1. * (N - (i + 1)) / N) * pow(R2, 1. * (i + 1) / N);
    Q = sqrt(Raux / R - 1);
    C = Q / (w * Raux);
    L = Q * R / w;
    s += QString("CP:%1;LS:%2;").arg(C).arg(L);
    Raux = R;
  }

  Q = sqrt(R / R2 - 1);
  C = Q / (w * R);
  L = Q * R2 / w;
  s += QString("CP:%1;LS:%2;").arg(C).arg(L);

  if (RL > RS) // Flip string
  {
    QString temp = "";
    QStringList strlist = s.split(";");
    for (int i = strlist.count() - 1; i >= 0; i--)
      temp += strlist.at(i) + ";";
    s = temp;
  }

  return s;
}

//--------------------------------------------------------------------------
// It calculates a lambda/8 + lambda/4 transformer to match a complex load to a
// real source
QString MatchDialog::calcMatchingLambda8Lambda4(double r_real, double r_imag,
                                                double Z0, double Freq) {
  double RL = r_real, XL = r_imag;
  double l4 = SPEED_OF_LIGHT / (4. * Freq);
  double l8 = .5 * l4;
  r2z(RL, XL, Z0);
  double Zmm = sqrt(RL * RL + XL * XL);
  double Zm = sqrt((Z0 * RL * Zmm) / (Zmm - XL));
  return QString("TL:%1#%2;TL:%3#%4;").arg(Zm).arg(l4).arg(Zmm).arg(l8);
}

// Given a string code of inductors, capacitors and transmission lines, it
// generates the Qucs network. Notice that the schematic is split into three
// parts: components, wires and paintings, all of them are passed by reference.
void MatchDialog::SchematicParser(QString laddercode, int &x_pos, double Freq,
                                  tSubstrate Substrate, bool microsyn) {
  QStringList strlist = laddercode.split(";");//Slipt the string code to get the components
  QString component, tag, label;
  qDebug() << laddercode;
  double value, value2, er, width;
  int x_series = 120, x_shunt = 20; // x-axis spacing depending on whether the
                                    // component is placed in a series or shunt
                                    // configuration

  // Clear schematic strings
  QString componentstr = "", wirestr = "", paintingstr = "";

  // The string format is as follows: "XX<value>;XX<value2>;...XX<valueN>;"
  // where XX, YY, ZZ define the type of component and its configuration.
  //    LS: Series inductance
  //    CS: Series capacitor
  //    LP: Shunt inductance
  //    CP: Shunt capacitor
  //    TL: Series transmission line
  //    OU: Open stub (facing up)
  //    OL: Open stub (facing down)
  //    SU: Short circuited stub (facing up)
  //    SL: Short circuited stub (facing down)
  //    P1: Port 1
  //    LBL: LBL
  //    P2: Port 2
  //    DEV: Device label
  //    S2P: S-param simulation block

  for (int i = 0; i < strlist.count(); i++) {
    // Each token of the string descriptor has the following format:
    // 'tag:<value>;''tag:<value1>#<value2>;'
    // First, extract the tag
    component = strlist.at(i);
    int index_colon = component.indexOf(":");
    tag = component.mid(0, index_colon);

    // Now we remove the tag and the colon from the string
    component.remove(0, index_colon + 1);
    // At this point, we have the component parameters. In case of having more
    // than one parameter, they are separated by #
    int index = component.indexOf("#");
    // Transmission lines are characterised by its impedance and its length
    // whereas capacitors and inductors only depend on
    // its capacitance and inductance, respectively. So, the code below is aimed
    // to handle such difference
    if (index != -1) // The component has two values
    {
      value = component.mid(0, index).toDouble();
      value2 = component.mid(index + 1).toDouble();
    } else {
      if (!tag.compare("LBL")) // The value is a string
      {
        label = component;
      } else // The value is a double number. For a clearer representation in
             // the schematic, it will be shown with a limited precision
      {
        value = component.toDouble();
      }
    }

    // The following if-else structure is responsible for placing the
    // components, wires and paintings in the schematic
    if (!tag.compare("P1")) // Port 1 component
    {
      componentstr += QString("<Pac P1 1 %2 -30 18 -26 0 1 \"1\" 1 \"%1\" 1 "
                              "\"0 dBm\" 0 \"1 GHz\" 0>\n")
                          .arg(misc::num2str(value, 3, "Ohm")) // reference impedance
                          .arg(x_pos);
      componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
      wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos);
      wirestr += QString("<%1 -120 %2 -120>\n").arg(x_pos).arg(x_pos + 120);
      x_pos += 120;
    } else if (!tag.compare("LBL")) // Label
    {
      paintingstr += QString("<Text %1 -150 12 #000000 0 \"%2\">\n")
                         .arg(x_pos)
                         .arg(component); // Add 'Port 1' or 'Port 2' label
      x_pos += 50;
    } else if (!tag.compare("P2")) // Port 2 component (it need to be different
                                   // from P1 because of the wiring)
    {
      x_pos += 100;
      componentstr += QString("<Pac P2 1 %2 -30 18 -26 0 1 \"1\" 1 \"%1\" 1 "
                              "\"0 dBm\" 0 \"1 GHz\" 0>\n")
                          .arg(misc::num2str(value, 3, "Ohm")) // reference impedance
                          .arg(x_pos);
      componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
      wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos); // Vertical wire
      wirestr += QString("<%1 -120 %2 -120>\n")
                     .arg(x_pos - 100)
                     .arg(x_pos);   // Horizontal wire
    } else if (!tag.compare("DEV")) // Device painting
    {
      paintingstr += QString("<Text %1 -150 12 #000000 0 \"Device\">\n")
                         .arg(x_pos + 70); // Add 'Device' label
      paintingstr +=
          QString("<Rectangle %1 -160 90 50 #000000 0 1 #c0c0c0 1 0>\n")
              .arg(x_pos + 50); // Box surrounding the 'Device' label
      x_pos += 200;
    } else if (!tag.compare("LS")) // Series inductor
    {
      QString val = misc::num2str(value, 3, "H"); // Add prefix, unit - 3 significant digits
      componentstr += QString("<L L1 1 %1 -120 -26 10 0 0 \"%2\" 1 "
                              " 0>\n")
                          .arg(x_pos + 60)
                          .arg(val);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos)
                     .arg(x_pos + 30);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos + 90)
                     .arg(x_pos + x_series);
      x_pos += x_series;
    } else if (!tag.compare("CS")) // Series capacitor
    {
      QString val = misc::num2str(value, 3, "F"); // Add prefix, unit - 3 significant digits
      componentstr += QString("<C C1 1 %1 -120 -26 17 0 0 \"%2\" 1 "
                              " 0>\n")
                          .arg(x_pos + 60)
                          .arg(val);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos)
                     .arg(x_pos + 30);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos + 90)
                     .arg(x_pos + x_series);
      x_pos += x_series;
    } else if (!tag.compare("LP")) // Shunt inductor
    {
      QString val = misc::num2str(value, 3, "H"); // Add prefix, unit - 3 significant digits
      componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
      componentstr += QString("<L L1 1 %1 -30 5 -20 0 1 \"%2\" 1 "
                              " 0>\n")
                          .arg(x_pos)
                          .arg(val);
      wirestr += QString("<%1 -60 %1 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos - 20)
                     .arg(x_pos + x_shunt);
      x_pos += x_shunt;
    } else if (!tag.compare("CP")) // Shunt capacitor
    {
      QString val = misc::num2str(value, 3, "F"); // Add prefix, unit - 3 significant digits
      componentstr += QString("<GND * 1 %1 0 0 0 0 0>\n").arg(x_pos);
      componentstr += QString("<C C1 1 %1 -30 15 -20 0 1 \"%2\" 1 "
                              " 0>\n")
                          .arg(x_pos)
                          .arg(val);
      wirestr += QString("<%1 -60 %1 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos - 20)
                     .arg(x_pos + x_shunt);
      x_pos += x_shunt;
    } else if (!tag.compare("TL")) // Transmission line
    {
      if (microsyn) // Microstrip implementation
      {
        er = Substrate.er;
        getMicrostrip(value, Freq, &Substrate, width, er);
        QString val_width =
          misc::num2str(width, 3, "m"); // Add prefix, unit - 3 significant digits
        QString val_length =
          misc::num2str(value2 / sqrt(er), 3, "m"); // Add prefix, unit - 3 significant digits
        componentstr +=
            QString("<MLIN MS1 1 %3 -120 -26 20 0 0 \"Sub1\" 1 \"%1\" 1 \"%2\" "
                    "1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n")
                .arg(val_width)
                .arg(val_length)
                .arg(x_pos + 60);
      } else {
        // Add a prefix (although rarely needed here) and unit - 3 significant digits
        QString val_impedance = misc::num2str(value, 3, "Ohm");
        // Add prefix, unit - 3 significant digits
        QString val_length = misc::num2str(value2, 3, "m");
        componentstr += QString("<TLIN Line1 1 %3 -120 -26 20 0 0 \"%1\" 1 "
                                "\"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n")
                            .arg(val_impedance)
                            .arg(val_length)
                            .arg(x_pos + 60);
      }
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos)
                     .arg(x_pos + 30);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos + 90)
                     .arg(x_pos + x_series);
      x_pos += x_series;
    } else if (!tag.compare("OU")) // Open stub (upper)
    {
      if (microsyn) // Microstrip implementation
      {
        er = Substrate.er;
        getMicrostrip(value, Freq, &Substrate, width, er);
        // Add prefix, unit - 3 significant digits
        QString val_width = misc::num2str(width, 3, "m");
        // Add prefix, unit - 3 significant digits
        QString val_length = misc::num2str(value2 / sqrt(er), 3, "m");
        componentstr +=
            QString("<MLIN MS1 1 %3 -180 30 -30 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" "
                    "1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n")
                .arg(val_width)
                .arg(val_length)
                .arg(x_pos);
      } else {
        // Add a prefix (although rarely needed here) and unit - 3 significant digits
        QString val_impedance = misc::num2str(value, 3, "Ohm");
        // Add prefix, unit - 3 significant digits
        QString val_length = misc::num2str(value2, 3, "m");
        componentstr += QString("<TLIN Line1 1 %3 -180 30 -30 0 1 \"%1\" 1 "
                                "\"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n")
                            .arg(val_impedance)
                            .arg(val_length)
                            .arg(x_pos);
      }
      wirestr += QString("<%1 -150 %1 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos)
                     .arg(x_pos + x_shunt);
      // Here x_pos is not incremented since upper stubs does not overlap any
      // other component
    } else if (!tag.compare("OL")) // Open stub (lower)
    {
      if (microsyn) // Microstrip implementation
      {
        er = Substrate.er;
        getMicrostrip(value, Freq, &Substrate, width, er);
        // Add suffix mm, cm - 3 significant digits
        QString val_width = misc::num2str(width, 3, "m");
        // Add suffix mm, cm - 3 significant digits
        QString val_length = misc::num2str(value2 / sqrt(er), 3, "m");
        componentstr +=
            QString("<MLIN MS1 1 %3 -60 -26 30 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" "
                    "1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n")
                .arg(val_width)
                .arg(val_length)
                .arg(x_pos);
      } else {
        // Add a prefix (although rarely needed here) and unit - 3 significant digits
        QString val_impedance = misc::num2str(value, 3, "Ohm");
        // Add suffix mm, cm - 3 significant digits
        QString val_length = misc::num2str(value2, 3, "m");
        componentstr += QString("<TLIN Line1 1 %3 -60 -26 30 0 1 \"%1\" 1 "
                                "\"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n")
                            .arg(val_impedance)
                            .arg(val_length)
                            .arg(x_pos);
      }
      wirestr += QString("<%1 -90 %1 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos - 20)
                     .arg(x_pos + x_shunt);
      x_pos += x_shunt;
    } else if (!tag.compare("SU")) // Short circuited stub (upper)
    {
      if (microsyn) // Microstrip implementation
      {
        er = Substrate.er;
        getMicrostrip(value, Freq, &Substrate, width, er);
        QString val_width = misc::num2str(width, 3, "m");
        QString val_length = misc::num2str(value2 / sqrt(er), 3, "m");
        componentstr +=
            QString("<MLIN MS1 1 %3 -180 30 -30 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" "
                    "1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n")
                .arg(val_width)
                .arg(val_length)
                .arg(x_pos);
      } else {
        QString val_impedance = misc::num2str(value, 3, "Ohm");
        QString val_length = misc::num2str(value2, 3, "m");
        componentstr += QString("<TLIN Line1 1 %3 -180 30 -30 0 1 \"%1\" 1 "
                                "\"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n")
                            .arg(val_impedance)
                            .arg(val_length)
                            .arg(x_pos);
      }
      componentstr += QString("<GND * 1 %1 -210 0 0 0 2>\n").arg(x_pos);
      wirestr += QString("<%1 -150 %1 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos)
                     .arg(x_pos + x_shunt);
      // Here x_pos is not incremented since upper stubs does not overlap any
      // other component
    } else if (!tag.compare("SL")) // Short circuited stub (lower)
    {
      if (microsyn) // Microstrip implementation
      {
        er = Substrate.er;
        getMicrostrip(value, Freq, &Substrate, width, er);
        QString val_width = misc::num2str(width, 3, "m");
        QString val_length = misc::num2str(value2 / sqrt(er), 3, "m");
        componentstr +=
            QString("<MLIN MS1 1 %3 -60 30 -30 0 1 \"Sub1\" 1 \"%1\" 1 \"%2\" "
                    "1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n")
                .arg(val_width)
                .arg(val_length)
                .arg(x_pos);
      } else {
        QString val_impedance = misc::num2str(value, 3, "Ohm");
        QString val_length = misc::num2str(value2, 3, "m");
        componentstr += QString("<TLIN Line1 1 %3 -60 20 30 0 1 \"%1\" 1 "
                                "\"%2\" 1 \"0 dB\" 0 \"26.85\" 0>\n")
                            .arg(val_impedance)
                            .arg(val_length)
                            .arg(x_pos);
      }
      componentstr += QString("<GND * 1 %1 -30 0 0 0 0>\n").arg(x_pos);
      wirestr += QString("<%1 -90 %1 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos);
      wirestr += QString("<%1 -120 %2 -120 "
                         " 0 0 0 "
                         ">\n")
                     .arg(x_pos - 20)
                     .arg(x_pos + x_shunt);
      x_pos += x_shunt;
    } else if (!tag.compare("S2P")) // S-param simulation block
    {
      // Add the frequency range for the S-param simulation
      //   cover 1 octave below and 1 above, user will adjust if needed...
      double freq_start = Freq / 2.0;
      double freq_stop = 2.0 *Freq;
      QString val_freq_start = misc::num2str(freq_start, 3, "Hz");
      QString val_freq_stop = misc::num2str(freq_stop, 3, "Hz");

      componentstr +=
          QString("<.SP SP1 1 0 100 0 67 0 0 \"lin\" 1 \"%1\" 1 \"%2\" 1 "
                  "\"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n")
              .arg((val_freq_start))
              .arg((val_freq_stop));

      if (laddercode.indexOf("P2") == -1) // One port simulation
        componentstr += QString("<Eqn Eqn1 1 200 100 -28 15 0 0 "
                                "\"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
      else // Two ports simulation
        componentstr += QString("<Eqn Eqn1 1 200 100 -28 15 0 0 "
                                "\"S11_dB=dB(S[1,1])\" 1 \"S21_dB=dB(S[2,1])\" "
                                "1 \"S22_dB=dB(S[2,2])\" 1 \"yes\" 0>\n");
    } else if (!tag.compare("ZL")) // Complex load
    {
      double RL = value;
      double XL = value2;
      x_pos += 100;
      if ((RL > 1e-3) && (XL < -1e-3)) // R + C
      {
        QString val_Res = misc::num2str(RL, 3, "Ohm");
        // Need to use abs() because XL < 0
        QString val_Cap = misc::num2str(1 / (fabs(XL) * 2 * pi * Freq), 3, "F");
        componentstr +=
            QString(
                "<R R1 1 %1 -30 15 -26 0 -1 \"%2\" 1 \"26.85\" 0 \"US\" 0>\n")
                .arg(x_pos)
                .arg(val_Res);
        componentstr += QString("<C C1 1 %1 -90 15 -26 0 -1 \"%2\" 1 0>\n")
                            .arg(x_pos)
                            .arg(val_Cap);
        paintingstr +=
            QString("<Text %1 50 12 #000000 0 \"%4-j%5 %2 @ %3\">\n")
                .arg(x_pos)
                .arg(QChar(0x2126))
                .arg(misc::num2str(Freq, 3, "Hz"))
                .arg(RL)
                .arg(fabs(XL));
      } else if ((RL > 1e-3) && (XL > 1e-3)) // R + L
      {
        QString val_Res = misc::num2str(RL, 3, "Ohm");
        QString val_Ind = misc::num2str(XL / (2 * pi * Freq), 3, "H");
        componentstr +=
            QString(
                "<R R1 1 %1 -30 15 -26 0 -1 \"%2\" 1 \"26.85\" 0 \"US\" 0>\n")
                .arg(x_pos)
                .arg(val_Res);
        componentstr += QString("<L L1 1 %1 -90 15 -26 0 -1 \"%2\" 1 0>\n")
                            .arg(x_pos)
                            .arg(val_Ind);
        paintingstr +=
            QString("<Text %1 50 12 #000000 0 \"%4+j%5 %2 @ %3\">\n")
                .arg(x_pos)
                .arg(QChar(0x2126))
                .arg(misc::num2str(Freq, 3, "Hz"))
                .arg(RL)
                .arg(XL);
      } else if ((RL > 1e-3) && (fabs(XL) < 1e-3)) // R
      {
        QString val_Res = misc::num2str(RL, 3, "Ohm");
        componentstr +=
            QString(
                "<R R1 1 %1 -30 15 -26 0 -1 \"%2\" 1 \"26.85\" 0 \"US\" 0>\n")
                .arg(x_pos)
                .arg(val_Res);
        wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos); // Vertical wire
        paintingstr += QString("<Text %1 50 12 #000000 0 \"%4 %2 @ %3\">\n")
                           .arg(x_pos)
                           .arg(QChar(0x2126))
                           .arg(misc::num2str(Freq, 3, "Hz"))
                           .arg(RL);
      } else if ((RL < 1e-3) && (XL > 1e-3)) // L
      {
        QString val_Ind = misc::num2str(XL / (2 * pi * Freq), 3, "H");
        componentstr +=
            QString(
                "<L L1 1 %1 -30 15 -26 0 -1 \"%2\" 1 \"26.85\" 0 \"US\" 0>\n")
                .arg(x_pos)
                .arg(val_Ind);
        wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos); // Vertical wire
        paintingstr +=
            QString("<Text %1 50 12 #000000 0 \"j%4 %2 @ %3\">\n")
                .arg(x_pos)
                .arg(QChar(0x2126))
                .arg(misc::num2str(Freq, 3, "Hz"))
                .arg(XL);
      } else if ((RL < 1e-3) && (XL < -1e-3)) // C
      {
        // Need to use abs() because XL < 0
        QString val_Cap = misc::num2str(1 / (fabs(XL) * 2 * pi * Freq), 3, "F");
        componentstr +=
            QString(
                "<C C1 1 %1 -30 15 -26 0 -1 \"%2\" 1 \"26.85\" 0 \"US\" 0>\n")
                .arg(x_pos)
                .arg(val_Cap);
        wirestr += QString("<%1 -60 %1 -120>\n").arg(x_pos); // Vertical wire
        paintingstr +=
            QString("<Text %1 50 12 #000000 0 \"-j%4 %2 @ %3\">\n")
                .arg(x_pos)
                .arg(QChar(0x2126))
                .arg(misc::num2str(Freq, 3, "Hz"))
                .arg(fabs(XL));
      }
      wirestr += QString("<%1 -120 %2 -120>\n")
                     .arg(x_pos - 100)
                     .arg(x_pos); // Horizontal wire
      componentstr += QString("<GND * 1 %1 0 0 -1 0 0>\n").arg(x_pos);

      // Box surrounding the load
      paintingstr +=
          QString("<Rectangle %1 -150 200 200 #000000 0 1 #c0c0c0 1 0>\n")
              .arg(x_pos - 30);
    }
  }

  // Substrate
  if (microsyn)
    componentstr +=
        QString("<SUBST Sub1 1 400 200 -30 24 0 0 \"%1\" 1 \"%2mm\" 1 \"%3um\" "
                "1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n")
            .arg(Substrate.er)
            .arg(Substrate.height * 1e3)
            .arg(Substrate.thickness * 1e6)
            .arg(Substrate.tand)
            .arg(Substrate.resistivity)
            .arg(Substrate.roughness);

  // Schematic header
  QString Schematic = "<Qucs Schematic " PACKAGE_VERSION ">\n";

  // Add components
  Schematic += "<Components>\n";
  Schematic += componentstr;
  Schematic += "</Components>\n";

  // Add wires
  Schematic += "<Wires>\n";
  Schematic += wirestr;
  Schematic += "</Wires>\n";

  // Add paintings
  Schematic += "<Paintings>\n";
  Schematic += paintingstr;
  Schematic += "</Paintings>\n";

  //Copy the schematic into clipboard
  QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
}
