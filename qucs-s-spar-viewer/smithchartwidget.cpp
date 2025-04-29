/*
 * SmithChartWidget - A Qt widget for displaying Smith charts
 *
 * Copyright (C) 2025 Andrés Martínez Mera
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Created with assistance from Claude AI (Anthropic, 2025)
 */

#include "smithchartwidget.h"
#include <QDebug>
#include <QToolTip>

SmithChartWidget::SmithChartWidget(QWidget *parent)
    : QWidget(parent), z0(50.0), scaleFactor(1.0), panX(0.0), panY(0.0), m_showAdmittanceChart(false)
{
  // Default characteristic impedance
  setAttribute(Qt::WA_Hover);
  setMouseTracking(true);

         // Set white background
  setAutoFillBackground(true);
  QPalette pal = palette();
  pal.setColor(QPalette::Window, Qt::white);
  setPalette(pal);

         // Create a QVBoxLayout for controls at the top
  QVBoxLayout *controlsLayout = new QVBoxLayout();
  controlsLayout->setContentsMargins(5, 5, 5, 5);
  controlsLayout->setSpacing(5);

         // Create a horizontal layout for the Z0 selector
  QHBoxLayout *z0Layout = new QHBoxLayout();
  z0Layout->setContentsMargins(0, 0, 0, 0); // Small margins
  z0Layout->setAlignment(Qt::AlignLeft); // Align to left

         // Create the label
  QLabel *z0Label = new QLabel("Z<sub>0</sub>", this);

         // Create the Z0 combo box
  m_Z0ComboBox = new QComboBox(this);
  m_Z0ComboBox->addItem("50 Ω", 50.0);
  m_Z0ComboBox->addItem("75 Ω", 75.0);

         // Make the combo box smaller
  m_Z0ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_Z0ComboBox->setFixedWidth(80); // Set a fixed width

         // Set default value to 50 Ohm
  m_Z0ComboBox->setCurrentIndex(0);
  z0 = 50.0;

         // Add widgets to the Z0 layout
  z0Layout->addWidget(z0Label);
  z0Layout->addWidget(m_Z0ComboBox);
  z0Layout->addStretch(); // This pushes everything to the left

         // Add horizontal layout to controls layout
  controlsLayout->addLayout(z0Layout);

         // Create horizontal layout for checkboxes
  QHBoxLayout *checkboxLayout = new QHBoxLayout();
  checkboxLayout->setContentsMargins(0, 0, 0, 0);

         // Create checkbox for admittance chart
  m_ShowAdmittanceChartCheckBox = new QCheckBox("Y Chart", this);
  m_ShowAdmittanceChartCheckBox->setChecked(false);

         // Create checkbox for constant curves display
  m_ShowConstantCurvesCheckBox = new QCheckBox("Z Chart", this);
  m_ShowConstantCurvesCheckBox->setChecked(true); // Checked by default
  m_showConstantCurves = true; // Initialize to true

         // Add checkboxes to their layout
  checkboxLayout->addWidget(m_ShowAdmittanceChartCheckBox);
  checkboxLayout->addWidget(m_ShowConstantCurvesCheckBox);
  checkboxLayout->addStretch();

         // Add checkbox layout to controls layout
  controlsLayout->addLayout(checkboxLayout);

         // Create the main layout for the widget
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0); // Minimize margins for more chart space

  // Add the controls layout at the top
  mainLayout->addLayout(controlsLayout);

  // Add a widget to contain the chart (this is where your drawing will go)
  QWidget *chartWidget = new QWidget(this);
  chartWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(chartWidget, 1); // Give it a stretch factor of 1 to take available space

  // Create frequency range controls layout
  QGridLayout *freqRangeLayout = new QGridLayout();
  freqRangeLayout->setContentsMargins(5, 5, 5, 5);

  // Min frequency label and spinbox
  QLabel *minFreqLabel = new QLabel("Min:", this);
  m_minFreqSpinBox = new QDoubleSpinBox(this);
  m_minFreqSpinBox->setDecimals(1);
  m_minFreqSpinBox->setRange(0.0, 1000.0);
  m_minFreqSpinBox->setValue(0.0);
  m_minFreqSpinBox->setSingleStep(1);
  m_minFreqSpinBox->setMaximumWidth(70);
  connect(m_minFreqSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &SmithChartWidget::onMinFreqChanged);

  // Max frequency label and spinbox
  QLabel *maxFreqLabel = new QLabel("Max:", this);
  m_maxFreqSpinBox = new QDoubleSpinBox(this);
  m_maxFreqSpinBox->setDecimals(1);
  m_maxFreqSpinBox->setRange(0.1, 1000.0);
  m_maxFreqSpinBox->setValue(20.0);
  m_maxFreqSpinBox->setSingleStep(1);
  m_maxFreqSpinBox->setMaximumWidth(70);
  connect(m_maxFreqSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          this, &SmithChartWidget::onMaxFreqChanged);

  // Min frequency label and spinbox
  QLabel *FreqUnitsLabel = new QLabel("Units:", this);

  // Frequency unit combobox
  m_freqUnitComboBox = new QComboBox(this);
  m_freqUnitComboBox->addItem("Hz");
  m_freqUnitComboBox->addItem("kHz");
  m_freqUnitComboBox->addItem("MHz");
  m_freqUnitComboBox->addItem("GHz");
  m_freqUnitComboBox->setCurrentIndex(2); // Default to MHz
  connect(m_freqUnitComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &SmithChartWidget::onFreqUnitChanged);

  // Add widgets to the frequency range layout
  freqRangeLayout->addWidget(minFreqLabel, 0, 0);
  freqRangeLayout->addWidget(m_minFreqSpinBox, 0, 1);
  freqRangeLayout->addWidget(maxFreqLabel, 1, 0);
  freqRangeLayout->addWidget(m_maxFreqSpinBox, 1, 1);
  freqRangeLayout->addWidget(FreqUnitsLabel, 2, 0);
  freqRangeLayout->addWidget(m_freqUnitComboBox, 2, 1);

  // Create a horizontal layout for the bottom section
  QHBoxLayout *bottomLayout = new QHBoxLayout();

  // Add the frequency range controls to the bottom layout (left-aligned)
  QWidget *freqWidget = new QWidget(this);
  freqWidget->setLayout(freqRangeLayout);
  bottomLayout->addWidget(freqWidget);

  // Add a spacer to push everything to the left
  bottomLayout->addStretch(1);

  // Add the bottom layout to the main layout
  mainLayout->addLayout(bottomLayout);

         // Connect the signals
  connect(m_Z0ComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &SmithChartWidget::onZ0Changed);
  connect(m_ShowAdmittanceChartCheckBox, &QCheckBox::stateChanged,
          this, &SmithChartWidget::onShowAdmittanceChartChanged);
  connect(m_ShowConstantCurvesCheckBox, &QCheckBox::stateChanged,
          this, &SmithChartWidget::onShowConstantCurvesChanged);

  setLayout(mainLayout);
}

SmithChartWidget::~SmithChartWidget()
{
}

void SmithChartWidget::onZ0Changed(int index)
{
  // Get the selected Z0 value from the combo box
  z0 = m_Z0ComboBox->itemData(index).toDouble();

  // Update the chart
  update();
}

void SmithChartWidget::addTrace(const QString& name, const Trace& trace)
{
  traces[name] = trace;

         // Check if this trace's Z0 is already in the combo box
  bool found = false;
  for (int i = 0; i < m_Z0ComboBox->count(); i++) {
    if (qFuzzyCompare(m_Z0ComboBox->itemData(i).toDouble(), trace.Z0)) {
      found = true;
      break;
    }
  }

         // If not found, add it as an option
  if (!found) {
    QString itemText = QString("%1 Ω").arg(trace.Z0);
    m_Z0ComboBox->addItem(itemText, trace.Z0);
  }

  // Update frequency range based on new trace data
  updateFrequencyRange();

  update(); // Trigger a repaint
}

void SmithChartWidget::setCharacteristicImpedance(double z)
{
  z0 = z;
  update(); // Redraw the chart with the new Z0
}

void SmithChartWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

         //Save transformation matrix
  painter.save();

         // Apply zoom and pan transformations
  painter.translate(width() / 2.0 + panX, height() / 2.0 + panY);
  painter.scale(scaleFactor, scaleFactor);
  painter.translate(-width() / 2.0, -height() / 2.0);

         // 1. Draw the Smith Chart grid (circles and arcs)
  drawSmithChartGrid(&painter);

         // 2. Plot the impedance data
  plotImpedanceData(&painter);

  // 3. Draw markers
  drawMarkers(&painter);

         //Restore transformation matrix
  painter.restore();
}


void SmithChartWidget::mousePressEvent(QMouseEvent *event) {
  lastMousePos = event->pos();

  std::complex<double> reflectionCoefficient = widgetToSmithChart(lastMousePos);

         // Iterate through all traces
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const Trace& trace = it.value();
    std::complex<double> normalizedImpedance = (1.0 + reflectionCoefficient) / (1.0 - reflectionCoefficient);
    std::complex<double> impedance = normalizedImpedance * trace.Z0;
    emit impedanceSelected(impedance);
  }
}

void SmithChartWidget::drawSmithChartGrid(QPainter *painter) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

           // Calculate the center and radius
    QPointF center(width() / 2.0, height() / 2.0);
    double radius = qMin(width(), height()) / 2.0 - 10;

           // Draw the outer circle (|Γ| = 1)
    painter->setPen(QPen(Qt::black, 2));
    painter->drawEllipse(center, radius, radius);

           // Draw the real axis
    painter->drawLine(center - QPointF(radius, 0), center + QPointF(radius, 0));

           // Draw constant resistance circles - use the current z0 for labels
    if (m_showConstantCurves) {
      QVector<double> resistances = {0.2, 0.5, 1.0, 2.0, 5.0};
      painter->setPen(QPen(Qt::gray, 1));
      for (double r : resistances) {
        double x = radius * r / (1 + r);
        double y = radius / (1 + r);
        QPointF circleCenter(center.x() + x, center.y());
        painter->drawEllipse(circleCenter, y, y);

               //Paint label with actual impedance value based on Z0
        QPointF label_position(center.x() + x - y, center.y()-5);
        painter->setPen(QPen(Qt::black, 2));
        painter->drawText(label_position, QString::number(r * z0, 'f', 1));
        painter->setPen(QPen(Qt::gray, 1));
      }

             // Draw constant reactance arcs - use the current z0 for labels
      QVector<double> reactances = {0.2, 0.5, 1.0, 2.0, 5.0};
      for (double x : reactances) {
        drawReactanceArc(painter, center, radius, x);
        drawReactanceArc(painter, center, radius, -x);
      }
    }

         // Draw the admittance chart if enabled
  if (m_showAdmittanceChart) {
    // Use red pen for admittance chart
    QPen admittancePen(Qt::red, 1);
    painter->setPen(admittancePen);

           // Draw constant conductance circles (same math as resistance, just with
           // different colors and on the other side of the chart)
    QVector<double> conductances = {0.2, 0.5, 1.0, 2.0, 5.0};
    for (double g : conductances) {
      double x = -radius * g / (1 + g);  // Note the negative sign to mirror
      double y = radius / (1 + g);
      QPointF circleCenter(center.x() + x, center.y());
      painter->drawEllipse(circleCenter, y, y);

             // Paint label with actual admittance value based on Y0 = 1/Z0
      QPointF label_position(center.x() + x + y/2, center.y()-5);
      painter->setPen(QPen(Qt::red, 2));
      painter->drawText(label_position, QString::number(g / z0, 'f', 3));
      painter->setPen(admittancePen);
    }

           // Draw constant susceptance arcs
    QVector<double> susceptances = {0.2, 0.5, 1.0, 2.0, 5.0};
    for (double b : susceptances) {
      // For admittance, we just negate the reactance to get susceptance arcs
      // drawReactanceArc already handles positive/negative values
      drawSusceptanceArc(painter, center, radius, b);
      drawSusceptanceArc(painter, center, radius, -b);
    }
  }

  painter->restore();
}

void SmithChartWidget::drawReactanceArc(QPainter *painter, const QPointF &center, double radius, double reactance) {
  painter->setPen(QPen(Qt::gray, 1));
  double x = reactance;
  double chartRadius = radius;

         // Center coordinates in normalized form
  double normalizedCenterX = 1.0;
  double normalizedCenterY = 1.0 / x;

         // Circle radius in normalized form
  double normalizedRadius = 1.0 / std::abs(x);

         // Convert to widget coordinates
  double centerX = center.x() + chartRadius * normalizedCenterX;
  double centerY = center.y() - chartRadius * normalizedCenterY;
  double arcRadius = chartRadius * normalizedRadius;

  QRectF arcRect(
      centerX - arcRadius,
      centerY - arcRadius,
      2 * arcRadius,
      2 * arcRadius
      );

         // Calculate intersection with unit circle
  double d2 = normalizedCenterX * normalizedCenterX + normalizedCenterY * normalizedCenterY;
  double d = std::sqrt(d2);

         // Calculate central angle using cosine law
  double cos_theta = (d2 + normalizedRadius * normalizedRadius - 1.0) / (2.0 * d * normalizedRadius);
  double theta = std::acos(cos_theta);

         // Convert to degrees and calculate the arc angles
  double base_angle = std::atan2(normalizedCenterY, normalizedCenterX) * 180.0 / M_PI;
  double theta_deg = theta * 180.0 / M_PI;

         // Calculate start and sweep angles
  double startAngle, sweepAngle;
  if (x > 0) {
    startAngle = (base_angle - theta_deg) - 180;
    sweepAngle = 2 * theta_deg;
  } else {
    startAngle = (base_angle + theta_deg) - 180;
    sweepAngle = -2 * theta_deg;
  }

         // Draw the arc (Qt uses 16th of a degree)
  painter->drawArc(arcRect, startAngle * 16, sweepAngle * 16);

         // Variables to store the starting and ending points
  QPointF startPoint, endPoint;

         // Calculate the points
  calculateArcPoints(arcRect, startAngle, sweepAngle, startPoint, endPoint);

         // Draw points at start and end
  painter->setPen(QPen(Qt::red, 4));
  painter->drawPoint(startPoint);

         // Calculate the direction from the center to the start point
  QPointF directionStart = startPoint - center;
  double directionLengthStart = std::sqrt(directionStart.x() * directionStart.x() + directionStart.y() * directionStart.y());

         // Normalize the direction vector
  directionStart /= directionLengthStart;

         // Move the label outside the unit circle
  double labelOffset = 1.02; // Scale factor to place the label outside the unit circle
  QPointF labelPositionStart = center + directionStart * (chartRadius * labelOffset);

         // Draw labels
  painter->setPen(QPen(Qt::black, 2));
  painter->drawText(labelPositionStart, QString::number(reactance * z0, 'f', 1));

  // Calculate the direction from the center to the end point
  QPointF directionEnd = endPoint - center;
  double directionLengthEnd = std::sqrt(directionEnd.x() * directionEnd.x() + directionEnd.y() * directionEnd.y());

  // Normalize the direction vector
  directionEnd /= directionLengthEnd;

  // Move the label further OUTSIDE the unit circle
  labelOffset = 1.12; // Increased from 1.05 to 1.12 for more distance from circle
  QPointF labelPositionEnd = center + directionEnd * (chartRadius * labelOffset);

  // Draw the reactance value near the end point
  QString label = QString::number(reactance * z0, 'f', 1);
  painter->drawText(labelPositionEnd, label);
}

void SmithChartWidget::drawSusceptanceArc(QPainter *painter, const QPointF &center, double radius, double susceptance) {
  QPen admittancePen(Qt::red, 1);
  painter->setPen(admittancePen);

  double b = susceptance;
  double chartRadius = radius;

         // Center coordinates in normalized form
  double normalizedCenterX = -1.0;  // Negative for admittance chart
  double normalizedCenterY = 1.0 / b;

         // Circle radius in normalized form
  double normalizedRadius = 1.0 / std::abs(b);

         // Convert to widget coordinates
  double centerX = center.x() + chartRadius * normalizedCenterX;
  double centerY = center.y() - chartRadius * normalizedCenterY;
  double arcRadius = chartRadius * normalizedRadius;

  QRectF arcRect(
      centerX - arcRadius,
      centerY - arcRadius,
      2 * arcRadius,
      2 * arcRadius
      );

         // Calculate intersection with unit circle
  double d2 = normalizedCenterX * normalizedCenterX + normalizedCenterY * normalizedCenterY;
  double d = std::sqrt(d2);

         // Calculate central angle using cosine law
  double cos_theta = (d2 + normalizedRadius * normalizedRadius - 1.0) / (2.0 * d * normalizedRadius);
  double theta = std::acos(cos_theta);

         // Convert to degrees and calculate the arc angles
  double base_angle = std::atan2(normalizedCenterY, normalizedCenterX) * 180.0 / M_PI;
  double theta_deg = theta * 180.0 / M_PI;

         // Calculate start and sweep angles
  double startAngle, sweepAngle;
  if (b > 0) {
    startAngle = (base_angle - theta_deg) - 180;
    sweepAngle = 2 * theta_deg;
  } else {
    startAngle = (base_angle + theta_deg) - 180;
    sweepAngle = -2 * theta_deg;
  }

         // Draw the arc (Qt uses 16th of a degree)
  painter->drawArc(arcRect, startAngle * 16, sweepAngle * 16);

         // Variables to store the starting and ending points
  QPointF startPoint, endPoint;

         // Calculate the points
  calculateArcPoints(arcRect, startAngle, sweepAngle, startPoint, endPoint);

         // Draw points at start and end
  painter->setPen(QPen(Qt::red, 4));
  painter->drawPoint(startPoint);

         // Calculate the direction from the center to the start point
  QPointF directionStart = startPoint - center;
  double directionLengthStart = std::sqrt(directionStart.x() * directionStart.x() + directionStart.y() * directionStart.y());

         // Normalize the direction vector
  directionStart /= directionLengthStart;

         // Move the label outside the unit circle
  double labelOffset = 1.02; // Scale factor to place the label outside the unit circle
  QPointF labelPositionStart = center + directionStart * (chartRadius * labelOffset);

         // Draw labels
  painter->setPen(QPen(Qt::red, 2));
  painter->drawText(labelPositionStart, QString::number(susceptance / z0, 'f', 3));

  // Calculate the direction from the center to the end point
  QPointF directionEnd = endPoint - center;
  double directionLengthEnd = std::sqrt(directionEnd.x() * directionEnd.x() + directionEnd.y() * directionEnd.y());

  // Normalize the direction vector
  directionEnd /= directionLengthEnd;

  // Move the label INSIDE the unit circle
  labelOffset = 0.95; // Scale factor to place the label inside the unit circle
  QPointF labelPositionEnd = center + directionEnd * (chartRadius * labelOffset);

  // Draw the susceptance value near the end point
  QString label = QString::number(susceptance / z0, 'f', 3);
  painter->drawText(labelPositionEnd, label);
}

// Function to calculate the starting and ending points of the arc
void SmithChartWidget::calculateArcPoints(const QRectF& arcRect, double startAngle, double sweepAngle, QPointF& startPoint, QPointF& endPoint) {
  // Calculate the center of the ellipse
  double cx = arcRect.center().x();
  double cy = arcRect.center().y();

         // Convert angles from degrees to radians
  double startRad = qDegreesToRadians(startAngle);
  double endRad = qDegreesToRadians(startAngle + sweepAngle);

         // Calculate the starting point
  startPoint.setX(cx + (arcRect.width() / 2.0) * std::cos(startRad));
  startPoint.setY(cy - (arcRect.height() / 2.0) * std::sin(startRad)); // Subtract for Qt's coordinate system

         // Calculate the ending point
  endPoint.setX(cx + (arcRect.width() / 2.0) * std::cos(endRad));
  endPoint.setY(cy - (arcRect.height() / 2.0) * std::sin(endRad)); // Subtract for Qt's coordinate system
}

void SmithChartWidget::plotImpedanceData(QPainter *painter) {
  painter->save();

  QPointF center(width() / 2.0, height() / 2.0);
  double radius = qMin(width(), height()) / 2.0 - 10;

         // Iterate through the map of traces
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const Trace& trace = it.value();
    painter->setPen(trace.pen);

           // Check if there are at least two points to draw a line
    if (trace.impedances.size() < 2 || trace.frequencies.size() < 2) {
      continue;
    }

           // Find the range of indices that falls within the frequency range
    int startIdx = -1;
    int endIdx = -1;


    double minFreq = m_minFreqSpinBox->value();
    double maxFreq = m_maxFreqSpinBox->value();
    double multiplier = getFrequencyMultiplier();
    double min_freq_scaled = minFreq*multiplier;
    double max_freq_scaled = maxFreq*multiplier;


    for (int i = 0; i < trace.frequencies.size(); ++i) {
      double freq = trace.frequencies[i];

      // If frequency is within range
      if (freq >= min_freq_scaled && freq <= max_freq_scaled) {
        // If this is the first in-range point, set startIdx
        if (startIdx == -1) {
          startIdx = i;
        }
        // Always update endIdx to the latest in-range point
        endIdx = i;
      }
    }

    // If no points are in range, skip this trace
    if (startIdx == -1 || endIdx == -1) {
      continue;
    }

    // Convert the first in-range impedance point to widget coordinates
    std::complex<double> gammaPrev = (trace.impedances[startIdx] - trace.Z0) /
                                     (trace.impedances[startIdx] + trace.Z0);
    QPointF prevPoint(center.x() + radius * gammaPrev.real(),
                      center.y() - radius * gammaPrev.imag());

           // Iterate through the remaining in-range points and draw lines
    for (int i = startIdx + 1; i <= endIdx; ++i) {
      std::complex<double> gamma = (trace.impedances[i] - trace.Z0) /
                                   (trace.impedances[i] + trace.Z0);
      QPointF currentPoint(center.x() + radius * gamma.real(),
                           center.y() - radius * gamma.imag());

             // Draw a line between the previous point and the current point
      painter->drawLine(prevPoint, currentPoint);

             // Update the previous point
      prevPoint = currentPoint;
    }
  }

  painter->restore();
}


void SmithChartWidget::drawMarkers(QPainter *painter) {
  if (markers.isEmpty() || traces.isEmpty()) {
    return;
  }

  painter->save();
  QPointF center(width() / 2.0, height() / 2.0);
  double radius = qMin(width(), height()) / 2.0 - 10;

         // Font configuration for marker labels
  QFont markerFont = painter->font();
  markerFont.setBold(true);
  painter->setFont(markerFont);

         // Iterate through each trace
  for (auto traceIt = traces.constBegin(); traceIt != traces.constEnd(); ++traceIt) {
    const QString& traceName = traceIt.key();
    const Trace& trace = traceIt.value();

           // Skip traces with no frequency data
    if (trace.frequencies.isEmpty() || trace.impedances.isEmpty()) {
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

             // Interpolate impedance at marker frequency
      std::complex impedance = interpolateImpedance(trace.frequencies, trace.impedances, markerFreq);

             // Convert to reflection coefficient and then to widget coordinates
      std::complex gamma = (impedance - trace.Z0) / (impedance + trace.Z0);
      QPointF markerPoint(center.x() + radius * gamma.real(), center.y() - radius * gamma.imag());

             // Draw marker point
      painter->setPen(marker.pen);
      painter->setBrush(marker.pen.color());
      painter->drawEllipse(markerPoint, 4, 4);

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

      // Create label with marker ID, impedance value, and frequency
      QString label = QString("%1 [%2]: %3 %4\n%5%6j%7Ω")
                          .arg(markerId) // %1
                          .arg(traceName) // %2
                          .arg(freqValue, 0, 'g', 3)  // %3 Use 'g' for compact representation
                          .arg(freqUnit) // %4
                          .arg(impedance.real(), 0, 'f', 2) // %5
                          .arg(impedance.imag() >= 0 ? "+" : "") // %6
                          .arg(impedance.imag(), 0, 'f', 2); // %7

      // Draw label with background
      QFontMetrics fm(markerFont);
      QRect textRect = fm.boundingRect(QRect(0, 0, 300, 0), Qt::TextWordWrap, label);
      textRect.moveTo(markerPoint.x() + 8, markerPoint.y() - textRect.height() / 2);
      textRect.adjust(-6, -6, 6, 6);  // Increase padding

      painter->setPen(Qt::black);
      painter->setBrush(QColor(255, 255, 255, 200));
      painter->drawRect(textRect);
      painter->drawText(textRect.adjusted(6, 6, -6, -6), Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter, label);
    }
  }
  painter->restore();
}


std::complex<double> SmithChartWidget::interpolateImpedance(const QList<double>& frequencies,
                                                            const QList<std::complex<double>>& impedances,
                                                            double targetFreq) {
  // If exact match, return it
  for (int i = 0; i < frequencies.size(); i++) {
    if (qFuzzyCompare(frequencies[i], targetFreq)) {
      return impedances[i];
    }
  }

  // Find the two closest frequencies for interpolation
  int lowerIndex = -1;

  for (int i = 0; i < frequencies.size(); i++) {
    if (frequencies[i] <= targetFreq) {
      lowerIndex = i;
    } else {
      break;
    }
  }

  // Check if target frequency is outside the range
  if (lowerIndex == -1) {
    return impedances.first(); // Return first point if below range
  }
  if (lowerIndex == frequencies.size() - 1) {
    return impedances.last(); // Return last point if above range
  }

  // Perform linear interpolation
  int upperIndex = lowerIndex + 1;
  double lowerFreq = frequencies[lowerIndex];
  double upperFreq = frequencies[upperIndex];
  std::complex<double> lowerZ = impedances[lowerIndex];
  std::complex<double> upperZ = impedances[upperIndex];

  double t = (targetFreq - lowerFreq) / (upperFreq - lowerFreq);

  // Linear interpolation for both real and imaginary parts
  double realPart = lowerZ.real() + t * (upperZ.real() - lowerZ.real());
  double imagPart = lowerZ.imag() + t * (upperZ.imag() - lowerZ.imag());

  return std::complex<double>(realPart, imagPart);
}

QPointF SmithChartWidget::smithChartToWidget(const std::complex<double>& reflectionCoefficient)
{
  double gammaReal = reflectionCoefficient.real();
  double gammaImag = reflectionCoefficient.imag();
  double gammaMagSq = gammaReal * gammaReal + gammaImag * gammaImag;

  double x = gammaReal / (1 + gammaMagSq);
  double y = gammaImag / (1 + gammaMagSq);

         // Calculate the center and radius of the Smith Chart
  QPointF center(width() / 2.0, height() / 2.0);
  double radius = qMin(width(), height()) / 2.0 - 10; // Leave some margin

         // Scale and translate the Smith Chart coordinates to widget coordinates
         // (This depends on the size of your widget and the zoom level)
  double widgetX = center.x() + x * radius;  // Example scaling
  double widgetY = center.y() - y * radius; // and translation

  return QPointF(widgetX, widgetY);
}

std::complex<double> SmithChartWidget::widgetToSmithChart(const QPointF& widgetPoint)
{
  // Calculate the center and radius of the Smith Chart
  QPointF center(width() / 2.0, height() / 2.0);
  double radius = qMin(width(), height()) / 2.0 - 10; // Leave some margin

  double x = (widgetPoint.x() - center.x()) / radius;
  double y = (center.y() - widgetPoint.y()) / radius;

  double gammaReal = x / (x * x + y * y + 1);
  double gammaImag = y / (x * x + y * y + 1);

  return std::complex<double>(gammaReal, gammaImag);
}

// Remove all traces in a row
void SmithChartWidget::clearTraces() {
  traces.clear(); // Remove all traces
  update(); // Trigger a repaint to reflect the changes
}

QPen SmithChartWidget::getTracePen(const QString& traceName) const {
  if (traces.contains(traceName)) {
    return traces[traceName].pen;
  }
  // Return a default pen if the trace doesn't exist
  return QPen();
}

void SmithChartWidget::setTracePen(const QString& traceName, const QPen& pen) {
  if (traces.contains(traceName)) {
    traces[traceName].pen = pen;
    update(); // Trigger a repaint
  }
}

// Gives the traces displayed in the widget (i.e. name and properties). It clears the traces map by dropping frequency, data and Z0 data
QMap<QString, QPen> SmithChartWidget::getTracesInfo() const {

  QMap<QString, QPen> penMap;
  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    penMap.insert(it.key(), it.value().pen);
  }

  return penMap;

}

// Remove a trace given its name
void SmithChartWidget::removeTrace(const QString& traceName) {
  if (traces.contains(traceName)) {
    traces.remove(traceName);
    update(); // Trigger a repaint to reflect the changes
  }
}

// Add a marker with a string ID at the specified frequency
bool SmithChartWidget::addMarker(const QString& markerId, double frequency, const QPen& pen) {
  // Check if marker ID already exists
  if (markers.contains(markerId)) {
    return false;
  }

  // Check if any trace contains this frequency
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
    return false; // Frequency is not within the range of any trace
  }

  // Create a new marker
  Marker marker;
  marker.id = markerId;
  marker.frequency = frequency;
  marker.pen = pen;

  // Add to markers map
  markers.insert(markerId, marker);

  // Trigger repaint
  update();
  return true;
}

// Remove a marker given its ID
bool SmithChartWidget::removeMarker(const QString& markerId) {
  if (!markers.contains(markerId)) {
    return false;
  }

  markers.remove(markerId);
  update();
  return true;
}

// Remove all markers
void SmithChartWidget::clearMarkers() {
  markers.clear();
  update();
}

// Get map of all marker IDs and their frequencies
QMap<QString, double> SmithChartWidget::getMarkers() const {
  QMap<QString, double> markerFrequencies;

  for (auto it = markers.constBegin(); it != markers.constEnd(); ++it) {
    markerFrequencies.insert(it.key(), it.value().frequency);
  }

  return markerFrequencies;
}


void SmithChartWidget::onShowAdmittanceChartChanged(int state)
{
  m_showAdmittanceChart = (state == Qt::Checked);
  update(); // Trigger a repaint
}

void SmithChartWidget::onShowConstantCurvesChanged(int state)
{
  m_showConstantCurves = (state == Qt::Checked);
  update(); // Trigger a repaint
}


void SmithChartWidget::onMinFreqChanged(double value)
{
  // Convert from displayed unit to Hz
  double multiplier = getFrequencyMultiplier();
  m_minFreq = value * multiplier;

  // Make sure min is less than max
  double minFreq = m_minFreqSpinBox->value();
  double maxFreq = m_maxFreqSpinBox->value();

  if (minFreq > maxFreq) {
    m_minFreqSpinBox->blockSignals(true);
    m_minFreqSpinBox->setValue(maxFreq);
    m_minFreqSpinBox->blockSignals(false);
  }

  update(); // Redraw the chart with the new frequency range
}

void SmithChartWidget::onMaxFreqChanged(double value)
{
  // Convert from displayed unit to Hz
  double multiplier = getFrequencyMultiplier();
  m_maxFreq = value * multiplier;

  // Make sure max is greater than min
  double minFreq = m_minFreqSpinBox->value();
  double maxFreq = m_maxFreqSpinBox->value();
  if (maxFreq < minFreq) {
    m_maxFreqSpinBox->blockSignals(true);
    m_maxFreqSpinBox->setValue(minFreq);
    m_maxFreqSpinBox->blockSignals(false);
  }

  update(); // Redraw the chart with the new frequency range
}

void SmithChartWidget::onFreqUnitChanged(int index)
{
  // Store the current values in Hz
  double minFreq = m_minFreqSpinBox->value();
  double maxFreq = m_maxFreqSpinBox->value();
  double oldMinFreq = minFreq;
  double oldMaxFreq = maxFreq;

  // Update displayed values in the spinboxes
  double multiplier = getFrequencyMultiplier();
  m_minFreqSpinBox->blockSignals(true);
  m_maxFreqSpinBox->blockSignals(true);

  m_minFreqSpinBox->setValue(oldMinFreq / multiplier);
  m_maxFreqSpinBox->setValue(oldMaxFreq / multiplier);

  // Adjust spinbox ranges based on the selected unit
  double maxRange;
  switch (index) {
  case 0: // Hz
    maxRange = 1e12; // 1 THz in Hz
    break;
  case 1: // kHz
    maxRange = 1e9; // 1 THz in kHz
    break;
  case 2: // MHz
    maxRange = 1e6; // 1 THz in MHz
    break;
  case 3: // GHz
    maxRange = 1e3; // 1 THz in GHz
    break;
  default:
    maxRange = 1000.0;
  }

  m_minFreqSpinBox->setRange(0.0, maxRange);
  m_maxFreqSpinBox->setRange(0.1, maxRange);

  m_minFreqSpinBox->blockSignals(false);
  m_maxFreqSpinBox->blockSignals(false);

  update(); // Redraw the chart
}

double SmithChartWidget::getFrequencyMultiplier() const
{
  // Return multiplier to convert display units to Hz
  int freq_index = m_freqUnitComboBox->currentIndex();
  switch (freq_index) {
  case 0: return 1.0;         // Hz
  case 1: return 1e3;         // kHz
  case 2: return 1e6;         // MHz
  case 3: return 1e9;         // GHz
  default: return 1.0;
  }
}

void SmithChartWidget::updateFrequencyRange()
{
  // Find the minimum and maximum frequencies across all traces
  bool foundAny = false;
  double minFreq = std::numeric_limits<double>::max();
  double maxFreq = std::numeric_limits<double>::min();

  for (auto it = traces.constBegin(); it != traces.constEnd(); ++it) {
    const Trace& trace = it.value();
    if (!trace.frequencies.isEmpty()) {
      minFreq = qMin(minFreq, trace.frequencies.first());
      maxFreq = qMax(maxFreq, trace.frequencies.last());
      foundAny = true;
    }
  }

  // If we found any frequencies, update the range
  if (foundAny) {
    double multiplier = getFrequencyMultiplier();
    double min_freq_scaled = minFreq/multiplier;
    double max_freq_scaled = maxFreq/multiplier;

    // Update the display values
    m_minFreqSpinBox->blockSignals(true);
    m_maxFreqSpinBox->blockSignals(true);

    m_minFreqSpinBox->setValue(min_freq_scaled);
    m_minFreqSpinBox->setMinimum(min_freq_scaled);

    m_maxFreqSpinBox->setValue(max_freq_scaled);
    m_maxFreqSpinBox->setMaximum(max_freq_scaled);

    m_minFreqSpinBox->blockSignals(false);
    m_maxFreqSpinBox->blockSignals(false);
  }
}

// Update an existing marker's frequency
bool SmithChartWidget::updateMarkerFrequency(const QString& markerId, double newFrequency) {
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
  update();
  return true;
}
