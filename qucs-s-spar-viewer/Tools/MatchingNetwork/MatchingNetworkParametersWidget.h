/// @file MatchingNetworkParametersWidget.cpp
/// @brief GUI selecting the matching network topology parameters (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later
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

/// @class MatchingNetworkParametersWidget
/// @brief Widget for entering matching network design parameters
class MatchingNetworkParametersWidget : public QGroupBox {
  Q_OBJECT

public:
  /// @brief Constructor
  /// @param parent Parent widget
  explicit MatchingNetworkParametersWidget(QWidget* parent = nullptr);

  /// @brief Class destructor
  ~MatchingNetworkParametersWidget() {}

  /// @brief Get the design parameters
  /// @return Structure containing all design parameters
  MatchingNetworkDesignParameters getDesignParameters() const;

  /// @brief Get the current topology index
  /// @return Index of selected topology
  int getCurrentTopologyIndex() const;

  /// @brief Get the frequency scaling multiplier
  /// @param index Scale index (0=GHz, 1=MHz, 2=kHz, 3=Hz)
  /// @return Scale multiplier
  double getScaleFreq(int index) const;

  /// @brief Check if widget is collapsed
  /// @return True if collapsed, false otherwise
  bool isCollapsed() const { return m_isCollapsed; }

  /// @brief Set collapsed state of widget
  /// @param collapsed True to collapse, false to expand
  void setCollapsed(bool collapsed);

  /// @brief Set the title of the widget
  /// @param title Title text
  void setTitle(QString title) {
    titleLabel->setText(title);
  }

protected:
  /// @brief Handle mouse press events for collapsible header
  /// @param event Mouse event
  void mousePressEvent(QMouseEvent* event) override;

public slots:
  /// @brief Handle topology selection changes
  /// @param index Selected topology index
  void onTopologyChanged(int index);

private slots:
  /// @brief Handle parameter value changes
  void onParameterChanged();

  /// @brief Toggle collapse/expand state
  void onToggleCollapse(){
    setCollapsed(!m_isCollapsed);
  }

  /// @brief Adjust visibility of the Chebyshev transformer ripple
  void adjustChebyshevRippleVisibility();

private:
  /// @brief Setup UI components
  void setupUI();

  /// @brief Connect signal handlers
  void connectSignals();

  /// @name UI Components - Common
  /// @{
  QGridLayout* m_mainLayout;
  QWidget* m_contentWidget;
  QPushButton* m_toggleButton;
  QLabel* titleLabel;
  /// @}

  /// @name Topology selection
  /// @{
  QLabel* Topology_Label;
  QComboBox* Topology_Combo;
  /// @}

  /// @name Solution selection
  /// @{
  QWidget* SolutionWidget;
  QRadioButton* Solution1_RB;
  QRadioButton* Solution2_RB;
  /// @}

  /// @name Stub termination
  /// @{
  QLabel* StubTermination_Label;
  QComboBox* StubTermination_ComboBox;
  /// @}

  /// @name Weighting settings
  /// @{
  QGroupBox* Weighting_GroupBox;
  QComboBox* Weighting_Combo;
  QLabel* Ripple_Label;
  QDoubleSpinBox* Ripple_SpinBox;
  /// @}

  /// @name Number of sections
  /// @{
  QLabel* Sections_Label;
  QSpinBox* Sections_SpinBox;
  /// @}

  /// @name Reference impedance
  /// @{
  QLabel* Zin_Label;
  QDoubleSpinBox* ZinRSpinBox;
  QLabel* Ohm_Zin_Label;
  /// @}

  QGroupBox* groupBox;

  /// @name Transmission line implementation
  /// @{
  QLabel* TL_Implementation_Label;
  QComboBox* TL_Implementation_Combo;
  /// @}

  QGridLayout* mainLayout; ///< Main layout

  bool m_isCollapsed; ///< Collapsed state flag

signals:
  /// @brief Emitted when parameters change
  void parametersChanged();

  /// @brief Emitted when topology changes
  /// @param index New topology index
  void topologyChanged(int index);

  /// @brief Emitted when collapsed state changes
  /// @param collapsed New collapsed state
  void collapsedStateChanged(bool collapsed);
};

#endif // MATCHINGNETWORKPARAMETERSWIDGET_H
