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

#include "rectangularplotwidget.h"


///
/// @brief Class constructor
/// Sets the QCustomPlot object, default varialbes and creates the UI
/// @param parent Parent widget
///
RectangularPlotWidget::RectangularPlotWidget(QWidget *parent)
    : QWidget(parent), showTraceValues(true), axisSettingsLocked(false),
      fMin(1e20), fMax(-1) {
  // Create the QCustomPlot widget
  plotWidget = new QCustomPlot(this);

  // Set up the frequency units
  frequencyUnits << "Hz" << "kHz" << "MHz" << "GHz";

  y_autoscale = true; // Allow autoscaling when a trace is added

  // Setup the plot
  setupPlot();

  // Create the main layout
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(plotWidget);

  // Add the axis settings layout
  mainLayout->addLayout(setupAxisSettings());
  setLayout(mainLayout);
}

///
/// @brief Class destructor
///
RectangularPlotWidget::~RectangularPlotWidget() {
  // Clean up any remaining graphics items
  clearGraphicsItems();

  // QCustomPlot will be automatically deleted by Qt's parent-child system
}

///
/// @brief Configures the plot properties and the axes. It also creates the connections between the event and their function handlers
///
void RectangularPlotWidget::setupPlot() {
  // Configure basic plot properties
  plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                              QCP::iSelectPlottables);

  // Set up axes
  plotWidget->xAxis->setLabel("Frequency (MHz)");
  plotWidget->yAxis->setLabel("Magnitude (dB)");

  // Set up right y-axis (y2)
  plotWidget->yAxis2->setVisible(true);
  plotWidget->yAxis2->setLabel("Phase (deg)");
  plotWidget->yAxis2->setTickLabels(true);

  // Set up top x-axis to match bottom
  plotWidget->xAxis2->setVisible(true);
  plotWidget->xAxis2->setTickLabels(false);

  // Set default ranges
  plotWidget->xAxis->setRange(0, 1000);
  plotWidget->yAxis->setRange(-10, 10);
  plotWidget->yAxis2->setRange(-180, 180);

  // Hide right y-axis initially
  setRightYAxisEnabled(false);

  // Make left and bottom axes transfer their ranges to right and top axes
  connect(plotWidget->xAxis, SIGNAL(rangeChanged(QCPRange)), plotWidget->xAxis2,
          SLOT(setRange(QCPRange)));
  connect(plotWidget->yAxis, SIGNAL(rangeChanged(QCPRange)), plotWidget->yAxis2,
          SLOT(setRange(QCPRange)));

  // Connect range change signals to update axis settings widgets
  connect(plotWidget->xAxis,
          QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
          &RectangularPlotWidget::onXAxisRangeChanged);
  connect(plotWidget->yAxis,
          QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
          &RectangularPlotWidget::onYAxisRangeChanged);
  connect(plotWidget->yAxis2,
          QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
          &RectangularPlotWidget::onY2AxisRangeChanged);
}


///
/// @brief Add or update a trace on the plot
///
/// Automatically adjusts frequency and y-axis ranges unless settings are locked.
/// Applies auto-scaling with padding (5% of range).
/// Updates the plot to display the new trace.
///
/// @param name Unique trace identifier
/// @param trace Trace data including frequencies, values, units, and axis assignment
///
void RectangularPlotWidget::addTrace(const QString &name, const Trace &trace) {
  // Create a local copy of the trace that we can modify
  Trace traceCopy = trace;

  // Store the trace in the map
  traces[name] = traceCopy;

  // Only update frequency range if not locked and this trace has data
  if (!axisSettingsLocked && !traceCopy.frequencies.isEmpty()) {
    double traceMinFreq = traceCopy.frequencies.first();
    double traceMaxFreq = traceCopy.frequencies.last();

    // Update global min/max frequency (stored in Hz)
    if (traceMinFreq < fMin) {
      fMin = traceMinFreq;
    }
    if (traceMaxFreq > fMax) {
      fMax = traceMaxFreq;
    }

    // Get current frequency scale factor based on selected units
    double freqScale = getXscale();

    // Update the axis limits with the scaled frequency values
    xAxisMin->setValue(fMin * freqScale);
    xAxisMax->setValue(fMax * freqScale);

    // If this is the first trace being added, set a suitable division step
    if (traces.size() == 1) {
      // Calculate a nice step size based on the frequency range
      double range = (fMax - fMin) * freqScale;
      // Aim for approximately 8-10 divisions on the axis
      double step = calculateNiceStep(range);
      xAxisDiv->setValue(step);
    }

    // Update the x-axis
    updateXAxis();
  }

  // Only adjust y-axis and y2-axis ranges if not locked and trace has data
  if (!axisSettingsLocked && !traceCopy.trace.isEmpty() && y_autoscale) {
    // Find min and max values in the trace data
    double traceMin = std::numeric_limits<double>::max();
    double traceMax = std::numeric_limits<double>::lowest();

    const auto &traceConst = traceCopy.trace;        // const reference prevents detach (warning)
    for (double value : traceConst) {
      if (value < traceMin) {
        traceMin = value;
      }
      if (value > traceMax) {
        traceMax = value;
      }
    }


    // Add some padding (5% of range)
    double padding = (traceMax - traceMin) * 0.05;
    if (padding < 1.0) {
      padding = 1.0; // Minimum padding
    }

    // Update appropriate y-axis based on the trace's y_axis value
    if (traceCopy.y_axis == 2) {
      // Only adjust y2-axis if this is the first trace for y2 or if values
      // exceed current range
      if (traceMin < y2AxisMin->value() || traceMax > y2AxisMax->value() ||
          (getY2AxisTraceCount() == 1 && traces.size() == 1)) {
        y2AxisMin->setValue(floor((traceMin - padding) / 10) *
                            10); // Round to nearest 10 below
        y2AxisMax->setValue(ceil((traceMax + padding) / 10) *
                            10); // Round to nearest 10 above
      }
      updateY2Axis();
    } else {
      // Only adjust y-axis if this is the first trace or if values exceed
      // current range
      if (traceMin < yAxisMin->value() || traceMax > yAxisMax->value() ||
          (getYAxisTraceCount() == 1 && traces.size() == 1)) {
        double y_min =
            floor((traceMin - padding) / 5) * 5; // Round to nearest 5 below
        double y_max =
            ceil((traceMax + padding) / 5) * 5; // Round to nearest 5 above
        yAxisMin->setValue(y_min);
        yAxisMax->setValue(y_max);
        double y_step = round((y_max - y_min) / 10);
        y_step = ceil(y_step / 5) * 5; // round to 5
        yAxisDiv->setValue(y_step);
      }
      updateYAxis();
    }
  }

  updatePlot();
}


///
/// @brief Calculate a reasonable step size for axis divisions
///
/// Aims for approximately 8-10 divisions on the axis. Selects nice round
/// numbers (1, 2, 5, or 10 multiplied by appropriate power of 10).
///
/// @param range Total range to be divided
/// @return Optimal step size for tick marks
///
double RectangularPlotWidget::calculateNiceStep(double range) {
  // Target 8-10 divisions on the axis
  double rawStep = range / 8.0;

  // Find the magnitude of the step
  double magnitude = pow(10, floor(log10(rawStep)));

  // Normalize the step to a value between 1 and 10
  double normalizedStep = rawStep / magnitude;

  // Choose a nice step value (1, 2, 2.5, 5, or 10)
  double niceStep;
  if (normalizedStep < 1.5) {
    niceStep = 1;
  } else if (normalizedStep < 3) {
    niceStep = 2;
  } else if (normalizedStep < 7) {
    niceStep = 5;
  } else {
    niceStep = 10;
  }

  // Return the nice step size
  return niceStep * magnitude;
}

///
/// @brief Remove a trace from the plot
/// @param name Trace identifier to remove
///
void RectangularPlotWidget::removeTrace(const QString &name) {
  traces.remove(name);
  updatePlot();
}

///
/// @brief Remove all traces from the plot
///
void RectangularPlotWidget::clearTraces() {
  traces.clear();
  updatePlot();
}

///
/// @brief Get the pen style for a specific trace
/// @param traceName Name of the trace
/// @return QPen for the trace, or default black pen if not found
///
QPen RectangularPlotWidget::getTracePen(const QString &traceName) const {
  if (traces.contains(traceName)) {
    return traces[traceName].pen;
  }

  // Return a default-constructed pen with valid settings
  QPen defaultPen;
  defaultPen.setStyle(Qt::SolidLine);
  defaultPen.setColor(Qt::black);
  return defaultPen;
}

///
/// @brief Set the pen style for a specific trace
/// @param traceName Name of the trace
/// @param pen New pen style to apply
///
void RectangularPlotWidget::setTracePen(const QString &traceName,
                                        const QPen &pen) {
  if (traces.contains(traceName)) {
    traces[traceName].pen = pen;
    updatePlot();
  }
}


///
/// @brief Get information about all traces
/// @return Map of trace names to their pen styles
///
QMap<QString, QPen> RectangularPlotWidget::getTracesInfo() const {
  QMap<QString, QPen> penMap;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    penMap.insert(it.key(), it.value().pen);
  }
  return penMap;
}


///
/// @brief Add a vertical marker at a specific frequency
///
/// Creates a vertical line spanning the plot height at the specified frequency.
/// The marker must be within the frequency range of at least one trace.
/// Also creates intersection points and labels where the marker crosses traces with the trace values.
///
/// @param markerId Unique marker identifier
/// @param frequency Frequency in Hz where marker should be placed
/// @param pen Marker line style (default: red, width 2)
/// @return true if marker added successfully, false if exists or frequency out of range
///
bool RectangularPlotWidget::addMarker(const QString &markerId, double frequency,
                                      const QPen &pen) {
  if (markers.contains(markerId)) {
    return false;
  }

  bool frequencyInRange = false;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const Trace &trace = it.value();
    if (!trace.frequencies.isEmpty() &&
        frequency >= trace.frequencies.first() &&
        frequency <= trace.frequencies.last()) {
      frequencyInRange = true;
      break;
    }
  }

  if (!frequencyInRange) {
    return false;
  }

  Marker marker;
  marker.id = markerId;
  marker.frequency = frequency;
  marker.pen = pen;

  markers.insert(markerId, marker);
  updatePlot();
  return true;
}


///
/// @brief Remove a marker from the plot
/// @param markerId Marker identifier to remove
/// @return true if removed successfully, false if not found
///
bool RectangularPlotWidget::removeMarker(const QString &markerId) {
  if (!markers.contains(markerId)) {
    return false;
  }

  markers.remove(markerId);
  updatePlot();
  return true;
}

///
/// @brief Remove all markers from the plot
///
void RectangularPlotWidget::clearMarkers() {
  markers.clear();
  updatePlot();
}

///
/// @brief Get all markers and their frequencies
/// @return Map of marker IDs to their frequencies in Hz
///
QMap<QString, double> RectangularPlotWidget::getMarkers() const {
  QMap<QString, double> markerFrequencies;
  for (auto it = markers.constBegin(); it != markers.constEnd(); ++it) {
    markerFrequencies.insert(it.key(), it.value().frequency);
  }
  return markerFrequencies;
}

///
/// @brief Redraw the entire plot with current trace, marker, and limit data
///
/// Clears all existing graphics items and graphs, then recreates:
/// - Trace graphs with appropriate y-axis assignment
/// - Marker lines with frequency labels
/// - Intersection points with value labels
/// - Limit lines
/// - Shows/hides right y-axis based on trace assignments
/// @see addMarkerIntersections()
///
void RectangularPlotWidget::updatePlot() {
  // Clear existing graphics items and graphs
  clearGraphicsItems();
  plotWidget->clearGraphs();

  // Get the current scale factor based on selected units
  double freqScale = getXscale();

  // Add each trace as a new graph
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const QString &name = it.key();
    const Trace &trace = it.value();

    // Create a new graph for the trace
    QCPGraph *graph = plotWidget->addGraph();

    // Determine which y-axis to use
    if (trace.y_axis == 2) {
      graph->setKeyAxis(plotWidget->xAxis);
      graph->setValueAxis(plotWidget->yAxis2);
    } else {
      graph->setKeyAxis(plotWidget->xAxis);
      graph->setValueAxis(plotWidget->yAxis);
    }

    // Set the pen
    graph->setPen(trace.pen);
    graph->setName(name);

    // Add data points to the graph with proper frequency scaling
    QVector<double> xData, yData;
    for (int i = 0; i < trace.frequencies.size() && i < trace.trace.size();
         ++i) {
      // Scale the frequency values according to the current frequency units
      double scaledFreq = trace.frequencies[i] * freqScale;
      xData.append(scaledFreq);
      yData.append(trace.trace[i]);
    }

    graph->setData(xData, yData);

    // Store reference for future use
    traceGraphs[name] = graph;
  }

  // Show/hide right y-axis based on whether we have traces using it
  if (getY2AxisTraceCount() == 0) {
    setRightYAxisEnabled(false);
  } else {
    setRightYAxisEnabled(true);
  }

  // Draw markers if any
  for (auto it = markers.constBegin(); it != markers.constEnd(); ++it) {
    const Marker &marker = it.value();

    // Scale the marker frequency according to the current units
    double scaledMarkerFreq = marker.frequency * freqScale;

    // Create a vertical line for the marker
    QCPItemStraightLine *markerLine = new QCPItemStraightLine(plotWidget);
    markerLine->point1->setCoords(scaledMarkerFreq, -1e20);
    markerLine->point2->setCoords(scaledMarkerFreq, 1e20);
    markerLine->setPen(marker.pen);
    markerLines[marker.id] = markerLine;

    // Create a text label showing the frequency value
    QString unitText = xAxisUnits->currentText();
    QString freqText =
        QString::number(scaledMarkerFreq, 'f', 1) + " " + unitText;

    QCPItemText *markerLabel = new QCPItemText(plotWidget);
    markerLabel->setText(freqText);
    markerLabel->position->setCoords(scaledMarkerFreq,
                                     plotWidget->yAxis->range().upper);
    markerLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    markerLabel->setBrush(QBrush(Qt::white));
    markerLabel->setPen(marker.pen);
    markerLabels[marker.id] = markerLabel;

    // Add marker intersections with all traces
    addMarkerIntersections(marker.id, marker);
  }

  // Draw limits if any
  for (auto it = limits.constBegin(); it != limits.constEnd(); ++it) {
    const Limit &limit = it.value();

    // Create a graph for the limit
    QCPGraph *limitGraph = plotWidget->addGraph();

    // Determine which y-axis to use
    if (limit.y_axis == 1) {
      limitGraph->setKeyAxis(plotWidget->xAxis);
      limitGraph->setValueAxis(plotWidget->yAxis2);
    } else {
      limitGraph->setKeyAxis(plotWidget->xAxis);
      limitGraph->setValueAxis(plotWidget->yAxis);
    }

    // Set the pen
    limitGraph->setPen(limit.pen);
    limitGraph->setName(it.key());

    // Scale the frequencies according to the current units
    double scaledF1 = limit.f1 * freqScale;
    double scaledF2 = limit.f2 * freqScale;

    // Add the two points defining the limit line
    QVector<double> xData = {scaledF1, scaledF2};
    QVector<double> yData = {limit.y1, limit.y2};
    limitGraph->setData(xData, yData);

    limitGraphs[it.key()] = limitGraph;
  }

  // Replot to show all changes
  plotWidget->replot();
}

///
/// @brief Add intersection markers where a marker line crosses traces
///
/// For each trace, calculates the intersection point using linear interpolation
/// and creates a tracer (circular marker) at that point. Optionally adds a
/// text label showing the interpolated value.
///
/// @param markerId Marker identifier
/// @param marker Marker data containing frequency and style
/// @see updatePlot()
///
void RectangularPlotWidget::addMarkerIntersections(const QString &markerId,
                                                   const Marker &marker) {
  double freqScale = getXscale();
  double scaledMarkerFreq = marker.frequency * freqScale;

  for (auto traceIt = traces.constBegin(); traceIt != traces.constEnd();
       ++traceIt) {
    const Trace &trace = traceIt.value();

    // Find the intersection point of the marker with this trace
    double intersectionValue = -std::numeric_limits<double>::max();
    bool found = false;

    // Check if marker frequency is within trace's frequency range
    if (!trace.frequencies.isEmpty() &&
        marker.frequency >= trace.frequencies.first() &&
        marker.frequency <= trace.frequencies.last()) {

      // Find the closest frequency points in the trace
      int lowerIndex = -1;
      for (int i = 0; i < trace.frequencies.size() - 1; ++i) {
        if (trace.frequencies[i] <= marker.frequency &&
            marker.frequency <= trace.frequencies[i + 1]) {
          lowerIndex = i;
          break;
        }
      }

      // If we found an interval containing the marker frequency
      if (lowerIndex >= 0) {
        // Linear interpolation to find the value at marker frequency
        double f1 = trace.frequencies[lowerIndex];
        double f2 = trace.frequencies[lowerIndex + 1];
        double v1 = trace.trace[lowerIndex];
        double v2 = trace.trace[lowerIndex + 1];

        // Linear interpolation formula: v = v1 + (f - f1) * (v2 - v1) / (f2 -
        // f1)
        intersectionValue =
            v1 + (marker.frequency - f1) * (v2 - v1) / (f2 - f1);
        found = true;
      }
    }

    // If intersection was found, add a point marker
    if (found) {
      QString pointId = markerId + "_" + traceIt.key();

      // Create a tracer for the intersection point
      QCPItemTracer *tracer = new QCPItemTracer(plotWidget);

      // Find the corresponding graph
      if (traceGraphs.contains(traceIt.key())) {
        tracer->setGraph(traceGraphs[traceIt.key()]);
        tracer->setGraphKey(scaledMarkerFreq);
        tracer->setInterpolating(true);
        tracer->setStyle(QCPItemTracer::tsCircle);
        tracer->setPen(QPen(Qt::black));
        tracer->setBrush(QBrush(trace.pen.color()));
        tracer->setSize(7);

        intersectionPoints[pointId] = tracer;

        // Add value label for the intersection point
        if (showTraceValues) {
          QCPItemText *valueLabel = new QCPItemText(plotWidget);
          QString valueText = QString::number(intersectionValue, 'f', 1);
          valueText += QString(" %1").arg(trace.units);

          valueLabel->setText(valueText);
          valueLabel->position->setCoords(scaledMarkerFreq, intersectionValue);
          valueLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
          valueLabel->setBrush(QBrush(Qt::white));
          valueLabel->setPen(trace.pen);

          intersectionLabels[pointId] = valueLabel;
        }
      }
    }
  }
}

///
/// @brief Update X-axis based on control widget values
///
/// Applies range and tick settings from spin boxes. Prevents excessive
/// tick density and updates axis label with current frequency units.
///
void RectangularPlotWidget::updateXAxis() {
  double xMin = xAxisMin->value();
  double xMax = xAxisMax->value();
  double xDiv = xAxisDiv->value();

  double min_step = (xMax - xMin) / 10;
  if (xDiv < min_step) {
    // Avoid excessive ticking
    xDiv = min_step;
    xAxisDiv->setValue(xDiv);
  }

  // Temporarily disconnect the range changed signal to avoid feedback
  disconnect(plotWidget->xAxis,
             QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
             &RectangularPlotWidget::onXAxisRangeChanged);

  // Set the axis range in display units
  plotWidget->xAxis->setRange(xMin, xMax);

  // Reconnect the signal
  connect(plotWidget->xAxis,
          QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
          &RectangularPlotWidget::onXAxisRangeChanged);

  // Set ticks
  QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
  fixedTicker->setTickStep(xDiv);
  fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
  plotWidget->xAxis->setTicker(fixedTicker);

  // Set title
  plotWidget->xAxis->setLabel("Frequency (" + xAxisUnits->currentText() + ")");

  // Update the plot
  updatePlot();
}

///
/// @brief Update left Y-axis based on control widget values
///
void RectangularPlotWidget::updateYAxis() {
  double yMin = yAxisMin->value();
  double yMax = yAxisMax->value();
  double yDiv = yAxisDiv->value();

  double min_step = (yMax - yMin) / 20;

  if (yDiv < min_step) {
    // Avoid excessive ticking
    yDiv = min_step;
    yAxisDiv->setValue(yDiv);
  }

  // Temporarily disconnect the range changed signal to avoid feedback
  disconnect(plotWidget->yAxis,
             QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
             &RectangularPlotWidget::onYAxisRangeChanged);

  plotWidget->yAxis->setRange(yMin, yMax);

  // Reconnect the signal
  connect(plotWidget->yAxis,
          QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
          &RectangularPlotWidget::onYAxisRangeChanged);

  // Set ticks
  QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
  fixedTicker->setTickStep(yDiv);
  fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
  plotWidget->yAxis->setTicker(fixedTicker);

  updatePlot();
}

///
/// @brief Update right Y-axis based on control widget values
///
void RectangularPlotWidget::updateY2Axis() {
  double y2Min = y2AxisMin->value();
  double y2Max = y2AxisMax->value();
  double y2Div = y2AxisDiv->value();

  // Temporarily disconnect the range changed signal to avoid feedback
  disconnect(plotWidget->yAxis2,
             QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
             &RectangularPlotWidget::onY2AxisRangeChanged);

  plotWidget->yAxis2->setRange(y2Min, y2Max);

  // Reconnect the signal
  connect(plotWidget->yAxis2,
          QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this,
          &RectangularPlotWidget::onY2AxisRangeChanged);

  // Set ticks
  QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
  fixedTicker->setTickStep(y2Div);
  fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
  plotWidget->yAxis2->setTicker(fixedTicker);

  updatePlot();
}

///
/// @brief Handle frequency unit changes and rescale X-axis
///
/// Updates spin box values to reflect frequencies in the new unit scale
/// (Hz, kHz, MHz, or GHz) without changing the actual frequency range.
///
void RectangularPlotWidget::changeFreqUnits() {
  // Get the current scale factor based on selected units
  double freqScale = getXscale();

  // Block signals temporarily to avoid triggering updateXAxis() multiple times
  xAxisMin->blockSignals(true);
  xAxisMax->blockSignals(true);

  // Update the spinbox values with scaled values from the global min/max (in
  // Hz)
  xAxisMin->setValue(fMin * freqScale);
  xAxisMax->setValue(fMax * freqScale);

  // Re-enable signals
  xAxisMin->blockSignals(false);
  xAxisMax->blockSignals(false);

  // Update the axis
  updateXAxis();
}

///
/// @brief Create and configure axis settings UI widgets
/// @return Grid layout containing all axis configuration controls
///
QGridLayout *RectangularPlotWidget::setupAxisSettings() {
  QGridLayout *axisLayout = new QGridLayout();

  // X-axis settings
  xAxisLabel = new QLabel("<b>Frequency</b>");
  axisLayout->addWidget(xAxisLabel, 0, 0);

  xAxisMin = new QDoubleSpinBox();
  xAxisMin->setMinimum(0.1);
  xAxisMin->setMaximum(1000000);
  xAxisMin->setValue(0);
  xAxisMin->setDecimals(1);
  xAxisMin->setSingleStep(0.1);
  connect(xAxisMin, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateXAxis);

  axisLayout->addWidget(xAxisMin, 0, 1);

  xAxisMax = new QDoubleSpinBox();
  xAxisMax->setMinimum(0.1);
  xAxisMax->setMaximum(1000000);
  xAxisMax->setValue(1000);
  xAxisMax->setDecimals(1);
  xAxisMax->setSingleStep(0.1);
  connect(xAxisMax, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateXAxis);
  axisLayout->addWidget(xAxisMax, 0, 2);

  xAxisDiv = new QDoubleSpinBox();
  xAxisDiv->setMinimum(0.1);
  xAxisDiv->setMaximum(1000000);
  xAxisDiv->setValue(100);
  xAxisDiv->setSingleStep(1);
  connect(xAxisDiv, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateXAxis);

  axisLayout->addWidget(xAxisDiv, 0, 3);

  xAxisUnits = new QComboBox();
  xAxisUnits->addItems(frequencyUnits);
  xAxisUnits->setCurrentIndex(2);
  connect(xAxisUnits, &QComboBox::currentIndexChanged, this,
          [this](int) { changeFreqUnits(); });

  axisLayout->addWidget(xAxisUnits, 0, 4);

  // Left Y-axis settings
  QLabel *yAxisLabel = new QLabel("<b>y-axis</b>");
  axisLayout->addWidget(yAxisLabel, 1, 0);

  yAxisMin = new QDoubleSpinBox();
  yAxisMin->setMinimum(-1000000);
  yAxisMin->setMaximum(1000000);
  yAxisMin->setValue(-10);
  yAxisMin->setDecimals(1);
  yAxisMin->setSingleStep(1);
  connect(yAxisMin, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateYAxis);
  axisLayout->addWidget(yAxisMin, 1, 1);

  yAxisMax = new QDoubleSpinBox();
  yAxisMax->setMinimum(-1000000);
  yAxisMax->setMaximum(1000000);
  yAxisMax->setValue(10);
  yAxisMax->setDecimals(1);
  yAxisMax->setSingleStep(1);
  connect(yAxisMax, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateYAxis);

  axisLayout->addWidget(yAxisMax, 1, 2);

  yAxisDiv = new QDoubleSpinBox();
  yAxisDiv->setMinimum(0.1);
  yAxisDiv->setMaximum(1000000);
  yAxisDiv->setValue(5);
  yAxisDiv->setSingleStep(1);
  connect(yAxisDiv, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateYAxis);
  axisLayout->addWidget(yAxisDiv, 1, 3);

  yAxisUnits = new QLabel("dB");
  axisLayout->addWidget(yAxisUnits, 1, 4);

  // Right Y-axis settings
  y2AxisLabel = new QLabel("<b>y2-axis</b>");
  axisLayout->addWidget(y2AxisLabel, 2, 0);

  y2AxisMin = new QDoubleSpinBox();
  y2AxisMin->setMinimum(-1000000);
  y2AxisMin->setMaximum(1000000);
  y2AxisMin->setValue(-180);
  y2AxisMin->setDecimals(1);
  y2AxisMin->setSingleStep(10);
  connect(y2AxisMin, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateY2Axis);
  axisLayout->addWidget(y2AxisMin, 2, 1);

  y2AxisMax = new QDoubleSpinBox();
  y2AxisMax->setMinimum(-1000000);
  y2AxisMax->setMaximum(1000000);
  y2AxisMax->setValue(180);
  y2AxisMax->setDecimals(1);
  y2AxisMax->setSingleStep(10);
  connect(y2AxisMax, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateY2Axis);

  axisLayout->addWidget(y2AxisMax, 2, 2);

  y2AxisDiv = new QDoubleSpinBox();
  y2AxisDiv->setMinimum(0.1);
  y2AxisDiv->setMaximum(180);
  y2AxisDiv->setValue(45);
  y2AxisDiv->setSingleStep(5);
  connect(y2AxisDiv, &QDoubleSpinBox::valueChanged, this,
          &RectangularPlotWidget::updateY2Axis);
  axisLayout->addWidget(y2AxisDiv, 2, 3);

  y2AxisUnits = new QLabel("deg");
  axisLayout->addWidget(y2AxisUnits, 2, 4);

  // Add the Show Values checkbox in a new row
  QLabel *markerOptionsLabel = new QLabel("<b>Marker Options</b>");
  axisLayout->addWidget(markerOptionsLabel, 3, 0);

  showValuesCheckbox = new QCheckBox("Show Data Values");
  showValuesCheckbox->setChecked(true); // Default to showing values
  connect(showValuesCheckbox, &QCheckBox::toggled, this,
          &RectangularPlotWidget::toggleShowValues);

  axisLayout->addWidget(showValuesCheckbox, 3, 1, 1, 2); // Span 2 columns

  // Lock Axis Settings checkbox
  lockAxisCheckbox = new QCheckBox("Lock Axis Settings");
  lockAxisCheckbox->setChecked(false); // Default to unlocked
  connect(lockAxisCheckbox, &QCheckBox::toggled, this,
          &RectangularPlotWidget::toggleLockAxisSettings);

  axisLayout->addWidget(lockAxisCheckbox, 3, 3, 1, 2); // Span 2 columns

  return axisLayout;
}

// These "get" functions are used by the main program to put markers and limits
///
/// @brief Get minimum value of left Y-axis
/// @return Current minimum value
///
double RectangularPlotWidget::getYmin() { return yAxisMin->value(); }

///
/// @brief Get maximum value of left Y-axis
/// @return Current maximum value
///
double RectangularPlotWidget::getYmax() { return yAxisMax->value(); }

///
/// @brief Get division step of left Y-axis
/// @return Current division step
///
double RectangularPlotWidget::getYdiv() { return yAxisDiv->value(); }

///
/// @brief Set division step of left Y-axis
/// @param val New division step value
///
void RectangularPlotWidget::setYdiv(double val) { yAxisDiv->setValue(val); }

///
/// @brief Set minimum value of left Y-axis
/// @param val New minimum value
///
void RectangularPlotWidget::setYmin(double val) { yAxisMin->setValue(val); }

///
/// @brief Set maximum value of left Y-axis
/// @param val New maximum value
///
void RectangularPlotWidget::setYmax(double val) { yAxisMax->setValue(val); }

///
/// @brief Get minimum value of right Y-axis
/// @return Current minimum value
///
double RectangularPlotWidget::getY2min() { return y2AxisMin->value(); }

///
/// @brief Get maximum value of right Y-axis
/// @return Current maximum value
///
double RectangularPlotWidget::getY2max() { return y2AxisMax->value(); }

///
/// @brief Get division step of right Y-axis
/// @return Current division step
///
double RectangularPlotWidget::getY2div() { return y2AxisDiv->value(); }

///
/// @brief Get minimum value of X-axis
/// @return Current minimum value
///
double RectangularPlotWidget::getXmin() { return xAxisMin->value(); }

///
/// @brief Get maximum value of X-axis
/// @return Current maximum value
///
double RectangularPlotWidget::getXmax() { return xAxisMax->value(); }

///
/// @brief Get division step of X-axis
/// @return Current division step
///
double RectangularPlotWidget::getXdiv() { return xAxisDiv->value(); }

///
/// @brief Get current frequency units
/// @return String containing current units (Hz, kHz, MHz, or GHz)
///
QString RectangularPlotWidget::getXunits() { return xAxisUnits->currentText(); }

///
/// @brief Get the number of traces assigned to the left y-axis
/// @return Number of traces using left y-axis
///
int RectangularPlotWidget::getYAxisTraceCount() const {
  int count = 0;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    if (it.value().y_axis != 2) {
      count++;
    }
  }
  return count;
}

///
/// @brief Get the number of traces assigned to the right y-axis
/// @return Number of traces using right y-axis
///
int RectangularPlotWidget::getY2AxisTraceCount() const {
  int count = 0;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    if (it.value().y_axis == 2) {
      count++;
    }
  }
  return count;
}

///
/// @brief Get scale factor based on selected frequency units
/// @return Scale factor (1 for Hz, 1e-3 for kHz, 1e-6 for MHz, 1e-9 for GHz)
///
double RectangularPlotWidget::getXscale() {
  QString unit = xAxisUnits->currentText();
  double freqScale = 1;
  if (unit == "kHz") {
    freqScale = 1e-3; // Hz to kHz
  } else if (unit == "MHz") {
    freqScale = 1e-6; // Hz to MHz
  } else if (unit == "GHz") {
    freqScale = 1e-9; // Hz to GHz
  }
  return freqScale;
}

///
/// @brief Get the index of the frequency units combobox
/// @return Current frequency unit index
///
int RectangularPlotWidget::getFreqIndex() { return xAxisUnits->currentIndex(); }


///
/// @brief Update an existing marker's frequency
/// @param markerId Marker identifier
/// @param newFrequency New frequency in Hz
/// @return true if updated successfully, false if marker not found or frequency out of range
///
bool RectangularPlotWidget::updateMarkerFrequency(const QString &markerId,
                                                  double newFrequency) {
  // Check if marker exists
  if (!markers.contains(markerId)) {
    return false;
  }

  // Check if any trace contains this frequency
  bool frequencyInRange = false;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const Trace &trace = it.value();
    if (!trace.frequencies.isEmpty() &&
        newFrequency >= trace.frequencies.first() &&
        newFrequency <= trace.frequencies.last()) {
      frequencyInRange = true;
      break;
    }
  }

  if (!frequencyInRange) {
    return false; // Frequency is not within the range of any trace
  }

  // Update the marker's frequency
  markers[markerId].frequency = newFrequency;

  // Trigger repaint
  updatePlot();
  return true;
}

///
/// @brief Remove old graphic elements from the plot, such as the labels from past marker
/// positions
///
void RectangularPlotWidget::clearGraphicsItems() {
  // Remove all marker lines
  for (auto it = markerLines.begin(); it != markerLines.end(); ++it) {
    plotWidget->removeItem(it.value());
  }
  markerLines.clear();

  // Remove all marker labels
  for (auto it = markerLabels.begin(); it != markerLabels.end(); ++it) {
    plotWidget->removeItem(it.value());
  }
  markerLabels.clear();

  // Remove all intersection points
  for (auto it = intersectionPoints.begin(); it != intersectionPoints.end();
       ++it) {
    plotWidget->removeItem(it.value());
  }
  intersectionPoints.clear();

  // Remove all intersection labels
  for (auto it = intersectionLabels.begin(); it != intersectionLabels.end();
       ++it) {
    plotWidget->removeItem(it.value());
  }
  intersectionLabels.clear();

  // Remove all limit graphs
  for (auto it = limitGraphs.begin(); it != limitGraphs.end(); ++it) {
    plotWidget->removeGraph(it.value());
  }
  limitGraphs.clear();

  // Clear trace graphs map
  traceGraphs.clear();
}

///
/// @brief Toggle display of trace values at the intersections with the vertical marker
/// @param show true to show values, false to hide
///
void RectangularPlotWidget::toggleShowValues(bool show) {
  showTraceValues = show;
  updatePlot(); // Redraw with new setting
}

///
/// @brief Add a limit line to the plot
/// @param limitId Unique limit identifier
/// @param limit Limit data including coordinates and style
/// @return true if added successfully, false if limit already exists
///
bool RectangularPlotWidget::addLimit(const QString &limitId,
                                     const Limit &limit) {
  // Check if limit with this ID already exists
  if (limits.contains(limitId)) {
    return false;
  }

  // Store the limit in the map
  limits.insert(limitId, limit);

  // Update the plot to show the new limit
  updatePlot();
  return true;
}

///
/// @brief Remove a limit line from the plot
/// @param limitId Identifier of the limit to remove
///
void RectangularPlotWidget::removeLimit(const QString &limitId) {
  // Remove the limit if it exists
  if (limits.contains(limitId)) {
    limits.remove(limitId);
    // Update the plot to reflect the removal
    updatePlot();
  }
}

///
/// @brief Remove all limit lines from the plot
///
void RectangularPlotWidget::clearLimits() {
  limits.clear();
  updatePlot();
}

///
/// @brief Get all limits currently on the plot
/// @return Map of limit IDs to their limit data
///
QMap<QString, RectangularPlotWidget::Limit>
RectangularPlotWidget::getLimits() const {
  return limits;
}

///
/// @brief Update an existing limit line (by name)
/// @param limitId Limit identifier
/// @param limit New limit data
/// @return true if updated successfully, false if limit not found
///
bool RectangularPlotWidget::updateLimit(const QString &limitId,
                                        const Limit &limit) {
  // Check if the limit exists
  if (!limits.contains(limitId)) {
    return false;
  }

  // Update the limit in the map
  limits[limitId] = limit;

  // Update the plot to reflect the changes
  updatePlot();

  return true;
}

///
/// @brief Change the title of the left Y-axis
/// @param title New axis title
///
void RectangularPlotWidget::change_Y_axis_title(QString title) {
  plotWidget->yAxis->setLabel(title);
  plotWidget->replot();
}

///
/// @brief Change the units label of the left Y-axis
/// @param units New units text
///
void RectangularPlotWidget::change_Y_axis_units(QString units) {
  yAxisUnits->setText(units);
}

///
/// @brief Change the title of the right Y-axis
/// @param title New axis title
///
void RectangularPlotWidget::change_Y2_axis_title(QString title) {
  plotWidget->yAxis2->setLabel(title);
  plotWidget->replot();
}

///
/// @brief Change the units label of the right Y-axis
/// @param units New units text
///
void RectangularPlotWidget::change_Y2_axis_units(QString units) {
  y2AxisUnits->setText(units);
}

///
/// @brief Change the title of the X-axis
/// @param title New axis title
///
void RectangularPlotWidget::change_X_axis_title(QString title) {
  plotWidget->xAxis->setLabel(title);
  plotWidget->replot();
}

///
/// @brief Change the label of the X-axis (alias for change_X_axis_title)
/// @param title New axis label
///
void RectangularPlotWidget::change_X_axis_label(QString title) {
  // This function appears to be unused in the original code
  change_X_axis_title(title);
}

///
/// @brief Enable or disable the right Y-axis
/// @param enabled true to show right y-axis, false to hide
///
void RectangularPlotWidget::setRightYAxisEnabled(bool enabled) {
  // Hide or show the right y-axis
  plotWidget->yAxis2->setVisible(enabled);
  plotWidget->yAxis2->setTickLabels(enabled);

  // Hide/show the y2-axis label and related controls
  /*y2AxisLabel->setVisible(enabled);
  y2AxisMin->setVisible(enabled);
  y2AxisMax->setVisible(enabled);
  y2AxisDiv->setVisible(enabled);
  y2AxisUnits->setVisible(enabled);*/

  // Redraw the plot to reflect changes
  plotWidget->replot();
}

///
/// @brief Check if right Y-axis is currently enabled
/// @return true if right y-axis is visible, false otherwise
///
bool RectangularPlotWidget::isRightYAxisEnabled() const {
  return plotWidget->yAxis2->visible();
}

///
/// @brief Toggle lock state of axis settings
/// @param locked true to lock axis settings, false to unlock
///
void RectangularPlotWidget::toggleLockAxisSettings(bool locked) {
  axisSettingsLocked = locked;

  // Enable/disable all axis controls based on lock state
  xAxisMin->setEnabled(!locked);
  xAxisMax->setEnabled(!locked);
  xAxisDiv->setEnabled(!locked);
  xAxisUnits->setEnabled(!locked);

  yAxisMin->setEnabled(!locked);
  yAxisMax->setEnabled(!locked);
  yAxisDiv->setEnabled(!locked);

  y2AxisMin->setEnabled(!locked);
  y2AxisMax->setEnabled(!locked);
  y2AxisDiv->setEnabled(!locked);

  // Visual feedback - change the appearance of labels when locked
  QColor labelColor = locked ? QColor(120, 120, 120) : QColor(0, 0, 0);

  QPalette pal = xAxisLabel->palette();
  pal.setColor(QPalette::WindowText, labelColor);

  xAxisLabel->setPalette(pal);
  y2AxisLabel->setPalette(pal);

  // Find the y-axis label and update its palette too
  for (QObject *child : children()) {
    QLabel *label = qobject_cast<QLabel *>(child);
    if (label && label->text() == "<b>y-axis</b>") {
      label->setPalette(pal);
      break;
    }
  }
}

///
/// @brief Check if axis settings are currently locked
/// @return true if axis settings are locked, false otherwise
///
bool RectangularPlotWidget::areAxisSettingsLocked() const {
  return axisSettingsLocked;
}

///
/// @brief Enable or disable Y-axis autoscaling
/// @param value true to enable autoscaling, false to disable
///
void RectangularPlotWidget::set_y_autoscale(bool value) { y_autoscale = value; }

///
/// @brief Get current axis settings
/// @return AxisSettings structure containing all axis configuration values
///
RectangularPlotWidget::AxisSettings RectangularPlotWidget::getSettings() const {
  AxisSettings settings;
  settings.xAxisMin = xAxisMin->value();
  settings.xAxisMax = xAxisMax->value();
  settings.xAxisDiv = xAxisDiv->value();
  settings.xAxisUnits = xAxisUnits->currentText();

  settings.yAxisMin = yAxisMin->value();
  settings.yAxisMax = yAxisMax->value();
  settings.yAxisDiv = yAxisDiv->value();

  settings.y2AxisMin = y2AxisMin->value();
  settings.y2AxisMax = y2AxisMax->value();
  settings.y2AxisDiv = y2AxisDiv->value();

  settings.showValues = showValuesCheckbox->isChecked();
  settings.lockAxis = lockAxisCheckbox->isChecked();

  return settings;
}

///
/// @brief Apply axis settings from AxisSettings structure
/// @param settings AxisSettings structure with configuration values to apply
///
void RectangularPlotWidget::setSettings(const AxisSettings &settings) {
  xAxisMin->setValue(settings.xAxisMin);
  xAxisMax->setValue(settings.xAxisMax);
  xAxisDiv->setValue(settings.xAxisDiv);
  int index = xAxisUnits->findText(settings.xAxisUnits);
  if (index != -1) {
    xAxisUnits->setCurrentIndex(index);
  }

  yAxisMin->setValue(settings.yAxisMin);
  yAxisMax->setValue(settings.yAxisMax);
  yAxisDiv->setValue(settings.yAxisDiv);

  y2AxisMin->setValue(settings.y2AxisMin);
  y2AxisMax->setValue(settings.y2AxisMax);
  y2AxisDiv->setValue(settings.y2AxisDiv);

  showValuesCheckbox->setChecked(settings.showValues);
  lockAxisCheckbox->setChecked(settings.lockAxis);

  // Update axes to reflect new settings
  updateXAxis();
  updateYAxis();
  updateY2Axis();
  updatePlot();
}

/*
  FUNCTIONS FOR HANDLING THE AXIS SETUP WIDGETS WHEN THE USER ZOOMS OR PANS THE
  GRAPH
 */

///
/// @brief Handle X-axis range changes from user interaction
/// @param range New X-axis range
///
void RectangularPlotWidget::onXAxisRangeChanged(const QCPRange &range) {
  // Only update if axis settings are not locked and the change wasn't triggered
  // by our own update
  if (!axisSettingsLocked) {
    // Block signals to prevent recursive updates
    xAxisMin->blockSignals(true);
    xAxisMax->blockSignals(true);

    // Update the spin boxes with the new range
    xAxisMin->setValue(range.lower);
    xAxisMax->setValue(range.upper);

    // Calculate appropriate division step based on new range
    double rangeSize = range.upper - range.lower;
    double newDiv = calculateNiceStep(rangeSize);
    xAxisDiv->blockSignals(true);
    xAxisDiv->setValue(newDiv);
    xAxisDiv->blockSignals(false);

    // Re-enable signals
    xAxisMin->blockSignals(false);
    xAxisMax->blockSignals(false);
  }
}

///
/// @brief Handle left Y-axis range changes from user interaction
/// @param range New Y-axis range
///
void RectangularPlotWidget::onYAxisRangeChanged(const QCPRange &range) {
  // Only update if axis settings are not locked
  if (!axisSettingsLocked) {
    // Block signals to prevent recursive updates
    yAxisMin->blockSignals(true);
    yAxisMax->blockSignals(true);
    yAxisDiv->blockSignals(true);

    // Update the spin boxes with the new range
    yAxisMin->setValue(range.lower);
    yAxisMax->setValue(range.upper);

    // Calculate appropriate division step
    double rangeSize = range.upper - range.lower;
    double newDiv = rangeSize / 10.0; // Aim for about 10 divisions
    // Round to nice values (1, 2, 5, 10, etc.)
    newDiv = calculateNiceStep(rangeSize);
    yAxisDiv->setValue(newDiv);

    // Re-enable signals
    yAxisMin->blockSignals(false);
    yAxisMax->blockSignals(false);
    yAxisDiv->blockSignals(false);
  }
}

//
/// @brief Handle right Y-axis range changes from user interaction
/// @param range New Y2-axis range
///
void RectangularPlotWidget::onY2AxisRangeChanged(const QCPRange &range) {
  // Only update if axis settings are not locked
  if (!axisSettingsLocked) {
    // Block signals to prevent recursive updates
    y2AxisMin->blockSignals(true);
    y2AxisMax->blockSignals(true);
    y2AxisDiv->blockSignals(true);

    // Update the spin boxes with the new range
    y2AxisMin->setValue(range.lower);
    y2AxisMax->setValue(range.upper);

    // Calculate appropriate division step
    double rangeSize = range.upper - range.lower;
    double newDiv = calculateNiceStep(rangeSize);
    y2AxisDiv->setValue(newDiv);

    // Re-enable signals
    y2AxisMin->blockSignals(false);
    y2AxisMax->blockSignals(false);
    y2AxisDiv->blockSignals(false);
  }
}
