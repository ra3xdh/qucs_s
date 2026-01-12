/// @file polarplotwidget.h
/// @brief Construct polar plot widget with default configuration (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef POLARPLOTWIDGET_H
#define POLARPLOTWIDGET_H

#include "./QCustomPlot/qcustomplot.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QPen>
#include <QWidget>
#include <complex>
#include <limits>


/// @class PolarPlotWidget
/// @brief Widget for displaying complex data in polar plot format with markers and traces
///
/// PolarPlotWidget provides a polar plot visualization
/// with support for multiple traces and frequency markers
class PolarPlotWidget : public QWidget {
  Q_OBJECT

public:
  /// @brief Complex trace data structure with frequencies and display properties
  ///
  /// Stores complex-valued data points with corresponding frequencies for
  /// polar plot visualization.
  struct Trace {
    QList<std::complex<double>> values;  ///< Complex values for polar representation
    QList<double> frequencies;           ///< Corresponding frequencies for each point
    QPen pen;                            ///< Line style for this trace
    int displayMode;                     ///< 0: Magnitude/Phase, 1: Real/Imaginary
  };

  /// @brief Data structure for the frequency marker
  ///
  /// Defines a marker at a specific frequency with its appearance.
  struct Marker {
    QString id;          ///< Marker identifier
    double frequency;    ///< Frequency position in Hz
    QPen pen;            ///< Visual style for marker
  };

  /// @brief Settings for plot axes and display options
  struct AxisSettings {
    double freqMax;         ///< Maximum frequency value
    double freqMin;         ///< Minimum frequency value
    QString freqUnit;       ///< Frequency unit (Hz, kHz, MHz, GHz)
    double radius_min;      ///< Radial axis minimum
    double radius_max;      ///< Radial axis maximum
    double radius_div;      ///< Radial axis division spacing
    QString marker_format;  ///< Marker display format string
  };

  /// @brief Apply saved axis settings
  void setSettings(const AxisSettings& settings);

  /// @brief Get current axis settings
  /// @return Current axis configuration
  PolarPlotWidget::AxisSettings getSettings() const;

  /// @brief Construct a new polar plot widget
  /// @param parent Parent widget
  explicit PolarPlotWidget(QWidget* parent = nullptr);

  /// @brief Class destructor
  ~PolarPlotWidget(){
    clearGraphicsItems(); // Clean up any remaining graphics items
    }

  // Trace management functions

  ///
  /// @brief Add trace to the widget
  /// @param name Unique trace identifier
  /// @param trace Trace data to add
  ///
  void addTrace(const QString& name, const Trace& trace);

  /// @brief Remove trace by name
  /// @param name Trace identifier to remove
  void removeTrace(const QString& name);

  /// @brief Clear all traces from the plot
  void clearTraces();

  /// @brief Get pen style for a specific trace
  /// @param traceName Name of the trace
  /// @return Pen style, or default QPen if trace not found
  QPen getTracePen(const QString& traceName) const;

  /// @brief Set pen style for a specific trace
  /// @param traceName Name of the trace
  /// @param pen New pen style to apply
  void setTracePen(const QString& traceName, const QPen& pen);

  /// @brief Get all trace names and their pen styles
  /// @return Map of trace names to pen styles
  QMap<QString, QPen> getTracesInfo() const;

  // Axis value access functions

  /// @brief Get current radial axis maximum value
  /// @return Maximum radius value
  double getRmax() { return rAxisMax->value(); }

  /// @brief Get current radial axis minimum value
  /// @return Minimum radius value
  double getRmin() { return rAxisMin->value(); }

  /// @brief Get radial axis division spacing
  /// @return Division interval for radial grid
  double getRdiv() { return rAxisDiv->value(); }

  /// @brief Get current display mode
  /// @return 0 for Magnitude/Phase, 1 for Real/Imaginary
  int getDisplayMode() const {
    return displayModeCombo->currentIndex();
  }


  // Marker management functions

  /// @brief Add marker at specified frequency
  /// @param markerId Unique marker identifier
  /// @param frequency Frequency position in Hz
  /// @param pen Visual style for marker (default: red, width 2)
  /// @return true if marker added successfully, false if frequency out of range
  bool addMarker(const QString& markerId, double frequency,
                 const QPen& pen = QPen(Qt::red, 2));

  /// @brief Remove marker by ID
  /// @param markerId Identifier of marker to remove
  /// @return true if marker existed and was removed, false otherwise
  bool removeMarker(const QString& markerId);

  /// @brief Update existing marker to new frequency
  /// @param markerId Identifier of marker to update
  /// @param newFrequency New frequency position in Hz
  /// @return true if update successful, false if marker not found or frequency invalid
  bool updateMarkerFrequency(const QString& markerId, double newFrequency);

  /// @brief Clear all markers from the plot
  void clearMarkers() {
    markers.clear();
    updatePlot();
  }

  /// @brief Get all marker IDs and their frequencies
  /// @return Map of marker IDs to frequency values
  QMap<QString, double> getMarkers() const;

  /// @brief Access underlying QCustomPlot instance
  /// @return Pointer to internal QCustomPlot widget
  QCustomPlot* customPlot() const { return plot; }

private slots:
  /// @brief Update radial axis range from spinbox changes
  void updateRAxis();

  /// @brief Update angular axis display (currently placeholder)
  void updateAngleAxis() {
    updatePlot();
  }

  /// @brief Toggle between magnitude/phase and real/imaginary display modes
  void toggleDisplayMode() { updatePlot(); }

  /// @brief Handle frequency minimum spinbox value change
  /// @param value New minimum frequency in current units
  void onFMinChanged(double value){
    fMin = value * getFrequencyMultiplier();
    updatePlot();
  }

  /// @brief Handle frequency maximum spinbox value change
  /// @param value New maximum frequency in current units
  void onFMaxChanged(double value) {
    fMax = value * getFrequencyMultiplier();
    updatePlot();
  }

  /// @brief Handle frequency unit combobox change
  /// @note Recalculates frequency range using new unit multiplier.
  void onFUnitChanged() {
    fMin = fMinSpinBox->value() * getFrequencyMultiplier();
    fMax = fMaxSpinBox->value() * getFrequencyMultiplier();
    updatePlot();
  }

private slots: // Slot to update axis settings widget based on the user zoomming and panning

  /// @brief Enforce non-negative radial axis after user zoom/pan
  /// @note Updates spinbox values to reflect current axis range.
  void checkAxisRanges();

  /// @brief Overload for mouse event handling
  /// @param event Mouse event (unused)
  /// @see checkAxisRanges()
  void checkAxisRanges(QMouseEvent* event) {
    // This overload handles mouse events - just call the main function
    Q_UNUSED(event)
    checkAxisRanges();
  }

  /// @brief Prevent negative radial values during programmatic range changes
  /// @param newRange Attempted new range for radial axis
  /// @note Shifts range upward if lower bound is negative.
  void onRadialRangeChanged(const QCPRange& newRange);

private:
  QCustomPlot* plot;
  QCPPolarAxisAngular* angularAxis;
  QCPPolarAxisRadial* radialAxis;

  QDoubleSpinBox* rAxisMin;
  QDoubleSpinBox* rAxisMax;
  QDoubleSpinBox* rAxisDiv;
  QComboBox* displayModeCombo;

  QDoubleSpinBox* fMinSpinBox;
  QDoubleSpinBox* fMaxSpinBox;
  QComboBox* fUnitComboBox;
  double fMin;
  double fMax;
  QStringList frequencyUnits;

  QMap<QString, Trace> traces;
  QMap<QString, Marker> markers;
  QMap<QString, QList<QCPPolarGraph*>>
      traceGraphs; // Each trace can have multiple graphs for phase wrapping

  // Marker items for drawing
  QList<QCPItemEllipse*> markerItems;
  QList<QCPItemText*> markerLabels;

  /// @brief Update global frequency range from all traces
  /// @note Scans all traces to find minimum and maximum frequencies,
  /// then updates spinbox ranges accordingly.
  void updateFrequencyRange();

  /// @brief Create axis control widgets layout
  /// @return Grid layout containing frequency, radius, and display mode controls
  QGridLayout* setupAxisSettings();

  /// @brief Redraw all traces and markers
  /// @note Clears existing graphics, recreates polar graphs with phase wrap handling,
  /// and redraws all markers at interpolated positions.
  void updatePlot();

  /// @brief Clear marker graphics items from plot
  /// @note Removes all QCPItemEllipse and QCPItemText objects for markers.
  void clearGraphicsItems();

  /// @brief Draw markers at interpolated trace positions
  /// @note For each marker and trace combination, interpolates the complex value
  /// at the marker frequency and creates visual marker with label.
  void drawCustomMarkers();

  /// @brief Get frequency multiplier from current unit selection
  /// @return Multiplier (1.0 for Hz, 1e3 for kHz, 1e6 for MHz, 1e9 for GHz)
  double getFrequencyMultiplier() const;

  /// @brief Interpolate complex value at specific frequency
  /// @param trace Trace data to interpolate
  /// @param frequency Target frequency in Hz
  /// @return Interpolated complex value, or (0,0) if frequency not in range
  /// @note Uses linear interpolation on both real and imaginary components.
  std::complex<double> getComplexValueAtFrequency(const Trace& trace,
                                                  double frequency);
};

#endif // POLARPLOTWIDGET_H
