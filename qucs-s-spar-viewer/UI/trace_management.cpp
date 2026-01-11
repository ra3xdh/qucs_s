/// @file trace_management.cpp
/// @brief Implementation of the functions related to the trace management
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "qucs-s-spar-viewer.h"

void Qucs_S_SPAR_Viewer::removeTrace() {
  QToolButton *button = qobject_cast<QToolButton *>(sender());
  QString ID = button->objectName();
  ID.remove("Trace_RemoveButton_");

  // Find the display mode from the parent widget
  QWidget *scroll = button->parentWidget()->parentWidget()->parentWidget();
  QString scrollname = scroll->objectName();

  DisplayMode mode = DisplayMode::Magnitude_dB; // Display mode (Default: Magnitude_dB)
  if (!scrollname.compare(QString("magnitudePhaseScrollArea"))) {
    // Check if the trace is magnitude or phase
    if (ID.endsWith("_dB")) {
      mode = DisplayMode::Magnitude_dB;
    } else {
      // It's a phase trace
      mode = DisplayMode::Phase;
    }
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
  TraceProperties &props = traceMap[mode][ID];
  removeTraceByProps(mode, ID, props);
}

void Qucs_S_SPAR_Viewer::removeTraceByProps(DisplayMode mode,
                                            const QString &traceID,
                                            TraceProperties &props) {
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

  // 2) Get the row number of the widgets in the grid for filling gaps after
  // removal
  int index = targetLayout->indexOf(props.nameLabel);
  int row_to_remove, col, rowSpan, colSpan;
  targetLayout->getItemPosition(index, &row_to_remove, &col, &rowSpan,
                                &colSpan);

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

// This function is called when the user hits the button to add a trace
void Qucs_S_SPAR_Viewer::addTrace() {
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
  int num_traces = traceMap[traceInfo.displayMode]
                       .size(); // Number of traces in the display widget
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
void Qucs_S_SPAR_Viewer::addTrace(const TraceInfo &traceInfo,
                                  QColor trace_color, int trace_width,
                                  QString trace_style) {
  DisplayMode mode = traceInfo.displayMode;
  int n_trace = this->traceMap[mode].size() + 1; // Number of displayed traces

  // Magnitude and phase plots are on the same display, so it's needed count the
  // number of both types of graphs
  if (mode == DisplayMode::Magnitude_dB) {
    int n_phase_plots = this->traceMap[DisplayMode::Phase].size();
    n_trace += n_phase_plots;
  } else {
    if (mode == DisplayMode::Phase) {
      int n_magnitude_plots = this->traceMap[DisplayMode::Magnitude_dB].size();
      n_trace += n_magnitude_plots;
    }
  }

  // Get display name for the trace
  QString trace_name = traceInfo.displayName();

  // Check if the trace is already shown
  if (traceMap[mode].contains(trace_name)) {
    QMessageBox::information(this, tr("Warning"),
                             tr("This trace is already shown"));
    return;
  }

  // Get the appropriate layout based on the display mode
  QGridLayout *targetLayout;
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

  // Create UI widgets for the trace. Widgets are hold in traceMap
  // Label
  QLabel *new_trace_label = new QLabel(trace_name);
  new_trace_label->setObjectName(QStringLiteral("Trace_Name_") + trace_name);
  traceMap[mode][trace_name].nameLabel = new_trace_label;
  targetLayout->addWidget(new_trace_label, n_trace, 0);

  // Color picker
  QPushButton *new_trace_color = new QPushButton();
  new_trace_color->setObjectName(QStringLiteral("Trace_Color_") + trace_name);
  connect(new_trace_color, &QPushButton::clicked, this,
          &Qucs_S_SPAR_Viewer::changeTraceColor);

  QString styleSheet = QStringLiteral("QPushButton { background-color: %1; }")
                           .arg(trace_color.name());
  new_trace_color->setStyleSheet(styleSheet);
  new_trace_color->setAttribute(Qt::WA_TranslucentBackground);
  traceMap[mode][trace_name].colorButton = new_trace_color;
  targetLayout->addWidget(new_trace_color, n_trace, 1);

  // LineStyle
  QComboBox *new_trace_linestyle = new QComboBox();
  new_trace_linestyle->setObjectName(QStringLiteral("Trace_LineStyle_") +
                                     trace_name);
  new_trace_linestyle->addItem("Solid");
  new_trace_linestyle->addItem("- - - -");
  new_trace_linestyle->addItem("·······");
  new_trace_linestyle->addItem("-·-·-·-");
  new_trace_linestyle->addItem("-··-··-");
  int index = new_trace_linestyle->findText(trace_style);
  new_trace_linestyle->setCurrentIndex(index);
  connect(new_trace_linestyle, &QComboBox::currentIndexChanged, this,
          &Qucs_S_SPAR_Viewer::changeTraceLineStyle);

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
  connect(new_trace_width, &QSpinBox::valueChanged, this,
          &Qucs_S_SPAR_Viewer::changeTraceWidth);
  traceMap[mode][trace_name].width = new_trace_width;
  targetLayout->addWidget(new_trace_width, n_trace, 3);

  // Remove button
  QToolButton *new_trace_removebutton = new QToolButton();
  new_trace_removebutton->setObjectName(QStringLiteral("Trace_RemoveButton_") +
                                        trace_name);
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
  connect(new_trace_removebutton, &QPushButton::clicked, this,
          &Qucs_S_SPAR_Viewer::removeTrace);

  traceMap[mode][trace_name].deleteButton = new_trace_removebutton;
  targetLayout->addWidget(new_trace_removebutton, n_trace, 4);

  // Color settings
  QPen pen;
  pen.setColor(trace_color);
  pen.setStyle(pen_style);
  pen.setWidth(trace_width);

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
      // If data is not MSG or MAG, then add the "_dB" or "_ang" suffix to
      // indicate the data type
      QString dataSuffix;
      dataSuffix =
          (traceInfo.displayMode == DisplayMode::Magnitude_dB) ? "_dB" : "_ang";
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
    QString units =
        (traceInfo.displayMode == DisplayMode::Magnitude_dB) ? "dB" : "deg";
    int yaxis = (traceInfo.displayMode == DisplayMode::Magnitude_dB) ? 1 : 2;
    QString yaxis_title = (traceInfo.displayMode == DisplayMode::Magnitude_dB)
                              ? "Magnitude (dB)"
                              : "Phase (deg)";

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

    QList<double> sii_re =
        datasets[traceInfo.dataset][traceInfo.parameter + "_re"];
    QList<double> sii_im =
        datasets[traceInfo.dataset][traceInfo.parameter + "_im"];

    for (int i = 0; i < frequencies.size(); i++) {
      std::complex<double> sii(sii_re[i], sii_im[i]);
      std::complex<double> gamma = sii; // Reflection coefficient
      std::complex<double> impedance =
          Z0 * (1.0 + gamma) / (1.0 - gamma); // Convert to impedance
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
    QList<double> sij_re =
        datasets[traceInfo.dataset][traceInfo.parameter + "_re"];
    QList<double> sij_im =
        datasets[traceInfo.dataset][traceInfo.parameter + "_im"];

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
      QList<double> sij_re =
          datasets[traceInfo.dataset][traceInfo.parameter + "_re"];
      QList<double> sij_im =
          datasets[traceInfo.dataset][traceInfo.parameter + "_im"];

      // Add appropriate handling for S-parameters in natural units
      // (This part of the code wasn't fully implemented in the original)
    } else {
      // Other parameters (like Re{Zin}, Im{Zin}, etc.)
      // Calculate if needed
      if (datasets[traceInfo.dataset][traceInfo.parameter].isEmpty()) {
        calculate_Sparameter_trace(traceInfo.dataset, traceInfo.parameter);
      }

      QList<double> trace_data =
          datasets[traceInfo.dataset][traceInfo.parameter];

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

// Removes all traces, markers and limits
void Qucs_S_SPAR_Viewer::removeAll() {

  // 1) Remove limits
  removeAllLimits();

  // 2) Remove markers
  removeAllMarkers();

  // 3) Remove traces
  removeAllTraces();
}

// Remove all traces
void Qucs_S_SPAR_Viewer::removeAllTraces(){
  for (auto modeIt = traceMap.begin(); modeIt != traceMap.end(); ++modeIt) {
    DisplayMode mode = modeIt.key();
    QMap<QString, TraceProperties> &traces = modeIt.value();

    // Create a list of trace names to remove (to avoid iterator invalidation)
    QStringList traceNames = traces.keys();

    // Remove each trace
    for (const QString &traceName : std::as_const(traceNames)) {
      TraceProperties &props = traces[traceName];
      removeTraceByProps(mode, traceName, props);
    }
  }

  // Clear the trace map completely
  traceMap.clear();

  // Update all chart widgets
  Magnitude_PhaseChart->update();
  smithChart->update();
  polarChart->update();
  impedanceChart->update();
  stabilityChart->update();
  VSWRChart->update();
  GroupDelayChart->update();
}

// This is the handler that is triggered when the user hits the button to change
// the color of a given trace
void Qucs_S_SPAR_Viewer::changeTraceColor() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Select Color");
  if (color.isValid()) {
    // Do something with the selected color
    // For example, set the background color of the button
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {

      // 1) Find the display mode

      QWidget *scroll = button->parentWidget()->parentWidget()->parentWidget();
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
      QString styleSheet =
          QStringLiteral("QPushButton { background-color: %1; }")
              .arg(color.name());
      button->setStyleSheet(styleSheet);

      QString ID = button->objectName();
      ID.remove("Trace_Color_"); // Remove the preffix

      // 3) Remove trace from the layout and from the chart
      QPen currentPen(Qt::black); // Explicit constructor (to avoid warnings)
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

// This is the handler that is triggered when the user hits the button to change
// the line style of a given trace
void Qucs_S_SPAR_Viewer::changeTraceLineStyle() {
  QComboBox *combo = qobject_cast<QComboBox *>(sender());
  QString ID = combo->objectName();

  // 1) Find the display mode

  QWidget *scroll = combo->parentWidget()->parentWidget()->parentWidget();
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
  enum Qt::PenStyle PenStyle = Qt::SolidLine; // Default = solid line
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
  QPen currentPen(Qt::black); // Explicit constructor (to avoid warnings)
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

// This is the handler that is triggered when the user hits the button to change
// the line width of a given trace
void Qucs_S_SPAR_Viewer::changeTraceWidth() {
  QSpinBox *spinbox = qobject_cast<QSpinBox *>(sender());
  QString ID = spinbox->objectName();

  // 1) Find the display mode

  QWidget *scroll = spinbox->parentWidget()->parentWidget()->parentWidget();
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
  QPen currentPen(Qt::black); // Explicit constructor (to avoid warnings)
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
