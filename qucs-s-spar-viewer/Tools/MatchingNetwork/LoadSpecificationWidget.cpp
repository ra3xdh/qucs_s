/// @file LoadSpecificationWidget.cpp
/// @brief Widget for entering the load impedance data (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "LoadSpecificationWidget.h"

LoadSpecificationWidget::LoadSpecificationWidget(QWidget *parent)
    : QGroupBox(parent), m_twoPortMode(false), m_updatingValues(false),
      m_isCollapsed(false), m_Z0(50.0) {
  setupUI();

  // Set default values
  m_impedanceReal->setValue(75.0);
  m_impedanceImag->setValue(0.0);
  Z0_Port2 = 50; // Default value for the reference impedance of port 1
  Z0_Port2 = 50; // Default value for the reference impedance of port 2

  // Connect signals
  connect(m_impedanceReal, &QDoubleSpinBox::valueChanged, this,
          &LoadSpecificationWidget::onImpedanceChanged);

  connect(m_impedanceImag, &QDoubleSpinBox::valueChanged, this,
          &LoadSpecificationWidget::onImpedanceChanged);

  connect(m_reflectionReal, &QDoubleSpinBox::valueChanged, this,
          &LoadSpecificationWidget::onReflectionCoefficientChanged);

  connect(m_reflectionImag, &QDoubleSpinBox::valueChanged, this,
          &LoadSpecificationWidget::onReflectionCoefficientChanged);

  connect(m_formatCombo, &QComboBox::currentIndexChanged, this,
          &LoadSpecificationWidget::onFormatChanged);

  connect(m_inputMethodGroup, &QButtonGroup::buttonClicked, this,
          &LoadSpecificationWidget::onInputMethodChanged);

  connect(m_browseButton, &QPushButton::clicked, this,
          &LoadSpecificationWidget::onBrowseFile);

  connect(m_toggleButton, &QPushButton::clicked, this,
          [this]() { onToggleCollapse(); });

  onImpedanceChanged();

  // Collapsed by default. The height of this widget affects all the tabs. It's
  // good to keep it as compact as possible
  setCollapsed(true);
}

void LoadSpecificationWidget::setupUI() {
  // Create main layout for the group box
  QVBoxLayout *groupLayout = new QVBoxLayout(this);

  // Create header with title and collapse button
  QWidget *headerWidget = new QWidget();
  QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
  headerLayout->setContentsMargins(0, 0, 0, 0);

  // Collapse/expand button
  m_toggleButton = new QPushButton("▼");
  m_toggleButton->setFixedSize(20, 20);
  m_toggleButton->setStyleSheet(
      "QPushButton { border: none; font-weight: bold; }");

  headerLayout->addWidget(m_toggleButton);
  headerLayout->addWidget(new QLabel("Load Settings"));
  headerLayout->addStretch();

  // Create content widget that will be hidden/shown
  m_contentWidget = new QWidget();
  m_mainLayout = new QGridLayout(m_contentWidget);

  // Input method selection
  m_manualInputRadio = new QRadioButton("Manual input");
  m_fileInputRadio = new QRadioButton("File input");
  m_inputMethodGroup = new QButtonGroup(this);
  m_inputMethodGroup->addButton(m_manualInputRadio, 0);
  m_inputMethodGroup->addButton(m_fileInputRadio, 1);
  m_manualInputRadio->setChecked(true);

  m_browseButton = new QPushButton("Browse");
  m_browseButton->setEnabled(false);
  m_fileLabel = new QLabel("No file selected");

  m_mainLayout->addWidget(m_manualInputRadio, 0, 0);
  m_mainLayout->addWidget(m_fileInputRadio, 0, 1);
  m_mainLayout->addWidget(m_browseButton, 0, 2);
  m_mainLayout->addWidget(m_fileLabel, 0, 3, 1, 2);

  // Format selection
  m_formatLabel = new QLabel("Format:");
  m_formatCombo = new QComboBox();
  m_formatCombo->addItem("Real / Imaginary");
  m_formatCombo->addItem("Magnitude / Angle");

  m_mainLayout->addWidget(m_formatLabel, 1, 0);
  m_mainLayout->addWidget(m_formatCombo, 1, 1, 1, 2);

  m_mainLayout->setVerticalSpacing(1);

  setupOnePortUI();
  setupTwoPortUI();

  // Add widgets to main group layout
  groupLayout->addWidget(headerWidget);
  groupLayout->addWidget(m_contentWidget);
  groupLayout->setContentsMargins(10, 5, 10, 10);
}

void LoadSpecificationWidget::setupOnePortUI() {
  // Impedance widgets
  m_impedanceLabel = new QLabel("ZL:");
  m_impedanceReal = new QDoubleSpinBox();
  m_impedanceReal->setRange(-10000, 10000);
  m_impedanceReal->setDecimals(1);
  m_impedanceReal->setSingleStep(0.1);

  m_impedanceSeparator = new QLabel("+j");
  m_impedanceImag = new QDoubleSpinBox();
  m_impedanceImag->setRange(-10000, 10000);
  m_impedanceImag->setDecimals(1);
  m_impedanceImag->setSingleStep(0.1);

  m_impedanceUnit = new QLabel("Ω");

  m_mainLayout->addWidget(m_impedanceLabel, 2, 0);
  m_mainLayout->addWidget(m_impedanceReal, 2, 1);
  m_mainLayout->addWidget(m_impedanceSeparator, 2, 2);
  m_mainLayout->addWidget(m_impedanceImag, 2, 3);
  m_mainLayout->addWidget(m_impedanceUnit, 2, 4);

  // Reflection coefficient widgets
  m_reflectionLabel = new QLabel("ΓL:");
  m_reflectionReal = new QDoubleSpinBox();
  m_reflectionReal->setRange(-2.0, 2.0);
  m_reflectionReal->setDecimals(6);
  m_reflectionReal->setSingleStep(0.001);

  m_reflectionSeparator = new QLabel("+j");
  m_reflectionImag = new QDoubleSpinBox();
  m_reflectionImag->setRange(-2.0, 2.0);
  m_reflectionImag->setDecimals(6);
  m_reflectionImag->setSingleStep(0.001);

  m_mainLayout->addWidget(m_reflectionLabel, 3, 0);
  m_mainLayout->addWidget(m_reflectionReal, 3, 1);
  m_mainLayout->addWidget(m_reflectionSeparator, 3, 2);
  m_mainLayout->addWidget(m_reflectionImag, 3, 3);
}

void LoadSpecificationWidget::setupTwoPortUI() {
  m_twoPortWidget = new QWidget();
  m_twoPortLayout = new QGridLayout(m_twoPortWidget);

  // S11 parameter
  m_s11Label = new QLabel("S11:");
  m_s11Real = new QDoubleSpinBox();
  m_s11Real->setRange(-10.0, 10.0);
  m_s11Real->setDecimals(3);
  m_s11Real->setSingleStep(0.1);
  m_s11Real->setValue(0.5);
  m_s11Separator = new QLabel("+j");
  m_s11Imag = new QDoubleSpinBox();
  m_s11Imag->setRange(-10.0, 10.0);
  m_s11Imag->setDecimals(3);
  m_s11Imag->setSingleStep(0.1);
  m_s11Imag->setValue(0);

  m_twoPortLayout->addWidget(m_s11Label, 0, 0);
  m_twoPortLayout->addWidget(m_s11Real, 0, 1);
  m_twoPortLayout->addWidget(m_s11Separator, 0, 2);
  m_twoPortLayout->addWidget(m_s11Imag, 0, 3);

  // S12 parameter
  m_s12Label = new QLabel("S12:");
  m_s12Real = new QDoubleSpinBox();
  m_s12Real->setRange(-10.0, 10.0);
  m_s12Real->setDecimals(3);
  m_s12Real->setSingleStep(0.1);
  m_s12Real->setValue(0);
  m_s12Separator = new QLabel("+j");
  m_s12Imag = new QDoubleSpinBox();
  m_s12Imag->setRange(-100.0, 100.0);
  m_s12Imag->setDecimals(3);
  m_s12Imag->setSingleStep(0.1);
  m_s12Imag->setValue(0);

  m_twoPortLayout->addWidget(m_s12Label, 0, 4);
  m_twoPortLayout->addWidget(m_s12Real, 0, 5);
  m_twoPortLayout->addWidget(m_s12Separator, 0, 6);
  m_twoPortLayout->addWidget(m_s12Imag, 0, 7);

  // S21 parameter
  m_s21Label = new QLabel("S21:");
  m_s21Real = new QDoubleSpinBox();
  m_s21Real->setRange(-10.0, 10.0);
  m_s21Real->setDecimals(3);
  m_s21Real->setSingleStep(0.1);
  m_s21Real->setValue(0.5);
  m_s21Separator = new QLabel("+j");
  m_s21Imag = new QDoubleSpinBox();
  m_s21Imag->setRange(-100.0, 100.0);
  m_s21Imag->setDecimals(3);
  m_s21Imag->setSingleStep(0.1);
  m_s21Imag->setValue(0);

  m_twoPortLayout->addWidget(m_s21Label, 1, 0);
  m_twoPortLayout->addWidget(m_s21Real, 1, 1);
  m_twoPortLayout->addWidget(m_s21Separator, 1, 2);
  m_twoPortLayout->addWidget(m_s21Imag, 1, 3);

  // S22 parameter
  m_s22Label = new QLabel("S22:");
  m_s22Real = new QDoubleSpinBox();
  m_s22Real->setRange(-10.0, 10.0);
  m_s22Real->setDecimals(3);
  m_s22Real->setSingleStep(0.1);
  m_s22Real->setValue(0.5);
  m_s22Separator = new QLabel("+j");
  m_s22Imag = new QDoubleSpinBox();
  m_s22Imag->setRange(-10.0, 10.0);
  m_s22Imag->setDecimals(3);
  m_s22Imag->setSingleStep(0.1);
  m_s22Imag->setValue(0);

  m_twoPortLayout->addWidget(m_s22Label, 1, 4);
  m_twoPortLayout->addWidget(m_s22Real, 1, 5);
  m_twoPortLayout->addWidget(m_s22Separator, 1, 6);
  m_twoPortLayout->addWidget(m_s22Imag, 1, 7);

  // Connect S-parameter signals
  connect(m_s11Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s11Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s12Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s12Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s21Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s21Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s22Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s22Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &LoadSpecificationWidget::onSParameterChanged);

  m_mainLayout->addWidget(m_twoPortWidget, 4, 0, 1, 5);
  m_twoPortWidget->hide(); // Hidden by default (one-port mode)
}

std::complex<double>
LoadSpecificationWidget::getLoadImpedance_At_Fmatch() const {
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_impedanceReal->value(),
                                m_impedanceImag->value());
  } else { // Magnitude/Angle
    double mag = m_impedanceReal->value();
    double ang = m_impedanceImag->value() * M_PI / 180.0; // Convert to radians
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

QList<std::complex<double>> LoadSpecificationWidget::getZLdata() {
  const QList<double> &ZL_re = loadData["ZL_re"];
  const QList<double> &ZL_im = loadData["ZL_im"];
  QList<std::complex<double>> ZL_data;
  int n = std::min(ZL_re.size(), ZL_im.size());
  for (int i = 0; i < n; ++i) {
    ZL_data.append(std::complex<double>(ZL_re[i], ZL_im[i]));
  }
  return ZL_data;
}

QList<double> LoadSpecificationWidget::getFrequency() {
  return loadData["frequency"];
}

std::pair<std::complex<double>, std::complex<double>>
LoadSpecificationWidget::getTwoPortMatchingImpedances() const {
  const std::complex<double> S11 = getS11();
  const std::complex<double> S12 = getS12();
  const std::complex<double> S21 = getS21();
  const std::complex<double> S22 = getS22();
  constexpr double Z0 = 50.0;

  std::complex<double> delta = S11 * S22 - S12 * S21;

  double abs_S11 = std::abs(S11);
  double abs_S22 = std::abs(S22);
  double abs_delta = std::abs(delta);

  double B1 =
      1.0 + abs_S11 * abs_S11 - abs_S22 * abs_S22 - abs_delta * abs_delta;
  double B2 =
      1.0 + abs_S22 * abs_S22 - abs_S11 * abs_S11 - abs_delta * abs_delta;

  std::complex<double> C1 = S11 - delta * std::conj(S22);
  std::complex<double> C2 = S22 - delta * std::conj(S11);

  std::complex<double> sqrt_expr_S =
      std::sqrt(std::complex<double>(B1 * B1 - 4.0 * std::norm(C1), 0.0));
  std::complex<double> sqrt_expr_L =
      std::sqrt(std::complex<double>(B2 * B2 - 4.0 * std::norm(C2), 0.0));

  std::complex<double> gamma_S1 = (B1 + sqrt_expr_S) / (2.0 * C1);
  std::complex<double> gamma_S2 = (B1 - sqrt_expr_S) / (2.0 * C1);

  std::complex<double> gamma_L1 = (B2 + sqrt_expr_L) / (2.0 * C2);
  std::complex<double> gamma_L2 = (B2 - sqrt_expr_L) / (2.0 * C2);

  std::complex<double> ZS1 = Z0 * (1.0 + gamma_S1) / (1.0 - gamma_S1);
  std::complex<double> ZS2 = Z0 * (1.0 + gamma_S2) / (1.0 - gamma_S2);

  std::complex<double> ZL1 = Z0 * (1.0 + gamma_L1) / (1.0 - gamma_L1);
  std::complex<double> ZL2 = Z0 * (1.0 + gamma_L2) / (1.0 - gamma_L2);

  // Conjugate all impedances
  ZS1 = std::conj(ZS1);
  ZS2 = std::conj(ZS2);
  ZL1 = std::conj(ZL1);
  ZL2 = std::conj(ZL2);

  // Choose S1 by default, else S2 if negative real part
  if (ZS1.real() < 0 || ZL1.real() < 0) {
    return std::make_pair(ZS2, ZL2);
  }

  return std::make_pair(ZS1, ZL1);
}

std::complex<double> LoadSpecificationWidget::getReflectionCoefficient() const {
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_reflectionReal->value(),
                                m_reflectionImag->value());
  } else { // Magnitude/Angle
    double mag = m_reflectionReal->value();
    double ang = m_reflectionImag->value() * M_PI / 180.0; // Convert to radians
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS11() const {
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s11Real->value(), m_s11Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s11Real->value();
    double ang = m_s11Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS12() const {
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s12Real->value(), m_s12Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s12Real->value();
    double ang = m_s12Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS21() const {
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s21Real->value(), m_s21Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s21Real->value();
    double ang = m_s21Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS22() const {
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s22Real->value(), m_s22Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s22Real->value();
    double ang = m_s22Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

void LoadSpecificationWidget::setLoadImpedance(
    const std::complex<double> &impedance) {
  m_updatingValues = true;

  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    m_impedanceReal->setValue(impedance.real());
    m_impedanceImag->setValue(impedance.imag());
  } else { // Magnitude/Angle
    double mag = std::abs(impedance);
    double ang = std::arg(impedance) * 180.0 / M_PI; // Convert to degrees
    m_impedanceReal->setValue(mag);
    m_impedanceImag->setValue(ang);
  }

  m_updatingValues = false;
  updateReflectionCoefficient();
}

void LoadSpecificationWidget::setReflectionCoefficient(
    const std::complex<double> &gamma) {
  m_updatingValues = true;

  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    m_reflectionReal->setValue(gamma.real());
    m_reflectionImag->setValue(gamma.imag());
  } else { // Magnitude/Angle
    double mag = std::abs(gamma);
    double ang = std::arg(gamma) * 180.0 / M_PI; // Convert to degrees
    m_reflectionReal->setValue(mag);
    m_reflectionImag->setValue(ang);
  }

  m_updatingValues = false;
  updateImpedance();
}

void LoadSpecificationWidget::setTwoPortMode(bool enabled) {
  m_twoPortMode = enabled;

  if (enabled) {
    // Show two-port widgets, hide one-port widgets
    m_impedanceLabel->hide();
    m_impedanceReal->hide();
    m_impedanceSeparator->hide();
    m_impedanceImag->hide();
    m_impedanceUnit->hide();
    m_reflectionLabel->hide();
    m_reflectionReal->hide();
    m_reflectionSeparator->hide();
    m_reflectionImag->hide();
    m_twoPortWidget->show();

    // Update file selection for s2p files
    if (m_fileInputRadio->isChecked()) {
      m_fileLabel->setText("Select S2P file");
    }
  } else {
    // Show one-port widgets, hide two-port widgets
    m_impedanceLabel->show();
    m_impedanceReal->show();
    m_impedanceSeparator->show();
    m_impedanceImag->show();
    m_impedanceUnit->show();
    m_reflectionLabel->show();
    m_reflectionReal->show();
    m_reflectionSeparator->show();
    m_reflectionImag->show();
    m_twoPortWidget->hide();

    // Update file selection for s1p files
    if (m_fileInputRadio->isChecked()) {
      m_fileLabel->setText("Select S1P file");
    }
  }
}

void LoadSpecificationWidget::onImpedanceChanged() {
  if (m_updatingValues) {
    return;
  }

  updateReflectionCoefficient();
  emit impedanceChanged();
}

void LoadSpecificationWidget::onReflectionCoefficientChanged() {
  if (m_updatingValues) {
    return;
  }

  updateImpedance();
  emit reflectionCoefficientChanged();
}

void LoadSpecificationWidget::onFormatChanged() {
  updateImpedanceFormat();
  updateReflectionFormat();
  updateSParameterFormat();
}

void LoadSpecificationWidget::onInputMethodChanged() {
  bool fileInput = m_fileInputRadio->isChecked();
  m_browseButton->setEnabled(fileInput);

  // Enable/disable manual input widgets
  m_formatCombo->setEnabled(!fileInput);
  m_impedanceReal->setEnabled(!fileInput);
  m_impedanceImag->setEnabled(!fileInput);
  m_reflectionReal->setEnabled(!fileInput);
  m_reflectionImag->setEnabled(!fileInput);

  if (m_twoPortMode) {
    m_s11Real->setEnabled(!fileInput);
    m_s11Imag->setEnabled(!fileInput);
    m_s12Real->setEnabled(!fileInput);
    m_s12Imag->setEnabled(!fileInput);
    m_s21Real->setEnabled(!fileInput);
    m_s21Imag->setEnabled(!fileInput);
    m_s22Real->setEnabled(!fileInput);
    m_s22Imag->setEnabled(!fileInput);
  }
}

void LoadSpecificationWidget::onBrowseFile() {
  QString filter;
  if (m_twoPortMode) {
    filter = "S2P Files (*.s2p);;All Files (*)";
  } else {
    filter = "S1P Files (*.s1p);;All Files (*)";
  }

  spar_file_path =
      QFileDialog::getOpenFileName(this, "Select file", "", filter);

  if (!spar_file_path.isEmpty()) {
    m_currentFile = spar_file_path;
    m_fileLabel->setText(QFileInfo(spar_file_path).fileName());

    // Load S-parameter data from file
    loadData = readTouchstoneFile(spar_file_path);

    // Find the S-parameter data at the frequency the user wants to match and
    // present it on the UI

    // 1) Find the closest frequency to that the user specified
    const QList<double> &frequencies = loadData.value("frequency");

    // 2) Find the index of the closest frequency
    int closestIdx = -1;
    double minDiff = std::numeric_limits<double>::max();
    for (int i = 0; i < frequencies.size(); ++i) {
      double diff = qAbs(frequencies[i] - f_match);
      if (diff < minDiff) {
        minDiff = diff;
        closestIdx = i;
      }
    }

    // 3) Retrieve data from S-parameter traces
    QStringList keysToRetrieve;

    if (m_twoPortMode) {
      // 2-port matching
      keysToRetrieve.push_back("S11_re");
      keysToRetrieve.push_back("S11_im");

      keysToRetrieve.push_back("S12_re");
      keysToRetrieve.push_back("S12_im");

      keysToRetrieve.push_back("S21_re");
      keysToRetrieve.push_back("S21_im");

      keysToRetrieve.push_back("S22_re");
      keysToRetrieve.push_back("S22_im");

    } else {
      // 1-port matching
      keysToRetrieve.push_back("S11_re");
      keysToRetrieve.push_back("S11_im");
    }

    QMap<QString, double> result;
    if (closestIdx != -1) {
      for (const QString &key : keysToRetrieve) {
        if (loadData.contains(key) && loadData.value(key).size() > closestIdx) {
          result[key] = loadData.value(key)[closestIdx];
        }
      }
    }

    // 4) Show the data in the UI
    if (m_twoPortMode) {
      // 2-port matching
      std::complex<double> S11(result["S11_re"], result["S11_im"]);
      std::complex<double> S12(result["S12_re"], result["S12_im"]);
      std::complex<double> S21(result["S21_re"], result["S21_im"]);
      std::complex<double> S22(result["S22_re"], result["S22_im"]);

      // Block signals
      m_s11Real->blockSignals(true);
      m_s11Imag->blockSignals(true);

      m_s12Real->blockSignals(true);
      m_s12Imag->blockSignals(true);

      m_s21Real->blockSignals(true);
      m_s21Imag->blockSignals(true);

      m_s22Real->blockSignals(true);
      m_s22Imag->blockSignals(true);

      // Change S21 values
      m_s11Real->setValue(S11.real());
      m_s11Imag->setValue(S11.imag());

      m_s12Real->setValue(S12.real());
      m_s12Imag->setValue(S12.imag());

      m_s21Real->setValue(S21.real());
      m_s21Imag->setValue(S21.imag());

      m_s22Real->setValue(S22.real());
      m_s22Imag->setValue(S22.imag());

      // Enable signals
      m_s11Real->blockSignals(false);
      m_s11Imag->blockSignals(false);

      m_s12Real->blockSignals(false);
      m_s12Imag->blockSignals(false);

      m_s21Real->blockSignals(false);
      m_s21Imag->blockSignals(false);

      m_s22Real->blockSignals(false);
      m_s22Imag->blockSignals(false);

    } else {
      // 1-port matching
      std::complex<double> S11(result["S11_re"], result["S11_im"]);

      // Block signals
      m_reflectionReal->blockSignals(true);
      m_reflectionImag->blockSignals(true);

      m_impedanceReal->blockSignals(true);
      m_impedanceImag->blockSignals(true);

      // Set reflection coefficient widgets
      m_reflectionReal->setValue(S11.real());
      m_reflectionImag->setValue(S11.imag());

      // Set impedance widgets
      double Z0 = loadData["Z0"].at(0);
      std::complex<double> ZL = Z0 * (1.0 + S11) / (1.0 - S11);
      m_impedanceReal->setValue(ZL.real());
      m_impedanceImag->setValue(ZL.imag());

      // Enable signals
      m_reflectionReal->blockSignals(false);
      m_reflectionImag->blockSignals(false);

      m_impedanceReal->blockSignals(false);
      m_impedanceImag->blockSignals(false);

      // Calculate ZL data from S11 and store that in the loadData object
      QList<double> S11_re = loadData["S11_re"];
      QList<double> S11_im = loadData["S11_im"];

      for (int i = 0; i < S11_re.size(); ++i) {
        std::complex<double> S11(S11_re[i], S11_im[i]);
        std::complex<double> Z = Z0 * (1.0 + S11) / (1.0 - S11);
        loadData["ZL_re"].append(Z.real());
        loadData["ZL_im"].append(Z.imag());
      }
    }
  }
  emit impedanceChanged();
}

void LoadSpecificationWidget::onSParameterChanged() {
  if (m_updatingValues) {
    return;
  }
  emit sParametersChanged();
}

void LoadSpecificationWidget::updateReflectionCoefficient() {
  if (m_updatingValues) {
    return;
  }

  m_updatingValues = true;

  std::complex<double> ZL = getLoadImpedance_At_Fmatch();
  std::complex<double> gamma = (ZL - m_Z0) / (ZL + m_Z0);

  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    m_reflectionReal->setValue(gamma.real());
    m_reflectionImag->setValue(gamma.imag());
  } else { // Magnitude/Angle
    double mag = std::abs(gamma);
    double ang = std::arg(gamma) * 180.0 / M_PI;
    m_reflectionReal->setValue(mag);
    m_reflectionImag->setValue(ang);
  }

  m_updatingValues = false;
}

void LoadSpecificationWidget::updateImpedance() {
  if (m_updatingValues) {
    return;
  }

  m_updatingValues = true;

  std::complex<double> gamma = getReflectionCoefficient();
  std::complex<double> ZL = m_Z0 * (1.0 + gamma) / (1.0 - gamma);

  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    m_impedanceReal->setValue(ZL.real());
    m_impedanceImag->setValue(ZL.imag());
  } else { // Magnitude/Angle
    double mag = std::abs(ZL);
    double ang = std::arg(ZL) * 180.0 / M_PI;
    m_impedanceReal->setValue(mag);
    m_impedanceImag->setValue(ang);
  }

  m_updatingValues = false;
}

void LoadSpecificationWidget::updateImpedanceFormat() {
  std::complex<double> impedance = getLoadImpedance_At_Fmatch();

  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    m_impedanceSeparator->setText("+j");
    m_impedanceReal->setValue(impedance.real());
    m_impedanceImag->setValue(impedance.imag());
  } else { // Magnitude/Angle
    m_impedanceSeparator->setText("∠");
    m_impedanceReal->setValue(std::abs(impedance));
    m_impedanceImag->setValue(std::arg(impedance) * 180.0 / M_PI);
  }
}

void LoadSpecificationWidget::updateReflectionFormat() {
  std::complex<double> gamma = getReflectionCoefficient();

  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    m_reflectionSeparator->setText("+j");
    m_reflectionReal->setValue(gamma.real());
    m_reflectionImag->setValue(gamma.imag());
  } else { // Magnitude/Angle
    m_reflectionSeparator->setText("∠");
    m_reflectionReal->setValue(std::abs(gamma));
    m_reflectionImag->setValue(std::arg(gamma) * 180.0 / M_PI);
  }
}

void LoadSpecificationWidget::updateSParameterFormat() {
  if (!m_twoPortMode) {
    return;
  }

  std::complex<double> s11 = getS11();
  std::complex<double> s12 = getS12();
  std::complex<double> s21 = getS21();
  std::complex<double> s22 = getS22();

  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    m_s11Separator->setText("+j");
    m_s12Separator->setText("+j");
    m_s21Separator->setText("+j");
    m_s22Separator->setText("+j");

    m_s11Real->setValue(s11.real());
    m_s11Imag->setValue(s11.imag());
    m_s12Real->setValue(s12.real());
    m_s12Imag->setValue(s12.imag());
    m_s21Real->setValue(s21.real());
    m_s21Imag->setValue(s21.imag());
    m_s22Real->setValue(s22.real());
    m_s22Imag->setValue(s22.imag());
  } else { // Magnitude/Angle
    m_s11Separator->setText("∠");
    m_s12Separator->setText("∠");
    m_s21Separator->setText("∠");
    m_s22Separator->setText("∠");

    m_s11Real->setValue(std::abs(s11));
    m_s11Imag->setValue(std::arg(s11) * 180.0 / M_PI);
    m_s12Real->setValue(std::abs(s12));
    m_s12Imag->setValue(std::arg(s12) * 180.0 / M_PI);
    m_s21Real->setValue(std::abs(s21));
    m_s21Imag->setValue(std::arg(s21) * 180.0 / M_PI);
    m_s22Real->setValue(std::abs(s22));
    m_s22Imag->setValue(std::arg(s22) * 180.0 / M_PI);
  }
}

void LoadSpecificationWidget::setCollapsed(bool collapsed) {
  m_isCollapsed = collapsed;
  m_contentWidget->setVisible(!collapsed);
  m_toggleButton->setText(collapsed ? "▶" : "▼");

  // Update the size policy to allow proper resizing
  if (collapsed) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(sizeHint().height());
  } else {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMaximumHeight(QWIDGETSIZE_MAX);
    setMinimumHeight(0);
  }

  emit collapsedStateChanged(collapsed);
}

void LoadSpecificationWidget::mousePressEvent(QMouseEvent *event) {
  // Check if click is in the title area (first 25 pixels from top)
  if (event->position().y() <= 25) {
    onToggleCollapse();
  }
  QGroupBox::mousePressEvent(event);
}
