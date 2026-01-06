/// @file smithchartwidget.h
/// @brief Smith chart visualization and interaction widget (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef SMITHCHARTWIDGET_H
#define SMITHCHARTWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMap>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QSet>
#include <QVBoxLayout>
#include <QWidget>
#include <complex>

/// @brief Forward declaration.
class Qucs_S_SPAR_Viewer;


/// @class SmithChartWidget
/// @brief Smith chart visualization and interaction widget.
/// Provides impedance/admittance Smith chart drawing, trace management and
/// marker handling, with basic zoom, pan and frequency-range controls.
class SmithChartWidget : public QWidget {
  Q_OBJECT

public:
  /// @struct Trace
  /// @brief Data structure representing a trace on the plot
  struct Trace {
    QList<std::complex<double>> impedances; ///< Impedance samples for the trace [Ohm]
    QList<double> frequencies;              ///< Frequencies for each impedance sample [Hz]
    QPen pen;                               ///< Pen used to draw the trace.
    double Z0;                              ///< Characteristic impedance of the trace data [Ohm]
  };


  /// @struct Marker
  /// @brief Data structure for the frequency marker
  /// Defines a marker at a specific frequency with its appearance.
  struct Marker {
    QString id;        ///< Unique marker identifier
    double frequency;  ///< Marker frequency [Hz]
    QPen pen;          ///< Marker pen style
  };

  /// @struct AxisSettings
  /// @brief Settings for plot axes and display options
  struct AxisSettings {
    QString Z0;        ///< Diagram reference impedance [Ohm]
    double freqMax;    ///< Maximum frequency to shown on the diagram [Hz]
    double freqMin;    ///< Minimum frequency to shown on the diagram [Hz]
    QString freqUnit;  ///< Frequency units

    bool y_chart;      ///< Show admittance chart
    bool z_chart;      ///< Show impedance chart
  };

  /// @brief Returns the current axis settings
  /// @return AxisSettings of the diagram
  SmithChartWidget::AxisSettings getSettings() const;

  /// @brief Set the current axis settings
  /// @param settings New AxisSettings
  void setSettings(const AxisSettings& settings);

  /// @brief Class constructor
  /// \param parent Parent widget
  SmithChartWidget(QWidget* parent = nullptr);

  /// @brief Class destructor
  ~SmithChartWidget() override {}

  ///
  /// @brief Add or update a trace on the plot
  /// @param name Unique trace identifier
  /// @param trace Trace data structure
  ///
  void addTrace(const QString& name, const Trace& trace);

  /// @brief Remove a trace from the plot
  /// @param name Trace identifier
  void removeTrace(const QString&);

  /// @brief Remove all traces from the plot
  void clearTraces() {
    traces.clear(); // Remove all traces
    update();       // Trigger a repaint to reflect the changes
  }

  /// @brief Set the characteristic impedance of the diagram
  /// \param z0 Characteristic impedance (e.g. 50 Ohm, 75 Ohm)
  void setCharacteristicImpedance(double z) {
    z0 = z;
    update(); // Redraw the chart with the new Z0
  }

  /// @brief Get the characteristic impedance of the diagram
  /// \return z0 Characteristic impedance of the diagram
  double characteristicImpedance() const { return z0; }

  /// @brief Get the diagram QPen style of a trace (by name)
  /// \param traceName Name of the trace
  /// \return Trace QPen style object
  QPen getTracePen(const QString& traceName) const;

  /// @brief Sets the pen for an existing trace (given its name)
  /// @param traceName Name of the trace.
  /// @param pen New pen to use.
  void setTracePen(const QString& traceName, const QPen& pen);

  /// @brief Returns a map with trace names and their QPens.
  /// @return QMap object relating the name of the trace with the QPen style object
  QMap<QString, QPen> getTracesInfo() const;

  /// @brief Adds a marker at a given frequency.
  /// @param markerId Unique marker identifier.
  /// @param frequency Marker frequency (Hz).
  /// @param pen Pen used to draw marker and label.
  /// @note By default, the trace styling is red and its width = 2
  /// @return @c true on success, @c false if ID exists or no trace covers the frequency.
  bool addMarker(const QString& markerId, double frequency,
                 const QPen& pen = QPen(Qt::red, 2));

  /// @brief Remove a trace from the plot, given its name
  /// @param name Trace identifier
  bool removeMarker(const QString& markerId);

  /// @brief Update the frequency of an existing marker
  /// @param markerId Marker identifier
  /// @param newFrequency New frequency in Hz
  /// @return true if updated successfully, false if marker not found or frequency invali
  bool updateMarkerFrequency(const QString& markerId, double newFrequency);

  /// @brief Remove all markers from the plot
  void clearMarkers(){
    markers.clear();
    update();
  }

  /// @brief Get all markers and their frequencies
  /// @return Map of marker IDs to frequencies in Hz
  QMap<QString, double> getMarkers() const;

signals:
  /// @brief Emitted when a point on the chart is clicked.
  /// @param impedance Impedance corresponding to the clicked position (Ohms).
  void impedanceSelected(const std::complex<double>& impedance);

protected:
  /// @brief Reimplements paintEvent to draw the Smith chart and overlays.
  void paintEvent(QPaintEvent* event) override;

  /// @brief Handles mouse clicks to pick impedances and emit impedanceSelected().
  void mousePressEvent(QMouseEvent* event) override;

private:
  /// @brief Draws the Smith chart grid (circles, arcs and labels).
  /// @param painter Target painter.
  void drawSmithChartGrid(QPainter* painter);

  /// @brief Draws a constant reactance arc and its labels.
  /// @param painter Target painter.
  /// @param center Chart center.
  /// @param radius Chart radius.
  /// @param reactance Normalized reactance value.
  void drawReactanceArc(QPainter* painter, const QPointF& center, double radius,
                        double reactance);

  /// @brief Draws a constant susceptance arc and its labels.
  /// @param painter Target painter.
  /// @param center Chart center.
  /// @param radius Chart radius.
  /// @param reactance Normalized reactance value.
  void drawSusceptanceArc(QPainter* painter, const QPointF& center,
                          double radius, double susceptance);

  /// @brief Plots all traces within the selected frequency range.
  /// @param painter Target painter.
  void plotImpedanceData(QPainter* painter);

  /// @brief Draws all enabled markers for all traces.
  /// @param painter Target painter.
  void drawMarkers(QPainter* painter);

  /// @brief Converts a reflection coefficient to widget coordinates.
  /// @param reflectionCoefficient Reflection coefficient value
  /// @return Position of the refletion coefficient on the Smith Chart
  QPointF smithChartToWidget(const std::complex<double>& reflectionCoefficient);

  /// @brief Converts a widget coordinate to a reflection coefficient.
  /// @param widgetPoint Point on the Smith Chart
  /// @return Value of the reflection coefficient
  std::complex<double> widgetToSmithChart(const QPointF& widgetPoint);

  /// @brief Linearly interpolates impedance at a given frequency.
  /// @param frequencies Sorted frequency list [Hz]
  /// @param impedances Impedance samples at those frequencies [Ohm]
  /// @param targetFreq Target frequency [Hz]
  /// @return Interpolated impedance value.
  std::complex<double>
  interpolateImpedance(const QList<double>& frequencies,
                       const QList<std::complex<double>>& impedances,
                       double targetFreq);

  /// @brief Computes start and end points of an arc.
  /// @param arcRect Arc bounding rectangle.
  /// @param startAngle Start angle (degrees).
  /// @param sweepAngle Sweep angle (degrees).
  /// @param startPoint Output start point.
  /// @param endPoint Output end point.
  void calculateArcPoints(const QRectF& arcRect, double startAngle,
                          double sweepAngle, QPointF& startPoint,
                          QPointF& endPoint);

private:

  QMap<QString, Trace> traces;   ///< Map of the traces display in the Smith Chart, keyed by name
  QMap<QString, Marker> markers; ///< Map of markers, keyed by name


  double z0;            ///< Characteristic impedance of the diagram [Ohm]
  QPointF lastMousePos; ///< Last mouse position in widget coordinates.
  double scaleFactor;   ///< Zoom factor for the chart.
  double panX;          ///< Horizontal pan offset.
  double panY;          ///< Vertical pan offset

private slots:
  /// @brief Handles changes in Z0 combobox
  void onZ0Changed(int index);

  /// @brief Toggles admittance chart overlay
  /// @param int State of the visibility of the constant admittance lines
  void onShowAdmittanceChartChanged(int state) {
    m_showAdmittanceChart = (state == Qt::Checked);
    update(); // Trigger a repaint
  }

  /// @brief Toggles impedance constant-curve grid.
  /// @param int State of the visibility of the constant impedance lines
  void onShowConstantCurvesChanged(int state) {
    m_showConstantCurves = (state == Qt::Checked);
    update(); // Trigger a repaint
  }

private:
  QComboBox* m_Z0ComboBox;                  ///< Characteristic impedance input widget
  QCheckBox* m_ShowAdmittanceChartCheckBox; ///< Checkbox for admittance chart
  QCheckBox*
      m_ShowConstantCurvesCheckBox;         ///< Checkbox for reactance/resistance curves
  bool m_showConstantCurves;                ///< Flag for curve display
  bool m_showAdmittanceChart;               ///< Flag for admittance chart display
  QVBoxLayout* m_layout;                    ///< Main layout for controls and chart.

private:
  // Frequency range controls
  QDoubleSpinBox* m_minFreqSpinBox;///< Widget that controls the minimum frequency to be displayed
  QDoubleSpinBox* m_maxFreqSpinBox;///< Widget that controls the maximum frequency to be displayed
  QComboBox* m_freqUnitComboBox;   ///< Widget that controls the frequency scaling units
  double m_minFreq;                ///< Actual value of the minimum frequency [Hz]
  double m_maxFreq;                ///< Actual value of the maximum frequency [Hz]

private slots:
  /// @brief Slot called when minimum frequency value changes
  /// @param value Value of the minimum frequency settings
  void onMinFreqChanged(double value);

  /// @brief Slot called when maximum frequency value changes
  /// @param value Value of the maximum frequency settings
  void onMaxFreqChanged(double value);

  /// @brief  Slot called when the frequency scaling changes
  /// \param index Index of the frequency scaling
  void onFreqUnitChanged(int index);

  /// @brief Gets the frequency scaling factor based on the current selection
  /// \return Scale factor: (1, Hz); (1e3, kHz); (1e6, MHz), (1e9, GHz)
  double getFrequencyMultiplier() const;

  /// @brief Updates spinboxes from trace data frequency range.
  void updateFrequencyRange();
};

#endif // SMITHCHARTWIDGET_H
