/// @file MatchingNetworkParametersWidget.cpp
/// @brief GUI selecting the matching network topology parameters
/// (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "MatchingNetworkParametersWidget.h"

MatchingNetworkParametersWidget::MatchingNetworkParametersWidget(
    QWidget *parent)
    : QGroupBox(parent), m_isCollapsed(false) {
  setupUI();
  connectSignals();
}

void MatchingNetworkParametersWidget::setupUI() {

  // Create main layout for the group box
  QVBoxLayout *groupLayout = new QVBoxLayout(this);
  groupLayout->setSpacing(0);

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
  titleLabel = new QLabel("Matching Network Settings");
  headerLayout->addWidget(titleLabel);
  headerLayout->addStretch();

  // Create content widget that will be hidden/shown
  m_contentWidget = new QWidget();
  mainLayout = new QGridLayout(m_contentWidget);

  int layout_row =
      0; // Row index. This is useful to add a new line on the layout without
         // the need of modifying manually all the widgets.

  // Topology
  Topology_Label = new QLabel("Topology");
  Topology_Combo = new QComboBox();
  QStringList matching_methods;
  matching_methods.append(tr("L-section"));
  matching_methods.append(tr("Single stub"));
  matching_methods.append(tr("Double stub"));
  matching_methods.append(
      QString("%1 %2/4").arg(tr("Multisection "), QString(QChar(0xBB, 0x03))));
  matching_methods.append(tr("Cascaded L-sections"));
  matching_methods.append(QString("%1/8 + %1/4 line").arg(QChar(0xBB, 0x03)));
  Topology_Combo->addItems(matching_methods);
  mainLayout->addWidget(Topology_Label, layout_row, 0);
  mainLayout->addWidget(Topology_Combo, layout_row, 1);

  // Solution number widget
  SolutionWidget = new QWidget();
  QHBoxLayout *SolutionLayout = new QHBoxLayout();
  Solution1_RB = new QRadioButton("Solution 1");
  Solution2_RB = new QRadioButton("Solution 2");
  Solution1_RB->setChecked(true);
  SolutionLayout->addWidget(Solution1_RB);
  SolutionLayout->addWidget(Solution2_RB);
  SolutionWidget->setLayout(SolutionLayout);
  mainLayout->addWidget(SolutionWidget, layout_row, 2);

  // Transmission line implementation
  layout_row++;
  TL_Implementation_Label = new QLabel(QString("TLIN implementation"));
  TL_Implementation_Combo = new QComboBox();
  TL_Implementation_Combo->addItem("Ideal");
  TL_Implementation_Combo->addItem("Microstrip");
  // TL_Implementation_Combo->addItem("Stripline");
  mainLayout->addWidget(TL_Implementation_Label, layout_row, 0);
  mainLayout->addWidget(TL_Implementation_Combo, layout_row, 1);

  // Stub termination
  layout_row++;
  StubTermination_Label = new QLabel(QString("Stub Termination"));
  mainLayout->addWidget(StubTermination_Label, layout_row, 0);

  StubTermination_ComboBox = new QComboBox();
  StubTermination_ComboBox->addItem(QString("Open circuit"));
  StubTermination_ComboBox->addItem(QString("Short circuit"));
  mainLayout->addWidget(StubTermination_ComboBox, layout_row, 1);

  // Weighting settings
  layout_row++;
  Weighting_GroupBox = new QGroupBox(tr("Weighting"));
  QGridLayout *WeightingLayout = new QGridLayout();

  // Weighting method combobox
  QLabel *WeightingMethodLabel = new QLabel(tr("Method"));
  Weighting_Combo = new QComboBox();
  Weighting_Combo->addItem(tr("Binomial"));
  Weighting_Combo->addItem(tr("Chebyshev"));
  WeightingLayout->addWidget(WeightingMethodLabel, 0, 0);
  WeightingLayout->addWidget(Weighting_Combo, 0, 1);

  // Ripple parameter
  Ripple_Label = new QLabel(tr("Ripple"));
  Ripple_SpinBox = new QDoubleSpinBox();
  Ripple_SpinBox->setRange(0.001, 1.0);
  Ripple_SpinBox->setSingleStep(0.01);
  Ripple_SpinBox->setDecimals(3);
  Ripple_SpinBox->setValue(0.05);
  WeightingLayout->addWidget(Ripple_Label, 1, 0);
  WeightingLayout->addWidget(Ripple_SpinBox, 1, 1);
  Weighting_GroupBox->setLayout(WeightingLayout);
  mainLayout->addWidget(Weighting_GroupBox, layout_row, 0, 1, 3);

  // Hide ripple controls if Binomial selected
  Ripple_Label->setVisible(false);
  Ripple_SpinBox->setVisible(false);

  // Number of sections
  layout_row++;
  Sections_Label = new QLabel(tr("Sections"));
  Sections_SpinBox = new QSpinBox();
  Sections_SpinBox->setRange(2, 10);
  Sections_SpinBox->setValue(3);
  mainLayout->addWidget(Sections_Label, layout_row, 0);
  mainLayout->addWidget(Sections_SpinBox, layout_row, 1);

  // Input impedance
  layout_row++;
  Zin_Label = new QLabel("Z0");
  ZinRSpinBox = new QDoubleSpinBox();
  ZinRSpinBox->setMinimum(0.5);
  ZinRSpinBox->setMaximum(10000);
  ZinRSpinBox->setSingleStep(0.5);
  ZinRSpinBox->setValue(50);
  ZinRSpinBox->setDecimals(1);
  Ohm_Zin_Label = new QLabel(QChar(0xa9, 0x03));
  mainLayout->addWidget(Zin_Label, layout_row, 0);
  mainLayout->addWidget(ZinRSpinBox, layout_row, 1);
  mainLayout->addWidget(Ohm_Zin_Label, layout_row, 2);

  // Make it more compact
  mainLayout->setVerticalSpacing(1);

  // Add widgets to main group layout
  groupLayout->addWidget(headerWidget);
  groupLayout->addWidget(m_contentWidget);
  groupLayout->setContentsMargins(2, 2, 2, 2);

  // Set initial state
  onTopologyChanged(0);

  // Collapsed by default. The height of this widget affects all the tabs. It's
  // good to keep it as compact as possible
  setCollapsed(true);
}

void MatchingNetworkParametersWidget::connectSignals() {
  connect(Topology_Combo, &QComboBox::currentIndexChanged, this,
          &MatchingNetworkParametersWidget::onTopologyChanged);

  connect(Topology_Combo, &QComboBox::currentIndexChanged, this,
          &MatchingNetworkParametersWidget::topologyChanged);

  connect(TL_Implementation_Combo, &QComboBox::currentIndexChanged, this,
          &MatchingNetworkParametersWidget::onParameterChanged);

  connect(StubTermination_ComboBox, &QComboBox::currentIndexChanged, this,
          &MatchingNetworkParametersWidget::onParameterChanged);

  connect(ZinRSpinBox, &QDoubleSpinBox::valueChanged, this,
          &MatchingNetworkParametersWidget::onParameterChanged);

  connect(Solution1_RB, &QRadioButton::clicked, this,
          [this]() { onParameterChanged(); });

  connect(Solution2_RB, &QRadioButton::clicked, this,
          [this]() { onParameterChanged(); });

  connect(Weighting_Combo, &QComboBox::currentIndexChanged, this,
          &MatchingNetworkParametersWidget::adjustChebyshevRippleVisibility);

  connect(Ripple_SpinBox, &QDoubleSpinBox::valueChanged, this,
          &MatchingNetworkParametersWidget::onParameterChanged);

  connect(Sections_SpinBox, &QSpinBox::valueChanged, this,
          &MatchingNetworkParametersWidget::onParameterChanged);

  connect(m_toggleButton, &QPushButton::clicked, this,
          [this]() { onToggleCollapse(); });
}
void MatchingNetworkParametersWidget::onTopologyChanged(int index) {
  switch (index) {
  case 0: // L-section
    // Show L-section matching solutions
    Solution1_RB->show();
    Solution2_RB->show();

    // Hide TLIN implementation widgets
    TL_Implementation_Label->hide();
    TL_Implementation_Combo->hide();

    // Hide number of sections
    Sections_Label->hide();
    Sections_SpinBox->hide();

    // Hide open circuit termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

    // Hide lambda/4 weighting
    Weighting_GroupBox->hide();
    break;

  case 1: // Single-stub matching
  case 2: // Double-stub matching
    // Hide L-section matching solutions
    Solution1_RB->hide();
    Solution2_RB->hide();

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    // Hide number of sections
    Sections_Label->hide();
    Sections_SpinBox->hide();

    // Show stub termination options
    StubTermination_Label->show();
    StubTermination_ComboBox->show();

    // Hide lambda/4 weighting
    Weighting_GroupBox->hide();
    break;

  case 3: // Multisection lambda/4 transformer
    // Hide L-section matching solutions
    Solution1_RB->hide();
    Solution2_RB->hide();

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    // Show number of sections
    Sections_Label->show();
    Sections_SpinBox->show();

    // Hide stub termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

    // Show lambda/4 weighting
    Weighting_GroupBox->show();

    adjustChebyshevRippleVisibility();
    break;

  case 4: // Cascaded L-sections transformer
    // Show L-section matching solutions
    Solution1_RB->show();
    Solution2_RB->show();

    // Hide TLIN implementation widgets
    TL_Implementation_Label->hide();
    TL_Implementation_Combo->hide();

    // Show number of sections
    Sections_Label->show();
    Sections_SpinBox->show();

    // Hide stub termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

    // Hide lambda/4 weighting
    Weighting_GroupBox->hide();
    break;

  case 5: // lambda/8 + lambda/4
    // Hide L-section matching solutions
    Solution1_RB->hide();
    Solution2_RB->hide();

    // Show TLIN implementation widgets
    TL_Implementation_Label->show();
    TL_Implementation_Combo->show();

    // Hide number of sections
    Sections_Label->hide();
    Sections_SpinBox->hide();

    // Hide stub termination options
    StubTermination_Label->hide();
    StubTermination_ComboBox->hide();

    // Hide lambda/4 weighting
    Weighting_GroupBox->hide();
    break;

  default:
    break;
  }

  onParameterChanged();
}

void MatchingNetworkParametersWidget::adjustChebyshevRippleVisibility() {
  // Check response type and adjust the ripple visibility
  if (Weighting_Combo->currentText() == QString("Chebyshev")) {
    Ripple_Label->setVisible(true);
    Ripple_SpinBox->setVisible(true);
  } else {
    Ripple_Label->setVisible(false);
    Ripple_SpinBox->setVisible(false);
  }
  onParameterChanged();
}

void MatchingNetworkParametersWidget::onParameterChanged() {
  emit parametersChanged();
}

MatchingNetworkDesignParameters
MatchingNetworkParametersWidget::getDesignParameters() const {
  MatchingNetworkDesignParameters specs;

  specs.Z0 = ZinRSpinBox->value();
  specs.Topology = Topology_Combo->currentIndex();

  if (Solution1_RB->isChecked()) {
    specs.Solution = 1;
  } else {
    specs.Solution = 2;
  }

  // Single/double stub matching
  specs.OpenShort = StubTermination_ComboBox->currentIndex();

  // Multisection lambda/4 transformers
  specs.NSections = Sections_SpinBox->value();
  specs.Weigthing = Weighting_Combo->currentText();
  specs.gamma_MAX = Ripple_SpinBox->value();

  ////////////////////////////////////////////////////////////////////////////
  // Transmission line implementation
  static const QMap<QString, TransmissionLineType> tlMap{
      {"Ideal", TransmissionLineType::Ideal},
      {"Microstrip", TransmissionLineType::MLIN},
      {"Stripline", TransmissionLineType::SLIN}};

  const QString tlKey = TL_Implementation_Combo->currentText();
  if (tlMap.contains(tlKey)) {
    specs.TL_implementation = tlMap.value(tlKey);
  }
  ////////////////////////////////////////////////////////////////////////////

  return specs;
}

int MatchingNetworkParametersWidget::getCurrentTopologyIndex() const {
  return Topology_Combo->currentIndex();
}

double MatchingNetworkParametersWidget::getScaleFreq(int index) const {
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

void MatchingNetworkParametersWidget::setCollapsed(bool collapsed) {
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

void MatchingNetworkParametersWidget::mousePressEvent(QMouseEvent *event) {
  // Check if click is in the title area (first 25 pixels from top)
  if (event->position().y() <= 25) {
    onToggleCollapse();
  }
  QGroupBox::mousePressEvent(event);
}
