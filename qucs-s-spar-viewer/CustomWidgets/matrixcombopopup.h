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

class MatrixComboPopup : public QFrame {
  Q_OBJECT

public:
  MatrixComboPopup(const QStringList& sParams, const QStringList& otherParams,
                   QComboBox* parent = nullptr);
  void showBelow(QWidget* widget);

private slots:
  void selectItem();

private:
  QComboBox* parentCombo;
};

class MatrixComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit MatrixComboBox(QWidget* parent = nullptr);
  void setParameters(const QStringList& sParams,
                     const QStringList& otherParams);

protected:
  void showPopup() override;
  void hidePopup() override;
  void paintEvent(QPaintEvent* event) override;

private:
  QStringList sParams;
  QStringList otherParams;
  bool popupVisible;
  MatrixComboPopup* popup;
};

#endif // MATRIXCOMBOBOX_H
