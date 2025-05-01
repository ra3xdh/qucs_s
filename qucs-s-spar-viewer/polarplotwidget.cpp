#include "polarplotwidget.h"

PolarPlotWidget::PolarPlotWidget(QWidget *parent)
    : QWidget(parent), fMin(1e20), fMax(-1)
{
  // Initialize the chart and chart view
  polarChart = new QPolarChart();
  polarChart->legend()->hide();
  chartView = new QChartView(polarChart, this);
  chartView->setRenderHint(QPainter::Antialiasing);

         // Initialize axes
  radiusAxis = new QValueAxis();
  angleAxis = new QCategoryAxis();

         // Set up radius axis (the concentric circles)
  polarChart->addAxis(radiusAxis, QPolarChart::PolarOrientationRadial);
  radiusAxis->setTickCount(6);
  radiusAxis->setLabelFormat("%.1f");
  radiusAxis->setRange(0, 1.0);

         // Set up angle axis (the angle lines)
  polarChart->addAxis(angleAxis, QPolarChart::PolarOrientationAngular);
  // Add categories for each 45 degrees
  angleAxis->append("0°", 0);
  angleAxis->append("45°", 45);
  angleAxis->append("90°", 90);
  angleAxis->append("135°", 135);
  angleAxis->append("180°", 180);
  angleAxis->append("225°", 225);
  angleAxis->append("270°", 270);
  angleAxis->append("315°", 315);
  angleAxis->setRange(0, 360);
  angleAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

         // Set up the frequency units
  frequencyUnits << "Hz" << "kHz" << "MHz" << "GHz";

         // Initialize marker and value label lists
  markerLabels.clear();
  valueLabels.clear();

         // Create the main layout
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(chartView);

         // Add the axis settings layout
  mainLayout->addLayout(setupAxisSettings());
  setLayout(mainLayout);
}

PolarPlotWidget::~PolarPlotWidget()
{
  // Clean up any remaining graphics items
  clearGraphicsItems();

         // Delete the chart (which will delete all series)
  delete polarChart;
}

void PolarPlotWidget::addTrace(const QString& name, const Trace& trace) {
  traces[name] = trace;
  updateFrequencyRange(); // Update frequency range based on new trace
  updatePlot();
}

void PolarPlotWidget::updateFrequencyRange() {
  fMin = 1e20;
  fMax = -1;

  for (const auto& trace : traces) {
    if (!trace.frequencies.isEmpty()) {
      double traceMinFreq = trace.frequencies.first();
      double traceMaxFreq = trace.frequencies.last();

      if (traceMinFreq < fMin) fMin = traceMinFreq;
            if (traceMaxFreq > fMax) fMax = traceMaxFreq;
        }
  }

         // Update spin box values
  double freqMultiplier = getFrequencyMultiplier();
  fMinSpinBox->setValue(fMin / freqMultiplier);
  fMinSpinBox->setMinimum(fMin / freqMultiplier);

  fMaxSpinBox->setValue(fMax / freqMultiplier);
  fMaxSpinBox->setMaximum(fMax / freqMultiplier);
}

void PolarPlotWidget::removeTrace(const QString& name)
{
  traces.remove(name);
  updatePlot();
}

void PolarPlotWidget::clearTraces()
{
  traces.clear();
  updatePlot();
}

QPen PolarPlotWidget::getTracePen(const QString& traceName) const
{
  if (traces.contains(traceName)) {
    return traces[traceName].pen;
  }
  return QPen();
}

void PolarPlotWidget::setTracePen(const QString& traceName, const QPen& pen)
{
  if (traces.contains(traceName)) {
    traces[traceName].pen = pen;
    updatePlot();
  }
}

QMap<QString, QPen> PolarPlotWidget::getTracesInfo() const
{
  QMap<QString, QPen> penMap;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    penMap.insert(it.key(), it.value().pen);
  }
  return penMap;
}

bool PolarPlotWidget::addMarker(const QString& markerId, double frequency, const QPen& pen)
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

bool PolarPlotWidget::removeMarker(const QString& markerId)
{
  if (!markers.contains(markerId)) {
    return false;
  }

  markers.remove(markerId);
  updatePlot();
  return true;
}

void PolarPlotWidget::clearMarkers()
{
  markers.clear();
  updatePlot();
}

QMap<QString, double> PolarPlotWidget::getMarkers() const
{
  QMap<QString, double> markerFrequencies;
  for (auto it = markers.constBegin(); it != markers.constEnd(); ++it) {
    markerFrequencies.insert(it.key(), it.value().frequency);
  }
  return markerFrequencies;
}

void PolarPlotWidget::updatePlot() {
  clearGraphicsItems();

  QList<QAbstractSeries *> oldSeries = polarChart->series();
  for (QAbstractSeries *series : oldSeries) {
    polarChart->removeSeries(series);
    delete series;
  }

  const double PHASE_WRAP_THRESHOLD = 180.0;  // Degrees

  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const QString &name = it.key();
    const Trace &trace = it.value();

    QLineSeries *currentSeries = new QLineSeries();
    currentSeries->setPen(trace.pen);
    currentSeries->setName(name);

    double prevPhase = -1e3;  // Initialize with impossible value

    for (int i = 0; i < trace.values.size() && i < trace.frequencies.size(); ++i) {
      double frequency = trace.frequencies[i];
      if (frequency >= fMin && frequency <= fMax) {
        std::complex value = trace.values[i];
        double magnitude = std::abs(value);
        double phase = std::arg(value) * 180.0 / M_PI;
        if (phase < 0) phase += 360;

        // Check for phase wrap (only after first point)
        if (prevPhase != -1e3 && std::abs(phase - prevPhase) > PHASE_WRAP_THRESHOLD) {
          // Finalize current series
          polarChart->addSeries(currentSeries);
          currentSeries->attachAxis(angleAxis);
          currentSeries->attachAxis(radiusAxis);

          // Start new series
          currentSeries = new QLineSeries();
          currentSeries->setPen(trace.pen);
        }

        currentSeries->append(phase, magnitude);
        prevPhase = phase;

       // qDebug() << frequency << ": " << magnitude << "  " << phase;
      }
    }

    // Add the final series for this trace
    polarChart->addSeries(currentSeries);
    currentSeries->attachAxis(angleAxis);
    currentSeries->attachAxis(radiusAxis);
  }

  drawCustomMarkers();
  polarChart->update();
}

void PolarPlotWidget::updateRAxis()
{
  double rMin = rAxisMin->value();
  double rMax = rAxisMax->value();
  double rDiv = rAxisDiv->value();

  radiusAxis->setRange(rMin, rMax);
  radiusAxis->setTickInterval(rDiv);
  radiusAxis->setTickCount(floor((rMax - rMin) / rDiv) + 1);

  updatePlot();
}

void PolarPlotWidget::updateAngleAxis()
{
  // For angle axis updates - typically fixed at 360 degrees
  updatePlot();
}

void PolarPlotWidget::toggleDisplayMode()
{
  updatePlot();
}

bool PolarPlotWidget::updateMarkerFrequency(const QString& markerId, double newFrequency)
{
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
    return false;
  }

         // Update the marker's frequency
  markers[markerId].frequency = newFrequency;

         // Trigger repaint
  updatePlot();
  return true;
}

void PolarPlotWidget::clearGraphicsItems()
{
  // Remove all marker labels
  for (QGraphicsTextItem* label : markerLabels) {
    polarChart->scene()->removeItem(label);
    delete label;
  }
  markerLabels.clear();

  // Remove all value labels
  for (QGraphicsTextItem* label : valueLabels) {
    polarChart->scene()->removeItem(label);
    delete label;
  }
  valueLabels.clear();

  // Remove all marker items
  for (QGraphicsEllipseItem* item : markerItems) {
    polarChart->scene()->removeItem(item);
    delete item;
  }
  markerItems.clear();
}

QGridLayout* PolarPlotWidget::setupAxisSettings()
{
  QGridLayout *axisLayout = new QGridLayout();

  fMin = 1e20;
  fMax = -1;

  QLabel *FreqLabel = new QLabel("<b>Frequency</b>");
  axisLayout->addWidget(FreqLabel, 0, 0);

  // Create frequency controls
  fMinSpinBox = new QDoubleSpinBox;
  fMinSpinBox->setRange(0, 1e12);
  fMinSpinBox->setSingleStep(10);
  connect(fMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFMinChanged(double)));
  axisLayout->addWidget(fMinSpinBox, 0, 1);

  fMaxSpinBox = new QDoubleSpinBox;
  fMaxSpinBox->setRange(0, 1e12);
  fMaxSpinBox->setSingleStep(10);
  connect(fMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFMaxChanged(double)));
  axisLayout->addWidget(fMaxSpinBox, 0, 2);

  fUnitComboBox = new QComboBox;
  fUnitComboBox->addItems(frequencyUnits);
  fUnitComboBox->setCurrentIndex(2); // MHz
  connect(fUnitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFUnitChanged()));
  axisLayout->addWidget(fUnitComboBox, 0, 3);

         // Radius axis settings
  QLabel *rAxisLabel = new QLabel("<b>Radius</b>");
  axisLayout->addWidget(rAxisLabel, 1, 0);

  rAxisMin = new QDoubleSpinBox();
  rAxisMin->setMinimum(0.0);
  rAxisMin->setMaximum(1000000);
  rAxisMin->setValue(0.0);
  rAxisMin->setDecimals(2);
  rAxisMin->setSingleStep(0.1);
  connect(rAxisMin, SIGNAL(valueChanged(double)), this, SLOT(updateRAxis()));
  axisLayout->addWidget(rAxisMin, 1, 1);

  rAxisMax = new QDoubleSpinBox();
  rAxisMax->setMinimum(0.1);
  rAxisMax->setMaximum(1000000);
  rAxisMax->setValue(1.0);
  rAxisMax->setDecimals(2);
  rAxisMax->setSingleStep(0.1);
  connect(rAxisMax, SIGNAL(valueChanged(double)), this, SLOT(updateRAxis()));
  axisLayout->addWidget(rAxisMax, 1, 2);

  rAxisDiv = new QDoubleSpinBox();
  rAxisDiv->setMinimum(0.1);
  rAxisDiv->setMaximum(1000000);
  rAxisDiv->setValue(0.2);
  rAxisDiv->setDecimals(2);
  rAxisDiv->setSingleStep(0.1);
  connect(rAxisDiv, SIGNAL(valueChanged(double)), this, SLOT(updateRAxis()));
  axisLayout->addWidget(rAxisDiv, 1, 3);

         // Display mode
  QLabel *modeLabel = new QLabel("<b>Marker format</b>");
  axisLayout->addWidget(modeLabel, 2, 0);

  displayModeCombo = new QComboBox();
  displayModeCombo->addItem("Magnitude/Phase");
  displayModeCombo->addItem("Real/Imaginary");
  connect(displayModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(toggleDisplayMode()));
  axisLayout->addWidget(displayModeCombo, 2, 1, 1, 2);

  return axisLayout;
}

double PolarPlotWidget::getRmax()
{
  return rAxisMax->value();
}

double PolarPlotWidget::getRmin()
{
  return rAxisMin->value();
}

double PolarPlotWidget::getRdiv()
{
  return rAxisDiv->value();
}

int PolarPlotWidget::getDisplayMode() const
{
  return displayModeCombo->currentIndex();
}

std::complex<double> PolarPlotWidget::getComplexValueAtFrequency(const Trace& trace, double frequency)
{
  // Find the closest frequency points in the trace
  int lowerIndex = -1;
  for (int i = 0; i < trace.frequencies.size() - 1; ++i) {
    if (trace.frequencies[i] <= frequency && frequency <= trace.frequencies[i+1]) {
      lowerIndex = i;
      break;
    }
  }

         // If we found an interval containing the marker frequency
  if (lowerIndex >= 0) {
    // Linear interpolation to find the complex value at marker frequency
    double f1 = trace.frequencies[lowerIndex];
    double f2 = trace.frequencies[lowerIndex + 1];
    std::complex<double> v1 = trace.values[lowerIndex];
    std::complex<double> v2 = trace.values[lowerIndex + 1];

           // Linear interpolation formula applied to both real and imaginary parts
    double t = (frequency - f1) / (f2 - f1);
    double real = v1.real() + t * (v2.real() - v1.real());
    double imag = v1.imag() + t * (v2.imag() - v1.imag());

    return std::complex<double>(real, imag);
  }

         // If frequency is exactly at the first or last point
  if (std::abs(frequency - trace.frequencies.first()) < 1e-10 && !trace.values.isEmpty()) {
    return trace.values.first();
  }
  if (std::abs(frequency - trace.frequencies.last()) < 1e-10 && !trace.values.isEmpty()) {
    return trace.values.last();
  }

         // Return zero if not found (shouldn't happen if checks are proper)
  return std::complex<double>(0, 0);
}

std::complex<double> PolarPlotWidget::convertToDisplayFormat(const std::complex<double>& value, int mode)
{
  if (mode == 0) {
    // Magnitude/Phase - already handled in plotting function
    return value;
  } else {
    // Real/Imaginary - also handled in plotting function
    return value;
  }
}


void PolarPlotWidget::drawCustomMarkers()
{
  if (markers.isEmpty() || traces.isEmpty()) {
    return;
  }

  // Clear existing marker graphics items
  clearGraphicsItems();

  // Font configuration for marker labels
  QFont markerFont;
  markerFont.setBold(true);

  // Iterate through each trace
  for (auto traceIt = traces.constBegin(); traceIt != traces.constEnd(); ++traceIt) {
    const QString& traceName = traceIt.key();
    const Trace& trace = traceIt.value();

    // Skip traces with no frequency data
    if (trace.frequencies.isEmpty() || trace.values.isEmpty()) {
      continue;
    }

    // Draw markers for this trace
    for (auto markerIt = markers.constBegin(); markerIt != markers.constEnd(); ++markerIt) {
      const QString& markerId = markerIt.key();
      const Marker& marker = markerIt.value();
      double markerFreq = marker.frequency;

      // Check if marker frequency is within the trace frequency range
      if (markerFreq < trace.frequencies.first() || markerFreq > trace.frequencies.last()) {
        continue;
      }

      // Get interpolated complex value at marker frequency
      std::complex<double> value = getComplexValueAtFrequency(trace, markerFreq);

      // Convert to display format based on current mode
      double angle, radius;
      int displayMode = displayModeCombo->currentIndex();

      if (displayMode == 0) {
        // Magnitude/Phase mode
        radius = std::abs(value);
        angle = std::arg(value) * 180.0 / M_PI;
        if (angle < 0) angle += 360;
        } else {
        // Real/Imaginary mode
        angle = std::atan2(value.imag(), value.real()) * 180.0 / M_PI;
        if (angle < 0) angle += 360;
                radius = std::sqrt(value.real()*value.real() + value.imag()*value.imag());
      }

      // Create a marker point (custom drawn circle)
      QGraphicsEllipseItem* markerPoint = new QGraphicsEllipseItem(polarChart);
      QPointF pointPos = polarChart->mapToPosition(QPointF(angle, radius));
      markerPoint->setRect(pointPos.x() - 4, pointPos.y() - 4, 8, 8);
      markerPoint->setPen(marker.pen);
      markerPoint->setBrush(marker.pen.color());

      // Add to tracking list for cleanup
      markerItems.append(markerPoint);

      // Determine frequency unit and scaling
      QString freqUnit = "Hz";
      double freqValue = markerFreq;
      if (markerFreq >= 1e9) {
        freqUnit = "GHz";
        freqValue = markerFreq / 1e9;
      } else if (markerFreq >= 1e6) {
        freqUnit = "MHz";
        freqValue = markerFreq / 1e6;
      } else if (markerFreq >= 1e3) {
        freqUnit = "kHz";
        freqValue = markerFreq / 1e3;
      }

      // Create label with marker ID, value, and frequency
      QString label;
      if (displayMode == 0) {
        // Magnitude/Phase format
        label = QString("%1 [%2]: %3 %4\n%5∠%6°")
                    .arg(markerId)
                    .arg(traceName)
                    .arg(freqValue, 0, 'g', 3)
                    .arg(freqUnit)
                    .arg(radius, 0, 'f', 2)
                    .arg(angle, 0, 'f', 2);
      } else {
        // Real/Imaginary format
        label = QString("%1 [%2]: %3 %4\n%5%6j%7")
                    .arg(markerId)
                    .arg(traceName)
                    .arg(freqValue, 0, 'g', 3)
                    .arg(freqUnit)
                    .arg(value.real(), 0, 'f', 2)
                    .arg(value.imag() >= 0 ? "+" : "")
                    .arg(value.imag(), 0, 'f', 2);
      }

      // Create and position the label
      QGraphicsTextItem* markerLabel = new QGraphicsTextItem(polarChart);
      markerLabel->setFont(markerFont);

      // Set HTML formatted text with background similar to SmithChartWidget
      markerLabel->setHtml(
          QString("<div style='background-color: rgba(255, 255, 255, 200); "
                  "padding: 6px; border: 1px solid black;'>%1</div>")
              .arg(label.replace("\n", "<br>"))
          );

      // Position the label relative to the marker point
      QPointF labelPos = QPointF(pointPos.x() + 8, pointPos.y() - markerLabel->boundingRect().height() / 2);
      markerLabel->setPos(labelPos);

      // Add to tracking list
      markerLabels.append(markerLabel);
    }
  }
}


void PolarPlotWidget::onFMinChanged(double value) {
  fMin = value * getFrequencyMultiplier();
  updatePlot();
}

void PolarPlotWidget::onFMaxChanged(double value) {
  fMax = value * getFrequencyMultiplier();
  updatePlot();
}

void PolarPlotWidget::onFUnitChanged() {
  fMin = fMinSpinBox->value() * getFrequencyMultiplier();
  fMax = fMaxSpinBox->value() * getFrequencyMultiplier();
  updatePlot();
}

double PolarPlotWidget::getFrequencyMultiplier() const {
  int index = fUnitComboBox->currentIndex();
  switch (index) {
  case 0: return 1.0;   // Hz
  case 1: return 1e3;  // kHz
  case 2: return 1e6;  // MHz
  case 3: return 1e9;  // GHz
  default: return 1.0;
  }
}

