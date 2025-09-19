#include "SParameterCalculator.h"
#include <stdexcept>
#include <iomanip>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>

// Component constructor
Component_SPAR::Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, QMap<QString, double> val)
    : type(t), name(n), nodes(nds), value(val), frequency(0.0) {}

Component_SPAR::Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, QMap<QString, Complex> z)
    : type(t), name(n), nodes(nds), value(), frequency(0.0), Zvalue(z) {}

// Port constructor
Port::Port(int n, double z) : node(n), impedance(z) {}

// SParameterCalculator constructor
SParameterCalculator::SParameterCalculator() : numNodes(0), frequency(1e9) {}

// Set netlist and parse it
bool SParameterCalculator::setNetlist(const QString& netlist) {
  currentNetlist = netlist;
  return parseNetlist();
}

// Parse netlist from QString
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

    QStringList parts = trimmedLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    if (parts.isEmpty()) continue;

    QString name = parts[0];

    QString type = name;
    QRegularExpression re("^([A-Za-z]+)\\d+");
    QRegularExpressionMatch match = re.match(name);
    if (match.hasMatch()) {
      type = match.captured(1);  // Match only the alphabetic prefix before digits
    }


    QMap<QString, double> value;
    QMap<QString, Complex> zValue;

    if (type == QString("R") && parts.size() >= 4) {
      // Resistor: R1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double R = parseScaledValue(parts[3]);
      value["R"] = R;
      addComponent(ComponentType_SPAR::RESISTOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == QString("C") && parts.size() >= 4) {
      // Capacitor: C1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double C = parseScaledValue(parts[3]);
      value["C"] = C;
      addComponent(ComponentType_SPAR::CAPACITOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == QString("L") && parts.size() >= 4) {
      // Inductor: L1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double L = parseScaledValue(parts[3]);
      value["L"] = L;
      addComponent(ComponentType_SPAR::INDUCTOR, name.toStdString(), {node1, node2}, value);
    }
    else if (name.startsWith("Z", Qt::CaseInsensitive)) {
      if (parts.size() < 4) {
        cerr << "Error: Invalid complex impedance definition: " << line.toStdString() << endl;
        continue;
      }
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      QString zStr = parts[3];

             // Parse format "R±jX"
      double realPart = 0.0, imagPart = 0.0;
      QRegularExpression regex("([-+]?[0-9]*\\.?[0-9]+)([-+]j[0-9]*\\.?[0-9]+)?");
      QRegularExpressionMatch match = regex.match(zStr);
      if (match.hasMatch()) {
        realPart = match.captured(1).toDouble();
        if (match.captured(2).startsWith("+j") || match.captured(2).startsWith("-j")) {
          QString imagStr = match.captured(2);
          imagStr.remove('j');
          imagPart = imagStr.toDouble();
        }
      }
      Complex z(realPart, imagPart);
      zValue["Z"] = z;
      addComponent(ComponentType_SPAR::COMPLEX_IMPEDANCE, name.toStdString(), {node1, node2}, zValue);
    }
    else if ( ((type == QString("TLIN")) || (type == QString("OSTUB")) || (type == QString("SSTUB"))) && parts.size() >= 5) {
      // Transmission Line: T1 node1 node2 impedance length
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double Z0 = parseScaledValue(parts[3]);
      double Length = parseScaledValue(parts[4], QString("Length"));
      value["Z0"] = Z0;
      value["Length"] = Length; // Store the properly parsed length


      if (node1 > numNodes) numNodes = node1;
            if (node2 > numNodes) numNodes = node2;

      if (type == QString("TLIN")) {
        addComponent(ComponentType_SPAR::TRANSMISSION_LINE, name.toStdString(), {node1, node2}, value);
      } else {
        if (type == QString("OSTUB")) {
          addComponent(ComponentType_SPAR::OPEN_STUB, name.toStdString(), {node1, node2}, value);
        } else {
          addComponent(ComponentType_SPAR::SHORT_STUB, name.toStdString(), {node1, node2}, value);
        }
      }
    } else if ((type == QString("CLIN")) && (parts.size() >= 7)) {
      // Coupled Line: CLIN1 node1 node2 node3 node4 Z0e Z0o length
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      int node3 = parts[3].toInt();
      int node4 = parts[4].toInt();
      double Z0e = parseScaledValue(parts[5]);
      double Z0o = parseScaledValue(parts[6]);
      double Length = parseScaledValue(parts[7], QString("Length"));

      value["Z0e"] = Z0e;
      value["Z0o"] = Z0o;
      value["Length"] = Length; // Store the properly parsed length

             // Rest remains the same...
      for (int node : {node1, node2, node3, node4}) {
        if (node > numNodes) numNodes = node;
            }
      addComponent(ComponentType_SPAR::COUPLED_LINE, name.toStdString(), {node1, node2, node3, node4}, value);

    } else if (type == QString("COUPLER") && parts.size() >= 7) {
      // Ideal Coupler: COUPLER1 node1 node2 node3 node4 coupling_coefficient phase_deg [Z0]
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      int node3 = parts[3].toInt();
      int node4 = parts[4].toInt();
      double coupling_coeff = parseScaledValue(parts[5]); // Linear coupling coefficient k
      double phase_deg = parseScaledValue(parts[6]); // Phase shift in degrees
      double Z0 = 50.0; // Default impedance
      if (parts.size() >= 8) {
        Z0 = parseScaledValue(parts[7]);
      }

      value["k"] = coupling_coeff;  // Store as linear coefficient
      value["phase_deg"] = phase_deg; // Store phase in degrees
      value["Z0"] = Z0;

             // Update numNodes
      for (int node : {node1, node2, node3, node4}) {
        if (node > numNodes) numNodes = node;
            }
      addComponent(ComponentType_SPAR::IDEAL_COUPLER, name.toStdString(), {node1, node2, node3, node4}, value);
    }
    else if (type == QString("P") && parts.size() >= 2) {
      // Port: P1 node [impedance]
      int node = parts[1].toInt();
      double impedance = 50.0; // Default impedance
      if (parts.size() >= 3) {
        impedance = parseScaledValue(parts[2]);
      }
      // Update numNodes before adding the port
      if (node > numNodes) numNodes = node;
            addPort(node, impedance);
    }
    else if (type == QString("SPAR")) {
      if (parts.size() < 4) {
        cerr << "Error: Invalid SPAR definition: " << line.toStdString() << endl;
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
        if (node > numNodes) numNodes = node;
    }

      if (nodes.size() != 2) {
        cerr << "Error: SPAR must have exactly 2 nodes\n";
        continue;
      }

      // Determine port count based on nodes: if one node is 0 (GND), it's 1-port
      int numRFPorts = (nodes[0] == 0 || nodes[1] == 0) ? 1 : 2;

      // Check if next part is a filename
      if (idx < parts.size() && !parts[idx].startsWith("(")) {
        QString filename = parts[idx];

        // Load S-parameters from file
        QMap<QString, QList<double>> touchstoneData = readTouchstoneFile(filename);

        if (touchstoneData.isEmpty()) {
          cerr << "Error: Failed to load " << filename.toStdString() << endl;
          continue;
        }

        // Use port count from file if available, otherwise use node-based detection
        int filePortCount = touchstoneData.contains("n_ports") ?
                                touchstoneData["n_ports"].first() : numRFPorts;

        components.emplace_back(ComponentType_SPAR::FREQUENCY_DEPENDENT_SPAR_BLOCK,
                                name.toStdString(),
                                std::vector<int>(nodes.constBegin(), nodes.constEnd()),
                                touchstoneData, filePortCount);

        cout << "Loaded " << filePortCount << "-port S-parameter device from "
             << filename.toStdString() << endl;
      }
      else {
        // Inline S-matrix definition
        // Format: SPAR1 node1 node2 <S-matrix entries>
        // 1-port: (S11_re,S11_im)
        // 2-port: (S11_re,S11_im) (S12_re,S12_im); (S21_re,S21_im) (S22_re,S22_im)

        QString matrixStr;
        for (int k = idx; k < parts.size(); k++) {
          matrixStr += parts[k] + " ";
        }

        vector<vector<Complex>> Smat = parseInlineSMatrix(matrixStr, numRFPorts);

        if ((int)Smat.size() == numRFPorts) {
          components.emplace_back(ComponentType_SPAR::SPAR_BLOCK,
                                  name.toStdString(),
                                  std::vector<int>(nodes.constBegin(), nodes.constEnd()),
                                  Smat, numRFPorts);

          cout << "Added " << numRFPorts << "-port inline S-parameter device" << endl;
        } else {
          cerr << "Error: Failed to parse inline S-matrix\n";
        }
      }
    }
  }
  cout << "Parsed " << components.size() << " components, numNodes = " << numNodes << endl;
  return true;
}


// Matrix operations
vector<vector<Complex>> SParameterCalculator::createMatrix(int rows, int cols) {
  return vector<vector<Complex>>(rows, vector<Complex>(cols, Complex(0, 0)));
}

vector<vector<Complex>> SParameterCalculator::invertMatrix(const vector<vector<Complex>>& matrix) {
  int n = matrix.size();
  vector<vector<Complex>> augmented(n, vector<Complex>(2 * n, Complex(0, 0)));

         // Create augmented matrix [A|I]
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      augmented[i][j] = matrix[i][j];
    }
    augmented[i][i + n] = Complex(1, 0);
  }

         // Forward elimination
  for (int i = 0; i < n; i++) {
    // Find pivot
    int pivot = i;
    for (int j = i + 1; j < n; j++) {
      if (abs(augmented[j][i]) > abs(augmented[pivot][i])) {
        pivot = j;
      }
    }

           // Swap rows
    if (pivot != i) {
      swap(augmented[i], augmented[pivot]);
    }

           // Make diagonal element 1
    Complex diag = augmented[i][i];
    if (abs(diag) < 1e-12) {
      throw runtime_error("Matrix is singular and cannot be inverted");
    }

    for (int j = 0; j < 2 * n; j++) {
      augmented[i][j] /= diag;
    }

           // Eliminate column
    for (int j = 0; j < n; j++) {
      if (i != j) {
        Complex factor = augmented[j][i];
        for (int k = 0; k < 2 * n; k++) {
          augmented[j][k] -= factor * augmented[i][k];
        }
      }
    }
  }

         // Extract inverse matrix
  vector<vector<Complex>> inverse(n, vector<Complex>(n));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      inverse[i][j] = augmented[i][j + n];
    }
  }

  return inverse;
}

Complex SParameterCalculator::getImpedance(const Component_SPAR& comp, double freq) {
  double omega = 2 * M_PI * freq;
  switch (comp.type) {
  case ComponentType_SPAR::RESISTOR:
    return Complex(comp.value["R"], 0);

  case ComponentType_SPAR::COMPLEX_IMPEDANCE:
    return comp.Zvalue["Z"];

  case ComponentType_SPAR::CAPACITOR:
    return Complex(0, -1.0 / (omega * comp.value["C"]));

  case ComponentType_SPAR::INDUCTOR:
    return Complex(0, omega * comp.value["L"]);

  case ComponentType_SPAR::OPEN_STUB: {
    double Z0 = comp.value["Z0"];
    double len = comp.value["Length"];
    const double c = 299792458.0;
    double beta = omega / c;
    double cot_beta_l = 1.0 / tan(beta * len);
    return Complex(0, -Z0 * cot_beta_l);
  }

  case ComponentType_SPAR::SHORT_STUB: {
    double Z0 = comp.value["Z0"];
    double len = comp.value["Length"];
    const double c = 299792458.0;
    double beta = omega / c;
    double tan_beta_l = tan(beta * len);
    return Complex(0, Z0 * tan_beta_l);
  }

  default:
    return Complex(0, 0);
  }
}

vector<vector<Complex>> SParameterCalculator::calculateCoupledLineYMatrix(double Z0e, double Z0o, double length, double freq) {
  const double c = 299792458.0; // speed of light in m/s
  double omega = 2 * M_PI * freq;
  double beta = omega / c;
  double theta = beta * length;
  Complex j(0, 1);

         // Calculate even and odd mode parameters
  double Ye = 1.0 / Z0e; // Even mode admittance
  double Yo = 1.0 / Z0o; // Odd mode admittance

  Complex sinT = sin(theta);
  Complex cosT = cos(theta);

         // Handle the case where sin(theta) is very small
  if (abs(sinT) < 1e-12) {
    // Return zero matrix for resonant lengths
    return createMatrix(4, 4);
  }

         // Calculate Y-parameters for coupled line using even/odd mode analysis
         // The 4x4 Y-matrix for a coupled line is:
         // [Y11  Y12  Y13  Y14]
         // [Y21  Y22  Y23  Y24]
         // [Y31  Y32  Y33  Y34]
         // [Y41  Y42  Y43  Y44]

  vector<vector<Complex>> Y = createMatrix(4, 4);

         // Self admittances (diagonal terms)
  Complex Y11 = j * (Ye + Yo) * cosT / (2.0 * sinT);
  Complex Y22 = Y11;
  Complex Y33 = Y11;
  Complex Y44 = Y11;

         // Mutual admittances between same line (Y12, Y34)
  Complex Y12 = j * (Ye + Yo) / (2.0 * sinT);
  Complex Y34 = Y12;

         // Cross-coupling between different lines (Y13, Y24, Y14, Y23)
  Complex Y13 = j * (Ye - Yo) * cosT / (2.0 * sinT);
  Complex Y24 = Y13;
  Complex Y14 = j * (Ye - Yo) / (2.0 * sinT);
  Complex Y23 = Y14;

         // Fill the Y-matrix
  Y[0][0] = Y11; Y[0][1] = Y12; Y[0][2] = Y13; Y[0][3] = Y14;
  Y[1][0] = Y12; Y[1][1] = Y22; Y[1][2] = Y23; Y[1][3] = Y24;
  Y[2][0] = Y13; Y[2][1] = Y23; Y[2][2] = Y33; Y[2][3] = Y34;
  Y[3][0] = Y14; Y[3][1] = Y24; Y[3][2] = Y34; Y[3][3] = Y44;

  return Y;
}

void SParameterCalculator::addCoupledLineToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp) {
  if (comp.nodes.size() != 4) {
    cerr << "Error: Coupled line must have exactly 4 nodes" << endl;
    return;
  }

  double Z0e = comp.value["Z0e"];
  double Z0o = comp.value["Z0o"];
  double length = comp.value["Length"];

         // Calculate the 4x4 Y-matrix for the coupled line
  vector<vector<Complex>> coupledY = calculateCoupledLineYMatrix(Z0e, Z0o, length, frequency);

         // Add the coupled line Y-matrix to the global admittance matrix
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int node_i = comp.nodes[i];
      int node_j = comp.nodes[j];

             // Only add if both nodes are not ground (node 0)
      if (node_i > 0 && node_j > 0) {
        Y[node_i-1][node_j-1] += coupledY[i][j];
      }
    }
  }
}

vector<vector<Complex>> SParameterCalculator::buildAdmittanceMatrix() {
  vector<vector<Complex>> Y = createMatrix(numNodes, numNodes);

         // First handle ALL lumped elements (R, L, C)
  for (const auto& comp : components) {
    if (comp.type == ComponentType_SPAR::TRANSMISSION_LINE || comp.type == ComponentType_SPAR::COUPLED_LINE)
      continue; // TLIN and CLIN: do separately

    if (comp.type == ComponentType_SPAR::IDEAL_COUPLER) {
      addIdealCouplerToAdmittance(Y, comp);
      continue;
    }

    if (comp.type == ComponentType_SPAR::SPAR_BLOCK) {
      addSParamBlockToAdmittance(Y, comp);
      continue;
    }

    if (comp.type == ComponentType_SPAR::FREQUENCY_DEPENDENT_SPAR_BLOCK) {
      addFrequencyDependentSParamBlockToAdmittance(Y, comp);
      continue;
    }

    Complex impedance = getImpedance(comp, frequency);
    if (abs(impedance) < 1e-12)
      impedance = Complex(1e-12, 0); // Avoid division by zero!

    Complex admittance = Complex(1, 0) / impedance;
    if (comp.nodes.size() == 2) {
      int node1 = comp.nodes[0];
      int node2 = comp.nodes[1];

      if (node1 > 0)
        Y[node1-1][node1-1] += admittance;
            if (node2 > 0)
        Y[node2-1][node2-1] += admittance;
            if (node1 > 0 && node2 > 0) {
        Y[node1-1][node2-1] -= admittance;
        Y[node2-1][node1-1] -= admittance;
      }
    }
  }

         // TRANSMISSION LINES (TLIN)
  for (const auto& comp : components) {
    if (comp.type != ComponentType_SPAR::TRANSMISSION_LINE)
      continue;

           // Extract TLIN parameters
    int node1 = comp.nodes[0];
    int node2 = comp.nodes[1];
    double Z0 = comp.value["Z0"];
    double Length = comp.value["Length"]; // mm

    double freq = frequency;
    double c = 299792458.0; // speed of light [m/s], assume lossless line in air
    double l_m = Length;

    double beta = 2 * M_PI * freq / c;    // [rad/m]
    double theta = beta * l_m;            // [rad]
    Complex j(0, 1);
    Complex Z0c(Z0, 0);

           // Compute TLIN Y-matrix block
    Complex sinT = sin(theta);
    Complex cosT = cos(theta);

           // Handle the (rare) limit of sinT = 0 (open/short-circuit resonance)
    if (abs(sinT) < 1e-12) {
      // Pure open/short
      // Series open: Y=0 block; series short: Y=infinity block. Safer to just skip adding anything.
      continue;
    }

           // Standard 2-port, lossless TLIN block admittance matrix:
           // Y = (1 / (Z0 * sin(theta))) * [ -j cos(theta)   j;
           //                                   j         -j cos(theta)]
    Complex y11 = -j * cosT / (Z0c * sinT);
    Complex y12 =  j      / (Z0c * sinT);
    // y21 == y12; y22 == y11

    if (node1 > 0) {
      Y[node1-1][node1-1] += y11;
    }

    if (node2 > 0) {
      Y[node2-1][node2-1] += y11;
    }

    if (node1 > 0 && node2 > 0) {
      Y[node1-1][node2-1] += y12;
      Y[node2-1][node1-1] += y12;
    }
  }

         // COUPLED LINES (CLIN)
  for (const auto& comp : components) {
    if (comp.type == ComponentType_SPAR::COUPLED_LINE) {
      addCoupledLineToAdmittance(Y, comp);
    }
  }

         //Add small conductance to ground to prevent singular matrix (for all nodes)
  double gmin = 1e-12;
  for (int i = 0; i < numNodes; ++i)
    Y[i][i] += Complex(gmin, 0);

  return Y;
}

void SParameterCalculator::addComponent(ComponentType_SPAR type, const string& name,
                                        const vector<int>& nodes, QMap<QString, Complex> Zvalue) {
  components.emplace_back(type, name, nodes, Zvalue);

         // Update number of nodes
  for (int node : nodes) {
    if (node > numNodes) numNodes = node;
    }
}

void SParameterCalculator::addComponent(ComponentType_SPAR type, const string& name,
                                        const vector<int>& nodes,
                                        QMap<QString, QList<double>> freqDepData) {
  components.emplace_back(type, name, nodes, freqDepData);

         // Update number of nodes
  for (int node : nodes) {
    if (node > numNodes) numNodes = node;
  }
}


void SParameterCalculator::addComponent(ComponentType_SPAR type, const string& name,
                                        const vector<int>& nodes, QMap<QString, double> value) {
  components.emplace_back(type, name, nodes, value);

         // Update number of nodes
  for (int node : nodes) {
    if (node > numNodes) numNodes = node;
    }
}

void SParameterCalculator::addPort(int node, double impedance) {
  ports.emplace_back(node, impedance);
}

vector<vector<Complex>> SParameterCalculator::calculateSParameters() {
  if (ports.empty()) {
    throw runtime_error("No ports defined for S-parameter calculation");
  }

  int numPorts = ports.size();
  vector<vector<Complex>> S = createMatrix(numPorts, numPorts);
  vector<vector<Complex>> Y = buildAdmittanceMatrix();

         // Check all port nodes are within bounds
  for (const auto& port : ports) {
    if (port.node <= 0 || port.node > numNodes) {
      throw runtime_error("Port node " + to_string(port.node) +
                          " is out of bounds (1-" + to_string(numNodes) + ")");
    }
  }

  for (int j = 0; j < numPorts; j++) {
    int systemSize = numNodes + numPorts;
    vector<vector<Complex>> augmentedY = createMatrix(systemSize, systemSize);
    vector<Complex> excitation(systemSize, Complex(0, 0));

           // Copy the internal nodal admittance matrix
    for (int i = 0; i < numNodes; i++) {
      for (int k = 0; k < numNodes; k++) {
        augmentedY[i][k] = Y[i][k];
      }
    }

           // Add port equations
    for (int p = 0; p < numPorts; p++) {
      int portNode = ports[p].node - 1;
      int portEqn = numNodes + p;

      augmentedY[portEqn][portNode] = Complex(1, 0);
      augmentedY[portEqn][portEqn] = Complex(-1, 0);

      Complex Gp = Complex(1.0 / ports[p].impedance, 0);
      augmentedY[portNode][portEqn] = Gp;

      if (p == j) {
        excitation[portNode] = Complex(2.0 / ports[p].impedance, 0);
      }
    }

    try {
      vector<vector<Complex>> augmentedYinv = invertMatrix(augmentedY);
      vector<Complex> solution(systemSize, Complex(0, 0));

      for (int i = 0; i < systemSize; i++) {
        for (int k = 0; k < systemSize; k++) {
          solution[i] += augmentedYinv[i][k] * excitation[k];
        }
      }

      for (int i = 0; i < numPorts; i++) {
        Complex portVoltage = solution[numNodes + i];
        if (i == j) {
          S[i][j] = portVoltage - Complex(1, 0);
        } else {
          S[i][j] = portVoltage;
        }
      }

    } catch (const exception& e) {
      cerr << "Error solving for port " << j+1 << ": " << e.what() << endl;
      throw;
    }
  }

  return S;
}

void SParameterCalculator::printSParameters(const vector<vector<Complex>>& S) {
  int numPorts = S.size();

  cout << "S-Parameters at frequency " << frequency/1e9 << " GHz:" << endl;
  cout << "----------------------------------------" << endl;

  for (int i = 0; i < numPorts; i++) {
    for (int j = 0; j < numPorts; j++) {
      double mag = abs(S[i][j]);
      double phase = arg(S[i][j]) * 180.0 / M_PI;
      double magDB = 20 * log10(mag);

      cout << "S(" << i+1 << "," << j+1 << "): "
           << mag << " ∠" << phase << "° "
           << "(" << magDB << " dB)" << endl;
    }
  }
}

void SParameterCalculator::exportTouchstone(const QString& filename, const vector<vector<Complex>>& S) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    cerr << "Error: Cannot create output file " << filename.toStdString() << endl;
    return;
  }

  QTextStream out(&file);

         // Write header
  out << "! Touchstone file generated by SParameterCalculator\n";
  out << "# GHz S MA R " << ports[0].impedance << "\n";

         // Write S-parameters
  double freqGHz = frequency / 1e9;
  out << freqGHz;

  for (size_t i = 0; i < S.size(); i++) {
    for (size_t j = 0; j < S[i].size(); j++) {
      double mag = abs(S[i][j]);
      double phase = arg(S[i][j]) * 180.0 / M_PI;
      out << " " << mag << " " << phase;
    }
  }

  out << "\n";

  file.close();
  cout << "S-parameters exported to " << filename.toStdString() << endl;
}

void SParameterCalculator::clear() {
  components.clear();
  ports.clear();
  numNodes = 0;
}

void SParameterCalculator::setFrequencySweep(double start, double stop, int points) {
  f_start = start;
  f_stop = stop;
  n_points = points;
}

void SParameterCalculator::calculateSParameterSweep() {
  if (ports.empty()) {
    return;
  }

  sweepResults.clear();
  data.clear();

  int n_ports = ports.size();
  double Z0 = ports.at(0).impedance;
  data["n_ports"].append(n_ports);
  data["Z0"].append(Z0);

  double step = (n_points == 1) ? 0 : (f_stop - f_start) / (n_points - 1);

  for (int i = 0; i < n_points; ++i) {
    double freq = f_start + i * step;
    frequency = freq;
    data["frequency"].append(freq);

    try {
      auto S = calculateSParameters();
      sweepResults.push_back(S);

      for (int row = 1; row <= n_ports; ++row) {
        for (int col = 1; col <= n_ports; ++col) {
          // Get the S-parameter value (note: S matrix uses 0-based indexing)
          Complex sParam = S[row-1][col-1];

                 // Extract real and imaginary parts
          double re = sParam.real();
          double im = sParam.imag();

                 // Calculate magnitude and convert to dB
          double magnitude = sqrt(re * re + im * im);
          double dB = 20.0 * log10(magnitude);

                 // Calculate phase angle in degrees
          double ang = atan2(im, re) * 180.0 / M_PI;

          QString keyDb  = QString("S%1%2_dB").arg(row).arg(col);
          QString keyAng = QString("S%1%2_ang").arg(row).arg(col);
          QString keyRe  = QString("S%1%2_re").arg(row).arg(col);
          QString keyIm  = QString("S%1%2_im").arg(row).arg(col);

          data[keyDb].append(dB);
          data[keyAng].append(ang);
          data[keyRe].append(re);
          data[keyIm].append(im);
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "Error at frequency " << freq << " Hz: " << e.what() << std::endl;
      sweepResults.push_back(
          std::vector<std::vector<Complex>>(ports.size(),
                                            std::vector<Complex>(ports.size(), Complex(0, 0)))
          );
    }
  }
}

void SParameterCalculator::printSParameterSweep() const {
  double step = (n_points == 1) ? 0 : (f_stop - f_start) / (n_points - 1);

  for (int i = 0; i < n_points; ++i) {
    double freq = f_start + i * step;
    const auto& S = sweepResults[i];
    int numPorts = S.size();

    std::cout << "S-Parameters at frequency " << freq / 1e9 << " GHz (" << freq << " Hz):\n";
    std::cout << "----------------------------------------\n";

    for (int i = 0; i < numPorts; ++i) {
      for (int j = 0; j < numPorts; ++j) {
        double mag = abs(S[i][j]);
        double phase = arg(S[i][j]) * 180.0 / M_PI;
        double magDB = 20 * log10(mag);
        std::cout << "S(" << i+1 << "," << j+1 << "): "
                  << mag << " ∠" << phase << "° "
                  << "(" << magDB << " dB)\n";
      }
    }
    std::cout << std::endl;
  }
}

void SParameterCalculator::exportSweepTouchstone(const QString& filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    cerr << "Error: Cannot create output file " << filename.toStdString() << endl;
    return;
  }

  QTextStream out(&file);

         // Write header
  out << "! Touchstone file generated by SParameterCalculator\n";
  out << "# GHz S MA R " << (ports.empty() ? 50.0 : ports[0].impedance) << "\n";

         // Write S-parameters for each frequency
  double step = (n_points == 1) ? 0 : (f_stop - f_start) / (n_points - 1);

  for (int i = 0; i < n_points; ++i) {
    double freq = f_start + i * step;
    double freqGHz = freq / 1e9;
    const auto& S = sweepResults[i];

    out << freqGHz;

    for (const auto& rowVec : S) {
      for (const auto& value : rowVec) {
        double mag = abs(value);
        double phase = arg(value) * 180.0 / M_PI;
        out << " " << mag << " " << phase;
      }
    }

    out << "\n";
  }

  file.close();
  cout << "Frequency sweep exported to " << filename.toStdString() << endl;
}


QMap<QString, QList<double>> SParameterCalculator::getData(){
  return data;
}


double SParameterCalculator::parseScaledValue(const QString& input, QString unit_type) {
  // Regex to split number from unit
  static const QRegularExpression regex("^\\s*([+-]?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?)\\s*([a-zA-ZÂµ]*)\\s*$");
  QRegularExpressionMatch match = regex.match(input);


  if (!match.hasMatch())
    return 0.0;

  double value = match.captured(1).toDouble();
  QString unit = match.captured(2).trimmed();

  if (unit_type == QString("Length")){
    // Handle explicit length units FIRST (before SI prefixes)
    static const QMap<QString, double> lengthUnits = {
        {"mm", 1e-3},      // millimeters to meters
        {"cm", 1e-2},      // centimeters to meters
        {"dm", 1e-1},      // decimeters to meters
        {"m", 1.0},        // meters (base unit)
        {"km", 1e3},       // kilometers to meters
        {"mil", 25.4e-6},  // mils to meters
        {"in", 0.0254},    // inches to meters
        {"ft", 0.3048}     // feet to meters
    };

           // Check for exact length unit match first
    if (lengthUnits.contains(unit)) {
      return value * lengthUnits[unit];
    }
  } else {

    // SI prefixes for electrical units (capacitance, inductance, etc.)
    static const QMap<QString, double> scaleMap = {
        {"f", 1e-15},   // femto
        {"p", 1e-12},   // pico
        {"n", 1e-9},    // nano
        {"u", 1e-6},    // micro
        {"µ", 1e-6},    // micro (Unicode)
        {"m", 1e-3},    // mili
        {"", 1.0},      // no prefix
        {"k", 1e3},     // kilo
        {"K", 1e3},     // kilo
        {"M", 1e6},     // mega
        {"G", 1e9},     // giga
        {"T", 1e12}     // tera
    };

           // For electrical units, extract first character as prefix
    QString prefix;
    if (!unit.isEmpty()) {
      prefix = QString(unit.at(0));
    }
    double scale = scaleMap.value(prefix, 1.0);
    return value * scale;
  }

}


vector<vector<Complex>> SParameterCalculator::calculateIdealCouplerYMatrix(double k, double phase_deg, double Z0) {
  // k is the linear coupling coefficient (not dB)
  // phase_deg is the phase shift in degrees (180° for rat-race, 90° for quadrature hybrid)
  double t = sqrt(1.0 - k*k); // Transmission coefficient

         // Convert phase from degrees to radians and create complex phase factor
  double phase_rad = phase_deg * M_PI / 180.0;
  Complex phase_factor = Complex(cos(phase_rad), sin(phase_rad));

  double G0 = 1.0 / Z0; // Characteristic conductance

         // Create the S-matrix for an ideal directional coupler with configurable phase
         // Port assignment: 1=input, 2=through, 3=isolated, 4=coupled
         // For an ideal coupler with phase shift φ:
         // S = [0   t   0     k*e^(jφ)  ]
         //     [t   0   k*e^(jφ)  0     ]
         //     [0   k*e^(jφ)  0   t     ]
         //     [k*e^(jφ)  0   t   0     ]

  vector<vector<Complex>> S = createMatrix(4, 4);

         // Calculate coupling with phase shift
  Complex k_phase = k * phase_factor;

         // Fill the S-matrix according to ideal coupler theory with phase shift
  S[0][1] = Complex(t, 0);     // S21 = t (through)
  S[0][3] = k_phase;           // S41 = k*e^(jφ) (coupled with phase shift)
  S[1][0] = Complex(t, 0);     // S12 = t (reciprocity)
  S[1][2] = k_phase;           // S32 = k*e^(jφ) (coupled with phase shift)
  S[2][1] = k_phase;           // S23 = k*e^(jφ) (coupled with phase shift)
  S[2][3] = Complex(t, 0);     // S43 = t (through)
  S[3][0] = k_phase;           // S14 = k*e^(jφ) (coupled with phase shift)
  S[3][2] = Complex(t, 0);     // S34 = t (through)

         // Convert S-parameters to Y-parameters using: Y = G0 * (I - S) * inv(I + S)
  vector<vector<Complex>> I = createMatrix(4, 4);
  vector<vector<Complex>> I_minus_S = createMatrix(4, 4);
  vector<vector<Complex>> I_plus_S = createMatrix(4, 4);

         // Identity matrix
  for (int i = 0; i < 4; i++) {
    I[i][i] = Complex(1, 0);
  }

         // Calculate I - S and I + S
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      I_minus_S[i][j] = I[i][j] - S[i][j];
      I_plus_S[i][j] = I[i][j] + S[i][j];
    }
  }

  vector<vector<Complex>> Y = createMatrix(4, 4);

  try {
    // Y = G0 * (I - S) * inv(I + S)
    vector<vector<Complex>> I_plus_S_inv = invertMatrix(I_plus_S);

           // Matrix multiplication: (I - S) * inv(I + S)
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        Complex sum(0, 0);
        for (int k = 0; k < 4; k++) {
          sum += I_minus_S[i][k] * I_plus_S_inv[k][j];
        }
        Y[i][j] = G0 * sum;
      }
    }
  } catch (const exception& e) {
    cerr << "Error calculating coupler Y-matrix: " << e.what() << endl;
    // Return zero matrix on error
    return createMatrix(4, 4);
  }

  return Y;
}

void SParameterCalculator::addIdealCouplerToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp) {
  if (comp.nodes.size() != 4) {
    cerr << "Error: Ideal coupler must have exactly 4 nodes" << endl;
    return;
  }

  double k = comp.value["k"];           // Linear coupling coefficient
  double phase_deg = comp.value["phase_deg"]; // Phase shift in degrees
  double Z0 = comp.value["Z0"];         // Characteristic impedance

         // Calculate the 4x4 Y-matrix for the ideal coupler
  vector<vector<Complex>> couplerY = calculateIdealCouplerYMatrix(k, phase_deg, Z0);

         // Add the coupler Y-matrix to the global admittance matrix
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int node_i = comp.nodes[i];
      int node_j = comp.nodes[j];

             // Only add if both nodes are not ground (node 0)
      if (node_i > 0 && node_j > 0) {
        Y[node_i-1][node_j-1] += couplerY[i][j];
      }
    }
  }
}

// For the S-parameter component
vector<vector<Complex>> SParameterCalculator::convertS2Y(const vector<vector<Complex>>& S, double Z0) {

  int N = S.size();
  auto I = createMatrix(N, N);
  auto I_minus_S = createMatrix(N, N);
  auto I_plus_S = createMatrix(N, N);

  for (int i = 0; i < N; i++) {
    I[i][i] = Complex(1, 0);
    for (int j = 0; j < N; j++) {
      I_minus_S[i][j] = I[i][j] - S[i][j];
      I_plus_S[i][j]  = I[i][j] + S[i][j];
    }
  }

  auto I_plus_S_inv = invertMatrix(I_plus_S);

  vector<vector<Complex>> Y = createMatrix(N, N);
  double G0 = 1.0 / Z0;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      Complex sum(0,0);
      for (int k = 0; k < N; k++) {
        sum += I_minus_S[i][k] * I_plus_S_inv[k][j];
      }
      Y[i][j] = G0 * sum;
    }
  }

  return Y;
}

void SParameterCalculator::addSParamBlockToAdmittance(
    vector<vector<Complex>>& Y, const Component_SPAR& comp) {

  int numRFPorts = comp.numRFPorts;
  int numNodes = comp.nodes.size();

  // Validate S-matrix dimensions
  if ((int)comp.Smatrix.size() != numRFPorts) {
    cerr << "Error: S-matrix dimension (" << comp.Smatrix.size()
    << ") does not match RF port count (" << numRFPorts << ")\n";
    return;
  }

  if (numRFPorts == 1) {
    // One-port device (.s1p)
    addOnePortSParamToAdmittance(Y, comp);
  }
  else if (numRFPorts == 2) {
    // Two-port device (.s2p)
    addTwoPortSParamToAdmittance(Y, comp);
  }
  else {
    cerr << "Error: Only 1-port and 2-port S-parameter devices are supported. "
         << "Found " << numRFPorts << " ports.\n";
  }
}

// Handle one-port device (.s1p file)
void SParameterCalculator::addOnePortSParamToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp) {

  if (comp.nodes.size() != 2) {
    cerr << "Error: One-port S-parameter device must have exactly 2 circuit nodes\n";
    return;
  }

  // Extract S11 from 1x1 S-matrix
  Complex S11 = comp.Smatrix[0][0];
  double Z0 = comp.referenceImpedance;

  // Convert S11 to impedance: Z = Z0 * (1 + S11) / (1 - S11)
  Complex denominator = Complex(1,0) - S11;
  if (abs(denominator) < 1e-12) {
    cerr << "Warning: S11 = 1, impedance is infinite (open circuit)\n";
    return; // Don't add anything for open circuit
  }

  Complex Z_device = Z0 * (Complex(1,0) + S11) / denominator;

  // Convert to admittance
  if (abs(Z_device) < 1e-12) {
    cerr << "Warning: Device impedance is zero (short circuit)\n";
    Z_device = Complex(1e-12, 0); // Avoid division by zero
  }

  Complex Y_device = Complex(1,0) / Z_device;

  int node1 = comp.nodes[0];
  int node2 = comp.nodes[1];

  // Add to admittance matrix like a two-terminal device
  if (node1 > 0) Y[node1-1][node1-1] += Y_device;
    if (node2 > 0) Y[node2-1][node2-1] += Y_device;
    if (node1 > 0 && node2 > 0) {
    Y[node1-1][node2-1] -= Y_device;
    Y[node2-1][node1-1] -= Y_device;
  }
}

// Handle two-port device (.s2p file)
void SParameterCalculator::addTwoPortSParamToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp) {

  if (comp.nodes.size() != 2) {
    cerr << "Error: Two-port S-parameter device must have exactly 2 circuit nodes\n";
    return;
  }

  double Z0 = comp.referenceImpedance;

  // Convert 2x2 S-matrix to 2x2 Y-matrix
  vector<vector<Complex>> Y_device = convertS2Y(comp.Smatrix, Z0);

  int node1 = comp.nodes[0]; // Port 1 connection (port 2 grounded)
  int node2 = comp.nodes[1]; // Port 2 connection (port 1 grounded)

  // Add Y-parameters to global admittance matrix
  // Y11: self-admittance at node1
  if (node1 > 0) Y[node1-1][node1-1] += Y_device[0][0];

  // Y22: self-admittance at node2
  if (node2 > 0) Y[node2-1][node2-1] += Y_device[1][1];

  // Y12, Y21: mutual admittances
  if (node1 > 0 && node2 > 0) {
    Y[node1-1][node2-1] += Y_device[0][1];
    Y[node2-1][node1-1] += Y_device[1][0];
  }
}

// Enhanced frequency-dependent S-parameter handling
void SParameterCalculator::addFrequencyDependentSParamBlockToAdmittance(
    vector<vector<Complex>>& Y, const Component_SPAR& comp) {

  int numRFPorts = comp.numRFPorts;

  // Get interpolated S-matrix at current frequency
  vector<vector<Complex>> S_interp = interpolateFrequencyDependentSMatrix(comp, frequency);

  // Create temporary component with interpolated S-matrix for processing
  Component_SPAR tempComp = comp;
  tempComp.Smatrix = S_interp;

  // Process using the same logic as constant S-parameter blocks
  if (numRFPorts == 1) {
    addOnePortSParamToAdmittance(Y, tempComp);
  }
  else if (numRFPorts == 2) {
    addTwoPortSParamToAdmittance(Y, tempComp);
  }
  else {
    cerr << "Error: Only 1-port and 2-port frequency-dependent devices supported\n";
  }
}

// Helper method to add S-parameter device with explicit port count
void SParameterCalculator::addSParameterDevice(const string& name,
                                               const vector<int>& nodes,
                                               const vector<vector<Complex>>& Smatrix,
                                               int numRFPorts,
                                               double Z0 = 50.0) {

  // Validate inputs
  if (numRFPorts != 1 && numRFPorts != 2) {
    cerr << "Error: Only 1-port and 2-port devices supported\n";
    return;
  }

  if ((int)Smatrix.size() != numRFPorts) {
    cerr << "Error: S-matrix size doesn't match port count\n";
    return;
  }

  if ((numRFPorts == 1 && nodes.size() != 2) ||
      (numRFPorts == 2 && nodes.size() != 2)) {
    cerr << "Error: Node count mismatch for " << numRFPorts << "-port device\n";
    return;
  }

  components.emplace_back(ComponentType_SPAR::SPAR_BLOCK, name, nodes,
                          Smatrix, numRFPorts, Z0);

  // Update numNodes
  for (int node : nodes) {
    if (node > numNodes) numNodes = node;
    }
}


// To add a S-par block
void SParameterCalculator::addSParameterBlock(const string& name, const vector<int>& nodes, const vector<vector<Complex>>& Smatrix) {
  components.emplace_back(ComponentType_SPAR::SPAR_BLOCK, name, nodes, Smatrix);
  for (int node : nodes)
    if (node > numNodes) numNodes = node;
}



// Required for frequency-dependent components
vector<vector<Complex>> SParameterCalculator::interpolateFrequencyDependentSMatrix(
    const Component_SPAR& comp, double freq) {

  if (!comp.freqDepData.contains("frequency")) {
    cerr << "Error: Frequency-dependent S-parameter missing frequency data" << endl;
    int N = comp.nodes.size();
    return createMatrix(N, N); // Return zero matrix as fallback
  }

  const QList<double>& frequencies = comp.freqDepData["frequency"];
  int N = comp.nodes.size(); // Number of ports

  if (frequencies.isEmpty()) {
    return createMatrix(N, N);
  }

         // Handle edge cases
  if (freq <= frequencies.first()) {
    // Use first frequency point
    return extractSMatrixAtIndex(comp, 0);
  }
  if (freq >= frequencies.last()) {
    // Use last frequency point
    return extractSMatrixAtIndex(comp, frequencies.size() - 1);
  }

         // Find interpolation points
  int i = 0;
  for (i = 0; i < frequencies.size() - 1; i++) {
    if (freq >= frequencies[i] && freq <= frequencies[i + 1]) {
      break;
    }
  }

         // Linear interpolation between two frequency points
  double f1 = frequencies[i];
  double f2 = frequencies[i + 1];
  double t = (freq - f1) / (f2 - f1); // Interpolation parameter

  vector<vector<Complex>> S1 = extractSMatrixAtIndex(comp, i);
  vector<vector<Complex>> S2 = extractSMatrixAtIndex(comp, i + 1);

  vector<vector<Complex>> S_interp = createMatrix(N, N);

  for (int row = 0; row < N; row++) {
    for (int col = 0; col < N; col++) {
      // Interpolate real and imaginary parts separately
      Complex s1 = S1[row][col];
      Complex s2 = S2[row][col];

      double real_interp = s1.real() + t * (s2.real() - s1.real());
      double imag_interp = s1.imag() + t * (s2.imag() - s1.imag());

      S_interp[row][col] = Complex(real_interp, imag_interp);
    }
  }

  return S_interp;
}


vector<vector<Complex>> SParameterCalculator::extractSMatrixAtIndex(
    const Component_SPAR& comp, int freqIndex) {

  int N = comp.nodes.size();
  vector<vector<Complex>> S = createMatrix(N, N);

  for (int row = 0; row < N; row++) {
    for (int col = 0; col < N; col++) {
      QString reKey = QString("S%1%2_re").arg(row+1).arg(col+1);
      QString imKey = QString("S%1%2_im").arg(row+1).arg(col+1);

      double realPart = 0.0, imagPart = 0.0;

      if (comp.freqDepData.contains(reKey) &&
          freqIndex < comp.freqDepData[reKey].size()) {
        realPart = comp.freqDepData[reKey][freqIndex];
      }

      if (comp.freqDepData.contains(imKey) &&
          freqIndex < comp.freqDepData[imKey].size()) {
        imagPart = comp.freqDepData[imKey][freqIndex];
      }

      S[row][col] = Complex(realPart, imagPart);
    }
  }

  return S;
}


// Helper function to parse inline S-matrix
vector<vector<Complex>> SParameterCalculator::parseInlineSMatrix(const QString& matrixStr, int numPorts) {
  vector<vector<Complex>> Smat(numPorts, vector<Complex>(numPorts, Complex(0,0)));

  if (numPorts == 1) {
    // Parse single entry: (re,im)
    static const QRegularExpression rx("\\(([-+]?\\d*\\.?\\d+[a-zA-Z]*),([-+]?\\d*\\.?\\d+[a-zA-Z]*)\\)");
    QRegularExpressionMatch match = rx.match(matrixStr);
    if (match.hasMatch()) {
      double re = parseScaledValue(match.captured(1));
      double im = parseScaledValue(match.captured(2));
      Smat[0][0] = Complex(re, im);
    }
  }
  else if (numPorts == 2) {
    // Parse 2x2 matrix entries separated by semicolons
    QStringList rows = matrixStr.split(";", Qt::SkipEmptyParts);

    for (int r = 0; r < qMin(2, rows.size()); r++) {
      QString row = rows[r].trimmed();
      static const QRegularExpression rx("\\(([-+]?\\d*\\.?\\d+[a-zA-Z]*),([-+]?\\d*\\.?\\d+[a-zA-Z]*)\\)");
      QRegularExpressionMatchIterator it = rx.globalMatch(row);

      int c = 0;
      while (it.hasNext() && c < 2) {
        auto match = it.next();
        double re = parseScaledValue(match.captured(1));
        double im = parseScaledValue(match.captured(2));
        Smat[r][c] = Complex(re, im);
        c++;
      }
    }
  }

  return Smat;
}
