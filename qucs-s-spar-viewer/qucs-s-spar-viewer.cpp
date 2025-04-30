/*
 * qucs-s-spar-viewer.cpp - S-parameter viewer for Qucs-S
 *
 * copyright (C) 2024 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qucs-s-spar-viewer.h"

#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QValidator>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QLineSeries>


Qucs_S_SPAR_Viewer::Qucs_S_SPAR_Viewer()
{
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  centralWidget->setMaximumWidth(0); // Minimize central widget size

  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle("Qucs S-parameter Viewer " PACKAGE_VERSION);

  CreateMenuBar();

  // Set frequency units
  frequency_units << "Hz" << "kHz" << "MHz" << "GHz";


  // These are two maximum markers to find the lowest and the highest frequency in the data samples.
  // They are used to prevent the user from zooming out too much
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
  connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &Qucs_S_SPAR_Viewer::fileChanged);
  connect(fileWatcher, &QFileSystemWatcher::directoryChanged, this, &Qucs_S_SPAR_Viewer::directoryChanged);

  // Put the following widgets on the top to make them visible to the user
  dockFiles->raise();
  dockChart->raise();

  setDockNestingEnabled(true);
  setAcceptDrops(true);//Enable drag and drop feature to open files
  loadRecentFiles();// Load "Recent Files" list
}


void Qucs_S_SPAR_Viewer::CreateMenuBar(){
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
  connect(recentFilesMenu, &QMenu::aboutToShow, this, &Qucs_S_SPAR_Viewer::updateRecentFilesMenu);

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

  QAction * helpAboutQt = new QAction(tr("About Qt..."), this);
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

void Qucs_S_SPAR_Viewer::CreateRightPanel(){
  // Create left panel widgets
  setFileManagementDock();
  setTraceManagementDock();
  setMarkerManagementDock();
  setLimitManagementDock();

  // Notes
  Notes_Widget = new CodeEditor();
  dockNotes = new QDockWidget("Notes", this);
  dockNotes->setWidget(Notes_Widget);

  // Disable dock closing
  dockFiles->setFeatures(dockFiles->features() & ~QDockWidget::DockWidgetClosable);
  dockTracesList->setFeatures(dockTracesList->features() & ~QDockWidget::DockWidgetClosable);
  dockMarkers->setFeatures(dockMarkers->features() & ~QDockWidget::DockWidgetClosable);
  dockLimits->setFeatures(dockLimits->features() & ~QDockWidget::DockWidgetClosable);
  dockNotes->setFeatures(dockNotes->features() & ~QDockWidget::DockWidgetClosable);

  // Add all panel docks to the right area
  addDockWidget(Qt::RightDockWidgetArea, dockFiles);
  addDockWidget(Qt::RightDockWidgetArea, dockTracesList);
  addDockWidget(Qt::RightDockWidgetArea, dockMarkers);
  addDockWidget(Qt::RightDockWidgetArea, dockLimits);
  addDockWidget(Qt::RightDockWidgetArea, dockNotes);

         // Tabify the panel docks
  tabifyDockWidget(dockFiles, dockTracesList);
  tabifyDockWidget(dockTracesList, dockMarkers);
  tabifyDockWidget(dockMarkers, dockLimits);
  tabifyDockWidget(dockLimits, dockNotes);

  // Remove the tabify between chart docks as it's already done in CreateDisplayWidgets
  // tabifyDockWidget(dockChart, dockSmithChart);

  // To prevent the gap between left and right dock areas, we need to resize the dock widgets
  // This should be called after all dock widgets are set up, perhaps in a separate method
  resizeDocks({dockChart, dockSmithChart}, {width()/2, width()/2}, Qt::Horizontal);
  resizeDocks({dockFiles, dockTracesList, dockMarkers, dockLimits, dockNotes},
              {width()/4, width()/4, width()/4, width()/4, width()/4}, Qt::Horizontal);
}


void Qucs_S_SPAR_Viewer::setFileManagementDock(){

  dockFiles = new QDockWidget("S-parameter files", this);

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
  QString tooltip_message = QString("Add single data file (.dat, .snp). You can also drag and drop it.");
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

void Qucs_S_SPAR_Viewer::setTraceManagementDock(){

  dockTracesList = new QDockWidget("Traces List", this);

  QWidget * TracesGroup = new QWidget();
  QVBoxLayout *Traces_VBox = new QVBoxLayout(TracesGroup);

  // Trace addition box
  QWidget * TraceSelection_Widget = new QWidget(); // Add trace

  QGridLayout * DatasetsGrid = new QGridLayout(TraceSelection_Widget);
  QLabel *dataset_label = new QLabel("<b>Dataset</b>");
  DatasetsGrid->addWidget(dataset_label, 0, 0, Qt::AlignCenter);

  QLabel *Traces_label = new QLabel("<b>Traces</b>");
  DatasetsGrid->addWidget(Traces_label, 0, 1, Qt::AlignCenter);

  QLabel *displayTypeLabel = new QLabel("<b>Display Type</b>");
  DatasetsGrid->addWidget(displayTypeLabel, 0, 2, Qt::AlignCenter);

  QCombobox_traces = new MatrixComboBox();
  connect(QCombobox_traces, SIGNAL(currentIndexChanged(int)), SLOT(updateDisplayType()));
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
 connect(Button_add_trace, SIGNAL(clicked()), SLOT(addTrace())); // Connect button with the handler

  QCombobox_display_mode= new QComboBox();
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
  connect(QCombobox_datasets, SIGNAL(currentIndexChanged(int)), SLOT(updateTracesCombo())); // Each time the dataset is changed it is needed to update the traces combo.
                                                                                            // This is needed when the user has data with different number of ports.
  traceTabs = new QTabWidget(this); // Ensure 'this' is the parent
  connect(traceTabs, SIGNAL(currentChanged(int)), this, SLOT(raiseWidgetsOnTabSelection(int)));


  // Create tabs for Magnitude/Phase and Smith Chart
  magnitudePhaseTab = new QWidget(traceTabs); // Parent is traceTabs
  smithTab = new QWidget(traceTabs); // Parent is traceTabs
  polarTab = new QWidget(traceTabs); // Parent is traceTabs
  portImpedanceTab = new QWidget(traceTabs); // Parent is traceTabs
  stabilityTab = new QWidget(traceTabs); // Parent is traceTabs
  VSWRTab = new QWidget(traceTabs); // Parent is traceTabs
  GroupDelayTab = new QWidget(traceTabs); // Parent is traceTabs

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
  QLabel * Label_Name_mag = new QLabel("<b>Name</b>");
  QLabel * Label_Color_mag = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle_mag = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth_mag = new QLabel("<b>Width</b>");
  QLabel * Label_Remove_mag = new QLabel("<b>Remove</b>");

  magnitudePhaseLayout->addWidget(Label_Name_mag, 0, 0, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_Color_mag, 0, 1, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_LineStyle_mag, 0, 2, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_LineWidth_mag, 0, 3, Qt::AlignCenter);
  magnitudePhaseLayout->addWidget(Label_Remove_mag, 0, 4, Qt::AlignCenter);

  // Set Smith tab
  QLabel * Label_Name_Smith = new QLabel("<b>Name</b>");
  QLabel * Label_Color_Smith = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle_Smith = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth_Smith = new QLabel("<b>Width</b>");
  QLabel * Label_Remove_Smith = new QLabel("<b>Remove</b>");

  smithLayout->addWidget(Label_Name_Smith, 0, 0, Qt::AlignCenter);
  smithLayout->addWidget(Label_Color_Smith, 0, 1, Qt::AlignCenter);
  smithLayout->addWidget(Label_LineStyle_Smith, 0, 2, Qt::AlignCenter);
  smithLayout->addWidget(Label_LineWidth_Smith, 0, 3, Qt::AlignCenter);
  smithLayout->addWidget(Label_Remove_Smith, 0, 4, Qt::AlignCenter);

  // Set Polar tab
  QLabel * Label_Name_Polar = new QLabel("<b>Name</b>");
  QLabel * Label_Color_Polar = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle_Polar = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth_Polar = new QLabel("<b>Width</b>");
  QLabel * Label_Remove_Polar = new QLabel("<b>Remove</b>");

  polarLayout->addWidget(Label_Name_Polar, 0, 0, Qt::AlignCenter);
  polarLayout->addWidget(Label_Color_Polar, 0, 1, Qt::AlignCenter);
  polarLayout->addWidget(Label_LineStyle_Polar, 0, 2, Qt::AlignCenter);
  polarLayout->addWidget(Label_LineWidth_Polar, 0, 3, Qt::AlignCenter);
  polarLayout->addWidget(Label_Remove_Polar, 0, 4, Qt::AlignCenter);

  // Set "Port Impedance" tab
  QLabel * Label_Name_nu = new QLabel("<b>Name</b>");
  QLabel * Label_Color_nu = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle_nu = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth_nu = new QLabel("<b>Width</b>");
  QLabel * Label_Remove_nu = new QLabel("<b>Remove</b>");

  portImpedanceLayout->addWidget(Label_Name_nu, 0, 0, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_Color_nu, 0, 1, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_LineStyle_nu, 0, 2, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_LineWidth_nu, 0, 3, Qt::AlignCenter);
  portImpedanceLayout->addWidget(Label_Remove_nu, 0, 4, Qt::AlignCenter);

  // Set "Stability" tab
  QLabel * Label_Name_stab = new QLabel("<b>Name</b>");
  QLabel * Label_Color_stab = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle_stab = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth_stab = new QLabel("<b>Width</b>");
  QLabel * Label_Remove_stab = new QLabel("<b>Remove</b>");

  stabilityLayout->addWidget(Label_Name_stab, 0, 0, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_Color_stab, 0, 1, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_LineStyle_stab, 0, 2, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_LineWidth_stab, 0, 3, Qt::AlignCenter);
  stabilityLayout->addWidget(Label_Remove_stab, 0, 4, Qt::AlignCenter);

  // Set VSWR tab
  QLabel * Label_Name_VSWR = new QLabel("<b>Name</b>");
  QLabel * Label_Color_VSWR = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle_VSWR = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth_VSWR = new QLabel("<b>Width</b>");
  QLabel * Label_Remove_VSWR = new QLabel("<b>Remove</b>");

  VSWRLayout->addWidget(Label_Name_VSWR, 0, 0, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_Color_VSWR, 0, 1, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_LineStyle_VSWR, 0, 2, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_LineWidth_VSWR, 0, 3, Qt::AlignCenter);
  VSWRLayout->addWidget(Label_Remove_VSWR, 0, 4, Qt::AlignCenter);

  // Set "Group delay" tab
  QLabel * Label_Name_GD = new QLabel("<b>Name</b>");
  QLabel * Label_Color_GD = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle_GD = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth_GD = new QLabel("<b>Width</b>");
  QLabel * Label_Remove_GD = new QLabel("<b>Remove</b>");

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
  QLabel * Label_Name = new QLabel("<b>Name</b>");
  QLabel * Label_Color = new QLabel("<b>Color</b>");
  QLabel * Label_LineStyle = new QLabel("<b>Line Style</b>");
  QLabel * Label_LineWidth = new QLabel("<b>Width</b>");
  QLabel * Label_Remove = new QLabel("<b>Remove</b>");

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

  QWidget* MarkersGroup = new QWidget();
  QVBoxLayout* Markers_VBox = new QVBoxLayout(MarkersGroup);

  // Trace addition box
  QWidget* MarkerSelection_Widget = new QWidget();

  MarkersGrid = new QGridLayout(MarkerSelection_Widget);
  QLabel* Frequency_Marker_Label = new QLabel("<b>Frequency</b>");
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
  connect(Button_add_marker, SIGNAL(clicked()), SLOT(addMarker())); // Connect button with the handler
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
  connect(Button_Remove_All_Markers, SIGNAL(clicked()), SLOT(removeAllMarkers())); // Connect button with the handler
  MarkersGrid->addWidget(Button_Remove_All_Markers, 0, 1);

  // Marker management
  QWidget* MarkerList_Widget = new QWidget(); // Panel with the trace settings

  QLabel* Label_Marker = new QLabel("<b>Marker</b>");
  QLabel* Label_Freq_Marker = new QLabel("<b>Frequency</b>");
  QLabel* Label_Freq_Scale_Marker = new QLabel("<b>Units</b>");
  QLabel* Label_Remove_Marker = new QLabel("<b>Remove</b>");

  MarkersGrid = new QGridLayout(MarkerList_Widget);
  MarkersGrid->addWidget(Label_Marker, 0, 0, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Freq_Marker, 0, 1, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Freq_Scale_Marker, 0, 2, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Remove_Marker, 0, 3, Qt::AlignCenter);

  QScrollArea* scrollArea_Marker = new QScrollArea();
  scrollArea_Marker->setWidget(MarkerList_Widget);
  scrollArea_Marker->setWidgetResizable(true);

  // Create tab widget to hold the two marker tables
  QTabWidget* tabWidgetMarkers = new QTabWidget();
  connect(tabWidgetMarkers, SIGNAL(currentChanged(int)), this, SLOT(raiseWidgetsOnTabSelection(int)));


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
  Markers_VBox->addWidget(tabWidgetMarkers); // Add tab widget instead of single table

  dockMarkers->setWidget(MarkersGroup);
}

void Qucs_S_SPAR_Viewer::setLimitManagementDock(){
  // Limits dock
  dockLimits = new QDockWidget("Limits", this);

  QWidget * LimitsGroup = new QWidget();
  QVBoxLayout *Limits_VBox = new QVBoxLayout(LimitsGroup);

  // Limit addition box
  QWidget * AddLimit_Widget = new QWidget(); // Add trace

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
  connect(Button_add_Limit, SIGNAL(clicked()), SLOT(addLimit())); // Connect button with the handler
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
  connect(Button_Remove_All_Limits, SIGNAL(clicked()), SLOT(removeAllLimits())); // Connect button with the handler
  LimitsGrid->addWidget(Button_Remove_All_Limits, 0, 1);

  QGroupBox * LimitSettings = new QGroupBox("Settings");
  QGridLayout * LimitsSettingLayout = new QGridLayout(LimitSettings);
  QLabel * LimitsOffsetLabel = new QLabel("<b>Limits Offset</>");
  Limits_Offset = new QDoubleSpinBox();
  Limits_Offset->setValue(0);
  Limits_Offset->setSingleStep(0.1);
  Limits_Offset->setMaximum(1e4);
  Limits_Offset->setMinimum(-1e4);
  connect(Limits_Offset, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  LimitsSettingLayout->addWidget(LimitsOffsetLabel, 0, 0);
  LimitsSettingLayout->addWidget(Limits_Offset, 0, 1);

  // Limit management
  QWidget * LimitList_Widget = new QWidget(); // Panel with the trace settings

  QLabel * Label_Limit = new QLabel("<b>Limit</b>");
  QLabel * Label_Limit_Start = new QLabel("<b>Start</b>");
  QLabel * Label_Limit_Stop = new QLabel("<b>Stop</b>");
  QLabel * Label_Remove_Limit = new QLabel("<b>Remove</b>");

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




void Qucs_S_SPAR_Viewer::CreateDisplayWidgets(){
  // Chart settings
  Magnitude_PhaseChart = new RectangularPlotWidget(this);
  dockChart = new QDockWidget("Magnitude / Phase", this);
  dockChart->setWidget(Magnitude_PhaseChart);
  dockChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, dockChart);

  // Smith Chart
  smithChart = new SmithChartWidget(this);
  smithChart->setMinimumSize(300, 300);
  dockSmithChart = new QDockWidget("Smith", this);
  dockSmithChart->setWidget(smithChart);
  dockSmithChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, dockSmithChart);

  // Polar Chart
  polarChart = new PolarPlotWidget(this);
  polarChart->setMinimumSize(300, 300);
  dockPolarChart = new QDockWidget("Polar", this);
  dockPolarChart->setWidget(polarChart);
  dockPolarChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, dockPolarChart);

  // Port impedance chart settings
  impedanceChart = new RectangularPlotWidget(this);
  dockImpedanceChart = new QDockWidget("Port Impedance", this);
  dockImpedanceChart->setWidget(impedanceChart);
  dockImpedanceChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, dockImpedanceChart);
  impedanceChart->change_Y_axis_title(QString("Resistance (Ω)")); // Remove default labels
  impedanceChart->change_Y_axis_units(QString("Ω"));
  impedanceChart->change_Y2_axis_title(QString("Reactance (Ω)"));
  impedanceChart->change_Y2_axis_units(QString("Ω"));

  // Stability plot
  stabilityChart = new RectangularPlotWidget(this);
  dockStabilityChart = new QDockWidget("Stability", this);
  dockStabilityChart->setWidget(stabilityChart);
  dockStabilityChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, dockStabilityChart);
  stabilityChart->set_y_autoscale(false);
  stabilityChart->setRightYAxisEnabled(false); // Hide right y-axis
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
  addDockWidget(Qt::LeftDockWidgetArea, dockVSWRChart);
  VSWRChart->set_y_autoscale(false);
  VSWRChart->setRightYAxisEnabled(false); // Hide right y-axis
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
  addDockWidget(Qt::LeftDockWidgetArea, dockGroupDelayChart);
  GroupDelayChart->change_Y_axis_title(QString("Time (ns)")); // Remove default labels
  GroupDelayChart->change_Y_axis_units(QString("ns"));
  GroupDelayChart->setRightYAxisEnabled(false); // Hide right y-axis
  GroupDelayChart->setYdiv(50); // By default, 50 ns

  // Disable dock closing
  dockChart->setFeatures(dockChart->features() & ~QDockWidget::DockWidgetClosable);
  dockSmithChart->setFeatures(dockSmithChart->features() & ~QDockWidget::DockWidgetClosable);
  dockPolarChart->setFeatures(dockPolarChart->features() & ~QDockWidget::DockWidgetClosable);
  dockImpedanceChart->setFeatures(dockImpedanceChart->features() & ~QDockWidget::DockWidgetClosable);
  dockStabilityChart->setFeatures(dockStabilityChart->features() & ~QDockWidget::DockWidgetClosable);
  dockVSWRChart->setFeatures(dockVSWRChart->features() & ~QDockWidget::DockWidgetClosable);
  dockGroupDelayChart->setFeatures(dockGroupDelayChart->features() & ~QDockWidget::DockWidgetClosable);

  // Tabify the chart docks
  tabifyDockWidget(dockChart, dockSmithChart);
  tabifyDockWidget(dockSmithChart, dockPolarChart);
  tabifyDockWidget(dockPolarChart, dockImpedanceChart);
  tabifyDockWidget(dockImpedanceChart, dockStabilityChart);
  tabifyDockWidget(dockStabilityChart, dockVSWRChart);
  tabifyDockWidget(dockVSWRChart, dockGroupDelayChart);
}

void Qucs_S_SPAR_Viewer::setupScrollAreaForLayout(QGridLayout* &layout, QWidget* parentTab, const QString &objectName)
{
  // Save the original layout and its widgets
  QGridLayout* originalLayout = layout;
  QList<QWidget*> headerWidgets;

  // Save header row (assuming row 0 is the header)
  for (int col = 0; col < 5; col++) {
    QLayoutItem* item = originalLayout->itemAtPosition(0, col);
    if (item && item->widget()) {
      QWidget* widget = item->widget();
      headerWidgets.append(widget);
      originalLayout->removeWidget(widget);
    }
  }

  // Delete the original layout (but not its widgets)
  delete originalLayout;

  // Create a scroll area
  QScrollArea* scrollArea = new QScrollArea(parentTab);
  scrollArea->setObjectName(objectName);
  scrollArea->setWidgetResizable(true);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  // Create a widget to hold the new layout
  QWidget* container = new QWidget();

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
  QVBoxLayout* tabLayout = new QVBoxLayout(parentTab);
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

void Qucs_S_SPAR_Viewer::setupScrollableLayout()
{
  // Create scroll areas for both layouts
  setupScrollAreaForLayout(magnitudePhaseLayout, magnitudePhaseTab, "magnitudePhaseScrollArea");
  setupScrollAreaForLayout(smithLayout, smithTab, "smithScrollArea");
  setupScrollAreaForLayout(polarLayout, polarTab, "polarScrollArea");
  setupScrollAreaForLayout(portImpedanceLayout, portImpedanceTab, "portImpedanceScrollArea");
  setupScrollAreaForLayout(stabilityLayout, stabilityTab, "stabilityScrollArea");
  setupScrollAreaForLayout(VSWRLayout, VSWRTab, "VSWRScrollArea");
  setupScrollAreaForLayout(GroupDelayLayout, GroupDelayTab, "GroupDelayScrollArea");
}

Qucs_S_SPAR_Viewer::~Qucs_S_SPAR_Viewer()
{
  QSettings settings;
  settings.setValue("recentFiles", QVariant::fromValue(recentFiles));
  delete smithChart;
}

void Qucs_S_SPAR_Viewer::slotHelpIntro()
{
  QMessageBox::about(this, tr("Qucs-S S-parameter Help"),
    tr("This is a simple viewer for S-parameter data.\n"
         "It can show several .snp files at a time in the "
         "same diagram. Trace markers can also be added "
         "so that the user can read the trace value at "
         "at an specific frequency."));
}

void Qucs_S_SPAR_Viewer::slotHelpAboutQt()
{
      QMessageBox::aboutQt(this, tr("About Qt"));
}

void Qucs_S_SPAR_Viewer::slotHelpAbout()
{
    QMessageBox::about(this, tr("About..."),
    "Qucs-S S-parameter Viewer Version " PACKAGE_VERSION+
    tr("\nCopyright (C) 2025 by")+" Andrés Martínez Mera"
    "\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

void Qucs_S_SPAR_Viewer::slotQuit()
{
  qApp->quit();
}


void Qucs_S_SPAR_Viewer::addFile()
{
  QFileDialog dialog(this,
                     QStringLiteral("Select S-parameter data files"),
                     QDir::homePath(),
                     tr("S-Parameter Files (*.s1p *.s2p *.s3p *.s4p);;"
                        "Data Files (*.dat *.ngspice.dat);;"
                        "All Files (*.*)"));
  dialog.setFileMode(QFileDialog::ExistingFiles);

  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();

  addFiles(fileNames);
}

void Qucs_S_SPAR_Viewer::addFiles(QStringList fileNames)
{
  int existing_files = this->datasets.size(); // Get the number of entries in the map
  QString filename;

  if (existing_files == 0) {
    // Reset limits
    this->f_max = -1;
    this->f_min = 1e30;
  }

  // Remove from the list of files those that already exist in the database
  QStringList files_dataset = datasets.keys();

  for (int i = 0; i < fileNames.length(); i++) {
    filename = QFileInfo(fileNames.at(i)).fileName();
    // Check if this file already exists
    QString new_filename = filename.left(filename.lastIndexOf('.'));

    if (new_filename.endsWith(".dat")) {
      // This file has extension .dat.ngspice
      new_filename = new_filename.left(new_filename.length()-4);
    }


    if (files_dataset.contains(new_filename)) {
      // Remove it from the list of new files to load
      fileNames.removeAt(i);

             // Pop up a warning
      QMessageBox::information(
          this,
          tr("Warning"),
          tr("This file is already in the dataset."));
    }
  }

  // Read files
  int widget_counter = existing_files;
  int n_files = fileNames.length(); // Number of files to be added
  QStringList files_filtered; // Some of the files included may be discarded for not having s-parameter data. This list contain only the files to be added
  for (int i = existing_files; i < existing_files + n_files; i++) {
    // Create the file name label
    QString filename = QFileInfo(fileNames.at(i-existing_files)).fileName();


           // Determine the file extension
    QString fileExtension = QFileInfo(fileNames.at(i-existing_files)).suffix().toLower();

    QMap<QString, QList<double>> file_data;

           // Use appropriate function based on the file extension
    if (fileExtension.startsWith("s") && fileExtension.endsWith("p")) {
      file_data = readTouchstoneFile(fileNames.at(i-existing_files));
    } else if (fileExtension == "dat") {
      file_data = readQucsatorDataset(fileNames.at(i-existing_files));
    } else if (fileExtension == "ngspice") {
      file_data = readNGspiceData(fileNames.at(i-existing_files));
    } else {
      qWarning() << "Unsupported file extension: " << fileExtension;
      continue; // Skip unsupported files
    }


    if (file_data.isEmpty()) {
      // Stop the load process and remove file from the list of files to be added
      continue;
    } else {
      // It must contain basic S-parameter data
      if (file_data["n_ports"].at(0) == 0) {
        continue;
      }
    }
    files_filtered.append(filename);

    // Create widgets at this point. It's necessary to ensure that the files to be loaded contain S-parameter data
    CreateFileWidgets(filename, widget_counter);
    widget_counter++;

           // Add data to the dataset
    QString dataset_name = filename.left(filename.lastIndexOf('.')); // Remove file extension
    if (fileExtension == "ngspice") {
      // These files have extension .dat.ngspice. Remove the extension again to have only the file name
      dataset_name = dataset_name.left(dataset_name.length()-4);
    }


    datasets[dataset_name] = file_data;

    // Add file to watchedFilePaths map
    watchedFilePaths[dataset_name] = fileNames.at(i-existing_files);

    // Add new dataset to the trace selection combobox
    QCombobox_datasets->addItem(dataset_name);

           // Add optional traces based on number of ports
    addOptionalTraces(file_data);

           // Update traces
    updateTracesCombo();
  }

         // Apply default visualizations based on file types
  applyDefaultVisualizations(files_filtered);

  // Set up file watcher for the newly added files
  setupFileWatcher();
}


// Given a string path to a file, it reads the Touchstone data into the main dataset
QMap<QString, QList<double>> Qucs_S_SPAR_Viewer::readTouchstoneFile(const QString& filePath)
{
  QMap<QString, QList<double>> file_data; // Data structure to store the file data
  QString frequency_unit, parameter, format;
  double freq_scale = 1; // Hz
  double Z0 = 50; // System impedance. Typically 50 Ohm
  QStringList values;

  // Get the filename for extracting number of ports
  QString filename = QFileInfo(filePath).fileName();

  // Get the number of ports
  QString suffix = QFileInfo(filename).suffix();
  QRegularExpression regex("(?i)[sp]");
  QStringList numberParts = suffix.split(regex);
  int number_of_ports = numberParts[1].toInt();
  file_data["n_ports"].append(number_of_ports);

         // 1) Open the file
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Cannot open the file";
    return file_data;
  }

         // 2) Read data
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    line = line.simplified();

    if (line.isEmpty()) continue;
        if ((line.at(0).isNumber() == false) && (line.at(0) != '#')) {
      if (file_data["frequency"].size() == 0){
        // There's still no data
        continue;
      } else {
        //There's already data, so it's very likely that the S-par data has ended and
        //the following lines contain noise data. We must stop at this point.
        break;
      }
    }

    // Check for the option line
    if (line.at(0) == '#'){
      QStringList info = line.split(" ");
      frequency_unit = info.at(1); // Specifies the unit of frequency.
                                   // Legal values are Hz, kHz, MHz, and GHz. The default value is GHz.

      frequency_unit = frequency_unit.toLower();

      if (frequency_unit == "khz"){
        freq_scale = 1e3;
      } else {
        if (frequency_unit == "mhz"){
          freq_scale = 1e6;
        } else {
          if (frequency_unit == "ghz"){
            freq_scale = 1e9;
          }
        }
      }

      parameter = info.at(2); // specifies what kind of network parameter data is contained in the file
      format = info.at(3);    // Specifies the format of the network parameter data pairs
      Z0 = info.at(5).toDouble();
      file_data["Z0"].append(Z0);

      continue;
    }

           // Split line by whitespace
    values.clear();
    values = line.split(' ');

    file_data["frequency"].append(values[0].toDouble()*freq_scale); // in Hz

    double S_1, S_2, S_3, S_4;
    QString s1, s2, s3, s4;
    int index = 1, data_counter = 0;

    for (int i = 1; i<=number_of_ports; i++){
      for (int j = 1; j<=number_of_ports; j++){
        s1 = QStringLiteral("S") + QString::number(j) + QString::number(i) + QStringLiteral("_dB");
        s2 = s1.mid(0, s1.length() - 2).append("ang");
        s3 = s1.mid(0, s1.length() - 2).append("re");
        s4 = s1.mid(0, s1.length() - 2).append("im");

        S_1 = values[index].toDouble();
        S_2 = values[index+1].toDouble();

        convert_MA_RI_to_dB(&S_1, &S_2, &S_3, &S_4, format);

        file_data[s1].append(S_1);//dB
        file_data[s2].append(S_2);//ang
        file_data[s3].append(S_3);//re
        file_data[s4].append(S_4);//im
        index += 2;
        data_counter++;

               // Check if the next values are in the new line
        if ((index >= values.length()) && (data_counter < number_of_ports*number_of_ports)){
          line = in.readLine();
          line = line.simplified();
          values = line.split(' ');
          index = 0; // Reset index (it's a new line)
        }
      }
    }
  }

  file.close();
  return file_data;
}


// Given a string path to a file, it reads the Qucs dataset into the main dataset
QMap<QString, QList<double>> Qucs_S_SPAR_Viewer::readQucsatorDataset(const QString& filePath)
{
  QMap<QString, QList<double>> file_data; // Data structure to store the file data

  // 1) Open the file
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Cannot open the file";
    return file_data;
  }

  // 2) Read data
  QTextStream in(&file);
  QString line = in.readLine(); // First line should be <Qucs Dataset X.X.X>

  if (!line.contains("<Qucs Dataset")) {
    qDebug() << "Not a valid Qucs dataset file";
    file.close();
    return file_data;
  }

  // Initialize variables
  QString currentVariable;
  bool isReading = false;
  int maxPortNumber = 0; // Track maximum port number
  double z0Value = 50.0; // Default Z0 value

  while (!in.atEnd()) {
    line = in.readLine().trimmed();

    if (line.isEmpty()) continue;

    // Handle variable declaration lines
    if (line.startsWith("<indep ") || line.startsWith("<dep ")) {
      isReading = false;
      QStringList parts = line.split(" ");

      if (parts.size() >= 3) {
        currentVariable = parts[1];

        // Check if it's frequency
        if (line.startsWith("<indep ") && currentVariable == "frequency") {
          isReading = true;
        }
        // Check if it's Z0 (reference impedance)
        else if (line.startsWith("<indep ") && currentVariable == "Z0") {
          isReading = true;
        }
        // Check if it's an S-parameter in matrix form S[i,j]
        else if (line.startsWith("<dep ") && currentVariable.startsWith("S[")) {
          isReading = true;

          // Extract port numbers to determine maximum port
          QRegularExpression re("S\\[(\\d+),(\\d+)\\]");
          QRegularExpressionMatch match = re.match(currentVariable);

          if (match.hasMatch()) {
            int row = match.captured(1).toInt();
            int col = match.captured(2).toInt();
            maxPortNumber = qMax(maxPortNumber, qMax(row, col));
          }
        }
        else {
          // Skip other variables
          isReading = false;
          currentVariable = "";
        }
      }
    }
    // Handle data values
    else if (!currentVariable.isEmpty() && !line.startsWith("<")) {
      if (currentVariable == "frequency" && isReading) {
        // Store frequency value (in Hz)
        file_data["frequency"].append(line.toDouble());
      }
      else if (currentVariable == "Z0" && isReading) {
        // Store Z0 value
        z0Value = line.toDouble();
      }
      else if (currentVariable.startsWith("S[") && isReading) {
        // Handle S[i,j] complex format
        QRegularExpression reComplex("([+-]?\\d+\\.\\d+e[+-]\\d+)([+-])j(\\d+\\.\\d+e[+-]\\d+)");
        QRegularExpressionMatch matchComplex = reComplex.match(line);

        // Handle S[i,j] real format
        QRegularExpression reReal("([+-]?\\d+\\.\\d+e[+-]\\d+)");
        QRegularExpressionMatch matchReal = reReal.match(line);

        // Extract indices from S[i,j]
        QRegularExpression indexRe("S\\[(\\d+),(\\d+)\\]");
        QRegularExpressionMatch indexMatch = indexRe.match(currentVariable);

        if (indexMatch.hasMatch()) {
          int i = indexMatch.captured(1).toInt();
          int j = indexMatch.captured(2).toInt();
          // Convert to Sji format (where j is row, i is column)
          QString sparam = QString::number(j) + QString::number(i);
          QString base = "S" + sparam;

          double real, imag;

          if (matchComplex.hasMatch()) {
            // Parse complex number
            real = matchComplex.captured(1).toDouble();
            imag = matchComplex.captured(3).toDouble();
            if (matchComplex.captured(2) == "-") imag = -imag;
  }
          else if (matchReal.hasMatch()) {
            // Parse real number (imaginary part is zero)
            real = matchReal.captured(1).toDouble();
            imag = 0.0;
          }
          else {
            // Skip if no match
            continue;
          }

          // Calculate magnitude in dB and angle
          double mag = sqrt(real * real + imag * imag);
          double mag_db;
          if (mag == 0) {
            mag_db = -300;
          } else {
            mag_db = 20 * log10(mag);
          }
          double ang = atan2(imag, real) * 180 / M_PI;

          // Store values
          file_data[base + "_re"].append(real);
          file_data[base + "_im"].append(imag);
          file_data[base + "_dB"].append(mag_db);
          file_data[base + "_ang"].append(ang);
        }
      }
    }
  }

  file.close();

  // Store the number of ports based on the maximum port number found
  file_data["n_ports"].append(maxPortNumber);

  // Store Z0 value
  file_data["Z0"].append(z0Value);

  return file_data;
}


QMap<QString, QList<double>> Qucs_S_SPAR_Viewer::readNGspiceData(const QString& filePath)
{
  QMap<QString, QList<double>> file_data; // Data structure to store the file data

  // 1) Open the file
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Cannot open the file";
    return file_data;
  }

  // 2) Read data
  QTextStream in(&file);
  QString line = in.readLine(); // First line should be <Qucs Dataset X.X.X>

  if (!line.contains("<Qucs Dataset")) {
    qDebug() << "Not a valid Qucs dataset file";
    file.close();
    return file_data;
  }

  // Initialize variables
  QString currentVariable;
  bool isReading = false;
  int maxPortNumber = 0; // Track maximum port number
  double z0Value = 50.0; // Default Z0 value
  bool z0Found = false;  // Flag to track if Z0 has been found

  while (!in.atEnd()) {
    line = in.readLine().trimmed();

    if (line.isEmpty()) continue;

           // Handle variable declaration lines
    if (line.startsWith("<indep ") || line.startsWith("<dep ")) {
      isReading = false;
      QStringList parts = line.split(" ");

      if (parts.size() >= 3) {
        currentVariable = parts[1];

               // Check if it's frequency
        if (line.startsWith("<indep ") && currentVariable == "frequency") {
          isReading = true;
        }
        // Check if it's the reference impedance Z0
        else if (line.startsWith("<dep ") && currentVariable == "ac.z0") {
          isReading = true;
        }
        // Check if it's an S-parameter in NGspice format ac.v(s_j_i)
        else if (line.startsWith("<dep ") && currentVariable.contains("ac.v(s_")) {
          isReading = true;

                 // Extract port numbers to determine maximum port
          QRegularExpression re("ac\\.v\\(s_(\\d+)_(\\d+)\\)");
          QRegularExpressionMatch match = re.match(currentVariable);

          if (match.hasMatch()) {
            int row = match.captured(1).toInt();
            int col = match.captured(2).toInt();
            maxPortNumber = qMax(maxPortNumber, qMax(row, col));
          }
        }
        else {
          // Skip other variables (like y and z parameters)
          isReading = false;
          currentVariable = "";
        }
      }
    }
    // Handle data values
    else if (!currentVariable.isEmpty() && !line.startsWith("<")) {
      if (currentVariable == "frequency" && isReading) {
        // Store frequency value (in Hz)
        file_data["frequency"].append(line.toDouble());
      }
      else if (currentVariable == "ac.z0" && isReading && !z0Found) {
        // Parse the Z0 value from complex format, only use the first value
        QRegularExpression re("([+-]?\\d+\\.\\d+e[+-]\\d+)\\+j(\\d+\\.\\d+e[+-]\\d+)");
        QRegularExpressionMatch match = re.match(line);

        if (match.hasMatch()) {
          // Only use the real part for Z0 (imaginary is typically 0)
          z0Value = match.captured(1).toDouble();
          z0Found = true; // Set flag to indicate Z0 has been found
        }
      }
      else if (currentVariable.contains("ac.v(s_") && isReading) {
        // Handle NGspice complex format for S-parameters
        QRegularExpression re("([+-]?\\d+\\.\\d+e[+-]\\d+)([+-])j(\\d+\\.\\d+e[+-]\\d+)");
        QRegularExpressionMatch match = re.match(line);

        if (match.hasMatch()) {
          // Extract indices from ac.v(s_j_i)
          QRegularExpression indexRe("ac\\.v\\(s_(\\d+)_(\\d+)\\)");
          QRegularExpressionMatch indexMatch = indexRe.match(currentVariable);

          if (indexMatch.hasMatch()) {
            int j = indexMatch.captured(1).toInt();
            int i = indexMatch.captured(2).toInt();

                   // Convert to Sji format (where j is row, i is column)
            QString sparam = QString::number(j) + QString::number(i);

                   // Parse complex number
            double real = match.captured(1).toDouble();
            double imag = match.captured(3).toDouble();
            if (match.captured(2) == "-") imag = -imag;

                   // Store as re, im, dB, and ang
            QString base = "S" + sparam;

                   // Calculate magnitude in dB and angle
            double mag = sqrt(real * real + imag * imag);
            double mag_db = 20 * log10(mag);
            double ang = atan2(imag, real) * 180 / M_PI;

            file_data[base + "_re"].append(real);
            file_data[base + "_im"].append(imag);
            file_data[base + "_dB"].append(mag_db);
            file_data[base + "_ang"].append(ang);
          }
        }
      }
    }
  }

  file.close();

         // Store the number of ports based on the maximum port number found
  file_data["n_ports"].append(maxPortNumber);

         // Store Z0 value
  file_data["Z0"].append(z0Value);

  return file_data;
}

// Helper function to extract S-parameter indices from S[i,j] format
QString Qucs_S_SPAR_Viewer::extractSParamIndices(const QString& sparam)
{
  QRegularExpression re("S\\[(\\d+),(\\d+)\\]");
  QRegularExpressionMatch match = re.match(sparam);

  if (match.hasMatch()) {
    QString i = match.captured(2); // Second index in S[i,j]
    QString j = match.captured(1); // First index in S[i,j]
    return j + i; // Return in Sji format
  }

  return "";
}

// Once a file is loaded, this function adds to the display the default traces based in its nature
void Qucs_S_SPAR_Viewer::applyDefaultVisualizations(const QStringList& fileNames)
{
  if (fileNames.length() == 1) {
    QString filename = QFileInfo(fileNames.first()).fileName();
    filename = filename.left(filename.lastIndexOf('.'));

    if (filename.endsWith("dat")) {
      // Then it must be a .dat.ngspice extension
      filename = filename.left(filename.lastIndexOf('.'));
    }

           // Default behavior: If there's no more data loaded and a single S1P file is selected
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

           // Default behavior: If there's no more data loaded and a single S2P file is selected
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
         // Default behaviour: When adding multiple S2P file, then show the S21 of all traces
  if (fileNames.length() > 1) {
    bool all_s2p = true;
    for (const QString &key : datasets.keys()) { // Iterate over the keys of the map
      if (datasets[key]["n_ports"].at(0) != 2) {
        all_s2p = false;
        break;
      }
    }


    if (all_s2p == true) {
      for (int i = 0; i < fileNames.length(); i++) {
        QString filename = QFileInfo(fileNames.at(i)).fileName();
        filename = filename.left(filename.lastIndexOf('.'));

        // Color settings. The color of the first traces match the default policy
        QColor trace_color;
        if (i < default_colors.size()-1) {
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
void Qucs_S_SPAR_Viewer::addOptionalTraces(QMap<QString, QList<double>>& file_data)
{
  QStringList optional_traces;

  int number_of_ports = file_data["n_ports"].at(0);

  if (number_of_ports == 1) {
    optional_traces.append("Re{Zin}");
    optional_traces.append("Im{Zin}");
    optional_traces.append("VSWR{in}");
  }
  else if (number_of_ports == 2) {
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
void Qucs_S_SPAR_Viewer::CreateFileWidgets(QString filename, int position = 0) {

  if (position == 0) {
    position = this->datasets.size();
  }

  QLabel * Filename_Label = new QLabel(filename.left(filename.lastIndexOf('.')));
  Filename_Label->setObjectName(QStringLiteral("File_") + QString::number(position));
  List_FileNames.append(Filename_Label);
  filePaths.append(filename); // Add the file to the watchlist

  this->FilesGrid->addWidget(List_FileNames.last(), position,0,1,1);

  // Create the "Remove" button
  QToolButton * RemoveButton = new QToolButton();
  RemoveButton->setObjectName(QStringLiteral("Remove_") + QString::number(position));
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
  this->FilesGrid->addWidget(List_RemoveButton.last(), position,1,1,1);


  connect(RemoveButton, SIGNAL(clicked()), SLOT(removeFile())); // Connect button with the handler to remove the entry.

}




void Qucs_S_SPAR_Viewer::removeFile(QString ID)
{
  // Find the row number of the button to remove
  int row_to_remove = -1;
  QString dataset_to_remove;

  for (int i = 0; i < List_RemoveButton.size(); i++) {
    if (List_RemoveButton.at(i)->objectName() == ID) {
      row_to_remove = i;
      dataset_to_remove = List_FileNames.at(i)->text();
      delete List_RemoveButton.takeAt(i);  // Use takeAt() instead of at()
      delete List_FileNames.takeAt(i);     // Removes AND returns the pointer
      break;
    }
  }

  // Once removed the widgets, fill the gap in the layout
  if (row_to_remove >= 0) {
    removeAndCollapseRow(FilesGrid, row_to_remove);

    // Find all traces belonging to this dataset and remove them
    removeTracesByDataset(dataset_to_remove);
  }

  datasets.remove(dataset_to_remove);

  // Update datasets' combobox
  int index = QCombobox_datasets->findText(dataset_to_remove);
  QCombobox_datasets->removeItem(index);
  updateTracesCombo();
}


// This function is called whenever a s-par file is intended to be removed from the map of datasets
void Qucs_S_SPAR_Viewer::removeFile()
{
  QToolButton* button = qobject_cast<QToolButton*>(sender());
  QString ID = button->objectName();
  removeFile(ID);
}


// Given the name of a dataset, this function removes all traces related to it
void Qucs_S_SPAR_Viewer::removeTracesByDataset(const QString& dataset_to_remove) {
  // Iterate through the outer QMap (display modes)

  for (const DisplayMode& mode : traceMap.keys()) {
    QMap<QString, TraceProperties>& traces = traceMap[mode];

    // Iterate through the inner QMap (traces in the current mode)
    for (auto trace_it = traces.begin(); trace_it != traces.end(); ) {
      const QString& trace_name = trace_it.key();

      // Check if the trace belongs to the dataset to remove
      if (trace_name.startsWith(dataset_to_remove + ".")) {
        TraceProperties& props = trace_it.value();

        // Use the common function to remove the trace
        // We need to make a copy of the trace name because it will be invalidated
        // when the trace is removed from the map
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


void Qucs_S_SPAR_Viewer::removeAllFiles()
{
  // Remove files
  QStringList fileIDs;
  for (int i = 0; i < List_RemoveButton.size(); i++) {
    fileIDs.append(List_RemoveButton.at(i)->objectName());
  }

  // Remove each file by ID
  for (const QString& ID : fileIDs) {
    removeFile(ID);
  }

  // Remove all paths from the file watcher
  if (!fileWatcher->files().isEmpty()) {
    fileWatcher->removePaths(fileWatcher->files());
  }

  // Clear the watchedFilePaths map
  watchedFilePaths.clear();
}


void Qucs_S_SPAR_Viewer::removeTrace() {
  QToolButton* button = qobject_cast<QToolButton*>(sender());
  QString ID = button->objectName();
  ID.remove("Trace_RemoveButton_");

         // Find the display mode from the parent widget
  QWidget* scroll = button->parentWidget()->parentWidget()->parentWidget();
  QString scrollname = scroll->objectName();

  DisplayMode mode;
  if (!scrollname.compare(QString("magnitudePhaseScrollArea"))) {
    mode = DisplayMode::Magnitude_dB;
  } else if (!scrollname.compare(QString("smithScrollArea"))) {
    mode = DisplayMode::Smith;
  } else if (!scrollname.compare(QString("polarScrollArea"))) {
    mode = DisplayMode::Polar;
  } else if (!scrollname.compare(QString("portImpedanceScrollArea"))) {
    mode = DisplayMode::PortImpedance;
  } else if (!scrollname.compare(QString("stabilityScrollArea"))) {
    mode = DisplayMode::Stability;
  } else if (!scrollname.compare(QString("VSWRScrollArea"))) {
    mode = DisplayMode::VSWR;
  } else if (!scrollname.compare(QString("GroupDelayScrollArea"))) {
    mode = DisplayMode::GroupDelay;
  }

         // Get trace properties and call the common removal function
  TraceProperties& props = traceMap[mode][ID];
  removeTraceByProps(mode, ID, props);
}


void Qucs_S_SPAR_Viewer::removeTraceByProps(DisplayMode mode, const QString& traceID, TraceProperties& props) {
  // 1) Find the right layout
  QGridLayout *targetLayout;

         // Remove trace from the layout and from the chart
  switch (mode) {
  case DisplayMode::Magnitude_dB:
  case DisplayMode::Phase:
    targetLayout = magnitudePhaseLayout;
    Magnitude_PhaseChart->removeTrace(traceID);
    break;
  case DisplayMode::Smith:
    targetLayout = smithLayout;
    smithChart->removeTrace(traceID);
    break;
  case DisplayMode::Polar:
    targetLayout = polarLayout;
    polarChart->removeTrace(traceID);
    break;
  case DisplayMode::PortImpedance:
    targetLayout = portImpedanceLayout;
    impedanceChart->removeTrace(traceID);
    break;
  case DisplayMode::Stability:
    targetLayout = stabilityLayout;
    stabilityChart->removeTrace(traceID);
    break;
  case DisplayMode::VSWR:
    targetLayout = VSWRLayout;
    VSWRChart->removeTrace(traceID);
    break;
  case DisplayMode::GroupDelay:
    targetLayout = GroupDelayLayout;
    GroupDelayChart->removeTrace(traceID);
    break;
  }

         // 2) Get the row number of the widgets in the grid for filling gaps after removal
  int index = targetLayout->indexOf(props.nameLabel);
  int row_to_remove, col, rowSpan, colSpan;
  targetLayout->getItemPosition(index, &row_to_remove, &col, &rowSpan, &colSpan);

         // 3) Remove widgets
  targetLayout->removeWidget(props.nameLabel);
  delete props.nameLabel;
  props.nameLabel = nullptr;

  targetLayout->removeWidget(props.colorButton);
  delete props.colorButton;
  props.colorButton = nullptr;

  targetLayout->removeWidget(props.LineStyleComboBox);
  delete props.LineStyleComboBox;
  props.LineStyleComboBox = nullptr;

  targetLayout->removeWidget(props.width);
  delete props.width;
  props.width = nullptr;

  targetLayout->removeWidget(props.deleteButton);
  delete props.deleteButton;
  props.deleteButton = nullptr;

         // 4) Remove trace from the map
  traceMap[mode].remove(traceID);

         // 5) Fill the gap in the layout
  removeAndCollapseRow(targetLayout, row_to_remove);
}



void Qucs_S_SPAR_Viewer::removeAndCollapseRow(QGridLayout* targetLayout, int row_to_remove) {
  int rows = targetLayout->rowCount();
  int columns = targetLayout->columnCount();

  // 1. Remove all widgets in target row (if not already done)
  for(int col = 0; col < columns; ++col) {
    QLayoutItem* item = targetLayout->itemAtPosition(row_to_remove, col);
    if(item && item->widget()) {
      targetLayout->removeWidget(item->widget());
      delete item->widget();
    }
  }

  // 2. Shift all rows below upward
  for(int r = row_to_remove + 1; r < rows; ++r) {
    for(int c = 0; c < columns; ++c) {
      QLayoutItem* item = targetLayout->itemAtPosition(r, c);
      if(item && item->widget()) {
        QWidget* widget = item->widget();
        targetLayout->removeWidget(widget);
        targetLayout->addWidget(widget, r-1, c); // Move up one row
      }
    }
  }

  // 3.emove empty last row (if needed)
  if(row_to_remove == rows - 1) {
    // QGridLayout doesn't have removeRow(), so we must force layout update
    QWidget* container = targetLayout->parentWidget();
    if(container) {
      container->setUpdatesEnabled(false);
      // Add temporary dummy widget to last row to force recalc
      QWidget* temp = new QWidget();
      targetLayout->addWidget(temp, row_to_remove, 0);
      targetLayout->removeWidget(temp);
      delete temp;
      container->setUpdatesEnabled(true);
    }
  }
}



bool Qucs_S_SPAR_Viewer::removeSeriesByName(QChart* chart, const QString& name)
{
    QList<QAbstractSeries*> seriesList = chart->series();
    for (QAbstractSeries* series : seriesList) {
        if (series->name() == name) {
            chart->removeSeries(series);
            return true; // Series found and removed
        }
    }
    return false; // Series not found
}


void Qucs_S_SPAR_Viewer::convert_MA_RI_to_dB(double * S_1, double * S_2, double *S_3, double *S_4, QString format)
{
    double S_dB = *S_1, S_ang =*S_2;
    double S_re = *S_3, S_im = *S_4;
    if (format == "MA"){
        S_dB = 20*log10(*S_1);
        S_ang = *S_2;
        S_re = *S_1 * std::cos(*S_2 * M_PI/180);
        S_im = *S_1 * std::sin(*S_2 * M_PI/180);
    }else{
        if (format == "RI"){
        S_dB = 20*log10(sqrt((*S_1)*(*S_1) + (*S_2)*(*S_2)));
        S_ang = atan2(*S_2, *S_1) * 180 / M_PI;
        S_re = *S_1;
        S_im = *S_2;
        } else {
            // DB format
            double r = std::pow(10, *S_1 / 10.0);
            double theta = *S_2 * M_PI / 180.0;
            S_re = r * std::cos(theta);
            S_im = r * std::sin(theta);
        }

    }
    *S_1 = S_dB;
    *S_2 = S_ang;
    *S_3 = S_re;
    *S_4 = S_im;
}

// Gets the frequency scale unit from a String lke kHz, MHz, GHz
double Qucs_S_SPAR_Viewer::getFreqScale(QString frequency_unit)
{
    double freq_scale=1;
    if (frequency_unit == "kHz"){
        freq_scale = 1e-3;
    } else {
        if (frequency_unit == "MHz"){
            freq_scale = 1e-6;
        } else {
            if (frequency_unit == "GHz"){
                freq_scale = 1e-9;
            }
        }
    }
    return freq_scale;
}

// This function is called when the user hits the button to add a trace
void Qucs_S_SPAR_Viewer::addTrace()
{
  // Create a TraceInfo object from UI selections
  TraceInfo traceInfo;
  traceInfo.dataset = this->QCombobox_datasets->currentText();
  traceInfo.parameter = this->QCombobox_traces->currentText();

  // Convert display mode selection to enum
  QString displayModeText = this->QCombobox_display_mode->currentText();
  if (displayModeText == "dB") {
    traceInfo.displayMode = DisplayMode::Magnitude_dB;
  } else if (displayModeText == "Phase") {
    traceInfo.displayMode = DisplayMode::Phase;
  } else if (displayModeText == "Smith") {
    traceInfo.displayMode = DisplayMode::Smith;
  } else if (displayModeText == "Polar") {
    traceInfo.displayMode = DisplayMode::Polar;
  } else if (displayModeText == "n.u.") {
    if (traceInfo.parameter.contains("VSWR")) {
      traceInfo.displayMode = DisplayMode::VSWR;
    } else {
      if (traceInfo.parameter.contains("{Z")) {
        traceInfo.displayMode = DisplayMode::PortImpedance;
      } else {
      traceInfo.displayMode = DisplayMode::Stability;
      }
    }
  } else if (displayModeText == "Group Delay") {
    traceInfo.displayMode = DisplayMode::GroupDelay;
  }

  // Set line width
  int linewidth = 1;

         // Color settings
  QColor trace_color;
  int num_traces = traceMap[traceInfo.displayMode].size(); // Number of traces in the display widget
  if (num_traces >= default_colors.size()) {
    trace_color = QColor(QRandomGenerator::global()->bounded(256),
                         QRandomGenerator::global()->bounded(256),
                         QRandomGenerator::global()->bounded(256));
  } else {
    trace_color = this->default_colors.at(num_traces);
  }

         // Call the overloaded addTrace with the trace info
  addTrace(traceInfo, trace_color, linewidth);
}


// Overloaded method that uses the TraceInfo structure
void Qucs_S_SPAR_Viewer::addTrace(const TraceInfo& traceInfo, QColor trace_color, int trace_width, QString trace_style)
{
  DisplayMode mode = traceInfo.displayMode;
  int n_trace = this->traceMap[mode].size() + 1; // Number of displayed traces

         // Get display name for the trace
  QString trace_name = traceInfo.displayName();

  // Check if the trace is already shown
  if (traceMap[mode].contains(trace_name)) {
    QMessageBox::information(
        this,
        tr("Warning"),
        tr("This trace is already shown"));
    return;
  }

         // Get the appropriate layout based on the display mode
  QGridLayout* targetLayout;
  QString displayMode;
  switch (mode) {
  case DisplayMode::Smith:
    targetLayout = smithLayout;
    displayMode = QString("Smith");
    break;
  case DisplayMode::Magnitude_dB:
    displayMode = QString("Magnitude");
    targetLayout = magnitudePhaseLayout;
    break;
  case DisplayMode::Phase:
    displayMode = QString("Phase");
    targetLayout = magnitudePhaseLayout;
    break;
  case DisplayMode::Polar:
    displayMode = QString("Polar");
    targetLayout = polarLayout;
    break;
  case DisplayMode::PortImpedance:
    displayMode = QString("Port Impedance");
    targetLayout = portImpedanceLayout;
    break;
  case DisplayMode::Stability:
    displayMode = QString("Stability");
    targetLayout = stabilityLayout;
    break;
  case DisplayMode::VSWR:
    displayMode = QString("VSWR");
    targetLayout = VSWRLayout;
    break;
  case DisplayMode::GroupDelay:
    displayMode = QString("Group Delay");
    targetLayout = GroupDelayLayout;
    break;
  }

   // Create UI widgets for the trace (mostly unchanged from original code)
   // Label
  QLabel *new_trace_label = new QLabel(trace_name);
  new_trace_label->setObjectName(QStringLiteral("Trace_Name_") + trace_name);
  traceMap[mode][trace_name].nameLabel = new_trace_label;
  targetLayout->addWidget(new_trace_label, n_trace, 0);

  // Color picker
  QPushButton *new_trace_color = new QPushButton();
  new_trace_color->setObjectName(QStringLiteral("Trace_Color_") + trace_name);
  connect(new_trace_color, SIGNAL(clicked()), SLOT(changeTraceColor()));
  QString styleSheet = QStringLiteral("QPushButton { background-color: %1; }").arg(trace_color.name());
  new_trace_color->setStyleSheet(styleSheet);
  new_trace_color->setAttribute(Qt::WA_TranslucentBackground);
  traceMap[mode][trace_name].colorButton = new_trace_color;
  targetLayout->addWidget(new_trace_color, n_trace, 1);

  // LineStyle
  QComboBox *new_trace_linestyle = new QComboBox();
  new_trace_linestyle->setObjectName(QStringLiteral("Trace_LineStyle_") + trace_name);
  new_trace_linestyle->addItem("Solid");
  new_trace_linestyle->addItem("- - - -");
  new_trace_linestyle->addItem("·······");
  new_trace_linestyle->addItem("-·-·-·-");
  new_trace_linestyle->addItem("-··-··-");
  int index = new_trace_linestyle->findText(trace_style);
  new_trace_linestyle->setCurrentIndex(index);
  connect(new_trace_linestyle, SIGNAL(currentIndexChanged(int)), SLOT(changeTraceLineStyle()));
  traceMap[mode][trace_name].LineStyleComboBox = new_trace_linestyle;
  targetLayout->addWidget(new_trace_linestyle, n_trace, 2);

  // Determine pen style
  Qt::PenStyle pen_style;
  if (!trace_style.compare("Solid")) {
    pen_style = Qt::SolidLine;
  } else if (!trace_style.compare("- - - -")) {
    pen_style = Qt::DashLine;
  } else if (!trace_style.compare("·······")) {
    pen_style = Qt::DotLine;
  } else if (!trace_style.compare("- · - · -")) {
    pen_style = Qt::DashDotLine;
  } else if (!trace_style.compare("-··-··-")) {
    pen_style = Qt::DashDotDotLine;
  }

  // Line width
  QSpinBox *new_trace_width = new QSpinBox();
  new_trace_width->setObjectName(QStringLiteral("Trace_Width_") + trace_name);
  new_trace_width->setValue(trace_width);
  connect(new_trace_width, SIGNAL(valueChanged(int)), SLOT(changeTraceWidth()));
  traceMap[mode][trace_name].width = new_trace_width;
  targetLayout->addWidget(new_trace_width, n_trace, 3);

  // Remove button
  QToolButton *new_trace_removebutton = new QToolButton();
  new_trace_removebutton->setObjectName(QStringLiteral("Trace_RemoveButton_") + trace_name);
  QIcon icon(":/bitmaps/trash.png");
  new_trace_removebutton->setIcon(icon);
  new_trace_removebutton->setStyleSheet(R"(
            QToolButton {
                background-color: #FF0000;
                color: white;
                border-radius: 20px;
                margin: auto;
            }
        )");
  connect(new_trace_removebutton, SIGNAL(clicked()), SLOT(removeTrace()));
  traceMap[mode][trace_name].deleteButton = new_trace_removebutton;
  targetLayout->addWidget(new_trace_removebutton, n_trace, 4);

  // Create the series for the trace
  QLineSeries* series = new QLineSeries();
  series->setName(trace_name);

  // Color settings
  QPen pen;
  pen.setColor(trace_color);
  pen.setStyle(pen_style);
  pen.setWidth(trace_width);
  series->setPen(pen); // Apply the pen to the series

  // Create and add the appropriate trace based on display mode
  QList<double> frequencies = datasets[traceInfo.dataset]["frequency"];
  double Z0 = datasets[traceInfo.dataset]["Z0"].first();

  // Process the trace based on display mode
  switch (mode) {
  case DisplayMode::Magnitude_dB:
  case DisplayMode::Phase: {
    // Determine which data column to use

    QString fullParam;

    if (traceInfo.parameter.startsWith("S")) {
      // If data is not MSG or MAG, then add the "_dB" or "_ang" suffix to indicate the data type
      QString dataSuffix;
      dataSuffix = (traceInfo.displayMode == DisplayMode::Magnitude_dB) ? "_dB" : "_ang";
      fullParam = traceInfo.parameter + dataSuffix;
    } else {
      // MSG or MAG. No need to add the "_dB" suffix
      fullParam = traceInfo.parameter;
    }

    // Calculate if needed
    if (datasets[traceInfo.dataset][fullParam].isEmpty()) {
      calculate_Sparameter_trace(traceInfo.dataset, fullParam);
    }

    QList<double> trace_data = datasets[traceInfo.dataset][fullParam];

    // Set up trace properties
    QString units = (traceInfo.displayMode == DisplayMode::Magnitude_dB) ? "dB" : "deg";
    int yaxis = (traceInfo.displayMode == DisplayMode::Magnitude_dB) ? 1 : 2;
    QString yaxis_title = (traceInfo.displayMode == DisplayMode::Magnitude_dB) ?
                              "Magnitude (dB)" : "Phase (deg)";

    // Add the trace to the chart
    RectangularPlotWidget::Trace new_trace;
    new_trace.trace = trace_data;
    new_trace.frequencies = frequencies;
    new_trace.pen = pen;
    new_trace.Z0 = Z0;
    new_trace.units = units;
    new_trace.y_axis = yaxis;
    new_trace.y_axis_title = yaxis_title;
    Magnitude_PhaseChart->addTrace(trace_name, new_trace);
    break;
  }

  case DisplayMode::Smith: {
    // Convert S-parameters to impedances
    QList<std::complex<double>> impedances;

    QList<double> sii_re = datasets[traceInfo.dataset][traceInfo.parameter + "_re"];
    QList<double> sii_im = datasets[traceInfo.dataset][traceInfo.parameter + "_im"];

    for (int i = 0; i < frequencies.size(); i++) {
      std::complex<double> sii(sii_re[i], sii_im[i]);
      std::complex<double> gamma = sii; // Reflection coefficient
      std::complex<double> impedance = Z0 * (1.0 + gamma) / (1.0 - gamma); // Convert to impedance
      impedances.push_back(impedance);
    }

    // Set the impedance data to the Smith Chart widget
    SmithChartWidget::Trace new_trace;
    new_trace.impedances = impedances;
    new_trace.frequencies = frequencies;
    new_trace.pen = pen;
    new_trace.Z0 = Z0;

    SmithChartTraces.append(new_trace);

    QString TraceName = traceInfo.dataset + "." + traceInfo.parameter;
    smithChart->addTrace(TraceName, new_trace);
    break;
  }

  case DisplayMode::Polar: {
    // Polar plot
    QList<double> sij_re = datasets[traceInfo.dataset][traceInfo.parameter + "_re"];
    QList<double> sij_im = datasets[traceInfo.dataset][traceInfo.parameter + "_im"];

    QList<std::complex<double>> S;
    for (int i = 0; i < frequencies.size(); i++) {
      std::complex<double> sij(sij_re[i], sij_im[i]);
      S.push_back(sij);
    }

    // Set the data to the Polar Chart widget
    PolarPlotWidget::Trace new_trace;
    new_trace.frequencies = frequencies;
    new_trace.values = S;
    new_trace.pen = pen;

    PolarChartTraces.append(new_trace);

    QString TraceName = traceInfo.dataset + "." + traceInfo.parameter;
    polarChart->addTrace(TraceName, new_trace);
    break;
  }

  case DisplayMode::GroupDelay: {
    // Group Delay trace name
    QString fullParam = traceInfo.parameter + "_Group Delay";

    // Calculate if needed
    if (datasets[traceInfo.dataset][fullParam].isEmpty()) {
      calculate_Sparameter_trace(traceInfo.dataset, fullParam);
    }

    QList<double> trace_data = datasets[traceInfo.dataset][fullParam];

    RectangularPlotWidget::Trace new_trace;
    new_trace.frequencies = frequencies;
    new_trace.trace = trace_data;
    new_trace.pen = pen;
    new_trace.units = "ns";
    new_trace.y_axis = 1;
    new_trace.y_axis_title = fullParam;

    GroupDelayTraces.append(new_trace);

    QString TraceName = traceInfo.dataset + "." + fullParam;
    GroupDelayChart->addTrace(TraceName, new_trace);
    break;
  }

  case DisplayMode::Stability: {
    // Group Delay trace name
    QString fullParam = traceInfo.parameter;

           // Calculate if needed
    if (datasets[traceInfo.dataset][fullParam].isEmpty()) {
      calculate_Sparameter_trace(traceInfo.dataset, fullParam);
    }

    QList<double> trace_data = datasets[traceInfo.dataset][fullParam];

    RectangularPlotWidget::Trace new_trace;
    new_trace.frequencies = frequencies;
    new_trace.trace = trace_data;
    new_trace.pen = pen;
    new_trace.units = "";
    new_trace.y_axis = 1;
    new_trace.y_axis_title = "Time (ns)";

    stabilityChartTraces.append(new_trace);

    QString TraceName = traceInfo.dataset + "." + fullParam;
    stabilityChart->addTrace(TraceName, new_trace);
    break;
  }

  case DisplayMode::VSWR: {
    // Group Delay trace name
    QString fullParam = traceInfo.parameter;

           // Calculate if needed
    if (datasets[traceInfo.dataset][fullParam].isEmpty()) {
      calculate_Sparameter_trace(traceInfo.dataset, fullParam);
    }

    QList<double> trace_data = datasets[traceInfo.dataset][fullParam];

    RectangularPlotWidget::Trace new_trace;
    new_trace.frequencies = frequencies;
    new_trace.trace = trace_data;
    new_trace.pen = pen;
    new_trace.units = "";
    new_trace.y_axis = 1;
    new_trace.y_axis_title = "VSWR";

    VSWRChartTraces.append(new_trace);

    QString TraceName = traceInfo.dataset + "." + fullParam;
    VSWRChart->addTrace(TraceName, new_trace);
    break;
  }

  case DisplayMode::PortImpedance: {
    // Port impedance units display
    if (traceInfo.parameter.startsWith("S")) {
      // S-parameter. Real part -> left-y. Imaginary part -> right-y
      QList<double> sij_re = datasets[traceInfo.dataset][traceInfo.parameter + "_re"];
      QList<double> sij_im = datasets[traceInfo.dataset][traceInfo.parameter + "_im"];

      // Add appropriate handling for S-parameters in natural units
      // (This part of the code wasn't fully implemented in the original)
    } else {
      // Other parameters (like Re{Zin}, Im{Zin}, etc.)
      // Calculate if needed
      if (datasets[traceInfo.dataset][traceInfo.parameter].isEmpty()) {
        calculate_Sparameter_trace(traceInfo.dataset, traceInfo.parameter);
      }

      QList<double> trace_data = datasets[traceInfo.dataset][traceInfo.parameter];

      // Determine display characteristics
      QString units = "Ω";
      int yaxis = 1;
      QString y_axis_title;

      if (traceInfo.parameter.contains("Im{")) {
        yaxis = 2;
      }

      RectangularPlotWidget::Trace new_trace;
      new_trace.frequencies = frequencies;
      new_trace.trace = trace_data;
      new_trace.pen = pen;
      new_trace.units = units;
      new_trace.y_axis = yaxis;
      new_trace.y_axis_title = y_axis_title;

      impedanceChartTraces.append(new_trace);

      QString TraceName = traceInfo.dataset + "." + traceInfo.parameter;
      impedanceChart->addTrace(TraceName, new_trace);
    }
    break;
  }
  }
}



// This function is used for setting the available traces depending on the selected dataset
void Qucs_S_SPAR_Viewer::updateTracesCombo()
{
  QCombobox_traces->clear();
  QStringList sParams;
  QStringList otherParams;
  QString current_dataset = QCombobox_datasets->currentText();
  if (current_dataset.isEmpty()){
    return; // No datasets loaded. This happens if the user had one single file and deleted it
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
    otherParams.append(QStringLiteral("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209B)));
    otherParams.append(QStringLiteral("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209A)));
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

// This function adjust the display types available depending on the trace selected
void Qucs_S_SPAR_Viewer::updateDisplayType(){
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

// This is the handler that is triggered when the user hits the button to change the color of a given trace
void Qucs_S_SPAR_Viewer::changeTraceColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Select Color");
            if (color.isValid()) {
                // Do something with the selected color
                // For example, set the background color of the button
                QPushButton *button = qobject_cast<QPushButton*>(sender());
                if (button) {

                  // 1) Find the display mode

                  QWidget* scroll = button->parentWidget()->parentWidget()->parentWidget();
                  QString scrollname = scroll->objectName();

                  DisplayMode mode;
                  if (!scrollname.compare(QString("magnitudePhaseScrollArea"))) {
                    mode = DisplayMode::Magnitude_dB;
                  } else if (!scrollname.compare(QString("smithScrollArea"))) {
                    mode = DisplayMode::Smith;
                  } else if (!scrollname.compare(QString("polarScrollArea"))) {
                    mode = DisplayMode::Polar;
                  } else if (!scrollname.compare(QString("portImpedanceScrollArea"))) {
                    mode = DisplayMode::PortImpedance;
                  } else if (!scrollname.compare(QString("stabilityScrollArea"))) {
                    mode = DisplayMode::Stability;
                  } else if (!scrollname.compare(QString("VSWRScrollArea"))) {
                    mode = DisplayMode::VSWR;
                  } else if (!scrollname.compare(QString("GroupDelayScrollArea"))) {
                    mode = DisplayMode::GroupDelay;
                  }


                  // 2) Modify stylesheet
                  QString styleSheet = QStringLiteral("QPushButton { background-color: %1; }").arg(color.name());
                  button->setStyleSheet(styleSheet);

                  QString ID = button->objectName();
                  ID.remove("Trace_Color_"); // Remove the preffix


                  // 3) Remove trace from the layout and from the chart
                  QPen currentPen(Qt::black);  // Explicit constructor (to avoid warnings)
                  currentPen.setStyle(Qt::SolidLine);
                  currentPen.setWidth(1);

                  switch (mode) {
                  case DisplayMode::Magnitude_dB:
                  case DisplayMode::Phase:
                    currentPen = Magnitude_PhaseChart->getTracePen(ID);
                    currentPen.setColor(color);
                    Magnitude_PhaseChart->setTracePen(ID, currentPen);
                    break;
                  case DisplayMode::Smith:
                    currentPen = smithChart->getTracePen(ID);
                    currentPen.setColor(color);
                    smithChart->setTracePen(ID, currentPen);
                    break;
                  case DisplayMode::Polar:
                    currentPen = polarChart->getTracePen(ID);
                    currentPen.setColor(color);
                    polarChart->setTracePen(ID, currentPen);
                    break;
                  case DisplayMode::PortImpedance:
                    currentPen = impedanceChart->getTracePen(ID);
                    currentPen.setColor(color);
                    impedanceChart->setTracePen(ID, currentPen);
                    break;
                  case DisplayMode::Stability:
                    currentPen = stabilityChart->getTracePen(ID);
                    currentPen.setColor(color);
                    stabilityChart->setTracePen(ID, currentPen);
                    break;
                  case DisplayMode::VSWR:
                    currentPen = VSWRChart->getTracePen(ID);
                    currentPen.setColor(color);
                    VSWRChart->setTracePen(ID, currentPen);
                    break;
                  case DisplayMode::GroupDelay:
                    currentPen = GroupDelayChart->getTracePen(ID);
                    currentPen.setColor(color);
                    GroupDelayChart->setTracePen(ID, currentPen);
                    break;
                  }
                }
            }
}

// This is the handler that is triggered when the user hits the button to change the line style of a given trace
void Qucs_S_SPAR_Viewer::changeTraceLineStyle()
{
    QComboBox *combo = qobject_cast<QComboBox*>(sender());
    QString ID = combo->objectName();


    // 1) Find the display mode

    QWidget* scroll = combo->parentWidget()->parentWidget()->parentWidget();
    QString scrollname = scroll->objectName();

    DisplayMode mode;
    if (!scrollname.compare(QString("magnitudePhaseScrollArea"))) {
      mode = DisplayMode::Magnitude_dB;
    } else if (!scrollname.compare(QString("smithScrollArea"))) {
      mode = DisplayMode::Smith;
    } else if (!scrollname.compare(QString("polarScrollArea"))) {
      mode = DisplayMode::Polar;
    } else if (!scrollname.compare(QString("portImpedanceScrollArea"))) {
      mode = DisplayMode::PortImpedance;
    } else if (!scrollname.compare(QString("stabilityScrollArea"))) {
      mode = DisplayMode::Stability;
    } else if (!scrollname.compare(QString("VSWRScrollArea"))) {
      mode = DisplayMode::VSWR;
    } else if (!scrollname.compare(QString("GroupDelayScrollArea"))) {
      mode = DisplayMode::GroupDelay;
    }

    // 2) Get the trace name
    ID.remove("Trace_LineStyle_"); // Remove the preffix

    // New trace line style
    enum Qt::PenStyle PenStyle;
    switch (combo->currentIndex()) {
      case 0: // Solid
        PenStyle = Qt::SolidLine;
        break;
      case 1: // Dashed
        PenStyle = Qt::DashLine;
        break;
      case 2: // Dotted
        PenStyle = Qt::DotLine;
        break;
      case 3: // Dash Dot
        PenStyle = Qt::DashDotLine;
        break;
      case 4: // Dash Dot Dot Line
        PenStyle = Qt::DashDotDotLine;
        break;
    }


    // 3) Remove trace from the layout and from the chart
    QPen currentPen(Qt::black);  // Explicit constructor (to avoid warnings)
    currentPen.setStyle(Qt::SolidLine);
    currentPen.setWidth(1);

    switch (mode) {
    case DisplayMode::Magnitude_dB:
    case DisplayMode::Phase:
      currentPen = Magnitude_PhaseChart->getTracePen(ID);
      currentPen.setStyle(PenStyle);
      Magnitude_PhaseChart->setTracePen(ID, currentPen);
      break;
    case DisplayMode::Smith:
      currentPen = smithChart->getTracePen(ID);
      currentPen.setStyle(PenStyle);
      smithChart->setTracePen(ID, currentPen);
      break;
    case DisplayMode::Polar:
      currentPen = polarChart->getTracePen(ID);
      currentPen.setStyle(PenStyle);
      polarChart->setTracePen(ID, currentPen);
      break;
    case DisplayMode::PortImpedance:
      currentPen = impedanceChart->getTracePen(ID);
      currentPen.setStyle(PenStyle);
      impedanceChart->setTracePen(ID, currentPen);
      break;
    case DisplayMode::Stability:
      currentPen = stabilityChart->getTracePen(ID);
      currentPen.setStyle(PenStyle);
      stabilityChart->setTracePen(ID, currentPen);
      break;
    case DisplayMode::VSWR:
      currentPen = VSWRChart->getTracePen(ID);
      currentPen.setStyle(PenStyle);
      VSWRChart->setTracePen(ID, currentPen);
      break;
    case DisplayMode::GroupDelay:
       currentPen = GroupDelayChart->getTracePen(ID);
      currentPen.setStyle(PenStyle);
      GroupDelayChart->setTracePen(ID, currentPen);
      break;
    }
}

// This is the handler that is triggered when the user hits the button to change the line width of a given trace
void Qucs_S_SPAR_Viewer::changeTraceWidth() {
  QSpinBox *spinbox = qobject_cast<QSpinBox*>(sender());
  QString ID = spinbox->objectName();

  // 1) Find the display mode

  QWidget* scroll = spinbox->parentWidget()->parentWidget()->parentWidget();
  QString scrollname = scroll->objectName();

  DisplayMode mode;
  if (!scrollname.compare(QString("magnitudePhaseScrollArea"))) {
    mode = DisplayMode::Magnitude_dB;
  } else if (!scrollname.compare(QString("smithScrollArea"))) {
    mode = DisplayMode::Smith;
  } else if (!scrollname.compare(QString("polarScrollArea"))) {
    mode = DisplayMode::Polar;
  } else if (!scrollname.compare(QString("portImpedanceScrollArea"))) {
    mode = DisplayMode::PortImpedance;
  } else if (!scrollname.compare(QString("stabilityScrollArea"))) {
    mode = DisplayMode::Stability;
  } else if (!scrollname.compare(QString("VSWRScrollArea"))) {
    mode = DisplayMode::VSWR;
  } else if (!scrollname.compare(QString("GroupDelayScrollArea"))) {
    mode = DisplayMode::GroupDelay;
  }

  // New trace width
  int TraceWidth = spinbox->value();

  // 2) Get the trace name
  ID.remove("Trace_Width_"); // Remove the preffix

  // Remove trace from the layout and from the chart
  QPen currentPen(Qt::black);  // Explicit constructor (to avoid warnings)
  currentPen.setStyle(Qt::SolidLine);
  currentPen.setWidth(1);

  switch (mode) {
  case DisplayMode::Magnitude_dB:
  case DisplayMode::Phase:
    currentPen = Magnitude_PhaseChart->getTracePen(ID);
    currentPen.setWidth(TraceWidth);
    Magnitude_PhaseChart->setTracePen(ID, currentPen);
    break;
  case DisplayMode::Smith:
    currentPen = smithChart->getTracePen(ID);
    currentPen.setWidth(TraceWidth);
    smithChart->setTracePen(ID, currentPen);
    break;
  case DisplayMode::Polar:
    currentPen = polarChart->getTracePen(ID);
    currentPen.setWidth(TraceWidth);
    polarChart->setTracePen(ID, currentPen);
    break;
  case DisplayMode::PortImpedance:
    currentPen = impedanceChart->getTracePen(ID);
    currentPen.setWidth(TraceWidth);
    impedanceChart->setTracePen(ID, currentPen);
    break;
  case DisplayMode::Stability:
    currentPen = stabilityChart->getTracePen(ID);
    currentPen.setWidth(TraceWidth);
    stabilityChart->setTracePen(ID, currentPen);
    break;
  case DisplayMode::VSWR:
    currentPen = VSWRChart->getTracePen(ID);
    currentPen.setWidth(TraceWidth);
    VSWRChart->setTracePen(ID, currentPen);
    break;
  case DisplayMode::GroupDelay:
    currentPen = GroupDelayChart->getTracePen(ID);
    currentPen.setWidth(TraceWidth);
    GroupDelayChart->setTracePen(ID, currentPen);
    break;
  }
}


// Given a trace, it gives the minimum and the maximum values at both axis.
void Qucs_S_SPAR_Viewer::getMinMaxValues(QString filename, QString tracename, qreal& minX, qreal& maxX, qreal& minY, qreal& maxY) {
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

int Qucs_S_SPAR_Viewer::findClosestIndex(const QList<double>& list, double value)
{
    return std::min_element(list.begin(), list.end(),
        [value](double a, double b) {
            return std::abs(a - value) < std::abs(b - value);
        }) - list.begin();
}


void Qucs_S_SPAR_Viewer::addMarker(double freq, QString Freq_Marker_Scale){

    // If there are no traces in the display, show a message and exit
    if (traceMap.size() == 0){
      QMessageBox::information(
          this,
          tr("Warning"),
          tr("The display contains no traces.") );
      return;
    }

    double f_marker;
    if (freq == -1) {
      // There's no specific frequency argument, then pick the middle point
      double f1 = Magnitude_PhaseChart->getXmin();
      double f2 = Magnitude_PhaseChart->getXmax();
      f_marker = f1 + 0.5*(f2-f1);
    } else {
      f_marker= freq/getFreqScale(Freq_Marker_Scale);
      f_marker *= Magnitude_PhaseChart->getXscale();// Scale according to the x-axis units
    }

    int n_markers = getNumberOfMarkers();
    n_markers++;

    MarkerProperties props; // Struct to hold Marker widgets

    QString new_marker_name = QStringLiteral("Mkr%1").arg(n_markers);
    QLabel * new_marker_label = new QLabel(new_marker_name);
    new_marker_label->setObjectName(new_marker_name);
    props.nameLabel = new_marker_label;

    this->MarkersGrid->addWidget(new_marker_label, n_markers, 0);

    QString SpinBox_name = QStringLiteral("Mkr_SpinBox%1").arg(n_markers);
    QDoubleSpinBox * new_marker_Spinbox = new QDoubleSpinBox();
    new_marker_Spinbox->setObjectName(SpinBox_name);
    new_marker_Spinbox->setMinimum(Magnitude_PhaseChart->getXmin());
    new_marker_Spinbox->setMaximum(Magnitude_PhaseChart->getXmax());
    new_marker_Spinbox->setDecimals(1);
    new_marker_Spinbox->setValue(f_marker);
    connect(new_marker_Spinbox, SIGNAL(valueChanged(double)), SLOT(updateMarkerTable()));
    props.freqSpinBox = new_marker_Spinbox;
    this->MarkersGrid->addWidget(new_marker_Spinbox, n_markers, 1);

    QString Combobox_name = QStringLiteral("Mkr_ComboBox%1").arg(n_markers);
    QComboBox * new_marker_Combo = new QComboBox();
    new_marker_Combo->setObjectName(Combobox_name);
    new_marker_Combo->addItems(frequency_units);
    new_marker_Combo->setCurrentIndex(Magnitude_PhaseChart->getFreqIndex());
    connect(new_marker_Combo, SIGNAL(currentIndexChanged(int)), SLOT(changeMarkerLimits()));
    props.scaleComboBox = new_marker_Combo;
    this->MarkersGrid->addWidget(new_marker_Combo, n_markers, 2);

    // Remove button
    QString DeleteButton_name = QStringLiteral("Mkr_Delete_Btn%1").arg(n_markers);
    QToolButton * new_marker_removebutton = new QToolButton();
    new_marker_removebutton->setObjectName(DeleteButton_name);
    QIcon icon(":/bitmaps/trash.png"); // Use a resource path or a relative path
    new_marker_removebutton->setIcon(icon);
    new_marker_removebutton->setStyleSheet(R"(
            QToolButton {
                background-color: #FF0000;
                color: white;
                border-radius: 20px;
            }
        )");
    connect(new_marker_removebutton, SIGNAL(clicked()), SLOT(removeMarker()));
    props.deleteButton = new_marker_removebutton;
    this->MarkersGrid->addWidget(new_marker_removebutton, n_markers, 3, Qt::AlignCenter);

    // Add marker widgets to the marker map
    markerMap[new_marker_name] = props;


    // Add new entry to the table
    QString new_freq = QStringLiteral("%1 ").arg(QString::number(f_marker, 'f', 2)) + Freq_Marker_Scale;
    QTableWidgetItem *newfreq = new QTableWidgetItem(new_freq);


    // Add table entries

    // Magnitude / phase marker
    n_markers = tableMarkers_Magnitude_Phase->rowCount() + 1;
    tableMarkers_Magnitude_Phase->setRowCount(n_markers);
    tableMarkers_Magnitude_Phase->setRowCount(n_markers);
    tableMarkers_Magnitude_Phase->setItem(n_markers-1, 0, newfreq);

    // Smith chart marker
    n_markers = tableMarkers_Smith->rowCount() + 1;
    tableMarkers_Smith->setRowCount(n_markers);
    tableMarkers_Smith->setRowCount(n_markers);
    tableMarkers_Smith->setItem(n_markers-1, 0, newfreq);

    n_markers = tableMarkers_Polar->rowCount() + 1;
    tableMarkers_Polar->setRowCount(n_markers);
    tableMarkers_Polar->setRowCount(n_markers);
    tableMarkers_Polar->setItem(n_markers-1, 0, newfreq);

    n_markers = tableMarkers_PortImpedance->rowCount() + 1;
    tableMarkers_PortImpedance->setRowCount(n_markers);
    tableMarkers_PortImpedance->setRowCount(n_markers);
    tableMarkers_PortImpedance->setItem(n_markers-1, 0, newfreq);

    n_markers = tableMarkers_Stability->rowCount() + 1;
    tableMarkers_Stability->setRowCount(n_markers);
    tableMarkers_Stability->setRowCount(n_markers);
    tableMarkers_Stability->setItem(n_markers-1, 0, newfreq);

    n_markers = tableMarkers_VSWR->rowCount() + 1;
    tableMarkers_VSWR->setRowCount(n_markers);
    tableMarkers_VSWR->setRowCount(n_markers);
    tableMarkers_VSWR->setItem(n_markers-1, 0, newfreq);


    n_markers = tableMarkers_GroupDelay->rowCount() + 1;
    tableMarkers_GroupDelay->setRowCount(n_markers);
    tableMarkers_GroupDelay->setRowCount(n_markers);
    tableMarkers_GroupDelay->setItem(n_markers-1, 0, newfreq);

    changeMarkerLimits(Combobox_name);

    f_marker = getMarkerFreq(new_marker_name);

    // Define QPen
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCosmetic(true);

    // Add marker to the charts
    // IMPORTANT TO NOTE: There's an issue with the Qt Charts: If the marker is drawn when the dock is behind others, the position
    // of the dot marker will be wrong, For fixing that, it is needed to raise the dock before adding the marker. This doesn't happen
    // with the Smith Chart widget as it was developed from scratch

    // Find which of the docks is raised to restore that at the end
    bool isDockMagPhaseRaised = !dockChart->visibleRegion().isEmpty();
    bool isDockSmithRaised = !dockSmithChart->visibleRegion().isEmpty();
    bool isDockPolarRaised = !dockPolarChart->visibleRegion().isEmpty();
    bool isDockPortImpedanceRaised = !dockImpedanceChart->visibleRegion().isEmpty();
    bool isDockStabilityRaised = !dockStabilityChart->visibleRegion().isEmpty();
    bool isDockVSWRRaised = !dockVSWRChart->visibleRegion().isEmpty();

    dockChart->raise();
    Magnitude_PhaseChart->addMarker(new_marker_name, f_marker, pen); // Magnitude & Phase

    smithChart->addMarker(new_marker_name, f_marker); // Smith Chart

    dockPolarChart->raise();
    polarChart->addMarker(new_marker_name, f_marker); // Polar plot

    dockImpedanceChart->raise();
    impedanceChart->addMarker(new_marker_name, f_marker, pen); // Port impedance plot

    dockStabilityChart->raise();
    stabilityChart->addMarker(new_marker_name, f_marker, pen); // Stability plot

    dockVSWRChart->raise();
    VSWRChart->addMarker(new_marker_name, f_marker, pen); // VSWR plot

    dockGroupDelayChart->raise();
    GroupDelayChart->addMarker(new_marker_name, f_marker, pen); // Group delay

    // Restore original situation
    if (isDockMagPhaseRaised) {
      dockChart->raise();
    } else {
      if (isDockSmithRaised) {
        dockSmithChart->raise();
      } else {
        if (isDockPolarRaised) {
          dockPolarChart->raise();
        } else {
          if (isDockPortImpedanceRaised) {
            dockImpedanceChart->raise();
          } else {
            if (isDockStabilityRaised) {
              dockStabilityChart->raise();
            } else {
              if (isDockVSWRRaised) {
                dockVSWRChart->raise();
              }
            }
          }
        }
      }
    }
}


void Qucs_S_SPAR_Viewer::updateMarkerTable(){

    //If there are no markers, remove the entries and return
  int n_markers = getNumberOfMarkers();
    if (n_markers == 0){
        tableMarkers_Magnitude_Phase->clear();
        tableMarkers_Magnitude_Phase->setColumnCount(0);
        tableMarkers_Magnitude_Phase->setRowCount(0);

        tableMarkers_Smith->clear();
        tableMarkers_Smith->setColumnCount(0);
        tableMarkers_Smith->setRowCount(0);

        tableMarkers_Polar->clear();
        tableMarkers_Polar->setColumnCount(0);
        tableMarkers_Polar->setRowCount(0);

        tableMarkers_PortImpedance->clear();
        tableMarkers_PortImpedance->setColumnCount(0);
        tableMarkers_PortImpedance->setRowCount(0);

        tableMarkers_Stability->clear();
        tableMarkers_Stability->setColumnCount(0);
        tableMarkers_Stability->setRowCount(0);

        tableMarkers_VSWR->clear();
        tableMarkers_VSWR->setColumnCount(0);
        tableMarkers_VSWR->setRowCount(0);

        tableMarkers_GroupDelay->clear();
        tableMarkers_GroupDelay->setColumnCount(0);
        tableMarkers_GroupDelay->setRowCount(0);
        return;
    }

    // Reset headers
    QStringList header_Magnitude_Phase, header_Smith, header_Polar, header_PortImpedance, header_Stability, header_VSWR, header_GroupDelay;
    header_Magnitude_Phase.clear();
    header_Magnitude_Phase.append("freq");

    header_Smith = header_Magnitude_Phase;
    header_Polar = header_Magnitude_Phase;
    header_PortImpedance = header_Magnitude_Phase;
    header_Stability = header_Magnitude_Phase;
    header_VSWR = header_Magnitude_Phase;
    header_GroupDelay = header_Magnitude_Phase;

    // Build headers

    QStringList traces = traceMap[DisplayMode::Magnitude_dB].keys(); // Traces displayed in the magnitude / phase plot
    header_Magnitude_Phase.append(traces);

    traces = traceMap[DisplayMode::Smith].keys(); // Traces displayed in the Smith Chart
    header_Smith.append(traces);

    traces = traceMap[DisplayMode::Polar].keys(); // Traces displayed in the Polar Chart
    header_Polar.append(traces);

    traces = traceMap[DisplayMode::PortImpedance].keys(); // Traces displayed in the Port Impedance Chart
    header_PortImpedance.append(traces);

    traces = traceMap[DisplayMode::Stability].keys(); // Traces displayed in the Stability Chart
    header_Stability.append(traces);

    traces = traceMap[DisplayMode::VSWR].keys(); // Traces displayed in the VSWR Chart
    header_VSWR.append(traces);

    traces = traceMap[DisplayMode::GroupDelay].keys(); // Traces displayed in the Group Delay Chart
    header_GroupDelay.append(traces);

    // Update marker data
    updateMarkerData(*tableMarkers_Magnitude_Phase, DisplayMode::Magnitude_dB, header_Magnitude_Phase); // Magnitude and phase table
    updateMarkerData(*tableMarkers_Smith, DisplayMode::Smith, header_Smith); // Smith Chart table
    updateMarkerData(*tableMarkers_Polar, DisplayMode::Polar, header_Polar); // Polar Chart table
    updateMarkerData(*tableMarkers_PortImpedance, DisplayMode::PortImpedance, header_PortImpedance); // Port impedance Chart table
    updateMarkerData(*tableMarkers_Stability, DisplayMode::Stability, header_Stability); // Port impedance Chart table
    updateMarkerData(*tableMarkers_VSWR, DisplayMode::VSWR, header_VSWR); // Port impedance Chart table
    updateMarkerData(*tableMarkers_GroupDelay, DisplayMode::GroupDelay, header_GroupDelay); // Group Delay Chart table


    // Update markers
    QStringList marker_list = markerMap.keys();
    for (const QString &str : marker_list) {
      double marker_freq = getMarkerFreq(str);
      smithChart->updateMarkerFrequency(str, marker_freq); // Update Smith Chart widget markers
      polarChart->updateMarkerFrequency(str, marker_freq); // Update Polar Chart widget markers
      Magnitude_PhaseChart->updateMarkerFrequency(str, marker_freq); // Update magnitude / phase widget markers
      impedanceChart->updateMarkerFrequency(str, marker_freq); // Update port impedance widget markers
      stabilityChart->updateMarkerFrequency(str, marker_freq); // Update stability widget markers
      VSWRChart->updateMarkerFrequency(str, marker_freq); // Update VSWR widget markers
      GroupDelayChart->updateMarkerFrequency(str, marker_freq); // Update Group Delay Chart
    }
}


// Fill the different marker tables
void Qucs_S_SPAR_Viewer::updateMarkerData(QTableWidget& table, DisplayMode mode, QStringList header){

  QPointF P;
  qreal targetX;
  QString new_val;
  QString freq_marker;

  int n_markers = getNumberOfMarkers();
  int n_traces = header.size();
  table.setColumnCount(n_traces);
  table.setRowCount(n_markers);
  table.setHorizontalHeaderLabels(header);

  for (int c = 0; c<n_traces; c++){//Traces
    for (int r = 0; r<n_markers; r++){//Marker
      QString markerName;
      MarkerProperties mkr_props;
      getMarkerByPosition(r, markerName, mkr_props); // Get the whole marker given the position

             // Compose the marker text
      freq_marker = QStringLiteral("%1 ").arg(QString::number(mkr_props.freqSpinBox->value(), 'f', 1)) + mkr_props.scaleComboBox->currentText();

      if (c==0){
        // First column
        QTableWidgetItem *new_item = new QTableWidgetItem(freq_marker);
        table.setItem(r, c, new_item);
        continue;
      }
      targetX = getFreqFromText(freq_marker);


      QString trace_name = header.at(c);

      // Look into dataset for the trace data
      QStringList parts = {
          trace_name.section('.', 0, -2),
          trace_name.section('.', -1)
      };
      QString file = parts[0];
      QString trace = parts[1];

      // Find data on the dataset
      if (mode == DisplayMode::Smith){
          // Get R + j X
          QString sxx_re = trace;
          QString sxx_im = trace;

          sxx_re.replace("Smith", "re");
          sxx_im.replace("Smith", "im");

          QPointF sij_real = findClosestPoint(datasets[file]["frequency"], datasets[file][sxx_re], targetX);
          QPointF sij_imag = findClosestPoint(datasets[file]["frequency"], datasets[file][sxx_im], targetX);
          double Z0 = datasets[file]["Z0"].at(0);

          double S_real = sij_real.y();
          double S_imag = sij_imag.y();

          // Calculate VSWR
          double magnitude_Gamma = sqrt(S_real * S_real + S_imag * S_imag);
          double SWR = (1.0 + magnitude_Gamma) / (1.0 - magnitude_Gamma);

          // Calculate complex impedance
          std::complex<double> Gamma(S_real, S_imag);
          std::complex<double> Z = Z0 * (1.0 + Gamma) / (1.0 - Gamma);

          double imag_part = Z.imag();

          if (imag_part < 0) {
            new_val = QStringLiteral("Z=%1-j%2 Ω\nSWR = %3").arg(QString::number(Z.real(), 'f', 1)).arg(QString::number(Z.imag(), 'f', 1)).arg(QString::number(SWR, 'f', 2));
          } else {
            if (imag_part > 0) {
              new_val = QStringLiteral("Z=%1+j%2 Ω\nSWR = %3").arg(QString::number(Z.real(), 'f', 1)).arg(QString::number(Z.imag(), 'f', 1)).arg(QString::number(SWR, 'f', 2));
            } else {
              // Z is pure real
              new_val = QStringLiteral("Z=%1 Ω\nSWR = %3").arg(QString::number(Z.real(), 'f', 1)).arg(QString::number(SWR, 'f', 2));
            }
          }
        } else {
        if (mode == DisplayMode::Polar) {
            QString sxx_re = trace;
            QString sxx_im = trace;

            sxx_re.append("_re");
            sxx_im.append("_im");

            QPointF sij_real = findClosestPoint(datasets[file]["frequency"], datasets[file][sxx_re], targetX);
            QPointF sij_imag = findClosestPoint(datasets[file]["frequency"], datasets[file][sxx_im], targetX);

            double S_real = sij_real.y();
            double S_imag = sij_imag.y();

            std::complex<double> S(S_real, S_imag);

            double radius = std::abs(S);
            double angle = std::arg(S) * 180.0 / M_PI;
            if (angle < 0) angle += 360;

            new_val = QStringLiteral("%1∠%2").arg(QString::number(radius, 'f', 2)).arg(QString::number(angle, 'f', 1));
          } else {
            // Go directly to the dataset for data
            P = findClosestPoint(datasets[file]["frequency"], datasets[file][trace], targetX);
            new_val = QStringLiteral("%1").arg(QString::number(P.y(), 'f', 2));

            if (mode == DisplayMode::GroupDelay) {
              // Add units
              new_val += QString(" ns");
            }
          }
        }


      QTableWidgetItem *new_item = new QTableWidgetItem(new_val);
      table.setItem(r, c, new_item);
    }
  }
}

// Find the closest x-axis value in a series given a x value (not necesarily in the grid)
QPointF Qucs_S_SPAR_Viewer::findClosestPoint(const QList<double>& xValues, const QList<double>& yValues, double targetX)
{
  if (xValues.isEmpty() || yValues.isEmpty() || xValues.size() != yValues.size()) {
    return QPointF(); // Return invalid point if lists are empty or have different sizes
  }

         // Initialize with the first point
  QPointF closestPoint(xValues.first(), yValues.first());
  double minDistance = qAbs(targetX - closestPoint.x());

         // Iterate through all points to find the closest one
  for (int i = 0; i < xValues.size(); ++i) {
    double distance = qAbs(targetX - xValues[i]);
    if (distance < minDistance) {
      minDistance = distance;
      closestPoint = QPointF(xValues[i], yValues[i]);
    }
  }

  return closestPoint;
}


double Qucs_S_SPAR_Viewer::getFreqFromText(QString freq)
{
    // Remove any whitespace from the string
    freq = freq.simplified();

    // Regular expression to match the number and unit
    QRegularExpression re("(\\d+(?:\\.\\d+)?)(\\s*)(Hz|kHz|MHz|GHz)");
    re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(freq);

    if (match.hasMatch()) {
        double value = match.captured(1).toDouble();
        QString unit = match.captured(3).toLower();

        // Convert to Hz based on the unit
        if (unit == "khz") {
            return value * 1e3;
        } else if (unit == "mhz") {
            return value * 1e6;
        } else if (unit == "ghz") {
            return value * 1e9;
        } else {
            // Assume Hz if no unit or Hz is specified
            return value;
        }
    }

    // Return -1 if the input doesn't match the expected format
    return -1;
}


// This function is called when the user wants to remove a marker from the plot
void Qucs_S_SPAR_Viewer::removeMarker()
{
    QString ID = qobject_cast<QToolButton*>(sender())->objectName();
    //qDebug() << "Clicked button:" << ID;

    //Find the index of the button to remove
    int nmarkers = getNumberOfMarkers();

    MarkerProperties mkr_prop;
    QString mkr_name;
    for (int i = 0; i < nmarkers; i++) {
      getMarkerByPosition(i, mkr_name, mkr_prop);
      if (mkr_prop.deleteButton->objectName() == ID) {
          break;
      }
    }
    removeMarker(mkr_name); // Remove marker by name
}


void Qucs_S_SPAR_Viewer::removeMarker(const QString& markerName) {
  if (markerMap.contains(markerName)) {
    // Get the marker properties
    MarkerProperties& props = markerMap[markerName];

    // Delete all widgets
    delete props.nameLabel;
    delete props.freqSpinBox;
    delete props.scaleComboBox;
    delete props.deleteButton;

    // Remove from the map
    markerMap.remove(markerName);

    // Remove markers from the display widgets
    Magnitude_PhaseChart->removeMarker(markerName);
    smithChart->removeMarker(markerName);
    polarChart->removeMarker(markerName);
    impedanceChart->removeMarker(markerName);
    GroupDelayChart->removeMarker(markerName);

    updateMarkerTable();
    updateMarkerNames();
    updateGridLayout(MarkersGrid);
  }
}

// Removes all markers on a row
void Qucs_S_SPAR_Viewer::removeAllMarkers()
{
    int n_markers = getNumberOfMarkers();
    for (int i = 0; i < n_markers; i++) {
      QString marker_to_remove = QString("Mkr%1").arg(n_markers-i);
      removeMarker(marker_to_remove);
    }
    traceMap.clear();
}

// After removing a marker, the names of the other markers must be updated
void Qucs_S_SPAR_Viewer::updateMarkerNames()
{
  int n_markers = getNumberOfMarkers();
  for (int i = 0; i < n_markers; i++) {
    MarkerProperties mkr_props;
    QString mkr_name;

    getMarkerByPosition(i, mkr_name, mkr_props);

    QLabel * MarkerLabel = mkr_props.nameLabel;
    MarkerLabel->setText(QStringLiteral("Mkr%1").arg(i+1));
  }
}

// After removing a marker, the names of the other markers must be updated
void Qucs_S_SPAR_Viewer::updateLimitNames()
{
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {
    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);

    limit_props.LimitLabel->setText(QStringLiteral("Limit %1").arg(i+1));
  }
}

// This function is called when the user wants to remove a limit from the plot
void Qucs_S_SPAR_Viewer::removeLimit()
{
  QString ID = qobject_cast<QToolButton*>(sender())->objectName();
  //qDebug() << "Clicked button:" << ID;

  //Find the index of the button to remove
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {

    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);

    if (limit_props.Button_Delete_Limit->objectName() == ID) {
      removeLimit(limit_name);
      break;
    }
  }

}

void Qucs_S_SPAR_Viewer::removeLimit(QString limit_to_remove)
{

  // Get the widgets
  LimitProperties limit_props = limitsMap[limit_to_remove];

  // Remove the widgets
  delete limit_props.axis;
  delete limit_props.Button_Delete_Limit;
  delete limit_props.Couple_Value;
  delete limit_props.LimitLabel;
  delete limit_props.Separator;
  delete limit_props.Start_Freq;
  delete limit_props.Start_Freq_Scale;
  delete limit_props.Start_Value;
  delete limit_props.Stop_Freq;
  delete limit_props.Stop_Freq_Scale;
  delete limit_props.Stop_Value;


  // Remove limit entry from the map
  limitsMap.remove(limit_to_remove);


  // Remove limit lines from the plot
  Magnitude_PhaseChart->removeLimit(limit_to_remove);

  updateGridLayout(LimitsGrid);
  updateLimitNames();
}

void Qucs_S_SPAR_Viewer::removeAllLimits()
{
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {
    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);
    removeLimit(limit_name);
  }
}

// If the combobox associated to a marker changes, the limits of the marker must be updated too
void Qucs_S_SPAR_Viewer::changeMarkerLimits()
{
    QString ID = qobject_cast<QComboBox*>(sender())->objectName();
    //qDebug() << "Clicked button:" << ID;
    changeMarkerLimits(ID);

}

// If the combobox associated to a marker changes, the limits of the marker must be updated too
void Qucs_S_SPAR_Viewer::changeMarkerLimits(QString ID)
{
    //Find the index of the marker
    int index = -1;
    int nmarkers = getNumberOfMarkers();

    // Inspects all the markers' combobox and find that've been triggered
    for (int i = 0; i < nmarkers; i++) {
      MarkerProperties mkr_props;
      QString mkr_name;

      getMarkerByPosition(i, mkr_name, mkr_props);

      if (mkr_props.scaleComboBox->objectName() == ID) {
          index = i;
          break;
      }
    }

    // The lower and upper limits are given by the axis settings
    double f_upper = Magnitude_PhaseChart->getXmax();
    double f_lower = Magnitude_PhaseChart->getXmin();
    double f_scale = 1e-6;

    f_upper /=f_scale;
    f_lower /=f_scale;

    // Get markers properties
    MarkerProperties mkr_props;
    QString mkr_name;

    getMarkerByPosition(index, mkr_name, mkr_props);

    // Now we have to normalize this with respect to the marker's combo
    QString new_scale = mkr_props.scaleComboBox->currentText();
    double f_scale_combo = getFreqScale(new_scale);
    f_upper *= f_scale_combo;
    f_lower *= f_scale_combo;

    mkr_props.freqSpinBox->setMinimum(f_lower);
    mkr_props.freqSpinBox->setMaximum(f_upper);

    // Update minimum step
    double diff = f_upper - f_lower;
    if (diff < 1){
         mkr_props.freqSpinBox->setSingleStep(0.01);
    }else{
        if (diff < 10){
             mkr_props.freqSpinBox->setSingleStep(0.1);
        }else{
            if (diff < 100){
                 mkr_props.freqSpinBox->setSingleStep(1);
            }else{
                 mkr_props.freqSpinBox->setSingleStep(10);
            }

        }
    }

    updateMarkerTable();
}



void Qucs_S_SPAR_Viewer::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void Qucs_S_SPAR_Viewer::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    QStringList fileList;

    for (const QUrl &url : qAsConst(urls)) {
        if (url.isLocalFile()) {
            fileList << url.toLocalFile();
        }
    }

    if (!fileList.isEmpty()) {
      // Check if this is a session file
      if (fileList.size() == 1){
        if (fileList.first().endsWith(".spar", Qt::CaseInsensitive)) {// Then open it as a session settings file.
          // Remove traces and the dataset from the current session before loading the session file
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

void Qucs_S_SPAR_Viewer::addLimit(double f_limit1, QString f_limit1_unit, double f_limit2, QString f_limit2_unit, double y_limit1, double y_limit2, bool coupled)
{
  // If there are no traces in the display, show a message and exit
  if (traceMap.size() == 0){
    QMessageBox::information(
        this,
        tr("Warning"),
        tr("The display contains no traces.") );
    return;
  }

  if (f_limit1 == -1) {
    // There's no specific data passed. Then get it from the widgets
    double f1 = Magnitude_PhaseChart->getXmin();
    double f2 = Magnitude_PhaseChart->getXmax();
    f_limit1 = f1 + 0.25*(f2-f1);
    f_limit2 = f1 + 0.75*(f2-f1);

    double y1 = Magnitude_PhaseChart->getYmin();
    double y2 = Magnitude_PhaseChart->getYmax();

    y_limit1 = y1 + (y2-y1)/2;
    y_limit2 = y_limit1;

  }

  int n_limits = getNumberOfLimits();
  n_limits++;
  int limit_index = 3*n_limits-2;

  QString tooltip_message;

  QString new_limit_name = QStringLiteral("Limit %1").arg(n_limits);
  QLabel * new_limit_label = new QLabel(new_limit_name);
  new_limit_label->setObjectName(new_limit_name);
  limitsMap[new_limit_name].LimitLabel = new_limit_label;
  this->LimitsGrid->addWidget(new_limit_label, limit_index, 0);

  QString SpinBox_fstart_name = QStringLiteral("Lmt_Freq_Start_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox * new_limit_fstart_Spinbox = new QDoubleSpinBox();
  new_limit_fstart_Spinbox->setObjectName(SpinBox_fstart_name);
  new_limit_fstart_Spinbox->setMinimum(Magnitude_PhaseChart->getXmin());
  new_limit_fstart_Spinbox->setMaximum(Magnitude_PhaseChart->getXmax());
  new_limit_fstart_Spinbox->setSingleStep(Magnitude_PhaseChart->getXdiv()/5);
  new_limit_fstart_Spinbox->setValue(f_limit1);
  limitsMap[new_limit_name].Start_Freq = new_limit_fstart_Spinbox;
  this->LimitsGrid->addWidget(new_limit_fstart_Spinbox, limit_index, 1);

  QString Combobox_start_name = QStringLiteral("Lmt_Start_ComboBox_%1").arg(new_limit_name);
  QComboBox * new_start_limit_Combo = new QComboBox();
  new_start_limit_Combo->setObjectName(Combobox_start_name);
  new_start_limit_Combo->addItems(frequency_units);
  limitsMap[new_limit_name].Start_Freq_Scale = new_start_limit_Combo;
  this->LimitsGrid->addWidget(new_start_limit_Combo, limit_index, 2);

  QString SpinBox_fstop_name = QStringLiteral("Lmt_Freq_Stop_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox * new_limit_fstop_Spinbox = new QDoubleSpinBox();
  new_limit_fstop_Spinbox->setObjectName(SpinBox_fstop_name);
  new_limit_fstop_Spinbox->setMinimum(Magnitude_PhaseChart->getXmin());
  new_limit_fstop_Spinbox->setMaximum(Magnitude_PhaseChart->getXmax());
  new_limit_fstop_Spinbox->setSingleStep(Magnitude_PhaseChart->getXdiv()/5);
  new_limit_fstop_Spinbox->setValue(f_limit2);
  limitsMap[new_limit_name].Stop_Freq = new_limit_fstop_Spinbox;
  this->LimitsGrid->addWidget(new_limit_fstop_Spinbox, limit_index, 3);

  QString Combobox_stop_name = QStringLiteral("Lmt_Stop_ComboBox_%1").arg(new_limit_name);
  QComboBox * new_stop_limit_Combo = new QComboBox();
  new_stop_limit_Combo->setObjectName(Combobox_stop_name);
  new_stop_limit_Combo->addItems(frequency_units);

  if (f_limit1_unit.isEmpty()) {
    QString Mag_Phase_Units = Magnitude_PhaseChart->getXunits();
    if (Mag_Phase_Units.isEmpty()){
      new_start_limit_Combo->setCurrentIndex(1);
      new_stop_limit_Combo->setCurrentIndex(1);
    } else {
      int index = new_stop_limit_Combo->findText(Mag_Phase_Units);
      new_stop_limit_Combo->setCurrentIndex(index);
      new_start_limit_Combo->setCurrentIndex(index);
    }
  } else {
    // The units exist (e.g. loading session file)
    int index = new_start_limit_Combo->findText(f_limit1_unit, Qt::MatchFlag::MatchContains); // Find the index of the unit
    new_start_limit_Combo->setCurrentIndex(index);

    index = new_stop_limit_Combo->findText(f_limit2_unit, Qt::MatchFlag::MatchContains); // Find the index of the unit
    new_stop_limit_Combo->setCurrentIndex(index);
  }

  limitsMap[new_limit_name].Stop_Freq_Scale = new_stop_limit_Combo;
  this->LimitsGrid->addWidget(new_stop_limit_Combo, limit_index, 4);

  // Remove button
  QString DeleteButton_name = QStringLiteral("Lmt_Delete_Btn_%1").arg(new_limit_name);
  QToolButton * new_limit_removebutton = new QToolButton();
  new_limit_removebutton->setObjectName(DeleteButton_name);
  tooltip_message = QStringLiteral("Remove this limit");
  new_limit_removebutton->setToolTip(tooltip_message);
  QIcon icon(":/bitmaps/trash.png");
  new_limit_removebutton->setIcon(icon);
  new_limit_removebutton->setStyleSheet(R"(
            QToolButton {
                background-color: #FF0000;
                color: white;
                border-radius: 20px;
            }
        )");
  limitsMap[new_limit_name].Button_Delete_Limit = new_limit_removebutton;
  this->LimitsGrid->addWidget(new_limit_removebutton, limit_index, 5, Qt::AlignCenter);

  QString SpinBox_val_start_name = QStringLiteral("Lmt_Val_Start_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox * new_limit_val_start_Spinbox = new QDoubleSpinBox();
  new_limit_val_start_Spinbox->setObjectName(SpinBox_val_start_name);
  new_limit_val_start_Spinbox->setMinimum(-1000);
  new_limit_val_start_Spinbox->setMaximum(1000);
  new_limit_val_start_Spinbox->setValue(y_limit1);
  new_limit_val_start_Spinbox->setSingleStep(Magnitude_PhaseChart->getYdiv()/5);
  limitsMap[new_limit_name].Start_Value = new_limit_val_start_Spinbox;
  this->LimitsGrid->addWidget(new_limit_val_start_Spinbox, limit_index+1, 1);

  // Coupled spinbox value
  QString CoupleButton_name = QStringLiteral("Lmt_Couple_Btn_%1").arg(new_limit_name);
  QPushButton * new_limit_CoupleButton = new QPushButton("<--->");
  new_limit_CoupleButton->setObjectName(CoupleButton_name);
  new_limit_CoupleButton->setChecked(coupled);
  tooltip_message = QStringLiteral("Couple start and stop values");
  new_limit_CoupleButton->setToolTip(tooltip_message);
  limitsMap[new_limit_name].Couple_Value = new_limit_CoupleButton;
  this->LimitsGrid->addWidget(new_limit_CoupleButton, limit_index+1, 2);

  QString SpinBox_val_stop_name = QStringLiteral("Lmt_Val_Stop_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox * new_limit_val_stop_Spinbox = new QDoubleSpinBox();
  new_limit_val_stop_Spinbox->setObjectName(SpinBox_val_stop_name);
  new_limit_val_stop_Spinbox->setMinimum(-1000);
  new_limit_val_stop_Spinbox->setMaximum(1000);
  new_limit_val_stop_Spinbox->setValue(y_limit2);
  new_limit_val_stop_Spinbox->setSingleStep(Magnitude_PhaseChart->getYdiv()/5);
  limitsMap[new_limit_name].Stop_Value = new_limit_val_stop_Spinbox;
  this->LimitsGrid->addWidget(new_limit_val_stop_Spinbox, limit_index+1, 3);

  if (coupled){
    new_limit_CoupleButton->setText("<--->");
  } else {
    new_limit_CoupleButton->setText("<-X->");
  }

  QString QComboBox_yaxis_name = QStringLiteral("Combo_yaxis_%1").arg(new_limit_name);
  QComboBox * QComboBox_y_axis = new QComboBox();
  QComboBox_y_axis->setObjectName(QComboBox_yaxis_name);
  QComboBox_y_axis->addItem("Left Y");
  QComboBox_y_axis->addItem("Right Y");
  limitsMap[new_limit_name].axis = QComboBox_y_axis;
  this->LimitsGrid->addWidget(QComboBox_y_axis, limit_index+1, 4);

  QString Separator_name = QStringLiteral("Lmt_Separator_%1").arg(new_limit_name);
  QFrame * new_Separator = new QFrame();
  new_Separator->setObjectName(Separator_name);
  new_Separator->setFrameShape(QFrame::HLine);
  new_Separator->setFrameShadow(QFrame::Sunken);
  limitsMap[new_limit_name].Separator = new_Separator;
  this->LimitsGrid->addWidget(new_Separator, limit_index+2, 0, 1, 6);

  // Connect widgets to handler
  connect(limitsMap[new_limit_name].Start_Freq, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Start_Freq_Scale, SIGNAL(currentIndexChanged(int)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Stop_Freq, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Stop_Freq_Scale, SIGNAL(currentIndexChanged(int)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Button_Delete_Limit, SIGNAL(clicked()), SLOT(removeLimit()));
  connect(limitsMap[new_limit_name].Start_Value, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Couple_Value, SIGNAL(clicked(bool)), SLOT(coupleSpinBoxes()));
  connect(limitsMap[new_limit_name].Stop_Value, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].axis, SIGNAL(currentIndexChanged(int)), SLOT(updateLimits()));

  // Force to update the locked / unlocked status of the y-axis spinboxes
  limitsMap[new_limit_name].Couple_Value->click();

  // Add limit to the chart
  RectangularPlotWidget::Limit NewLimit;

  double f1 = limitsMap[new_limit_name].Start_Freq->value();
  QString scale = limitsMap[new_limit_name].Start_Freq_Scale->currentText();
  double f1_scale = getFreqScale(scale);
  f1 = f1 / f1_scale;


  double f2 = limitsMap[new_limit_name].Stop_Freq->value();
  scale = limitsMap[new_limit_name].Stop_Freq_Scale->currentText();
  double f2_scale = getFreqScale(scale);
  f2 = f2 / f2_scale;

  NewLimit.f1 = f1;
  NewLimit.f2 = f2;
  NewLimit.y1 = limitsMap[new_limit_name].Start_Value->value();
  NewLimit.y2 = limitsMap[new_limit_name].Stop_Value->value();
  NewLimit.pen = QPen(Qt::black, 2, Qt::SolidLine);

  Magnitude_PhaseChart->addLimit(new_limit_name, NewLimit);
  Magnitude_PhaseChart->update();
}

void Qucs_S_SPAR_Viewer::coupleSpinBoxes(){

  QPushButton* button = qobject_cast<QPushButton*>(sender());
  // Get the button ID, from it we can get the index and then lock the upper limit spinbox
  QString name_button = button->objectName();


  int nlimits = getNumberOfLimits();

  // Get limit
  QString limit_name;
  LimitProperties limit_props;
  for (int i = 0; i < nlimits; i++) {
    getLimitByPosition(i, limit_name, limit_props);
    if (limit_props.Couple_Value->objectName() == name_button) {
      break;
    }
  }

  if (limit_props.Couple_Value->text() == "<--->"){
    limit_props.Couple_Value->setText("<-X->");
    QString tooltip_message = QStringLiteral("Uncouple start and stop values");
    limit_props.Couple_Value->setToolTip(tooltip_message);

    // Couple limit spinboxes
    double start_value = limit_props.Start_Value->value();
    limit_props.Stop_Value->setValue(start_value);
    limit_props.Stop_Value->setDisabled(true);

  }else{
    limit_props.Couple_Value->setText("<--->");
    limit_props.Stop_Value->setEnabled(true);
  }
}

// This function is called when a limit widget is changed. It is needed in case some value-coupling
// button is activated
void Qucs_S_SPAR_Viewer::updateLimits()
{
  // First check if some value-coupling button is activated. If not, simply call updateTraces()
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {

    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);

    if (limit_props.Couple_Value->text() == "<-X->"){
      // The control is locked. Set the stop value equal to the start value
      double val_start = limit_props.Start_Value->value();
      limit_props.Stop_Value->setValue(val_start);
    }
  }

  // Catch the widget limit that triggered this function and update its value in the chart
  QObject * WidgetTriggered = sender();

  QString ObjectName = WidgetTriggered->objectName();

  int lastUnderscoreIndex = ObjectName.lastIndexOf('_');
  QString limit_name = ObjectName.mid(lastUnderscoreIndex + 1);

  // Get the actual values of the widgets corresponding to that limit
  RectangularPlotWidget::Limit limit_props;
  double f1 = limitsMap[limit_name].Start_Freq->value();
  QString scale = limitsMap[limit_name].Start_Freq_Scale->currentText();
  double f1_scale = getFreqScale(scale);
  f1 = f1 / f1_scale;


  double f2 = limitsMap[limit_name].Stop_Freq->value();
  scale = limitsMap[limit_name].Stop_Freq_Scale->currentText();
  double f2_scale = getFreqScale(scale);
  f2 = f2 / f2_scale;

  limit_props.f1 = f1;
  limit_props.f2 = f2;
  limit_props.y1 = limitsMap[limit_name].Start_Value->value();
  limit_props.y2 = limitsMap[limit_name].Stop_Value->value();
  limit_props.y_axis = limitsMap[limit_name].axis->currentIndex();
  limit_props.pen = QPen(Qt::black, 2, Qt::SolidLine);


  Magnitude_PhaseChart->updateLimit(limit_name, limit_props);



  Magnitude_PhaseChart->update();
}


void Qucs_S_SPAR_Viewer::slotSave()
{
  if (savepath.isEmpty()){
    slotSaveAs();
    return;
  }
  save();
}

void Qucs_S_SPAR_Viewer::slotSaveAs()
{
  if (datasets.isEmpty()){
    // Nothing to save
    QMessageBox::information(
        this,
        tr("Error"),
        tr("Nothing to save: No data was loaded.") );
    return;
  }

  // Get the path to save
  savepath = QFileDialog::getSaveFileName(this,
                                              tr("Save session"),
                                              QDir::homePath() + "/ViewerSession.spar",
                                              tr("Qucs-S snp viewer session (*.spar);"));

  // If the user decides not to enter a path, then return.
  if (savepath.isEmpty()){
    return;
  }
  save();
}


void Qucs_S_SPAR_Viewer::slotLoadSession()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open S-parameter Viewer Session"),
                                                  QDir::homePath(),
                                                  tr("Qucs-S snp viewer session (*.spar);"));

  loadSession(fileName);
}

bool Qucs_S_SPAR_Viewer::save() {
  if (savepath.isEmpty()) {
    return false; // No save path specified
  }

  QFile file(savepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Save Session"), tr("Could not open file for writing."));
    return false;
  }

  QXmlStreamWriter xml(&file);
  xml.setAutoFormatting(true);
  xml.writeStartDocument("1.0", "UTF-8");

  xml.writeStartElement("session");
  xml.writeAttribute("version", "1.0"); // Add version attribute

         // Save window geometry and state
  xml.writeStartElement("settings");
  xml.writeTextElement("geometry", saveGeometry().toBase64());
  xml.writeTextElement("state", saveState().toBase64());
  xml.writeEndElement(); // settings

         // Save datasets
  if (!datasets.isEmpty()) {  //Check empty map
    xml.writeStartElement("datasets");
    for (const QString& datasetName : datasets.keys()) {
      if (!datasetName.isEmpty() && !datasets[datasetName].isEmpty()) {  //Validate data
        xml.writeStartElement("dataset");
        xml.writeAttribute("name", datasetName);

               // Save dataset data
        const QMap<QString, QList<double>>& dataset = datasets[datasetName];
        for (const QString& key : dataset.keys()) {
          xml.writeStartElement("data");
          xml.writeAttribute("key", key);
          for (double value : dataset[key]) {
            xml.writeTextElement("value", QString::number(value, 'g', 15)); // Consistent number formatting
          }
          xml.writeEndElement(); // data
        }

        xml.writeEndElement(); // dataset
      }
    }
    xml.writeEndElement(); // datasets
  }

  // Save traces
  if (!traceMap.isEmpty()) { //Check empty map
    xml.writeStartElement("traces");
    // Loop display mode
    for (const DisplayMode& mode : traceMap.keys()) {
      // For each display mode, get all the traces
      QMap<QString, TraceProperties>& traces = traceMap[mode];
      // Iterate through the inner QMap (traces in the current mode)
      for (const QString& traceName : traces.keys()) {
          TraceProperties props = traces[traceName];
          int dotIndex = traceName.indexOf('.');

          QString dataset = traceName.left(dotIndex);
          QString trace = traceName.mid(dotIndex + 1);

          if (trace.endsWith("_n.u.")) {
            trace.chop(5);
          }

          xml.writeStartElement("trace");
          xml.writeAttribute("display", QString::number((int) mode)); // The display mode is saved as an integer
          xml.writeAttribute("dataset", dataset); // Extract dataset name
          xml.writeAttribute("name", trace);
          xml.writeAttribute("color", props.colorButton->palette().color(QPalette::Button).name());
          xml.writeAttribute("width", QString::number(props.width->value()));
          xml.writeAttribute("style", props.LineStyleComboBox->currentText());
          xml.writeEndElement(); // trace
        }
    }
    xml.writeEndElement(); // traces
  }

         // Save markers
  if (!markerMap.isEmpty()) { //Check empty map
    xml.writeStartElement("markers");
    for (const QString& markerName : markerMap.keys()) {
      MarkerProperties props = markerMap[markerName];
      xml.writeStartElement("marker");
      xml.writeAttribute("frequency", QString::number(props.freqSpinBox->value()));
      xml.writeAttribute("scale", props.scaleComboBox->currentText());
      xml.writeEndElement(); // marker
    }
    xml.writeEndElement(); // markers
  }

         // Save limits
  if (!limitsMap.isEmpty()) { //Check empty map
    xml.writeStartElement("limits");
    for (const QString& limitName : limitsMap.keys()) {
      LimitProperties props = limitsMap[limitName];
      xml.writeStartElement("limit");
      xml.writeAttribute("start_freq", QString::number(props.Start_Freq->value()));
      xml.writeAttribute("stop_freq", QString::number(props.Stop_Freq->value()));
      xml.writeAttribute("start_value", QString::number(props.Start_Value->value()));
      xml.writeAttribute("stop_value", QString::number(props.Stop_Value->value()));
      xml.writeAttribute("start_freq_scale", props.Start_Freq_Scale->currentText());
      xml.writeAttribute("stop_freq_scale", props.Stop_Freq_Scale->currentText());
      xml.writeAttribute("axis", props.axis->currentText());
      xml.writeEndElement(); // limit
    }
    xml.writeEndElement(); // limits
  }

         // Save notes
  xml.writeStartElement("notes");
  xml.writeCDATA(Notes_Widget->toPlainText()); // Use CDATA for notes
  xml.writeEndElement(); // notes

  xml.writeEndElement(); // session
  xml.writeEndDocument();

  if (file.error() != QFile::NoError) {
    QMessageBox::warning(this, tr("Save Session"), tr("Error writing file: ") + file.errorString());
    return false;
  }

  file.close();
  return true;
}



void Qucs_S_SPAR_Viewer::loadSession(QString session_file) {
  QFile file(session_file);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Load Session"), tr("Could not open file for reading."));
    return;
  }

  QXmlStreamReader xml(&file);

         // Clear current state
  removeAllFiles();
  removeAllMarkers();
  removeAllLimits();

  while (!xml.atEnd() && !xml.hasError()) {
    QXmlStreamReader::TokenType token = xml.readNext();

    if (token == QXmlStreamReader::StartElement) {
      if (xml.name() == QStringLiteral("settings")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("settings"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("geometry")) {
              restoreGeometry(QByteArray::fromBase64(xml.readElementText().toLatin1()));
            } else if (xml.name() == QStringLiteral("state")) {
              restoreState(QByteArray::fromBase64(xml.readElementText().toLatin1()));
            }
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("datasets")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("datasets"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("dataset")) {
            QString datasetName = xml.attributes().value("name").toString();
            QMap<QString, QList<double>> dataset;

            while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("dataset"))) {
              if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("data")) {
                QString key = xml.attributes().value("key").toString();
                QList<double> values;

                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("data"))) {
                  if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("value")) {
                    values.append(xml.readElementText().toDouble());
                  }
                  xml.readNext();
                }

                dataset[key] = values;
              }
              xml.readNext();
            }

            datasets[datasetName] = dataset;
            QCombobox_datasets->addItem(datasetName); // Add dataset to the combobox

            // Add dataset to the file list
            CreateFileWidgets(datasetName);
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("traces")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("traces"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("trace")) {
            QString traceName = xml.attributes().value("name").toString();
            traceName = traceName.mid(traceName.indexOf('.') + 1); // Remove all that comes before the dot, including the dot.
            QString dataset = xml.attributes().value("dataset").toString(); // Read dataset from attributes
            DisplayMode displayMode = (DisplayMode) xml.attributes().value("display").toInt(); // Display mode (e.g. rectangular, polar, Smith, etc.)
            QColor color(xml.attributes().value("color").toString()); // Read color from attributes
            int width = xml.attributes().value("width").toString().toInt(); // Read width from attributes
            QString style = xml.attributes().value("style").toString(); // Read style from attributes

            // Parse the traceName to determine parameter and display mode
            QString parameter;

            // Assuming the format is like "S11_dB", "S21_Smith", etc.
            if (traceName.contains("_")) {
              // Split parameter and mode
              parameter = traceName.left(traceName.indexOf('_'));
            } else {
              // If no underscore, assume the whole name is the parameter and use Magnitude_dB as default
              parameter = traceName;
            }

            // Create TraceInfo struct
            TraceInfo traceInfo = {dataset, parameter, displayMode};

            // Add the trace
            addTrace(traceInfo, color, width, style);
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("markers")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("markers"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("marker")) {
            double freq = xml.attributes().value("frequency").toString().toDouble(); // Read frequency from attributes
            QString scale = xml.attributes().value("scale").toString(); // Read scale from attributes
            addMarker(freq, scale);
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("limits")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("limits"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == QStringLiteral("limit")) {
            double start_freq = xml.attributes().value("start_freq").toString().toDouble(); // Read start_freq from attributes
            double stop_freq = xml.attributes().value("stop_freq").toString().toDouble(); // Read stop_freq from attributes
            double start_value = xml.attributes().value("start_value").toString().toDouble(); // Read start_value from attributes
            double stop_value = xml.attributes().value("stop_value").toString().toDouble(); // Read stop_value from attributes
            QString start_freq_scale = xml.attributes().value("start_freq_scale").toString(); // Read start_freq_scale from attributes
            QString stop_freq_scale = xml.attributes().value("stop_freq_scale").toString(); // Read stop_freq_scale from attributes
            QString axis = xml.attributes().value("axis").toString(); // Read axis from attributes
            addLimit(start_freq, start_freq_scale, stop_freq, stop_freq_scale, start_value, stop_value, true);
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("notes")) {
        Notes_Widget->setPlainText(xml.readElementText());
      }
    }
  }

  if (xml.hasError()) {
    QMessageBox::warning(this, tr("Load Session"), tr("Error parsing XML: ") + xml.errorString());
  }

  file.close();

  // Update UI
  updateTracesCombo();
  updateMarkerTable();
}

void Qucs_S_SPAR_Viewer::updateGridLayout(QGridLayout* layout)
{
  // Store widget information
  struct WidgetInfo {
    QWidget* widget;
    int row, column, rowSpan, columnSpan;
    Qt::Alignment alignment;
  };
  QVector<WidgetInfo> widgetInfos;

  // Collect information about remaining widgets
  for (int i = 0; i < layout->count(); ++i) {
    QLayoutItem* item = layout->itemAt(i);
    QWidget* widget = item->widget();
    if (widget) {
      int row, column, rowSpan, columnSpan;
      layout->getItemPosition(i, &row, &column, &rowSpan, &columnSpan);
      widgetInfos.push_back({widget, row, column, rowSpan, columnSpan, item->alignment()});
    }
  }

  // Clear the layout
  while (layout->count() > 0) {
    QLayoutItem* item = layout->takeAt(0);
    delete item;
  }

  // Re-add widgets with updated positions
  int row = 0;
  for (const auto& info : widgetInfos) {
    int newColumn = info.column;

    if (info.columnSpan == layout->columnCount()){// Separator widget
      row++;
    }

    layout->addWidget(info.widget, row, newColumn, info.rowSpan, info.columnSpan, info.alignment);

    if (info.columnSpan == layout->columnCount()){
      row++;
    }

    if (newColumn == layout->columnCount()-1) {
      row++;
    }
  }

}

// Add session file to the recent files list
void Qucs_S_SPAR_Viewer::addRecentFile(const QString& filePath) {
  recentFiles.insert(recentFiles.begin(), filePath);
  recentFiles.erase(std::unique(recentFiles.begin(), recentFiles.end()), recentFiles.end());
  if (recentFiles.size() > 10) {
    recentFiles.resize(10);
  }
}

// This function updates teh "Recent Files" list whenever the user hovers the mouse over the menu
void Qucs_S_SPAR_Viewer::updateRecentFilesMenu() {
  recentFilesMenu->clear();
  for (const auto& filePath : recentFiles) {
    QAction* action = recentFilesMenu->addAction(filePath);
    connect(action, &QAction::triggered, this, [this, filePath]() {
      loadSession(filePath);
    });
  }
  recentFilesMenu->addSeparator();
  recentFilesMenu->addAction("Clear Recent Files", this, &Qucs_S_SPAR_Viewer::clearRecentFiles);
}

void Qucs_S_SPAR_Viewer::clearRecentFiles() {
  recentFiles.clear();
}

// Save "Recent Files" list. This is called when the program is about to close
void Qucs_S_SPAR_Viewer::saveRecentFiles() {
  QSettings settings;
  settings.setValue("recentFiles", QVariant::fromValue(recentFiles));
}

// Load "Recent Files" list. This is called when the program starts up
void Qucs_S_SPAR_Viewer::loadRecentFiles() {
  QSettings settings;
  recentFiles = settings.value("recentFiles").value<std::vector<QString>>();
}

// This function is called when the user requests a trace which can be calculated from the S-parameters
void Qucs_S_SPAR_Viewer::calculate_Sparameter_trace(QString file, QString metric){


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
    for(int n = 1; n < numPoints; ++n) {
      double delta = unwrappedPhase[n] - unwrappedPhase[n-1];

             // Remove 360° discontinuities
      while(delta > 180.0) {
        unwrappedPhase[n] -= 360.0;
        delta = unwrappedPhase[n] - unwrappedPhase[n-1];
      }
      while(delta < -180.0) {
        unwrappedPhase[n] += 360.0;
        delta = unwrappedPhase[n] - unwrappedPhase[n-1];
      }
    }

           // Group delay calculation
    groupDelay.reserve(numPoints);

           // First point (forward difference)
    if(numPoints > 1) {
      double df = freq[1] - freq[0];
      double val = df != 0 ?
                       -(unwrappedPhase[1] - unwrappedPhase[0]) / (360.0 * df) : 0;
      val *= 1e9; // Convert to ns
      groupDelay.append(val);
    }

           // Central differences for interior points
    for(int n = 1; n < numPoints - 1; ++n) {
      double df = freq[n+1] - freq[n-1];
      double val = df != 0 ?
                       -(unwrappedPhase[n+1] - unwrappedPhase[n-1]) / (360.0 * df) : 0;
      val *= 1e9; // Convert to ns
      groupDelay.append(val);
    }

           // Last point (backward difference)
    if(numPoints > 1) {
      double df = freq[numPoints-1] - freq[numPoints-2];
      double val = df != 0 ?
                       -(unwrappedPhase[numPoints-1] - unwrappedPhase[numPoints-2]) / (360.0 * df) : 0;
      val *= 1e9; // Convert to ns
      groupDelay.append(val);
    }

    QString trace_name_GD = QString("S%1%2_Group Delay").arg(port_in).arg(port_out);
    datasets[file][trace_name_GD].append(groupDelay);
    return;
  }


  for (int i = 0; i < datasets[file]["S11_re"].size(); i++) {
    // S-parameter data (n.u.)
    double s11_re =  datasets[file]["S11_re"][i];
    double s11_im =  datasets[file]["S11_im"][i];
    s11 = std::complex<double>(s11_re, s11_im);
    s11_conj = std::complex<double>(s11_re, -s11_im);

    if ( datasets[file]["n_ports"].last() == 2) {
      double s12_re =  datasets[file]["S12_re"][i];
      double s12_im =  datasets[file]["S12_im"][i];
      double s21_re =  datasets[file]["S21_re"][i];
      double s21_im =  datasets[file]["S21_im"][i];
      double s22_re =  datasets[file]["S22_re"][i];
      double s22_im =  datasets[file]["S22_im"][i];
      s12 = std::complex<double> (s12_re, s12_im);
      s21 = std::complex<double> (s21_re, s21_im);
      s22 = std::complex<double> (s22_re, s22_im);
      s22_conj = std::complex<double> (s22_re, -s22_im);
    }

    double delta = abs(s11*s22 - s12*s21); // Determinant of the S matrix

      if (!metric.compare("|Δ|")) {
        datasets[file]["|Δ|"].append(delta);
      } else {
        if (!metric.compare("K")) {
          double K = (1 - abs(s11)*abs(s11) - abs(s22)*abs(s22) + delta*delta) / (2*abs(s12*s21)); // Rollet factor.
          datasets[file]["K"].append(K);
        } else {
          if (!metric.compare("μₛ")) {
            double mu = (1 - abs(s11)*abs(s11)) / (abs(s22-delta*s11_conj) + abs(s12*s21));
            datasets[file]["μₛ"].append(mu);
          } else {
            if (!metric.compare("μₚ")) {
              double mu_p = (1 - abs(s22)*abs(s22)) / (abs(s11-delta*s22_conj) + abs(s12*s21));
              datasets[file]["μₚ"].append(mu_p);
            } else {
              if (!metric.compare("MSG")) {
                double MSG = abs(s21) / abs(s12);
                MSG = 10*log10(MSG);
                datasets[file]["MSG"].append(MSG);
              } else {
                if (!metric.compare("MAG")) {
                  double K = (1 - abs(s11)*abs(s11) - abs(s22)*abs(s22) + delta*delta) / (2*abs(s12*s21)); // Rollet factor.
                  double MSG = abs(s21) / abs(s12);
                  double MAG = MSG * (K - std::sqrt(K * K - 1));
                  MAG = 10*log10(abs(MAG));
                  datasets[file]["MAG"].append(MAG);
                } else {
                  if (!metric.compare("Zin")) {
                    std::complex<double> Zin = std::complex<double>(Z0) * (1.0 + s11) / (1.0 - s11);
                    datasets[file]["Re{Zin}"].append(Zin.real());
                    datasets[file]["Im{Zin}"].append(Zin.imag());
                  } else {
                    if (!metric.compare("Zout")) {
                      std::complex<double> Zout = std::complex<double>(Z0) * (1.0 + s22) / (1.0 - s22);
                      datasets[file]["Re{Zout}"].append(Zout.real());
                      datasets[file]["Im{Zout}"].append(Zout.imag());
                    } else {
                      if (!metric.compare("Re{Zin}")) {
                        std::complex<double> Zin = std::complex<double>(Z0) * (1.0 + s11) / (1.0 - s11);
                        datasets[file]["Re{Zin}"].append(Zin.real());
                      } else {
                        if (!metric.compare("Im{Zin}")) {
                          std::complex<double> Zin = std::complex<double>(Z0) * (1.0 + s11) / (1.0 - s11);
                          datasets[file]["Im{Zin}"].append(Zin.imag());
                        } else {
                          if (!metric.compare("Re{Zout}")) {
                            std::complex<double> Zout = std::complex<double>(Z0) * (1.0 + s22) / (1.0 - s22);
                            datasets[file]["Re{Zout}"].append(Zout.real());
                          } else {
                            if (!metric.compare("Im{Zout}")) {
                              std::complex<double> Zout = std::complex<double>(Z0) * (1.0 + s22) / (1.0 - s22);
                              datasets[file]["Im{Zout}"].append(Zout.imag());
                            } else {
                              if (!metric.compare("VSWR{in}")) {
                                double s11_magnitude = abs(s11);
                                double VSWR = (1 + s11_magnitude) / (1 - s11_magnitude);
                                datasets[file]["VSWR{in}"].append(VSWR);
                              } else {
                                if (!metric.compare("VSWR{out}")){
                                  double s22_magnitude = abs(s22);
                                  double VSWR = (1 + s22_magnitude) / (1 - s22_magnitude);
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



// Gets the marker frequency based on the marker name
double Qucs_S_SPAR_Viewer::getMarkerFreq(QString markerName){
  // Check if marker exists
  if (!markerMap.contains(markerName)) {
    qWarning() << "Marker" << markerName << "not found!";
    return 0.0;
  }

  // Get the marker properties
  const MarkerProperties& props = markerMap[markerName];

  // Get the base frequency from the spin box
  double baseFrequency = props.freqSpinBox->value();

  // Get the scale factor from the combo box
  QString scaleText = props.scaleComboBox->currentText();
  double scaleFactor = 1.0;

  // Convert scale text to actual multiplication factor
  if (scaleText == "Hz") {
    scaleFactor = 1.0;
  } else if (scaleText == "kHz") {
    scaleFactor = 1e3;
  } else if (scaleText == "MHz") {
    scaleFactor = 1e6;
  } else if (scaleText == "GHz") {
    scaleFactor = 1e9;
  } else if (scaleText == "THz") {
    scaleFactor = 1e12;
  }

  // Apply scale factor to base frequency
  return baseFrequency * scaleFactor;
}

// Get the marker given the position of the entry
bool Qucs_S_SPAR_Viewer::getMarkerByPosition(int position, QString& outMarkerName, MarkerProperties& outProperties) {
  // Check if position is valid
  if (position < 0 || position >= markerMap.size()) {
    qWarning() << "Invalid position:" << position;
    return false;
  }

  // Get an iterator to the beginning of the map
  auto it = markerMap.begin();

  // Advance the iterator by 'position' steps
  std::advance(it, position);

  // Get the marker name and properties
  outMarkerName = it.key();
  outProperties = it.value();

  return true;
}

// Get the marker given the position of the entry
bool Qucs_S_SPAR_Viewer::getLimitByPosition(int position, QString& outLimitName, LimitProperties& outProperties) {
  // Check if position is valid
  if (position < 0 || position >= limitsMap.size()) {
    qWarning() << "Invalid position:" << position;
    return false;
  }

  // Get an iterator to the beginning of the map
  auto it = limitsMap.begin();

         // Advance the iterator by 'position' steps
  std::advance(it, position);

         // Get the marker name and properties
  outLimitName = it.key();
  outProperties = it.value();

  return true;
}

/*
// Get the trace given the position of the entry
bool Qucs_S_SPAR_Viewer::getTraceByPosition(int position, QString& outTraceName, TraceProperties& outProperties) {
  // Check if position is valid
  if (position < 0 || position >= traceMap.size()) {
    qWarning() << "Invalid position:" << position;
    return false;
  }

         // Get an iterator to the beginning of the map
  auto it = traceMap.begin();

         // Advance the iterator by 'position' steps
  std::advance(it, position);

         // Get the marker name and properties
  outTraceName = it.key();
  outProperties = it.value();

  return true;
}
*/

// Returns the total number of markers
int Qucs_S_SPAR_Viewer::getNumberOfMarkers(){
  return markerMap.keys().size();
}

// Returns the total number of traces
int Qucs_S_SPAR_Viewer::getNumberOfTraces(){
  return traceMap.keys().size();
}

// Returns the total number of limits
int Qucs_S_SPAR_Viewer::getNumberOfLimits(){
  return limitsMap.keys().size();
}


// Setup file watcher to monitor S-parameter files
void Qucs_S_SPAR_Viewer::setupFileWatcher()
{
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
void Qucs_S_SPAR_Viewer::fileChanged(const QString &path)
{
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
         // Wait a moment to see if the file reappears and to ensure file is fully written
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
    for (auto it = watchedFilePaths.begin(); it != watchedFilePaths.end(); ++it) {
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

  // Scan for new S-parameter files
  QDir dir(path);
  const QStringList newFiles = dir.entryList({"*.dat", "*.s*", "*.dat.ngspice"}, QDir::Files);

  QStringList paths;
  for(const QString& file : newFiles) {
    const QString fullPath = dir.absoluteFilePath(file);
    if(!filePaths.contains(file)) {
      paths.append(fullPath);
    }
  }
  this->addFiles(paths);
}

// This function is called when a file in the dataset has changes. It updates the traces in the display widgets
void Qucs_S_SPAR_Viewer::updateAllPlots(const QString& datasetName)
{
  // Refresh all traces on each chart
  updateTracesInWidget(Magnitude_PhaseChart, datasetName);
  updateTracesInWidget(smithChart, datasetName);
  updateTracesInWidget(polarChart, datasetName);
  updateTracesInWidget(impedanceChart, datasetName);
  updateTracesInWidget(GroupDelayChart, datasetName);
}

void Qucs_S_SPAR_Viewer::updateTracesInWidget(QWidget* widget, const QString& datasetName)
{
  if (!widget || !datasets.contains(datasetName))
    return;

  QMap<QString, QList<double>> dataset = datasets[datasetName];

  // Handle RectangularPlotWidget
  if (auto* rectWidget = qobject_cast<RectangularPlotWidget*>(widget)) {
    // Get current traces info to preserve settings like pen colors
    QMap<QString, QPen> tracesInfo = rectWidget->getTracesInfo();

    for (auto traceIt = tracesInfo.begin(); traceIt != tracesInfo.end(); ++traceIt) {
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
          const auto& traces = rectWidget->getTracesInfo();
          if (traces.contains(traceName)) {
            updatedTrace.pen = tracePen;
            // Other properties would need to be retrieved if available
            updatedTrace.units = ""; // Set appropriate units
            updatedTrace.Z0 = 50.0;  // Default or preserved value
            updatedTrace.y_axis = 0;  // Default to left axis
            updatedTrace.y_axis_title = dataKey;  // Default or preserved value
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
  else if (auto* polarWidget = qobject_cast<PolarPlotWidget*>(widget)) {
    // Get current traces info
    QMap<QString, QPen> tracesInfo = polarWidget->getTracesInfo();

    for (auto traceIt = tracesInfo.begin(); traceIt != tracesInfo.end(); ++traceIt) {
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

        if (dataset.contains("frequency") && dataset.contains(realKey) && dataset.contains(imagKey)) {
          // Set the updated data - convert real/imag to complex values
          updatedTrace.frequencies = dataset["frequency"];
          updatedTrace.values.clear();

          for (int i = 0; i < dataset["frequency"].size(); i++) {
            if (i < dataset[realKey].size() && i < dataset[imagKey].size()) {
              std::complex<double> value(dataset[realKey][i], dataset[imagKey][i]);
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

    // Polar widgets don't have an explicit updatePlot method based on the header
    // but we should trigger a redraw
    polarWidget->update();
  }
  // Handle SmithChartWidget
  else if (auto* smithWidget = qobject_cast<SmithChartWidget*>(widget)) {
    // Get current traces info
    QMap<QString, QPen> tracesInfo = smithWidget->getTracesInfo();

    for (auto traceIt = tracesInfo.begin(); traceIt != tracesInfo.end(); ++traceIt) {
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

        if (dataset.contains("frequency") && dataset.contains(realKey) && dataset.contains(imagKey)) {
          // Set the updated frequency data
          updatedTrace.frequencies = dataset["frequency"];
          updatedTrace.impedances.clear();

          QList<double> sii_re = datasets[file][realKey];
          QList<double> sii_im = datasets[file][imagKey];

          for (int i = 0; i < dataset["frequency"].size(); i++) {
            std::complex<double> sii(sii_re[i], sii_im[i]);
            std::complex<double> gamma = sii; // Reflection coefficient
            std::complex<double> impedance = Z0 * (1.0 + gamma) / (1.0 - gamma); // Convert to impedance
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
    //smithWidget->update();
  }
}


bool Qucs_S_SPAR_Viewer::isSparamFile(const QString& path) {
  QFileInfo fi(path);
  return fi.exists() &&
         (path.endsWith(".dat", Qt::CaseInsensitive) ||
          QRegularExpression(R"(\.s\d+p$)", QRegularExpression::CaseInsensitiveOption)
              .match(path).hasMatch());
}

void Qucs_S_SPAR_Viewer::addPathToWatcher(const QString &path) {
  if (QFileInfo(path).isDir()) {
    fileWatcher->addPath(path);
    qDebug() << "Watching directory:" << path;

           // Find all files ending with ".dat" or ".snp" (n is an integer), case-insensitive
    QDir dir(path);
    QStringList filters;
    filters << "*.dat" << "*.DAT" << "*.s*" << "*.S*"; // Add uppercase patterns
    dir.setNameFilters(filters);

    QStringList matchingFiles;
    QRegularExpression snpRegex(R"(\.s\d+p$)", QRegularExpression::CaseInsensitiveOption); // Case-insensitive

           // Iterate through all files in the directory
    for (const QString &fileName : dir.entryList(QDir::Files)) {
      QString lowerFileName = fileName.toLower();
      if (lowerFileName.endsWith(".dat") || snpRegex.match(fileName).hasMatch()) {
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


// This function is triggered when a trace-type tab is clicked in the trace management tab
void Qucs_S_SPAR_Viewer::raiseWidgetsOnTabSelection(int index) {
  switch (index){
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

