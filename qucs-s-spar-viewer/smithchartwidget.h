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

#ifndef SMITHCHARTWIDGET_H
#define SMITHCHARTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMap>
#include <complex>
#include <QPen>
#include <QSet>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>

class Qucs_S_SPAR_Viewer; // Forward declaration

class SmithChartWidget : public QWidget {
  Q_OBJECT

public:
  struct Trace {
    QList<std::complex<double>> impedances;
    QList<double> frequencies;
    QPen pen;
    double Z0;
  };

  struct Marker {
    QString id;
    double frequency;
    QPen pen;
  };

  SmithChartWidget(QWidget *parent = nullptr);
  ~SmithChartWidget() override;

  void addTrace(const QString& name, const Trace& trace);
  void removeTrace(const QString&);
  void clearTraces();
  void setCharacteristicImpedance(double z0);
  double characteristicImpedance() const { return z0; }
  QPen getTracePen(const QString& traceName) const;
  void setTracePen(const QString& traceName, const QPen& pen);
  QMap<QString, QPen> getTracesInfo() const;

  bool addMarker(const QString& markerId, double frequency, const QPen& pen = QPen(Qt::red, 2));
  bool removeMarker(const QString& markerId);
  bool updateMarkerFrequency(const QString& markerId, double newFrequency);

  void clearMarkers();
  QMap<QString, double> getMarkers() const;

signals:
  void impedanceSelected(const std::complex<double>& impedance);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;

private:
  void drawSmithChartGrid(QPainter *painter);
  void drawReactanceArc(QPainter *painter, const QPointF &center, double radius, double reactance);
  void drawSusceptanceArc(QPainter *painter, const QPointF &center, double radius, double susceptance);
  void plotImpedanceData(QPainter *painter);

  void drawMarkers(QPainter *painter);
  QPointF smithChartToWidget(const std::complex<double>& reflectionCoefficient);
  std::complex<double> widgetToSmithChart(const QPointF& widgetPoint);
  std::complex<double> interpolateImpedance(const QList<double>& frequencies,
                                            const QList<std::complex<double>>& impedances,
                                            double targetFreq);

  void calculateArcPoints(const QRectF& arcRect, double startAngle, double sweepAngle, QPointF& startPoint, QPointF& endPoint);

private:
  QMap<QString, Trace> traces;  // Changed from QList to QMap
  QMap<QString, Marker> markers; // Store markers by ID instead of frequency
  double z0; // Characteristic impedance
  QPointF lastMousePos;

         // Add a scale factor for zoom functionality
  double scaleFactor;
  double panX;
  double panY;

private slots:
  void onZ0Changed(int index);
  void onShowAdmittanceChartChanged(int);
  void onShowConstantCurvesChanged(int);

private:
  QComboBox *m_Z0ComboBox;
  QCheckBox *m_ShowAdmittanceChartCheckBox; // Checkbox for admittance chart
  QCheckBox *m_ShowConstantCurvesCheckBox; // Checkbox for reactance/resistance curves
  bool m_showConstantCurves; // Flag for curve display
  bool m_showAdmittanceChart; // Flag for admittance chart display
  QVBoxLayout *m_layout;

private:
  // Frequency range controls
  QDoubleSpinBox *m_minFreqSpinBox;
  QDoubleSpinBox *m_maxFreqSpinBox;
  QComboBox *m_freqUnitComboBox;
  double m_minFreq;  // Minimum frequency in Hz
  double m_maxFreq;  // Maximum frequency in Hz

  // Add to private slots: section in smithchartwidget.h
private slots:
  void onMinFreqChanged(double value);
  void onMaxFreqChanged(double value);
  void onFreqUnitChanged(int index);
  double getFrequencyMultiplier() const;
  void updateFrequencyRange();

};

#endif // SMITHCHARTWIDGET_H
