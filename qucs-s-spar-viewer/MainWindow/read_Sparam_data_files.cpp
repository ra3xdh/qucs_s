#include "qucs-s-spar-viewer.h"


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
