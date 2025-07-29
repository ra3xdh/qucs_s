#include "SParameterCalculator.h"
#include <stdexcept>
#include <iomanip>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>

// Component constructor
Component_SPAR::Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, QMap<QString, double> val)
                              : type(t), name(n), nodes(nds), value(val), frequency(0.0) {}

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

  for (const QString& line : lines) {
    QString trimmedLine = line.trimmed();

    // Skip comments and empty lines
    if (trimmedLine.isEmpty() || trimmedLine.startsWith('*')) {
      continue;
    }

    QStringList parts = trimmedLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    if (parts.isEmpty()) continue;

    QString name = parts[0];
    char type = name[0].toUpper().toLatin1();
    QMap<QString, double> value;

    if (type == 'R' && parts.size() >= 4) {
      // Resistor: R1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double R = parseScaledValue(parts[3]);
      value["R"] = R;
      addComponent(ComponentType_SPAR::RESISTOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == 'C' && parts.size() >= 4) {
      // Capacitor: C1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double C = parseScaledValue(parts[3]);
      value["C"] = C;
      addComponent(ComponentType_SPAR::CAPACITOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == 'L' && parts.size() >= 4) {
      // Inductor: L1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double L = parseScaledValue(parts[3]);
      value["L"] = L;
      addComponent(ComponentType_SPAR::INDUCTOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == 'T' && parts.size() >= 5) {
      // Transmission Line: T1 node1 node2 impedance length
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double Z0 = parseScaledValue(parts[3]);
      double Length = parseScaledValue(parts[4]);
      value["Z0"] = Z0;
      value["Length"] = Length;
      // Update numNodes before adding component
      if (node1 > numNodes) {
        numNodes = node1;
      }
      if (node2 > numNodes) {
         numNodes = node2;
      }
      addComponent(ComponentType_SPAR::TRANSMISSION_LINE, name.toStdString(), {node1, node2}, value);
    }
    else if (type == 'P' && parts.size() >= 2) {
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
  }

  return true;
}

// Matrix operations (unchanged)
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
  case ComponentType_SPAR::CAPACITOR:
    return Complex(0, -1.0 / (omega * comp.value["C"]));
  case ComponentType_SPAR::INDUCTOR:
    return Complex(0, omega * comp.value["L"]);
  // Transmission lines are handled in buildAdmittanceMatrix()
  default:
    return Complex(0, 0);
  }
}



vector<vector<Complex>> SParameterCalculator::buildAdmittanceMatrix() {
  vector<vector<Complex>> Y = createMatrix(numNodes, numNodes);

  // First handle ALL lumped elements (R, L, C)
  for (const auto& comp : components) {
    if (comp.type == ComponentType_SPAR::TRANSMISSION_LINE)
      continue; // TLIN: do separately

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

  // Now add all TRANSMISSION LINES (TLIN)
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
    double l_m = Length * 1e-3; // mm -> m

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

  //Add small conductance to ground to prevent singular matrix (for all nodes)
  double gmin = 1e-12;
  for (int i = 0; i < numNodes; ++i)
    Y[i][i] += Complex(gmin, 0);

  return Y;
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
          QString keyAng = QString("S%1%2ang").arg(row).arg(col);
          QString keyRe  = QString("S%1%2re").arg(row).arg(col);
          QString keyIm  = QString("S%1%2im").arg(row).arg(col);

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


double SParameterCalculator::parseScaledValue(const QString& input) {
  // Regex to split number from unit (no space required)
  QRegularExpression regex("^\\s*([+-]?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?)\\s*([a-zA-Zµ]*)\\s*$");
  QRegularExpressionMatch match = regex.match(input);

  if (!match.hasMatch())
    return 0.0;

  double value = match.captured(1).toDouble();
  QString unit = match.captured(2).trimmed();

         // SI prefixes and their multipliers
  static const QMap<QString, double> scaleMap = {
      {"f", 1e-15},
      {"p", 1e-12},
      {"n", 1e-9},
      {"u", 1e-6}, {"µ", 1e-6},
      {"m", 1e-3},
      {"", 1},
      {"k", 1e3}, {"K", 1e3},
      {"M", 1e6},
      {"G", 1e9},
      {"T", 1e12}
  };

         // Extract the prefix only (first letter of the unit)
  QString prefix;
  if (!unit.isEmpty()) {
    QChar first = unit.at(0);
    prefix = QString(first);
  }

  double scale = scaleMap.value(prefix, 1.0);
  return value * scale;
}
