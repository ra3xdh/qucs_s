/// @file netpropertiesdialog.cpp
/// @brief Dialog for setting the style and the name of a net (class implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date July 25, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "netpropertiesdialog.h"
#include "extsimkernels/spicecompat.h"
#include "main.h"

#include <QColorDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>


NetPropertiesDialog::NetPropertiesDialog(const QColor& initialColor, int initialLineWidth,
                                         const QString& initialName, const QString& initialValue,
                                         const QString& netlistName,
                                         QWidget *parent)
    : QDialog(parent), m_netcolor(initialColor), m_lineWidth(initialLineWidth) {

  setWindowTitle(tr("Net Properties"));

  QVBoxLayout *layout = new QVBoxLayout(this);

  // --- Net info group box ---
  QGroupBox *infoGroup = new QGroupBox(tr("Net info"), this);
  QGridLayout *infoLayout = new QGridLayout(infoGroup);
  infoLayout->addWidget(new QLabel(tr("Netlist name:"), this), 0, 0);
  m_netlistNameLabel = new QLabel(netlistName.isEmpty() ? tr("(auto)") : netlistName, this);
  m_netlistNameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
  infoLayout->addWidget(m_netlistNameLabel, 0, 1, 1, 2);
  layout->addWidget(infoGroup);

  // --- Label group box ---
  QGroupBox *labelGroup = new QGroupBox(tr("Label"), this);
  QGridLayout *labelLayout = new QGridLayout(labelGroup);

  labelLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);

  Expr_NetName.setPattern("[a-zA-Z]([0-9a-zA-Z]|_(?!_))+\\!{0,1}");
  Validator_NetName = new QRegularExpressionValidator(Expr_NetName, this);
  NodeName = new QLineEdit(this);
  NodeName->setText(initialName);
  NodeName->setValidator(Validator_NetName);
  labelLayout->addWidget(NodeName, 0, 1, 1, 2);

  labelLayout->addWidget(new QLabel(tr("Initial voltage:"), this), 1, 0);
  InitValue = new QLineEdit(this);
  InitValue->setText(initialValue);

  // Set validator for the initial voltage: Ensure that's a number and handles suffixes
  Expr_InitialVoltage.setPattern(
      "^[+-]?(\\d+\\.?\\d*|\\.\\d+)([eE][+-]?\\d+)?(MEG|T|G|K|M|U|N|P|F)?$");
  Expr_InitialVoltage.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  Validator_InitialVoltage = new QRegularExpressionValidator(Expr_InitialVoltage, this);
  InitValue->setValidator(Validator_InitialVoltage);
  labelLayout->addWidget(InitValue, 1, 1, 1, 2);
  layout->addWidget(labelGroup);

  // --- Style group box ---
  QGroupBox *styleGroup = new QGroupBox(tr("Style"), this);
  QVBoxLayout *styleLayout = new QVBoxLayout(styleGroup);

  QHBoxLayout *colorRow = new QHBoxLayout();
  colorRow->addWidget(new QLabel(tr("Net color:"), this));

  chooseColorBtn = new QPushButton();
  connect(chooseColorBtn, &QPushButton::clicked, this, &NetPropertiesDialog::slotChooseColor);
  updateColorButtonBackground(m_netcolor);
  colorRow->addWidget(chooseColorBtn);
  styleLayout->addLayout(colorRow);

  QHBoxLayout *widthRow = new QHBoxLayout();
  widthRow->addWidget(new QLabel(tr("Line width:"), this));
  m_widthSpin = new QSpinBox(this);
  m_widthSpin->setRange(1, 10);
  m_widthSpin->setValue(initialLineWidth);
  connect(m_widthSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v){ m_lineWidth = v; });
  widthRow->addWidget(m_widthSpin);
  widthRow->addStretch();

  QPushButton *defaultBtn = new QPushButton(tr("Default Style"), this);
  connect(defaultBtn, &QPushButton::clicked, this, &NetPropertiesDialog::slotUseDefaultStyle);
  widthRow->addWidget(defaultBtn);
  widthRow->addStretch();
  styleLayout->addLayout(widthRow);
  layout->addWidget(styleGroup);

  // --- OK / Cancel ---
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &NetPropertiesDialog::slotOk);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &NetPropertiesDialog::slotCancel);
  layout->addWidget(buttonBox);

  setFocusProxy(NodeName);
}

void NetPropertiesDialog::slotChooseColor()
{
  QColor initial_color;

  if (m_netcolor.isValid()){
    initial_color = m_netcolor;
  } else {
    // Default color
    initial_color = Qt::darkBlue;
  }
  QColor chosen = QColorDialog::getColor(initial_color, this, tr("Select Net Color"));
  if (chosen.isValid()) {
    m_netcolor = chosen;
    // Update the background color of the color-picking button
    updateColorButtonBackground(m_netcolor);
  }
}

void NetPropertiesDialog::slotUseDefaultStyle()
{
  // Default color
  m_netcolor = Qt::darkBlue;

  // Default linewidth
  m_lineWidth = 2;

  // Update the background color of the color-picking button
  updateColorButtonBackground(m_netcolor);

  // Update width sinbox
  m_widthSpin->setValue(m_lineWidth);
}


void NetPropertiesDialog::updateColorButtonBackground(QColor background_color){
  QString styleSheet = QStringLiteral("QPushButton { background-color: %1; }")
  .arg(background_color.name());
  chooseColorBtn->setStyleSheet(styleSheet);
  chooseColorBtn->setAttribute(Qt::WA_TranslucentBackground);
}

void NetPropertiesDialog::slotCancel()
{
  reject();
}

void NetPropertiesDialog::slotOk()
{
  // First check ensure that the user has entered a name for the label. That's mandatory.
  QString name = NodeName->text().trimmed();
  QString initialVoltage = InitValue->text().trimmed();

  if (name.isEmpty() && !initialVoltage.isEmpty()) {
    name = QString("Vstart_%1V").arg(initialVoltage);
  }

  if ((QucsSettings.DefaultSimulator == spicecompat::simNgspice) ||
      (QucsSettings.DefaultSimulator == spicecompat::simSpiceOpus)) {
    QString nod = NodeName->text().trimmed();
    if (!nod.isEmpty() && !spicecompat::check_nodename(nod)) {
      QMessageBox::warning(this, tr("SPICE checker"),
                           QString(tr("Node name \"%1\" is Nutmeg reserved keyword!\n"
                                      "Please select another node name!")).arg(nod),
                           QMessageBox::Ok);
      return;   // stay open — don't discard the color/width choice too
    }
  }
  NodeName->setText(name);
  InitValue->setText(initialVoltage);
  accept();
}