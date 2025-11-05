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

// Load "Recent Files" list. This is called when the program starts up
void Qucs_S_SPAR_Viewer::loadRecentFiles() {
  QSettings settings;
  recentFiles = settings.value("recentFiles").value<std::vector<QString>>();
}

void Qucs_S_SPAR_Viewer::clearRecentFiles() {
  recentFiles.clear();
}

// Save "Recent Files" list. This is called when the program is about to close
void Qucs_S_SPAR_Viewer::saveRecentFiles() {
  QSettings settings;
  settings.setValue("recentFiles", QVariant::fromValue(recentFiles));
}

void Qucs_S_SPAR_Viewer::slotSave() {
  if (savepath.isEmpty()) {
    slotSaveAs();
    return;
  }
  save();
}

void Qucs_S_SPAR_Viewer::slotSaveAs() {
  if (datasets.isEmpty()) {
    // Nothing to save
    QMessageBox::information(this, tr("Error"),
                             tr("Nothing to save: No data was loaded."));
    return;
  }

  // Get the path to save
  savepath = QFileDialog::getSaveFileName(
      this, tr("Save session"), QDir::homePath() + "/ViewerSession.spar",
      tr("Qucs-S snp viewer session (*.spar);"));

  // If the user decides not to enter a path, then return.
  if (savepath.isEmpty()) {
    return;
  }
  save();
}

void Qucs_S_SPAR_Viewer::slotLoadSession() {
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open S-parameter Viewer Session"), QDir::homePath(),
      tr("Qucs-S snp viewer session (*.spar)"));

  loadSession(fileName);
}

void Qucs_S_SPAR_Viewer::loadSession(QString session_file) {
  QFile file(session_file);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Load Session"),
                         tr("Could not open file for reading."));
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
      // qDebug() << xml.name();
      if (xml.name() == QStringLiteral("settings")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                 xml.name() == QStringLiteral("settings"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QStringLiteral("geometry")) {
              restoreGeometry(
                  QByteArray::fromBase64(xml.readElementText().toLatin1()));
            } else if (xml.name() == QStringLiteral("state")) {
              restoreState(
                  QByteArray::fromBase64(xml.readElementText().toLatin1()));
            }
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("datasets")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                 xml.name() == QStringLiteral("datasets"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement &&
              xml.name() == QStringLiteral("dataset")) {
            QString datasetName = xml.attributes().value("name").toString();
            QMap<QString, QList<double>> dataset;

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == QStringLiteral("dataset"))) {
              if (xml.tokenType() == QXmlStreamReader::StartElement &&
                  xml.name() == QStringLiteral("data")) {
                QString key = xml.attributes().value("key").toString();
                QList<double> values;

                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == QStringLiteral("data"))) {
                  if (xml.tokenType() == QXmlStreamReader::StartElement &&
                      xml.name() == QStringLiteral("value")) {
                    values.append(xml.readElementText().toDouble());
                  }
                  xml.readNext();
                }

                dataset[key] = values;
              }
              xml.readNext();
            }

            datasets[datasetName] = dataset;
            QCombobox_datasets->addItem(
                datasetName); // Add dataset to the combobox

            // Add dataset to the file list
            CreateFileWidgets(datasetName);
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("traces")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                 xml.name() == QStringLiteral("traces"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement &&
              xml.name() == QStringLiteral("trace")) {
            QString traceName = xml.attributes().value("name").toString();
            traceName         = traceName.mid(
                traceName.indexOf('.') +
                1); // Remove all that comes before the dot, including the dot.
            QString dataset = xml.attributes()
                                  .value("dataset")
                                  .toString(); // Read dataset from attributes
            DisplayMode displayMode =
                (DisplayMode)xml.attributes()
                    .value("display")
                    .toInt(); // Display mode (e.g. rectangular, polar, Smith,
                              // etc.)
            QColor color(xml.attributes()
                             .value("color")
                             .toString()); // Read color from attributes
            int width = xml.attributes()
                            .value("width")
                            .toString()
                            .toInt(); // Read width from attributes
            QString style = xml.attributes()
                                .value("style")
                                .toString(); // Read style from attributes

            // Parse the traceName to determine parameter and display mode
            QString parameter;

            // Assuming the format is like "S11_dB", "S21_Smith", etc.
            if (traceName.contains("_")) {
              // Split parameter and mode
              parameter = traceName.left(traceName.indexOf('_'));
            } else {
              // If no underscore, assume the whole name is the parameter and
              // use Magnitude_dB as default
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
        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                 xml.name() == QStringLiteral("markers"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement &&
              xml.name() == QStringLiteral("marker")) {
            double freq = xml.attributes()
                              .value("frequency")
                              .toString()
                              .toDouble(); // Read frequency from attributes
            QString scale = xml.attributes()
                                .value("scale")
                                .toString(); // Read scale from attributes
            addMarker(freq, scale);
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("limits")) {
        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                 xml.name() == QStringLiteral("limits"))) {
          if (xml.tokenType() == QXmlStreamReader::StartElement &&
              xml.name() == QStringLiteral("limit")) {
            double start_freq =
                xml.attributes()
                    .value("start_freq")
                    .toString()
                    .toDouble(); // Read start_freq from attributes
            double stop_freq =
                xml.attributes()
                    .value("stop_freq")
                    .toString()
                    .toDouble(); // Read stop_freq from attributes
            double start_value =
                xml.attributes()
                    .value("start_value")
                    .toString()
                    .toDouble(); // Read start_value from attributes
            double stop_value =
                xml.attributes()
                    .value("stop_value")
                    .toString()
                    .toDouble(); // Read stop_value from attributes
            QString start_freq_scale =
                xml.attributes()
                    .value("start_freq_scale")
                    .toString(); // Read start_freq_scale from attributes
            QString stop_freq_scale =
                xml.attributes()
                    .value("stop_freq_scale")
                    .toString(); // Read stop_freq_scale from attributes
            QString axis = xml.attributes()
                               .value("axis")
                               .toString(); // Read axis from attributes
            addLimit(start_freq, start_freq_scale, stop_freq, stop_freq_scale,
                     start_value, stop_value, true);
          }
          xml.readNext();
        }
      } else if (xml.name() == QStringLiteral("notes")) {
        Notes_Widget->setPlainText(xml.readElementText());
      } else if (xml.name() == QStringLiteral("MagnitudePhaseChartSettings")) {
        loadRectangularPlotSettings(xml, Magnitude_PhaseChart,
                                    "MagnitudePhaseChartSettings");
      } else if (xml.name() == QStringLiteral("ImpedanceChartSettings")) {
        loadRectangularPlotSettings(xml, impedanceChart,
                                    "ImpedanceChartSettings");
      } else if (xml.name() == QStringLiteral("StabilityChartSettings")) {
        loadRectangularPlotSettings(xml, stabilityChart,
                                    "StabilityChartSettings");
      } else if (xml.name() == QStringLiteral("VSWRChartSettings")) {
        loadRectangularPlotSettings(xml, VSWRChart, "VSWRChartSettings");
      } else if (xml.name() == QStringLiteral("GroupDelayChartSettings")) {
        loadRectangularPlotSettings(xml, GroupDelayChart,
                                    "GroupDelayChartSettings");
      } else if (xml.name() == QStringLiteral("SmithChartSettings")) {
        loadSmithPlotSettings(xml, smithChart, "SmithChartSettings");
      } else if (xml.name() == QStringLiteral("PolarChartSettings")) {
        loadPolarPlotSettings(xml, polarChart, "PolarChartSettings");
      }
    }
  }

  if (xml.hasError()) {
    QMessageBox::warning(this, tr("Load Session"),
                         tr("Error parsing XML: ") + xml.errorString());
  }

  file.close();

  // Update UI
  updateTracesCombo();
  updateMarkerTable();
}

bool Qucs_S_SPAR_Viewer::save() {
  if (savepath.isEmpty()) {
    return false; // No save path specified
  }

  QFile file(savepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Save Session"),
                         tr("Could not open file for writing."));
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
  if (!datasets.isEmpty()) { // Check empty map
    xml.writeStartElement("datasets");
    for (const QString& datasetName : datasets.keys()) {
      if (!datasetName.isEmpty() &&
          !datasets[datasetName].isEmpty()) { // Validate data
        xml.writeStartElement("dataset");
        xml.writeAttribute("name", datasetName);

        // Save dataset data
        const QMap<QString, QList<double>>& dataset = datasets[datasetName];
        for (const QString& key : dataset.keys()) {
          xml.writeStartElement("data");
          xml.writeAttribute("key", key);
          for (double value : dataset[key]) {
            xml.writeTextElement(
                "value", QString::number(value, 'g',
                                         15)); // Consistent number formatting
          }
          xml.writeEndElement(); // data
        }

        xml.writeEndElement(); // dataset
      }
    }
    xml.writeEndElement(); // datasets
  }

  // Save traces
  if (!traceMap.isEmpty()) { // Check empty map
    xml.writeStartElement("traces");
    // Loop display mode
    for (const DisplayMode& mode : traceMap.keys()) {
      // For each display mode, get all the traces
      QMap<QString, TraceProperties>& traces = traceMap[mode];
      // Iterate through the inner QMap (traces in the current mode)
      for (const QString& traceName : traces.keys()) {
        TraceProperties props = traces[traceName];
        int dotIndex          = traceName.indexOf('.');

        QString dataset = traceName.left(dotIndex);
        QString trace   = traceName.mid(dotIndex + 1);

        if (trace.endsWith("_n.u.")) {
          trace.chop(5);
        }

        xml.writeStartElement("trace");
        xml.writeAttribute(
            "display",
            QString::number(
                (int)mode)); // The display mode is saved as an integer
        xml.writeAttribute("dataset", dataset); // Extract dataset name
        xml.writeAttribute("name", trace);
        xml.writeAttribute(
            "color",
            props.colorButton->palette().color(QPalette::Button).name());
        xml.writeAttribute("width", QString::number(props.width->value()));
        xml.writeAttribute("style", props.LineStyleComboBox->currentText());
        xml.writeEndElement(); // trace
      }
    }
    xml.writeEndElement(); // traces
  }

  // Save markers
  if (!markerMap.isEmpty()) { // Check empty map
    xml.writeStartElement("markers");
    for (const QString& markerName : markerMap.keys()) {
      MarkerProperties props = markerMap[markerName];
      xml.writeStartElement("marker");
      xml.writeAttribute("frequency",
                         QString::number(props.freqSpinBox->value()));
      xml.writeAttribute("scale", props.scaleComboBox->currentText());
      xml.writeEndElement(); // marker
    }
    xml.writeEndElement(); // markers
  }

  // Save limits
  if (!limitsMap.isEmpty()) { // Check empty map
    xml.writeStartElement("limits");
    for (const QString& limitName : limitsMap.keys()) {
      LimitProperties props = limitsMap[limitName];
      xml.writeStartElement("limit");
      xml.writeAttribute("start_freq",
                         QString::number(props.Start_Freq->value()));
      xml.writeAttribute("stop_freq",
                         QString::number(props.Stop_Freq->value()));
      xml.writeAttribute("start_value",
                         QString::number(props.Start_Value->value()));
      xml.writeAttribute("stop_value",
                         QString::number(props.Stop_Value->value()));
      xml.writeAttribute("start_freq_scale",
                         props.Start_Freq_Scale->currentText());
      xml.writeAttribute("stop_freq_scale",
                         props.Stop_Freq_Scale->currentText());
      xml.writeAttribute("axis", props.axis->currentText());
      xml.writeEndElement(); // limit
    }
    xml.writeEndElement(); // limits
  }

  // Save notes
  xml.writeStartElement("notes");
  xml.writeCDATA(Notes_Widget->toPlainText()); // Use CDATA for notes
  xml.writeEndElement();                       // notes

  // Save charts state
  saveRectangularPlotSettings(xml, Magnitude_PhaseChart,
                              "MagnitudePhaseChartSettings");
  saveRectangularPlotSettings(xml, impedanceChart, "ImpedanceChartSettings");
  saveRectangularPlotSettings(xml, stabilityChart, "StabilityChartSettings");
  saveRectangularPlotSettings(xml, VSWRChart, "VSWRChartSettings");
  saveRectangularPlotSettings(xml, GroupDelayChart, "GroupDelayChartSettings");

  saveSmithPlotSettings(xml, smithChart, "SmithChartSettings");
  savePolarPlotSettings(xml, polarChart, "PolarChartSettings");

  xml.writeEndElement(); // session
  xml.writeEndDocument();

  if (file.error() != QFile::NoError) {
    QMessageBox::warning(this, tr("Save Session"),
                         tr("Error writing file: ") + file.errorString());
    return false;
  }

  file.close();
  return true;
}

// Add session file to the recent files list
void Qucs_S_SPAR_Viewer::addRecentFile(const QString& filePath) {
  recentFiles.insert(recentFiles.begin(), filePath);
  recentFiles.erase(std::unique(recentFiles.begin(), recentFiles.end()),
                    recentFiles.end());
  if (recentFiles.size() > 10) {
    recentFiles.resize(10);
  }
}

// This function updates teh "Recent Files" list whenever the user hovers the
// mouse over the menu
void Qucs_S_SPAR_Viewer::updateRecentFilesMenu() {
  recentFilesMenu->clear();
  for (const auto& filePath : recentFiles) {
    QAction* action = recentFilesMenu->addAction(filePath);
    connect(action, &QAction::triggered, this,
            [this, filePath]() { loadSession(filePath); });
  }
  recentFilesMenu->addSeparator();
  recentFilesMenu->addAction("Clear Recent Files", this,
                             &Qucs_S_SPAR_Viewer::clearRecentFiles);
}

void Qucs_S_SPAR_Viewer::savePolarPlotSettings(QXmlStreamWriter& xml,
                                               PolarPlotWidget* widget,
                                               const QString& elementName) {
  if (!widget) {
    return;
  }

  auto settings = widget->getSettings();

  xml.writeStartElement(elementName);

  xml.writeTextElement("freqMin", QString::number(settings.freqMin));
  xml.writeTextElement("freqMax", QString::number(settings.freqMax));
  xml.writeTextElement("freqUnit", settings.freqUnit);

  xml.writeTextElement("radius_min", QString::number(settings.radius_min));
  xml.writeTextElement("radius_max", QString::number(settings.radius_max));
  xml.writeTextElement("radius_div", QString::number(settings.radius_div));

  xml.writeTextElement("marker_format", settings.marker_format);

  xml.writeEndElement(); // elementName
}

void Qucs_S_SPAR_Viewer::loadPolarPlotSettings(QXmlStreamReader& xml,
                                               PolarPlotWidget* widget,
                                               const QString& elementName) {
  if (!widget) {
    return;
  }

  if (!(xml.isStartElement() && xml.name() == elementName)) {
    // Not positioned at the correct start element, return early
    return;
  }
  xml.readNext();
  PolarPlotWidget::AxisSettings settings;

  // Read inside the element until the corresponding end element
  while (!(xml.tokenType() == QXmlStreamReader::EndElement)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      QStringView name = xml.name();
      QString text     = xml.readElementText();

      if (name == QStringView(u"freqMin")) {
        settings.freqMin = text.toDouble();
      } else if (name == QStringView(u"freqMax")) {
        settings.freqMax = text.toDouble();
      } else if (name == QStringView(u"freqUnit")) {
        settings.freqUnit = text;
      } else if (name == QStringView(u"radius_min")) {
        settings.radius_min = text.toDouble();
      } else if (name == QStringView(u"radius_max")) {
        settings.radius_max = text.toDouble();
      } else if (name == QStringView(u"radius_div")) {
        settings.radius_div = text.toDouble();
      } else if (name == QStringView(u"marker_format")) {
        settings.marker_format = text;
      }
    }
    xml.readNext();
  }

  widget->setSettings(settings);
  xml.readNext();
}

void Qucs_S_SPAR_Viewer::saveRectangularPlotSettings(
    QXmlStreamWriter& xml, RectangularPlotWidget* widget,
    const QString& elementName) {
  if (!widget) {
    return;
  }

  auto settings = widget->getSettings();

  xml.writeStartElement(elementName);

  xml.writeTextElement("xAxisMin", QString::number(settings.xAxisMin));
  xml.writeTextElement("xAxisMax", QString::number(settings.xAxisMax));
  xml.writeTextElement("xAxisDiv", QString::number(settings.xAxisDiv));
  xml.writeTextElement("xAxisUnits", settings.xAxisUnits);

  xml.writeTextElement("yAxisMin", QString::number(settings.yAxisMin));
  xml.writeTextElement("yAxisMax", QString::number(settings.yAxisMax));
  xml.writeTextElement("yAxisDiv", QString::number(settings.yAxisDiv));

  xml.writeTextElement("y2AxisMin", QString::number(settings.y2AxisMin));
  xml.writeTextElement("y2AxisMax", QString::number(settings.y2AxisMax));
  xml.writeTextElement("y2AxisDiv", QString::number(settings.y2AxisDiv));

  xml.writeTextElement("showValues", settings.showValues ? "true" : "false");
  xml.writeTextElement("lockAxis", settings.lockAxis ? "true" : "false");

  xml.writeEndElement(); // elementName
}

void Qucs_S_SPAR_Viewer::loadRectangularPlotSettings(
    QXmlStreamReader& xml, RectangularPlotWidget* widget,
    const QString& elementName) {
  if (!widget) {
    return;
  }

  if (!(xml.isStartElement() && xml.name() == elementName)) {
    // Not positioned at the correct start element, return early
    return;
  }
  xml.readNext();
  RectangularPlotWidget::AxisSettings settings;

  // Read inside the element until the corresponding end element
  while (!(xml.tokenType() == QXmlStreamReader::EndElement)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      QStringView name = xml.name();
      QString text     = xml.readElementText();

      if (name == QStringView(u"xAxisMin")) {
        settings.xAxisMin = text.toDouble();
      } else if (name == QStringView(u"xAxisMax")) {
        settings.xAxisMax = text.toDouble();
      } else if (name == QStringView(u"xAxisDiv")) {
        settings.xAxisDiv = text.toDouble();
      } else if (name == QStringView(u"xAxisUnits")) {
        settings.xAxisUnits = text;
      } else if (name == QStringView(u"yAxisMin")) {
        settings.yAxisMin = text.toDouble();
      } else if (name == QStringView(u"yAxisMax")) {
        settings.yAxisMax = text.toDouble();
      } else if (name == QStringView(u"yAxisDiv")) {
        settings.yAxisDiv = text.toDouble();
      } else if (name == QStringView(u"y2AxisMin")) {
        settings.y2AxisMin = text.toDouble();
      } else if (name == QStringView(u"y2AxisMax")) {
        settings.y2AxisMax = text.toDouble();
      } else if (name == QStringView(u"y2AxisDiv")) {
        settings.y2AxisDiv = text.toDouble();
      } else if (name == QStringView(u"showValues")) {
        settings.showValues = (text == "true");
      } else if (name == QStringView(u"lockAxis")) {
        settings.lockAxis = (text == "true");
      }
    }
    xml.readNext();
  }

  widget->setSettings(settings);
  xml.readNext();
}

void Qucs_S_SPAR_Viewer::saveSmithPlotSettings(QXmlStreamWriter& xml,
                                               SmithChartWidget* widget,
                                               const QString& elementName) {
  if (!widget) {
    return;
  }

  auto settings = widget->getSettings();

  xml.writeStartElement(elementName);

  xml.writeTextElement("Z0", settings.Z0);
  xml.writeTextElement("freqMin", QString::number(settings.freqMin));
  xml.writeTextElement("freqMax", QString::number(settings.freqMax));
  xml.writeTextElement("freqUnit", settings.freqUnit);

  xml.writeTextElement("z_chart", settings.z_chart ? "true" : "false");
  xml.writeTextElement("y_chart", settings.y_chart ? "true" : "false");

  xml.writeEndElement(); // elementName
}

void Qucs_S_SPAR_Viewer::loadSmithPlotSettings(QXmlStreamReader& xml,
                                               SmithChartWidget* widget,
                                               const QString& elementName) {
  if (!widget) {
    return;
  }

  if (!(xml.isStartElement() && xml.name() == elementName)) {
    // Not positioned at the correct start element, return early
    return;
  }
  xml.readNext();
  SmithChartWidget::AxisSettings settings;

  // Read inside the element until the corresponding end element
  while (!(xml.tokenType() == QXmlStreamReader::EndElement)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      QStringView name = xml.name();
      QString text     = xml.readElementText();

      if (name == QStringView(u"freqMin")) {
        settings.freqMin = text.toDouble();
      } else if (name == QStringView(u"freqMax")) {
        settings.freqMax = text.toDouble();
      } else if (name == QStringView(u"freqUnit")) {
        settings.freqUnit = text;
      } else if (name == QStringView(u"Z0")) {
        settings.Z0 = text;
      } else if (name == QStringView(u"y_chart")) {
        settings.y_chart = (text == "true");
      } else if (name == QStringView(u"z_chart")) {
        settings.z_chart = (text == "true");
      }
    }
    xml.readNext();
  }

  widget->setSettings(settings);
  xml.readNext();
}
