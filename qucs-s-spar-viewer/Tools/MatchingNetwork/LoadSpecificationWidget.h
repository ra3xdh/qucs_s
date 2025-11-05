/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

#ifndef LOADSPECIFICATIONWIDGET_H
#define LOADSPECIFICATIONWIDGET_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>
#include <cmath>
#include <complex>

#include "../../Misc/general.h"

class LoadSpecificationWidget : public QGroupBox {
  Q_OBJECT

public:
  explicit LoadSpecificationWidget(QWidget* parent = nullptr);
  ~LoadSpecificationWidget();

  // Getters
  std::array<std::complex<double>, 4> getSParameters() const;
  std::complex<double> getLoadImpedance_At_Fmatch() const;
  QList<std::complex<double>> getZLdata();
  QList<double> getFrequency();
  std::pair<std::complex<double>, std::complex<double>>
  getTwoPortMatchingImpedances() const;
  std::complex<double> getReflectionCoefficient() const;
  QString getSparFilePath();
  bool isTwoPortMode() const { return m_twoPortMode; }

  // S-parameter getters (for two-port mode)
  std::complex<double> getS11() const;
  std::complex<double> getS12() const;
  std::complex<double> getS21() const;
  std::complex<double> getS22() const;

  // Setters
  void setLoadImpedance(const std::complex<double>& impedance);
  void setReflectionCoefficient(const std::complex<double>& gamma);
  void setTwoPortMode(bool enabled);
  void setReferenceImpedance(double Z0) {
    m_Z0 = Z0;
    updateReflectionCoefficient();
  }
  void setCollapsed(bool collapsed);
  void setFmatch(double freq) { f_match = freq; }
  bool isCollapsed() const { return m_isCollapsed; }

  // Reference impedance of the source and the load port. Required for the
  // 2-port matching. This data is provided by the main widget
  double Z0_Port1, Z0_Port2;

protected:
  void mousePressEvent(QMouseEvent* event) override;

private slots:
  void onImpedanceChanged();
  void onReflectionCoefficientChanged();
  void onFormatChanged();
  void onInputMethodChanged();
  void onBrowseFile();
  void onSParameterChanged();
  void onToggleCollapse();

private:
  void setupUI();
  void setupOnePortUI();
  void setupTwoPortUI();
  void updateReflectionCoefficient();
  void updateImpedance();
  void updateSParameterDisplays();
  void updateImpedanceFormat();
  void updateReflectionFormat();
  void updateSParameterFormat();

  double f_match;

  QMap<QString, QList<double>> loadData; // It could be either 1-port or 2-port
  QString spar_file_path; // Path to the S-parameter file (this is required for
                          // simulation)

  // UI Components - One Port
  QGridLayout* m_mainLayout;
  QWidget* m_contentWidget;
  QPushButton* m_toggleButton;

  // Input method selection
  QRadioButton* m_manualInputRadio;
  QRadioButton* m_fileInputRadio;
  QButtonGroup* m_inputMethodGroup;
  QPushButton* m_browseButton;
  QLabel* m_fileLabel;

  // Format selection
  QLabel* m_formatLabel;
  QComboBox* m_formatCombo;

  // Manual input widgets - Impedance
  QLabel* m_impedanceLabel;
  QDoubleSpinBox* m_impedanceReal;
  QLabel* m_impedanceSeparator;
  QDoubleSpinBox* m_impedanceImag;
  QLabel* m_impedanceUnit;

  // Manual input widgets - Reflection coefficient
  QLabel* m_reflectionLabel;
  QDoubleSpinBox* m_reflectionReal;
  QLabel* m_reflectionSeparator;
  QDoubleSpinBox* m_reflectionImag;

  // Two-port widgets
  QWidget* m_twoPortWidget;
  QGridLayout* m_twoPortLayout;

  // S-parameter widgets
  QLabel* m_s11Label;
  QDoubleSpinBox* m_s11Real;
  QLabel* m_s11Separator;
  QDoubleSpinBox* m_s11Imag;

  QLabel* m_s12Label;
  QDoubleSpinBox* m_s12Real;
  QLabel* m_s12Separator;
  QDoubleSpinBox* m_s12Imag;

  QLabel* m_s21Label;
  QDoubleSpinBox* m_s21Real;
  QLabel* m_s21Separator;
  QDoubleSpinBox* m_s21Imag;

  QLabel* m_s22Label;
  QDoubleSpinBox* m_s22Real;
  QLabel* m_s22Separator;
  QDoubleSpinBox* m_s22Imag;

  // Internal state
  bool m_twoPortMode;
  bool m_updatingValues;
  bool m_isCollapsed;
  double m_Z0; // Reference impedance
  QString m_currentFile;

signals:
  void impedanceChanged();
  void reflectionCoefficientChanged();
  void sParametersChanged();
  void collapsedStateChanged(bool collapsed);
};

#endif // LOADSPECIFICATIONWIDGET_H
