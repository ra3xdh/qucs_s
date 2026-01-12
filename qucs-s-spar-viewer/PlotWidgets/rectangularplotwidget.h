/// @file rectangularplotwidget.h
/// @brief Widget for displaying rectangular (Cartesian) plots with dual y-axes (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef RECTANGULARPLOTWIDGET_H
#define RECTANGULARPLOTWIDGET_H

#include "./QCustomPlot/qcustomplot.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QPen>
#include <QVBoxLayout>
#include <QWidget>
#include <complex>
#include <limits>

/// @class RectangularPlotWidget
/// @brief Widget for displaying rectangular (Cartesian) plots with dual y-axes
/// It provides a rectangular plot visualization
/// with support for multiple traces, frequency-based markers, limit lines, and
/// independent left/right y-axes.
/// @note Features include axis locking, auto-scaling, and configurable frequency units.
class RectangularPlotWidget : public QWidget {
  Q_OBJECT

public:
  /// @struct Trace
  /// @brief Data structure representing a trace on the plot
  /// @note It contains the y-axis data and the x-axis data (frequency) in separate QList<double>
  /// @note The struct also contains information about the y-axis units, the y-axis to use (left or right)
  /// as well as the trace identifier
  struct Trace {
    QList<double> trace;          ///< Y-axis values
    QList<double> frequencies;    ///< X-axis frequency values in Hz
    QString units;                ///< Units for the trace values (e.g., "dB")
    QPen pen;                     ///< Visual style for the trace line
    double Z0;                    ///< Reference impedance
    int y_axis;                   ///< Axis assignment: 0 for left, 2 for right
    QString y_axis_title;         ///< Title for the y-axis
  };


  /// @struct Marker
  /// @brief Data structure for the frequency marker
  /// @note Defines a marker at a specific frequency with its appearance.
  struct Marker {
    QString id;        ///< Unique marker identifier
    double frequency;  ///< Marker frequency [Hz]
    QPen pen;          ///< Marker pen style
  };

  /// @struct Limit
  /// @brief Data structure representing a limit line segment
  struct Limit {
    double f1;   ///< Start frequency [Hz]
    double f2;   ///< End frequency [Hz]
    double y1;   ///< Y-value at start frequency
    double y2;   ///< Y-value at end frequency
    int y_axis;  ///< Axis assignment: 0 for left, 1 for right
    QPen pen;    ///< Limit line style
  };

  /// @struct AxisSettings
  /// @brief Settings for plot axes and display options
  struct AxisSettings {
    double xAxisMin;      ///< Minimum X-axis value
    double xAxisMax;      ///< Maximum X-axis value
    double xAxisDiv;      ///< X-axis tick interval
    QString xAxisUnits;   ///< Frequency units (Hz, kHz, MHz, GHz)
    double yAxisMin;      ///< Minimum left Y-axis value
    double yAxisMax;      ///< Maximum left Y-axis value
    double yAxisDiv;      ///< Left Y-axis tick interval
    double y2AxisMin;     ///< Minimum right Y-axis value
    double y2AxisMax;     ///< Maximum right Y-axis value
    double y2AxisDiv;     ///< Right Y-axis tick interval
    bool showValues;      ///< Show marker intersection values
    bool lockAxis;        ///< Lock axis settings to prevent auto-adjustment
  };

  /// @brief Class constructor
  /// @param parent Parent widget
  explicit RectangularPlotWidget(QWidget* parent = nullptr);

  /// @brief Destructor
  ~RectangularPlotWidget();

  /// @brief Add or update a trace on the plot
  /// @param name Unique trace identifier
  /// @param trace Trace data structure
  void addTrace(const QString& name, const Trace& trace);

  /// @brief Remove a trace from the plot
  /// @param name Trace identifier
  void removeTrace(const QString& name) {
    traces.remove(name);
    updatePlot();
  }

  /// @brief Remove all traces from the plot
  void clearTraces() {
    traces.clear();
    updatePlot();
  }

  /// @brief Calculate an aesthetically pleasing step size for axis divisions
  /// @param range The range to divide
  /// @return Nice step value (1, 2, 5, 10, etc. × power of 10)
  double calculateNiceStep(double range);

  /// @brief Get the pen style for a specific trace
  /// @param traceName Name of the trace
  /// @return QPen for the trace, or default pen if not found
  QPen getTracePen(const QString& traceName) const;

  /// @brief Set the pen style for a specific trace
  /// @param traceName Name of the trace
  /// @param pen New pen style
  void setTracePen(const QString& traceName, const QPen& pen);

  /// @brief Get information about all traces
  /// @return Map of trace names to their pen styles
  QMap<QString, QPen> getTracesInfo() const;

  /// @brief Get maximum left Y-axis value
  double getYmax() { return yAxisMax->value(); }

  ///  @brief Get minimum left Y-axis value
  double getYmin() { return yAxisMin->value(); }

  /// @brief Get left Y-axis division interval
  double getYdiv() { return yAxisDiv->value(); }

  /// @brief Set left Y-axis division interval
  /// @param val New division value
  void setYdiv(double val) { yAxisDiv->setValue(val); }

  /// @brief Set maximum left Y-axis value
  /// @param val New maximum value
  void setYmax(double val) { yAxisMax->setValue(val); }

  /// @brief Set minimum left Y-axis value
  /// @param val New minimum value
  void setYmin(double val) { yAxisMin->setValue(val); }

  /// @brief Get maximum right Y-axis value
  double getY2max() { return y2AxisMax->value(); }

  /// @brief Get minimum right Y-axis value
  double getY2min() { return y2AxisMin->value(); }

  /// @brief Get right Y-axis division interval
  double getY2div() { return y2AxisDiv->value(); }

  /// @brief Get maximum X-axis value
  double getXmax() { return xAxisMax->value(); }

  /// @brief Get minimum X-axis value
  double getXmin(){ return xAxisMin->value(); }

  /// @brief Get X-axis division interval
  double getXdiv() { return xAxisDiv->value(); }

  /// @brief Get frequency scale factor for current units
  /// @return Scale factor (1.0 for Hz, 1e-3 for kHz, 1e-6 for MHz, 1e-9 for GHz)
  double getXscale();

  /// @brief Get current frequency unit string
  /// @return String containing current units (Hz, kHz, MHz, or GHz)
  QString getXunits() { return xAxisUnits->currentText(); }

  /// @brief Get frequency unit combo box index
  /// @return Current frequency unit index
  int getFreqIndex()  { return xAxisUnits->currentIndex(); }

  /// @brief Redraw the entire plot with current data
  void updatePlot();

  /// @brief Enable or disable automatic Y-axis scaling
  /// @param value true to enable auto-scaling, false to disable
  void set_y_autoscale(bool value) { y_autoscale = value; }

  /// @brief Check if axis settings are locked
  /// @return true if axes are locked and won't auto-adjust
  bool areAxisSettingsLocked() const {
    return axisSettingsLocked;
  }

  /// @brief Enable or disable the right Y-axis
  /// @param enabled true to show right Y-axis
  void setRightYAxisEnabled(bool enabled);

  /// @brief Check if right Y-axis is enabled
  /// @return true if right Y-axis is visible
  bool isRightYAxisEnabled() const {
    return plotWidget->yAxis2->visible();
  }

  /// @brief Set left Y-axis title
  /// @param title New title text
  void change_Y_axis_title(QString title) {
    plotWidget->yAxis->setLabel(title);
    plotWidget->replot();
  }

  /// @brief Set left Y-axis unit label
  /// @param units New unit text
  void change_Y_axis_units(QString units){
    yAxisUnits->setText(units);
  }

  /// @brief Set right Y-axis title
  /// @param title New title text
  void change_Y2_axis_title(QString title){
    plotWidget->yAxis2->setLabel(title);
    plotWidget->replot();
  }

  /// @brief Set right Y-axis unit label
  /// @param units New unit text
  void change_Y2_axis_units(QString units) {
    y2AxisUnits->setText(units);
  }

  /// @brief Set X-axis title
  /// @param title New title text
  void change_X_axis_title(QString title){
    plotWidget->xAxis->setLabel(title);
    plotWidget->replot();
  }


  QLabel* xAxisLabel; ///< x-axis label

  /// @brief Add a vertical marker at a specific frequency
  /// @param markerId Unique marker identifier
  /// @param frequency Frequency in Hz
  /// @param pen Marker line style (default: red, width 2)
  /// @return true if added successfully, false if marker exists or frequency out of range
  bool addMarker(const QString& markerId, double frequency,
                 const QPen& pen = QPen(Qt::red, 2));

  /// @brief Remove a marker from the plot
  /// @param markerId Marker identifier
  /// @return true if removed successfully, false if not found
  bool removeMarker(const QString& markerId);

  /// @brief Update the frequency of an existing marker
  /// @param markerId Marker identifier
  /// @param newFrequency New frequency in Hz
  /// @return true if updated successfully, false if marker not found or frequency invali
  bool updateMarkerFrequency(const QString& markerId, double newFrequency);

  /// @brief Remove all markers from the plot
  void clearMarkers() {
    markers.clear();
    updatePlot();
  }

  /// @brief Get all markers and their frequencies
  /// @return Map of marker IDs to frequencies in Hz
  QMap<QString, double> getMarkers() const;

  /// @brief Add a limit line to the plot
  /// @param LimitId Limit identifier
  /// @param limit Limit line properties
  /// @return true if added successfully, false if limit already exists
  bool addLimit(const QString& LimitId, const Limit& limit);

  /// @brief Remove a limit line from the plot
  /// @param LimitID Limit identifier
  void removeLimit(const QString& LimitID);

  /// @brief Remove all limit lines from the plot
  void clearLimits() {
    limits.clear();
    updatePlot();
  }

  /// @brief Get all defined limits
  /// @return Map of limit IDs to limit properties
  QMap<QString, Limit> getLimits() const {
    return limits;
  }

  /// @brief Update an existing limit line
  /// @param limitId Limit identifier
  /// @param limit New limit properties
  /// @return true if updated successfully, false if not found
  bool updateLimit(const QString& limitId, const Limit& limit);

  /// @brief Access the underlying QCustomPlot widget
  /// @return Pointer to the QCustomPlot instance
  QCustomPlot* customPlot() const { return plotWidget; }

  /// @brief Get current axis settings
  /// @return AxisSettings structure with all current configurations
  AxisSettings getSettings() const;

  /// @brief Apply axis settings to the plot
  /// @param settings Settings structure to apply
  void setSettings(const AxisSettings& settings);

private slots:
  /// @brief Update x-axis based on control widget values
  void updateXAxis();

  /// @brief Update left y-axis based on control widget values
  void updateYAxis();

  /// @brief Update right y-axis based on control widget values
  void updateY2Axis();

  /// @brief Handle frequency unit changes and rescale axis
  void changeFreqUnits();

  /// @brief Toggle display of trace values at the intersections with the vertical
  /// marker
  /// @param show true to show values, false to hide
  void toggleShowValues(bool show);

  /// @brief Lock / unlock the automatic axis settings
  /// \param locked. If true, freeze the axis autoadjust. If false, let the widget to set the axis limits automatically
  void toggleLockAxisSettings(bool locked);

private slots:
  /// @brief Handle x-axis range changes from user interaction (panning and zooming)
  void onXAxisRangeChanged(const QCPRange& range);

  /// @brief Handle left y-axis range changes from user interaction (panning and zooming)
  void onYAxisRangeChanged(const QCPRange& range);

  /// @brief Handle right y-axis range changes from user interaction (panning and zooming)
  void onY2AxisRangeChanged(const QCPRange& range);

private:
  QCustomPlot* plotWidget;           ///< Main plot widget

  QDoubleSpinBox* xAxisMin;          ///< X-axis minimum control
  QDoubleSpinBox* xAxisMax;          ///< X-axis maximum control
  QDoubleSpinBox* xAxisDiv;          ///< X-axis division control
  QComboBox* xAxisUnits;             ///< Frequency unit selector

  QDoubleSpinBox* yAxisMin;          ///< Left Y-axis minimum control
  QDoubleSpinBox* yAxisMax;          ///< Left Y-axis maximum control
  QDoubleSpinBox* yAxisDiv;          ///< Left Y-axis division control
  QLabel* yAxisUnits;                ///< Left Y-axis unit label

  QDoubleSpinBox* y2AxisMin;         ///< Right Y-axis minimum control
  QDoubleSpinBox* y2AxisMax;         ///< Right Y-axis maximum control
  QDoubleSpinBox* y2AxisDiv;         ///< Right Y-axis division control
  QLabel* y2AxisUnits;               ///< Right Y-axis unit label
  QLabel* y2AxisLabel;               ///< Right Y-axis title label

  QCheckBox* showValuesCheckbox;     ///< Checkbox for value display toggle
  bool showTraceValues;              ///< Flag for showing marker values

  QCheckBox* lockAxisCheckbox;       ///< Checkbox for axis lock toggle
  bool axisSettingsLocked;           ///< Flag for axis lock state

  QStringList frequencyUnits;        ///< Available frequency units
  double fMin;                       ///< Global minimum frequency in Hz
  double fMax;                       ///< Global maximum frequency in Hz

  bool y_autoscale;                  ///< Enable Y-axis auto-scaling

  QMap<QString, Trace> traces;       ///< All trace data
  QMap<QString, Marker> markers;     ///< All marker data
  QMap<QString, Limit> limits;       ///< All limit line data

  QMap<QString, QCPGraph*> traceGraphs;              ///< Trace graph objects
  QMap<QString, QCPItemStraightLine*> markerLines;   ///< Marker line items
  QMap<QString, QCPItemText*> markerLabels;          ///< Marker frequency labels
  QMap<QString, QCPItemTracer*> intersectionPoints;  ///< Marker-trace intersection points
  QMap<QString, QCPItemText*> intersectionLabels;    ///< Intersection value labels
  QMap<QString, QCPGraph*> limitGraphs;              ///< Limit line graphs

  /// @brief Create and configure axis control widgets
  /// @return Grid layout containing all controls
  QGridLayout* setupAxisSettings();

  /// @brief Remove all graphics items from the plot
  void clearGraphicsItems();

  /// @brief Configure initial plot properties and axes
  void setupPlot();

  /// @brief Add intersection markers where a marker line crosses traces
  /// @param markerId Marker identifier
  /// @param marker Marker data
  void addMarkerIntersections(const QString& markerId, const Marker& marker);

  /// @brief Gets the number of traces assigned to left Y-axis
  /// @return Number of traces using left Y-axis
  int getYAxisTraceCount() const;

  /// @brief Gets the number of traces assigned to left Y-axis
  /// @return Number of traces using right Y-axis
  int getY2AxisTraceCount() const;
};

#endif // RECTANGULARPLOTWIDGET_H
