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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qucs-s-spar-viewer.h"

#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QValidator>

Qucs_S_SPAR_Viewer::Qucs_S_SPAR_Viewer() {
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  centralWidget->setMaximumWidth(0); // Minimize central widget size

  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle(
      "Qucs S-parameter Viewer & RF Circuit Synthesis " PACKAGE_VERSION);

  CreateMenuBar();

  // Set frequency units
  frequency_units << "Hz" << "kHz" << "MHz" << "GHz";

  // These are two maximum markers to find the lowest and the highest frequency
  // in the data samples. They are used to prevent the user from zooming out too
  // much
  f_min = 1e20;
  f_max = -1;

  // Load default colors
  default_colors.append(QColor(Qt::red));
  default_colors.append(QColor(Qt::blue));
  default_colors.append(QColor(Qt::darkGreen));
  default_colors.append(QColor(Qt::black));
  default_colors.append(QColor(Qt::darkCyan));
  default_colors.append(QColor(Qt::darkYellow));
  default_colors.append(QColor(Qt::darkMagenta));

  CreateDisplayWidgets();
  CreateRightPanel();

  // Initialize file watcher
  fileWatcher = new QFileSystemWatcher(this);
  connect(fileWatcher, &QFileSystemWatcher::fileChanged, this,
          &Qucs_S_SPAR_Viewer::fileChanged);
  connect(fileWatcher, &QFileSystemWatcher::directoryChanged, this,
          &Qucs_S_SPAR_Viewer::directoryChanged);

  // Put the following widgets on the top to make them visible to the user
  dockFiles->raise();
  dockChart->raise();

  setDockNestingEnabled(true);
  setAcceptDrops(true); // Enable drag and drop feature to open files
  loadRecentFiles();    // Load "Recent Files" list
}

void Qucs_S_SPAR_Viewer::CreateMenuBar() {
  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction *fileQuit = new QAction(tr("&Quit"), this);
  fileQuit->setShortcut(QKeySequence::Quit);
  connect(fileQuit, SIGNAL(triggered(bool)), SLOT(slotQuit()));

  QAction *fileOpenSession = new QAction(tr("&Open session file"), this);
  fileOpenSession->setShortcut(QKeySequence::Open);
  connect(fileOpenSession, SIGNAL(triggered(bool)), SLOT(slotLoadSession()));

  QAction *fileSaveAsSession = new QAction(tr("&Save session as ..."), this);
  fileSaveAsSession->setShortcut(QKeySequence::SaveAs);
  connect(fileSaveAsSession, SIGNAL(triggered(bool)), SLOT(slotSaveAs()));

  QAction *fileSaveSession = new QAction(tr("&Save session"), this);
  fileSaveSession->setShortcut(QKeySequence::Save);
  connect(fileSaveSession, SIGNAL(triggered(bool)), SLOT(slotSave()));

  recentFilesMenu = fileMenu->addMenu("Recent Files");
  connect(recentFilesMenu, &QMenu::aboutToShow, this,
          &Qucs_S_SPAR_Viewer::updateRecentFilesMenu);

  fileMenu->addAction(fileOpenSession);
  fileMenu->addAction(fileSaveSession);
  fileMenu->addAction(fileSaveAsSession);
  fileMenu->addAction(fileQuit);

  QMenu *helpMenu = new QMenu(tr("&Help"));

  QAction *helpHelp = new QAction(tr("&Help"), this);
  helpHelp->setShortcut(Qt::Key_F1);
  helpMenu->addAction(helpHelp);
  connect(helpHelp, SIGNAL(triggered(bool)), SLOT(slotHelpIntro()));

  QAction *helpAbout = new QAction(tr("&About"), this);
  helpMenu->addAction(helpAbout);
  connect(helpAbout, SIGNAL(triggered(bool)), SLOT(slotHelpAbout()));

  helpMenu->addSeparator();

  QAction *helpAboutQt = new QAction(tr("About Qt..."), this);
  helpMenu->addAction(helpAboutQt);
  connect(helpAboutQt, SIGNAL(triggered(bool)), SLOT(slotHelpAboutQt()));

  menuBar()->addMenu(fileMenu);
  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);
}

// This function populates the left panel with the following widgets:
// - Files manager
// - Traces manager
// - Markers maanger
// - Limits manager
// - Notebook

void Qucs_S_SPAR_Viewer::CreateRightPanel() {
  // Create left panel widgets
  setFileManagementDock();
  setTraceManagementDock();
  setMarkerManagementDock();
  setLimitManagementDock();
  setToolsDock();

  // Notes
  Notes_Widget = new CodeEditor();
  dockNotes = new QDockWidget("Notes", this);
  dockNotes->setObjectName("dockNotes");
  dockNotes->setWidget(Notes_Widget);

  // Disable dock closing
  dockFiles->setFeatures(dockFiles->features() &
                         ~QDockWidget::DockWidgetClosable);
  dockTracesList->setFeatures(dockTracesList->features() &
                              ~QDockWidget::DockWidgetClosable);
  dockMarkers->setFeatures(dockMarkers->features() &
                           ~QDockWidget::DockWidgetClosable);
  dockLimits->setFeatures(dockLimits->features() &
                          ~QDockWidget::DockWidgetClosable);
  dockTools->setFeatures(dockTools->features() &
                         ~QDockWidget::DockWidgetClosable);
  dockNotes->setFeatures(dockNotes->features() &
                         ~QDockWidget::DockWidgetClosable);

  // Add all panel docks to the right area
  addDockWidget(Qt::RightDockWidgetArea, dockFiles);
  addDockWidget(Qt::RightDockWidgetArea, dockTracesList);
  addDockWidget(Qt::RightDockWidgetArea, dockMarkers);
  addDockWidget(Qt::RightDockWidgetArea, dockLimits);
  addDockWidget(Qt::RightDockWidgetArea, dockTools);
  addDockWidget(Qt::RightDockWidgetArea, dockNotes);

  // Tabify the panel docks
  tabifyDockWidget(dockFiles, dockTracesList);
  tabifyDockWidget(dockTracesList, dockMarkers);
  tabifyDockWidget(dockMarkers, dockLimits);
  tabifyDockWidget(dockLimits, dockTools);
  tabifyDockWidget(dockTools, dockNotes);

  // Remove the tabify between chart docks as it's already done in
  // CreateDisplayWidgets tabifyDockWidget(dockChart, dockSmithChart);

  // To prevent the gap between left and right dock areas, we need to resize the
  // dock widgets This should be called after all dock widgets are set up,
  // perhaps in a separate method
  resizeDocks({dockChart, dockSmithChart}, {width() / 2, width() / 2},
              Qt::Horizontal);
  resizeDocks({dockFiles, dockTracesList, dockMarkers, dockLimits, dockNotes},
              {width() / 4, width() / 4, width() / 4, width() / 4, width() / 4},
              Qt::Horizontal);
}

void Qucs_S_SPAR_Viewer::setFileManagementDock() {

  dockFiles = new QDockWidget("S-parameter files", this);
  dockFiles->setObjectName("dockFiles");

  QScrollArea *scrollArea_Files = new QScrollArea();
  FileList_Widget = new QWidget();
  QWidget *FilesGroup = new QWidget();

  FilesGrid = new QGridLayout(FileList_Widget);

  vLayout_Files = new QVBoxLayout(FilesGroup);

  QWidget *Buttons = new QWidget();
  QHBoxLayout *hLayout_Files_Buttons = new QHBoxLayout(Buttons);

  Button_Add_File = new QPushButton("Add file", this);
  Button_Add_File->setStyleSheet("QPushButton {background-color: green;\
                                  border-style: outset;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  color: white;\
                                  min-width: 10em;\
                                  padding: 6px;\
                              }");
  QString tooltip_message = QString(
      "Add single data file (.dat, .snp). You can also drag and drop it.");
  Button_Add_File->setToolTip(tooltip_message);
  connect(Button_Add_File, SIGNAL(clicked()), SLOT(addFile()));

  Delete_All_Files = new QPushButton("Delete all", this);
  Delete_All_Files->setStyleSheet("QPushButton {background-color: red;\
                                  border-style: outset;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  color: white;\
                                  min-width: 10em;\
                                  padding: 6px;\
                              }");
  tooltip_message = QString("Remove all data files.");
  Button_Add_File->setToolTip(tooltip_message);
  connect(Delete_All_Files, SIGNAL(clicked()), SLOT(removeAllFiles()));

  hLayout_Files_Buttons->addWidget(Button_Add_File);
  hLayout_Files_Buttons->addWidget(Delete_All_Files);

  scrollArea_Files->setWidget(FileList_Widget);
  scrollArea_Files->setWidgetResizable(true);
  vLayout_Files->addWidget(scrollArea_Files, Qt::AlignTop);
  vLayout_Files->addWidget(Buttons, Qt::AlignBottom);
  vLayout_Files->setStretch(0, 3);
  vLayout_Files->setStretch(1, 1);

  dockFiles->setWidget(FilesGroup);
}

void Qucs_S_SPAR_Viewer::setTraceManagementDock() {

  dockTracesList = new QDockWidget("Traces List", this);
  dockTracesList->setObjectName("TracesDock");

  QWidget *TracesGroup = new QWidget();
  QVBoxLayout *Traces_VBox = new QVBoxLayout(TracesGroup);

  // Trace addition box
  QWidget *TraceSelection_Widget = new QWidget(); // Add trace

  QGridLayout *DatasetsGrid = new QGridLayout(TraceSelection_Widget);
  QLabel *dataset_label = new QLabel("<b>Dataset</b>");
  DatasetsGrid->addWidget(dataset_label, 0, 0, Qt::AlignCenter);

  QLabel *Traces_label = new QLabel("<b>Traces</b>");
  DatasetsGrid->addWidget(Traces_label, 0, 1, Qt::AlignCenter);

  QLabel *displayTypeLabel = new QLabel("<b>Display Type</b>");
  DatasetsGrid->addWidget(displayTypeLabel, 0, 2, Qt::AlignCenter);

  QCombobox_traces = new MatrixComboBox();
  connect(QCombobox_traces, SIGNAL(currentIndexChanged(int)),
          SLOT(updateDisplayType()));
  DatasetsGrid->addWidget(QCombobox_traces, 1, 1);

  Button_add_trace = new QPushButton("Add trace");
  Button_add_trace->setStyleSheet("QPushButton {background-color: green;\
                                  border-style: outset;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  color: white;\
                                  min-width: 10em;\
                                  padding: 6px;\
                              }");
  connect(Button_add_trace, SIGNAL(clicked()),
          SLOT(addTrace())); // Connect button with the handler

  QCombobox_display_mode = new QComboBox();
  QCombobox_display_mode->addItem("dB");
  QCombobox_display_mode->addItem("Phase");
  QCombobox_display_mode->addItem("Smith");
  QCombobox_display_mode->addItem("Polar");
  QCombobox_display_mode->addItem("Group Delay");
  QCombobox_display_mode->addItem("n.u.");
  QCombobox_display_mode->setCurrentIndex(0); // Default to dB
  QCombobox_display_mode->setObjectName("DisplayTypeCombo");
  DatasetsGrid->addWidget(QCombobox_display_mode, 1, 2);

  DatasetsGrid->addWidget(Button_add_trace, 1, 3);

  QCombobox_datasets = new QComboBox();
  DatasetsGrid->addWidget(QCombobox_datasets, 1, 0);
  connect(QCombobox_datasets, SIGNAL(currentIndexChanged(int)),
          SLOT(updateTracesCombo())); // Each time the dataset is changed it is
                                      // needed to update the traces combo. This
                                      // is needed when the user has data with
                                      // different number of ports.
  traceTabs = new QTabWidget(this);   // Ensure 'this' is the parent
  connect(traceTabs, SIGNAL(currentChanged(int)), this,
          SLOT(raiseWidgetsOnTabSelection(int)));

  // Create tabs for Magnitude/Phase and Smith Chart
  magnitudePhaseTab = new QWidget(traceTabs); // Parent is traceTabs
  smithTab = new QWidget(traceTabs);          // Parent is traceTabs
  polarTab = new QWidget(traceTabs);          // Parent is traceTabs
  portImpedanceTab = new QWidget(traceTabs);  // Parent is traceTabs
  stabilityTab = new QWidget(traceTabs);      // Parent is traceTabs
  VSWRTab = new QWidget(traceTabs);           // Parent is traceTabs
  GroupDelayTab = new QWidget(traceTabs);     // Parent is traceTabs

  // Add tabs to the tab widget
  traceTabs->addTab(magnitudePhaseTab, "Magnitude/Phase");
  traceTabs->addTab(smithTab, "Smith");
  traceTabs->addTab(polarTab, "Polar");
  traceTabs->addTab(portImpedanceTab, "Port Impedance");
  traceTabs->addTab(stabilityTab, "Stability");
  traceTabs->addTab(VSWRTab, "VSWR");
  traceTabs->addTab(GroupDelayTab, "Group Delay");

  // Create layouts for each tab
  magnitudePhaseLayout = new QGridLayout(magnitudePhaseTab);
  smithLayout = new QGridLayout(smithTab);
  polarLayout = new QGridLayout(polarTab);
  portImpedanceLayout = new QGridLayout(portImpedanceTab);
  stabilityLayout = new QGridLayout(stabilityTab);
  VSWRLayout = new QGridLayout(VSWRTab);
  GroupDelayLayout = new QGridLayout(GroupDelayTab);

  // Set the layouts on the tabs
  magnitudePhaseTab->setLayout(magnitudePhaseLayout);
  smithTab->setLayout(smithLayout);
  polarTab->setLayout(polarLayout);
  portImpedanceTab->setLayout(portImpedanceLayout);
  stabilityTab->setLayout(stabilityLayout);
  VSWRTab->setLayout(VSWRLayout);
  GroupDelayTab->setLayout(GroupDelayLayout);

  // Set Magnitude tab
  QLabel *Label_Name_mag = new QLabel("<b>Name</b>");
  QLabel *Label_Color_mag = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle_mag = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth_mag = new QLabel("<b>Width</b>");
  QLabel *Label_Remove_mag = new QLabel("<b>Remove</b>");

  magnitudePhaseLayout->addWidget(Label_Name_mag, 0, 0, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_Color_mag, 0, 1, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_LineStyle_mag, 0, 2, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_LineWidth_mag, 0, 3, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_Remove_mag, 0, 4, Qt::AlignCenter);

  // Set Smith tab
  QLabel *Label_Name_Smith = new QLabel("<b>Name</b>");
  QLabel *Label_Color_Smith = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle_Smith = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth_Smith = new QLabel("<b>Width</b>");
  QLabel *Label_Remove_Smith = new QLabel("<b>Remove</b>");

  smithLayout->addWidget(Label_Name_Smith, 0, 0, Qt::AlignCenter);
  smithLayout->addWidget(Label_Color_Smith, 0, 1, Qt::AlignCenter);
  smithLayout->addWidget(Label_LineStyle_Smith, 0, 2, Qt::AlignCenter);
  smithLayout->addWidget(Label_LineWidth_Smith, 0, 3, Qt::AlignCenter);
  smithLayout->addWidget(Label_Remove_Smith, 0, 4, Qt::AlignCenter);

  // Set Polar tab
  QLabel *Label_Name_Polar = new QLabel("<b>Name</b>");
  QLabel *Label_Color_Polar = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle_Polar = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth_Polar = new QLabel("<b>Width</b>");
  QLabel *Label_Remove_Polar = new QLabel("<b>Remove</b>");

  polarLayout->addWidget(Label_Name_Polar, 0, 0, Qt::AlignCenter);
  polarLayout->addWidget(Label_Color_Polar, 0, 1, Qt::AlignCenter);
  polarLayout->addWidget(Label_LineStyle_Polar, 0, 2, Qt::AlignCenter);
  polarLayout->addWidget(Label_LineWidth_Polar, 0, 3, Qt::AlignCenter);
  polarLayout->addWidget(Label_Remove_Polar, 0, 4, Qt::AlignCenter);

  // Set "Port Impedance" tab
  QLabel *Label_Name_nu = new QLabel("<b>Name</b>");
  QLabel *Label_Color_nu = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle_nu = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth_nu = new QLabel("<b>Width</b>");
  QLabel *Label_Remove_nu = new QLabel("<b>Remove</b>");

  portImpedanceLayout->addWidget(Label_Name_nu, 0, 0, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_Color_nu, 0, 1, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_LineStyle_nu, 0, 2, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_LineWidth_nu, 0, 3, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_Remove_nu, 0, 4, Qt::AlignCenter);

  // Set "Stability" tab
  QLabel *Label_Name_stab = new QLabel("<b>Name</b>");
  QLabel *Label_Color_stab = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle_stab = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth_stab = new QLabel("<b>Width</b>");
  QLabel *Label_Remove_stab = new QLabel("<b>Remove</b>");

  stabilityLayout->addWidget(Label_Name_stab, 0, 0, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_Color_stab, 0, 1, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_LineStyle_stab, 0, 2, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_LineWidth_stab, 0, 3, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_Remove_stab, 0, 4, Qt::AlignCenter);

  // Set VSWR tab
  QLabel *Label_Name_VSWR = new QLabel("<b>Name</b>");
  QLabel *Label_Color_VSWR = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle_VSWR = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth_VSWR = new QLabel("<b>Width</b>");
  QLabel *Label_Remove_VSWR = new QLabel("<b>Remove</b>");

  VSWRLayout->addWidget(Label_Name_VSWR, 0, 0, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_Color_VSWR, 0, 1, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_LineStyle_VSWR, 0, 2, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_LineWidth_VSWR, 0, 3, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_Remove_VSWR, 0, 4, Qt::AlignCenter);

  // Set "Group delay" tab
  QLabel *Label_Name_GD = new QLabel("<b>Name</b>");
  QLabel *Label_Color_GD = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle_GD = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth_GD = new QLabel("<b>Width</b>");
  QLabel *Label_Remove_GD = new QLabel("<b>Remove</b>");

  GroupDelayLayout->addWidget(Label_Name_GD, 0, 0, Qt::AlignCenter);
  GroupDelayLayout->addWidget(Label_Color_GD, 0, 1, Qt::AlignCenter);
  GroupDelayLayout->addWidget(Label_LineStyle_GD, 0, 2, Qt::AlignCenter);
  GroupDelayLayout->addWidget(Label_LineWidth_GD, 0, 3, Qt::AlignCenter);
  GroupDelayLayout->addWidget(Label_Remove_GD, 0, 4, Qt::AlignCenter);

  setupScrollableLayout();

  Traces_VBox->addWidget(TraceSelection_Widget);
  Traces_VBox->addWidget(traceTabs);

  // Trace management
  // Titles
  TracesList_Widget = new QWidget(); // Panel with the trace settings
  QLabel *Label_Name = new QLabel("<b>Name</b>");
  QLabel *Label_Color = new QLabel("<b>Color</b>");
  QLabel *Label_LineStyle = new QLabel("<b>Line Style</b>");
  QLabel *Label_LineWidth = new QLabel("<b>Width</b>");
  QLabel *Label_Remove = new QLabel("<b>Remove</b>");

  TracesGrid = new QGridLayout(TracesList_Widget);
  TracesGrid->addWidget(Label_Name, 0, 0, Qt::AlignCenter);
  TracesGrid->addWidget(Label_Color, 0, 1, Qt::AlignCenter);
  TracesGrid->addWidget(Label_LineStyle, 0, 2, Qt::AlignCenter);
  TracesGrid->addWidget(Label_LineWidth, 0, 3, Qt::AlignCenter);
  TracesGrid->addWidget(Label_Remove, 0, 4, Qt::AlignCenter);

  dockTracesList->setWidget(TracesGroup);
}

void Qucs_S_SPAR_Viewer::setMarkerManagementDock() {
  // Markers dock
  dockMarkers = new QDockWidget("Markers", this);
  dockMarkers->setObjectName("dockMarkers");

  QWidget *MarkersGroup = new QWidget();
  QVBoxLayout *Markers_VBox = new QVBoxLayout(MarkersGroup);

  // Trace addition box
  QWidget *MarkerSelection_Widget = new QWidget();

  MarkersGrid = new QGridLayout(MarkerSelection_Widget);
  QLabel *Frequency_Marker_Label = new QLabel("<b>Frequency</b>");
  MarkersGrid->addWidget(Frequency_Marker_Label, 0, 0, Qt::AlignCenter);

  Button_add_marker = new QPushButton("Add marker");
  Button_add_marker->setStyleSheet("QPushButton {background-color: green;\
                                  border-style: outset;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  color: white;\
                                  min-width: 10em;\
                                  padding: 6px;\
                              }");
  connect(Button_add_marker, SIGNAL(clicked()),
          SLOT(addMarker())); // Connect button with the handler
  MarkersGrid->addWidget(Button_add_marker, 0, 0);

  Button_Remove_All_Markers = new QPushButton("Remove all");
  Button_Remove_All_Markers->setStyleSheet("QPushButton {background-color: red;\
                                  border-style: outset;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  color: white;\
                                  min-width: 10em;\
                                  padding: 6px;\
                              }");
  connect(Button_Remove_All_Markers, SIGNAL(clicked()),
          SLOT(removeAllMarkers())); // Connect button with the handler
  MarkersGrid->addWidget(Button_Remove_All_Markers, 0, 1);

  // Marker management
  QWidget *MarkerList_Widget = new QWidget(); // Panel with the trace settings

  QLabel *Label_Marker = new QLabel("<b>Marker</b>");
  QLabel *Label_Freq_Marker = new QLabel("<b>Frequency</b>");
  QLabel *Label_Freq_Scale_Marker = new QLabel("<b>Units</b>");
  QLabel *Label_Remove_Marker = new QLabel("<b>Remove</b>");

  MarkersGrid = new QGridLayout(MarkerList_Widget);
  MarkersGrid->addWidget(Label_Marker, 0, 0, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Freq_Marker, 0, 1, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Freq_Scale_Marker, 0, 2, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Remove_Marker, 0, 3, Qt::AlignCenter);

  QScrollArea *scrollArea_Marker = new QScrollArea();
  scrollArea_Marker->setWidget(MarkerList_Widget);
  scrollArea_Marker->setWidgetResizable(true);

  // Create tab widget to hold the two marker tables
  QTabWidget *tabWidgetMarkers = new QTabWidget();
  connect(tabWidgetMarkers, SIGNAL(currentChanged(int)), this,
          SLOT(raiseWidgetsOnTabSelection(int)));

  // Create the two tables for different marker types
  tableMarkers_Magnitude_Phase = new QTableWidget(1, 1, this);
  tableMarkers_Smith = new QTableWidget(1, 1, this);
  tableMarkers_Polar = new QTableWidget(1, 1, this);
  tableMarkers_PortImpedance = new QTableWidget(1, 1, this);
  tableMarkers_Stability = new QTableWidget(1, 1, this);
  tableMarkers_VSWR = new QTableWidget(1, 1, this);
  tableMarkers_GroupDelay = new QTableWidget(1, 1, this);

  // Add tables to tabs
  tabWidgetMarkers->addTab(tableMarkers_Magnitude_Phase, "Magnitude/Phase");
  tabWidgetMarkers->addTab(tableMarkers_Smith, "Smith");
  tabWidgetMarkers->addTab(tableMarkers_Polar, "Polar");
  tabWidgetMarkers->addTab(tableMarkers_PortImpedance, "Port Impedance");
  tabWidgetMarkers->addTab(tableMarkers_Stability, "Stability");
  tabWidgetMarkers->addTab(tableMarkers_VSWR, "VSWR");
  tabWidgetMarkers->addTab(tableMarkers_GroupDelay, "Group Delay");

  Markers_VBox->addWidget(MarkerSelection_Widget);
  Markers_VBox->addWidget(scrollArea_Marker);
  Markers_VBox->addWidget(
      tabWidgetMarkers); // Add tab widget instead of single table

  dockMarkers->setWidget(MarkersGroup);
}

void Qucs_S_SPAR_Viewer::setLimitManagementDock() {
  // Limits dock
  dockLimits = new QDockWidget("Limits", this);
  dockLimits->setObjectName("dockLimits");

  QWidget *LimitsGroup = new QWidget();
  QVBoxLayout *Limits_VBox = new QVBoxLayout(LimitsGroup);

  // Limit addition box
  QWidget *AddLimit_Widget = new QWidget(); // Add trace

  LimitsGrid = new QGridLayout(AddLimit_Widget);

  Button_add_Limit = new QPushButton("Add Limit");
  Button_add_Limit->setStyleSheet("QPushButton {background-color: green;\
                                  border-style: outset;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  color: white;\
                                  min-width: 10em;\
                                  padding: 6px;\
                              }");
  connect(Button_add_Limit, SIGNAL(clicked()),
          SLOT(addLimit())); // Connect button with the handler
  LimitsGrid->addWidget(Button_add_Limit, 0, 0);

  Button_Remove_All_Limits = new QPushButton("Remove all");
  Button_Remove_All_Limits->setStyleSheet("QPushButton {background-color: red;\
                                  border-style: outset;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  color: white;\
                                  min-width: 10em;\
                                  padding: 6px;\
                              }");
  connect(Button_Remove_All_Limits, SIGNAL(clicked()),
          SLOT(removeAllLimits())); // Connect button with the handler
  LimitsGrid->addWidget(Button_Remove_All_Limits, 0, 1);

  QGroupBox *LimitSettings = new QGroupBox("Settings");
  QGridLayout *LimitsSettingLayout = new QGridLayout(LimitSettings);
  QLabel *LimitsOffsetLabel = new QLabel("<b>Limits Offset</>");
  Limits_Offset = new QDoubleSpinBox();
  Limits_Offset->setValue(0);
  Limits_Offset->setSingleStep(0.1);
  Limits_Offset->setMaximum(1e4);
  Limits_Offset->setMinimum(-1e4);
  connect(Limits_Offset, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  LimitsSettingLayout->addWidget(LimitsOffsetLabel, 0, 0);
  LimitsSettingLayout->addWidget(Limits_Offset, 0, 1);

  // Limit management
  QWidget *LimitList_Widget = new QWidget(); // Panel with the trace settings

  QLabel *Label_Limit = new QLabel("<b>Limit</b>");
  QLabel *Label_Limit_Start = new QLabel("<b>Start</b>");
  QLabel *Label_Limit_Stop = new QLabel("<b>Stop</b>");
  QLabel *Label_Remove_Limit = new QLabel("<b>Remove</b>");

  LimitsGrid = new QGridLayout(LimitList_Widget);
  LimitsGrid->addWidget(Label_Limit, 0, 0, Qt::AlignCenter);
  LimitsGrid->addWidget(Label_Limit_Start, 0, 1, 1, 2, Qt::AlignCenter);
  LimitsGrid->addWidget(Label_Limit_Stop, 0, 3, 1, 2, Qt::AlignCenter);
  LimitsGrid->addWidget(Label_Remove_Limit, 0, 5, Qt::AlignCenter);

  QScrollArea *scrollArea_Limits = new QScrollArea();
  scrollArea_Limits->setWidget(LimitList_Widget);
  scrollArea_Limits->setWidgetResizable(true);

  Limits_VBox->addWidget(AddLimit_Widget);
  Limits_VBox->addWidget(LimitSettings);
  Limits_VBox->addWidget(scrollArea_Limits);

  dockLimits->setWidget(LimitsGroup);
}

void Qucs_S_SPAR_Viewer::CreateDisplayWidgets() {
  // Chart settings
  Magnitude_PhaseChart = new RectangularPlotWidget(this);
  dockChart = new QDockWidget("Magnitude / Phase", this);
  dockChart->setWidget(Magnitude_PhaseChart);
  dockChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  dockChart->setObjectName("dockChart");
  addDockWidget(Qt::LeftDockWidgetArea, dockChart);

  // Smith Chart
  smithChart = new SmithChartWidget(this);
  smithChart->setMinimumSize(300, 300);
  dockSmithChart = new QDockWidget("Smith", this);
  dockSmithChart->setWidget(smithChart);
  dockSmithChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  dockSmithChart->setObjectName("dockSmithChart");
  addDockWidget(Qt::LeftDockWidgetArea, dockSmithChart);

  // Polar Chart
  polarChart = new PolarPlotWidget(this);
  polarChart->setMinimumSize(300, 300);
  dockPolarChart = new QDockWidget("Polar", this);
  dockPolarChart->setWidget(polarChart);
  dockPolarChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  dockPolarChart->setObjectName("dockPolarChart");
  addDockWidget(Qt::LeftDockWidgetArea, dockPolarChart);

  // Port impedance chart settings
  impedanceChart = new RectangularPlotWidget(this);
  dockImpedanceChart = new QDockWidget("Port Impedance", this);
  dockImpedanceChart->setWidget(impedanceChart);
  dockImpedanceChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  dockImpedanceChart->setObjectName("dockImpedanceChart");
  addDockWidget(Qt::LeftDockWidgetArea, dockImpedanceChart);
  impedanceChart->change_Y_axis_title(
      QString("Resistance (Ω)")); // Remove default labels
  impedanceChart->change_Y_axis_units(QString("Ω"));
  impedanceChart->change_Y2_axis_title(QString("Reactance (Ω)"));
  impedanceChart->change_Y2_axis_units(QString("Ω"));

  // Stability plot
  stabilityChart = new RectangularPlotWidget(this);
  dockStabilityChart = new QDockWidget("Stability", this);
  dockStabilityChart->setWidget(stabilityChart);
  dockStabilityChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  dockStabilityChart->setObjectName("dockStabilityChart");
  addDockWidget(Qt::LeftDockWidgetArea, dockStabilityChart);
  stabilityChart->set_y_autoscale(false);
  stabilityChart->setRightYAxisEnabled(false);      // Hide right y-axis
  stabilityChart->change_Y_axis_title(QString("")); // Remove default labels
  stabilityChart->change_Y_axis_title(QString(""));
  stabilityChart->change_Y2_axis_title(QString(""));
  stabilityChart->change_Y2_axis_title(QString(""));
  stabilityChart->change_Y_axis_units(QString(""));
  stabilityChart->setYmin(0);
  stabilityChart->setYdiv(0.25);
  stabilityChart->setYmax(3); // Typically the stability lies -1 and +infty

  // VSWR plot
  VSWRChart = new RectangularPlotWidget(this);
  dockVSWRChart = new QDockWidget("VSWR", this);
  dockVSWRChart->setWidget(VSWRChart);
  dockVSWRChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  dockVSWRChart->setObjectName("dockVSWRChart");
  addDockWidget(Qt::LeftDockWidgetArea, dockVSWRChart);
  VSWRChart->set_y_autoscale(false);
  VSWRChart->setRightYAxisEnabled(false);          // Hide right y-axis
  VSWRChart->change_Y_axis_title(QString("VSWR")); // Remove default labels
  VSWRChart->change_Y2_axis_title(QString(""));
  VSWRChart->change_Y_axis_units(QString(""));
  VSWRChart->setYmin(1);
  VSWRChart->setYdiv(0.5);
  VSWRChart->setYmax(5); // Typically the VSWR lies 1 and +infty

  // Group delay chart settings
  GroupDelayChart = new RectangularPlotWidget(this);
  dockGroupDelayChart = new QDockWidget("Group Delay", this);
  dockGroupDelayChart->setWidget(GroupDelayChart);
  dockGroupDelayChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  dockGroupDelayChart->setObjectName("dockGroupDelayChart");
  addDockWidget(Qt::LeftDockWidgetArea, dockGroupDelayChart);
  GroupDelayChart->change_Y_axis_title(
      QString("Time (ns)")); // Remove default labels
  GroupDelayChart->change_Y_axis_units(QString("ns"));
  GroupDelayChart->setRightYAxisEnabled(false); // Hide right y-axis
  GroupDelayChart->setYdiv(50);                 // By default, 50 ns

  // Disable dock closing
  dockChart->setFeatures(dockChart->features() &
                         ~QDockWidget::DockWidgetClosable);
  dockSmithChart->setFeatures(dockSmithChart->features() &
                              ~QDockWidget::DockWidgetClosable);
  dockPolarChart->setFeatures(dockPolarChart->features() &
                              ~QDockWidget::DockWidgetClosable);
  dockImpedanceChart->setFeatures(dockImpedanceChart->features() &
                                  ~QDockWidget::DockWidgetClosable);
  dockStabilityChart->setFeatures(dockStabilityChart->features() &
                                  ~QDockWidget::DockWidgetClosable);
  dockVSWRChart->setFeatures(dockVSWRChart->features() &
                             ~QDockWidget::DockWidgetClosable);
  dockGroupDelayChart->setFeatures(dockGroupDelayChart->features() &
                                   ~QDockWidget::DockWidgetClosable);

  // Tabify the chart docks
  tabifyDockWidget(dockChart, dockSmithChart);
  tabifyDockWidget(dockSmithChart, dockPolarChart);
  tabifyDockWidget(dockPolarChart, dockImpedanceChart);
  tabifyDockWidget(dockImpedanceChart, dockStabilityChart);
  tabifyDockWidget(dockStabilityChart, dockVSWRChart);
  tabifyDockWidget(dockVSWRChart, dockGroupDelayChart);
}

void Qucs_S_SPAR_Viewer::setupScrollAreaForLayout(QGridLayout *&layout,
                                                  QWidget *parentTab,
                                                  const QString &objectName) {
  // Save the original layout and its widgets
  QGridLayout *originalLayout = layout;
  QList<QWidget *> headerWidgets;

  // Save header row (assuming row 0 is the header)
  for (int col = 0; col < 5; col++) {
    QLayoutItem *item = originalLayout->itemAtPosition(0, col);
    if (item && item->widget()) {
      QWidget *widget = item->widget();
      headerWidgets.append(widget);
      originalLayout->removeWidget(widget);
    }
  }

  // Delete the original layout (but not its widgets)
  delete originalLayout;

  // Create a scroll area
  QScrollArea *scrollArea = new QScrollArea(parentTab);
  scrollArea->setObjectName(objectName);
  scrollArea->setWidgetResizable(true);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  // Create a widget to hold the new layout
  QWidget *container = new QWidget();

  // Create a new grid layout for the container
  layout = new QGridLayout(container);

  // Restore header widgets to the new layout
  for (int col = 0; col < headerWidgets.size(); col++) {
    if (col < headerWidgets.size()) {
      layout->addWidget(headerWidgets[col], 0, col);
    }
  }

  // Set the widget to the scroll area
  scrollArea->setWidget(container);

  // Create a new layout for the tab to hold the scroll area
  QVBoxLayout *tabLayout = new QVBoxLayout(parentTab);
  tabLayout->addWidget(scrollArea);
  tabLayout->setContentsMargins(0, 0, 0, 0);

  // Store the scroll area reference for later access
  if (objectName == "magnitudePhaseScrollArea") {
    magnitudePhaseScrollArea = scrollArea;
  } else if (objectName == "smithScrollArea") {
    smithScrollArea = scrollArea;
  } else if (objectName == "polarScrollArea") {
    polarScrollArea = scrollArea;
  } else if (objectName == "nuScrollArea") {
    nuScrollArea = scrollArea;
  } else if (objectName == "GroupDelayScrollArea") {
    GroupDelayScrollArea = scrollArea;
  }
}

void Qucs_S_SPAR_Viewer::setupScrollableLayout() {
  // Create scroll areas for both layouts
  setupScrollAreaForLayout(magnitudePhaseLayout, magnitudePhaseTab,
                           "magnitudePhaseScrollArea");
  setupScrollAreaForLayout(smithLayout, smithTab, "smithScrollArea");
  setupScrollAreaForLayout(polarLayout, polarTab, "polarScrollArea");
  setupScrollAreaForLayout(portImpedanceLayout, portImpedanceTab,
                           "portImpedanceScrollArea");
  setupScrollAreaForLayout(stabilityLayout, stabilityTab,
                           "stabilityScrollArea");
  setupScrollAreaForLayout(VSWRLayout, VSWRTab, "VSWRScrollArea");
  setupScrollAreaForLayout(GroupDelayLayout, GroupDelayTab,
                           "GroupDelayScrollArea");
}

Qucs_S_SPAR_Viewer::~Qucs_S_SPAR_Viewer() {
  QSettings settings;
  settings.setValue("recentFiles", QVariant::fromValue(recentFiles));
  delete smithChart;
}

void Qucs_S_SPAR_Viewer::slotHelpIntro() {
  QMessageBox msgBox(this);
  msgBox.setWindowTitle(tr("Help"));
  msgBox.setIcon(QMessageBox::Information);

  msgBox.setText(
      tr("<b>Qucs-S S-parameter Viewer & RF Circuit Synthesis Tool "
         "Help</b><br><br>"
         "This is a simple viewer for S-parameter data with RF circuit "
         "synthesis capabilities.<br>"
         "It can show several .snp files at a time in the same diagram. "
         "Trace markers can also be added so that the user can read the trace "
         "value at "
         "a specific frequency.<br><br>"
         "Additionally, this viewer now includes basic filter design features, "
         "and supports visualization with polar plots and Smith charts. "
         "These specialized plots help in analyzing impedance, reflection "
         "coefficients, "
         "and other key S-parameter properties relevant to RF design and "
         "filter analysis."));

  msgBox.exec();
}
void Qucs_S_SPAR_Viewer::slotHelpAboutQt() {
  QMessageBox::aboutQt(this, tr("About Qt"));
}
void Qucs_S_SPAR_Viewer::slotHelpAbout() {
  QMessageBox::about(
      this, tr("About..."),
      "Qucs-S S-parameter Viewer & RF Circuit Synthesis Tool "
      "Version " PACKAGE_VERSION +
          tr("\nCopyright (C) 2025 by") +
          " Andrés Martínez Mera"
          "\n"
          "\nThis program is free software: you can redistribute it and/or "
          "modify"
          "\nit under the terms of the GNU General Public License as published "
          "by"
          "\nthe Free Software Foundation, either version 3 of the License, or"
          "\n(at your option) any later version."
          "\n"
          "\nThis program is distributed in the hope that it will be useful,"
          "\nbut WITHOUT ANY WARRANTY; without even the implied warranty of"
          "\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
          "\nSee the GNU General Public License for more details."
          "\nYou should have received a copy of the GNU General Public License"
          "\nalong with this program. If not, see "
          "<https://www.gnu.org/licenses/>.\n\n");
}

void Qucs_S_SPAR_Viewer::slotQuit() { qApp->quit(); }

// Helper function to extract S-parameter indices from S[i,j] format
QString Qucs_S_SPAR_Viewer::extractSParamIndices(const QString &sparam) {
  QRegularExpression re("S\\[(\\d+),(\\d+)\\]");
  QRegularExpressionMatch match = re.match(sparam);

  if (match.hasMatch()) {
    QString i = match.captured(2); // Second index in S[i,j]
    QString j = match.captured(1); // First index in S[i,j]
    return j + i;                  // Return in Sji format
  }

  return "";
}

// Once a file is loaded, this function adds to the display the default traces
// based in its nature
void Qucs_S_SPAR_Viewer::applyDefaultVisualizations(
    const QStringList &fileNames) {
  if (fileNames.length() == 1) {
    QString filename = QFileInfo(fileNames.first()).fileName();
    filename = filename.left(filename.lastIndexOf('.'));

    if (filename.endsWith("dat")) {
      // Then it must be a .dat.ngspice extension
      filename = filename.left(filename.lastIndexOf('.'));
    }

    // Default behavior: If there's no more data loaded and a single S1P file is
    // selected
    if ((datasets[filename]["n_ports"].at(0) == 1) && (datasets.size() == 1)) {
      // Create TraceInfo structs
      TraceInfo s11_dB = {filename, "S11", DisplayMode::Magnitude_dB};
      TraceInfo s11_Smith = {filename, "S11", DisplayMode::Smith};
      TraceInfo s11_Polar = {filename, "S11", DisplayMode::Polar};

      // Create TraceInfo structs for impedance
      TraceInfo reZin = {filename, "Re{Zin}", DisplayMode::PortImpedance};
      TraceInfo imZin = {filename, "Im{Zin}", DisplayMode::PortImpedance};

      // VSWR
      TraceInfo VSWR = {filename, "VSWR{in}", DisplayMode::VSWR};

      // Add traces with appropriate colors
      this->addTrace(s11_dB, Qt::red, 1);
      this->addTrace(s11_Smith, Qt::darkBlue, 1);
      this->addTrace(s11_Polar, Qt::red, 1);

      this->addTrace(reZin, Qt::darkBlue, 1);
      this->addTrace(imZin, Qt::red, 1);

      this->addTrace(VSWR, Qt::red, 1);
    }

    // Default behavior: If there's no more data loaded and a single S2P file is
    // selected
    if ((datasets[filename]["n_ports"].at(0) == 2) && (datasets.size() == 1)) {
      // Create TraceInfo structs for S-parameters in dB
      TraceInfo s21_dB = {filename, "S21", DisplayMode::Magnitude_dB};
      TraceInfo s11_dB = {filename, "S11", DisplayMode::Magnitude_dB};
      TraceInfo s22_dB = {filename, "S22", DisplayMode::Magnitude_dB};

      // Create TraceInfo structs for Smith chart
      TraceInfo s11_Smith = {filename, "S11", DisplayMode::Smith};
      TraceInfo s22_Smith = {filename, "S22", DisplayMode::Smith};

      // Create TraceInfo structs for Polar display
      TraceInfo s11_Polar = {filename, "S11", DisplayMode::Polar};
      TraceInfo s22_Polar = {filename, "S22", DisplayMode::Polar};

      // Create TraceInfo structs for impedance
      TraceInfo reZin = {filename, "Re{Zin}", DisplayMode::PortImpedance};
      TraceInfo imZin = {filename, "Im{Zin}", DisplayMode::PortImpedance};

      // Create TraceInfo struct for group delay
      TraceInfo s21_GroupDelay = {filename, "S21", DisplayMode::GroupDelay};

      // VSWR
      TraceInfo VSWRin = {filename, "VSWR{in}", DisplayMode::VSWR};
      TraceInfo VSWRout = {filename, "VSWR{out}", DisplayMode::VSWR};

      // Stability
      TraceInfo K = {filename, "K", DisplayMode::Stability};
      TraceInfo mu_s = {filename, "μₛ", DisplayMode::Stability};
      TraceInfo mu_p = {filename, "μₚ", DisplayMode::Stability};

      // Add all traces with appropriate colors
      this->addTrace(s21_dB, Qt::red, 1);
      this->addTrace(s11_dB, Qt::darkBlue, 1);
      this->addTrace(s22_dB, Qt::darkGreen, 1);
      this->addTrace(s11_Smith, Qt::darkBlue, 1);
      this->addTrace(s22_Smith, Qt::darkGreen, 1);
      this->addTrace(s11_Polar, Qt::darkBlue, 1);
      this->addTrace(s22_Polar, Qt::darkGreen, 1);
      this->addTrace(reZin, Qt::darkBlue, 1);
      this->addTrace(imZin, Qt::red, 1);
      this->addTrace(s21_GroupDelay, Qt::darkBlue, 1);
      this->addTrace(VSWRin, Qt::red, 1);
      this->addTrace(VSWRout, Qt::blue, 1);
      this->addTrace(K, Qt::red, 1);
      this->addTrace(mu_s, Qt::blue, 1);
      this->addTrace(mu_p, Qt::darkGreen, 1);
    }
  }
  // Default behaviour: When adding multiple S2P file, then show the S21 of all
  // traces
  if (fileNames.length() > 1) {
    bool all_s2p = true;
    for (const QString &key :
         datasets.keys()) { // Iterate over the keys of the map
      if (datasets[key]["n_ports"].at(0) != 2) {
        all_s2p = false;
        break;
      }
    }

    if (all_s2p == true) {
      for (int i = 0; i < fileNames.length(); i++) {
        QString filename = QFileInfo(fileNames.at(i)).fileName();
        filename = filename.left(filename.lastIndexOf('.'));

        // Color settings. The color of the first traces match the default
        // policy
        QColor trace_color;
        if (i < default_colors.size() - 1) {
          trace_color = default_colors[i];
        } else {
          // Pick a random color
          trace_color = QColor(QRandomGenerator::global()->bounded(256),
                               QRandomGenerator::global()->bounded(256),
                               QRandomGenerator::global()->bounded(256));
        }

        // Create TraceInfo struct for S21 magnitude in dB
        TraceInfo s21_dB = {filename, "S21", DisplayMode::Magnitude_dB};

        // Add the trace with random color
        this->addTrace(s21_dB, trace_color, 1);
      }
    }
  }

  // Show the trace settings widget
  dockTracesList->raise();
}

// Adds optional traces depending on the number of ports of the device
void Qucs_S_SPAR_Viewer::addOptionalTraces(
    QMap<QString, QList<double>> &file_data) {
  QStringList optional_traces;

  int number_of_ports = file_data["n_ports"].at(0);

  if (number_of_ports == 1) {
    optional_traces.append("Re{Zin}");
    optional_traces.append("Im{Zin}");
    optional_traces.append("VSWR{in}");
  } else if (number_of_ports == 2) {
    optional_traces.append("delta");
    optional_traces.append("K");
    optional_traces.append("mu");
    optional_traces.append("mu_p");
    optional_traces.append("MSG");
    optional_traces.append("MAG");
    optional_traces.append("Re{Zin}");
    optional_traces.append("Im{Zin}");
    optional_traces.append("VSWR{in}");
    optional_traces.append("Re{Zout}");
    optional_traces.append("Im{Zout}");
    optional_traces.append("VSWR{out}");
  }

  for (int i = 0; i < optional_traces.size(); i++) {
    if (!file_data.contains(optional_traces[i])) {
      // If not, create an empty list
      file_data[optional_traces[i]] = QList<double>();
    }
  }
}

// This function creates the label and the button in the file list
void Qucs_S_SPAR_Viewer::CreateFileWidgets(QString filename, int position) {

  if (position == 0) {
    position = this->datasets.size();
  }

  QLabel *Filename_Label = new QLabel(filename.left(filename.lastIndexOf('.')));
  Filename_Label->setObjectName(QStringLiteral("File_") +
                                QString::number(position));
  List_FileNames.append(Filename_Label);
  filePaths.append(filename); // Add the file to the watchlist

  this->FilesGrid->addWidget(List_FileNames.last(), position, 0, 1, 1);

  // Create the "Remove" button
  QToolButton *RemoveButton = new QToolButton();
  RemoveButton->setObjectName(QStringLiteral("Remove_") +
                              QString::number(position));
  QIcon icon(":/bitmaps/trash.png"); // Use a resource path or a relative path
  RemoveButton->setIcon(icon);

  RemoveButton->setStyleSheet("QToolButton {background-color: red;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: beige;\
                                  font: bold 14px;\
                                  margin: auto;\
                              }");

  List_RemoveButton.append(RemoveButton);
  this->FilesGrid->addWidget(List_RemoveButton.last(), position, 1, 1, 1);

  connect(RemoveButton, SIGNAL(clicked()),
          SLOT(removeFile())); // Connect button with the handler to remove the
                               // entry.
}

// Given the name of a dataset, this function removes all traces related to it
void Qucs_S_SPAR_Viewer::removeTracesByDataset(
    const QString &dataset_to_remove) {
  // Iterate through the outer QMap (display modes)

  for (const DisplayMode &mode : traceMap.keys()) {
    QMap<QString, TraceProperties> &traces = traceMap[mode];

    // Iterate through the inner QMap (traces in the current mode)
    for (auto trace_it = traces.begin(); trace_it != traces.end();) {
      const QString &trace_name = trace_it.key();

      // Check if the trace belongs to the dataset to remove
      if (trace_name.startsWith(dataset_to_remove + ".")) {
        TraceProperties &props = trace_it.value();

        // Use the common function to remove the trace
        // We need to make a copy of the trace name because it will be
        // invalidated when the trace is removed from the map
        QString traceName = trace_name;

        // Move to next trace before removing current one
        ++trace_it;

        // Remove the trace using the common function
        removeTraceByProps(mode, traceName, props);
      } else {
        ++trace_it; // Increment iterator if not removed
      }
    }
  }
}

void Qucs_S_SPAR_Viewer::removeAndCollapseRow(QGridLayout *targetLayout,
                                              int row_to_remove) {
  int rows = targetLayout->rowCount();
  int columns = targetLayout->columnCount();

  // 1. Remove all widgets in target row (if not already done)
  for (int col = 0; col < columns; ++col) {
    QLayoutItem *item = targetLayout->itemAtPosition(row_to_remove, col);
    if (item && item->widget()) {
      targetLayout->removeWidget(item->widget());
      delete item->widget();
    }
  }

  // 2. Shift all rows below upward
  for (int r = row_to_remove + 1; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      QLayoutItem *item = targetLayout->itemAtPosition(r, c);
      if (item && item->widget()) {
        QWidget *widget = item->widget();
        targetLayout->removeWidget(widget);
        targetLayout->addWidget(widget, r - 1, c); // Move up one row
      }
    }
  }

  // 3.emove empty last row (if needed)
  if (row_to_remove == rows - 1) {
    // QGridLayout doesn't have removeRow(), so we must force layout update
    QWidget *container = targetLayout->parentWidget();
    if (container) {
      container->setUpdatesEnabled(false);
      // Add temporary dummy widget to last row to force recalc
      QWidget *temp = new QWidget();
      targetLayout->addWidget(temp, row_to_remove, 0);
      targetLayout->removeWidget(temp);
      delete temp;
      container->setUpdatesEnabled(true);
    }
  }
}

// This function is used for setting the available traces depending on the
// selected dataset
void Qucs_S_SPAR_Viewer::updateTracesCombo() {
  QCombobox_traces->clear();
  QStringList sParams;
  QStringList otherParams;
  QString current_dataset = QCombobox_datasets->currentText();
  if (current_dataset.isEmpty()) {
    return; // No datasets loaded. This happens if the user had one single file
            // and deleted it
  }

  int n_ports = datasets[current_dataset]["n_ports"].at(0);

  for (int i = 1; i <= n_ports; i++) {
    for (int j = 1; j <= n_ports; j++) {
      sParams.append(QStringLiteral("S%1%2").arg(i).arg(j)); // Magnitude (dB)
    }
  }

  if (n_ports == 1) {
    // Additional traces
    otherParams.append("Re{Zin}");
    otherParams.append("Im{Zin}");
    otherParams.append("VSWR{in}");
  }

  if (n_ports == 2) {
    // Additional traces
    otherParams.append(QStringLiteral("|%1|").arg(QChar(0x0394)));
    otherParams.append("K");
    otherParams.append(
        QStringLiteral("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209B)));
    otherParams.append(
        QStringLiteral("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209A)));
    otherParams.append("MAG");
    otherParams.append("MSG");
    otherParams.append("Re{Zin}");
    otherParams.append("Im{Zin}");
    otherParams.append("VSWR{in}");
    otherParams.append("Re{Zout}");
    otherParams.append("Im{Zout}");
    otherParams.append("VSWR{out}");
  }

  QCombobox_traces->setParameters(sParams, otherParams);
}

// This function adjust the display types available depending on the trace
// selected
void Qucs_S_SPAR_Viewer::updateDisplayType() {
  QString trace_selected = QCombobox_traces->currentText();
  QCombobox_display_mode->clear();
  QStringList display_mode;
  if (trace_selected.startsWith("S")) {
    // Allow all modes
    display_mode.append("dB");
    display_mode.append("Phase");
    display_mode.append("Smith");
    display_mode.append("Polar");
    if (trace_selected.at(1) != trace_selected.at(2)) {
      display_mode.append("Group Delay");
    }
  } else {
    if ((!trace_selected.compare("MAG")) || (!trace_selected.compare("MSG"))) {
      display_mode.append("dB");
    } else {
      display_mode.append("n.u.");
    }
  }
  QCombobox_display_mode->addItems(display_mode);
}

// Given a trace, it gives the minimum and the maximum values at both axis.
void Qucs_S_SPAR_Viewer::getMinMaxValues(QString filename, QString tracename,
                                         qreal &minX, qreal &maxX, qreal &minY,
                                         qreal &maxY) {
  // Find the minimum and the maximum in the x-axis
  QList<double> freq = datasets[filename]["frequency"];
  minX = freq.first();
  maxX = freq.last();

  // Find minimum and maximum in the y-axis
  QList<double> trace_data = datasets[filename][tracename];

  auto minIterator = std::min_element(trace_data.begin(), trace_data.end());
  auto maxIterator = std::max_element(trace_data.begin(), trace_data.end());

  minY = *minIterator;
  maxY = *maxIterator;

  if (tracename.endsWith("_ang")) {
    // Phase traces range from -180 to 180 degrees
    minY = -180;
    maxY = 180;
  } else {
    // Magnitude traces (dB)
    auto minIterator = std::min_element(trace_data.begin(), trace_data.end());
    auto maxIterator = std::max_element(trace_data.begin(), trace_data.end());
    minY = *minIterator;
    maxY = *maxIterator;
  }
}

void Qucs_S_SPAR_Viewer::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void Qucs_S_SPAR_Viewer::dropEvent(QDropEvent *event) {
  QList<QUrl> urls = event->mimeData()->urls();
  QStringList fileList;

  for (const QUrl &url : qAsConst(urls)) {
    if (url.isLocalFile()) {
      fileList << url.toLocalFile();
    }
  }

  if (!fileList.isEmpty()) {
    // Check if this is a session file
    if (fileList.size() == 1) {
      if (fileList.first().endsWith(
              ".spar", Qt::CaseInsensitive)) { // Then open it as a session
                                               // settings file.
        // Remove traces and the dataset from the current session before loading
        // the session file
        removeAllFiles();
        loadSession(fileList.first());
        this->activateWindow();
        return;
      }
    }

    addFiles(fileList);
    this->activateWindow();
  }
}

void Qucs_S_SPAR_Viewer::updateGridLayout(QGridLayout *layout) {
  // Store widget information
  struct WidgetInfo {
    QWidget *widget;
    int row, column, rowSpan, columnSpan;
    Qt::Alignment alignment;
  };
  QVector<WidgetInfo> widgetInfos;

  // Collect information about remaining widgets
  for (int i = 0; i < layout->count(); ++i) {
    QLayoutItem *item = layout->itemAt(i);
    QWidget *widget = item->widget();
    if (widget) {
      int row, column, rowSpan, columnSpan;
      layout->getItemPosition(i, &row, &column, &rowSpan, &columnSpan);
      widgetInfos.push_back(
          {widget, row, column, rowSpan, columnSpan, item->alignment()});
    }
  }

  // Clear the layout
  while (layout->count() > 0) {
    QLayoutItem *item = layout->takeAt(0);
    delete item;
  }

  // Re-add widgets with updated positions
  int row = 0;
  for (const auto &info : widgetInfos) {
    int newColumn = info.column;

    if (info.columnSpan == layout->columnCount()) { // Separator widget
      row++;
    }

    layout->addWidget(info.widget, row, newColumn, info.rowSpan,
                      info.columnSpan, info.alignment);

    if (info.columnSpan == layout->columnCount()) {
      row++;
    }

    if (newColumn == layout->columnCount() - 1) {
      row++;
    }
  }
}

// This function is called when the user requests a trace which can be
// calculated from the S-parameters
void Qucs_S_SPAR_Viewer::calculate_Sparameter_trace(QString file,
                                                    QString metric) {

  if (metric.startsWith("S") && !metric.contains("Group")) {
    // All S-parameters are already calculated.
    return;
  }

  std::complex<double> s11, s12, s21, s22, s11_conj, s22_conj;
  double Z0 = datasets[file]["Z0"].last();

  // Check if it must calculate the Group delay
  if (metric.contains("Group Delay")) {
    QString port_in = metric.at(1);
    QString port_out = metric.at(2);

    QString trace_phase = QString("S%1%2_ang").arg(port_in).arg(port_out);

    QList<double> Sij_ang = datasets[file][trace_phase];
    QList<double> freq = datasets[file]["frequency"];
    QList<double> groupDelay;
    const int numPoints = Sij_ang.size();

    // Phase unwrapping
    QList<double> unwrappedPhase = Sij_ang;
    for (int n = 1; n < numPoints; ++n) {
      double delta = unwrappedPhase[n] - unwrappedPhase[n - 1];

      // Remove 360° discontinuities
      while (delta > 180.0) {
        unwrappedPhase[n] -= 360.0;
        delta = unwrappedPhase[n] - unwrappedPhase[n - 1];
      }
      while (delta < -180.0) {
        unwrappedPhase[n] += 360.0;
        delta = unwrappedPhase[n] - unwrappedPhase[n - 1];
      }
    }

    // Group delay calculation
    groupDelay.reserve(numPoints);

    // First point (forward difference)
    if (numPoints > 1) {
      double df = freq[1] - freq[0];
      double val =
          df != 0 ? -(unwrappedPhase[1] - unwrappedPhase[0]) / (360.0 * df) : 0;
      val *= 1e9; // Convert to ns
      groupDelay.append(val);
    }

    // Central differences for interior points
    for (int n = 1; n < numPoints - 1; ++n) {
      double df = freq[n + 1] - freq[n - 1];
      double val = df != 0 ? -(unwrappedPhase[n + 1] - unwrappedPhase[n - 1]) /
                                 (360.0 * df)
                           : 0;
      val *= 1e9; // Convert to ns
      groupDelay.append(val);
    }

    // Last point (backward difference)
    if (numPoints > 1) {
      double df = freq[numPoints - 1] - freq[numPoints - 2];
      double val = df != 0 ? -(unwrappedPhase[numPoints - 1] -
                               unwrappedPhase[numPoints - 2]) /
                                 (360.0 * df)
                           : 0;
      val *= 1e9; // Convert to ns
      groupDelay.append(val);
    }

    QString trace_name_GD =
        QString("S%1%2_Group Delay").arg(port_in).arg(port_out);
    datasets[file][trace_name_GD].append(groupDelay);
    return;
  }

  for (int i = 0; i < datasets[file]["S11_re"].size(); i++) {
    // S-parameter data (n.u.)
    double s11_re = datasets[file]["S11_re"][i];
    double s11_im = datasets[file]["S11_im"][i];
    s11 = std::complex<double>(s11_re, s11_im);
    s11_conj = std::complex<double>(s11_re, -s11_im);

    if (datasets[file]["n_ports"].last() == 2) {
      double s12_re = datasets[file]["S12_re"][i];
      double s12_im = datasets[file]["S12_im"][i];
      double s21_re = datasets[file]["S21_re"][i];
      double s21_im = datasets[file]["S21_im"][i];
      double s22_re = datasets[file]["S22_re"][i];
      double s22_im = datasets[file]["S22_im"][i];
      s12 = std::complex<double>(s12_re, s12_im);
      s21 = std::complex<double>(s21_re, s21_im);
      s22 = std::complex<double>(s22_re, s22_im);
      s22_conj = std::complex<double>(s22_re, -s22_im);
    }

    double delta = abs(s11 * s22 - s12 * s21); // Determinant of the S matrix

    if (!metric.compare("|Δ|")) {
      datasets[file]["|Δ|"].append(delta);
    } else {
      if (!metric.compare("K")) {
        double K =
            (1 - abs(s11) * abs(s11) - abs(s22) * abs(s22) + delta * delta) /
            (2 * abs(s12 * s21)); // Rollet factor.
        datasets[file]["K"].append(K);
      } else {
        if (!metric.compare("μₛ")) {
          double mu = (1 - abs(s11) * abs(s11)) /
                      (abs(s22 - delta * s11_conj) + abs(s12 * s21));
          datasets[file]["μₛ"].append(mu);
        } else {
          if (!metric.compare("μₚ")) {
            double mu_p = (1 - abs(s22) * abs(s22)) /
                          (abs(s11 - delta * s22_conj) + abs(s12 * s21));
            datasets[file]["μₚ"].append(mu_p);
          } else {
            if (!metric.compare("MSG")) {
              double MSG = abs(s21) / abs(s12);
              MSG = 10 * log10(MSG);
              datasets[file]["MSG"].append(MSG);
            } else {
              if (!metric.compare("MAG")) {
                double K = (1 - abs(s11) * abs(s11) - abs(s22) * abs(s22) +
                            delta * delta) /
                           (2 * abs(s12 * s21)); // Rollet factor.
                double MSG = abs(s21) / abs(s12);
                double MAG = MSG * (K - std::sqrt(K * K - 1));
                MAG = 10 * log10(abs(MAG));
                datasets[file]["MAG"].append(MAG);
              } else {
                if (!metric.compare("Zin")) {
                  std::complex<double> Zin =
                      std::complex<double>(Z0) * (1.0 + s11) / (1.0 - s11);
                  datasets[file]["Re{Zin}"].append(Zin.real());
                  datasets[file]["Im{Zin}"].append(Zin.imag());
                } else {
                  if (!metric.compare("Zout")) {
                    std::complex<double> Zout =
                        std::complex<double>(Z0) * (1.0 + s22) / (1.0 - s22);
                    datasets[file]["Re{Zout}"].append(Zout.real());
                    datasets[file]["Im{Zout}"].append(Zout.imag());
                  } else {
                    if (!metric.compare("Re{Zin}")) {
                      std::complex<double> Zin =
                          std::complex<double>(Z0) * (1.0 + s11) / (1.0 - s11);
                      datasets[file]["Re{Zin}"].append(Zin.real());
                    } else {
                      if (!metric.compare("Im{Zin}")) {
                        std::complex<double> Zin = std::complex<double>(Z0) *
                                                   (1.0 + s11) / (1.0 - s11);
                        datasets[file]["Im{Zin}"].append(Zin.imag());
                      } else {
                        if (!metric.compare("Re{Zout}")) {
                          std::complex<double> Zout = std::complex<double>(Z0) *
                                                      (1.0 + s22) / (1.0 - s22);
                          datasets[file]["Re{Zout}"].append(Zout.real());
                        } else {
                          if (!metric.compare("Im{Zout}")) {
                            std::complex<double> Zout =
                                std::complex<double>(Z0) * (1.0 + s22) /
                                (1.0 - s22);
                            datasets[file]["Im{Zout}"].append(Zout.imag());
                          } else {
                            if (!metric.compare("VSWR{in}")) {
                              double s11_magnitude = abs(s11);
                              double VSWR =
                                  (1 + s11_magnitude) / (1 - s11_magnitude);
                              datasets[file]["VSWR{in}"].append(VSWR);
                            } else {
                              if (!metric.compare("VSWR{out}")) {
                                double s22_magnitude = abs(s22);
                                double VSWR =
                                    (1 + s22_magnitude) / (1 - s22_magnitude);
                                datasets[file]["VSWR{out}"].append(VSWR);
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

// Setup file watcher to monitor S-parameter files
void Qucs_S_SPAR_Viewer::setupFileWatcher() {
  // Clear existing paths
  if (!fileWatcher->files().isEmpty()) {
    fileWatcher->removePaths(fileWatcher->files());
  }

  // Add each file path to watcher
  for (auto it = watchedFilePaths.begin(); it != watchedFilePaths.end(); ++it) {
    QString filePath = it.value();
    if (QFile::exists(filePath)) {
      fileWatcher->addPath(filePath);

      // Also watch the directory for renamed/deleted files
      QFileInfo fileInfo(filePath);
      fileWatcher->addPath(fileInfo.absolutePath());
    }
  }
}

// Handle file changed events
void Qucs_S_SPAR_Viewer::fileChanged(const QString &path) {
  // Don't process the same file within a short time window
  static QMap<QString, QDateTime> lastProcessedTimes;
  static const int debounceTime = 500; // milliseconds

  QDateTime currentTime = QDateTime::currentDateTime();
  if (lastProcessedTimes.contains(path)) {
    if (lastProcessedTimes[path].msecsTo(currentTime) < debounceTime) {
      qDebug() << "Debouncing file change for:" << path;
      return;
    }
  }
  lastProcessedTimes[path] = currentTime;

  QFileInfo fileInfo(path);
  QString fileName = fileInfo.fileName();

  // Some file systems might report the file as deleted when modified
  // Wait a moment to see if the file reappears and to ensure file is fully
  // written
  QTimer::singleShot(200, this, [this, path, fileName, fileInfo]() {
    if (!QFile::exists(path)) {
      qDebug() << "File no longer exists:" << path;
      return;
    }

    // Wait a bit more to ensure the file is completely written and unlocked
    QFile file(path);
    int attempts = 0;
    const int maxAttempts = 5;
    while (attempts < maxAttempts) {
      if (file.open(QIODevice::ReadOnly)) {
        file.close();
        break;
      }
      QThread::msleep(100);
      attempts++;
    }

    if (attempts == maxAttempts) {
      qWarning() << "Could not open file after multiple attempts:" << path;
      return;
    }

    // Find the dataset associated with this file
    QString datasetName;
    for (auto it = watchedFilePaths.begin(); it != watchedFilePaths.end();
         ++it) {
      if (it.value() == path) {
        datasetName = it.key();
        break;
      }
    }

    if (datasetName.isEmpty()) {
      qDebug() << "File changed but not in our datasets:" << path;
      return;
    }

    qDebug() << "Reloading file:" << path << "for dataset:" << datasetName;

    // Determine the file extension
    QString fileExtension = fileInfo.suffix().toLower();
    QMap<QString, QList<double>> file_data;

    // Use appropriate function based on the file extension
    if (fileExtension.startsWith("s") && fileExtension.endsWith("p")) {
      file_data = readTouchstoneFile(path);
    } else if (fileExtension == "dat") {
      file_data = readQucsatorDataset(path);
    } else if (fileExtension == "ngspice") {
      file_data = readNGspiceData(path);
    } else {
      qWarning() << "Unsupported file extension: " << fileExtension;
      return;
    }

    // Verify we actually loaded data
    if (file_data.isEmpty()) {
      qWarning() << "Failed to load data from file:" << path;
      return;
    }

    // Replace the dataset with updated data
    datasets[datasetName] = file_data;

    // Update any plots that use this dataset
    updateAllPlots(datasetName);

    // Make sure the file watcher is still watching this file
    if (!fileWatcher->files().contains(path)) {
      fileWatcher->addPath(path);
    }

    qDebug() << "Successfully updated dataset:" << datasetName;
  });
}

// Handle directory changed events
void Qucs_S_SPAR_Viewer::directoryChanged(const QString &path) {
  qDebug() << "Directory changed:" << path;
  QDir dir(path);

  // List all files
  const QStringList allFiles = dir.entryList(QDir::Files);

  // Regular expression for S-Parameter files (*.s1p, *.s2p, ..., *.snp) and
  // data files
  QRegularExpression sparamRegex(R"(\.s\d+p$)",
                                 QRegularExpression::CaseInsensitiveOption);
  QRegularExpression snpRegex(R"(\.s\d+p$)",
                              QRegularExpression::CaseInsensitiveOption);
  QRegularExpression datRegex(R"(\.dat$)",
                              QRegularExpression::CaseInsensitiveOption);
  QRegularExpression ngspiceDatRegex(R"(\.ngspice\.dat$)",
                                     QRegularExpression::CaseInsensitiveOption);

  QStringList filesToAdd;
  for (const QString &file : allFiles) {
    // Match *.snp (n is 1 or more digits), *.dat, *.ngspice.dat
    if (sparamRegex.match(file).hasMatch() || datRegex.match(file).hasMatch() ||
        ngspiceDatRegex.match(file).hasMatch()) {
      if (!filePaths.contains(file)) {
        filesToAdd.append(file);
      }
    }
  }

  if (filesToAdd.isEmpty()) {
    return;
  }

  // Create dialog
  QDialog dialog(this);
  dialog.setWindowTitle("Select files to add");
  QVBoxLayout *layout = new QVBoxLayout(&dialog);
  QListWidget *listWidget = new QListWidget(&dialog);
  listWidget->setSelectionMode(QAbstractItemView::NoSelection);

  // Add files as checkable items
  for (const QString &file : filesToAdd) {
    QListWidgetItem *item = new QListWidgetItem(file, listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
  }
  layout->addWidget(listWidget);

  // Horizontal layout for select/unselect all buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;
  QPushButton *selectAllButton = new QPushButton("Select All", &dialog);
  QPushButton *unselectAllButton = new QPushButton("Unselect All", &dialog);
  buttonLayout->addWidget(selectAllButton);
  buttonLayout->addWidget(unselectAllButton);
  layout->addLayout(buttonLayout);

  // Add OK/Cancel buttons
  QDialogButtonBox *buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
  layout->addWidget(buttonBox);

  QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog,
                   &QDialog::accept);
  QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog,
                   &QDialog::reject);

  // Select All functionality
  QObject::connect(selectAllButton, &QPushButton::clicked, [&]() {
    for (int i = 0; i < listWidget->count(); ++i) {
      QListWidgetItem *item = listWidget->item(i);
      item->setCheckState(Qt::Checked);
    }
  });

  // Unselect All functionality
  QObject::connect(unselectAllButton, &QPushButton::clicked, [&]() {
    for (int i = 0; i < listWidget->count(); ++i) {
      QListWidgetItem *item = listWidget->item(i);
      item->setCheckState(Qt::Unchecked);
    }
  });

  // Show dialog and process selection
  if (dialog.exec() == QDialog::Accepted) {
    QStringList selectedFiles;
    for (int i = 0; i < listWidget->count(); ++i) {
      QListWidgetItem *item = listWidget->item(i);
      if (item->checkState() == Qt::Checked) {
        selectedFiles.append(dir.absoluteFilePath(item->text()));
      }
    }
    this->addFiles(selectedFiles);
  }
}

// This function is called when a file in the dataset has changes. It updates
// the traces in the display widgets
void Qucs_S_SPAR_Viewer::updateAllPlots(const QString &datasetName) {
  // Refresh all traces on each chart
  updateTracesInWidget(Magnitude_PhaseChart, datasetName);
  updateTracesInWidget(smithChart, datasetName);
  updateTracesInWidget(polarChart, datasetName);
  updateTracesInWidget(impedanceChart, datasetName);
  updateTracesInWidget(GroupDelayChart, datasetName);
}

void Qucs_S_SPAR_Viewer::updateTracesInWidget(QWidget *widget,
                                              const QString &datasetName) {
  if (!widget || !datasets.contains(datasetName)) {
    return;
  }

  QMap<QString, QList<double>> dataset = datasets[datasetName];

  // Handle RectangularPlotWidget
  if (auto *rectWidget = qobject_cast<RectangularPlotWidget *>(widget)) {
    // Get current traces info to preserve settings like pen colors
    QMap<QString, QPen> tracesInfo = rectWidget->getTracesInfo();

    for (auto traceIt = tracesInfo.begin(); traceIt != tracesInfo.end();
         ++traceIt) {
      QString traceName = traceIt.key();
      QStringList parts = traceName.split(".");
      QString file = parts[0];
      QString trace = parts[1];
      QPen tracePen = traceIt.value();

      if (file == datasetName) {
        // Create a new updated trace with the same properties
        RectangularPlotWidget::Trace updatedTrace;

        QString dataKey = traceName;

        calculate_Sparameter_trace(file, trace);
        dataset = datasets[datasetName];

        if (dataset.contains("frequency") && dataset.contains(trace)) {
          // Set the updated data
          updatedTrace.frequencies = dataset["frequency"];
          updatedTrace.trace = dataset[trace];

          // Preserve properties from the existing trace if possible
          // Get the existing trace to copy properties
          const auto &traces = rectWidget->getTracesInfo();
          if (traces.contains(traceName)) {
            updatedTrace.pen = tracePen;
            // Other properties would need to be retrieved if available
            updatedTrace.units = "";             // Set appropriate units
            updatedTrace.Z0 = 50.0;              // Default or preserved value
            updatedTrace.y_axis = 0;             // Default to left axis
            updatedTrace.y_axis_title = dataKey; // Default or preserved value
          }

          // Update the trace in the widget
          rectWidget->removeTrace(traceName);
          rectWidget->addTrace(traceName, updatedTrace);
        }
      }
    }

    // Update the widget display
    rectWidget->updatePlot();
  }
  // Handle PolarPlotWidget
  else if (auto *polarWidget = qobject_cast<PolarPlotWidget *>(widget)) {
    // Get current traces info
    QMap<QString, QPen> tracesInfo = polarWidget->getTracesInfo();

    for (auto traceIt = tracesInfo.begin(); traceIt != tracesInfo.end();
         ++traceIt) {
      QString traceName = traceIt.key();
      QPen tracePen = traceIt.value();

      QStringList parts = traceName.split(".");
      QString file = parts[0];
      QString trace = parts[1];

      if (file == datasetName) {
        // Create a new updated trace with the same properties
        PolarPlotWidget::Trace updatedTrace;

        QString realKey = trace + "_re";
        QString imagKey = trace + "_im";

        if (dataset.contains("frequency") && dataset.contains(realKey) &&
            dataset.contains(imagKey)) {
          // Set the updated data - convert real/imag to complex values
          updatedTrace.frequencies = dataset["frequency"];
          updatedTrace.values.clear();

          for (int i = 0; i < dataset["frequency"].size(); i++) {
            if (i < dataset[realKey].size() && i < dataset[imagKey].size()) {
              std::complex<double> value(dataset[realKey][i],
                                         dataset[imagKey][i]);
              updatedTrace.values.append(value);
            }
          }

          // Preserve display mode and pen
          updatedTrace.pen = tracePen;
          updatedTrace.displayMode = polarWidget->getDisplayMode();

          // Update the trace in the widget
          polarWidget->removeTrace(traceName);
          polarWidget->addTrace(traceName, updatedTrace);
        }
      }
    }

    // Polar widgets don't have an explicit updatePlot method based on the
    // header but we should trigger a redraw
    polarWidget->update();
  }
  // Handle SmithChartWidget
  else if (auto *smithWidget = qobject_cast<SmithChartWidget *>(widget)) {
    // Get current traces info
    QMap<QString, QPen> tracesInfo = smithWidget->getTracesInfo();

    for (auto traceIt = tracesInfo.begin(); traceIt != tracesInfo.end();
         ++traceIt) {
      QString traceName = traceIt.key();
      QPen tracePen = traceIt.value();

      QStringList parts = traceName.split(".");
      QString file = parts[0];
      QString trace = parts[1];

      if (file == datasetName) {
        // Create a new updated trace
        SmithChartWidget::Trace updatedTrace;

        QString realKey = trace + "_re";
        QString imagKey = trace + "_im";
        double Z0 = datasets[file]["Z0"].at(0);

        if (dataset.contains("frequency") && dataset.contains(realKey) &&
            dataset.contains(imagKey)) {
          // Set the updated frequency data
          updatedTrace.frequencies = dataset["frequency"];
          updatedTrace.impedances.clear();

          QList<double> sii_re = datasets[file][realKey];
          QList<double> sii_im = datasets[file][imagKey];

          for (int i = 0; i < dataset["frequency"].size(); i++) {
            std::complex<double> sii(sii_re[i], sii_im[i]);
            std::complex<double> gamma = sii; // Reflection coefficient
            std::complex<double> impedance =
                Z0 * (1.0 + gamma) / (1.0 - gamma); // Convert to impedance
            updatedTrace.impedances.append(impedance);
          }

          // Preserve pen and Z0
          updatedTrace.pen = tracePen;
          updatedTrace.Z0 = smithWidget->characteristicImpedance();

          // Update the trace in the widget
          smithWidget->removeTrace(traceName);
          smithWidget->addTrace(traceName, updatedTrace);
        }
      }
    }

    // Trigger a repaint of the Smith chart
    // smithWidget->update();
  }
}

bool Qucs_S_SPAR_Viewer::isSparamFile(const QString &path) {
  QFileInfo fi(path);
  return fi.exists() &&
         (path.endsWith(".dat", Qt::CaseInsensitive) ||
          QRegularExpression(R"(\.s\d+p$)",
                             QRegularExpression::CaseInsensitiveOption)
              .match(path)
              .hasMatch());
}

void Qucs_S_SPAR_Viewer::addPathToWatcher(const QString &path) {
  if (QFileInfo(path).isDir()) {
    fileWatcher->addPath(path);
    qDebug() << "Watching directory:" << path;

    // Find all files ending with ".dat" or ".snp" (n is an integer),
    // case-insensitive
    QDir dir(path);
    QStringList filters;
    filters << "*.dat" << "*.DAT" << "*.s*" << "*.S*"; // Add uppercase patterns
    dir.setNameFilters(filters);

    QStringList matchingFiles;
    QRegularExpression snpRegex(
        R"(\.s\d+p$)",
        QRegularExpression::CaseInsensitiveOption); // Case-insensitive

    // Iterate through all files in the directory
    for (const QString &fileName : dir.entryList(QDir::Files)) {
      QString lowerFileName = fileName.toLower();
      if (lowerFileName.endsWith(".dat") ||
          snpRegex.match(fileName).hasMatch()) {
        matchingFiles.append(dir.absoluteFilePath(fileName)); // Add full path
      }
    }

    // Add matching files to the program database
    if (!matchingFiles.isEmpty()) {
      addFiles(matchingFiles);
      qDebug() << "Added files to database:" << matchingFiles;
    }
  }
}

// This function is triggered when a trace-type tab is clicked in the trace
// management tab
void Qucs_S_SPAR_Viewer::raiseWidgetsOnTabSelection(int index) {
  switch (index) {
  case 0:
    // Magnitude / phase tab
    dockChart->raise();
    QCombobox_display_mode->setCurrentText("dB");
    QCombobox_traces->setCurrentText("S11");
    break;
  case 1:
    // Smith Chart
    dockSmithChart->raise();
    QCombobox_display_mode->setCurrentText("Smith");
    QCombobox_traces->setCurrentText("S11");
    break;
  case 2:
    // Polar Chart
    dockPolarChart->raise();
    QCombobox_display_mode->setCurrentText("Polar");
    QCombobox_traces->setCurrentText("S11");
    break;
  case 3:
    // Port impedance Chart
    dockImpedanceChart->raise();
    QCombobox_display_mode->setCurrentText("n.u.");
    QCombobox_traces->setCurrentText("Re{Zin}");
    break;
  case 4:
    // Stability Chart
    dockStabilityChart->raise();
    QCombobox_display_mode->setCurrentText("n.u.");
    break;
  case 5:
    // VSWR Chart
    dockVSWRChart->raise();
    QCombobox_traces->setCurrentText("VSWR{in}");
    QCombobox_display_mode->setCurrentText("n.u.");
    break;
  case 6:
    // Group Delay Chart
    dockGroupDelayChart->raise();
    QCombobox_display_mode->setCurrentText("Group Delay");
    break;
  }
}

// Triggers synthesis when a tool is selected
void Qucs_S_SPAR_Viewer::callTools(bool visible) {
  if (visible) {
    // Dock is now visible - trigger your function
    int index = toolsTabWidget->currentIndex();
    switch (index) {
    case 0: // Filter design tool
      FilterTool->synthesize();
      break;
    case 1: // Matching tool
      MatchingTool->design();
      break;
    case 2: // Power Combining tool
      PowerCombTool->design();
      break;
    case 3: // Attenuator tool
      AttenuatorTool->design();
      break;
    }
  }
}
