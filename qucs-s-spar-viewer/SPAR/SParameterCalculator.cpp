/// @file SParameterCalculator.cpp
/// @brief S-parameter network analysis calculator (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "SParameterCalculator.h"

Complex SParameterCalculator::getImpedance(const Component_SPAR &comp,
                                           double freq) {
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

vector<vector<Complex>> SParameterCalculator::buildAdmittanceMatrix() {
  vector<vector<Complex>> Y = createMatrix(numNodes, numNodes);

  // First handle ALL lumped elements (R, L, C)
  for (const auto &comp : components) {

    // TRANSMISSION LINES (TLIN)
    if (comp.type == ComponentType_SPAR::TRANSMISSION_LINE) {
      addTransmissionLineToAdmittance(Y, comp);
      continue;
    }

    // Microstrip line model
    if (comp.type == ComponentType_SPAR::MICROSTRIP_LINE) {
      addMicrostripLineToAdmittance(Y, comp);
      continue;
    }

    // Microstrip coupled line model
    if (comp.type == ComponentType_SPAR::MICROSTRIP_COUPLED_LINES) {
      addMicrostripCoupledLinesToAdmittance(Y, comp);
      continue;
    }

    // Microstrip via model
    if (comp.type == ComponentType_SPAR::MICROSTRIP_VIA) {
      addMicrostripViaToAdmittance(Y, comp);
      continue;
    }

    // COUPLED LINES (CLIN)
    if (comp.type == ComponentType_SPAR::COUPLED_LINE) {
      addCoupledLineToAdmittance(Y, comp);
      continue;
    }

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
    if (abs(impedance) < 1e-12) {
      impedance = Complex(1e-12, 0); // Avoid division by zero!
    }

    Complex admittance = Complex(1, 0) / impedance;
    if (comp.nodes.size() == 2) {
      int node1 = comp.nodes[0];
      int node2 = comp.nodes[1];

      if (node1 > 0) {
        Y[node1 - 1][node1 - 1] += admittance;
      }
      if (node2 > 0) {
        Y[node2 - 1][node2 - 1] += admittance;
      }
      if (node1 > 0 && node2 > 0) {
        Y[node1 - 1][node2 - 1] -= admittance;
        Y[node2 - 1][node1 - 1] -= admittance;
      }
    }
  }

  // Add small conductance to ground to prevent singular matrix (for all nodes)
  double gmin = 1e-12;
  for (int i = 0; i < numNodes; ++i) {
    Y[i][i] += Complex(gmin, 0);
  }

  return Y;
}

void SParameterCalculator::addComponent(ComponentType_SPAR type,
                                        const string &name,
                                        const vector<int> &nodes,
                                        QMap<QString, Complex> Zvalue) {
  components.emplace_back(type, name, nodes, Zvalue);

  // Update number of nodes
  for (int node : nodes) {
    if (node > numNodes) {
      numNodes = node;
    }
  }
}

void SParameterCalculator::addComponent(
    ComponentType_SPAR type, const string &name, const vector<int> &nodes,
    QMap<QString, QList<double>> freqDepData) {
  components.emplace_back(type, name, nodes, freqDepData);

  // Update number of nodes
  for (int node : nodes) {
    if (node > numNodes) {
      numNodes = node;
    }
  }
}

void SParameterCalculator::addComponent(ComponentType_SPAR type,
                                        const string &name,
                                        const vector<int> &nodes,
                                        QMap<QString, double> value) {
  components.emplace_back(type, name, nodes, value);

  // Update number of nodes
  for (int node : nodes) {
    if (node > numNodes) {
      numNodes = node;
    }
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
  for (const auto &port : ports) {
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

    } catch (const exception &e) {
      cerr << "Error solving for port " << j + 1 << ": " << e.what() << endl;
      throw;
    }
  }

  return S;
}

void SParameterCalculator::setFrequencySweep(double start, double stop,
                                             int points) {
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
          Complex sParam = S[row - 1][col - 1];

          // Extract real and imaginary parts
          double re = sParam.real();
          double im = sParam.imag();

          // Calculate magnitude and convert to dB
          double magnitude = sqrt(re * re + im * im);
          double dB = 20.0 * log10(magnitude);

          // Calculate phase angle in degrees
          double ang = atan2(im, re) * 180.0 / M_PI;

          QString keyDb = QString("S%1%2_dB").arg(row).arg(col);
          QString keyAng = QString("S%1%2_ang").arg(row).arg(col);
          QString keyRe = QString("S%1%2_re").arg(row).arg(col);
          QString keyIm = QString("S%1%2_im").arg(row).arg(col);

          data[keyDb].append(dB);
          data[keyAng].append(ang);
          data[keyRe].append(re);
          data[keyIm].append(im);
        }
      }
    } catch (const std::exception &e) {
      std::cerr << "Error at frequency " << freq << " Hz: " << e.what()
                << std::endl;
      sweepResults.push_back(std::vector<std::vector<Complex>>(
          ports.size(), std::vector<Complex>(ports.size(), Complex(0, 0))));
    }
  }
}
