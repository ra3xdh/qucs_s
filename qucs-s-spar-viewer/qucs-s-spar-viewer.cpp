/****************************************************************************
**     Qucs Attenuator Synthesis
**     qucsattenuator.cpp
**
**
**
**
**
**
**
*****************************************************************************/

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

  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle("Qucs S-parameter Viewer " PACKAGE_VERSION);

  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction *fileQuit = new QAction(tr("&Quit"), this);
  fileQuit->setShortcut(QKeySequence::Quit);
  connect(fileQuit, SIGNAL(triggered(bool)), SLOT(slotQuit()));

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

  // Left panel
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
  connect(Delete_All_Files, SIGNAL(clicked()), SLOT(removeAllFiles()));


  hLayout_Files_Buttons->addWidget(Button_Add_File);
  hLayout_Files_Buttons->addWidget(Delete_All_Files);

  scrollArea_Files->setWidget(FileList_Widget);
  scrollArea_Files->setWidgetResizable(true);
  vLayout_Files->addWidget(scrollArea_Files, Qt::AlignTop);
  vLayout_Files->addWidget(Buttons, Qt::AlignBottom);
  vLayout_Files->setStretch(0, 3);
  vLayout_Files->setStretch(1, 1);

  // Chart settings
  chart = new QChart;
  chart->createDefaultAxes();
  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(nullptr);
  dockChart = new QDockWidget("Chart", this);
  dockChart->setWidget(chartView);
  dockChart->setAllowedAreas(Qt::AllDockWidgetAreas);
  addDockWidget(Qt::LeftDockWidgetArea, dockChart);



  // These are two maximum markers to find the lowest and the highest frequency in the data samples.
  // They are used to prevent the user from zooming out too much
  f_min = 1e20;
  f_max = -1;
  y_min = 1e4;
  y_max = -1e4;

  // Load default colors
  default_colors.append(QColor(Qt::red));
  default_colors.append(QColor(Qt::blue));
  default_colors.append(QColor(Qt::darkGreen));

  // Right panel
  QWidget * SettingsGroup = new QWidget();
  QGridLayout * SettingsGrid = new QGridLayout(SettingsGroup);
  SettingsGrid->setSpacing(5);
  SettingsGrid->setColumnMinimumWidth(3, 20);

  // First row (min, max, div)
  QLabel *axis_min = new QLabel("<b>min</b>");
  SettingsGrid->addWidget(axis_min, 0, 1, Qt::AlignCenter);

  QLabel *axis_max = new QLabel("<b>max</b>");
  SettingsGrid->addWidget(axis_max, 0, 2, Qt::AlignCenter);

  QLabel *axis_div = new QLabel("<b>div</b>");
  SettingsGrid->addWidget(axis_div, 0, 3, Qt::AlignCenter);

  // x-axis
  QLabel *x_axis = new QLabel("<b>x-axis</b>");
  SettingsGrid->addWidget(x_axis, 1, 0);

  QSpinBox_x_axis_min = new QDoubleSpinBox();
  QSpinBox_x_axis_min->setMinimum(0.1);
  QSpinBox_x_axis_min->setMaximum(1000000);
  QSpinBox_x_axis_min->setValue(0);
  QSpinBox_x_axis_min->setDecimals(1);
  QSpinBox_x_axis_min->setSingleStep(0.1);
  connect(QSpinBox_x_axis_min, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
  SettingsGrid->addWidget(QSpinBox_x_axis_min, 1, 1);

  QSpinBox_x_axis_max = new QDoubleSpinBox();
  QSpinBox_x_axis_max->setMinimum(0.1);
  QSpinBox_x_axis_max->setMaximum(1000000);
  QSpinBox_x_axis_max->setValue(1000);
  QSpinBox_x_axis_max->setDecimals(1);
  QSpinBox_x_axis_max->setSingleStep(0.1);
  connect(QSpinBox_x_axis_max, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
  SettingsGrid->addWidget(QSpinBox_x_axis_max, 1, 2);

  // Available x-axis div
  QComboBox_x_axis_div = new QComboBox();
  available_x_axis_div.clear();
  available_x_axis_div << 2000 << 1000 << 500 << 400 << 200 << 100 << 50 << 25 << 20 << 10 << 5 << 1 << 0.5 << 0.2 << 0.1;

  for (const double &value : qAsConst(available_x_axis_div)) {
    QComboBox_x_axis_div->addItem(QString::number(value));
  }

  connect(QComboBox_x_axis_div, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));
  SettingsGrid->addWidget(QComboBox_x_axis_div, 1, 3);

  QCombobox_x_axis_units = new QComboBox();
  QCombobox_x_axis_units->addItem("Hz");
  QCombobox_x_axis_units->addItem("kHz");
  QCombobox_x_axis_units->addItem("MHz");
  QCombobox_x_axis_units->addItem("GHz");
  QCombobox_x_axis_units->setCurrentIndex(2);
  connect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), SLOT(changeFreqUnits()));
  SettingsGrid->addWidget(QCombobox_x_axis_units, 1, 4);

  // y-axis
  QLabel *y_axis = new QLabel("<b>y-axis</b>");
  SettingsGrid->addWidget(y_axis, 2, 0);

  QSpinBox_y_axis_min = new QDoubleSpinBox();
  QSpinBox_y_axis_min->setMinimum(-150);
  QSpinBox_y_axis_min->setValue(-50);
  QSpinBox_y_axis_min->setDecimals(1);
  connect(QSpinBox_y_axis_min, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
  SettingsGrid->addWidget(QSpinBox_y_axis_min, 2, 1);

  QSpinBox_y_axis_max = new QDoubleSpinBox();
  QSpinBox_y_axis_max->setMinimum(-150);
  QSpinBox_y_axis_max->setValue(0);
  QSpinBox_y_axis_max->setDecimals(1);
  connect(QSpinBox_y_axis_max, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
  SettingsGrid->addWidget(QSpinBox_y_axis_max, 2, 2);

  // Available x-axis div
  QComboBox_y_axis_div = new QComboBox();
  available_y_axis_div.clear();
  available_y_axis_div << 50 << 25 << 20 << 10 << 5 << 2 << 1 << 0.5 << 0.2 << 0.1;
  for (const double &value : qAsConst(available_y_axis_div)) {
      QComboBox_y_axis_div->addItem(QString::number(value));
  }
  connect(QComboBox_y_axis_div, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));
  SettingsGrid->addWidget(QComboBox_y_axis_div, 2, 3);

  /*QCombobox_y_axis_units = new QComboBox();
  QCombobox_y_axis_units->addItem("dB");
  SettingsGrid->addWidget(QCombobox_y_axis_units, 2, 4);*/

  QLabel *y2_axis = new QLabel("<b>y2-axis</b>");
  SettingsGrid->addWidget(y2_axis, 3, 0);

  QSpinBox_y2_axis_min = new QDoubleSpinBox();
  QSpinBox_y2_axis_min->setMinimum(0);
  SettingsGrid->addWidget(QSpinBox_y2_axis_min, 3, 1);

  QSpinBox_y2_axis_max = new QDoubleSpinBox();
  QSpinBox_y2_axis_max->setMinimum(0);
  SettingsGrid->addWidget(QSpinBox_y2_axis_max, 3, 2);

  QSpinBox_y2_axis_div = new QDoubleSpinBox();
  QSpinBox_y2_axis_div->setMinimum(0);
  SettingsGrid->addWidget(QSpinBox_y2_axis_div, 3, 3);

  /*QCombobox_y2_axis_units = new QComboBox();
  QCombobox_y2_axis_units->addItem("dB");
  SettingsGrid->addWidget(QCombobox_y2_axis_units, 3, 4);*/

  // Hide y2 axis (temporary)
  y2_axis->hide();
  QSpinBox_y2_axis_min->hide();
  QSpinBox_y2_axis_max->hide();
  QSpinBox_y2_axis_div->hide();
 // QCombobox_y2_axis_units->hide();

  // Lock axis settings button
  Lock_axis_settings_Button =  new QPushButton("Lock Axes");
  Lock_axis_settings_Button->setCheckable(true);
  connect(Lock_axis_settings_Button, SIGNAL(clicked(bool)), SLOT(lock_unlock_axis_settings()));
  lock_axis = false;

  SettingsGrid->addWidget(Lock_axis_settings_Button, 0, 4);

  QWidget * TracesGroup = new QWidget();
  QVBoxLayout *Traces_VBox = new QVBoxLayout(TracesGroup);

  // Trace addition box
  QWidget * TraceSelection_Widget = new QWidget(); // Add trace

  QGridLayout * DatasetsGrid = new QGridLayout(TraceSelection_Widget);
  QLabel *dataset_label = new QLabel("<b>Dataset</b>");
  DatasetsGrid->addWidget(dataset_label, 0, 0, Qt::AlignCenter);

  QLabel *Traces_label = new QLabel("<b>Traces</b>");
  DatasetsGrid->addWidget(Traces_label, 0, 1, Qt::AlignCenter);

  QLabel *empty_label = new QLabel("<b>Empty</b>");
  DatasetsGrid->addWidget(empty_label, 0, 2, Qt::AlignCenter);
  empty_label->hide();

  QCombobox_datasets = new QComboBox();
  DatasetsGrid->addWidget(QCombobox_datasets, 1, 0);
  connect(QCombobox_datasets, SIGNAL(currentIndexChanged(int)), SLOT(updateTracesCombo())); // Each time the dataset is changed it is needed to update the traces combo.
                                                                    // This is needed when the user has data with different number of ports.


  QCombobox_traces = new QComboBox();
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

  DatasetsGrid->addWidget(Button_add_trace, 1, 2);

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

  QScrollArea *scrollArea_Traces = new QScrollArea();
  scrollArea_Traces->setWidget(TracesList_Widget);
  scrollArea_Traces->setWidgetResizable(true);

  Traces_VBox->addWidget(TraceSelection_Widget);
  Traces_VBox->addWidget(scrollArea_Traces);

  // Markers dock
  QWidget * MarkersGroup = new QWidget();
  QVBoxLayout *Markers_VBox = new QVBoxLayout(MarkersGroup);

  // Trace addition box
  QWidget * MarkerSelection_Widget = new QWidget();

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
  QWidget * MarkerList_Widget = new QWidget(); // Panel with the trace settings

  QLabel * Label_Marker = new QLabel("<b>Marker</b>");
  QLabel * Label_Freq_Marker = new QLabel("<b>Frequency</b>");
  QLabel * Label_Freq_Scale_Marker = new QLabel("<b>Units</b>");
  QLabel * Label_Remove_Marker = new QLabel("<b>Remove</b>");

  MarkersGrid = new QGridLayout(MarkerList_Widget);
  MarkersGrid->addWidget(Label_Marker, 0, 0, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Freq_Marker, 0, 1, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Freq_Scale_Marker, 0, 2, Qt::AlignCenter);
  MarkersGrid->addWidget(Label_Remove_Marker, 0, 3, Qt::AlignCenter);


  QScrollArea *scrollArea_Marker = new QScrollArea();
  scrollArea_Marker->setWidget(MarkerList_Widget);
  scrollArea_Marker->setWidgetResizable(true);

  tableMarkers = new QTableWidget(1, 1, this);

  Markers_VBox->addWidget(MarkerSelection_Widget);
  Markers_VBox->addWidget(scrollArea_Marker);
  Markers_VBox->addWidget(tableMarkers);

  // Limits dock

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
  Limits_VBox->addWidget(scrollArea_Limits);



  dockFiles = new QDockWidget("S-parameter files", this);
  dockAxisSettings = new QDockWidget("Axis Settings", this);
  dockTracesList = new QDockWidget("Traces List", this);
  dockMarkers = new QDockWidget("Markers", this);
  dockLimits = new QDockWidget("Limits", this);

  // Disable dock closing
  dockChart->setFeatures(dockChart->features() & ~QDockWidget::DockWidgetClosable);
  dockFiles->setFeatures(dockFiles->features() & ~QDockWidget::DockWidgetClosable);
  dockAxisSettings->setFeatures(dockAxisSettings->features() & ~QDockWidget::DockWidgetClosable);
  dockTracesList->setFeatures(dockTracesList->features() & ~QDockWidget::DockWidgetClosable);
  dockMarkers->setFeatures(dockMarkers->features() & ~QDockWidget::DockWidgetClosable);
  dockLimits->setFeatures(dockLimits->features() & ~QDockWidget::DockWidgetClosable);

  dockAxisSettings->setWidget(SettingsGroup);
  dockTracesList->setWidget(TracesGroup);
  dockFiles->setWidget(FilesGroup);
  dockMarkers->setWidget(MarkersGroup);
  dockLimits->setWidget(LimitsGroup);

  addDockWidget(Qt::RightDockWidgetArea, dockAxisSettings);
  addDockWidget(Qt::RightDockWidgetArea, dockTracesList);
  addDockWidget(Qt::RightDockWidgetArea, dockFiles);
  addDockWidget(Qt::RightDockWidgetArea, dockMarkers);
  addDockWidget(Qt::RightDockWidgetArea, dockLimits);

  splitDockWidget(dockTracesList, dockAxisSettings, Qt::Vertical);
  tabifyDockWidget(dockFiles, dockTracesList);
  tabifyDockWidget(dockTracesList, dockMarkers);
  tabifyDockWidget(dockMarkers, dockLimits);
  dockFiles->raise();
  setDockNestingEnabled(true);

  // Set the height of the axis settings widget to its minimum. This makes the layout much clearer
  int minHeight = dockAxisSettings->minimumSizeHint().height();
  dockAxisSettings->setFixedHeight(minHeight);

  setAcceptDrops(true);//Enable drag and drop feature to open files
}

Qucs_S_SPAR_Viewer::~Qucs_S_SPAR_Viewer()
{
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
    tr("\nCopyright (C) 2024 by")+" Andrés Martínez Mera"
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
    QFileDialog dialog(this, QString("Select S-parameter data files (.snp)"), QDir::homePath(),
                       tr("S-Parameter Files (*.s1p *.s2p *.s3p *.s4p);;All Files (*.*)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    addFiles(fileNames);
}

void Qucs_S_SPAR_Viewer::addFiles(QStringList fileNames)
{
    int existing_files = this->datasets.size(); // Get the number of entries in the map

    // Variables for reading the Touchstone data
    QStringList values;
    QString filename;

    if (existing_files == 0){
        // Reset limits
        this->f_max = -1;
        this->f_min = 1e30;
        this->y_min = 1e30;
        this->y_max = -1e30;
    }


    // Remove from the list of files those that already exist in the database
    QStringList files_dataset = datasets.keys();

    for (int i = 0; i < fileNames.length(); i++){
      filename = QFileInfo(fileNames.at(i)).fileName();
      // Check if this file already exists
      QString new_filename = filename.left(filename.indexOf('.'));
      if (files_dataset.contains(new_filename)){
        // Remove it from the list of new files to load
        fileNames.removeAt(i);

        // Pop up a warning
        QMessageBox::information(
            this,
            tr("Warning"),
            tr("This file is already in the dataset.") );

      }
    }

    // Read files
    for (int i = existing_files; i < existing_files+fileNames.length(); i++)
    {
        // Create the file name label
        filename = QFileInfo(fileNames.at(i-existing_files)).fileName();

        QLabel * Filename_Label = new QLabel(filename.left(filename.indexOf('.')));
        Filename_Label->setObjectName(QString("File_") + QString::number(i));
        List_FileNames.append(Filename_Label);
        this->FilesGrid->addWidget(List_FileNames.last(), i,0,1,1);

        // Create the "Remove" button
        QToolButton * RemoveButton = new QToolButton();
        RemoveButton->setObjectName(QString("Remove_") + QString::number(i));
        QIcon icon(":/bitmaps/trash.png"); // Use a resource path or a relative path
        RemoveButton->setIcon(icon);

        RemoveButton->setStyleSheet("QToolButton {background-color: red;\
                                        border-width: 2px;\
                                        border-radius: 10px;\
                                        border-color: beige;\
                                        font: bold 14px;\
                                    }");

        List_RemoveButton.append(RemoveButton);
        this->FilesGrid->addWidget(List_RemoveButton.last(), i,1,1,1);


        connect(RemoveButton, SIGNAL(clicked()), SLOT(removeFile())); // Connect button with the handler to remove the entry.

        // Read the Touchstone file.
        // Please see https://ibis.org/touchstone_ver2.0/touchstone_ver2_0.pdf
        QMap<QString, QList<double>> file_data; // Data structure to store the file data
        QString frequency_unit, parameter, format;
        double freq_scale = 1; // Hz
        double Z0=50; // System impedance. Typically 50 Ohm

        // Get the number of ports
        QString suffix = QFileInfo(filename).suffix();
        QRegularExpression regex("(?i)[sp]");
        QStringList numberParts = suffix.split(regex);
        int number_of_ports = numberParts[1].toInt();
        file_data["n_ports"].append(number_of_ports);

        // 1) Open the file
        QFile file(fileNames.at(i-existing_files));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Cannot open the file";
            break;
        }

        // 2) Read data
        QTextStream in(&file);
        while (!in.atEnd()) {
          QString line = in.readLine();
          line = line.simplified();
          //qDebug() << line;

           if (line.isEmpty()) continue;
           if ((line.at(0).isNumber() == false) && (line.at(0) != '#')) {
               if (file_data["frequency"].size() == 0){
                   // There's still no data
                   continue;
               }else{
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


               parameter = info.at(2); // specifies what kind of network parameter data is contained in the file. Legal
                                       // values are:
                                       // S for Scattering parameters,
                                       // Y for Admittance parameters,
                                       // Z for Impedance parameters,
                                       // H for Hybrid-h parameters,
                                       // G for Hybrid-g parameters.
                                       // The default value is S.

               format = info.at(3);   // Specifies the format of the network parameter data pairs. Legal values are:
                                      // DB for decibel-angle (decibel = 20 × log 10|magnitude|)
                                      // MA for magnitude-angle,
                                      // RI for real-imaginary.
                                      // Angles are given in degrees. Note that this format does not apply to noise
                                      // parameters (refer to the “Noise Parameter Data” section later in this
                                      // specification). The default value is MA.

               Z0 = info.at(5).toDouble();
               file_data["Rn"].append(Z0); // Specifies the reference resistance in ohms, where n is a real, positive number of
                                           // ohms. The default reference resistance is 50 ohms. Note that this is overridden
                                           // by the [Reference] keyword, described below, for files of [Version] 2.0 and above

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
                   s1 = QString("S") + QString::number(j) + QString::number(i) + QString("_dB");
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
           if (number_of_ports == 1){
               double s11_re = file_data["S11_re"].last();
               double s11_im = file_data["S11_im"].last();
               std::complex<double> s11 (s11_re, s11_im);

               // Calculate Zin and Zout
               std::complex<double> Zin = Z0 * (1.0 + s11) / (1.0 - s11);

               file_data["Re{Zin}"].append(Zin.real()); // Re{Zin}
               file_data["Im{Zin}"].append(Zin.imag()); // Im{Zin}

           }
           if (number_of_ports == 2){
               // Compute delta, K, mus, mup, MAG and MSG
               double s11_re = file_data["S11_re"].last();
               double s11_im = file_data["S11_im"].last();
               double s12_re = file_data["S12_re"].last();
               double s12_im = file_data["S12_im"].last();
               double s21_re = file_data["S21_re"].last();
               double s21_im = file_data["S21_im"].last();
               double s22_re = file_data["S22_re"].last();
               double s22_im = file_data["S22_im"].last();

               std::complex<double> s11 (s11_re, s11_im);
               std::complex<double> s11_conj (s11_re, -s11_im);
               std::complex<double> s12 (s12_re, s12_im);
               std::complex<double> s21 (s21_re, s21_im);
               std::complex<double> s22 (s22_re, s22_im);
               std::complex<double> s22_conj (s22_re, -s22_im);

               double delta = abs(s11*s22 - s12*s21); // Determinant of the S matrix
               double K = (1 - abs(s11)*abs(s11) - abs(s22)*abs(s22) + delta*delta) / (2*abs(s12*s21)); // Rollet factor.
               double mu = (1 - abs(s11)*abs(s11)) / (abs(s22-delta*s11_conj) + abs(s12*s21));
               double mu_p = (1 - abs(s22)*abs(s22)) / (abs(s11-delta*s22_conj) + abs(s12*s21));
               double MSG = abs(s21) / abs(s12);
               double MAG = MSG * (K - std::sqrt(K * K - 1));

               // Calculate Zin and Zout
               std::complex<double> Zin = std::complex<double>(Z0) * (1.0 + s11) / (1.0 - s11);
               std::complex<double> Zout = std::complex<double>(Z0) * (1.0 + s22) / (1.0 - s22);

               // Convert MSG and MAG to dB scale
               MSG = 10*log10(MSG);
               MAG = 10*log10(abs(MAG));

               file_data["delta"].append(delta); //delta
               file_data["K"].append(delta); //K
               file_data["mu"].append(mu); //mu
               file_data["mu_p"].append(mu_p); //mu_p
               file_data["MSG"].append(MSG); //MSG
               file_data["MAG"].append(MAG); //MAG
               file_data["Re{Zin}"].append(Zin.real()); // Re{Zin}
               file_data["Im{Zin}"].append(Zin.imag()); // Im{Zin}
               file_data["Re{Zout}"].append(Zout.real()); // Re{Zout}
               file_data["Im{Zout}"].append(Zout.imag()); // Im{Zout}
           }
        }
        // 3) Add data to the dataset
        filename = filename.left(filename.indexOf('.')); // Remove file extension
        datasets[filename] = file_data;
        file.close();

        // 4) Add new dataset to the trace selection combobox
        QCombobox_datasets->addItem(filename);
        // Update traces
        updateTracesCombo();
    }

    // Default behavior: If there's no more data loaded and a single S1P file is selected, then automatically plot S11
    if ((fileNames.length() == 1) && (fileNames.first().toLower().endsWith(".s1p")) && (datasets.size() == 1)){
        this->addTrace(filename, QString("S11"), Qt::red);

        adjust_x_axis_to_file(filename);
        adjust_y_axis_to_trace(filename, "S11");
    }

    // Default behavior: If there's no more data loaded and a single S2P file is selected, then automatically plot S21, S11 and S22
    if ((fileNames.length() == 1) && (fileNames.first().toLower().endsWith(".s2p")) && (datasets.size() == 1)){
        this->addTrace(filename, QString("S21"), Qt::red);
        this->addTrace(filename, QString("S11"), Qt::blue);
        this->addTrace(filename, QString("S22"), Qt::darkGreen);

        adjust_x_axis_to_file(filename);
        adjust_y_axis_to_trace(filename, "S11");
        adjust_y_axis_to_trace(filename, "S21");
    }

    // Default behaviour: When adding multiple S2P file, then show the S21 of all traces
    if (fileNames.length() > 1){
        bool all_s2p = true;
        for (int i = 0; i < fileNames.length(); i++){
            if (!fileNames.at(i).toLower().endsWith(".s2p")){
                all_s2p = false;
                break;
            }
        }
        if (all_s2p == true){
            QString filename;
            for (int i = 0; i < fileNames.length(); i++){
                filename = QFileInfo(fileNames.at(i)).fileName();
                filename = filename.left(filename.indexOf('.'));
                // Pick a random color
                QColor trace_color = QColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
                this->addTrace(filename, QString("S21"), trace_color);
                adjust_y_axis_to_trace(filename, "S21");
            }
            // Update the frequency setting to fit the last s2p file
            adjust_x_axis_to_file(filename);

        }
    }

    // Show the trace settings widget
    dockTracesList->raise();
}

// This function is called whenever a s-par file is intended to be removed from the map of datasets
void Qucs_S_SPAR_Viewer::removeFile()
{
    QString ID = qobject_cast<QToolButton*>(sender())->objectName();
    //qDebug() << "Clicked button:" << ID;

    //Find the index of the button to remove
    int index_to_delete = -1;
    for (int i = 0; i < List_RemoveButton.size(); i++) {
        if (List_RemoveButton.at(i)->objectName() == ID) {
            index_to_delete = i;
            break;
        }
    }

    removeFile(index_to_delete);
}


void Qucs_S_SPAR_Viewer::removeFile(int index_to_delete)
{


    // Delete the label
    QLabel* labelToRemove = List_FileNames.at(index_to_delete);
    QString dataset_to_remove = labelToRemove->text();
    FilesGrid->removeWidget(labelToRemove);
    List_FileNames.removeAt(index_to_delete);
    delete labelToRemove;

    // Delete the button
    QToolButton* ButtonToRemove = List_RemoveButton.at(index_to_delete);
    FilesGrid->removeWidget(ButtonToRemove);
    List_RemoveButton.removeAt(index_to_delete);
    delete ButtonToRemove;

    // Look for the widgets associated to the trace and remove them
    QList<int> indices_to_remove;
    for (int i = 0; i < List_TraceNames.size(); i++){
        QString trace_name = List_TraceNames.at(i)->text();
        QStringList parts = trace_name.split(".");//Trace name = dataset + trace
        QString dataset_trace = parts[0];
        if (dataset_trace == dataset_to_remove ){
            QString Label_Object_Name = List_TraceNames.at(i)->objectName();

            //Find the index of the button to remove
            int index_to_delete = -1;
            for (int j = 0; j < List_TraceNames.size(); j++) {
                if (List_TraceNames.at(j)->objectName() == Label_Object_Name) {
                    index_to_delete = j;
                    break;
                }
            }
            indices_to_remove.append(index_to_delete);
        }
    }

    // Once the list of widgets to remove is known, then remove them on a row
    std::sort(indices_to_remove.begin(), indices_to_remove.end(), std::greater<int>()); // Sort the items to avoid segfault when removing the widgets
    removeTrace(indices_to_remove);

    // Delete the map entry
    datasets.remove(dataset_to_remove);

    // Rebuild the dataset combobox based on the available datasets.
    QStringList new_dataset_entries = datasets.keys();

    disconnect(QCombobox_datasets, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTracesCombo())); // Needed to avoid segfault
    QCombobox_datasets->clear();
    QCombobox_datasets->addItems(new_dataset_entries);
    connect(QCombobox_datasets, SIGNAL(currentIndexChanged(int)), SLOT(updateTracesCombo())); // Connect the signal again

    // Update the combobox for trace selection
    updateTracesCombo();

    // Now it is needed to readjust the widgets in the grid layout
    // Move up all widgets below the removed row
    for (int r = index_to_delete+1; r < FilesGrid->rowCount(); r++) {
        for (int c = 0; c < FilesGrid->columnCount(); c++) {
            QLayoutItem* item = FilesGrid->itemAtPosition(r, c);
            if (item) {
                int oldRow, oldCol, rowSpan, colSpan;
                FilesGrid->getItemPosition(FilesGrid->indexOf(item), &oldRow, &oldCol, &rowSpan, &colSpan);
                FilesGrid->removeItem(item);
                FilesGrid->addItem(item, oldRow - 1, oldCol, rowSpan, colSpan);
            }
        }
    }

    // Check if there are more files. If not, remove markers and limits
    if (datasets.size() == 0)
    {
        removeAllMarkers();
        removeAllLimits();
    }
}

void Qucs_S_SPAR_Viewer::removeAllFiles()
{
    int n_files = List_RemoveButton.size();
    for (int i = 0; i < n_files; i++) {
        removeFile(n_files-i-1);
    }
}


// This function is called when the user wants to remove a trace from the plot
void Qucs_S_SPAR_Viewer::removeTrace()
{
    QString ID = qobject_cast<QToolButton*>(sender())->objectName();
    //qDebug() << "Clicked button:" << ID;

    //Find the index of the button to remove
    int index_to_delete = -1;
    for (int i = 0; i < List_Button_DeleteTrace.size(); i++) {
        if (List_Button_DeleteTrace.at(i)->objectName() == ID) {
            index_to_delete = i;
            break;
        }
    }
    removeTrace(index_to_delete);
}

// This function is called when the user wants to remove a trace from the plot
void Qucs_S_SPAR_Viewer::removeTrace(QList<int> indices_to_delete)
{
    if (indices_to_delete.isEmpty())
        return;

    for (int i = 0; i < indices_to_delete.size(); i++)
        removeTrace(indices_to_delete.at(i));
}

void Qucs_S_SPAR_Viewer::removeTrace(int index_to_delete)
{
    // Delete the label
    QLabel* labelToRemove = List_TraceNames.at(index_to_delete);
    QString trace_name = labelToRemove->text();
    TracesGrid->removeWidget(labelToRemove);
    List_TraceNames.removeAt(index_to_delete);
    delete labelToRemove;

    // Delete the color button
    QPushButton* ColorButtonToRemove = List_Trace_Color.at(index_to_delete);
    TracesGrid->removeWidget(ColorButtonToRemove);
    List_Trace_Color.removeAt(index_to_delete);
    delete ColorButtonToRemove;

    // Delete the linestyle combo
    QComboBox* ComboToRemove = List_Trace_LineStyle.at(index_to_delete);
    TracesGrid->removeWidget(ComboToRemove);
    List_Trace_LineStyle.removeAt(index_to_delete);
    delete ComboToRemove;

    // Delete the width spinbox
    QSpinBox * SpinToRemove = List_TraceWidth.at(index_to_delete);
    TracesGrid->removeWidget(SpinToRemove);
    List_TraceWidth.removeAt(index_to_delete);
    delete SpinToRemove;

    // Delete the "delete" button
    QToolButton* ButtonToRemove = List_Button_DeleteTrace.at(index_to_delete);
    TracesGrid->removeWidget(ButtonToRemove);
    List_Button_DeleteTrace.removeAt(index_to_delete);
    delete ButtonToRemove;

    // Remove the trace from the QMap
    trace_list.removeAll(trace_name);

    // Update graphs in QChart plot
    removeSeriesByName(chart, trace_name);

    // Update the chart limits.
    this->f_max = -1;
    this->f_min = 1e30;

    QStringList files = datasets.keys();
    for (int i = 0; i < files.size(); i++){
        adjust_x_axis_to_file(files[i]);
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
        S_re = *S_1 * std::cos(*S_2);
        S_im = *S_1 * std::sin(*S_2);
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
double Qucs_S_SPAR_Viewer::getFreqScale()
{
    QString frequency_unit = QCombobox_x_axis_units->currentText();
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


void Qucs_S_SPAR_Viewer::addTrace()
{
    QString selected_dataset, selected_trace;
    selected_dataset = this->QCombobox_datasets->currentText();
    selected_trace = this->QCombobox_traces->currentText();

    // Color settings
    QColor trace_color;
    QPen pen;
    int num_traces = trace_list.size();
    if (num_traces >= 3){
        trace_color = QColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
        pen.setColor(trace_color);
    }
    else {
        trace_color = this->default_colors.at(num_traces);
    }

    addTrace(selected_dataset, selected_trace, trace_color);
}


// Read the dataset and trace Comboboxes and add a trace to the display list
void Qucs_S_SPAR_Viewer::addTrace(QString selected_dataset, QString selected_trace, QColor trace_color)
{
    int n_trace = this->trace_list.size()+1; // Number of displayed traces;
    // Get the name of the selected dataset


    // Get the name of the trace to plot
    QString trace_name = selected_dataset;
    trace_name.append("."); // Separate the dataset from the trace name with a point
    trace_name.append(selected_trace);

    if (trace_list.contains(trace_name)){
        QMessageBox::information(
            this,
            tr("Warning"),
            tr("This trace is already shown") );
        return;
    }

    // Add the trace to the list of displayed list and create the widgets associated to the trace properties

    // Label
    QLabel * new_trace_label = new QLabel(trace_name);
    new_trace_label->setObjectName(QString("Trace_Name_") + QString::number(n_trace));
    List_TraceNames.append(new_trace_label);
    this->TracesGrid->addWidget(new_trace_label, n_trace, 0);

    // Color picker
    QPushButton * new_trace_color = new QPushButton();
    new_trace_color->setObjectName(QString("Trace_Color_") + QString::number(n_trace));
    connect(new_trace_color, SIGNAL(clicked()), SLOT(changeTraceColor()));
    QString styleSheet = QString("QPushButton { background-color: %1; }").arg(trace_color.name());
    new_trace_color->setStyleSheet(styleSheet);
    new_trace_color->setAttribute(Qt::WA_TranslucentBackground); // Needed for Windows buttons to behave as they should
    List_Trace_Color.append(new_trace_color);
    this->TracesGrid->addWidget(new_trace_color, n_trace, 1);

    // Line Style
    QComboBox * new_trace_linestyle = new QComboBox();
    new_trace_linestyle->setObjectName(QString("Trace_LineStyle_") + QString::number(n_trace));
    new_trace_linestyle->addItem("Solid");
    new_trace_linestyle->addItem("- - - -");
    new_trace_linestyle->addItem("·······");
    new_trace_linestyle->addItem("-·-·-·-");
    new_trace_linestyle->addItem("-··-··-");
    connect(new_trace_linestyle, SIGNAL(currentIndexChanged(int)), SLOT(changeTraceLineStyle()));
    List_Trace_LineStyle.append(new_trace_linestyle);
    this->TracesGrid->addWidget(new_trace_linestyle, n_trace, 2);

    // Line width
    QSpinBox * new_trace_width = new QSpinBox();
    new_trace_width->setObjectName(QString("Trace_Width_") + QString::number(n_trace));
    new_trace_width->setValue(1);
    connect(new_trace_width, SIGNAL(valueChanged(int)), SLOT(changeTraceWidth()));
    List_TraceWidth.append(new_trace_width);
    this->TracesGrid->addWidget(new_trace_width, n_trace, 3);


    // Remove button
    QToolButton * new_trace_removebutton = new QToolButton();
    new_trace_removebutton->setObjectName(QString("Trace_RemoveButton_") + QString::number(n_trace));
    QIcon icon(":/bitmaps/trash.png"); // Use a resource path or a relative path
    new_trace_removebutton->setIcon(icon);
    new_trace_removebutton->setStyleSheet(R"(
            QToolButton {
                background-color: #FF0000;
                color: white;
                border-radius: 20px;
            }
        )");
    connect(new_trace_removebutton, SIGNAL(clicked()), SLOT(removeTrace()));
    List_Button_DeleteTrace.append(new_trace_removebutton);
    this->TracesGrid->addWidget(new_trace_removebutton, n_trace, 4, Qt::AlignCenter);

    adjust_x_axis_to_file(selected_dataset);
    adjust_y_axis_to_trace(selected_dataset, selected_trace);

    QLineSeries* series = new QLineSeries();
    series->setName(trace_name);
    trace_list.append(trace_name);

    // Color settings
    QPen pen;
    pen.setColor(trace_color);
    series->setPen(pen);// Apply the pen to the series

    chart->addSeries(series);


    updatePlot();
}

// This function is used for setting the available traces depending on the selected dataset
void Qucs_S_SPAR_Viewer::updateTracesCombo()
{
    QCombobox_traces->clear();
    QStringList traces;
    QString current_dataset = QCombobox_datasets->currentText();
    if (current_dataset.isEmpty())
        return; // No datasets loaded. This happens if the user had one single file and deleted it
    int n_ports = datasets[current_dataset]["n_ports"].at(0);

    for (int i=1; i<=n_ports; i++){
        for (int j=1; j<=n_ports; j++){
            traces.append(QString("S") + QString::number(i) + QString::number(j));
        }
    }

    if(n_ports == 1){
        // Additional traces
        traces.append("Re{Zin}");
        traces.append("Im{Zin}");
    }

    if(n_ports == 2){
        // Additional traces
        traces.append(QString("|%1|").arg(QChar(0x0394)));
        traces.append("K");
        traces.append(QString("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209B)));
        traces.append(QString("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209A)));
        traces.append("MAG");
        traces.append("MSG");
        traces.append("Re{Zin}");
        traces.append("Im{Zin}");
        traces.append("Re{Zout}");
        traces.append("Im{Zout}");
    }

    QCombobox_traces->addItems(traces);
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
                  QString styleSheet = QString("QPushButton { background-color: %1; }").arg(color.name());
                  button->setStyleSheet(styleSheet);

                    QString ID = button->objectName();

                    int index_to_change_color = -1;
                    for (int i = 0; i < List_Trace_Color.size(); i++) {
                        if (List_Trace_Color.at(i)->objectName() == ID) {
                            index_to_change_color = i;
                            break;
                        }
                    }

                    QLabel* label = List_TraceNames.at(index_to_change_color);
                    QString trace_name = label->text();

                    // Change the color of the series named based on its name
                    const auto seriesList = chart->series();
                    for (QAbstractSeries *s : seriesList) {
                        QLineSeries *lineSeries = qobject_cast<QLineSeries*>(s);
                        if (lineSeries && lineSeries->name() == trace_name) {
                            QPen pen = lineSeries->pen();
                            pen.setColor(color);
                            lineSeries->setPen(pen);
                            break;
                        }
                    }
                }
            }
}

// This is the handler that is triggered when the user hits the button to change the line style of a given trace
void Qucs_S_SPAR_Viewer::changeTraceLineStyle()
{
    QComboBox *combo = qobject_cast<QComboBox*>(sender());
    const auto seriesList = chart->series();

    QString ID = combo->objectName();

    int index_to_change_linestyle = -1;
    for (int i = 0; i < List_Trace_LineStyle.size(); i++) {
        if (List_Trace_LineStyle.at(i)->objectName() == ID) {
            index_to_change_linestyle = i;
            break;
        }
    }

    QLabel* label = List_TraceNames.at(index_to_change_linestyle);
    QString trace_name = label->text();

    for (QAbstractSeries *s : seriesList) {
        QLineSeries *lineSeries = qobject_cast<QLineSeries*>(s);
        if (lineSeries && lineSeries->name() == trace_name) {
            QPen pen = lineSeries->pen();
            switch (combo->currentIndex()) {
                case 0: // Solid
                    pen.setStyle(Qt::SolidLine);
                    break;
                case 1: // Dashed
                    pen.setStyle(Qt::DashLine);
                    break;
                case 2: // Dotted
                    pen.setStyle(Qt::DotLine);
                    break;
                case 3: // Dash Dot
                    pen.setStyle(Qt::DashDotLine);
                    break;
                case 4: // Dash Dot Dot Line
                    pen.setStyle(Qt::DashDotDotLine);
                    break;
            }
            lineSeries->setPen(pen);
            break;
        }
    }
}

// This is the handler that is triggered when the user hits the button to change the line width of a given trace
void Qucs_S_SPAR_Viewer::changeTraceWidth()
{
    QSpinBox *spinbox = qobject_cast<QSpinBox*>(sender());
    const auto seriesList = chart->series();

    QString ID = spinbox->objectName();

    int index_to_change_linestyle = -1;
    for (int i = 0; i < List_TraceWidth.size(); i++) {
        if (List_TraceWidth.at(i)->objectName() == ID) {
            index_to_change_linestyle = i;
            break;
        }
    }

    QLabel* label = List_TraceNames.at(index_to_change_linestyle);
    QString trace_name = label->text();

    for (QAbstractSeries *s : seriesList) {
        QLineSeries *lineSeries = qobject_cast<QLineSeries*>(s);
        if (lineSeries && lineSeries->name() == trace_name) {
            QPen pen = lineSeries->pen();
            pen.setWidth(spinbox->value());
            lineSeries->setPen(pen);
            break;
        }
    }
}

void Qucs_S_SPAR_Viewer::updatePlot()
{
  if (lock_axis == false){
    // Update axes
    update_X_axis();
    update_Y_axis();
  }

    // Trim the traces according to the new settings
    updateTraces();
    updateMarkerTable();

}

// This is the handler that updates the x-axis when the x-axis QSpinBoxes change their value
void Qucs_S_SPAR_Viewer::update_X_axis()
{
    // Get the user limits or adjust
    adjust_x_axis_div();
    double x_min = QSpinBox_x_axis_min->value();
    double x_max = QSpinBox_x_axis_max->value();
    double x_div = QComboBox_x_axis_div->currentText().toDouble();

    // Update spinbox limits
    disconnect(QSpinBox_x_axis_max, SIGNAL(valueChanged(double)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
    disconnect(QSpinBox_x_axis_min, SIGNAL(valueChanged(double)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
    QSpinBox_x_axis_min->setMaximum(x_max);
    QSpinBox_x_axis_max->setMinimum(x_min);
    connect(QSpinBox_x_axis_min, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
    connect(QSpinBox_x_axis_max, SIGNAL(valueChanged(double)), SLOT(updatePlot()));

    // Remove the axis in order to build a new one later
    if (xAxis != NULL) {
        chart->removeAxis(xAxis);
    }

    // x-axis settings
    xAxis = new QValueAxis();
    xAxis->setRange(x_min, x_max);  // Set the range of the axis
    xAxis->setTickInterval(x_div);  // Set the interval between ticks
    xAxis->setTickCount(floor((x_max-x_min)/x_div)+1);
    xAxis->setTitleText("frequency " + QCombobox_x_axis_units->currentText());

    // Add the axis to the chart
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->legend()->hide();

}

// This is the handler that updates the y-axis when the y-axis QSpinBoxes change their value
void Qucs_S_SPAR_Viewer::update_Y_axis()
{
    // y-axis
    double y_min = QSpinBox_y_axis_min->value();
    double y_max = QSpinBox_y_axis_max->value();
    double y_div = QComboBox_y_axis_div->currentText().toDouble();

    // Update spinbox limits
    disconnect(QSpinBox_y_axis_max, SIGNAL(valueChanged(double)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
    disconnect(QSpinBox_y_axis_min, SIGNAL(valueChanged(double)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
    QSpinBox_y_axis_min->setMaximum(y_max);
    QSpinBox_y_axis_max->setMinimum(y_min);
    connect(QSpinBox_y_axis_min, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
    connect(QSpinBox_y_axis_max, SIGNAL(valueChanged(double)), SLOT(updatePlot()));

    if (yAxis != NULL){
        chart->removeAxis(yAxis);
    }

    // y-axis settings
    yAxis = new QValueAxis();
    yAxis->setRange(y_min, y_max);  // Set the range of the axis
    yAxis->setTickInterval(y_div);  // Set the interval between ticks
    yAxis->setTickCount(floor((y_max-y_min)/y_div)+1);
    yAxis->setTitleText("S (dB)");

    // Add the axis to the chart
    chart->addAxis(yAxis, Qt::AlignLeft);
}

// Each time the x-axis or the y-axis settings change, the traces need to be realigned with respect to
// the new axis. Otherwise, the trace is show as it was with the initial axis settings, without any kind of rescaling
void Qucs_S_SPAR_Viewer::updateTraces()
{
    // Get the series
    QList<QAbstractSeries *> seriesList = chart->series();

    // Remove series from the chart
    for (QAbstractSeries *series : qAsConst(seriesList)) {
      chart->removeSeries(series);
    }


    double freq_scale = getFreqScale();

    // User settings
    double x_axis_min = QSpinBox_x_axis_min->value()/freq_scale;
    double x_axis_max = QSpinBox_x_axis_max->value()/freq_scale;

    double y_axis_min = QSpinBox_y_axis_min->value();
    double y_axis_max = QSpinBox_y_axis_max->value();

    // Remove marker and limit traces
    // Iterate through the series list
    QList<QAbstractSeries *> seriesToRemove;
    for (QAbstractSeries *series : qAsConst(seriesList)) {
        //qDebug() << series->name();
        if (series->name().startsWith("Mkr", Qt::CaseInsensitive)) {
            seriesToRemove.append(series);
        }
        if (series->name().startsWith("Limit", Qt::CaseInsensitive)) {
          seriesToRemove.append(series);
        }
    }
    for (QAbstractSeries *series : qAsConst(seriesToRemove)) {
        seriesList.removeOne(series);

        // If the series is added to a chart, remove it from the chart as well
        if (series->chart()) {
            series->chart()->removeSeries(series);
        }

        // Delete the series object to free memory
        delete series;
    }

    // Iterate over all the traces and, if needed:
    // 1) Find if the data in the dataset can cover the new frequency span
    // 2) If so, trim the trace according to the new limits
    // 3) If not, add extra padding

    for (QAbstractSeries *series : qAsConst(seriesList)) {
        QString trace_name = series->name();
        qreal minX_trace, maxX_trace, minY_trace, maxY_trace;

        QStringList trace_name_parts = trace_name.split('.');
        QString data_file = trace_name_parts[0];
        QString trace_file = trace_name_parts[1];

        if (trace_file.at(0) == 'S'){
            trace_file = trace_file + QString("_dB");
        }
        if (trace_file == QString("|%1|").arg(QChar(0x0394))){
            trace_file = "delta";
        }
        if (trace_file == QString("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209B))){
            trace_file = "mu";
        }
        if (trace_file == QString("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209A))){
            trace_file = "mu_p";
        }

        // Check the limits of the data in the dataset in order to see if the new settings given by the user
        // exceed the limits of the available data
        getMinMaxValues(data_file, trace_file, minX_trace, maxX_trace, minY_trace, maxY_trace);

        // Find the closest indices to the minimum and the maximum given by the user
        int minIndex = findClosestIndex(datasets[data_file]["frequency"], x_axis_min);
        int maxIndex = findClosestIndex(datasets[data_file]["frequency"], x_axis_max);

        QList<double> freq_trimmed = datasets[data_file]["frequency"].mid(minIndex, maxIndex - minIndex + 1);
        std::transform(freq_trimmed.begin(), freq_trimmed.end(), freq_trimmed.begin(),
                       [freq_scale](double value) { return value * freq_scale; });

        QList<double> data_trimmed = datasets[data_file][trace_file].mid(minIndex, maxIndex - minIndex + 1);

        // Get the series data
        QXYSeries *xySeries = qobject_cast<QXYSeries*>(series);
        xySeries->clear(); // Remove its data

        // Apply clipping if the data exceeds the lower/upper limits
        for (int i = 0; i < freq_trimmed.size(); i++) {
            double y_value = data_trimmed[i];

            // Data exceeds the upper limit
            if (y_value > y_axis_max){
                y_value = y_axis_max;
            }

            // Data exceeds the lower limit
            if (y_value < y_axis_min){
                y_value = y_axis_min;
            }

            // Add (clipped) data to the series
            xySeries->append(QPointF(freq_trimmed[i], y_value));
        }

    }

    // Add marker traces. One per trace
    for (int c = 1; c<tableMarkers->columnCount(); c++){//Traces
        QScatterSeries *marker_series = new QScatterSeries();
        marker_series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        marker_series->setMarkerSize(10);
        marker_series->setColor(Qt::black);

        for (int r = 0; r<tableMarkers->rowCount(); r++){//Marker
            QString y_val = tableMarkers->item(r,c)->text();
            QString text = tableMarkers->item(r,0)->text();
            QStringList parts = text.split(' ');
            QString freq = parts[0];
            QString freq_scale = parts[1];
            double x = freq.toDouble()/getFreqScale(freq_scale);
            x *= getFreqScale();// Normalize x with respect to the axis scale
            double y = y_val.toDouble();
            marker_series->append(x, y);
        }
        QString trace_name = tableMarkers->horizontalHeaderItem(c)->text();
        QString marker_series_name = QString("Mkr_%1").arg(trace_name);
        marker_series->setName(marker_series_name);
        seriesList.append(marker_series);
    }

    // Add the marker vertical bar
    int n_rows = tableMarkers->rowCount();
    int n_cols = tableMarkers->columnCount();
    if (n_cols > 1){
        for (int r = 0; r<n_rows; r++){//Marker
            QString text = tableMarkers->item(r,0)->text();
            QStringList parts = text.split(' ');
            QString freq = parts[0];
            QString freq_scale = parts[1];
            double x = freq.toDouble()/getFreqScale(freq_scale);
            x *= getFreqScale();// Normalize x with respect to the axis scale
            QLineSeries *verticalLine = new QLineSeries();
            verticalLine->append(x, y_axis_min);
            verticalLine->append(x, y_axis_max);
            verticalLine->setPen(QPen(Qt::black, 1, Qt::DashLine));

            QString verticalLine_name = QString("Mkr_%1").arg(r);
            verticalLine->setName(verticalLine_name);

            seriesList.append(verticalLine);
        }
    }

    // Add limits
    for (int i = 0; i < List_LimitNames.size(); i++){
      // Start frequency
      double fstart = List_Limit_Start_Freq[i]->value();
      double fstart_scale = getFreqScale(List_Limit_Start_Freq_Scale[i]->currentText());
      fstart = fstart / fstart_scale;// Hz
      fstart *= getFreqScale();// Normalize x with respect to the axis scale

      // Stop frequency
      double fstop = List_Limit_Stop_Freq[i]->value();
      double fstop_scale = getFreqScale(List_Limit_Stop_Freq_Scale[i]->currentText());
      fstop = fstop / fstop_scale;// Hz
      fstop *= getFreqScale();// Normalize x with respect to the axis scale

      // Start value
      double val_start = List_Limit_Start_Value[i]->value();

      // Stop value
      double val_stop = List_Limit_Stop_Value[i]->value();

      QLineSeries *limitLine = new QLineSeries();
      limitLine->append(fstart, val_start);
      limitLine->append(fstop, val_stop);
      limitLine->setPen(QPen(Qt::black, 2));

      QString limitLine_name = QString("Limit_%1").arg(i);
      limitLine->setName(limitLine_name);

      seriesList.append(limitLine);
    }

    // Add series again to the chart. Each series must be linked to an axis
    for (QAbstractSeries *series : qAsConst(seriesList)) {
            chart->addSeries(series);
            series->attachAxis(xAxis);
            series->attachAxis(yAxis);
        }
    chart->update();
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

}

int Qucs_S_SPAR_Viewer::findClosestIndex(const QList<double>& list, double value)
{
    return std::min_element(list.begin(), list.end(),
        [value](double a, double b) {
            return std::abs(a - value) < std::abs(b - value);
        }) - list.begin();
}

// Ensures that the frequency settings limits does not show numbers like 0.001 or 500000
void Qucs_S_SPAR_Viewer::checkFreqSettingsLimits(QString filename, double& fmin, double& fmax){
    QList<double> frequency = datasets[filename]["frequency"];

    while (true) {
        fmin = frequency.first();
        fmax = frequency.last();

        // Check frequency scale setting
        double freq_scale = getFreqScale();

        // Normalize the minimum and maximum frequencies
        fmin *= freq_scale;
        fmax *= freq_scale;

        // Exit condition
        if ((fmax > 1) && (fmax < 3000)){
            break;
        }

        if (fmax > 3000){
            // Downscale
            int index = QCombobox_x_axis_units->currentIndex();
            if (index < 3) {
                disconnect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                disconnect(QSpinBox_x_axis_max, SIGNAL(valueChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                disconnect(QSpinBox_x_axis_min, SIGNAL(valueChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                QCombobox_x_axis_units->setCurrentIndex(index+1);               
                QSpinBox_x_axis_min->setValue(fmin);
                QSpinBox_x_axis_max->setValue(fmax);
                connect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));
                connect(QSpinBox_x_axis_min, SIGNAL(valueChanged(int)), SLOT(updatePlot()));
                connect(QSpinBox_x_axis_max, SIGNAL(valueChanged(int)), SLOT(updatePlot()));

            } else{
                // It's not possible to downscale more. Break the loop
                break;
            }
        }else{
            // Upscale
            int index = QCombobox_x_axis_units->currentIndex();
            if (index > 0) {
                disconnect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                disconnect(QSpinBox_x_axis_max, SIGNAL(valueChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                disconnect(QSpinBox_x_axis_min, SIGNAL(valueChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                QCombobox_x_axis_units->setCurrentIndex(index-1);
                QSpinBox_x_axis_min->setValue(fmin);
                QSpinBox_x_axis_max->setValue(fmax);
                connect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));
                connect(QSpinBox_x_axis_min, SIGNAL(valueChanged(int)), SLOT(updatePlot()));
                connect(QSpinBox_x_axis_max, SIGNAL(valueChanged(int)), SLOT(updatePlot()));

            } else{
                // It's not possible to upscale more. Break the loop
                break;
            }
        }
    }

    return;
}


// Automatically adjust the y-axis depending on the y-axis values of the traces displayed
void Qucs_S_SPAR_Viewer::adjust_y_axis_to_trace(QString filename, QString tracename){
    qreal minX, maxX, minY, maxY;

    if (tracename.at(0) == 'S'){
        tracename = tracename + QString("_dB");
    }
    if (tracename == QString("|%1|").arg(QChar(0x0394))){
        tracename = "delta";
    }
    if (tracename == QString("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209B))){
        tracename = "mu";
    }
    if (tracename == QString("%1%2").arg(QChar(0x03BC)).arg(QChar(0x209A))){
        tracename = "mu_p";
    }


    getMinMaxValues(filename, tracename, minX, maxX, minY, maxY);

    if (maxY > this->y_max) {
        maxY = 5.0 * std::ceil(maxY / 5.0);
        this->y_max = maxY;
    }

    if (minY < this->y_min) {
        minY = 5.0 * std::floor(minY / 5.0);
        this->y_min = minY;
    }

    //Adjust the y-axis div depending on the limits
    double y_div = QComboBox_y_axis_div->currentText().toDouble();

    if ((y_div > y_max-y_min) || ((y_max-y_min)/y_div > 10) || ((y_max-y_min)/y_div < 5)){
        // No ticks or excesive ticks
        int new_index = findClosestIndex(available_y_axis_div, (y_max-y_min)/10);
        disconnect(QComboBox_y_axis_div, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot()));
        QComboBox_y_axis_div->setCurrentIndex(new_index);
        connect(QComboBox_y_axis_div, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));
    }

    disconnect(QSpinBox_y_axis_min, SIGNAL(valueChanged(double)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
    disconnect(QSpinBox_y_axis_max, SIGNAL(valueChanged(double)), this, SLOT(updatePlot()));
    QSpinBox_y_axis_min->setValue(y_min);
    QSpinBox_y_axis_max->setValue(y_max);
    connect(QSpinBox_y_axis_min, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
    connect(QSpinBox_y_axis_max, SIGNAL(valueChanged(double)), SLOT(updatePlot()));

    updatePlot();
}


// Automatically adjust the x-axis depending on the range of the traces displayed
void Qucs_S_SPAR_Viewer::adjust_x_axis_to_file(QString filename){
    QList<double> frequency = datasets[filename]["frequency"];

    double fmin = frequency.first();
    double fmax = frequency.last();

    if (fmin < this->f_min) this->f_min = fmin;
    if (fmax > this->f_max) this->f_max = fmax;


    while (true) {
        fmin = this->f_min;
        fmax = this->f_max;

        // Check frequency scale setting
        double freq_scale = getFreqScale();

        // Normalize the minimum and maximum frequencies
        fmin *= freq_scale;
        fmax *= freq_scale;

        // Exit condition
        if ((fmax > 1) && (fmax < 3000)){
            break;
        }

        if (fmax >= 3000){
            // Downscale
            int index = QCombobox_x_axis_units->currentIndex();
            if (index < 3) {
                disconnect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                QCombobox_x_axis_units->setCurrentIndex(index+1);
                connect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));
            } else{
                // It's not possible to downscale more. Break the loop
                break;
            }
        }else{
            // Upscale
            int index = QCombobox_x_axis_units->currentIndex();
            if (index > 0) {
                disconnect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot())); // Needed to avoid duplicating the call to the update function
                QCombobox_x_axis_units->setCurrentIndex(index-1);
                connect(QCombobox_x_axis_units, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));


            } else{
                // It's not possible to upscale more. Break the loop
                break;
            }
        }
    }

    // Disconnect handlers to avoid duplicating the call to the update function
    disconnect(QSpinBox_x_axis_min, SIGNAL(valueChanged(double)), this, SLOT(updatePlot()));
    disconnect(QSpinBox_x_axis_max, SIGNAL(valueChanged(double)), this, SLOT(updatePlot()));

    // Round to 1 decimal place
    fmin = round(fmin * 10.0) / 10.0;
    fmax = round(fmax * 10.0) / 10.0;
    QSpinBox_x_axis_min->setValue(fmin);
    QSpinBox_x_axis_max->setValue(fmax);
    QSpinBox_x_axis_max->setMinimum(fmin); // The upper limit cannot be lower than the lower limit
    QSpinBox_x_axis_max->setMaximum(fmax); // The lower limit cannot be higher than the higher limit

    // Update x-axis tick
    adjust_x_axis_div();

    // Connect the handlers again
    connect(QSpinBox_x_axis_min, SIGNAL(valueChanged(double)), SLOT(updatePlot()));
    connect(QSpinBox_x_axis_max, SIGNAL(valueChanged(double)), SLOT(updatePlot()));

    updatePlot();

}


void Qucs_S_SPAR_Viewer::addMarker(){

    // If there are no traces in the display, show a message and exit
    if (trace_list.size() == 0){
      QMessageBox::information(
          this,
          tr("Warning"),
          tr("The display contains no traces.") );
      return;
    }

    double f1 = QSpinBox_x_axis_min->value();
    double f2 = QSpinBox_x_axis_max->value();
    double f_marker = f1 + 0.5*(f2-f1);

    QString Freq_Marker_Scale = QCombobox_x_axis_units->currentText();

    int n_markers = List_MarkerNames.size();
    n_markers++;

    QString new_marker_name = QString("Mkr%1").arg(n_markers);
    QLabel * new_marker_label = new QLabel(new_marker_name);
    new_marker_label->setObjectName(new_marker_name);
    List_MarkerNames.append(new_marker_label);
    this->MarkersGrid->addWidget(new_marker_label, n_markers, 0);

    QString SpinBox_name = QString("Mkr_SpinBox%1").arg(n_markers);
    QDoubleSpinBox * new_marker_Spinbox = new QDoubleSpinBox();
    new_marker_Spinbox->setObjectName(SpinBox_name);
    new_marker_Spinbox->setMaximum(QSpinBox_x_axis_max->minimum());
    new_marker_Spinbox->setMaximum(QSpinBox_x_axis_max->maximum());
    new_marker_Spinbox->setValue(f_marker);
    connect(new_marker_Spinbox, SIGNAL(valueChanged(double)), SLOT(updateMarkerTable()));
    List_MarkerFreq.append(new_marker_Spinbox);
    this->MarkersGrid->addWidget(new_marker_Spinbox, n_markers, 1);

    QString Combobox_name = QString("Mkr_ComboBox%1").arg(n_markers);
    QComboBox * new_marker_Combo = new QComboBox();
    new_marker_Combo->setObjectName(Combobox_name);
    new_marker_Combo->addItem("Hz");
    new_marker_Combo->addItem("kHz");
    new_marker_Combo->addItem("MHz");
    new_marker_Combo->addItem("GHz");
    new_marker_Combo->setCurrentIndex(QCombobox_x_axis_units->currentIndex());
    connect(new_marker_Combo, SIGNAL(currentIndexChanged(int)), SLOT(changeMarkerLimits()));
    List_MarkerScale.append(new_marker_Combo);
    this->MarkersGrid->addWidget(new_marker_Combo, n_markers, 2);

    // Remove button
    QString DeleteButton_name = QString("Mkr_Delete_Btn%1").arg(n_markers);
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
    List_Button_DeleteMarker.append(new_marker_removebutton);
    this->MarkersGrid->addWidget(new_marker_removebutton, n_markers, 3, Qt::AlignCenter);

    // Add new entry to the table
    tableMarkers->setRowCount(n_markers);
    QString new_freq = QString("%1 ").arg(QString::number(f_marker, 'f', 2)) + Freq_Marker_Scale;
    QTableWidgetItem *newfreq = new QTableWidgetItem(new_freq);
    tableMarkers->setItem(n_markers-1, 0, newfreq);

    changeMarkerLimits(Combobox_name);

}


void Qucs_S_SPAR_Viewer::updateMarkerTable(){

    //If there are no markers, remove the entries and return
    int n_markers = List_MarkerNames.size();
    if (n_markers == 0){
        tableMarkers->clear();
        tableMarkers->setColumnCount(0);
        tableMarkers->setRowCount(0);
        updateTraces();
        return;
    }

    //Ensure that the size of the table is correct
    QList<QAbstractSeries *> seriesList = chart->series();

    // Update marker header
    QStringList headers;
    headers.clear();
    headers.append("freq");
    for (QAbstractSeries *series : qAsConst(seriesList)) {
        QString series_name = series->name();
        if (series_name.startsWith("Mkr", Qt::CaseSensitive)){
            //Markers are traces in the QChart, but they cannot be added as markers again!
            continue;
        }
        headers.append(series_name);
    }

    tableMarkers->setColumnCount(headers.size());// The first row is for the frequency
    tableMarkers->setRowCount(n_markers);
    tableMarkers->setHorizontalHeaderLabels(headers);

    QPointF P;
    qreal targetX;
    QString new_val;
    QString freq_marker;
    // Update each marker
    // Columns are traces. Rows are markers
    for (int c = 0; c<tableMarkers->columnCount(); c++){//Traces
        for (int r = 0; r<tableMarkers->rowCount(); r++){//Marker
            freq_marker = QString("%1 ").arg(QString::number(List_MarkerFreq[r]->value(), 'f', 1)) + List_MarkerScale[r]->currentText();

            if (c==0){
                // First column
                QTableWidgetItem *new_item = new QTableWidgetItem(freq_marker);
                tableMarkers->setItem(r, c, new_item);
                continue;
            }
            targetX = getFreqFromText(freq_marker);
            // Look into dataset, traces may be clipped.
            // It is important to grab the data from the dataset, not from the displayed trace.
            QString trace_name = seriesList[c-1]->name();
            QStringList parts = trace_name.split(".");
            QString file = parts[0];
            QString trace = parts[1];
            if (trace.at(0) == "S"){
              trace.append("_dB");
            }
            P = findClosestPoint(datasets[file]["frequency"], datasets[file][trace], targetX);
            new_val = QString("%1").arg(QString::number(P.y(), 'f', 2));
            QTableWidgetItem *new_item = new QTableWidgetItem(new_val);
            tableMarkers->setItem(r, c, new_item);
        }
    }

    updateTraces();//The markers need to be updated in the chart
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
    int index_to_delete = -1;
    for (int i = 0; i < List_Button_DeleteMarker.size(); i++) {
        if (List_Button_DeleteMarker.at(i)->objectName() == ID) {
            index_to_delete = i;
            break;
        }
    }
    removeMarker(index_to_delete);
}


void Qucs_S_SPAR_Viewer::removeMarker(int index_to_delete)
{
    // Delete the label
    QLabel* labelToRemove = List_MarkerNames.at(index_to_delete);
    MarkersGrid->removeWidget(labelToRemove);
    List_MarkerNames.removeAt(index_to_delete);
    delete labelToRemove;

    // Delete the SpinBox
    QDoubleSpinBox * SpinBoxToRemove = List_MarkerFreq.at(index_to_delete);
    MarkersGrid->removeWidget(SpinBoxToRemove);
    List_MarkerFreq.removeAt(index_to_delete);
    delete SpinBoxToRemove;

    // Delete the linestyle combo
    QComboBox* ComboToRemove = List_MarkerScale.at(index_to_delete);
    MarkersGrid->removeWidget(ComboToRemove);
    List_MarkerScale.removeAt(index_to_delete);
    delete ComboToRemove;

    // Delete the "delete" button
    QToolButton* ButtonToRemove = List_Button_DeleteMarker.at(index_to_delete);
    MarkersGrid->removeWidget(ButtonToRemove);
    List_Button_DeleteMarker.removeAt(index_to_delete);
    delete ButtonToRemove;

    updateMarkerTable();
}

void Qucs_S_SPAR_Viewer::removeAllMarkers()
{
    int n_markers = List_MarkerNames.size();
    for (int i = 0; i < n_markers; i++) {
        removeMarker(n_markers-i-1);
    }
}

// This function is called when the user wants to remove a limit from the plot
void Qucs_S_SPAR_Viewer::removeLimit()
{
  QString ID = qobject_cast<QToolButton*>(sender())->objectName();
  //qDebug() << "Clicked button:" << ID;

         //Find the index of the button to remove
  int index_to_delete = -1;
  for (int i = 0; i < List_Button_Delete_Limit.size(); i++) {
    if (List_Button_Delete_Limit.at(i)->objectName() == ID) {
      index_to_delete = i;
      break;
    }
  }
  removeLimit(index_to_delete);
}

void Qucs_S_SPAR_Viewer::removeLimit(int index_to_delete)
{
  // Delete the label
  QLabel* labelToRemove = List_LimitNames.at(index_to_delete);
  LimitsGrid->removeWidget(labelToRemove);
  List_LimitNames.removeAt(index_to_delete);
  delete labelToRemove;

  // Delete the fstart SpinBox
  QDoubleSpinBox * SpinBox_fstart_ToRemove = List_Limit_Start_Freq.at(index_to_delete);
  LimitsGrid->removeWidget(SpinBox_fstart_ToRemove);
  List_Limit_Start_Freq.removeAt(index_to_delete);
  delete SpinBox_fstart_ToRemove;

  // Delete the fstop SpinBox
  QDoubleSpinBox * SpinBox_fstop_ToRemove = List_Limit_Stop_Freq.at(index_to_delete);
  LimitsGrid->removeWidget(SpinBox_fstop_ToRemove);
  List_Limit_Stop_Freq.removeAt(index_to_delete);
  delete SpinBox_fstop_ToRemove;

  // Delete the start value SpinBox
  QDoubleSpinBox * SpinBox_val_start_ToRemove = List_Limit_Start_Value.at(index_to_delete);
  LimitsGrid->removeWidget(SpinBox_val_start_ToRemove);
  List_Limit_Start_Value.removeAt(index_to_delete);
  delete SpinBox_val_start_ToRemove;

  // Delete the stop value SpinBox
  QDoubleSpinBox * SpinBox_val_stop_ToRemove = List_Limit_Stop_Value.at(index_to_delete);
  LimitsGrid->removeWidget(SpinBox_val_stop_ToRemove);
  List_Limit_Stop_Value.removeAt(index_to_delete);
  delete SpinBox_val_stop_ToRemove;

  //Delete frequency scale combo. fstart
  QComboBox* Combo_fstart_ToRemove = List_Limit_Start_Freq_Scale.at(index_to_delete);
  LimitsGrid->removeWidget(Combo_fstart_ToRemove);
  List_Limit_Start_Freq_Scale.removeAt(index_to_delete);
  delete Combo_fstart_ToRemove;

  //Delete frequency scale combo. fstop
  QComboBox* Combo_fstop_ToRemove = List_Limit_Stop_Freq_Scale.at(index_to_delete);
  LimitsGrid->removeWidget(Combo_fstop_ToRemove);
  List_Limit_Stop_Freq_Scale.removeAt(index_to_delete);
  delete Combo_fstop_ToRemove;

  // Delete the "delete" button
  QToolButton* ButtonToRemove = List_Button_Delete_Limit.at(index_to_delete);
  LimitsGrid->removeWidget(ButtonToRemove);
  List_Button_Delete_Limit.removeAt(index_to_delete);
  delete ButtonToRemove;

  // Delete the "coupled" button
  QPushButton* ButtonCoupledToRemove = List_Couple_Value.at(index_to_delete);
  LimitsGrid->removeWidget(ButtonCoupledToRemove);
  List_Couple_Value.removeAt(index_to_delete);
  delete ButtonCoupledToRemove;

  updateTraces();
}

void Qucs_S_SPAR_Viewer::removeAllLimits()
{
  int n_limits = List_LimitNames.size();
  for (int i = 0; i < n_limits; i++) {
    removeLimit(n_limits-i-1);
  }
}

void Qucs_S_SPAR_Viewer::changeFreqUnits()
{
    // Adjust x-axis settings maximum depending on the units combo
    double freq_scale = getFreqScale();
    double fmax = this->f_max*freq_scale;
    double fmin = this->f_min*freq_scale;

    // Update fmax Spinbox
    QSpinBox_x_axis_max->setMaximum(fmax);
    QSpinBox_x_axis_max->setValue(fmax);
    if (fmax > 1000){
        // Step 1
        QSpinBox_x_axis_max->setSingleStep(1);
    } else {
        if (fmax > 100){
            // Step 0.1
            QSpinBox_x_axis_max->setSingleStep(0.1);
        } else {
            // Step 0.01
            QSpinBox_x_axis_max->setSingleStep(0.01);
        }
    }

    // Update fmin Spinbox
    QSpinBox_x_axis_min->setMaximum(fmin);
    QSpinBox_x_axis_min->setValue(fmin);
    if (fmin > 1000){
        // Step 1
        QSpinBox_x_axis_min->setSingleStep(1);
    } else {
        if (fmin > 100){
            // Step 0.1
            QSpinBox_x_axis_min->setSingleStep(0.1);
        } else {
            // Step 0.01
            QSpinBox_x_axis_min->setSingleStep(0.01);
        }
    }

    // Adjust div
    adjust_x_axis_div();

    updatePlot();
}


void Qucs_S_SPAR_Viewer::adjust_x_axis_div()
{
    double x_min = QSpinBox_x_axis_min->value();
    double x_max = QSpinBox_x_axis_max->value();
    double x_div = QComboBox_x_axis_div->currentText().toDouble();

    if ((x_div > x_max-x_min) || ((x_max-x_min)/x_div > 15) || ((x_max-x_min)/x_div < 5) ){
        // No ticks or excesive ticks
        int new_index = findClosestIndex(available_x_axis_div, (x_max-x_min)/5);
        disconnect(QComboBox_x_axis_div, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot()));
        QComboBox_x_axis_div->setCurrentIndex(new_index);
        connect(QComboBox_x_axis_div, SIGNAL(currentIndexChanged(int)), SLOT(updatePlot()));

        //Update the step of the spinboxes
        double step = QComboBox_x_axis_div->currentText().toDouble()/10;
        QSpinBox_x_axis_min->setSingleStep(step);
        QSpinBox_x_axis_max->setSingleStep(step);
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
    for (int i = 0; i < List_MarkerScale.size(); i++) {
        if (List_MarkerScale.at(i)->objectName() == ID) {
            index = i;
            break;
        }
    }

    // The lower and upper limits are given by the axis settings
    double f_upper = QSpinBox_x_axis_max->value();
    double f_lower = QSpinBox_x_axis_min->value();
    double f_scale = getFreqScale();

    f_upper /=f_scale;
    f_lower /=f_scale;

    // Now we have to normalize this with respect to the marker's combo
    QString new_scale = List_MarkerScale.at(index)->currentText();
    double f_scale_combo = getFreqScale(new_scale);
    f_upper *= f_scale_combo;
    f_lower *= f_scale_combo;

    List_MarkerFreq.at(index)->setMinimum(f_lower);
    List_MarkerFreq.at(index)->setMaximum(f_upper);

    // Update minimum step
    double diff = f_upper - f_lower;
    if (diff < 1){
        List_MarkerFreq.at(index)->setSingleStep(0.01);
    }else{
        if (diff < 10){
            List_MarkerFreq.at(index)->setSingleStep(0.1);
        }else{
            if (diff < 100){
                List_MarkerFreq.at(index)->setSingleStep(1);
            }else{
                List_MarkerFreq.at(index)->setSingleStep(10);
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
        addFiles(fileList);
    }
}


void Qucs_S_SPAR_Viewer::lock_unlock_axis_settings()
{
  if (lock_axis == true){
    lock_axis = false;
    Lock_axis_settings_Button->setText("Lock Axes");
    //Frozen axes inputs
    QSpinBox_x_axis_min->setEnabled(true);
    QSpinBox_x_axis_max->setEnabled(true);
    QComboBox_x_axis_div->setEnabled(true);
    QCombobox_x_axis_units->setEnabled(true);
    QSpinBox_y_axis_min->setEnabled(true);
    QSpinBox_y_axis_max->setEnabled(true);
    QComboBox_y_axis_div->setEnabled(true);
  }
  else{
    lock_axis = true;
    Lock_axis_settings_Button->setText("Unlock Axes");
    //Unfrozen axes inputs
    QSpinBox_x_axis_min->setDisabled(true);
    QSpinBox_x_axis_max->setDisabled(true);
    QComboBox_x_axis_div->setDisabled(true);
    QCombobox_x_axis_units->setDisabled(true);
    QSpinBox_y_axis_min->setDisabled(true);
    QSpinBox_y_axis_max->setDisabled(true);
    QComboBox_y_axis_div->setDisabled(true);
  }
}


void Qucs_S_SPAR_Viewer::addLimit()
{
  // If there are no traces in the display, show a message and exit
  if (trace_list.size() == 0){
    QMessageBox::information(
        this,
        tr("Warning"),
        tr("The display contains no traces.") );
    return;
  }

  double f1 = QSpinBox_x_axis_min->value();
  double f2 = QSpinBox_x_axis_max->value();
  double f_limit1 = f1 + 0.25*(f2-f1);
  double f_limit2 = f1 + 0.75*(f2-f1);

  double y1 = QSpinBox_y_axis_min->value();
  double y2 = QSpinBox_y_axis_max->value();
  double y_lim = y1 + (y2-y1)/2;

  int n_limits = List_LimitNames.size();
  n_limits++;
  int limit_index = 3*n_limits-2;

  QString tooltip_message;

  QString new_limit_name = QString("Limit %1").arg(n_limits);
  QLabel * new_limit_label = new QLabel(new_limit_name);
  new_limit_label->setObjectName(new_limit_name);
  List_LimitNames.append(new_limit_label);
  this->LimitsGrid->addWidget(new_limit_label, limit_index, 0);

  QString SpinBox_fstart_name = QString("Lmt_Freq_Start_SpinBox%1").arg(n_limits);
  QDoubleSpinBox * new_limit_fstart_Spinbox = new QDoubleSpinBox();
  new_limit_fstart_Spinbox->setObjectName(SpinBox_fstart_name);
  new_limit_fstart_Spinbox->setMaximum(QSpinBox_x_axis_max->minimum());
  new_limit_fstart_Spinbox->setMaximum(QSpinBox_x_axis_max->maximum());
  new_limit_fstart_Spinbox->setValue(f_limit1);
  connect(new_limit_fstart_Spinbox, SIGNAL(valueChanged(double)), SLOT(updateTraces()));
  List_Limit_Start_Freq.append(new_limit_fstart_Spinbox);
  this->LimitsGrid->addWidget(new_limit_fstart_Spinbox, limit_index, 1);

  QString Combobox_start_name = QString("Lmt_Start_ComboBox%1").arg(n_limits);
  QComboBox * new_start_limit_Combo = new QComboBox();
  new_start_limit_Combo->setObjectName(Combobox_start_name);
  new_start_limit_Combo->addItem("Hz");
  new_start_limit_Combo->addItem("kHz");
  new_start_limit_Combo->addItem("MHz");
  new_start_limit_Combo->addItem("GHz");
  new_start_limit_Combo->setCurrentIndex(QCombobox_x_axis_units->currentIndex());
  connect(new_start_limit_Combo, SIGNAL(currentIndexChanged(int)), SLOT(updateTraces()));
  List_Limit_Start_Freq_Scale.append(new_start_limit_Combo);
  this->LimitsGrid->addWidget(new_start_limit_Combo, limit_index, 2);

  QString SpinBox_fstop_name = QString("Lmt_Freq_Stop_SpinBox%1").arg(n_limits);
  QDoubleSpinBox * new_limit_fstop_Spinbox = new QDoubleSpinBox();
  new_limit_fstop_Spinbox->setObjectName(SpinBox_fstop_name);
  new_limit_fstop_Spinbox->setMaximum(QSpinBox_x_axis_max->minimum());
  new_limit_fstop_Spinbox->setMaximum(QSpinBox_x_axis_max->maximum());
  new_limit_fstop_Spinbox->setValue(f_limit2);
  connect(new_limit_fstop_Spinbox, SIGNAL(valueChanged(double)), SLOT(updateTraces()));
  List_Limit_Stop_Freq.append(new_limit_fstop_Spinbox);
  this->LimitsGrid->addWidget(new_limit_fstop_Spinbox, limit_index, 3);

  QString Combobox_stop_name = QString("Lmt_Stop_ComboBox%1").arg(n_limits);
  QComboBox * new_stop_limit_Combo = new QComboBox();
  new_stop_limit_Combo->setObjectName(Combobox_stop_name);
  new_stop_limit_Combo->addItem("Hz");
  new_stop_limit_Combo->addItem("kHz");
  new_stop_limit_Combo->addItem("MHz");
  new_stop_limit_Combo->addItem("GHz");
  new_stop_limit_Combo->setCurrentIndex(QCombobox_x_axis_units->currentIndex());
  connect(new_stop_limit_Combo, SIGNAL(currentIndexChanged(int)), SLOT(updateTraces()));
  List_Limit_Stop_Freq_Scale.append(new_stop_limit_Combo);
  this->LimitsGrid->addWidget(new_stop_limit_Combo, limit_index, 4);

  QString SpinBox_val_start_name = QString("Lmt_Val_Start_SpinBox%1").arg(n_limits);
  QDoubleSpinBox * new_limit_val_start_Spinbox = new QDoubleSpinBox();
  new_limit_val_start_Spinbox->setObjectName(SpinBox_val_start_name);
  new_limit_val_start_Spinbox->setMaximum(QSpinBox_y_axis_max->minimum());
  new_limit_val_start_Spinbox->setMaximum(QSpinBox_y_axis_max->maximum());
  new_limit_val_start_Spinbox->setValue(y_lim);
  connect(new_limit_val_start_Spinbox, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  List_Limit_Start_Value.append(new_limit_val_start_Spinbox);
  this->LimitsGrid->addWidget(new_limit_val_start_Spinbox, limit_index+1, 1);

  QString SpinBox_val_stop_name = QString("Lmt_Val_Stop_SpinBox%1").arg(n_limits);
  QDoubleSpinBox * new_limit_val_stop_Spinbox = new QDoubleSpinBox();
  new_limit_val_stop_Spinbox->setObjectName(SpinBox_val_stop_name);
  new_limit_val_stop_Spinbox->setMaximum(QSpinBox_y_axis_max->minimum());
  new_limit_val_stop_Spinbox->setMaximum(QSpinBox_y_axis_max->maximum());
  new_limit_val_stop_Spinbox->setValue(y_lim);
  connect(new_limit_val_stop_Spinbox, SIGNAL(valueChanged(double)), SLOT(updateLimits()));
  List_Limit_Stop_Value.append(new_limit_val_stop_Spinbox);
  this->LimitsGrid->addWidget(new_limit_val_stop_Spinbox, limit_index+1, 3);

  // Remove button
  QString DeleteButton_name = QString("Lmt_Delete_Btn%1").arg(n_limits);
  QToolButton * new_limit_removebutton = new QToolButton();
  new_limit_removebutton->setObjectName(DeleteButton_name);
  tooltip_message = QString("Remove this limit");
  new_limit_removebutton->setToolTip(tooltip_message);
  QIcon icon(":/bitmaps/trash.png"); // Use a resource path or a relative path
  new_limit_removebutton->setIcon(icon);
  new_limit_removebutton->setStyleSheet(R"(
            QToolButton {
                background-color: #FF0000;
                color: white;
                border-radius: 20px;
            }
        )");
  connect(new_limit_removebutton, SIGNAL(clicked()), SLOT(removeLimit()));
  List_Button_Delete_Limit.append(new_limit_removebutton);
  this->LimitsGrid->addWidget(new_limit_removebutton, limit_index, 5, Qt::AlignCenter);

  // Coupled spinbox value
  QString CoupleButton_name = QString("Lmt_Couple_Btn%1").arg(n_limits);
  QPushButton * new_limit_CoupleButton = new QPushButton("<--->");
  new_limit_CoupleButton->setObjectName(CoupleButton_name);
  new_limit_CoupleButton->setCheckable(true);// Toggle button. It lets coupled and uncouple the value spinboxes
  tooltip_message = QString("Couple start and stop values");
  new_limit_CoupleButton->setToolTip(tooltip_message);
  connect(new_limit_CoupleButton, SIGNAL(clicked(bool)), SLOT(coupleSpinBoxes()));
  List_Couple_Value.append(new_limit_CoupleButton);
  this->LimitsGrid->addWidget(new_limit_CoupleButton, limit_index+1, 2);



  QString Separator_name = QString("Lmt_Separator%1").arg(n_limits);
  QFrame * new_Separator = new QFrame();
  new_Separator->setObjectName(Separator_name);
  new_Separator->setFrameShape(QFrame::HLine);
  new_Separator->setFrameShadow(QFrame::Sunken);
  List_Separators.append(new_Separator);
  this->LimitsGrid->addWidget(new_Separator, limit_index+2, 0, 1, 6);

  updateTraces();

}

void Qucs_S_SPAR_Viewer::coupleSpinBoxes(){

  QPushButton* button = qobject_cast<QPushButton*>(sender());
  // Get the button ID, from it we can get the index and then lock the upper limit spinbox
  QString name_button = button->objectName();
  int index = name_button.remove("Lmt_Couple_Btn").toInt();
  QDoubleSpinBox * upper_limit_spinbox = List_Limit_Stop_Value.at(index-1);

  if (button->text() == "<--->"){
    button->setText("<-X->");
    QString tooltip_message = QString("Uncouple start and stop values");
    button->setToolTip(tooltip_message);
    QDoubleSpinBox * lower_limit_spinbox = List_Limit_Start_Value.at(index-1);
    upper_limit_spinbox->setValue(lower_limit_spinbox->value());
    upper_limit_spinbox->setDisabled(true);
  }else{
    button->setText("<--->");
    upper_limit_spinbox->setEnabled(true);
  }
}

// This function is called when a limit widget is changed. It is needed in case some value-coupling
// button is activated
void Qucs_S_SPAR_Viewer::updateLimits()
{
  // First check if some value-coupling button is activated. If not, simply call updateTraces()
  int n_limits = List_Couple_Value.size();
  for (int i = 0; i < n_limits; i++) {
    QPushButton* button = List_Couple_Value.at(i);
    if (button->text() == "<-X->"){
      // The control is locked. Set the stop value equal to the start value
      QDoubleSpinBox* start = List_Limit_Start_Value.at(i);
      QDoubleSpinBox* stop = List_Limit_Stop_Value.at(i);
      double val_start = start->value();
      stop->setValue(val_start);
    }
  }
  updateTraces();
}
