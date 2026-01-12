/// @file SP_Component.cpp
/// @brief Implementation of function for the S-parameter analysis of the
/// S-parameter block device
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../SParameterCalculator.h"

vector<vector<Complex>>
SParameterCalculator::convertS2Y(const vector<vector<Complex>> &S, double Z0) {

  int N = S.size();
  auto I = createMatrix(N, N);
  auto I_minus_S = createMatrix(N, N);
  auto I_plus_S = createMatrix(N, N);

  for (int i = 0; i < N; i++) {
    I[i][i] = Complex(1, 0);
    for (int j = 0; j < N; j++) {
      I_minus_S[i][j] = I[i][j] - S[i][j];
      I_plus_S[i][j] = I[i][j] + S[i][j];
    }
  }

  auto I_plus_S_inv = invertMatrix(I_plus_S);

  vector<vector<Complex>> Y = createMatrix(N, N);
  double G0 = 1.0 / Z0;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      Complex sum(0, 0);
      for (int k = 0; k < N; k++) {
        sum += I_minus_S[i][k] * I_plus_S_inv[k][j];
      }
      Y[i][j] = G0 * sum;
    }
  }

  return Y;
}

void SParameterCalculator::addSParamBlockToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {

  int numRFPorts = comp.numRFPorts;

  // Validate S-matrix dimensions
  if ((int)comp.Smatrix.size() != numRFPorts) {
    cerr << "Error: S-matrix dimension (" << comp.Smatrix.size()
         << ") does not match RF port count (" << numRFPorts << ")\n";
    return;
  }

  if (numRFPorts == 1) {
    // One-port device (.s1p)
    addOnePortSParamToAdmittance(Y, comp);
  } else if (numRFPorts == 2) {
    // Two-port device (.s2p)
    addTwoPortSParamToAdmittance(Y, comp);
  } else {
    cerr << "Error: Only 1-port and 2-port S-parameter devices are supported. "
         << "Found " << numRFPorts << " ports.\n";
  }
}

void SParameterCalculator::addOnePortSParamToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {

  if (comp.nodes.size() != 2) {
    cerr << "Error: One-port S-parameter device must have exactly 2 circuit "
            "nodes\n";
    return;
  }

  // Extract S11 from 1x1 S-matrix
  Complex S11 = comp.Smatrix[0][0];
  double Z0 = comp.referenceImpedance;

  // Convert S11 to impedance: Z = Z0 * (1 + S11) / (1 - S11)
  Complex denominator = Complex(1, 0) - S11;
  if (abs(denominator) < 1e-12) {
    cerr << "Warning: S11 = 1, impedance is infinite (open circuit)\n";
    return; // Don't add anything for open circuit
  }

  Complex Z_device = Z0 * (Complex(1, 0) + S11) / denominator;

  // Convert to admittance
  if (abs(Z_device) < 1e-12) {
    cerr << "Warning: Device impedance is zero (short circuit)\n";
    Z_device = Complex(1e-12, 0); // Avoid division by zero
  }

  Complex Y_device = Complex(1, 0) / Z_device;

  int node1 = comp.nodes[0];
  int node2 = comp.nodes[1];

  // Add to admittance matrix like a two-terminal device
  if (node1 > 0) {
    Y[node1 - 1][node1 - 1] += Y_device;
  }
  if (node2 > 0) {
    Y[node2 - 1][node2 - 1] += Y_device;
  }
  if (node1 > 0 && node2 > 0) {
    Y[node1 - 1][node2 - 1] -= Y_device;
    Y[node2 - 1][node1 - 1] -= Y_device;
  }
}

void SParameterCalculator::addTwoPortSParamToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {

  if (comp.nodes.size() != 2) {
    cerr << "Error: Two-port S-parameter device must have exactly 2 circuit "
            "nodes\n";
    return;
  }

  double Z0 = comp.referenceImpedance;

  // Convert 2x2 S-matrix to 2x2 Y-matrix
  vector<vector<Complex>> Y_device = convertS2Y(comp.Smatrix, Z0);

  int node1 = comp.nodes[0]; // Port 1 connection (port 2 grounded)
  int node2 = comp.nodes[1]; // Port 2 connection (port 1 grounded)

  // Add Y-parameters to global admittance matrix
  // Y11: self-admittance at node1
  if (node1 > 0) {
    Y[node1 - 1][node1 - 1] += Y_device[0][0];
  }

  // Y22: self-admittance at node2
  if (node2 > 0) {
    Y[node2 - 1][node2 - 1] += Y_device[1][1];
  }

  // Y12, Y21: mutual admittances
  if (node1 > 0 && node2 > 0) {
    Y[node1 - 1][node2 - 1] += Y_device[0][1];
    Y[node2 - 1][node1 - 1] += Y_device[1][0];
  }
}

void SParameterCalculator::addFrequencyDependentSParamBlockToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {

  int numRFPorts = comp.numRFPorts;

  // Get interpolated S-matrix at current frequency
  vector<vector<Complex>> S_interp =
      interpolateFrequencyDependentSMatrix(comp, frequency);

  // Create temporary component with interpolated S-matrix for processing
  Component_SPAR tempComp = comp;
  tempComp.Smatrix = S_interp;

  // Process using the same logic as constant S-parameter blocks
  if (numRFPorts == 1) {
    addOnePortSParamToAdmittance(Y, tempComp);
  } else if (numRFPorts == 2) {
    addTwoPortSParamToAdmittance(Y, tempComp);
  } else {
    cerr << "Error: Only 1-port and 2-port frequency-dependent devices "
            "supported\n";
  }
}

///
/// @brief Adds S-parameter device component to circuit
/// @param name Component identifier string
/// @param nodes Vector of node numbers for connections
/// @param Smatrix S-parameter matrix (frequency-independent)
/// @param numRFPorts Number of RF ports (1, 2, 3, or 4)
/// @param Z0 Reference impedance for S-parameters (typically 50Ω)
///
void SParameterCalculator::addSParameterDevice(
    const string &name, const vector<int> &nodes,
    const vector<vector<Complex>> &Smatrix, int numRFPorts, double Z0 = 50.0) {

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

  components.emplace_back(ComponentType_SPAR::SPAR_BLOCK, name, nodes, Smatrix,
                          numRFPorts, Z0);

  // Update numNodes
  for (int node : nodes) {
    if (node > numNodes) {
      numNodes = node;
    }
  }
}

// To add a S-par block
void SParameterCalculator::addSParameterBlock(
    const string &name, const vector<int> &nodes,
    const vector<vector<Complex>> &Smatrix) {
  components.emplace_back(ComponentType_SPAR::SPAR_BLOCK, name, nodes, Smatrix);
  for (int node : nodes) {
    if (node > numNodes) {
      numNodes = node;
    }
  }
}

///
/// @brief Interpolates S-matrix from frequency-dependent data
/// @param comp Component containing S-parameter data
/// @param freq Target frequency for interpolation (Hz)
/// @return Interpolated S-parameter matrix at specified frequency
/// @details Performs linear interpolation between adjacent frequency points.
///          If freq is outside data range, uses nearest endpoint values (no
///          extrapolation). The real and the imaginary parts are interpolated
///          independently.
/// @note Required for frequency-dependent components
///
vector<vector<Complex>>
SParameterCalculator::interpolateFrequencyDependentSMatrix(
    const Component_SPAR &comp, double freq) {

  if (!comp.freqDepData.contains("frequency")) {
    cerr << "Error: Frequency-dependent S-parameter missing frequency data"
         << endl;
    int N = comp.nodes.size();
    return createMatrix(N, N); // Return zero matrix as fallback
  }

  const QList<double> &frequencies = comp.freqDepData["frequency"];
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

///
/// @brief Extracts S-matrix at specific frequency index
/// @param comp Component containing frequency-dependent S-parameter data
/// @param freqIndex Index into frequency array
/// @return S-parameter matrix at the indexed frequency point
/// @details Direct lookup without interpolation.
/// @note Used when analysis frequency exactly matches a tabulated point, or as
/// part of interpolation routine.
///
vector<vector<Complex>>
SParameterCalculator::extractSMatrixAtIndex(const Component_SPAR &comp,
                                            int freqIndex) {

  int N = comp.nodes.size();
  vector<vector<Complex>> S = createMatrix(N, N);

  for (int row = 0; row < N; row++) {
    for (int col = 0; col < N; col++) {
      QString reKey = QString("S%1%2_re")
                          .arg(QString::number(row + 1),  // %1
                               QString::number(col + 1)); // %2

      QString imKey = QString("S%1%2_im")
                          .arg(QString::number(row + 1),  // %1
                               QString::number(col + 1)); // %2

      double realPart = 0.0, imagPart = 0.0;

      if (comp.freqDepData.contains(reKey)) {
        const auto &values = comp.freqDepData.value(reKey);
        if (freqIndex < values.size()) {
          realPart = values.at(freqIndex);
        }
      }

      if (comp.freqDepData.contains(imKey)) {
        const auto &values = comp.freqDepData.value(imKey);
        if (freqIndex < values.size()) {
          imagPart = values.at(freqIndex);
        }
      }

      S[row][col] = Complex(realPart, imagPart);
    }
  }

  return S;
}

///
/// @brief Parses inline S-matrix from netlist string format
/// @param matrixStr String containing S-parameters in format: (re,im) (re,im);
/// ...
/// @param numPorts Number of ports
/// @return S-parameter matrix extracted from string
/// @details Format examples:
///          1-port: (S11_re,S11_im)
///          2-port: (S11_re,S11_im) (S12_re,S12_im); (S21_re,S21_im)
///          (S22_re,S22_im) Semicolons separate rows, parentheses contain
///          complex pairs (real,imag).
///
vector<vector<Complex>>
SParameterCalculator::parseInlineSMatrix(const QString &matrixStr,
                                         int numPorts) {
  vector<vector<Complex>> Smat(numPorts,
                               vector<Complex>(numPorts, Complex(0, 0)));

  if (numPorts == 1) {
    // Parse single entry: (re,im)
    static const QRegularExpression rx(
        "\\(([-+]?\\d*\\.?\\d+[a-zA-Z]*),([-+]?\\d*\\.?\\d+[a-zA-Z]*)\\)");
    QRegularExpressionMatch match = rx.match(matrixStr);
    if (match.hasMatch()) {
      double re = parseScaledValue(match.captured(1));
      double im = parseScaledValue(match.captured(2));
      Smat[0][0] = Complex(re, im);
    }
  } else if (numPorts == 2) {
    // Parse 2x2 matrix entries separated by semicolons
    QStringList rows = matrixStr.split(";", Qt::SkipEmptyParts);

    for (int r = 0; r < qMin(2, rows.size()); r++) {
      QString row = rows[r].trimmed();
      static const QRegularExpression rx(
          "\\(([-+]?\\d*\\.?\\d+[a-zA-Z]*),([-+]?\\d*\\.?\\d+[a-zA-Z]*)\\)");
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
