#include "SParameterCalculator.h"
#include <stdexcept>
#include <iomanip>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>

// Component constructor
Component_SPAR::Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, double val)
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

    if (type == 'R' && parts.size() >= 4) {
      // Resistor: R1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double value = parseScaledValue(parts[3]);
      addComponent(ComponentType_SPAR::RESISTOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == 'C' && parts.size() >= 4) {
      // Capacitor: C1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double value = parseScaledValue(parts[3]);
      addComponent(ComponentType_SPAR::CAPACITOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == 'L' && parts.size() >= 4) {
      // Inductor: L1 node1 node2 value
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double value = parseScaledValue(parts[3]);
      addComponent(ComponentType_SPAR::INDUCTOR, name.toStdString(), {node1, node2}, value);
    }
    else if (type == 'T' && parts.size() >= 4) {
      // Transmission Line: T1 node1 node2 impedance
      int node1 = parts[1].toInt();
      int node2 = parts[2].toInt();
      double impedance = parseScaledValue(parts[3]);
      addComponent(ComponentType_SPAR::TRANSMISSION_LINE, name.toStdString(), {node1, node2}, impedance);
    }
    else if (type == 'P' && parts.size() >= 2) {
      // Port: P1 node [impedance]
      int node = parts[1].toInt();
      double impedance = parseScaledValue(parts[2]);
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
    return Complex(comp.value, 0);
  case ComponentType_SPAR::CAPACITOR:
    return Complex(0, -1.0 / (omega * comp.value));
  case ComponentType_SPAR::INDUCTOR:
    return Complex(0, omega * comp.value);
  case ComponentType_SPAR::TRANSMISSION_LINE:
    return Complex(comp.value, 0); // Characteristic impedance
  default:
    return Complex(0, 0);
  }
}

vector<vector<Complex>> SParameterCalculator::buildAdmittanceMatrix() {
  vector<vector<Complex>> Y = createMatrix(numNodes, numNodes);

  for (const auto& comp : components) {
    if (comp.type == ComponentType_SPAR::VOLTAGE_SOURCE ||
        comp.type == ComponentType_SPAR::CURRENT_SOURCE) {
      continue; // Handle sources separately
    }

    Complex impedance = getImpedance(comp, frequency);
    Complex admittance = Complex(1, 0) / impedance;

    if (comp.nodes.size() == 2) {
      int node1 = comp.nodes[0];
      int node2 = comp.nodes[1];

             // Add to diagonal elements
      if (node1 > 0) Y[node1-1][node1-1] += admittance;
            if (node2 > 0) Y[node2-1][node2-1] += admittance;

             // Add to off-diagonal elements
      if (node1 > 0 && node2 > 0) {
        Y[node1-1][node2-1] -= admittance;
        Y[node2-1][node1-1] -= admittance;
      }
    }
  }

  return Y;
}

void SParameterCalculator::addComponent(ComponentType_SPAR type, const string& name,
                                        const vector<int>& nodes, double value) {
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

  for (int i = 0; i < S.size(); i++) {
    for (int j = 0; j < S[i].size(); j++) {
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

      for (int i = 1; i <= n_ports; i++) {
        for (int j = 1; j <= n_ports; j++) {
          // Get the S-parameter value (note: S matrix uses 0-based indexing)
          Complex sParam = S[i-1][j-1];

          // Extract real and imaginary parts
          double re = sParam.real();
          double im = sParam.imag();

          // Calculate magnitude and convert to dB
          double magnitude = sqrt(re * re + im * im);
          double dB = 20.0 * log10(magnitude);

          // Calculate phase angle in degrees
          double ang = atan2(im, re) * 180.0 / M_PI;

          QString keyDb  = QString("S%1%2_dB").arg(j).arg(i);
          QString keyAng = QString("S%1%2ang").arg(j).arg(i);
          QString keyRe  = QString("S%1%2re").arg(j).arg(i);
          QString keyIm  = QString("S%1%2im").arg(j).arg(i);

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

    for (int row = 0; row < S.size(); row++) {
      for (int col = 0; col < S[row].size(); col++) {
        double mag = abs(S[row][col]);
        double phase = arg(S[row][col]) * 180.0 / M_PI;
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
