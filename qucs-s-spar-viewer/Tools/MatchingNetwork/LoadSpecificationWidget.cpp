#include "LoadSpecificationWidget.h"

LoadSpecificationWidget::LoadSpecificationWidget(QWidget *parent)
    : QGroupBox(parent)
      , m_twoPortMode(false)
      , m_updatingValues(false)
      , m_isCollapsed(false)
      , m_Z0(50.0)
{
  setupUI();

  // Set default values
  m_impedanceReal->setValue(75.0);
  m_impedanceImag->setValue(0.0);
  Z0_Port2 = 50; // Default value for the reference impedance of port 1
  Z0_Port2 = 50; // Default value for the reference impedance of port 2

  // Connect signals
  connect(m_impedanceReal, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onImpedanceChanged);
  connect(m_impedanceImag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onImpedanceChanged);
  connect(m_reflectionReal, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onReflectionCoefficientChanged);
  connect(m_reflectionImag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onReflectionCoefficientChanged);
  connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LoadSpecificationWidget::onFormatChanged);
  connect(m_inputMethodGroup, &QButtonGroup::buttonClicked, this, &LoadSpecificationWidget::onInputMethodChanged);
  connect(m_browseButton, &QPushButton::clicked, this, &LoadSpecificationWidget::onBrowseFile);
  connect(m_toggleButton, SIGNAL(clicked(bool)), this, SLOT(onToggleCollapse()));

  // Initialize display
  onImpedanceChanged();
}

LoadSpecificationWidget::~LoadSpecificationWidget(){
}

void LoadSpecificationWidget::setupUI()
{
  // Create main layout for the group box
  QVBoxLayout* groupLayout = new QVBoxLayout(this);

         // Create header with title and collapse button
  QWidget* headerWidget = new QWidget();
  QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
  headerLayout->setContentsMargins(0, 0, 0, 0);

         // Collapse/expand button
  m_toggleButton = new QPushButton("▼");
  m_toggleButton->setFixedSize(20, 20);
  m_toggleButton->setStyleSheet("QPushButton { border: none; font-weight: bold; }");

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

  setupOnePortUI();
  setupTwoPortUI();

         // Add widgets to main group layout
  groupLayout->addWidget(headerWidget);
  groupLayout->addWidget(m_contentWidget);
  groupLayout->setContentsMargins(10, 5, 10, 10);
}

void LoadSpecificationWidget::setupOnePortUI()
{
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

void LoadSpecificationWidget::setupTwoPortUI()
{
  m_twoPortWidget = new QWidget();
  m_twoPortLayout = new QGridLayout(m_twoPortWidget);

  // S11 parameter
  m_s11Label = new QLabel("S11:");
  m_s11Real = new QDoubleSpinBox();
  m_s11Real->setRange(-10.0, 10.0);
  m_s11Real->setDecimals(2);
  m_s11Real->setSingleStep(0.1);
  m_s11Real->setValue(0.5);
  m_s11Separator = new QLabel("+j");
  m_s11Imag = new QDoubleSpinBox();
  m_s11Imag->setRange(-10.0, 10.0);
  m_s11Imag->setDecimals(2);
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
  m_s12Real->setDecimals(2);
  m_s12Real->setSingleStep(0.1);
  m_s12Real->setValue(0);
  m_s12Separator = new QLabel("+j");
  m_s12Imag = new QDoubleSpinBox();
  m_s12Imag->setRange(-100.0, 100.0);
  m_s12Imag->setDecimals(2);
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
  m_s21Real->setDecimals(2);
  m_s21Real->setSingleStep(0.1);
  m_s21Real->setValue(0.5);
  m_s21Separator = new QLabel("+j");
  m_s21Imag = new QDoubleSpinBox();
  m_s21Imag->setRange(-100.0, 100.0);
  m_s21Imag->setDecimals(2);
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
  m_s22Real->setDecimals(2);
  m_s22Real->setSingleStep(0.1);
  m_s22Real->setValue(0.5);
  m_s22Separator = new QLabel("+j");
  m_s22Imag = new QDoubleSpinBox();
  m_s22Imag->setRange(-10.0, 10.0);
  m_s22Imag->setDecimals(2);
  m_s22Imag->setSingleStep(0.1);
  m_s22Imag->setValue(0);

  m_twoPortLayout->addWidget(m_s22Label, 1, 4);
  m_twoPortLayout->addWidget(m_s22Real, 1, 5);
  m_twoPortLayout->addWidget(m_s22Separator, 1, 6);
  m_twoPortLayout->addWidget(m_s22Imag, 1, 7);

  // Connect S-parameter signals
  connect(m_s11Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s11Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s12Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s12Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s21Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s21Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s22Real, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);
  connect(m_s22Imag, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LoadSpecificationWidget::onSParameterChanged);

  m_mainLayout->addWidget(m_twoPortWidget, 4, 0, 1, 5);
  m_twoPortWidget->hide(); // Hidden by default (one-port mode)
}

std::complex<double> LoadSpecificationWidget::getLoadImpedance() const
{
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_impedanceReal->value(), m_impedanceImag->value());
  } else { // Magnitude/Angle
    double mag = m_impedanceReal->value();
    double ang = m_impedanceImag->value() * M_PI / 180.0; // Convert to radians
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::pair<std::complex<double>, std::complex<double>> LoadSpecificationWidget::getTwoPortMatchingImpedances() const
{
  // Get the S-parameters
  std::complex<double> S11 = getS11();
  std::complex<double> S12 = getS12();
  std::complex<double> S21 = getS21();
  std::complex<double> S22 = getS22();

  std::complex<double> Gamma_L = std::conj(S22);
  std::complex<double> Gamma_S = std::conj(S11);


         // Calculate input and output reflection coefficients (Γin, Γout) for conjugate matching
         // These formulas assume the load and source are conjugate matched.
         // Reference: Pozar, Microwave Engineering 571
  std::complex<double> Gamma_in = S11 + (S12 * S21 * Gamma_L) / (1.0 - S22 * Gamma_L);
  std::complex<double> Gamma_out = S22 + (S12 * S21 * Gamma_S) / (1.0 - S11 * Gamma_S);

         // Convert reflection coefficients to matching impedances using Smith chart formula
  std::complex<double> Zin_match = Z0_Port1 * (1.0 + Gamma_in) / (1.0 - Gamma_in);
  std::complex<double> Zout_match = Z0_Port2 * (1.0 + Gamma_out) / (1.0 - Gamma_out);

  return std::make_pair(Zin_match, Zout_match);
}


std::complex<double> LoadSpecificationWidget::getReflectionCoefficient() const
{
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_reflectionReal->value(), m_reflectionImag->value());
  } else { // Magnitude/Angle
    double mag = m_reflectionReal->value();
    double ang = m_reflectionImag->value() * M_PI / 180.0; // Convert to radians
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS11() const
{
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s11Real->value(), m_s11Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s11Real->value();
    double ang = m_s11Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS12() const
{
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s12Real->value(), m_s12Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s12Real->value();
    double ang = m_s12Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS21() const
{
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s21Real->value(), m_s21Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s21Real->value();
    double ang = m_s21Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

std::complex<double> LoadSpecificationWidget::getS22() const
{
  if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
    return std::complex<double>(m_s22Real->value(), m_s22Imag->value());
  } else { // Magnitude/Angle
    double mag = m_s22Real->value();
    double ang = m_s22Imag->value() * M_PI / 180.0;
    return std::complex<double>(mag * cos(ang), mag * sin(ang));
  }
}

void LoadSpecificationWidget::setLoadImpedance(const std::complex<double>& impedance)
{
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

void LoadSpecificationWidget::setReflectionCoefficient(const std::complex<double>& gamma)
{
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

void LoadSpecificationWidget::setTwoPortMode(bool enabled)
{
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

void LoadSpecificationWidget::onImpedanceChanged()
{
  if (m_updatingValues) return;
    
  updateReflectionCoefficient();
  emit impedanceChanged();
}

void LoadSpecificationWidget::onReflectionCoefficientChanged()
{
  if (m_updatingValues) return;
    
  updateImpedance();
  emit reflectionCoefficientChanged();
}

void LoadSpecificationWidget::onFormatChanged()
{
  updateImpedanceFormat();
  updateReflectionFormat();
  updateSParameterFormat();
}

void LoadSpecificationWidget::onInputMethodChanged()
{
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

void LoadSpecificationWidget::onBrowseFile()
{
  QString filter;
  if (m_twoPortMode) {
    filter = "S2P Files (*.s2p);;All Files (*)";
  } else {
    filter = "S1P Files (*.s1p);;All Files (*)";
  }

  QString fileName = QFileDialog::getOpenFileName(this, "Select file", "", filter);

  if (!fileName.isEmpty()) {
    m_currentFile = fileName;
    m_fileLabel->setText(QFileInfo(fileName).fileName());

    if (m_twoPortMode) {
      loadS2PFile(fileName);
    } else {
      loadS1PFile(fileName);
    }
  }
}

void LoadSpecificationWidget::onSParameterChanged()
{
  if (m_updatingValues) return;
    emit sParametersChanged();
}

void LoadSpecificationWidget::updateReflectionCoefficient()
{
  if (m_updatingValues) return;
    
  m_updatingValues = true;

  std::complex<double> ZL = getLoadImpedance();
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

void LoadSpecificationWidget::updateImpedance()
{
  if (m_updatingValues) return;
    
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

void LoadSpecificationWidget::updateImpedanceFormat()
{
  std::complex<double> impedance = getLoadImpedance();

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

void LoadSpecificationWidget::updateReflectionFormat()
{
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

void LoadSpecificationWidget::updateSParameterFormat()
{
  if (!m_twoPortMode) return;
    
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

void LoadSpecificationWidget::loadS1PFile(const QString& filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Error", "Cannot open file: " + filename);
    return;
  }

  QTextStream in(&file);
  QString line;
  bool foundData = false;
  double freq = 0;
  std::complex<double> s11;

  while (!in.atEnd()) {
    line = in.readLine().trimmed();

    // Skip comments and empty lines
    if (line.isEmpty() || line.startsWith('!') || line.startsWith('#'))
      continue;
            
    // Parse frequency unit and format from header
    if (line.startsWith('#')) {
      // TODO: Parse S1P header for frequency unit and format
      continue;
    }

    QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (parts.size() >= 3) {
      bool ok1, ok2, ok3;
      freq = parts[0].toDouble(&ok1);
      double real_or_mag = parts[1].toDouble(&ok2);
      double imag_or_ang = parts[2].toDouble(&ok3);

      if (ok1 && ok2 && ok3) {
        // Assume RI format for now (could be enhanced to parse format from header)
        s11 = std::complex<double>(real_or_mag, imag_or_ang);
        foundData = true;
        break; // Use first frequency point
      }
    }
  }

  if (foundData) {
    // Convert S11 to load impedance: ZL = Z0 * (1 + S11) / (1 - S11)
    std::complex<double> ZL = m_Z0 * (1.0 + s11) / (1.0 - s11);
    setLoadImpedance(ZL);
    emit impedanceChanged();
  } else {
    QMessageBox::warning(this, "Error", "No valid data found in S1P file");
  }
}

void LoadSpecificationWidget::loadS2PFile(const QString& filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "Error", "Cannot open file: " + filename);
    return;
  }

  QTextStream in(&file);
  QString line;
  bool foundData = false;
  double freq = 0;
  std::complex<double> s11, s12, s21, s22;

  while (!in.atEnd()) {
    line = in.readLine().trimmed();

    // Skip comments and empty lines
    if (line.isEmpty() || line.startsWith('!'))
      continue;
            
    // Parse frequency unit and format from header
    if (line.startsWith('#')) {
      // TODO: Parse S2P header for frequency unit and format
      continue;
    }

    QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (parts.size() >= 9) {
      bool ok[9];
      freq = parts[0].toDouble(&ok[0]);
      double s11_a = parts[1].toDouble(&ok[1]);
      double s11_b = parts[2].toDouble(&ok[2]);
      double s21_a = parts[3].toDouble(&ok[3]);
      double s21_b = parts[4].toDouble(&ok[4]);
      double s12_a = parts[5].toDouble(&ok[5]);
      double s12_b = parts[6].toDouble(&ok[6]);
      double s22_a = parts[7].toDouble(&ok[7]);
      double s22_b = parts[8].toDouble(&ok[8]);

      bool allOk = true;
      for (int i = 0; i < 9; i++) {
        if (!ok[i]) {
          allOk = false;
          break;
        }
      }

      if (allOk) {
        // Assume RI format for now (could be enhanced to parse format from header)
        s11 = std::complex<double>(s11_a, s11_b);
        s12 = std::complex<double>(s12_a, s12_b);
        s21 = std::complex<double>(s21_a, s21_b);
        s22 = std::complex<double>(s22_a, s22_b);
        foundData = true;
        break; // Use first frequency point
      }
    }
  }

  if (foundData) {
    m_updatingValues = true;

    // Set S-parameter values based on current format
    if (m_formatCombo->currentIndex() == 0) { // Real/Imaginary
      m_s11Real->setValue(s11.real());
      m_s11Imag->setValue(s11.imag());
      m_s12Real->setValue(s12.real());
      m_s12Imag->setValue(s12.imag());
      m_s21Real->setValue(s21.real());
      m_s21Imag->setValue(s21.imag());
      m_s22Real->setValue(s22.real());
      m_s22Imag->setValue(s22.imag());
    } else { // Magnitude/Angle
      m_s11Real->setValue(std::abs(s11));
      m_s11Imag->setValue(std::arg(s11) * 180.0 / M_PI);
      m_s12Real->setValue(std::abs(s12));
      m_s12Imag->setValue(std::arg(s12) * 180.0 / M_PI);
      m_s21Real->setValue(std::abs(s21));
      m_s21Imag->setValue(std::arg(s21) * 180.0 / M_PI);
      m_s22Real->setValue(std::abs(s22));
      m_s22Imag->setValue(std::arg(s22) * 180.0 / M_PI);
    }

    m_updatingValues = false;
    emit sParametersChanged();
  } else {
    QMessageBox::warning(this, "Error", "No valid data found in S2P file");
  }
}

void LoadSpecificationWidget::setCollapsed(bool collapsed)
{
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

void LoadSpecificationWidget::mousePressEvent(QMouseEvent* event)
{
  // Check if click is in the title area (first 25 pixels from top)
  if (event->position().y() <= 25) {
    onToggleCollapse();
  }
  QGroupBox::mousePressEvent(event);
}

void LoadSpecificationWidget::onToggleCollapse()
{
  setCollapsed(!m_isCollapsed);
}


std::array<std::complex<double>, 4> LoadSpecificationWidget::getSParameters() const {
  return { getS11(), getS12(), getS21(), getS22() };
}
