#ifndef QUCSSPARVIEWER_H
#define QUCSSPARVIEWER_H

#include "codeeditor.h"
#include "smithchartwidget.h"
#include "rectangularplotwidget.h"
#include "polarplotwidget.h"
#include "matrixcombopopup.h"

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

// Struct to hold all the widgets related a marker
struct MarkerProperties {
  QLabel* nameLabel;
  QDoubleSpinBox* freqSpinBox;
  QComboBox* scaleComboBox;
  QToolButton* deleteButton;
};

// Structures for trace management
enum class DisplayMode {
  Magnitude_dB,
  Phase,
  Smith,
  Polar,
  PortImpedance,
  Stability,
  VSWR,
  GroupDelay
};

// Struct to hold all the widgets related a trace
struct TraceProperties {
  QLabel* nameLabel;
  QSpinBox* width;
  QPushButton * colorButton;
  QComboBox* LineStyleComboBox;
  QToolButton* deleteButton;
  QString display_mode;
};


// Struct to hold all the widgets related a limit
struct LimitProperties {
  QLabel* LimitLabel;
  QDoubleSpinBox * Start_Freq;
  QDoubleSpinBox * Stop_Freq;
  QDoubleSpinBox * Start_Value;
  QDoubleSpinBox * Stop_Value;
  QComboBox * Start_Freq_Scale;
  QComboBox * Stop_Freq_Scale;
  QComboBox * axis;
  QToolButton * Button_Delete_Limit;
  QFrame* Separator;
  QPushButton* Couple_Value;
};

// Structure to hold trace information
struct TraceInfo {
  QString dataset;
  QString parameter;     // e.g., "S11", "S21"
  DisplayMode displayMode;

  // Helper to generate a unique identifier for the trace
  QString uniqueId() const {
    if ((displayMode == DisplayMode::Magnitude_dB) || (displayMode == DisplayMode::Phase)) {
      return QString("%1.%2_%3").arg(dataset).arg(parameter).arg(static_cast<int>(displayMode));
    } else {
      return QString("%1.%2").arg(dataset).arg(parameter);
    }
  }

  // Helper to get a human-readable name for the trace
  QString displayName() const {
    QString modeName = QString("");

    if (displayMode == DisplayMode::Magnitude_dB) {
      modeName = "dB";
    } else {
      if (displayMode == DisplayMode::Phase) {
        modeName = "Phase";
      } else {
        return QString("%1.%2").arg(dataset).arg(parameter);
      }
    }
    return QString("%1.%2_%3").arg(dataset).arg(parameter).arg(modeName);
  }
};

class Qucs_S_SPAR_Viewer : public QMainWindow
{
 Q_OBJECT
 public:
  Qucs_S_SPAR_Viewer();
  ~Qucs_S_SPAR_Viewer();
  void addPathToWatcher(const QString &path); // It's needed to pass the directory to watch from the main program

 private slots:
  void slotHelpIntro();
  void slotHelpAbout();
  void slotHelpAboutQt();
  void slotQuit();
  void slotSave();
  void slotSaveAs();
  void slotLoadSession();

  void raiseWidgetsOnTabSelection(int index);

  void addFile();
  void addFiles(QStringList);
  QMap<QString, QList<double>> readTouchstoneFile(const QString& filePath);
  QMap<QString, QList<double>> readQucsatorDataset(const QString& filePath);
  QMap<QString, QList<double>> readNGspiceData(const QString& filePath);
  QString extractSParamIndices(const QString& sparam);
  void applyDefaultVisualizations(const QStringList& fileNames);
  void addOptionalTraces(QMap<QString, QList<double>>& file_data);
  void removeFile();
  void removeFile(QString ID);
  void removeAllFiles();
  void removeTracesByDataset(const QString& dataset_to_remove);
  void removeTraceByProps(DisplayMode mode, const QString& traceID, TraceProperties& props);
  void CreateFileWidgets(QString filename, int position);

  // File watching functions
  void setupFileWatcher();
  void fileChanged(const QString &path);
  void directoryChanged(const QString &path);

  void addTrace();
  void addTrace(const TraceInfo& traceInfo, QColor trace_color, int trace_width, QString trace_style = "Solid");
  void removeTrace();
  //void removeTrace(const QString& );
  //void removeTrace(QStringList);
  void removeAndCollapseRow(QGridLayout* targetLayout, int row_to_remove);
  int getNumberOfTraces();
  //bool getTraceByPosition(int position, QString& outTraceName, TraceProperties& outProperties);

  void updateTracesCombo();
  void updateDisplayType();

  void changeTraceColor();
  void changeTraceLineStyle();
  void changeTraceWidth();
  void changeMarkerLimits();
  void changeMarkerLimits(QString);

  void addMarker(double freq = -1, QString Freq_Marker_Scale = QString("MHz"));
  void removeMarker();
  void removeMarker(const QString &);
  void removeAllMarkers();
  int getNumberOfMarkers();
  void updateMarkerTable();
  void updateMarkerNames();
  void updateMarkerData(QTableWidget & layout, DisplayMode mode, QStringList header);
  bool getMarkerByPosition(int position, QString& outMarkerName, MarkerProperties& outProperties);

  void addLimit(double f_limit1=-1, QString f_limit1_unit = "", double f_limit2=-1, QString f_limit2_unit = "", double y_limit1=-1, double y_limit2=-1, bool coupled=true);
  void removeLimit();
  void removeLimit(QString);
  void removeAllLimits();
  void updateLimits();
  void updateLimitNames();
  int getNumberOfLimits();
  bool getLimitByPosition(int, QString&, LimitProperties&);

  void coupleSpinBoxes();

  void updateGridLayout(QGridLayout*);
  void updateAllPlots(const QString& datasetName);
  void updateTracesInWidget(QWidget* widget, const QString& datasetName);

  void calculate_Sparameter_trace(QString, QString);

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

  // This structure groups the widgets related to the traces so that they can be accessed by name
  QMap<DisplayMode, QMap<QString, TraceProperties>>  traceMap;

  QTabWidget *traceTabs;
  QWidget *magnitudePhaseTab, *smithTab, *polarTab, *portImpedanceTab, *stabilityTab, *VSWRTab, *GroupDelayTab;
  QGridLayout *magnitudePhaseLayout, *smithLayout, *polarLayout, *portImpedanceLayout, *stabilityLayout, *VSWRLayout, *GroupDelayLayout;

  // Axis settings widgets
  QPushButton *Lock_axis_settings_Button;
  bool lock_axis;
  QStringList frequency_units;

  // Trace management widgets
  QComboBox *QCombobox_datasets, *QCombobox_display_mode;
  MatrixComboBox *QCombobox_traces;
  QPushButton *Button_add_trace;
  QTableWidget *Traces_Widget;

  // Scrollable trace areas
  void setupScrollableLayout();
  void setupScrollAreaForLayout(QGridLayout* &layout, QWidget* parentTab, const QString &objectName);
  QScrollArea *magnitudePhaseScrollArea, *smithScrollArea, *polarScrollArea, *nuScrollArea, *GroupDelayScrollArea;

  // Datasets
  QMap<QString, QMap<QString, QList<double>>> datasets;

  /*
      KEY       |         DATA
  Filename1.s2p | {"freq", "S11_dB", ..., "S22_ang"}
      ...       |          ...
  Filenamek.s3p | {"freq", "S11_dB", ..., "S33_ang"}
  */

  // File watching variables
  QFileSystemWatcher *fileWatcher;
  QMap<QString, QString> watchedFilePaths;

  // Rectangular plot
  RectangularPlotWidget *Magnitude_PhaseChart;
  QDockWidget *dockChart;
  double f_min, f_max; // Minimum (maximum) values of the display
  QList<QColor> default_colors;
  QList<QGraphicsItem*> textLabels;
  bool removeSeriesByName(QChart*, const QString&);

  // Smith Chart
  SmithChartWidget *smithChart;
  QDockWidget *dockSmithChart;
  QList<SmithChartWidget::Trace> SmithChartTraces;

  // Polar plot
  PolarPlotWidget *polarChart;
  QDockWidget *dockPolarChart;
  QList<PolarPlotWidget::Trace> PolarChartTraces;

  // Port impedance plot (Rectangular plot)
  RectangularPlotWidget *impedanceChart;
  QDockWidget *dockImpedanceChart;
  QList<RectangularPlotWidget::Trace> impedanceChartTraces;

  // Stability plot (Rectangular plot)
  RectangularPlotWidget *stabilityChart;
  QDockWidget *dockStabilityChart;
  QList<RectangularPlotWidget::Trace> stabilityChartTraces;

  // VSWR plot (Rectangular plot)
  RectangularPlotWidget *VSWRChart;
  QDockWidget *dockVSWRChart;
  QList<RectangularPlotWidget::Trace> VSWRChartTraces;

  // Group delay plot (Rectangular plot)
  RectangularPlotWidget *GroupDelayChart;
  QDockWidget *dockGroupDelayChart;
  QList<RectangularPlotWidget::Trace> GroupDelayTraces;

  // Markers
  QDockWidget *dockMarkers;
  QWidget *Marker_Widget;
  QGridLayout * MarkersGrid;
  QPushButton *Button_add_marker, *Button_Remove_All_Markers;
  QTableWidget* tableMarkers_Magnitude_Phase,  *tableMarkers_Smith, *tableMarkers_Polar, *tableMarkers_PortImpedance, *tableMarkers_Stability, *tableMarkers_VSWR, *tableMarkers_GroupDelay;
  QMap<QString, MarkerProperties> markerMap; // All marker widgets are here. This way they can be accessed by the name of the marker

  double getMarkerFreq(QString);

  // Limits
  QDockWidget *dockLimits;
  QWidget *Limits_Widget;
  QGridLayout * LimitsGrid;
  QPushButton *Button_add_Limit, *Button_Remove_All_Limits;
  QDoubleSpinBox * Limits_Offset;
  QMap<QString, LimitProperties> limitsMap; // This structure groups the widgets related to the traces so that they can be accessed by name

  // Save
  QString savepath;
  bool save();
  void loadSession(QString);

  // Save rectangular plot settings
  void saveRectangularPlotSettings(QXmlStreamWriter &xml, RectangularPlotWidget *widget, const QString &elementName);
  void loadRectangularPlotSettings(QXmlStreamReader &xml, RectangularPlotWidget *widget, const QString &elementName);

  void saveSmithPlotSettings(QXmlStreamWriter &xml, SmithChartWidget *widget, const QString &elementName);
  void loadSmithPlotSettings(QXmlStreamReader &xml, SmithChartWidget *widget, const QString &elementName);

  void savePolarPlotSettings(QXmlStreamWriter &xml, PolarPlotWidget *widget, const QString &elementName);
  void loadPolarPlotSettings(QXmlStreamReader &xml, PolarPlotWidget *widget, const QString &elementName);


  // Notes
  QDockWidget *dockNotes;
  CodeEditor *Notes_Widget;

  // Recent files
  std::vector<QString> recentFiles;
  QMenu* recentFilesMenu;
  void updateRecentFilesMenu();
  void loadRecentFiles();
  void addRecentFile(const QString&);
  void clearRecentFiles();
  void saveRecentFiles();

  // Setup UI
  void CreateMenuBar();
  void CreateDisplayWidgets(); // Setup magnitude/phase and Smith charts

  void CreateRightPanel(); // Setup managing docks
  void setFileManagementDock(); // Setup file managment dock
  void setTraceManagementDock(); // Setup trace managment dock
  void setMarkerManagementDock(); // Setup marker managment dock
  void setLimitManagementDock(); // Setup marker managment dock

  // Utilities
  void convert_MA_RI_to_dB(double *, double *, double *, double *, QString);
  double getFreqScale(QString);
  void getMinMaxValues(QString, QString, qreal&, qreal&, qreal&, qreal&);
  void checkFreqSettingsLimits(QString filename, double& fmin, double& fmax);
  int findClosestIndex(const QList<double>&, double);
  void adjust_x_axis_to_file(QString);
  void adjust_y_axis_to_trace(QString, QString);
  void adjust_x_axis_div();
  QPointF findClosestPoint(const QList<double>&, const QList<double>&, qreal);
  double getFreqFromText(QString);

  // File monitoring
  void setupSimulationWatcher();
  QStringList getWatchDirectories() const;
  bool isSparamFile(const QString& path); // Used to accept only data files when scanning project directories
  QStringList filePaths; // Full path of the files in the progrom. It's used for file monitoring.
};

#endif

