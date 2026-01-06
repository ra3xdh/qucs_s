/// @file LoadSpecificationWidget.h
/// @brief Widget for entering the load impedance data (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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

/// @class LoadSpecificationWidget
/// @brief Widget for entering the load impedance data
class LoadSpecificationWidget : public QGroupBox {
  Q_OBJECT

public:
  /// @brief Class constructor
  /// @param parent Parent widget
  explicit LoadSpecificationWidget(QWidget* parent = nullptr);

  /// @brief Class destructor
  ~LoadSpecificationWidget() {}

  /// @name Getters
  /// @{

  /// @brief Get all S-parameters as an array [S11, S12, S21, S22]
  /// @return Array containing S11, S12, S21, S22
  std::array<std::complex<double>, 4> getSParameters() const {
    return {getS11(), getS12(), getS21(), getS22()};
  }

  /// @brief Get load impedance at matching frequency
  /// @return Complex load impedance
  std::complex<double> getLoadImpedance_At_Fmatch() const;

  /// @brief Get load impedance data over frequency
  /// @return List of complex impedances
  QList<std::complex<double>> getZLdata();

  /// @brief Get frequency data points
  /// @return List of frequencies in Hz
  QList<double> getFrequency();

  /// @brief Calculate two-port matching impedances (source and load)
  /// @return Pair of complex impedances (source, load)
  std::pair<std::complex<double>, std::complex<double>>
  getTwoPortMatchingImpedances() const;

  /// @brief Get reflection coefficient
  /// @return Complex reflection coefficient
  std::complex<double> getReflectionCoefficient() const;

  /// @brief Get S-parameter file path
  /// @return Path to S-parameter file
  QString getSparFilePath() { return spar_file_path; }

  /// @brief Check if widget is in two-port mode
  /// @return True if in two-port mode, false otherwise
  bool isTwoPortMode() const { return m_twoPortMode; }
  /// @}

  /// @name S-parameter getters (for two-port mode)
  /// @{
  ///
  /// @brief Get S11 parameter
  /// @return Complex S11 value
  std::complex<double> getS11() const;

  /// @brief Get S12 parameter
  /// @return Complex S12 value
  std::complex<double> getS12() const;

  /// @brief Get S21 parameter
  /// @return Complex S21 value
  std::complex<double> getS21() const;

  /// @brief Get S22 parameter
  /// @return Complex S22 value
  std::complex<double> getS22() const;
  /// @}

  /// @name Setters
  /// @{

  /// @brief Set load impedance
  /// @param impedance Complex impedance value
  void setLoadImpedance(const std::complex<double>& impedance);

  /// @brief Set reflection coefficient
  /// @param gamma Complex reflection coefficient
  void setReflectionCoefficient(const std::complex<double>& gamma);

  /// @brief Enable or disable two-port mode
  /// @param enabled True to enable two-port mode, false for one-port
  void setTwoPortMode(bool enabled);

  /// @brief Set reference impedance
  /// @param Z0 Reference impedance in ohms
  void setReferenceImpedance(double Z0) {
    m_Z0 = Z0;
    updateReflectionCoefficient();
  }

  /// @brief Set collapsed state of widget
  /// @param collapsed True to collapse, false to expand
  void setCollapsed(bool collapsed);

  /// @brief Set matching frequency
  /// @param freq Frequency in Hz
  void setFmatch(double freq) { f_match = freq; }

  /// @brief Check if widget is collapsed
  /// @return True if collapsed, false otherwise
  bool isCollapsed() const { return m_isCollapsed; }

  // Reference impedance of the source and the load port. Required for the
  // 2-port matching. This data is provided by the main widget
  double Z0_Port1, Z0_Port2;

protected:
  /// @brief Handle mouse press events for collapsible header
  /// @param event Mouse event
  void mousePressEvent(QMouseEvent* event) override;

private slots:
  /// @brief Handle impedance value changes
  void onImpedanceChanged();

  /// @brief Handle reflection coefficient value changes
  void onReflectionCoefficientChanged();

  /// @brief Handle format selection changes (Real/Imag vs Mag/Angle)
  void onFormatChanged();

  /// @brief Handle input method changes (manual vs file)
  void onInputMethodChanged();

  /// @brief Handle file browse button click
  void onBrowseFile();

  /// @brief Handle S-parameter value changes
  void onSParameterChanged();

  /// @brief Toggle collapse/expand state
  void onToggleCollapse() {
    setCollapsed(!m_isCollapsed);
  }


private:
  /// @brief Setup main UI layout
  void setupUI();

  /// @brief Setup one-port UI elements
  void setupOnePortUI();

  /// @brief Setup two-port UI elements
  void setupTwoPortUI();

  /// @brief Update reflection coefficient from impedance
  void updateReflectionCoefficient();

  /// @brief Update impedance from reflection coefficient
  void updateImpedance();

  /// @brief Update S-parameter display values
  void updateSParameterDisplays();

  /// @brief Update impedance format display
  void updateImpedanceFormat();

  /// @brief Update reflection coefficient format display
  void updateReflectionFormat();

  /// @brief Update S-parameter format display
  void updateSParameterFormat();

  double f_match; ///< Matching frequency

  QMap<QString, QList<double>> loadData; ///< 1-port or 2-port data
  QString spar_file_path; ///< Path to S-parameter file (required for simulation)

  /// @name UI Components - Common
  /// @{
  QGridLayout* m_mainLayout;
  QWidget* m_contentWidget;
  QPushButton* m_toggleButton;
  /// @}

  /// @name Input method selection
  /// @{
  QRadioButton* m_manualInputRadio;
  QRadioButton* m_fileInputRadio;
  QButtonGroup* m_inputMethodGroup;
  QPushButton* m_browseButton;
  QLabel* m_fileLabel;
  /// @}

  /// @name Format selection
  /// @{
  QLabel* m_formatLabel;
  QComboBox* m_formatCombo;
  /// @}

  /// @name Manual input widgets - Impedance
  /// @{
  QLabel* m_impedanceLabel;
  QDoubleSpinBox* m_impedanceReal;
  QLabel* m_impedanceSeparator;
  QDoubleSpinBox* m_impedanceImag;
  QLabel* m_impedanceUnit;
  /// @}

  /// @name Manual input widgets - Reflection coefficient
  /// @{
  QLabel* m_reflectionLabel;
  QDoubleSpinBox* m_reflectionReal;
  QLabel* m_reflectionSeparator;
  QDoubleSpinBox* m_reflectionImag;
  /// @}

  /// @name Two-port widgets
  /// @{
  QWidget* m_twoPortWidget;
  QGridLayout* m_twoPortLayout;
  /// @}

  /// @name S-parameter widgets
  /// @{
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
  /// @}

  /// @name Internal state
  /// @{
  bool m_twoPortMode;      ///< Two-port mode flag
  bool m_updatingValues;   ///< Updating values flag (prevents signal loops)
  bool m_isCollapsed;      ///< Collapsed state flag
  double m_Z0;             ///< Reference impedance
  QString m_currentFile;   ///< Current file path
  /// @}

signals:
  /// @brief Emitted when impedance value changes
  void impedanceChanged();

  /// @brief Emitted when reflection coefficient changes
  void reflectionCoefficientChanged();

  /// @brief Emitted when S-parameters change
  void sParametersChanged();

  /// @brief Emitted when collapsed state changes
  /// @param collapsed New collapsed state
  void collapsedStateChanged(bool collapsed);
};

#endif // LOADSPECIFICATIONWIDGET_H
