/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MATRIXCOMBOBOX_H
#define MATRIXCOMBOBOX_H

#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QPaintEvent>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QApplication>
#include <QEvent>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHoverEvent>
#include <QScreen>
#include <QScrollArea>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <cmath> // for sqrt

///
/// @brief Custom popup displaying S-parameters in matrix layout with other parameters below
///
class MatrixComboPopup : public QFrame {
  Q_OBJECT

public:
  ///
  /// @brief Constructor
  /// @param sParams S-parameter list for matrix layout
  /// @param otherParams Additional parameters shown as list
  /// @param parent Parent combo box
  ///
  MatrixComboPopup(const QStringList& sParams, const QStringList& otherParams,
                   QComboBox* parent = nullptr);

  ///
  /// @brief Shows popup below specified widget
  /// @param widget Widget to align below
  ///
  void showBelow(QWidget* widget);

private slots:
    ///
    /// @brief Handles item selection from popup
    ///
  void selectItem();

private:
  QComboBox* parentCombo;
};

///
/// @brief ComboBox with custom matrix-style popup for parameter selection
///
class MatrixComboBox : public QComboBox {
  Q_OBJECT

public:
  ///
  /// @brief Constructor
  /// @param parent Parent widget
  ///
  explicit MatrixComboBox(QWidget* parent = nullptr);

  ///
  /// @brief Sets parameters to display in popup
  /// @param sParams S-parameters for matrix layout
  /// @param otherParams Additional parameters for list
  ///
  void setParameters(const QStringList& sParams,
                     const QStringList& otherParams);

protected:
  ///
  /// @brief Shows custom matrix popup instead of standard dropdown
  ///
  void showPopup() override;

  ///
  /// @brief Hides the custom popup
  ///
  void hidePopup() override;

  ///
  /// @brief Custom paint to show popup state
  /// @param event Paint event
  ///
  void paintEvent(QPaintEvent* event) override;

private:
  QStringList sParams;
  QStringList otherParams;
  bool popupVisible;
  MatrixComboPopup* popup;
};

///
/// @brief Custom button with hover effect and shadow
///
class HoverButton : public QPushButton {
public:
  ///
  /// @brief Constructor
  /// @param text Button text
  /// @param parent Parent widget
  ///
  HoverButton(const QString& text, QWidget* parent = nullptr)
      : QPushButton(text, parent) {
    setFlat(true);
    setMouseTracking(true);

           // Create initial style
    setStyleSheet("QPushButton {"
                  "   padding: 5px;"
                  "   border: none;"
                  "   background-color: transparent;"
                  "   text-align: left;"
                  "}"
                  "QPushButton:hover {"
                  "   background-color: rgba(0, 0, 0, 10%);"
                  "   border-radius: 3px;"
                  "}");

           // Create shadow effect (hidden by default)
    shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 0);
    shadowEffect->setEnabled(false);
    setGraphicsEffect(shadowEffect);
  }

protected:
  ///
  /// @brief Handles hover events to show/hide shadow
  /// @param event Event to process
  /// @return Event handling result
  ///
  bool event(QEvent* event) override {
    if (event->type() == QEvent::HoverEnter) {
      shadowEffect->setEnabled(true);
    } else if (event->type() == QEvent::HoverLeave) {
      shadowEffect->setEnabled(false);
    }
    return QPushButton::event(event);
  }

private:
  QGraphicsDropShadowEffect* shadowEffect;
};



#endif // MATRIXCOMBOBOX_H
