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

#include "SParameterCalculator.h"

// Being the netlist save in currentNetlist, this function reads the netlist
// line by line and add each component

bool SParameterCalculator::parseNetlist() {
  if (currentNetlist.isEmpty()) {
    cerr << "Error: No netlist content provided" << endl;
    return false;
  }

  // Clear existing circuit
  clear();

  // Split netlist into lines
  QStringList lines = currentNetlist.split('\n', Qt::SkipEmptyParts);

  for (const QString& line : qAsConst(lines)) {
    QString trimmedLine = line.trimmed();

    // Skip comments and empty lines
    if (trimmedLine.isEmpty() || trimmedLine.startsWith('*')) {
      continue;
    }

    QStringList parts =
        trimmedLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    if (parts.isEmpty()) {
      continue;
    }

    QString name = parts[0];

    QString type = name;
    QRegularExpression re("^([A-Za-z]+)\\d+");
    QRegularExpressionMatch match = re.match(name);
    if (match.hasMatch()) {
      type =
          match.captured(1); // Match only the alphabetic prefix before digits
    }

    QMap<QString, double> value;
    QMap<QString, Complex> zValue;

    if (type == QString("R") && parts.size() >= 4) {
      // Resistor: R1 node1 node2 value
      int node1  = parts[1].toInt();
      int node2  = parts[2].toInt();
      double R   = parseScaledValue(parts[3]);
      value["R"] = R;
      addComponent(ComponentType_SPAR::RESISTOR, name.toStdString(),
                   {node1, node2}, value);
    } else if (type == QString("C") && parts.size() >= 4) {
      // Capacitor: C1 node1 node2 value
      int node1  = parts[1].toInt();
      int node2  = parts[2].toInt();
      double C   = parseScaledValue(parts[3]);
      value["C"] = C;
      addComponent(ComponentType_SPAR::CAPACITOR, name.toStdString(),
                   {node1, node2}, value);
    } else if (type == QString("L") && parts.size() >= 4) {
      // Inductor: L1 node1 node2 value
      int node1  = parts[1].toInt();
      int node2  = parts[2].toInt();
      double L   = parseScaledValue(parts[3]);
      value["L"] = L;
      addComponent(ComponentType_SPAR::INDUCTOR, name.toStdString(),
                   {node1, node2}, value);
    } else if (name.startsWith("Z", Qt::CaseInsensitive)) {
      if (parts.size() < 4) {
        cerr << "Error: Invalid complex impedance definition: "
             << line.toStdString() << endl;
        continue;
      }
      int node1    = parts[1].toInt();
      int node2    = parts[2].toInt();
      QString zStr = parts[3];

      // Parse format "R±jX"
      double realPart = 0.0, imagPart = 0.0;
      QRegularExpression regex(
          "^\\s*"
          "([-+]?\\d*\\.?\\d+(?:[kKmM]?))" // Real part with optional suffix in
                                           // the same group
          "\\s*"
          "(?:Ohm)?"
          "\\s*"
          "((?:[-+]\\s*j\\s*\\d*\\.?\\d+(?:[kKmM]?))?)" // Imaginary part with
                                                        // suffix, everything in
                                                        // one group
          "\\s*"
          "(?:Ohm)?"
          "\\s*$");

      QRegularExpressionMatch match = regex.match(zStr);
      if (match.hasMatch()) {
        realPart = parseScaledValue(match.captured(1));
        if (match.captured(2).startsWith("+j") ||
            match.captured(2).startsWith("-j")) {
          QString imagStr = match.captured(2);
          imagStr.remove('j');
          imagPart = parseScaledValue(imagStr);
        }
      }
      Complex z(realPart, imagPart);
      zValue["Z"] = z;
      addComponent(ComponentType_SPAR::COMPLEX_IMPEDANCE, name.toStdString(),
                   {node1, node2}, zValue);
    } else if (((type == QString("TLIN")) || (type == QString("OSTUB")) ||
                (type == QString("SSTUB"))) &&
               parts.size() >= 5) {
      // Transmission Line: TLIN node1 node2 impedance length
      int node1       = parts[1].toInt();
      int node2       = parts[2].toInt();
      double Z0       = parseScaledValue(parts[3]);
      double Length   = parseScaledValue(parts[4], QString("Length"));
      value["Z0"]     = Z0;
      value["Length"] = Length; // Store the properly parsed length

      if (node1 > numNodes) {
        numNodes = node1;
      }
      if (node2 > numNodes) {
        numNodes = node2;
      }

      if (type == QString("TLIN")) {
        addComponent(ComponentType_SPAR::TRANSMISSION_LINE, name.toStdString(),
                     {node1, node2}, value);
      } else {
        if (type == QString("OSTUB")) {
          addComponent(ComponentType_SPAR::OPEN_STUB, name.toStdString(),
                       {node1, node2}, value);
        } else {
          addComponent(ComponentType_SPAR::SHORT_STUB, name.toStdString(),
                       {node1, node2}, value);
        }
      }
    } else if ((type == QString("MLIN"))) {
      // Microstrip Transmission Line: MLIN node1 node2 length width er h cond
      // th tand
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();

      double Width  = parseScaledValue(parts[3], QString("Length"));
      double Length = parseScaledValue(parts[4], QString("Length"));
      double er     = parseScaledValue(
          parts[5]); // Dielectric permittivity of the substrate
      double h    = parseScaledValue(parts[6]); // substrate height
      double cond = parseScaledValue(parts[7]); // Metal conductivity
      double th   = parseScaledValue(parts[8]); // Metal thickness
      double tand = parseScaledValue(
          parts[9]); // Dissipation factor of the substrate material

      value["Width"]  = Width;
      value["Length"] = Length;
      value["er"]     = er;
      value["h"]      = h;
      value["cond"]   = cond;
      value["th"]     = th;
      value["tand"]   = tand;

      addComponent(ComponentType_SPAR::MICROSTRIP_LINE, name.toStdString(),
                   {node1, node2}, value);

    } else if ((type == QString("MSCOUP"))) {
      // Microstrip Coupled Transmission Lines: MCOUP node1 node2 node3 node4
      // width length gap er h cond th tand
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      int node3 = parts[3].toInt();
      int node4 = parts[4].toInt();

      double W  = parseScaledValue(parts[5], QString("Length"));
      double L  = parseScaledValue(parts[6], QString("Length"));
      double S  = parseScaledValue(parts[7], QString("Length"));
      double er = parseScaledValue(
          parts[8]); // Dielectric permittivity of the substrate
      double h    = parseScaledValue(parts[9]);  // substrate height
      double cond = parseScaledValue(parts[10]); // Metal conductivity
      double th   = parseScaledValue(parts[11]); // Metal thickness
      double tand = parseScaledValue(
          parts[12]); // Dissipation factor of the substrate material

      value["W"]    = W;
      value["L"]    = L;
      value["S"]    = S;
      value["er"]   = er;
      value["h"]    = h;
      value["cond"] = cond;
      value["th"]   = th;
      value["tand"] = tand;

      addComponent(ComponentType_SPAR::MICROSTRIP_COUPLED_LINES,
                   name.toStdString(), {node1, node2, node3, node4}, value);
    } else if ((type == QString("MSTEP"))) {
      // Microstrip Transmission Line step model: MSTEP node1 node2 length width
      // er h cond th tand
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();

      double W1 = parseScaledValue(parts[3], QString("Length"));
      double W2 = parseScaledValue(parts[4], QString("Length"));
      double er = parseScaledValue(
          parts[5]); // Dielectric permittivity of the substrate
      double h    = parseScaledValue(parts[6]); // substrate height
      double cond = parseScaledValue(parts[7]); // Metal conductivity
      double th   = parseScaledValue(parts[8]); // Metal thickness
      double tand = parseScaledValue(
          parts[9]); // Dissipation factor of the substrate material

      value["W1"]   = W1;
      value["W2"]   = W2;
      value["er"]   = er;
      value["h"]    = h;
      value["cond"] = cond;
      value["th"]   = th;
      value["tand"] = tand;

      addComponent(ComponentType_SPAR::MICROSTRIP_STEP, name.toStdString(),
                   {node1, node2}, value);

    } else if ((type == QString("MSOPEN"))) {
      // Microstrip Transmission Line step model: MSOPEN node1 node2 length
      // width er h cond th tand
      int node1 = parts[1].toInt();

      double W  = parseScaledValue(parts[2], QString("Length"));
      double er = parseScaledValue(
          parts[3]); // Dielectric permittivity of the substrate
      double h    = parseScaledValue(parts[4]); // substrate height
      double cond = parseScaledValue(parts[4]); // Metal conductivity
      double th   = parseScaledValue(parts[6]); // Metal thickness
      double tand = parseScaledValue(
          parts[7]); // Dissipation factor of the substrate material

      value["W"]    = W;
      value["er"]   = er;
      value["h"]    = h;
      value["cond"] = cond;
      value["th"]   = th;
      value["tand"] = tand;

      addComponent(ComponentType_SPAR::MICROSTRIP_OPEN, name.toStdString(),
                   {node1}, value);

    } else if ((type == QString("MSVIA"))) {
      // Microstrip Transmission Line model: MSVIA node1 diameter er h cond th
      // tand
      int node1 = parts[1].toInt();
      double D  = parseScaledValue(parts[2], QString("Length")); // Via diameter
      int N     = parts[3].toInt(); // Number of vias in parallel
      double er = parseScaledValue(
          parts[4]); // Dielectric permittivity of the substrate
      double h    = parseScaledValue(parts[5]); // substrate height
      double cond = parseScaledValue(parts[6]); // Metal conductivity
      double th   = parseScaledValue(parts[7]); // Metal thickness
      double tand = parseScaledValue(
          parts[8]); // Dissipation factor of the substrate material

      value["D"]    = D;
      value["N"]    = N;
      value["er"]   = er;
      value["h"]    = h;
      value["cond"] = cond;
      value["th"]   = th;
      value["tand"] = tand;

      addComponent(ComponentType_SPAR::MICROSTRIP_VIA, name.toStdString(),
                   {node1}, value);

    } else if ((type == QString("CLIN")) && (parts.size() >= 7)) {
      // Coupled Line: CLIN1 node1 node2 node3 node4 Z0e Z0o length
      int node1     = parts[1].toInt();
      int node2     = parts[2].toInt();
      int node3     = parts[3].toInt();
      int node4     = parts[4].toInt();
      double Z0e    = parseScaledValue(parts[5]);
      double Z0o    = parseScaledValue(parts[6]);
      double Length = parseScaledValue(parts[7], QString("Length"));

      value["Z0e"]    = Z0e;
      value["Z0o"]    = Z0o;
      value["Length"] = Length; // Store the properly parsed length

      // Rest remains the same...
      for (int node : {node1, node2, node3, node4}) {
        if (node > numNodes) {
          numNodes = node;
        }
      }
      addComponent(ComponentType_SPAR::COUPLED_LINE, name.toStdString(),
                   {node1, node2, node3, node4}, value);

    } else if (type == QString("COUPLER") && parts.size() >= 7) {
      // Ideal Coupler: COUPLER1 node1 node2 node3 node4 coupling_coefficient
      // phase_deg [Z0]
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      int node3 = parts[3].toInt();
      int node4 = parts[4].toInt();
      double coupling_coeff =
          parseScaledValue(parts[5]); // Linear coupling coefficient k
      double phase_deg = parseScaledValue(parts[6]); // Phase shift in degrees
      double Z0        = 50.0;                       // Default impedance
      if (parts.size() >= 8) {
        Z0 = parseScaledValue(parts[7]);
      }

      value["k"]         = coupling_coeff; // Store as linear coefficient
      value["phase_deg"] = phase_deg;      // Store phase in degrees
      value["Z0"]        = Z0;

      // Update numNodes
      for (int node : {node1, node2, node3, node4}) {
        if (node > numNodes) {
          numNodes = node;
        }
      }
      addComponent(ComponentType_SPAR::IDEAL_COUPLER, name.toStdString(),
                   {node1, node2, node3, node4}, value);
    } else if (type == QString("P") && parts.size() >= 2) {
      // Port: P1 node [impedance]
      int node         = parts[1].toInt();
      double impedance = 50.0; // Default impedance
      if (parts.size() >= 3) {
        impedance = parseScaledValue(parts[2]);
      }
      // Update numNodes before adding the port
      if (node > numNodes) {
        numNodes = node;
      }
      addPort(node, impedance);
    } else if (type == QString("SPAR")) {
      if (parts.size() < 4) {
        cerr << "Error: Invalid SPAR definition: " << line.toStdString()
             << endl;
        continue;
      }

      // Extract nodes
      QVector<int> nodes;
      int idx = 1;

      // Parse exactly 2 nodes for both 1-port and 2-port devices
      for (int i = 0; i < 2 && idx < parts.size(); i++, idx++) {
        bool ok;
        int node = parts[idx].toInt(&ok);
        if (!ok) {
          cerr << "Error: Invalid node in SPAR definition\n";
          break;
        }
        nodes.push_back(node);
        if (node > numNodes) {
          numNodes = node;
        }
      }

      if (nodes.size() != 2) {
        cerr << "Error: SPAR must have exactly 2 nodes\n";
        continue;
      }

      // Determine port count based on nodes: if one node is 0 (GND), it's
      // 1-port
      int numRFPorts = (nodes[0] == 0 || nodes[1] == 0) ? 1 : 2;

      // Check if next part is a filename
      if (idx < parts.size() && !parts[idx].startsWith("(")) {
        QString filename = parts[idx];

        // Load S-parameters from file
        QMap<QString, QList<double>> touchstoneData =
            readTouchstoneFile(filename);

        if (touchstoneData.isEmpty()) {
          cerr << "Error: Failed to load " << filename.toStdString() << endl;
          continue;
        }

        // Use port count from file if available, otherwise use node-based
        // detection
        int filePortCount = touchstoneData.contains("n_ports")
                                ? touchstoneData["n_ports"].first()
                                : numRFPorts;

        components.emplace_back(
            ComponentType_SPAR::FREQUENCY_DEPENDENT_SPAR_BLOCK,
            name.toStdString(),
            std::vector<int>(nodes.constBegin(), nodes.constEnd()),
            touchstoneData, filePortCount);

        cout << "Loaded " << filePortCount << "-port S-parameter device from "
             << filename.toStdString() << endl;
      } else {
        // Inline S-matrix definition
        // Format: SPAR1 node1 node2 <S-matrix entries>
        // 1-port: (S11_re,S11_im)
        // 2-port: (S11_re,S11_im) (S12_re,S12_im); (S21_re,S21_im)
        // (S22_re,S22_im)

        QString matrixStr;
        for (int k = idx; k < parts.size(); k++) {
          matrixStr += parts[k] + " ";
        }

        vector<vector<Complex>> Smat =
            parseInlineSMatrix(matrixStr, numRFPorts);

        if ((int)Smat.size() == numRFPorts) {
          components.emplace_back(
              ComponentType_SPAR::SPAR_BLOCK, name.toStdString(),
              std::vector<int>(nodes.constBegin(), nodes.constEnd()), Smat,
              numRFPorts);

          cout << "Added " << numRFPorts << "-port inline S-parameter device"
               << endl;
        } else {
          cerr << "Error: Failed to parse inline S-matrix\n";
        }
      }
    }
  }
  cout << "Parsed " << components.size()
       << " components, numNodes = " << numNodes << endl;
  return true;
}
