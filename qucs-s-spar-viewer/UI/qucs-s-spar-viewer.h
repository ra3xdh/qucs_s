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

/// @struct tQucsSettings
/// @brief Structure to hold Qucs application settings
struct tQucsSettings {
  int x, y;          ///< Position of main window (x, y coordinates)
  QFont font;        ///< Application font
  QString LangDir;   ///< Language directory path
  QString Language;  ///< Current language setting
};

extern struct tQucsSettings QucsSettings;

/// @struct MarkerProperties
/// @brief Structure to hold all widgets related to a marker
struct MarkerProperties {
  QLabel* nameLabel;           ///< Label displaying the marker name
  QDoubleSpinBox* freqSpinBox; ///< Spin box for frequency input
  QComboBox* scaleComboBox;    ///< Combo box for frequency scale selection
  QToolButton* deleteButton;   ///< Button to delete the marker
};

/// @enum DisplayMode
/// @brief Enumeration of available trace display modes
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

/// @struct TraceProperties
/// @brief Structure to hold all widgets related to a trace
struct TraceProperties {
  QLabel* nameLabel;             ///< Label displaying the trace name
  QSpinBox* width;               ///< Spin box for line width
  QPushButton* colorButton;      ///< Button for color selection
  QComboBox* LineStyleComboBox;  ///< Combo box for line style selection
  QToolButton* deleteButton;     ///< Button to delete the trace
  QString display_mode;          ///< Current display mode string
};


/// @struct LimitProperties
/// @brief Structure to hold all widgets related to a limit line
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

/// @struct TraceInfo
/// @brief Structure to hold trace identification and configuration information
struct TraceInfo {
  QString dataset;          ///< Dataset name the trace belongs to
  QString parameter;        ///< Parameter name (e.g., "S11", "S21")
  DisplayMode displayMode;  ///< Display mode for the trace

  /// @brief Generate a unique identifier for the trace
  /// @return QString Unique trace identifier
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

  /// @brief Get a human-readable name for the trace
  /// @return QString Human-readable trace name
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


/// @class Qucs_S_SPAR_Viewer
/// @brief Main application class for S-parameter viewer (and its RF circuit synthesis tools)
///
/// This class provides a comprehensive tool for:
/// - Loading and displaying S-parameter data from Touchstone files as well as from Qucs-S data files
/// - Visualizing data on multiple chart types (rectangular, Smith, polar)
/// - Markers and limit lines support
/// - RF circuit design synthesis
/// - Real-time file monitoring
class Qucs_S_SPAR_Viewer : public QMainWindow {
    Q_OBJECT
  public:
    /// @brief Constructor for the main viewer window
    Qucs_S_SPAR_Viewer();

     /// @brief Destructor
    ~Qucs_S_SPAR_Viewer();

    /// @brief Add a directory path to the file watcher
    /// @param path Directory path to watch for changes
    void addPathToWatcher(const QString& path);

    /// @brief Add a file from QFileInfo (used by main program)
    /// @param fileInfo File information object containing the file to open
    /// @note The main qucs program uses this function to open a Touchstone file from the Project View
    void addFile(const QFileInfo& fileInfo);


    private slots:
    // Menu actions
    /// @brief Show introduction help dialog
    ///
    /// Displays a message box with a short description of the program
    void slotHelpIntro();

    /// @brief Licensing and author credits
    /// @note This tool uses QCustomPlot, developed by Emanuel Eichhammer.
    /// https://www.qcustomplot.com
    void slotHelpAbout();

    /// @brief Information regarding the Qt libraries
    void slotHelpAboutQt();

    /// @brief Close the program
    void slotQuit() { qApp->quit(); }

    /// @brief Saves session to existing path or prompts for new path
    void slotSave();

    /// @brief Prompts user for save path and saves session
    /// @note The session file has .spar extension
    void slotSaveAs();

    /// @brief Opens file dialog to load a session file
    void slotLoadSession();

    /// @brief Raise appropriate widgets when trace tab is selected
    ///
    /// Synchronizes the trace tab selection with:
    /// - Active chart dock (raises matching chart)
    ///
    /// @param index Index of selected trace tab (0-6)
    ///
    /// Tab mapping:
    /// - 0: Magnitude/Phase → dB mode, S11
    /// - 1: Smith Chart → Smith mode, S11
    /// - 2: Polar → Polar mode, S11
    /// - 3: Port Impedance → n.u. mode, Re{Zin}
    /// - 4: Stability → n.u. mode
    /// - 5: VSWR → n.u. mode, VSWR{in}
    /// - 6: Group Delay → Group Delay mode
    void raiseWidgetsOnTabSelection(int index);

    // File management
    /// @brief Open file dialog to add files
    ///
    /// Displays a file selection dialog allowing the user to choose files.
    /// Supported formats:
    /// - Touchstone files (.s1p, .s2p, .s3p, .s4p)
    /// - Qucsator data files (.dat)
    /// - NGspice data files (.ngspice.dat)
    ///
    /// @see addFiles(QStringList)
    void addFile();

    /// @brief Add a single file using QFileInfo
    ///
    /// Wrapper function for adding a single file. Used when opening
    /// files from command line.
    ///
    /// @param fileInfo QFileInfo object containing file path information
    /// @note Validates file existence before attempting to load
    /// @note Shows warning dialog if file doesn't exist
    /// @note Internally calls addFiles(QStringList) with single-item list
    void addFiles(QStringList);

    /// @brief Remove file via UI
    /// Called when a remove button is clicked. Identifies which file
    /// to remove based on the sender's object name and calls removeFile(QString).
    /// @note This is a Qt slot connected to remove button click signals
    /// @note Gets the button ID from sender()->objectName()
    /// @see removeFile(QString)
    void removeFile();

    /// @brief Remove a specific file by its ID
    /// @param ID Unique identifier of the file to remove
    /// @note The ID corresponds to the button's objectName
    /// @note All traces associated with this dataset are automatically removed
    /// @note The datasets combo box is updated
    void removeFile(QString ID);

    /// @brief Remove all loaded files from the application
    /// @note This also removes all associated traces and markers
    /// @note File watcher paths are cleared to stop monitoring
    /// @see removeFile()
    void removeAllFiles();

    /// @brief Remove all traces associated with a dataset given its name (QString)
    ///
    /// Iterates through all display modes and removes any traces
    /// that belong to the specified dataset.
    ///
    /// @param dataset_to_remove Name of the dataset whose traces should be removed
    ///
    /// @note Uses removeTraceByProps() to handle the actual removal
    void removeTracesByDataset(const QString& dataset_to_remove);

    /// @brief Read Qucsator dataset file
    /// @param filePath Path to the dataset file
    /// @return QMap containing the parsed data
    QMap<QString, QList<double>> readQucsatorDataset(const QString& filePath);

    /// @brief Read NGspice data file
    /// @param filePath Path to the data file
    /// @return QMap containing the parsed data
    QMap<QString, QList<double>> readNGspiceData(const QString& filePath);

    /// @brief Extract S-parameter indices from parameter string
    /// @param sparam S-parameter string (e.g., "S[1,2]")
    /// @return QString Extracted indices
    QString extractSParamIndices(const QString& sparam);

    /// @brief Apply default trace visualizations for newly loaded files depending
    /// on their type
    ///
    /// Automatically creates appropriate default traces based on file
    /// characteristics:
    /// - Single S1P file: S11 in dB, Smith, Polar, impedance, and VSWR
    /// - Single S2P file: S21, S11, S22 in dB; S11, S22 in Smith and Polar;
    ///                    impedance, group delay, VSWR, and stability factors
    /// - Multiple S2P files: S21 in dB for each file with different colors
    ///
    /// @param fileNames List of file names that were loaded
    void applyDefaultVisualizations(const QStringList& fileNames);

    /// @brief Add optional traces depending on the number of ports of the data file
    ///
    /// Adds trace placeholders for derived parameters (i.e. computer from the
    /// S-data) based on number of ports:
    /// - 1-port: Re{Zin}, Im{Zin}, VSWR{in}
    /// - 2-port: delta, K, mu, mu_p, MSG, MAG, Re{Zin}, Im{Zin}, VSWR{in},
    ///           Re{Zout}, Im{Zout}, VSWR{out}
    ///
    /// @param file_data Reference to file data map to add trace placeholders to
    ///
    /// @note Traces are added as empty lists and populated when requested. It makes
    /// no sense to compute all of them without necessity
    void addOptionalTraces(QMap<QString, QList<double>>& file_data);

    void removeTraceByProps(DisplayMode mode, const QString& traceID,
                            TraceProperties& props);

    /// @brief Create file management widgets for a loaded file
    /// @param filename Name of the file
    /// @param position Position in the file list (0 = append)
    void CreateFileWidgets(QString filename, int position = 0);

    // File watching
    /// @brief Setup file system watcher
    ///
    /// Configures the file watcher to monitor:
    /// - Individual S-parameter files for changes
    /// - Directories containing watched files for additions/deletions
    void setupFileWatcher();

    /// @brief Handle file change event from file watcher
    ///
    /// Implements debouncing and robust file handling:
    /// 1. Debounces rapid file change events (500ms window)
    /// 2. Waits for file to be completely written (200ms delay + retry logic)
    /// 3. Reloads the file data using appropriate parser
    /// 4. Updates all plots using the new data
    /// 5. Re-adds file to watcher if needed
    ///
    /// @param path Full path to the changed file
    void fileChanged(const QString& path);

    /// @brief Handle directory change event from file watcher
    ///
    /// Monitors watched directories for new S-parameter files:
    /// 1. Scans directory for .snp, .dat, and .ngspice.dat files
    /// 2. Identifies files not already loaded
    /// 3. Presents dialog for user to select which new files to add
    /// 4. Loads selected files
    ///
    /// @param path Full path to the changed directory
    void directoryChanged(const QString& path); ///< Handle directory change event

    // Trace management
    /// @brief Add a new trace via dialog
    ///
    void addTrace();

    /// @brief Add a trace with specified properties
    /// @param traceInfo Trace information structure
    /// @param trace_color Color for the trace
    /// @param trace_width Line width for the trace
    /// @param trace_style Line style (default: "Solid")
    void addTrace(const TraceInfo& traceInfo, QColor trace_color, int trace_width,
                  QString trace_style = "Solid");

    /// @brief Remove a trace via dialog
    void removeTrace();

    /// @brief Remove all traces
    void removeAllTraces();

    /// @brief Remove all traces, markers and limits
    /// @note It also removes the widgets and the datasets created by the synthesis tools
    void removeAll();

    /// @brief Remove a row from grid layout and collapse remaining rows
    ///
    /// Performs three operations:
    /// 1. Removes and deletes all widgets in the target row
    /// 2. Shifts all rows below upward by one position
    /// 3. Forces layout update to remove empty space
    ///
    /// @param targetLayout Grid layout to modify
    /// @param row_to_remove Index of row to remove
    void removeAndCollapseRow(QGridLayout* targetLayout, int row_to_remove);

    /// @brief Get the total number of traces
    /// @return int Number of traces
    int getNumberOfTraces() { return traceMap.keys().size(); }

    /// @brief Clean datasets from tools except specified one
    /// @param excludeDataset Dataset name to exclude from cleaning
    void cleanToolsDatasets(const QString& excludeDataset = QString());

    /// @brief Update traces combo box based on selected dataset
    ///
    /// Populates the traces combo box with:
    /// - All S-parameters available for the current dataset (based on port count)
    /// - Additional derived parameters (stability factors, impedance, etc.)
    void updateTracesCombo();

    /// @brief Update available display types based on selected trace
    void updateDisplayType();

    // Trace appearance
    void changeTraceColor();           ///< Change trace color
    void changeTraceLineStyle();       ///< Change trace line style
    void changeTraceWidth();           ///< Change trace width

    /// @brief Update marker frequency limits when scale changes.
    ///
    /// @note Connected to marker scale combo box currentIndexChanged signal
    /// @note Extracts ID from sender's object name
    void changeMarkerLimits();

    /// @brief Update marker frequency spinbox limits based on selected scale
    ///
    /// Adjusts marker frequency spinbox min/max values when frequency unit changes:
    /// @param ID Object name of the combo box that triggered the change
    void changeMarkerLimits(QString);  ///< Change marker limits for specific marker

    // Marker management
    /// @brief Add a new marker
    /// @param freq Frequency for the marker (-1 for default)
    /// @param Freq_Marker_Scale Frequency scale (default: "MHz")
    void addMarker(double freq = -1, QString Freq_Marker_Scale = QString("MHz"));

    /// @brief Removes a marker via UI.
    /// Identifies the marker by the button's object name and removes it.
    /// This function is called when the user wants to remove a marker from the plot
    void removeMarker();

    /// @brief Removes a marker from the plot by name (QString).
    /// @param markerName QString The name of the marker to remove
    void removeMarker(const QString&);

    /// @brief Removes all markers from the plot.
    /// Iterates through all markers in reverse order and removes them.
    /// @note Calls removeMarker(QString) for all markers found
    /// @see removeMarker(QString)
    void removeAllMarkers();

    /// @brief Get total number of markers
    /// \return int Total number of markers
    int getNumberOfMarkers() { return markerMap.keys().size(); }

    /// @brief Updates all marker tables with current marker data.
    void updateMarkerTable();

    /// @brief Updates marker names after a marker is removed
    /// This is needed to keep consistent numbering.
    void updateMarkerNames();

    /// @brief Update marker data in table
    /// @param layout Table widget to update
    /// @param mode Display mode
    /// @param header Header labels for the table
    void updateMarkerData(QTableWidget& layout, DisplayMode mode, QStringList header);

    /// @brief Get marker properties by position
    /// @param position Marker position
    /// @param outMarkerName Output parameter for marker name
    /// @param outProperties Output parameter for marker properties
    /// @return bool True if marker found
    bool getMarkerByPosition(int position, QString& outMarkerName, MarkerProperties& outProperties);

    // Limit management
    /// @brief Add a new limit line
    /// @param f_limit1 Start frequency (-1 for default)
    /// @param f_limit1_unit Start frequency unit
    /// @param f_limit2 Stop frequency (-1 for default)
    /// @param f_limit2_unit Stop frequency unit
    /// @param y_limit1 Start value (-1 for default)
    /// @param y_limit2 Stop value (-1 for default)
    /// @param coupled Whether start/stop values are coupled
    void addLimit(double f_limit1 = -1, QString f_limit1_unit = "",
                  double f_limit2 = -1, QString f_limit2_unit = "",
                  double y_limit1 = -1, double y_limit2 = -1,
                  bool coupled = true);

    /// @brief Remove limit via UI
    ///
    /// Called when the user hits the delete button for a limit.
    /// Identifies the limit by button ID and removes it.
    ///
    /// @note Iterates through all limits to find matching button
    /// @note Calls removeLimit(QString) to perform actual removal
    /// @see removeLimit(QString)
    void onLimitDeleteClicked(bool);

    /// @brief Remove a specific limit by name
    ///
    /// Performs complete removal:
    /// 1. Retrieves limit properties from limitsMap
    /// 2. Deletes all associated widgets (11 total):
    ///    - axis selector, delete button, couple button
    ///    - label, separator
    ///    - start/stop frequency spinboxes and scale selectors
    ///    - start/stop value spinboxes
    /// 3. Removes entry from limitsMap
    /// 4. Removes limit line from the chart
    /// 5. Updates grid layout to remove gaps
    /// 6. Updates remaining limit names/numbers
    ///
    /// @param limit_to_remove Name of the limit to remove (e.g., "Limit 1")
    ///
    /// @note Layout is collapsed after removal
    /// @note Remaining limits are renumbered sequentially
    void removeLimit(QString);

    /// @brief Remove all limits from the chart
    ///
    /// Iterates through all limits and removes each one individually.
    ///
    /// @note Calls removeLimit() for each limit in the map
    /// @note Layout updates and name renumbering handled by removeLimit()
    /// @see removeLimit(QString)
    void removeAllLimits();

    /// @brief Update limit line visualization in the chart
    ///
    /// Called when any limit widget value changes. Performs:
    /// 1. Checks all limits for value coupling and synchronizes if needed
    /// 2. Identifies which limit triggered the update (from sender)
    /// 3. Extracts limit name from widget object name
    /// 4. Retrieves current widget values
    /// 5. Converts frequencies using appropriate scale factors
    /// 6. Updates the limit in the chart
    /// 7. Refreshes the chart display
    void updateLimits();

    /// @brief Update limit names after removal
    ///
    /// Renumbers all remaining limits sequentially (Limit 1, Limit 2, etc.)
    /// after a limit has been removed. This maintains consistent numbering
    /// without gaps.
    ///
    /// @note Called automatically after removeLimit()
    /// @note Only updates the label text, not the internal map keys
    /// @note Numbers start from 1 for display purposes
    void updateLimitNames();

    /// @brief Get the total number of limits
    /// \return int Number of limits
    int getNumberOfLimits() { return limitsMap.keys().size(); }

    /// @brief Get limit properties by index
    ///
    /// Retrieves limit information by its index in the limitsMap.
    /// Uses iterator advancement to access the map by position.
    ///
    /// @param position Zero-based index of the limit in the map
    /// @param outLimitName Output parameter for limit name
    /// @param outProperties Output parameter for limit properties structure
    /// @return bool True if position is valid and limit retrieved, false otherwise
    bool getLimitByPosition(int, QString&, LimitProperties&);

    /// @brief Toggle coupling between start and stop Y values
    ///
    /// Controls whether start and stop Y values are synchronized:
    /// - Coupled mode ("<--->"): Stop value matches start value, stop spinbox
    /// disabled
    /// - Uncoupled mode ("<-X->"): Values independent, both spinboxes enabled
    ///
    /// @note In coupled mode, stop value is set to match start value and disabled
    void coupleSpinBoxes();

    /// @brief Update grid layout by collapsing empty rows
    ///
    /// Collects all widget information, clears the layout, and re-adds
    /// widgets in a compact arrangement without gaps.
    ///
    /// @param layout Grid layout to update
    ///
    /// @note Preserves widget properties including alignment and span
    void updateGridLayout(QGridLayout*);

    /// @brief Update all plots for a specific dataset
    /// @param datasetName Name of the dataset to update
    void updateAllPlots(const QString& datasetName);

    /// @brief Update traces in a specific widget
    ///
    /// Handles the three widget types separatedly:
    /// - RectangularPlotWidget
    /// - PolarPlotWidget
    /// - SmithChartWidget
    ///
    /// Preserves trace visual properties (color, width, style) during update.
    ///
    /// @param widget Chart widget to update
    /// @param datasetName Dataset name containing the data
    void updateTracesInWidget(QWidget* widget, const QString& datasetName);

    /// @brief Calculate derived S-parameter trace
    /// @param file File/dataset name
    /// @param metric Metric to calculate (e.g., "K", "VSWR")
    void calculate_Sparameter_trace(QString, QString);

  protected:
    /// @brief Handle drag enter event for file drop
    /// \param event Drag enter event
    void dragEnterEvent(QDragEnterEvent* event) override;

    /// @brief Handle drop event for file drop
    /// \param event Drop event
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

    /// @brief Based on the display mode (key), items groups all the widgets related to a trace (string)
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
    QPushButton* Button_Remove_all;          ///< Remove all traces, markers and limits
    QTableWidget* Traces_Widget;             ///< Table for trace display

    // Set-up Scrollable trace areas
    /// @brief Sets all the scrollable areas on a single call. Internally, it calls setupScrollAreaForLayout() for all the areas
    /// \see setupScrollAreaForLayout
    void setupScrollableLayout();

    /// @brief Given the layout and the parent, it sets up the scrollable areas
    /// \param layout
    /// \param parentTab
    /// \param objectName
    void setupScrollAreaForLayout(QGridLayout*& layout, QWidget* parentTab,
                                  const QString& objectName);


    // Scroll areas
    QScrollArea *magnitudePhaseScrollArea;   ///< Scroll area for mag/phase
    QScrollArea *smithScrollArea;            ///< Scroll area for Smith chart
    QScrollArea *polarScrollArea;            ///< Scroll area for polar
    QScrollArea *nuScrollArea;               ///< Scroll area for impedance
    QScrollArea *GroupDelayScrollArea;       ///< Scroll area for group delay

    // Datasets
    /// @brief Based on the file name (key), it groups all the relevant traces
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

    /// @brief All marker widgets are here. This way they can be accessed by name (map key)
    QMap<QString, MarkerProperties> markerMap;

    /// @brief Gets the marker frequency (in Hz) given the marker name.
    ///
    /// @param markerName The name of the marker
    /// @return The frequency in Hz, or 0.0 if marker not found
    double getMarkerFreq(QString);

    // Limit widgets
    QDockWidget* dockLimits;                 ///< Dock for limits
    QWidget* Limits_Widget;                  ///< Limits widget container
    QGridLayout* LimitsGrid;                 ///< Grid layout for limits
    QPushButton *Button_add_Limit;           ///< Button to add limit
    QPushButton *Button_Remove_All_Limits;   ///< Button to remove all limits
    QDoubleSpinBox* Limits_Offset;           ///< Spin box for limit offset

    /// @brief Groups the widgets related to the traces. They are accessible by name (map key)
    QMap<QString, LimitProperties> limitsMap;

    // Tools
    QDockWidget* dockTools;                  ///< Dock for design tools
    GraphWidget* SchematicWidget;            ///< Schematic viewer widget
    QTabWidget* toolsTabWidget;              ///< Tab widget for the RF tools
    SimulationSetup* SimulationSetupWidget;  ///< Simulation setup widget

    /// @brief Circuit description object.
    /// \note It needs to be a member variable since the simulation can be triggered by a change in the simulation settings, i.e. in this case there's no SchematicContent object to emit
    SchematicContent Circuit;

    // Export widgets
    QLabel* LabelExportSchematic;            ///< Label for export
    QPushButton* ButtonExportSchematic;      ///< Export button
    QComboBox* ComboExportSchematic;         ///< Export format combo
    QComboBox* ComboExportOutputMethod;      ///< Export method combo

    /// @brief Update schematic content
    /// \note This is called by the updateSimulation() function
    void updateSchematicContent();

    /// @brief Trigger synthesis when a design tool is selected
    ///
    /// Automatically runs the synthesis/design function for the currently
    /// selected tool when the tools dock becomes visible.
    ///
    /// @param visible True if tools dock is now visible (TO DO: Review if this is
    /// needed)
    /// @todo Can the visible bool be removed?
    void callTools(bool visible);

    /// @brief Handle tool tab change
    /// \param index int Tab index
    void onToolsTabChanged(int index);

    // Design tools
    QTabWidget* toolsTabs;                   ///< Tab widget for design tools

    /// @brief Datasets from design tools
    /// \note The synthesis tools create datasets, but they need to be removed when switching
    /// between tools. It makes no sense to keep, e.g. the filter dataset if the user decides to
    /// switch to the attenuator panel. This structure is used to see which datasets must be removed
    QStringList Tools_Datasets;              ///< Datasets created by the synthesis tools
    FilterDesignTool* FilterTool;            ///< Filter design tool
    MatchingNetworkDesignTool* MatchingTool; ///< Matching network tool
    PowerCombiningTool* PowerCombTool;       ///< Power combining tool
    AttenuatorDesignTool* AttenuatorTool;    ///< Attenuator design tool

    // S-parameter simulation class
    SParameterCalculator SPAR_engine;

    // Substrate
    MS_Substrate MS_Subs;

    /// @brief Netlist editing tool
    NetlistScratchPad* Netlist_Tool;

    // Save
    QString savepath;

    /// @brief Saves current session to file
    /// @return True if save succeeded, false otherwise
    bool save();

    /// @brief Loads session from specified file
    /// @param session_file Path to the session file
    /// @note This is used by slotLoadSession()
    /// @see slotLoadSession()
    void loadSession(QString path);

    /// @brief Saves rectangular plot settings to XML
    /// @param xml XML stream writer
    /// @param widget Rectangular plot widget
    /// @param elementName XML element name
    /// @note Called from save()
    /// @see save()
    void saveRectangularPlotSettings(QXmlStreamWriter& xml,
                                     RectangularPlotWidget* widget,
                                     const QString& elementName);

    /// @brief Loads rectangular plot settings from XML
    /// @param xml XML stream reader
    /// @param widget Rectangular plot widget
    /// @param elementName XML element name
    /// @note Called from loadSession()
    /// @see loadSession()
    void loadRectangularPlotSettings(QXmlStreamReader& xml,
                                     RectangularPlotWidget* widget,
                                     const QString& elementName);

    /// @brief Saves Smith chart settings to XML
    /// @param xml XML stream writer
    /// @param widget Smith chart widget
    /// @param elementName XML element name
    /// @note Called from save()
    /// @see save()
    void saveSmithPlotSettings(QXmlStreamWriter& xml, SmithChartWidget* widget,
                               const QString& elementName);

    /// @brief Loads Smith chart settings from XML
    /// @param xml XML stream reader
    /// @param widget Smith chart widget
    /// @param elementName XML element name
    /// @note Called from loadSession()
    /// @see loadSession()
    void loadSmithPlotSettings(QXmlStreamReader& xml, SmithChartWidget* widget,
                               const QString& elementName);

    /// @brief Saves polar plot settings to XML
    /// @param xml XML stream writer
    /// @param widget Polar plot widget
    /// @param elementName XML element name
    /// @note Called from save()
    /// @see save()
    void savePolarPlotSettings(QXmlStreamWriter& xml, PolarPlotWidget* widget,
                               const QString& elementName);

    /// @brief Loads polar plot settings from XML
    /// @param xml XML stream reader
    /// @param widget Polar plot widget
    /// @param elementName XML element name
    /// @note Called from loadSession()
    /// @see loadSession()
    void loadPolarPlotSettings(QXmlStreamReader& xml, PolarPlotWidget* widget,
                               const QString& elementName);

    // Notes
    QDockWidget* dockNotes;   ///< Code editor dock
    CodeEditor* Notes_Widget; ///< Code editor widget for notes

    // Recent files
    std::vector<QString> recentFiles;  ///< List of recent files
    QMenu* recentFilesMenu;            ///< Menu for recent files

    /// @brief Updates recent files menu with current list
    void updateRecentFilesMenu();

    /// @brief Loads recent files list from QSettings
    /// @note This is called when the program starts up
    void loadRecentFiles();

    /// @brief Adds file to recent files list (max 10 entries)
    /// @param filePath Path to add to recent files
    void addRecentFile(const QString&);

    /// @brief Clears the recent files list
    void clearRecentFiles(){ recentFiles.clear(); }

    /// @brief Saves recent files list to QSettings
    /// @note This is called when the program is about to close
    void saveRecentFiles();

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
    /// @brief Get minimum and maximum values for a trace
    /// @param filename Dataset filename
    /// @param tracename Trace name
    /// @param minX Output minimum X value
    /// @param maxX Output maximum X value
    /// @param minY Output minimum Y value
    /// @param maxY Output maximum Y value
    void getMinMaxValues(QString, QString, qreal&, qreal&, qreal&, qreal&);

    /// @brief Adjust X axis to file data
    /// \param path to the file
    void adjust_x_axis_to_file(QString);
    void adjust_y_axis_to_trace(QString, QString);
    void adjust_x_axis_div();

    // File monitoring
    void setupSimulationWatcher();
    QStringList getWatchDirectories() const;

    /// @brief Check if a file is an S-parameter data file
    ///
    /// @param path Full file path to check
    /// @return bool True if file is S-parameter format, false otherwise
    /// @note It's used to accept only data files when scenning project directories
    bool isSparamFile(const QString& path);

    QStringList filePaths; // Full path of the files in the progrom. It's used for
                           // file monitoring.

  private slots:
    /// @brief Update simulation traces
    /// @param SI Schematic description
    void updateSimulation(SchematicContent SI);

    /// @brief Force simulation update
    void updateSimulation();

    /// @brief Update substrate parameters
    void updateSubstrate();

    /// @brief Export schematic (as text) to Qucs-S
    void exportSchematic();
};

#endif
