#ifndef POLARPLOTWIDGET_H
#define POLARPLOTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QPolarChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QPen>
#include <QGraphicsTextItem>
#include <QCheckBox>
#include <complex>
#include <limits>

class PolarPlotWidget : public QWidget
{
  Q_OBJECT

public:
  struct Trace {
    QList<std::complex<double>> values;    // Complex values (for polar representation)
    QList<double> frequencies;             // Corresponding frequencies for each point
    QPen pen;                              // Line style for this trace
    int displayMode;                       // 0: Magnitude/Phase, 1: Real/Imaginary
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

  explicit PolarPlotWidget(QWidget *parent = nullptr);
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

  QList<QGraphicsEllipseItem*> markerItems;

  // Marker management functions
  void drawCustomMarkers();
  bool addMarker(const QString& markerId, double frequency, const QPen& pen = QPen(Qt::red, 2));
  bool removeMarker(const QString& markerId);
  bool updateMarkerFrequency(const QString& markerId, double newFrequency);
  void clearMarkers();

  QMap<QString, double> getMarkers() const;

         // Access to underlying chart
  QPolarChart *chart() const { return polarChart; }

private slots:
  void updateRAxis();
  void updateAngleAxis();
  void toggleDisplayMode();

  void onFMinChanged(double value);
  void onFMaxChanged(double value);
  void onFUnitChanged();
  double getFrequencyMultiplier() const;

private:
  QPolarChart *polarChart;
  QChartView *chartView;
  QValueAxis *radiusAxis;
  QCategoryAxis *angleAxis;

  QDoubleSpinBox *rAxisMin;
  QDoubleSpinBox *rAxisMax;
  QDoubleSpinBox *rAxisDiv;
  QComboBox *displayModeCombo;

  QDoubleSpinBox *fMinSpinBox;
  QDoubleSpinBox *fMaxSpinBox;
  QComboBox *fUnitComboBox;
  double fMin;
  double fMax;
  QStringList frequencyUnits;
  void updateFrequencyRange();

  QMap<QString, Trace> traces;
  QMap<QString, Marker> markers;

         // Lists to keep track of graphics items
  QList<QGraphicsTextItem*> markerLabels;
  QList<QGraphicsTextItem*> valueLabels;

  QGridLayout* setupAxisSettings();
  void updatePlot();
  void clearGraphicsItems();

  // Helper to find a complex value at a specific frequency using interpolation
  std::complex<double> getComplexValueAtFrequency(const Trace& trace, double frequency);

  // Convert between display modes (magnitude/phase <-> real/imaginary)
  std::complex<double> convertToDisplayFormat(const std::complex<double>& value, int mode);
};

#endif // POLARPLOTWIDGET_H
