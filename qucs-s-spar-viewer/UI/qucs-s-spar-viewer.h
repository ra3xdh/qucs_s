/// @file qucs-s-spar-viewer.h
/// @brief Main application class for S-parameter viewer (and its RF circuit synthesis tools)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef QUCSSPARVIEWER_H
#define QUCSSPARVIEWER_H

#include "../PlotWidgets/polarplotwidget.h"
#include "../PlotWidgets/rectangularplotwidget.h"
#include "../PlotWidgets/smithchartwidget.h"

#include "../UI/CustomWidgets/codeeditor.h"
#include "../UI/CustomWidgets/matrixcombopopup.h"

#include "../Tools/AttenuatorDesign/AttenuatorDesignTool.h"
#include "../Tools/Filtering/FilterDesignTool.h"
#include "../Tools/MatchingNetwork/MatchingNetworkDesignTool.h"
#include "../Tools/NetlistScratchPad/netlistscratchpad.h"
#include "../Tools/PowerCombining/PowerCombiningTool.h"
#include "../Tools/SimulationSetup/simulationsetup.h"

#include "../SPAR/SParameterCalculator.h"

#include "../Misc/general.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QTableWidget>
#include <QtGlobal>
#include <complex>
#include <utility> // std::as_const()

class QComboBox;
class QTableWidget;
class QLineEdit;
class QIntValidator;
class QDoubleValidator;
class QLabel;
class QPushButton;

///
/// @struct tQucsSettings
/// @brief Structure to hold Qucs application settings
///
struct tQucsSettings {
  int x, y;          ///< Position of main window (x, y coordinates)
  QFont font;        ///< Application font
  QString LangDir;   ///< Language directory path
  QString Language;  ///< Current language setting
};

extern struct tQucsSettings QucsSettings;

///
/// @struct MarkerProperties
/// @brief Structure to hold all widgets related to a marker
///
struct MarkerProperties {
  QLabel* nameLabel;           ///< Label displaying the marker name
  QDoubleSpinBox* freqSpinBox; ///< Spin box for frequency input
  QComboBox* scaleComboBox;    ///< Combo box for frequency scale selection
  QToolButton* deleteButton;   ///< Button to delete the marker
};

///
/// @enum DisplayMode
/// @brief Enumeration of available trace display modes
///
enum class DisplayMode {
  Magnitude_dB,   ///< Display magnitude in dB
  Phase,          ///< Display phase
  Smith,          ///< Display S data on Smith chart
  Polar,          ///< Display S data on polar plot
  PortImpedance,  ///< Display port impedance
  Stability,      ///< Display stability metrics
  VSWR,           ///< Display VSWR
  GroupDelay      ///< Display group delay
};

///
/// @struct TraceProperties
/// @brief Structure to hold all widgets related to a trace
///
struct TraceProperties {
  QLabel* nameLabel;             ///< Label displaying the trace name
  QSpinBox* width;               ///< Spin box for line width
  QPushButton* colorButton;      ///< Button for color selection
  QComboBox* LineStyleComboBox;  ///< Combo box for line style selection
  QToolButton* deleteButton;     ///< Button to delete the trace
  QString display_mode;          ///< Current display mode string
};


///
/// @struct LimitProperties
/// @brief Structure to hold all widgets related to a limit line
///
struct LimitProperties {
  QLabel* LimitLabel;                 ///< Label displaying the limit name
  QDoubleSpinBox* Start_Freq;         ///< Spin box for start frequency
  QDoubleSpinBox* Stop_Freq;          ///< Spin box for stop frequency
  QDoubleSpinBox* Start_Value;        ///< Spin box for start value of the limit line
  QDoubleSpinBox* Stop_Value;         ///< Spin box for stop value of the limit line
  QComboBox* Start_Freq_Scale;        ///< QComboBox for the scaling of the start frequency
  QComboBox* Stop_Freq_Scale;         ///< QComboBox for the scaling of the stop frequency
  QComboBox* axis;                    ///< Combo box for the y-axis selection
  QToolButton* Button_Delete_Limit;   ///< Button to delete the limit line
  QFrame* Separator;                  ///< Visual separator frame
  QPushButton* Couple_Value;          ///< Button to couple start/stop values
};

///
/// @struct TraceInfo
/// @brief Structure to hold trace identification and configuration information
///
struct TraceInfo {
  QString dataset;          ///< Dataset name the trace belongs to
  QString parameter;        ///< Parameter name (e.g., "S11", "S21")
  DisplayMode displayMode;  ///< Display mode for the trace

  ///
  /// @brief Generate a unique identifier for the trace
  /// @return QString Unique trace identifier
  ///
  QString uniqueId() const {
    if ((displayMode == DisplayMode::Magnitude_dB) ||
        (displayMode == DisplayMode::Phase)) {
      QString displayMode_str = QString::number(static_cast<int>(displayMode)); // Assign to variable to avoid warning
      return QString("%1.%2_%3")
          .arg(dataset, parameter, displayMode_str);
    } else {
      return QString("%1.%2").arg(dataset, parameter);
    }
  }

  ///
  /// @brief Get a human-readable name for the trace
  /// @return QString Human-readable trace name
  ///
  QString displayName() const {
    QString modeName = QString("");

    if (displayMode == DisplayMode::Magnitude_dB) {
      modeName = "dB";
    } else {
      if (displayMode == DisplayMode::Phase) {
        modeName = "Phase";
      } else {
        return QString("%1.%2").arg(dataset, parameter);
      }
    }
    return QString("%1.%2_%3").arg(dataset, parameter, modeName);
  }
};


///
/// @class Qucs_S_SPAR_Viewer
/// @brief Main application class for S-parameter viewer (and its RF circuit synthesis tools)
///
/// This class provides a comprehensive tool for:
/// - Loading and displaying S-parameter data from Touchstone files as well as from Qucs-S data files
/// - Visualizing data on multiple chart types (rectangular, Smith, polar)
/// - Markers and limit lines support
/// - RF circuit design synthesis
/// - Real-time file monitoring
///
class Qucs_S_SPAR_Viewer : public QMainWindow {
  Q_OBJECT
public:
  ///
  /// @brief Constructor for the main viewer window
  ///
  Qucs_S_SPAR_Viewer();

   ///
   /// @brief Destructor
   ///
  ~Qucs_S_SPAR_Viewer();

  ///
  /// @brief Add a directory path to the file watcher
  /// @param path Directory path to watch for changes
  ///
  void addPathToWatcher(const QString& path);

  ///
  /// @brief Add a file from QFileInfo (used by main program)
  /// @param fileInfo File information object containing the file to open
  /// @note The main qucs program uses this function to open a Touchstone file from the Project View
  ///
  void addFile(const QFileInfo& fileInfo);


  private slots:
  // Menu actions
  void slotHelpIntro();      ///< Show introduction help dialog
  void slotHelpAbout();      ///< Show about dialog
  void slotHelpAboutQt();    ///< Show Qt about dialog
  void slotQuit();           ///< Quit application
  void slotSave();           ///< Save current session
  void slotSaveAs();         ///< Save session to new file
  void slotLoadSession();    ///< Load a session file

  ///
  /// @brief Raise appropriate widgets when a tab is selected
  /// @param index Index of the selected tab
  ///
  void raiseWidgetsOnTabSelection(int index);


  // File management
  void addFile();                                    ///< Add a new file dialog
  void addFiles(QStringList);                        ///< Add multiple files
  void removeFile();                                 ///< Remove selected file
  void removeFile(QString ID);                       ///< Remove file by ID
  void removeAllFiles();                             ///< Remove all loaded files
  void removeTracesByDataset(const QString& dataset_to_remove);  ///< Remove all traces for a dataset


  ///
  /// @brief Read Qucsator dataset file
  /// @param filePath Path to the dataset file
  /// @return QMap containing the parsed data
  ///
  QMap<QString, QList<double>> readQucsatorDataset(const QString& filePath);

  ///
  /// @brief Read NGspice data file
  /// @param filePath Path to the data file
  /// @return QMap containing the parsed data
  ///
  QMap<QString, QList<double>> readNGspiceData(const QString& filePath);

  ///
  /// @brief Extract S-parameter indices from parameter string
  /// @param sparam S-parameter string (e.g., "S[1,2]")
  /// @return QString Extracted indices
  ///
  QString extractSParamIndices(const QString& sparam);

  ///
  /// @brief Apply default trace visualizations for loaded files
  /// @param fileNames List of file names to process
  ///
  void applyDefaultVisualizations(const QStringList& fileNames);

  ///
  /// @brief Add optional derived traces (impedance, stability, etc.)
  /// @param file_data Reference to file data map to add traces to
  ///
  void addOptionalTraces(QMap<QString, QList<double>>& file_data);

  void removeTraceByProps(DisplayMode mode, const QString& traceID,
                          TraceProperties& props);

  ///
  /// @brief Create file management widgets for a loaded file
  /// @param filename Name of the file
  /// @param position Position in the file list (0 = append)
  ///
  void CreateFileWidgets(QString filename, int position = 0);

  // File watching
  void setupFileWatcher();                    ///< Initialize file watching system
  void fileChanged(const QString& path);      ///< Handle file change event
  void directoryChanged(const QString& path); ///< Handle directory change event

  // Trace management
  ///
  /// \brief Add a new trace via dialog
  ///
  void addTrace();

  ///
  /// @brief Add a trace with specified properties
  /// @param traceInfo Trace information structure
  /// @param trace_color Color for the trace
  /// @param trace_width Line width for the trace
  /// @param trace_style Line style (default: "Solid")
  ///
  void addTrace(const TraceInfo& traceInfo, QColor trace_color, int trace_width,
                QString trace_style = "Solid");

  ///
  /// \brief Remove a trace via dialog
  ///
  void removeTrace();


  ///
  /// @brief Remove a row from grid layout and collapse remaining rows
  /// @param targetLayout Target grid layout
  /// @param row_to_remove Row index to remove
  ///
  void removeAndCollapseRow(QGridLayout* targetLayout, int row_to_remove);

  ///
  /// @brief Get the total number of traces
  /// @return int Number of traces
  ///
  int getNumberOfTraces();

  ///
  /// @brief Clean datasets from tools except specified one
  /// @param excludeDataset Dataset name to exclude from cleaning
  ///
  void cleanToolsDatasets(const QString& excludeDataset = QString());

  ///
  /// \brief Update the traces combo box
  ///
  void updateTracesCombo();

  ///
  /// \brief Update display type options
  ///
  void updateDisplayType();

  // Trace appearance
  void changeTraceColor();           ///< Change trace color
  void changeTraceLineStyle();       ///< Change trace line style
  void changeTraceWidth();           ///< Change trace width
  void changeMarkerLimits();         ///< Change marker limits
  void changeMarkerLimits(QString);  ///< Change marker limits for specific marker

  // Marker management
  ///
  /// @brief Add a new marker
  /// @param freq Frequency for the marker (-1 for default)
  /// @param Freq_Marker_Scale Frequency scale (default: "MHz")
  ///
  void addMarker(double freq = -1, QString Freq_Marker_Scale = QString("MHz"));

  ///
  /// \brief Remove selected marker (via dialog)
  ///
  void removeMarker();

  ///
  /// \brief Remove marker by name
  ///
  void removeMarker(const QString&);

  ///
  /// \brief Remove all markers
  ///
  void removeAllMarkers();

  ///
  /// \brief Get total number of markers
  /// \return int Total number of markers
  ///
  int getNumberOfMarkers();

  ///
  /// \brief Update marker table display
  ///
  void updateMarkerTable();

  ///
  /// \brief Update marker name labels
  ///
  void updateMarkerNames();

  ///
  /// @brief Update marker data in table
  /// @param layout Table widget to update
  /// @param mode Display mode
  /// @param header Header labels for the table
  ///
  void updateMarkerData(QTableWidget& layout, DisplayMode mode, QStringList header);

  ///
  /// @brief Get marker properties by position
  /// @param position Marker position
  /// @param outMarkerName Output parameter for marker name
  /// @param outProperties Output parameter for marker properties
  /// @return bool True if marker found
   ///
  bool getMarkerByPosition(int position, QString& outMarkerName, MarkerProperties& outProperties);

  // Limit management
  ///
  /// @brief Add a new limit line
  /// @param f_limit1 Start frequency (-1 for default)
  /// @param f_limit1_unit Start frequency unit
  /// @param f_limit2 Stop frequency (-1 for default)
  /// @param f_limit2_unit Stop frequency unit
  /// @param y_limit1 Start value (-1 for default)
  /// @param y_limit2 Stop value (-1 for default)
  /// @param coupled Whether start/stop values are coupled
  ///
  void addLimit(double f_limit1 = -1, QString f_limit1_unit = "",
                double f_limit2 = -1, QString f_limit2_unit = "",
                double y_limit1 = -1, double y_limit2 = -1,
                bool coupled = true);

  ///
  /// \brief Remove selected limit (via dialog)
  ///
  void onLimitDeleteClicked(bool);

  ///
  /// \brief Remove limit by name
  ///
  void removeLimit(QString);

  ///
  /// \brief Remove all limits
  ///
  void removeAllLimits();

  ///
  /// \brief Update limit displays
  ///
  void updateLimits();

  ///
  /// \brief Update the name of the limit lines
  ///
  void updateLimitNames();

  ///
  /// \brief Get the total number of limits
  /// \return int Number of limits
  ///
  int getNumberOfLimits();

  ///
  /// @brief Get limit properties by position
  /// @param position Limit position
  /// @param outLimitName Output parameter for limit name
  /// @param outProperties Output parameter for limit properties
  /// @return bool True if limit found
  ///
  bool getLimitByPosition(int, QString&, LimitProperties&);

  ///
  /// \brief Couple limit start/stop spinboxes
  ///
  void coupleSpinBoxes();

  ///
  /// @brief Update grid layout arrangement
  /// @param layout Grid layout to update
  ///
  void updateGridLayout(QGridLayout*);

  ///
  /// @brief Update all plots for a specific dataset
  /// @param datasetName Name of the dataset to update
  ///
  void updateAllPlots(const QString& datasetName);

  ///
  /// @brief Update traces in a specific widget
  /// @param widget Widget containing traces to update
  /// @param datasetName Dataset name to update
  ///
  void updateTracesInWidget(QWidget* widget, const QString& datasetName);

  ///
  /// @brief Calculate derived S-parameter trace
  /// @param file File/dataset name
  /// @param metric Metric to calculate (e.g., "K", "VSWR")
  ///
  void calculate_Sparameter_trace(QString, QString);

protected:
  ///
  /// \brief Handle drag enter event for file drop
  /// \param event Drag enter event
  ///
  void dragEnterEvent(QDragEnterEvent* event) override;

  ///
  /// \brief Handle drop event for file drop
  /// \param event Drop event
  ///
  void dropEvent(QDropEvent* event) override;

private:

  // UI Components - File Management
  QDockWidget* dockFiles;              ///< Dock widget for file management
  QTableWidget* spar_files_Widget;     ///< Table widget for file list
  QPushButton *Button_Add_File;        ///< Button to add files
  QPushButton *Delete_All_Files;       ///< Button to delete all files
  QList<QPushButton*> Button_DeleteFile;  ///< List of delete buttons
  QVBoxLayout* vLayout_Files;          ///< Vertical layout for files
  QWidget* FileList_Widget;            ///< Widget containing file list
  QGridLayout* FilesGrid;              ///< Grid layout for files
  QList<QLabel*> List_FileNames;       ///< List of file name labels
  QList<QToolButton*> List_RemoveButton;  ///< List of remove buttons

  // UI Components - Trace Management
  QDockWidget* dockTracesList;         ///< Dock widget for trace list
  QWidget* TracesList_Widget;          ///< Widget containing trace list
  QGridLayout* TracesGrid;             ///< Grid layout for traces

  ///
  /// \brief Based on the display mode (key), items groups all the widgets related to a trace (string)
  ///
  QMap<DisplayMode, QMap<QString, TraceProperties>> traceMap;

  QTabWidget* traceTabs;               ///< Tab widget for different trace types
  QWidget *magnitudePhaseTab;          ///< Tab for magnitude/phase traces
  QWidget *smithTab;                   ///< Tab for Smith chart traces
  QWidget *polarTab;                   ///< Tab for polar traces
  QWidget *portImpedanceTab;           ///< Tab for impedance traces
  QWidget *stabilityTab;               ///< Tab for stability traces
  QWidget *VSWRTab;                    ///< Tab for VSWR traces
  QWidget *GroupDelayTab;              ///< Tab for group delay traces

  QGridLayout *magnitudePhaseLayout;   ///< Layout for magnitude/phase tab
  QGridLayout *smithLayout;            ///< Layout for Smith chart tab
  QGridLayout *polarLayout;            ///< Layout for polar tab
  QGridLayout *portImpedanceLayout;    ///< Layout for impedance tab
  QGridLayout *stabilityLayout;        ///< Layout for stability tab
  QGridLayout *VSWRLayout;             ///< Layout for VSWR tab
  QGridLayout *GroupDelayLayout;       ///< Layout for group delay tab


  // Trace selection widgets
  QPushButton* Lock_axis_settings_Button;  ///< Button to lock axis settings
  bool lock_axis;                          ///< Axis lock state
  QStringList frequency_units;             ///< Available frequency units
  QComboBox *QCombobox_datasets;           ///< Dataset selection combo box
  QComboBox *QCombobox_display_mode;       ///< Display mode combo box
  MatrixComboBox* QCombobox_traces;        ///< Trace selection combo box
  QPushButton* Button_add_trace;           ///< Button to add trace
  QTableWidget* Traces_Widget;             ///< Table for trace display

  // Set-up Scrollable trace areas
  ///
  /// \brief Sets all the scrollable areas on a single call. Internally, it calls setupScrollAreaForLayout() for all the areas
  /// \see setupScrollAreaForLayout
  ///
  void setupScrollableLayout();

  ///
  /// \brief Given the layout and the parent, it sets up the scrollable areas
  /// \param layout
  /// \param parentTab
  /// \param objectName
  ///
  void setupScrollAreaForLayout(QGridLayout*& layout, QWidget* parentTab,
                                const QString& objectName);


  // Scroll areas
  QScrollArea *magnitudePhaseScrollArea;   ///< Scroll area for mag/phase
  QScrollArea *smithScrollArea;            ///< Scroll area for Smith chart
  QScrollArea *polarScrollArea;            ///< Scroll area for polar
  QScrollArea *nuScrollArea;               ///< Scroll area for impedance
  QScrollArea *GroupDelayScrollArea;       ///< Scroll area for group delay

  // Datasets
  ///
  /// \brief Based on the file name (key), it groups all the relevant traces
  ///
  ///
  QMap<QString, QMap<QString, QList<double>>> datasets;

  /* DATASET STRUCTURE
      KEY       |         DATA
  Filename1.s2p | {"freq", "S11_dB", ..., "S22_ang"}
      ...       |          ...
  Filenamek.s3p | {"freq", "S11_dB", ..., "S33_ang"}
  */

  // File watching
  QFileSystemWatcher* fileWatcher;          ///< File system watcher object
  QMap<QString, QString> watchedFilePaths;  ///< Relates the file name with a file path


  // Plot widgets
  // Magnitude plot
  RectangularPlotWidget* Magnitude_PhaseChart;  ///< Magnitude/phase chart
  QDockWidget* dockChart;                       ///< Dock for mag/phase chart
  double f_min, f_max;                          ///< Frequency range limits
  QList<QColor> default_colors;                 ///< Default trace colors
  QList<QGraphicsItem*> textLabels;             ///< Text labels on charts

  // Smith Chart
  SmithChartWidget* smithChart;                      ///< Smith chart widget
  QDockWidget* dockSmithChart;                       ///< Dock for Smith chart
  QList<SmithChartWidget::Trace> SmithChartTraces;   ///< Smith chart traces

  // Polar plot
  PolarPlotWidget* polarChart;                       ///< Polar plot widget
  QDockWidget* dockPolarChart;                       ///< Dock for polar plot
  QList<PolarPlotWidget::Trace> PolarChartTraces;    ///< Polar plot traces

  // Port impedance plot (Rectangular plot)
  RectangularPlotWidget* impedanceChart;                     ///< Impedance chart widget
  QDockWidget* dockImpedanceChart;                           ///< Dock for impedance chart
  QList<RectangularPlotWidget::Trace> impedanceChartTraces;  ///< Impedance traces

  // Stability plot (Rectangular plot)
  RectangularPlotWidget* stabilityChart;                     ///< Stability chart widget
  QDockWidget* dockStabilityChart;                           ///< Dock for stability chart
  QList<RectangularPlotWidget::Trace> stabilityChartTraces;  ///< Stability traces

  // VSWR plot (Rectangular plot)
  RectangularPlotWidget* VSWRChart;                     ///< VSWR chart widget
  QDockWidget* dockVSWRChart;                           ///< Dock for VSWR chart
  QList<RectangularPlotWidget::Trace> VSWRChartTraces;  ///< VSWR traces

  // Group delay plot (Rectangular plot)
  RectangularPlotWidget* GroupDelayChart;                ///< Group delay chart widget
  QDockWidget* dockGroupDelayChart;                      ///< Dock for group delay chart
  QList<RectangularPlotWidget::Trace> GroupDelayTraces;  ///< Group delay traces

  // Markers
  QDockWidget* dockMarkers;                ///< Dock for markers
  QWidget* Marker_Widget;                  ///< Marker widget container
  QGridLayout* MarkersGrid;                ///< Grid layout for markers
  QPushButton *Button_add_marker;          ///< Button to add marker
  QPushButton *Button_Remove_All_Markers;  ///< Button to remove all markers

  QTableWidget *tableMarkers_Magnitude_Phase;  ///< Marker table for mag/phase
  QTableWidget *tableMarkers_Smith;            ///< Marker table for Smith
  QTableWidget *tableMarkers_Polar;            ///< Marker table for polar
  QTableWidget *tableMarkers_PortImpedance;    ///< Marker table for impedance
  QTableWidget *tableMarkers_Stability;        ///< Marker table for stability
  QTableWidget *tableMarkers_VSWR;             ///< Marker table for VSWR
  QTableWidget *tableMarkers_GroupDelay;       ///< Marker table for group delay

  ///
  /// \brief All marker widgets are here. This way they can be accessed by name (map key)
  ///
  QMap<QString, MarkerProperties> markerMap;

  /**
   * @brief Get frequency value for a marker
   * @param markerName Name of the marker
   * @return double Frequency value
   */
  double getMarkerFreq(QString);

  // Limit widgets
  QDockWidget* dockLimits;                 ///< Dock for limits
  QWidget* Limits_Widget;                  ///< Limits widget container
  QGridLayout* LimitsGrid;                 ///< Grid layout for limits
  QPushButton *Button_add_Limit;           ///< Button to add limit
  QPushButton *Button_Remove_All_Limits;   ///< Button to remove all limits
  QDoubleSpinBox* Limits_Offset;           ///< Spin box for limit offset

  ///
  /// \brief Groups the widgets related to the traces. They are accessible by name (map key)
  ///
  QMap<QString, LimitProperties> limitsMap;

  // Tools
  QDockWidget* dockTools;                  ///< Dock for design tools
  GraphWidget* SchematicWidget;            ///< Schematic viewer widget
  QTabWidget* toolsTabWidget;              ///< Tab widget for the RF tools
  SimulationSetup* SimulationSetupWidget;  ///< Simulation setup widget

  ///
  /// \brief Circuit description object.
  /// \note It needs to be a member variable since the simulation can be triggered by a change in the simulation settings, i.e. in this case there's no SchematicContent object to emit
  ///
  SchematicContent Circuit;

  // Export widgets
  QLabel* LabelExportSchematic;            ///< Label for export
  QPushButton* ButtonExportSchematic;      ///< Export button
  QComboBox* ComboExportSchematic;         ///< Export format combo
  QComboBox* ComboExportOutputMethod;      ///< Export method combo

  ///
  /// \brief Update schematic content
  /// \note This is called by the updateSimulation() function
  ///
  void updateSchematicContent();

  ///
  /// \brief Triggers synthesis tools based on the tab selected
  /// \param visible [TO DO: See if this flag can be removed]
  ///
  void callTools(bool visible);

  ///
  /// \brief Handle tool tab change
  /// \param index int Tab index
  ///
  void onToolsTabChanged(int index);

  // Design tools
  QTabWidget* toolsTabs;                   ///< Tab widget for design tools

  ///
  /// \brief Datasets from design tools
  /// \note The synthesis tools create datasets, but they need to be removed when switching
  /// between tools. It makes no sense to keep, e.g. the filter dataset if the user decides to
  /// switch to the attenuator panel. This structure is used to see which datasets must be removed
  ///
  QStringList Tools_Datasets;
  FilterDesignTool* FilterTool;            ///< Filter design tool
  MatchingNetworkDesignTool* MatchingTool; ///< Matching network tool
  PowerCombiningTool* PowerCombTool;       ///< Power combining tool
  AttenuatorDesignTool* AttenuatorTool;    ///< Attenuator design tool

  // S-parameter simulation class
  SParameterCalculator SPAR_engine;

  // Substrate
  MS_Substrate MS_Subs;

  ///
  /// \brief Netlist editing tool
  ///
  NetlistScratchPad* Netlist_Tool;

  // Save
  QString savepath;

  ///
  /// \brief Save current session
  /// \return bool Status of the operation
  ///
  bool save();

  ///
  /// \brief Loads a previous session from a file
  /// \param path QString indicating the full path to the session file
  ///
  void loadSession(QString path);

  // Save rectangular plot settings
  void saveRectangularPlotSettings(QXmlStreamWriter& xml,
                                   RectangularPlotWidget* widget,
                                   const QString& elementName);
  void loadRectangularPlotSettings(QXmlStreamReader& xml,
                                   RectangularPlotWidget* widget,
                                   const QString& elementName);

  void saveSmithPlotSettings(QXmlStreamWriter& xml, SmithChartWidget* widget,
                             const QString& elementName);
  void loadSmithPlotSettings(QXmlStreamReader& xml, SmithChartWidget* widget,
                             const QString& elementName);

  void savePolarPlotSettings(QXmlStreamWriter& xml, PolarPlotWidget* widget,
                             const QString& elementName);
  void loadPolarPlotSettings(QXmlStreamReader& xml, PolarPlotWidget* widget,
                             const QString& elementName);

  // Notes
  QDockWidget* dockNotes;   ///< Code editor dock
  CodeEditor* Notes_Widget; ///< Code editor widget for notes

  // Recent files
  std::vector<QString> recentFiles;  ///< List of recent files
  QMenu* recentFilesMenu;            ///< Menu for recent files
  void updateRecentFilesMenu();      ///< Update recent files menu
  void loadRecentFiles();            ///< Load recent files list
  void addRecentFile(const QString&);  ///< Add file to recent list
  void clearRecentFiles();           ///< Clear recent files list
  void saveRecentFiles();            ///< Save recent files list

  // Setup UI
  void CreateMenuBar();              ///< Create menu bar
  void CreateDisplayWidgets();       ///< Create display widgets
  void CreateRightPanel();           ///< Create right panel
  void setFileManagementDock();      ///< Setup file management dock
  void setTraceManagementDock();     ///< Setup trace management dock
  void setMarkerManagementDock();    ///< Setup marker management dock
  void setLimitManagementDock();     ///< Setup limit management dock
  void setToolsDock();               ///< Setup tools dock

  // Utility methods
  ///
  /// @brief Get minimum and maximum values for a trace
  /// @param filename Dataset filename
  /// @param tracename Trace name
  /// @param minX Output minimum X value
  /// @param maxX Output maximum X value
  /// @param minY Output minimum Y value
  /// @param maxY Output maximum Y value
  ///
  void getMinMaxValues(QString, QString, qreal&, qreal&, qreal&, qreal&);

  ///
  /// \brief Adjust X axis to file data
  /// \param path to the file
  ///
  void adjust_x_axis_to_file(QString);
  void adjust_y_axis_to_trace(QString, QString);
  void adjust_x_axis_div();

  // File monitoring
  void setupSimulationWatcher();
  QStringList getWatchDirectories() const;
  bool isSparamFile(const QString& path); // Used to accept only data files when
                                          // scanning project directories
  QStringList filePaths; // Full path of the files in the progrom. It's used for
                         // file monitoring.

private slots:
  ///
  /// @brief Update simulation traces
  /// @param SI Schematic description
  ///
  void updateSimulation(SchematicContent SI);

  ///
  /// \brief Force simulation update
  ///
  void updateSimulation();

  ///
  /// \brief Update substrate parameters
  ///
  void updateSubstrate();

  ///
  /// \brief Export schematic (as text) to Qucs-S
  ///
  void exportSchematic();
};

#endif
