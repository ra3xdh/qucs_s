#include "rectangularplotwidget.h"

RectangularPlotWidget::RectangularPlotWidget(QWidget *parent)
    : QWidget(parent), showTraceValues(true), axisSettingsLocked(false), fMin(1e20), fMax(-1)
{
  // Initialize the chart and chart view
  ChartWidget = new QChart();
  ChartWidget->legend()->hide();
  chartView = new QChartView(ChartWidget, this);
  chartView->setRenderHint(QPainter::Antialiasing);

         // Initialize axes
  xAxis = new QValueAxis();
  yAxis = new QValueAxis();
  y2Axis = new QValueAxis();

  ChartWidget->addAxis(xAxis, Qt::AlignBottom);
  ChartWidget->addAxis(yAxis, Qt::AlignLeft);
  ChartWidget->addAxis(y2Axis, Qt::AlignRight);

  yAxis->setTitleText("Magnitude (dB)");
  y2Axis->setTitleText("Phase (deg)");

         // Set up the frequency units
  frequencyUnits << "Hz" << "kHz" << "MHz" << "GHz";

  y_autoscale = true; // Allow autoscaling when a trace is added

         // Initialize our marker label lists
  markerLabels.clear();
  intersectionLabels.clear();

         // Create the main layout
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(chartView);

         // Add the axis settings layout
  mainLayout->addLayout(setupAxisSettings());
  setLayout(mainLayout);
}

RectangularPlotWidget::~RectangularPlotWidget()
{
  // Clean up any remaining graphics items
  clearGraphicsItems();

  // Delete the chart (which will delete all series)
  delete ChartWidget;
}


void RectangularPlotWidget::addTrace(const QString& name, const Trace& trace)
{
  // Create a local copy of the trace that we can modify
  Trace traceCopy = trace;

  // Store the (potentially modified) trace in the map
  traces[name] = traceCopy;

  // Only update frequency range if not locked and this trace has data
  if (!axisSettingsLocked && !traceCopy.frequencies.isEmpty()) {
    double traceMinFreq = traceCopy.frequencies.first();
    double traceMaxFreq = traceCopy.frequencies.last();

    // Update global min/max frequency (stored in Hz)
    if (traceMinFreq < fMin) fMin = traceMinFreq;
        if (traceMaxFreq > fMax) fMax = traceMaxFreq;

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

    for (double value : traceCopy.trace) {
      if (value < traceMin) traceMin = value;
            if (value > traceMax) traceMax = value;
        }

    // Add some padding (5% of range)
    double padding = (traceMax - traceMin) * 0.05;
    if (padding < 1.0) padding = 1.0; // Minimum padding

    // Update appropriate y-axis based on the trace's y_axis value
    if (traceCopy.y_axis == 2) {
      // Only adjust y2-axis if this is the first trace for y2 or if values exceed current range
      if (traceMin < y2AxisMin->value() || traceMax > y2AxisMax->value() ||
          (getY2AxisTraceCount() == 1 && traces.size() == 1)) {
        y2AxisMin->setValue(floor((traceMin - padding) / 10) * 10); // Round to nearest 10 below
        y2AxisMax->setValue(ceil((traceMax + padding) / 10) * 10);  // Round to nearest 10 above
      }
      updateY2Axis();
    } else {
      // Only adjust y-axis if this is the first trace or if values exceed current range
      if (traceMin < yAxisMin->value() || traceMax > yAxisMax->value() ||
          (getYAxisTraceCount() == 1 && traces.size() == 1)) {
        double y_min = floor((traceMin - padding) / 5) * 5; // Round to nearest 5 below
        double y_max = ceil((traceMax + padding) / 5) * 5; // Round to nearest 5 above
        yAxisMin->setValue(y_min);
        yAxisMax->setValue(y_max);
        double y_step = round((y_max - y_min)/10);
        y_step = ceil(y_step / 5) * 5; // round to 5
        yAxisDiv->setValue(y_step);

      }
      updateYAxis();
    }
  }

  updatePlot();
}


// The first time a trace is added to the plot it is needed to find a suitable step. This function helps to do that
double RectangularPlotWidget::calculateNiceStep(double range)
{
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

void RectangularPlotWidget::removeTrace(const QString& name)
{
  traces.remove(name);
  updatePlot();
}

void RectangularPlotWidget::clearTraces()
{
  traces.clear();
  updatePlot();
}

QPen RectangularPlotWidget::getTracePen(const QString& traceName) const
{
  if (traces.contains(traceName)) {
    return traces[traceName].pen;
  }

  // Return a default-constructed pen with valid settings
  QPen defaultPen;
  defaultPen.setStyle(Qt::SolidLine);  // Explicit initialization
  defaultPen.setColor(Qt::black);      // Explicit initialization
  return defaultPen;
}


void RectangularPlotWidget::setTracePen(const QString& traceName, const QPen& pen)
{
  if (traces.contains(traceName)) {
    traces[traceName].pen = pen;
    updatePlot();
  }
}

QMap<QString, QPen> RectangularPlotWidget::getTracesInfo() const
{
  QMap<QString, QPen> penMap;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    penMap.insert(it.key(), it.value().pen);
  }
  return penMap;
}

bool RectangularPlotWidget::addMarker(const QString& markerId, double frequency, const QPen& pen)
{
  if (markers.contains(markerId)) {
    return false;
  }

  bool frequencyInRange = false;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const Trace& trace = it.value();
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

bool RectangularPlotWidget::removeMarker(const QString& markerId)
{
  if (!markers.contains(markerId)) {
    return false;
  }

  markers.remove(markerId);
  updatePlot();
  return true;
}

void RectangularPlotWidget::clearMarkers()
{
  markers.clear();
  updatePlot();
}

QMap<QString, double> RectangularPlotWidget::getMarkers() const
{
  QMap<QString, double> markerFrequencies;
  for (auto it = markers.constBegin(); it != markers.constEnd(); ++it) {
    markerFrequencies.insert(it.key(), it.value().frequency);
  }
  return markerFrequencies;
}

void RectangularPlotWidget::updatePlot()
{
  // Clear existing graphics items
  clearGraphicsItems();

  // Get the current scale factor based on selected units
  double freqScale = getXscale();

         // Remove all existing series from the chart
  QList<QAbstractSeries*> oldSeries = ChartWidget->series();
  for (QAbstractSeries* series : oldSeries) {
    ChartWidget->removeSeries(series);
    delete series;
  }

  // Add each trace as a new series
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const QString& name = it.key();
    const Trace& trace = it.value();

           // Create a new line series for the trace
    QLineSeries* series = new QLineSeries();
    series->setPen(trace.pen);
    series->setName(name);

           // Add data points to the series with proper frequency scaling
    for (int i = 0; i < trace.frequencies.size() && i < trace.trace.size(); ++i) {
      // Scale the frequency values according to the current frequency units
      double scaledFreq = trace.frequencies[i] * freqScale;
      series->append(scaledFreq, trace.trace[i]);
    }

    // Add the series to the chart
    ChartWidget->addSeries(series);

    // Attach to the appropriate axes
    series->attachAxis(xAxis);
    if (trace.y_axis == 2) {
      series->attachAxis(y2Axis);
    } else {
      series->attachAxis(yAxis);
    }
  }

  // Draw markers if any
  for (auto it = markers.constBegin(); it != markers.constEnd(); ++it) {
    const Marker& marker = it.value();

           // Create a vertical line series for each marker
    QLineSeries* markerSeries = new QLineSeries();
    markerSeries->setPen(marker.pen);
    markerSeries->setName(marker.id);

           // Find the y-range to cover based on current axes
    double yBottom = yAxis->min();
    double yTop = yAxis->max();

           // Scale the marker frequency according to the current units
    double scaledMarkerFreq = marker.frequency * freqScale;

           // Add a vertical line at the marker frequency
    markerSeries->append(scaledMarkerFreq, yBottom);
    markerSeries->append(scaledMarkerFreq, yTop);

           // Add the marker series to the chart
    ChartWidget->addSeries(markerSeries);
    markerSeries->attachAxis(xAxis);
    markerSeries->attachAxis(yAxis);

           // Create a text label showing just the frequency value
    QString unitText = xAxisUnits->currentText();
    QString freqText = QString::number(scaledMarkerFreq, 'f', 1) + " " + unitText;

    // Create a QGraphicsTextItem for the marker frequency label
    QGraphicsTextItem* markerLabel = new QGraphicsTextItem(ChartWidget);
    markerLabel->setHtml("<div style='text-align:center; background:white; padding:2px; border:1px solid " +
                         marker.pen.color().name() + "'>" + freqText + "</div>");

    // Get the position at the top of the marker line
    QPointF labelPos = ChartWidget->mapToPosition(QPointF(scaledMarkerFreq, yTop), markerSeries);

    // Position slightly above the chart
    labelPos.setY(labelPos.y() - 25); // Move label up by 25 pixels

    // Center the label horizontally on the line
    QRectF labelRect = markerLabel->boundingRect();
    labelPos.setX(labelPos.x() - labelRect.width()/2);

    markerLabel->setPos(labelPos);

    // Add to our tracking list for later cleanup
    markerLabels.append(markerLabel);

    // Add marker intersections with all traces
    for (auto traceIt = traces.constBegin(); traceIt != traces.constEnd(); ++traceIt) {
      const Trace& trace = traceIt.value();

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
          if (trace.frequencies[i] <= marker.frequency && marker.frequency <= trace.frequencies[i+1]) {
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

          // Linear interpolation formula: v = v1 + (f - f1) * (v2 - v1) / (f2 - f1)
          intersectionValue = v1 + (marker.frequency - f1) * (v2 - v1) / (f2 - f1);
          found = true;
        }
      }

      // If intersection was found, add a point marker
      if (found) {
        QScatterSeries* intersectionPoint = new QScatterSeries();
        intersectionPoint->setMarkerSize(7);

        // Use the same color as the trace for the intersection point
        intersectionPoint->setColor(trace.pen.color());
        intersectionPoint->setBorderColor(Qt::black);

        // Get the y-axis for this trace
        QAbstractAxis* traceYAxis = (trace.y_axis == 2) ? y2Axis : yAxis;

        // Add the point to the chart
        intersectionPoint->append(scaledMarkerFreq, intersectionValue);
        ChartWidget->addSeries(intersectionPoint);
        intersectionPoint->attachAxis(xAxis);
        intersectionPoint->attachAxis(traceYAxis);

        // Add value label for the intersection point
        if (showTraceValues) {
          QGraphicsTextItem* valueLabel = new QGraphicsTextItem(ChartWidget);
          QString valueText = QString::number(intersectionValue, 'f', 1);
          valueText += QString(" %1").arg(trace.units); // Attach units

          valueLabel->setHtml("<div style='background:white; padding:1px; border:1px solid " +
                              trace.pen.color().name() + "'>" + valueText + "</div>");


          // Position the label near the intersection point
          QPointF labelPos = ChartWidget->mapToPosition(
              QPointF(scaledMarkerFreq, intersectionValue), intersectionPoint);
          labelPos.setX(labelPos.x() + 5); // Offset slightly to avoid overlapping the point
          valueLabel->setPos(labelPos);

          // Add to our tracking list for later cleanup
          intersectionLabels.append(valueLabel);
        }
      }
    }
  }


  // Draw limits if any
  for (auto it = limits.constBegin(); it != limits.constEnd(); ++it) {
    const Limit& limit = it.value();

    // Create a line series for the limit
    QLineSeries* limitSeries = new QLineSeries();
    limitSeries->setPen(limit.pen);
    limitSeries->setName(it.key());

    // Scale the frequencies according to the current units
    double scaledF1 = limit.f1 * freqScale;
    double scaledF2 = limit.f2 * freqScale;

    // Add the two points defining the limit line
    limitSeries->append(scaledF1, limit.y1);
    limitSeries->append(scaledF2, limit.y2);

    // Add the limit series to the chart
    ChartWidget->addSeries(limitSeries);

    // Attach to the appropriate axes
    limitSeries->attachAxis(xAxis);
    if (limit.y_axis == 1) {
      limitSeries->attachAxis(y2Axis);
    } else {
      limitSeries->attachAxis(yAxis);
    }
  }

  // Refresh the chart
  ChartWidget->update();
}

void RectangularPlotWidget::updateXAxis()
{
  double xMin = xAxisMin->value();
  double xMax = xAxisMax->value();
  double xDiv = xAxisDiv->value();

  double min_step = (xMax-xMin)/10;
  if (xDiv < min_step) {
    // Avoid excessive ticking
    xDiv = min_step;
    xAxisDiv->setValue(xDiv);
  }

         // Set the axis range in display units
  xAxis->setRange(xMin, xMax);
  xAxis->setTickInterval(xDiv);
  xAxis->setTickCount(floor((xMax - xMin) / xDiv) + 1);
  xAxis->setTitleText("frequency (" + xAxisUnits->currentText() + ")");

         // Instead of trying to modify existing series, just redraw everything
  updatePlot();
}

void RectangularPlotWidget::updateYAxis()
{
  double yMin = yAxisMin->value();
  double yMax = yAxisMax->value();
  double yDiv = yAxisDiv->value();

  double min_step = (yMax-yMin)/20;

  if (yDiv < min_step) {
    // Avoid excessive ticking
    yDiv = min_step;
    yAxisDiv->setValue(yDiv);
  }

  yAxis->setRange(yMin, yMax);
  yAxis->setTickInterval(yDiv);
  yAxis->setTickCount(floor((yMax - yMin) / yDiv) + 1);

  updatePlot();
}

void RectangularPlotWidget::updateY2Axis()
{
  double y2Min = y2AxisMin->value();
  double y2Max = y2AxisMax->value();
  double y2Div = y2AxisDiv->value();

  y2Axis->setRange(y2Min, y2Max);
  y2Axis->setTickInterval(y2Div);
  y2Axis->setTickCount(floor((y2Max - y2Min) / y2Div) + 1);

  updatePlot();
}


void RectangularPlotWidget::changeFreqUnits()
{
  // Get the current scale factor based on selected units
  double freqScale = getXscale();

         // Block signals temporarily to avoid triggering updateXAxis() multiple times
  xAxisMin->blockSignals(true);
  xAxisMax->blockSignals(true);

  // Update the spinbox values with scaled values from the global min/max (in Hz)
  xAxisMin->setValue(fMin * freqScale);
  xAxisMax->setValue(fMax * freqScale);

  // Re-enable signals
  xAxisMin->blockSignals(false);
  xAxisMax->blockSignals(false);

  // Update the axis
  updateXAxis();
}

QGridLayout* RectangularPlotWidget::setupAxisSettings()
{
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
  connect(xAxisMin, SIGNAL(valueChanged(double)), this, SLOT(updateXAxis()));
  axisLayout->addWidget(xAxisMin, 0, 1);

  xAxisMax = new QDoubleSpinBox();
  xAxisMax->setMinimum(0.1);
  xAxisMax->setMaximum(1000000);
  xAxisMax->setValue(1000);
  xAxisMax->setDecimals(1);
  xAxisMax->setSingleStep(0.1);
  connect(xAxisMax, SIGNAL(valueChanged(double)), this, SLOT(updateXAxis()));
  axisLayout->addWidget(xAxisMax, 0, 2);

  xAxisDiv = new QDoubleSpinBox();
  xAxisDiv->setMinimum(0.1);
  xAxisDiv->setMaximum(1000000);
  xAxisDiv->setValue(100);
  xAxisDiv->setSingleStep(1);
  connect(xAxisDiv, SIGNAL(valueChanged(double)), this, SLOT(updateXAxis()));
  axisLayout->addWidget(xAxisDiv, 0, 3);

  xAxisUnits = new QComboBox();
  xAxisUnits->addItems(frequencyUnits);
  xAxisUnits->setCurrentIndex(2);
  connect(xAxisUnits, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFreqUnits()));
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
  connect(yAxisMin, SIGNAL(valueChanged(double)), this, SLOT(updateYAxis()));
  axisLayout->addWidget(yAxisMin, 1, 1);

  yAxisMax = new QDoubleSpinBox();
  yAxisMax->setMinimum(-1000000);
  yAxisMax->setMaximum(1000000);
  yAxisMax->setValue(10);
  yAxisMax->setDecimals(1);
  yAxisMax->setSingleStep(1);
  connect(yAxisMax, SIGNAL(valueChanged(double)), this, SLOT(updateYAxis()));
  axisLayout->addWidget(yAxisMax, 1, 2);

  yAxisDiv = new QDoubleSpinBox();
  yAxisDiv->setMinimum(0.1);
  yAxisDiv->setMaximum(1000000);
  yAxisDiv->setValue(5);
  yAxisDiv->setSingleStep(1);
  connect(yAxisDiv, SIGNAL(valueChanged(double)), this, SLOT(updateYAxis()));
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
  connect(y2AxisMin, SIGNAL(valueChanged(double)), this, SLOT(updateY2Axis()));
  axisLayout->addWidget(y2AxisMin, 2, 1);

  y2AxisMax = new QDoubleSpinBox();
  y2AxisMax->setMinimum(-1000000);
  y2AxisMax->setMaximum(1000000);
  y2AxisMax->setValue(180);
  y2AxisMax->setDecimals(1);
  y2AxisMax->setSingleStep(10);
  connect(y2AxisMax, SIGNAL(valueChanged(double)), this, SLOT(updateY2Axis()));
  axisLayout->addWidget(y2AxisMax, 2, 2);

  y2AxisDiv = new QDoubleSpinBox();
  y2AxisDiv->setMinimum(0.1);
  y2AxisDiv->setMaximum(1000000);
  y2AxisDiv->setValue(45);
  y2AxisDiv->setSingleStep(5);
  connect(y2AxisDiv, SIGNAL(valueChanged(double)), this, SLOT(updateY2Axis()));
  axisLayout->addWidget(y2AxisDiv, 2, 3);

  y2AxisUnits = new QLabel("deg");
  axisLayout->addWidget(y2AxisUnits, 2, 4);

         // Add the Show Values checkbox in a new row
  QLabel *markerOptionsLabel = new QLabel("<b>Marker Options</b>");
  axisLayout->addWidget(markerOptionsLabel, 3, 0);

  showValuesCheckbox = new QCheckBox("Show Data Values");
  showValuesCheckbox->setChecked(true);  // Default to showing values
  connect(showValuesCheckbox, SIGNAL(toggled(bool)), this, SLOT(toggleShowValues(bool)));
  axisLayout->addWidget(showValuesCheckbox, 3, 1, 1, 2);  // Span 2 columns

  // Lock Axis Settings checkbox
  lockAxisCheckbox = new QCheckBox("Lock Axis Settings");
  lockAxisCheckbox->setChecked(false);  // Default to unlocked
  connect(lockAxisCheckbox, SIGNAL(toggled(bool)), this, SLOT(toggleLockAxisSettings(bool)));
  axisLayout->addWidget(lockAxisCheckbox, 3, 3, 1, 2);  // Span 2 columns

  return axisLayout;
}

// These "get" functions are used by the main program to put markers and limits
double RectangularPlotWidget::getYmin(){
  return yAxisMin->value();
}

double RectangularPlotWidget::getYmax(){
  return yAxisMax->value();
}

double RectangularPlotWidget::getYdiv(){
  return yAxisDiv->value();
}

void RectangularPlotWidget::setYdiv(double val){
  yAxisDiv->setValue(val);
}

void RectangularPlotWidget::setYmin(double val){
  yAxisMin->setValue(val);
}

void RectangularPlotWidget::setYmax(double val){
  yAxisMax->setValue(val);
}

double RectangularPlotWidget::getY2min(){
  return y2AxisMin->value();
}

double RectangularPlotWidget::getY2max(){
  return y2AxisMax->value();
}

double RectangularPlotWidget::getY2div(){
  return y2AxisDiv->value();
}

double RectangularPlotWidget::getXmin(){
  return xAxisMin->value();
}

double RectangularPlotWidget::getXmax(){
  return xAxisMax->value();
}

double RectangularPlotWidget::getXdiv(){
  return xAxisDiv->value();
}

QString RectangularPlotWidget::getXunits(){
  return xAxisUnits->currentText();
}


// Count traces assigned to the primary y-axis
int RectangularPlotWidget::getYAxisTraceCount() const
{
  int count = 0;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    if (it.value().y_axis != 2) {
      count++;
    }
  }
  return count;
}

// Count traces assigned to the secondary y-axis
int RectangularPlotWidget::getY2AxisTraceCount() const
{
  int count = 0;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    if (it.value().y_axis == 2) {
      count++;
    }
  }
  return count;
}


// Returns a scale factor depending on the selection of the frequency units
double RectangularPlotWidget::getXscale(){
  QString unit = xAxisUnits->currentText();
  double freqScale = 1;
  if (unit == "kHz") {
    freqScale = 1e-3;  // Hz to kHz
  } else if (unit == "MHz") {
    freqScale = 1e-6;  // Hz to MHz
  } else if (unit == "GHz") {
    freqScale = 1e-9;  // Hz to GHz
  }
  return freqScale;
}

// Gets the index of the frequency units combobox
int RectangularPlotWidget::getFreqIndex(){
  return xAxisUnits->currentIndex();
}


bool RectangularPlotWidget::updateMarkerFrequency(const QString& markerId, double newFrequency) {
  // Check if marker exists
  if (!markers.contains(markerId)) {
    return false;
  }

         // Check if any trace contains this frequency
  bool frequencyInRange = false;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const Trace& trace = it.value();
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

// Removes the old graphic elements, such as the labels from past marker positions
void RectangularPlotWidget::clearGraphicsItems()
{
  // Remove all marker labels
  for (QGraphicsTextItem* label : markerLabels) {
    ChartWidget->scene()->removeItem(label);
    delete label;
  }
  markerLabels.clear();

  // Remove all intersection labels
  for (QGraphicsTextItem* label : intersectionLabels) {
    ChartWidget->scene()->removeItem(label);
    delete label;
  }
  intersectionLabels.clear();
}


void RectangularPlotWidget::toggleShowValues(bool show)
{
  showTraceValues = show;
  updatePlot();  // Redraw with new setting
}


bool RectangularPlotWidget::addLimit(const QString& limitId, const Limit& limit)
{
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


void RectangularPlotWidget::removeLimit(const QString& limitId)
{
  // Remove the limit if it exists
  if (limits.contains(limitId)) {
    limits.remove(limitId);
    // Update the plot to reflect the removal
    updatePlot();
  }
}

void RectangularPlotWidget::clearLimits()
{
  limits.clear();
  updatePlot();
}


QMap<QString, RectangularPlotWidget::Limit> RectangularPlotWidget::getLimits() const
{
  return limits;
}


// Update limit given the name
bool RectangularPlotWidget::updateLimit(const QString& limitId, const Limit& limit)
{
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

void RectangularPlotWidget::change_Y_axis_title(QString title) {
  yAxis->setTitleText(title);
}

void RectangularPlotWidget::change_Y_axis_units(QString units) {
  yAxisUnits->setText(units);
}

void RectangularPlotWidget::change_Y2_axis_title(QString title) {
  y2Axis->setTitleText(title);
}

void RectangularPlotWidget::change_Y2_axis_units(QString units) {
  y2AxisUnits->setText(units);
}

void RectangularPlotWidget::change_X_axis_title(QString title) {
  xAxis->setTitleText(title);
}


void RectangularPlotWidget::setRightYAxisEnabled(bool enabled)
{
  // Hide or show the right y-axis
  y2Axis->setVisible(enabled);

         // Hide/show the y2-axis label and related controls
  y2AxisLabel->setVisible(enabled);
  y2AxisMin->setVisible(enabled);
  y2AxisMax->setVisible(enabled);
  y2AxisDiv->setVisible(enabled);
  y2AxisUnits->setVisible(enabled);

         // If disabling, remove all traces associated with the right y-axis
  if (!enabled) {
    QList<QAbstractSeries*> seriesToRemove;
    for (QAbstractSeries* series : ChartWidget->series()) {
      // Check if the series is attached to the right y-axis
      if (series->attachedAxes().contains(y2Axis)) {
        seriesToRemove.append(series);
      }
    }

           // Remove identified series
    for (QAbstractSeries* series : seriesToRemove) {
      ChartWidget->removeSeries(series);
      delete series;
    }
  }

         // Redraw the plot to reflect changes
  updatePlot();
}

bool RectangularPlotWidget::isRightYAxisEnabled() const
{
  return y2Axis->isVisible();
}


// Handle lock axis checkbox
void RectangularPlotWidget::toggleLockAxisSettings(bool locked)
{
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
  for (QObject* child : children()) {
    QLabel* label = qobject_cast<QLabel*>(child);
    if (label && label->text() == "<b>y-axis</b>") {
      label->setPalette(pal);
      break;
    }
  }
}


// Public function to let check whether the axes are locked or not
bool RectangularPlotWidget::areAxisSettingsLocked() const
{
  return axisSettingsLocked;
}


void RectangularPlotWidget::set_y_autoscale(bool value){
  y_autoscale = value;
}


// Send settings to the main program
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

// Get settings from the main program
void RectangularPlotWidget::setSettings(const AxisSettings& settings) {
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
