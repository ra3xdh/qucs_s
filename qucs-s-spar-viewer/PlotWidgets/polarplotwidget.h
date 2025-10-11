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

class PolarPlotWidget : public QWidget {
  Q_OBJECT

public:
  struct Trace {
    QList<std::complex<double>>
        values;                // Complex values (for polar representation)
    QList<double> frequencies; // Corresponding frequencies for each point
    QPen pen;                  // Line style for this trace
    int displayMode;           // 0: Magnitude/Phase, 1: Real/Imaginary
  };

  struct Marker {
    QString id;
    double frequency;
    QPen pen;
  };

  struct AxisSettings {
    double freqMax;
    double freqMin;
    QString freqUnit;

    double radius_min;
    double radius_max;
    double radius_div;

    QString marker_format;
  };

  void setSettings(const AxisSettings& settings);
  PolarPlotWidget::AxisSettings getSettings() const;

  explicit PolarPlotWidget(QWidget* parent = nullptr);
  ~PolarPlotWidget();

  // Trace management functions
  void addTrace(const QString& name, const Trace& trace);
  void removeTrace(const QString& name);
  void clearTraces();
  QPen getTracePen(const QString& traceName) const;
  void setTracePen(const QString& traceName, const QPen& pen);
  QMap<QString, QPen> getTracesInfo() const;

  // Axis value access functions
  double getRmax();
  double getRmin();
  double getRdiv();
  int getDisplayMode() const;

  // Marker management functions
  bool addMarker(const QString& markerId, double frequency,
                 const QPen& pen = QPen(Qt::red, 2));
  bool removeMarker(const QString& markerId);
  bool updateMarkerFrequency(const QString& markerId, double newFrequency);
  void clearMarkers();

  QMap<QString, double> getMarkers() const;

  // Access to underlying plot
  QCustomPlot* customPlot() const { return plot; }

private slots:
  void updateRAxis();
  void updateAngleAxis();
  void toggleDisplayMode();

  void onFMinChanged(double value);
  void onFMaxChanged(double value);
  void onFUnitChanged();

private slots: // Slot to update axis settings widget based on the user zoom
  void checkAxisRanges();
  void checkAxisRanges(QMouseEvent* event);
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

  void updateFrequencyRange();
  QGridLayout* setupAxisSettings();
  void updatePlot();
  void clearGraphicsItems();
  void drawCustomMarkers();
  double getFrequencyMultiplier() const;

  // Helper to find a complex value at a specific frequency using interpolation
  std::complex<double> getComplexValueAtFrequency(const Trace& trace,
                                                  double frequency);

  // Convert between display modes (magnitude/phase <-> real/imaginary)
  std::complex<double> convertToDisplayFormat(const std::complex<double>& value,
                                              int mode);
};

#endif // POLARPLOTWIDGET_H
