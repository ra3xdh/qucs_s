#ifndef QUCSSPARVIEWER_H
#define QUCSSPARVIEWER_H

#include <QMainWindow>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QGridLayout>
#include <QColorDialog>
#include <QScrollArea>
#include <QtCharts>
#include <QtGlobal>
#include <complex>

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
using namespace QtCharts;
#endif

class QComboBox;
class QTableWidget;
class QLineEdit;
class QIntValidator;
class QDoubleValidator;
class QLabel;
class QPushButton;

struct tQucsSettings
{
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString Language;
};

extern struct tQucsSettings QucsSettings;


class Qucs_S_SPAR_Viewer : public QMainWindow
{
 Q_OBJECT
 public:
  Qucs_S_SPAR_Viewer();
  ~Qucs_S_SPAR_Viewer();

 private slots:
  void slotHelpIntro();
  void slotHelpAbout();
  void slotHelpAboutQt();
  void slotQuit();
  void slotSave();
  void slotSaveAs();
  void slotLoadSession();

  void addFile();
  void addFiles(QStringList);
  void removeFile();
  void removeFile(int);
  void removeAllFiles();

  void addTrace();
  void addTrace(QString, QString, QColor, int trace_width = 1, QString trace_style = "Solid");
  void removeTrace();
  void removeTrace(int);
  void removeTrace(QList<int>);

  void updatePlot();
  void updateTraces();
  void updateTracesCombo();

  void changeTraceColor();
  void changeTraceLineStyle();
  void changeTraceWidth();
  void changeFreqUnits();
  void changeMarkerLimits();
  void changeMarkerLimits(QString);

  void update_X_axis();
  void update_Y_axis();
  void lock_unlock_axis_settings();

  void addMarker(double freq = -1);
  void removeMarker();
  void removeMarker(int);
  void removeAllMarkers();
  void updateMarkerTable();

  void addLimit(double f_limit1=-1, QString f_limit1_unit = "", double f_limit2=-1, QString f_limit2_unit = "", double y_limit1=-1, double y_limit2=-1, bool coupled=false);
  void removeLimit();
  void removeLimit(int);
  void removeAllLimits();
  void updateLimits();

  void coupleSpinBoxes();

 protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;

 private:
  QDockWidget *dockFiles;
  QTableWidget * spar_files_Widget;
  QPushButton *Button_Add_File, *Delete_All_Files;

  // File list
  QList<QPushButton*> Button_DeleteFile;

  // Filenames and remove buttons
  QVBoxLayout *vLayout_Files;
  QWidget * FileList_Widget;
  QGridLayout * FilesGrid;
  QList<QLabel *> List_FileNames;
  QList<QToolButton *> List_RemoveButton;

  // Trace list
  QDockWidget *dockTracesList;
  QWidget * TracesList_Widget;
  QGridLayout * TracesGrid;
  QList<QLabel *> List_TraceNames;
  QList<QSpinBox *> List_TraceWidth;
  QList<QPushButton *> List_Trace_Color;
  QList<QComboBox *> List_Trace_LineStyle;
  QList<QComboBox *> List_Trace_Type;
  QList<QToolButton*> List_Button_DeleteTrace;

  // Axis settings widgets
  QDockWidget *dockAxisSettings;
  QComboBox *QCombobox_x_axis_units;//, *QCombobox_y_axis_units, *QCombobox_y2_axis_units;
  QDoubleSpinBox *QSpinBox_x_axis_min, *QSpinBox_x_axis_max;
  QList<double> available_x_axis_div;
  QComboBox *QComboBox_x_axis_div;
  QDoubleSpinBox *QSpinBox_y_axis_min, *QSpinBox_y_axis_max, *QSpinBox_y_axis_div;
  QList<double> available_y_axis_div;
  QComboBox *QComboBox_y_axis_div;
  QDoubleSpinBox *QSpinBox_y2_axis_min, *QSpinBox_y2_axis_max, *QSpinBox_y2_axis_div;
  QPushButton *Lock_axis_settings_Button;
  bool lock_axis;
  QStringList frequency_units;

  // Trace management widgets
  QComboBox *QCombobox_datasets, *QCombobox_traces;
  QPushButton *Button_add_trace;
  QTableWidget *Traces_Widget;

  // Datasets
  QMap<QString, QMap<QString, QList<double>>> datasets;

  /*
      KEY       |         DATA
  Filename1.s2p | {"freq", "S11_dB", ..., "S22_ang"}
      ...       |          ...
  Filenamek.s3p | {"freq", "S11_dB", ..., "S33_ang"}
  */

  // Trace data
  QList<QString> trace_list;
  QMap<QString, QList<QString>> trace_properties;

  // Chart
  QChart *chart;
  QDockWidget *dockChart;
  QValueAxis *xAxis, *yAxis;
  double f_min, f_max, y_min, y_max; // Minimum (maximum) values of the display
  QList<QColor> default_colors;
  bool removeSeriesByName(QChart*, const QString&);

  // Markers
  QDockWidget *dockMarkers;
  QWidget *Marker_Widget;
  QGridLayout * MarkersGrid;
  QTableWidget *tableMarkers;
  QPushButton *Button_add_marker, *Button_Remove_All_Markers;

  QList<QLabel *> List_MarkerNames;
  QList<QDoubleSpinBox *> List_MarkerFreq;
  QList<QComboBox *> List_MarkerScale;
  QList<QToolButton*> List_Button_DeleteMarker;

  // Limits
  QDockWidget *dockLimits;
  QWidget *Limits_Widget;
  QGridLayout * LimitsGrid;
  QPushButton *Button_add_Limit, *Button_Remove_All_Limits;
  QList<QLabel *> List_LimitNames;
  QList<QDoubleSpinBox *> List_Limit_Start_Freq, List_Limit_Stop_Freq;
  QList<QDoubleSpinBox *> List_Limit_Start_Value, List_Limit_Stop_Value;
  QList<QComboBox *> List_Limit_Start_Freq_Scale, List_Limit_Stop_Freq_Scale;
  QList<QToolButton*> List_Button_Delete_Limit;
  QList<QFrame*> List_Separators;
  QList<QPushButton*> List_Couple_Value;

  // Save
  QString savepath;
  bool save();
  void loadSession(QString);

  // Utilities
  void convert_MA_RI_to_dB(double *, double *, double *, double *, QString);
  double getFreqScale();
  double getFreqScale(QString);
  void getMinMaxValues(QString, QString, qreal&, qreal&, qreal&, qreal&);
  void checkFreqSettingsLimits(QString filename, double& fmin, double& fmax);
  int findClosestIndex(const QList<double>&, double);
  void adjust_x_axis_to_file(QString);
  void adjust_y_axis_to_trace(QString, QString);
  void adjust_x_axis_div();
  QPointF findClosestPoint(const QList<double>&, const QList<double>&, qreal);
  double getFreqFromText(QString);
};

#endif
