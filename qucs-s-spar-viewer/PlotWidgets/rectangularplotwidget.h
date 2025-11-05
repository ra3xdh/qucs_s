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

class RectangularPlotWidget : public QWidget {
  Q_OBJECT

public:
  struct Trace {
    QList<double> trace;
    QList<double> frequencies;
    QString units;
    QPen pen;
    double Z0;
    int y_axis;
    QString y_axis_title;
  };

  struct Marker {
    QString id;
    double frequency;
    QPen pen;
  };

  struct Limit {
    double f1;  // Start frequency
    double f2;  // End frequency
    double y1;  // Start y
    double y2;  // End y
    int y_axis; // 0: Left y-axis; 1: Right y-axis
    QPen pen;
  };

  // Struct to exchange the widget settings with the main program
  struct AxisSettings {
    double xAxisMin;
    double xAxisMax;
    double xAxisDiv;
    QString xAxisUnits;

    double yAxisMin;
    double yAxisMax;
    double yAxisDiv;

    double y2AxisMin;
    double y2AxisMax;
    double y2AxisDiv;

    bool showValues;
    bool lockAxis;
  };

  explicit RectangularPlotWidget(QWidget* parent = nullptr);
  ~RectangularPlotWidget();

  void addTrace(const QString& name, const Trace& trace);
  double calculateNiceStep(double range);
  void removeTrace(const QString& name);
  void clearTraces();
  QPen getTracePen(const QString& traceName) const;
  void setTracePen(const QString& traceName, const QPen& pen);
  QMap<QString, QPen> getTracesInfo() const;

  double getYmax();
  double getYmin();
  double getYdiv();
  void setYdiv(double);
  void setYmax(double);
  void setYmin(double);

  double getY2max();
  double getY2min();
  double getY2div();

  double getXmax();
  double getXmin();
  double getXdiv();
  double getXscale();
  QString getXunits();
  int getFreqIndex();

  void updatePlot();
  void set_y_autoscale(bool value);

  bool areAxisSettingsLocked() const;

  void setRightYAxisEnabled(bool enabled);
  bool isRightYAxisEnabled() const;

  void change_Y_axis_title(QString title);
  void change_Y_axis_units(QString title);
  void change_Y2_axis_title(QString title);
  void change_Y2_axis_units(QString title);
  void change_X_axis_title(QString title);
  void change_X_axis_label(QString title);

  QLabel* xAxisLabel;

  bool addMarker(const QString& markerId, double frequency,
                 const QPen& pen = QPen(Qt::red, 2));
  bool removeMarker(const QString& markerId);
  bool updateMarkerFrequency(const QString& markerId, double newFrequency);
  void clearMarkers();
  QMap<QString, double> getMarkers() const;

  bool addLimit(const QString& LimitId, const Limit& limit);
  void removeLimit(const QString& LimitID);
  void clearLimits();
  QMap<QString, Limit> getLimits() const;
  bool updateLimit(const QString& limitId, const Limit& limit);

  QCustomPlot* customPlot() const { return plotWidget; }

  // Exchange the settings with the main program
  AxisSettings getSettings() const;
  void setSettings(const AxisSettings& settings);

private slots:
  void updateXAxis();
  void updateYAxis();
  void updateY2Axis();
  void changeFreqUnits();
  void toggleShowValues(bool show);
  void toggleLockAxisSettings(bool locked);

private slots
    : // To handle axis settings widgets when panning or zooming the plot
  void onXAxisRangeChanged(const QCPRange& range);
  void onYAxisRangeChanged(const QCPRange& range);
  void onY2AxisRangeChanged(const QCPRange& range);

private:
  QCustomPlot* plotWidget;

  QDoubleSpinBox* xAxisMin;
  QDoubleSpinBox* xAxisMax;
  QDoubleSpinBox* xAxisDiv;
  QComboBox* xAxisUnits;

  QDoubleSpinBox* yAxisMin;
  QDoubleSpinBox* yAxisMax;
  QDoubleSpinBox* yAxisDiv;
  QLabel* yAxisUnits;

  QDoubleSpinBox* y2AxisMin;
  QDoubleSpinBox* y2AxisMax;
  QDoubleSpinBox* y2AxisDiv;
  QLabel* y2AxisUnits;
  QLabel* y2AxisLabel;

  QCheckBox* showValuesCheckbox;
  bool showTraceValues;

  QCheckBox* lockAxisCheckbox;
  bool axisSettingsLocked;

  QStringList frequencyUnits;
  double fMin;
  double fMax;

  bool y_autoscale;

  QMap<QString, Trace> traces;
  QMap<QString, Marker> markers;
  QMap<QString, Limit> limits;

  // QCustomPlot specific members
  QMap<QString, QCPGraph*> traceGraphs;
  QMap<QString, QCPItemStraightLine*> markerLines;
  QMap<QString, QCPItemText*> markerLabels;
  QMap<QString, QCPItemTracer*> intersectionPoints;
  QMap<QString, QCPItemText*> intersectionLabels;
  QMap<QString, QCPGraph*> limitGraphs;

  QGridLayout* setupAxisSettings();
  void clearGraphicsItems();
  void setupPlot();
  void addMarkerIntersections(const QString& markerId, const Marker& marker);

  int getYAxisTraceCount() const;
  int getY2AxisTraceCount() const;
};

#endif // RECTANGULARPLOTWIDGET_H
