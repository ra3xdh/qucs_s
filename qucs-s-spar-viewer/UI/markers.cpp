/// @file markers.cpp
/// @brief Implementation of the functions related to the markers management
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later
#include "qucs-s-spar-viewer.h"

void Qucs_S_SPAR_Viewer::removeMarker() {
  QString ID = qobject_cast<QToolButton *>(sender())->objectName();
  // qDebug() << "Clicked button:" << ID;

  // Find the index of the button to remove
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

void Qucs_S_SPAR_Viewer::removeMarker(const QString &markerName) {
  if (markerMap.contains(markerName)) {
    // Get the marker properties
    MarkerProperties &props = markerMap[markerName];

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

void Qucs_S_SPAR_Viewer::removeAllMarkers() {
  int n_markers = getNumberOfMarkers();
  for (int i = 0; i < n_markers; i++) {
    QString marker_to_remove = QString("Mkr%1").arg(n_markers - i);
    removeMarker(marker_to_remove);
  }
}

void Qucs_S_SPAR_Viewer::updateMarkerNames() {
  int n_markers = getNumberOfMarkers();
  for (int i = 0; i < n_markers; i++) {
    MarkerProperties mkr_props;
    QString mkr_name;

    getMarkerByPosition(i, mkr_name, mkr_props);

    QLabel *MarkerLabel = mkr_props.nameLabel;
    MarkerLabel->setText(QStringLiteral("Mkr%1").arg(i + 1));
  }
}

bool Qucs_S_SPAR_Viewer::getMarkerByPosition(int position,
                                             QString &outMarkerName,
                                             MarkerProperties &outProperties) {
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

void Qucs_S_SPAR_Viewer::updateMarkerTable() {

  // If there are no markers, remove the entries and return
  int n_markers = getNumberOfMarkers();
  if (n_markers == 0) {
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
  QStringList header_Magnitude_Phase, header_Smith, header_Polar,
      header_PortImpedance, header_Stability, header_VSWR, header_GroupDelay;
  header_Magnitude_Phase.clear();
  header_Magnitude_Phase.append("freq");

  header_Smith = header_Magnitude_Phase;
  header_Polar = header_Magnitude_Phase;
  header_PortImpedance = header_Magnitude_Phase;
  header_Stability = header_Magnitude_Phase;
  header_VSWR = header_Magnitude_Phase;
  header_GroupDelay = header_Magnitude_Phase;

  // Build headers

  QStringList traces =
      traceMap[DisplayMode::Magnitude_dB]
          .keys(); // Traces displayed in the magnitude / phase plot
  header_Magnitude_Phase.append(traces);

  traces = traceMap[DisplayMode::Smith]
               .keys(); // Traces displayed in the Smith Chart
  header_Smith.append(traces);

  traces = traceMap[DisplayMode::Polar]
               .keys(); // Traces displayed in the Polar Chart
  header_Polar.append(traces);

  traces = traceMap[DisplayMode::PortImpedance]
               .keys(); // Traces displayed in the Port Impedance Chart
  header_PortImpedance.append(traces);

  traces = traceMap[DisplayMode::Stability]
               .keys(); // Traces displayed in the Stability Chart
  header_Stability.append(traces);

  traces =
      traceMap[DisplayMode::VSWR].keys(); // Traces displayed in the VSWR Chart
  header_VSWR.append(traces);

  traces = traceMap[DisplayMode::GroupDelay]
               .keys(); // Traces displayed in the Group Delay Chart
  header_GroupDelay.append(traces);

  // Update marker data
  updateMarkerData(*tableMarkers_Magnitude_Phase, DisplayMode::Magnitude_dB,
                   header_Magnitude_Phase); // Magnitude and phase table
  updateMarkerData(*tableMarkers_Smith, DisplayMode::Smith,
                   header_Smith); // Smith Chart table
  updateMarkerData(*tableMarkers_Polar, DisplayMode::Polar,
                   header_Polar); // Polar Chart table
  updateMarkerData(*tableMarkers_PortImpedance, DisplayMode::PortImpedance,
                   header_PortImpedance); // Port impedance Chart table
  updateMarkerData(*tableMarkers_Stability, DisplayMode::Stability,
                   header_Stability); // Port impedance Chart table
  updateMarkerData(*tableMarkers_VSWR, DisplayMode::VSWR,
                   header_VSWR); // Port impedance Chart table
  updateMarkerData(*tableMarkers_GroupDelay, DisplayMode::GroupDelay,
                   header_GroupDelay); // Group Delay Chart table

  // Update markers
  QStringList marker_list = markerMap.keys();
  for (const QString &str : std::as_const(marker_list)) {
    double marker_freq = getMarkerFreq(str);
    smithChart->updateMarkerFrequency(
        str, marker_freq); // Update Smith Chart widget markers
    polarChart->updateMarkerFrequency(
        str, marker_freq); // Update Polar Chart widget markers
    Magnitude_PhaseChart->updateMarkerFrequency(
        str, marker_freq); // Update magnitude / phase widget markers
    impedanceChart->updateMarkerFrequency(
        str, marker_freq); // Update port impedance widget markers
    stabilityChart->updateMarkerFrequency(
        str, marker_freq); // Update stability widget markers
    VSWRChart->updateMarkerFrequency(str,
                                     marker_freq); // Update VSWR widget markers
    GroupDelayChart->updateMarkerFrequency(
        str, marker_freq); // Update Group Delay Chart
  }
}

void Qucs_S_SPAR_Viewer::updateMarkerData(QTableWidget &table, DisplayMode mode,
                                          QStringList header) {

  QPointF P;
  qreal targetX;
  QString new_val;
  QString freq_marker;

  int n_markers = getNumberOfMarkers();
  int n_traces = header.size();
  table.setColumnCount(n_traces);
  table.setRowCount(n_markers);
  table.setHorizontalHeaderLabels(header);

  for (int c = 0; c < n_traces; c++) {    // Traces
    for (int r = 0; r < n_markers; r++) { // Marker
      QString markerName;
      MarkerProperties mkr_props;
      getMarkerByPosition(r, markerName,
                          mkr_props); // Get the whole marker given the position

      // Compose the marker text
      freq_marker = QStringLiteral("%1 ").arg(QString::number(
                        mkr_props.freqSpinBox->value(), 'f', 1)) +
                    mkr_props.scaleComboBox->currentText();

      if (c == 0) {
        // First column
        QTableWidgetItem *new_item = new QTableWidgetItem(freq_marker);
        table.setItem(r, c, new_item);
        continue;
      }
      targetX = getFreqFromText(freq_marker);

      QString trace_name = header.at(c);

      // Look into dataset for the trace data
      QStringList parts = {trace_name.section('.', 0, -2),
                           trace_name.section('.', -1)};
      QString file = parts[0];
      QString trace = parts[1];

      // Find data on the dataset
      if (mode == DisplayMode::Smith) {
        // Get R + j X
        QString sxx_re = trace;
        QString sxx_im = trace;

        sxx_re.replace("Smith", "re");
        sxx_im.replace("Smith", "im");

        QPointF sij_real = findClosestPoint(datasets[file]["frequency"],
                                            datasets[file][sxx_re], targetX);
        QPointF sij_imag = findClosestPoint(datasets[file]["frequency"],
                                            datasets[file][sxx_im], targetX);
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
          new_val = QStringLiteral("Z=%1-j%2 Ω\nSWR = %3")
                        .arg(QString::number(Z.real(), 'f', 1),
                             QString::number(Z.imag(), 'f', 1),
                             QString::number(SWR, 'f', 2));
        } else {
          if (imag_part > 0) {
            new_val = QStringLiteral("Z=%1+j%2 Ω\nSWR = %3")
                          .arg(QString::number(Z.real(), 'f', 1),
                               QString::number(Z.imag(), 'f', 1),
                               QString::number(SWR, 'f', 2));
          } else {
            // Z is pure real
            new_val = QStringLiteral("Z=%1 Ω\nSWR = %3")
                          .arg(QString::number(Z.real(), 'f', 1),
                               QString::number(SWR, 'f', 2));
          }
        }
      } else {
        if (mode == DisplayMode::Polar) {
          QString sxx_re = trace;
          QString sxx_im = trace;

          sxx_re.append("_re");
          sxx_im.append("_im");

          QPointF sij_real = findClosestPoint(datasets[file]["frequency"],
                                              datasets[file][sxx_re], targetX);
          QPointF sij_imag = findClosestPoint(datasets[file]["frequency"],
                                              datasets[file][sxx_im], targetX);

          double S_real = sij_real.y();
          double S_imag = sij_imag.y();

          std::complex<double> S(S_real, S_imag);

          double radius = std::abs(S);
          double angle = std::arg(S) * 180.0 / M_PI;
          if (angle < 0) {
            angle += 360;
          }

          new_val = QStringLiteral("%1∠%2").arg(QString::number(radius, 'f', 2),
                                                QString::number(angle, 'f', 1));
        } else {
          // Go directly to the dataset for data
          P = findClosestPoint(datasets[file]["frequency"],
                               datasets[file][trace], targetX);
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

double Qucs_S_SPAR_Viewer::getMarkerFreq(QString markerName) {
  // Check if marker exists
  if (!markerMap.contains(markerName)) {
    qWarning() << "Marker" << markerName << "not found!";
    return 0.0;
  }

  // Get the marker properties
  const MarkerProperties &props = markerMap[markerName];

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

void Qucs_S_SPAR_Viewer::addMarker(double freq, QString Freq_Marker_Scale) {

  // If there are no traces in the display, show a message and exit
  if (traceMap.size() == 0) {
    QMessageBox::information(this, tr("Warning"),
                             tr("The display contains no traces."));
    return;
  }

  double f_marker;
  if (freq == -1) {
    // There's no specific frequency argument, then pick the middle point
    double f1 = Magnitude_PhaseChart->getXmin();
    double f2 = Magnitude_PhaseChart->getXmax();
    f_marker = f1 + 0.5 * (f2 - f1);
  } else {
    f_marker = freq / getFreqScale(Freq_Marker_Scale);
    f_marker *= Magnitude_PhaseChart
                    ->getXscale(); // Scale according to the x-axis units
  }

  int n_markers = getNumberOfMarkers();
  n_markers++;

  MarkerProperties props; // Struct to hold Marker widgets

  QString new_marker_name = QStringLiteral("Mkr%1").arg(n_markers);
  QLabel *new_marker_label = new QLabel(new_marker_name);
  new_marker_label->setObjectName(new_marker_name);
  props.nameLabel = new_marker_label;

  this->MarkersGrid->addWidget(new_marker_label, n_markers, 0);

  QString SpinBox_name = QStringLiteral("Mkr_SpinBox%1").arg(n_markers);
  QDoubleSpinBox *new_marker_Spinbox = new QDoubleSpinBox();
  new_marker_Spinbox->setObjectName(SpinBox_name);
  new_marker_Spinbox->setMinimum(Magnitude_PhaseChart->getXmin());
  new_marker_Spinbox->setMaximum(Magnitude_PhaseChart->getXmax());
  new_marker_Spinbox->setDecimals(1);
  new_marker_Spinbox->setValue(f_marker);
  connect(new_marker_Spinbox, &QDoubleSpinBox::valueChanged, this,
          &Qucs_S_SPAR_Viewer::updateMarkerTable);
  props.freqSpinBox = new_marker_Spinbox;
  this->MarkersGrid->addWidget(new_marker_Spinbox, n_markers, 1);

  QString Combobox_name = QStringLiteral("Mkr_ComboBox%1").arg(n_markers);
  QComboBox *new_marker_Combo = new QComboBox();
  new_marker_Combo->setObjectName(Combobox_name);
  new_marker_Combo->addItems(frequency_units);
  new_marker_Combo->setCurrentIndex(Magnitude_PhaseChart->getFreqIndex());
  connect(new_marker_Combo, &QComboBox::currentIndexChanged, this,
          [this](int) { Qucs_S_SPAR_Viewer::changeMarkerLimits(); });
  props.scaleComboBox = new_marker_Combo;
  this->MarkersGrid->addWidget(new_marker_Combo, n_markers, 2);

  // Remove button
  QString DeleteButton_name = QStringLiteral("Mkr_Delete_Btn%1").arg(n_markers);
  QToolButton *new_marker_removebutton = new QToolButton();
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
  QObject::connect(new_marker_removebutton, &QToolButton::clicked, this,
                   [this]() { removeMarker(); }); // calls the no‑arg overload
  props.deleteButton = new_marker_removebutton;
  this->MarkersGrid->addWidget(new_marker_removebutton, n_markers, 3,
                               Qt::AlignCenter);

  // Add marker widgets to the marker map
  markerMap[new_marker_name] = props;

  // Add new entry to the table
  QString new_freq =
      QStringLiteral("%1 ").arg(QString::number(f_marker, 'f', 2)) +
      Freq_Marker_Scale;
  QTableWidgetItem *newfreq = new QTableWidgetItem(new_freq);

  // Add table entries

  // Magnitude / phase marker
  n_markers = tableMarkers_Magnitude_Phase->rowCount() + 1;
  tableMarkers_Magnitude_Phase->setRowCount(n_markers);
  tableMarkers_Magnitude_Phase->setRowCount(n_markers);
  tableMarkers_Magnitude_Phase->setItem(n_markers - 1, 0, newfreq);

  // Smith chart marker
  n_markers = tableMarkers_Smith->rowCount() + 1;
  tableMarkers_Smith->setRowCount(n_markers);
  tableMarkers_Smith->setRowCount(n_markers);
  tableMarkers_Smith->setItem(n_markers - 1, 0, newfreq);

  n_markers = tableMarkers_Polar->rowCount() + 1;
  tableMarkers_Polar->setRowCount(n_markers);
  tableMarkers_Polar->setRowCount(n_markers);
  tableMarkers_Polar->setItem(n_markers - 1, 0, newfreq);

  n_markers = tableMarkers_PortImpedance->rowCount() + 1;
  tableMarkers_PortImpedance->setRowCount(n_markers);
  tableMarkers_PortImpedance->setRowCount(n_markers);
  tableMarkers_PortImpedance->setItem(n_markers - 1, 0, newfreq);

  n_markers = tableMarkers_Stability->rowCount() + 1;
  tableMarkers_Stability->setRowCount(n_markers);
  tableMarkers_Stability->setRowCount(n_markers);
  tableMarkers_Stability->setItem(n_markers - 1, 0, newfreq);

  n_markers = tableMarkers_VSWR->rowCount() + 1;
  tableMarkers_VSWR->setRowCount(n_markers);
  tableMarkers_VSWR->setRowCount(n_markers);
  tableMarkers_VSWR->setItem(n_markers - 1, 0, newfreq);

  n_markers = tableMarkers_GroupDelay->rowCount() + 1;
  tableMarkers_GroupDelay->setRowCount(n_markers);
  tableMarkers_GroupDelay->setRowCount(n_markers);
  tableMarkers_GroupDelay->setItem(n_markers - 1, 0, newfreq);

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
  // IMPORTANT TO NOTE: There's an issue with the Qt Charts: If the marker is
  // drawn when the dock is behind others, the position of the dot marker will
  // be wrong, For fixing that, it is needed to raise the dock before adding the
  // marker. This doesn't happen with the Smith Chart widget as it was developed
  // from scratch

  // Find which of the docks is raised to restore that at the end
  bool isDockMagPhaseRaised = !dockChart->visibleRegion().isEmpty();
  bool isDockSmithRaised = !dockSmithChart->visibleRegion().isEmpty();
  bool isDockPolarRaised = !dockPolarChart->visibleRegion().isEmpty();
  bool isDockPortImpedanceRaised =
      !dockImpedanceChart->visibleRegion().isEmpty();
  bool isDockStabilityRaised = !dockStabilityChart->visibleRegion().isEmpty();
  bool isDockVSWRRaised = !dockVSWRChart->visibleRegion().isEmpty();

  dockChart->raise();
  Magnitude_PhaseChart->addMarker(new_marker_name, f_marker,
                                  pen); // Magnitude & Phase

  smithChart->addMarker(new_marker_name, f_marker); // Smith Chart

  dockPolarChart->raise();
  polarChart->addMarker(new_marker_name, f_marker); // Polar plot

  dockImpedanceChart->raise();
  impedanceChart->addMarker(new_marker_name, f_marker,
                            pen); // Port impedance plot

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
