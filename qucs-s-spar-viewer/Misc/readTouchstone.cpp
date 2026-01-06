/// @file readTouchstone.cpp
/// @brief Touchstone data reader
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "general.h"

QMap<QString, QList<double>> readTouchstoneFile(const QString &filePath) {
  QMap<QString, QList<double>>
      file_data; // Data structure to store the file data
  QString frequency_unit, parameter, format;
  double freq_scale = 1; // Hz
  double Z0 = 50;        // System impedance. Typically 50 Ohm
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

    if (line.isEmpty()) {
      continue;
    }
    if ((line.at(0).isNumber() == false) && (line.at(0) != '#')) {
      if (file_data["frequency"].size() == 0) {
        // There's still no data
        continue;
      } else {
        // There's already data, so it's very likely that the S-par data has
        // ended and the following lines contain noise data. We must stop at
        // this point.
        break;
      }
    }

    // Check for the option line
    if (line.at(0) == '#') {
      QStringList info = line.split(" ");
      frequency_unit = info.at(1); // Specifies the unit of frequency.
                                   // Legal values are Hz, kHz, MHz, and GHz.
                                   // The default value is GHz.

      frequency_unit = frequency_unit.toLower();

      if (frequency_unit == "khz") {
        freq_scale = 1e3;
      } else {
        if (frequency_unit == "mhz") {
          freq_scale = 1e6;
        } else {
          if (frequency_unit == "ghz") {
            freq_scale = 1e9;
          }
        }
      }

      parameter = info.at(2); // specifies what kind of network parameter data
                              // is contained in the file
      format = info.at(
          3); // Specifies the format of the network parameter data pairs
      Z0 = info.at(5).toDouble();
      file_data["Z0"].append(Z0);

      continue;
    }

    // Split line by whitespace
    values.clear();
    values = line.split(' ');

    file_data["frequency"].append(values[0].toDouble() * freq_scale); // in Hz

    double S_1, S_2, S_3, S_4;
    QString s1, s2, s3, s4;
    int index = 1, data_counter = 0;

    for (int i = 1; i <= number_of_ports; i++) {
      for (int j = 1; j <= number_of_ports; j++) {
        s1 = QStringLiteral("S") + QString::number(j) + QString::number(i) +
             QStringLiteral("_dB");
        s2 = s1.mid(0, s1.length() - 2).append("ang");
        s3 = s1.mid(0, s1.length() - 2).append("re");
        s4 = s1.mid(0, s1.length() - 2).append("im");

        S_1 = values[index].toDouble();
        S_2 = values[index + 1].toDouble();

        convert_MA_RI_to_dB(&S_1, &S_2, &S_3, &S_4, format);

        file_data[s1].append(S_1); // dB
        file_data[s2].append(S_2); // ang
        file_data[s3].append(S_3); // re
        file_data[s4].append(S_4); // im
        index += 2;
        data_counter++;

        // Check if the next values are in the new line
        if ((index >= values.length()) &&
            (data_counter < number_of_ports * number_of_ports)) {
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
