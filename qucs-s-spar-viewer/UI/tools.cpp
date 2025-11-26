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

#include "qucs-s-spar-viewer.h"
void Qucs_S_SPAR_Viewer::setToolsDock() {
  // Tools dock
  dockTools = new QDockWidget("Circuit Synthesis", this);
  dockTools->setObjectName("dockTools");

  // Main container widget for the dock
  QWidget *container = new QWidget();

  // Vertical layout for the container
  QVBoxLayout *layout = new QVBoxLayout(container);
  layout->setContentsMargins(0, 0, 0,
                             0); // Optional: remove margins

  // Tab widget
  toolsTabWidget = new QTabWidget();
  FilterTool = new FilterDesignTool(this);
  MatchingTool = new MatchingNetworkDesignTool(this);
  PowerCombTool = new PowerCombiningTool(this);
  AttenuatorTool = new AttenuatorDesignTool(this);
  Netlist_Tool = new NetlistScratchPad(this);
  SimulationSetupWidget = new SimulationSetup(this);

  // Set default substrate to the tools
  MS_Subs = SimulationSetupWidget->get_MS_Substrate();
  FilterTool->set_MS_Subs(MS_Subs);

  toolsTabWidget->addTab(FilterTool, "Filter Design");
  toolsTabWidget->addTab(MatchingTool, "Matching");
  toolsTabWidget->addTab(PowerCombTool, "Power Combining");
  toolsTabWidget->addTab(AttenuatorTool, "Attenuator Design");
  toolsTabWidget->addTab(Netlist_Tool, "Netlist");
  toolsTabWidget->addTab(SimulationSetupWidget, "Settings");

  // Schematic widget
  SchematicWidget = new GraphWidget(this); // Schematic window

  // Export option. Just a line at the bottom of the schematic
  LabelExportSchematic = new QLabel("Export schematic");
  QFont labelFont = LabelExportSchematic->font();
  labelFont.setBold(true);
  LabelExportSchematic->setFont(labelFont);

  // Export formats
  ComboExportSchematic = new QComboBox();
  ComboExportSchematic->addItem("Qucs-S: Qucsator-RF");
  ComboExportSchematic->addItem("Qucs-S: NGSpice");
  ComboExportSchematic->addItem("Qucs-S: Xyce");

  QLabel *exportArrowLabel = new QLabel("→");
  exportArrowLabel->setStyleSheet(
      "font: bold 18pt 'DejaVu Sans'; color: black;");

  // Export method
  ComboExportOutputMethod = new QComboBox();
  ComboExportOutputMethod->addItem("Clipboard");
  ComboExportOutputMethod->addItem("File");

  ButtonExportSchematic = new QPushButton("Export");
  ButtonExportSchematic->setStyleSheet("QPushButton { "
                                       "background-color: rgb(254, 170, 0); "
                                       "color: white; "
                                       "border-radius: 6px; "
                                       "padding: 4px 12px; "
                                       "font-weight: bold; "
                                       "} "
                                       "QPushButton:hover { "
                                       "background-color: rgb(255, 200, 30); "
                                       "} "
                                       "QPushButton:pressed { "
                                       "background-color: rgb(230, 140, 0); "
                                       "}");

  // Layout container
  QWidget *containerExport = new QWidget();
  QHBoxLayout *layoutExport = new QHBoxLayout(containerExport);
  layoutExport->addWidget(LabelExportSchematic);
  layoutExport->addWidget(ComboExportSchematic);
  layoutExport->addWidget(exportArrowLabel);
  layoutExport->addWidget(ComboExportOutputMethod);
  layoutExport->addWidget(ButtonExportSchematic);
  layoutExport->setAlignment(Qt::AlignCenter);

  // Add widgets to layout
  layout->addWidget(toolsTabWidget);
  layout->addWidget(SchematicWidget); // This will appear *below* the tab widget
  layout->addWidget(
      containerExport); // This will appear *below* the schematic window

  // Set the layout container as the dock widget's content
  dockTools->setWidget(container);

  // Connect with tools to update the simulated traces
  connect(FilterTool, SIGNAL(updateSimulation(SchematicContent)), this,
          SLOT(updateSimulation(SchematicContent)));
  connect(MatchingTool, SIGNAL(updateSimulation(SchematicContent)), this,
          SLOT(updateSimulation(SchematicContent)));
  connect(PowerCombTool, SIGNAL(updateSimulation(SchematicContent)), this,
          SLOT(updateSimulation(SchematicContent)));
  connect(AttenuatorTool, SIGNAL(updateSimulation(SchematicContent)), this,
          SLOT(updateSimulation(SchematicContent)));
  connect(Netlist_Tool, SIGNAL(updateSimulation(SchematicContent)), this,
          SLOT(updateSimulation(SchematicContent)));
  connect(SimulationSetupWidget, SIGNAL(updateSimulation()), this,
          SLOT(updateSimulation()));
  connect(SimulationSetupWidget, SIGNAL(updateSubstrate()), this,
          SLOT(updateSubstrate()));
  connect(ButtonExportSchematic, SIGNAL(clicked(bool)), this,
          SLOT(exportSchematic()));

  // Update simulation and schematic when the user clicks on another tab
  connect(toolsTabWidget, &QTabWidget::currentChanged, this,
          &Qucs_S_SPAR_Viewer::onToolsTabChanged);
  connect(dockTools, &QDockWidget::visibilityChanged, this,
          &Qucs_S_SPAR_Viewer::callTools);
}
void Qucs_S_SPAR_Viewer::updateSimulation() {
  QString netlist = Circuit.getSParameterNetlist();
  qDebug() << "Netlist";
  qDebug() << netlist;

  SPAR_engine.setNetlist(netlist);
  double fstart = SimulationSetupWidget->getFstart();
  double fstop = SimulationSetupWidget->getFstop();
  int npoints = SimulationSetupWidget->getNpoints();

  // Pass settings to the S-parameter engine
  SPAR_engine.setFrequencySweep(fstart, fstop, npoints);
  SPAR_engine.calculateSParameterSweep();
  QMap<QString, QList<double>> data = SPAR_engine.getData();

  ///////////////////////////////////////////////////////////
  // Update the data on the Schematic Object. This is needed
  // to set the frequency sweep when exporting the schematic
  QString fstart_text = SimulationSetupWidget->getFstart_as_Text();
  QString fstop_text = SimulationSetupWidget->getFstop_as_Text();
  Circuit.setFrequencySweep(fstart_text, fstop_text, npoints);
  ///////////////////////////////////////////////////////////

  if (data.isEmpty()) {
    return;
  }

  QString dataset_name = Circuit.Name;

  // Update data
  datasets[dataset_name] = data;

  // After simulation, once the data has been updated in the datasets structure,
  // it is needed to refresh the list of available traces. This is needed
  // because in the Power Combining synthesis there are topologies with
  // different number of ports, if the "trace refresh" is not done at this
  // point, all combiners would have only a S(3x3) available to select
  updateTracesCombo();

  // Check if the dataset exists in the Combo, if not add it
  if (QCombobox_datasets->findText(dataset_name) == -1) {
    QCombobox_datasets->addItem(dataset_name);
  }

  // Check if the trace has been added
  QMap<QString, TraceProperties> rect_traces =
      traceMap[DisplayMode::Magnitude_dB];

  QStringList displayed_traces_keys = rect_traces.keys();

  QRegularExpression regex(dataset_name,
                           QRegularExpression::CaseInsensitiveOption);
  QStringList filteredList = displayed_traces_keys.filter(regex);

  if (filteredList.isEmpty()) {
    // If not Filter, check if Power Combiner to add extra traces
    if (Circuit.Type == QString("Power Combiner")) {
      TraceInfo s11_dB = {dataset_name, "S11",
                          DisplayMode::Magnitude_dB}; // Input reflection
      TraceInfo s21_dB = {dataset_name, "S21",
                          DisplayMode::Magnitude_dB}; // Insertion loss
      TraceInfo s31_dB = {dataset_name, "S31",
                          DisplayMode::Magnitude_dB}; // Insertion loss branch 2
      TraceInfo s32_dB = {dataset_name, "S32",
                          DisplayMode::Magnitude_dB}; // Isolation
      this->addTrace(s11_dB, Qt::blue, 1);
      this->addTrace(s21_dB, Qt::red, 1);
      this->addTrace(s31_dB, Qt::darkGreen, 1);
      this->addTrace(s32_dB, Qt::black, 1);
    } else {
      if (Circuit.Type == QString("Matching-1-port")) {
        // 1) Force the tools' datasets clearing. This is done when the user
        // changes the tool tab, but it need to be enforced here because the
        // matching tool have two modes: 1-port and 2-port matching Toggling
        // between these modes leads to clutter the display.
        cleanToolsDatasets(dataset_name);
        // 2) Add S11 only
        TraceInfo s11_dB = {dataset_name, "S11",
                            DisplayMode::Magnitude_dB}; // Input reflection
        this->addTrace(s11_dB, Qt::blue, 1);
      } else {
        if (Circuit.Type == QString("Matching-2-ports")) {

          // 1) Force the tools' datasets clearing. This is done when the user
          // changes the tool tab, but it need to be enforced here because the
          // matching tool have two modes: 1-port and 2-port matching Toggling
          // between these modes leads to clutter the display.
          cleanToolsDatasets(dataset_name);
          TraceInfo s11_dB = {dataset_name, "S11",
                              DisplayMode::Magnitude_dB}; // Input reflection
          TraceInfo s21_dB = {dataset_name, "S21",
                              DisplayMode::Magnitude_dB}; // Gain
          TraceInfo s22_dB = {dataset_name, "S22",
                              DisplayMode::Magnitude_dB}; // Output reflection

          // 2) Add S11, S21, and S22
          this->addTrace(s11_dB, Qt::blue, 1);
          this->addTrace(s21_dB, Qt::red, 1);
          this->addTrace(s22_dB, Qt::darkGreen, 1);

        } else {
          // Default traces (used in filter and attenuator design tools)
          TraceInfo s11_dB = {dataset_name, "S11",
                              DisplayMode::Magnitude_dB}; // Input reflection
          TraceInfo s21_dB = {dataset_name, "S21",
                              DisplayMode::Magnitude_dB}; // Insertion loss
          this->addTrace(s11_dB, Qt::blue, 1);
          this->addTrace(s21_dB, Qt::red, 1);
        }
      }
    }
  }
  updateAllPlots(dataset_name);

  updateSchematicContent();
}
void Qucs_S_SPAR_Viewer::updateSubstrate() {

  // Get the new substrate
  MS_Subs = SimulationSetupWidget->get_MS_Substrate();

  // Update the substrate in all tools
  FilterTool->set_MS_Subs(MS_Subs);

  if (Circuit.Type == QString("Filter")) {
    FilterTool->design();
  } else if (Circuit.Type == QString("Power Combiner")) {
    PowerCombTool->design();
  } else if (Circuit.Type == QString("Attenuator")) {
    AttenuatorTool->design();
  } else {
    // Matching network
    MatchingTool->design();
  }

  // Check the last circuit being synthesized and trigger synthesis
  // Trigger synthesis
}
void Qucs_S_SPAR_Viewer::updateSimulation(SchematicContent SI) {
  Circuit = SI;
  Tools_Datasets.append(SI.Name);

  QString tool_netlist = SI.getSParameterNetlist();
  QString scratchpad = Netlist_Tool->getText();

  if (tool_netlist != scratchpad) {
    Netlist_Tool->blockSignals(true);
    Netlist_Tool->setText(tool_netlist);
    Netlist_Tool->blockSignals(false);
  }

  updateSimulation();
}
void Qucs_S_SPAR_Viewer::updateSchematicContent() {
  SchematicWidget->clear(); // Remove the components in the scene

  if (!Circuit.getComponents().isEmpty()) {
    SchematicWidget->setSchematic(Circuit);
  }
}
// This function handles the event of selecting a new tab in the design tools.
// First, it's needed to remove the traces from other designer tabs (otherwise,
// it may clutter the display) Second, it calls callTools() function to trigger
// circuit synthesis
void Qucs_S_SPAR_Viewer::onToolsTabChanged(int index) {

  if (index < 4) {
    // Remove other design datasets.
    // This does not make sense if the netlist tool is selected since it can be
    // used to test values and compare with the trace of the previously selected
    // tool
    cleanToolsDatasets();
  }

  // Trigger circuit synthesis
  if (index < 5) {
    // Avoid simulate again when the "Settings" tab is selected
    callTools(true);
  }
}
// Removes the datasets generated by the tools and their traces
void Qucs_S_SPAR_Viewer::cleanToolsDatasets(const QString &excludeDataset) {
  for (const QString &ID : qAsConst(Tools_Datasets)) {
    if (excludeDataset.isEmpty() || ID != excludeDataset) {
      removeFile(ID);
    }
  }
  if (excludeDataset.isEmpty()) {
    Tools_Datasets.clear();
  } else {
    Tools_Datasets = QStringList{excludeDataset};
  }
}
// Manage the schematic exportExport schematic
void Qucs_S_SPAR_Viewer::exportSchematic() {
  int formatToExport = ComboExportSchematic->currentIndex();
  QString schematicText; // Output netlist

  switch (formatToExport) {
  case 0: // Front-end: Qucs-S, Back-end: Qucsator-RF
    schematicText =
        Circuit.exportSchematic(QString("Qucs-S"), QString("Qucsator"));
    break;
  case 1: // Front-end: Qucs-S, Back-end: NGspice
    schematicText =
        Circuit.exportSchematic(QString("Qucs-S"), QString("NGspice"));
    break;
  case 2: // Front-end: Qucs-S, Back-end: Xyce
    schematicText = Circuit.exportSchematic(QString("Qucs-S"), QString("Xyce"));
    break;
  }

  // Was the processing ok?
  if (schematicText == QString("-1")) {
    // Unsupported components were found -> Abort export
    return;
  }

  // Output method: File or clipboard
  QString outputMethod = ComboExportOutputMethod->currentText();

  if (outputMethod == "Clipboard") {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(schematicText);
    QMessageBox::information(this, "Export success",
                             "Schematic text copied to clipboard.");
  } else if (outputMethod == "File") {
    QString fileName = QFileDialog::getSaveFileName(this, "Export schematic",
                                                    QDir::homePath() + "/" +
                                                        Circuit.Type + ".sch",
                                                    "Qucs-S schematic (*.sch)");

    if (fileName.isEmpty())
      return;

    // Enforce .sch extension if user omits it
    if (!fileName.endsWith(".sch", Qt::CaseInsensitive))
      fileName += ".sch";

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QMessageBox::critical(this, "Export failed",
                            "Unable to open file for writing:\n" + fileName);
      return;
    }

    QTextStream out(&file);
    out << schematicText;
    file.close();

    QMessageBox::information(this, "Export success",
                             "Schematic successfully saved to:\n" + fileName);
  }
}
