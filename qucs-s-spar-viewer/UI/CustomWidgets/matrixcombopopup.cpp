/// @file matrixcombopopup.cpp
/// @brief Custom popup displaying S-parameters in matrix layout with other
/// parameters below (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "matrixcombopopup.h"

MatrixComboPopup::MatrixComboPopup(const QStringList &sParams,
                                   const QStringList &otherParams,
                                   QComboBox *parent)
    : QFrame(parent), parentCombo(parent) {
  // Style as a popup menu
  setFrameStyle(QFrame::Panel | QFrame::Plain);
  setLineWidth(1);

  // Use popup window flags so it behaves like a dropdown
  setWindowFlags(Qt::Popup | Qt::FramelessWindowHint |
                 Qt::NoDropShadowWindowHint);

  // Apply a shadow to the whole popup
  QGraphicsDropShadowEffect *popupShadow = new QGraphicsDropShadowEffect(this);
  popupShadow->setBlurRadius(15);
  popupShadow->setColor(QColor(0, 0, 0, 80));
  popupShadow->setOffset(0, 2);
  setGraphicsEffect(popupShadow);

  // Set background color
  setStyleSheet("background-color: white;");

  // Main layout
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(4, 4, 4, 4);
  mainLayout->setSpacing(2);

  // Create matrix layout for S-parameters
  QGridLayout *matrixLayout = new QGridLayout();
  matrixLayout->setSpacing(2);

  // Calculate the size of the matrix (assuming it's square)
  int matrixSize =
      static_cast<int>(std::sqrt(static_cast<double>(sParams.size())));
  if (matrixSize * matrixSize < sParams.size()) {
    matrixSize++; // Adjust if it's not a perfect square
  }

  int paramIndex = 0;
  for (int i = 0; i < matrixSize && paramIndex < sParams.size(); i++) {
    for (int j = 0; j < matrixSize && paramIndex < sParams.size(); j++) {
      HoverButton *button = new HoverButton(sParams[paramIndex], this);
      button->setProperty("paramName", sParams[paramIndex]);
      connect(button, &QPushButton::clicked, this,
              &MatrixComboPopup::selectItem);
      matrixLayout->addWidget(button, i, j);
      paramIndex++;
    }
  }

  mainLayout->addLayout(matrixLayout);

  // Add separator
  QFrame *separator = new QFrame(this);
  separator->setFrameShape(QFrame::HLine);
  separator->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(separator);

  // Add other parameters as a list
  for (const QString &param : otherParams) {
    HoverButton *button = new HoverButton(param, this);
    button->setProperty("paramName", param);
    connect(button, &QPushButton::clicked, this, &MatrixComboPopup::selectItem);
    mainLayout->addWidget(button);
  }

  // Install event filter to detect clicks outside
  qApp->installEventFilter(this);
}

void MatrixComboPopup::showBelow(QWidget *widget) {
  // Set the width to match the parent widget
  setMinimumWidth(widget->width());
  adjustSize();

  // Position directly below the parent widget
  QPoint pos = widget->mapToGlobal(QPoint(0, widget->height()));

  // Ensure the popup is fully visible on screen
  QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
  int popupHeight = height();
  int popupWidth = width();

  // If popup would go below screen, show it above the widget instead
  if (pos.y() + popupHeight > screenGeometry.bottom()) {
    pos.setY(widget->mapToGlobal(QPoint(0, 0)).y() - popupHeight);
  }

  // If popup would go off right edge, align with right edge of widget
  if (pos.x() + popupWidth > screenGeometry.right()) {
    pos.setX(widget->mapToGlobal(QPoint(widget->width() - popupWidth, 0)).x());
  }

  move(pos);
  show();
  setFocus();
}

void MatrixComboPopup::selectItem() {
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  if (button && parentCombo) {
    QString text = button->property("paramName").toString();
    int index = parentCombo->findText(text);
    if (index >= 0) {
      parentCombo->setCurrentIndex(index);
    }
    close();
  }
}

// Constructor
MatrixComboBox::MatrixComboBox(QWidget *parent)
    : QComboBox(parent), popupVisible(false), popup(nullptr) {}

void MatrixComboBox::setParameters(const QStringList &sParams,
                                   const QStringList &otherParams) {
  this->sParams = sParams;
  this->otherParams = otherParams;

  // Add all items to the standard combo box
  clear();
  addItems(sParams + otherParams);
}

void MatrixComboBox::showPopup() {
  // Don't use the standard popup
  if (popup) {
    delete popup;
  }

  popupVisible = true;
  update(); // Update appearance

  // Create our custom popup
  popup = new MatrixComboPopup(sParams, otherParams, this);

  // When popup is closed
  connect(popup, &QWidget::destroyed, [this]() {
    popup = nullptr;
    popupVisible = false;
    update();
  });

  // Show it directly below this widget
  popup->showBelow(this);
}

void MatrixComboBox::hidePopup() {
  if (popup) {
    popup->close();
    popup = nullptr;
  }
  popupVisible = false;
  update();
}

void MatrixComboBox::paintEvent(QPaintEvent *event) {
  QStyleOptionComboBox opt;
  initStyleOption(&opt);

  if (popupVisible) {
    opt.state |= QStyle::State_On;
  }

  QComboBox::paintEvent(event);
}
