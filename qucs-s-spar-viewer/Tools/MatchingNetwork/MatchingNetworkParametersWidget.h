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

#ifndef MATCHINGNETWORKPARAMETERSWIDGET_H
#define MATCHINGNETWORKPARAMETERSWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>
#include <complex>

#include "../../Schematic/structures.h"

class MatchingNetworkParametersWidget : public QGroupBox {
  Q_OBJECT

public:
  explicit MatchingNetworkParametersWidget(QWidget* parent = nullptr);
  ~MatchingNetworkParametersWidget();

  // Public method to get the design parameters
  MatchingNetworkDesignParameters getDesignParameters() const;

  // Public method to get the current topology index
  int getCurrentTopologyIndex() const;

  // Public method to get the frequency scaling
  double getScaleFreq(int index) const;

  bool isCollapsed() const { return m_isCollapsed; }
  void setCollapsed(bool collapsed);
  void setTitle(QString title);

protected:
  void mousePressEvent(QMouseEvent* event) override;

public slots:
  void onTopologyChanged(int index);

private slots:
  void onParameterChanged();
  void onToggleCollapse();

  // Required to adjust the visility of the ripple in the Chebyshev transformer
  void adjustChebyshevRippleVisibility();

private:
  // UI Components

  QGridLayout* m_mainLayout;
  QWidget* m_contentWidget;
  QPushButton* m_toggleButton;

  QLabel* titleLabel;

  QLabel* Topology_Label;
  QComboBox* Topology_Combo;

  QWidget* SolutionWidget;
  QRadioButton* Solution1_RB;
  QRadioButton* Solution2_RB;

  QLabel* StubTermination_Label;
  QComboBox* StubTermination_ComboBox;

  QGroupBox* Weighting_GroupBox;
  QComboBox* Weighting_Combo;
  QLabel* Ripple_Label;
  QDoubleSpinBox* Ripple_SpinBox;

  QLabel* Sections_Label;
  QSpinBox* Sections_SpinBox;

  QLabel* Zin_Label;
  QDoubleSpinBox* ZinRSpinBox;
  QLabel* Ohm_Zin_Label;

  QGroupBox* groupBox;

  // Transmission line implementation
  QLabel* TL_Implementation_Label;
  QComboBox* TL_Implementation_Combo;

  // Layout
  QGridLayout* mainLayout;

  void setupUI();
  void connectSignals();

  // State
  bool m_isCollapsed;

signals:
  void parametersChanged();
  void topologyChanged(int index);
  void collapsedStateChanged(bool collapsed);
};

#endif // MATCHINGNETWORKPARAMETERSWIDGET_H
