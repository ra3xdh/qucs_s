/// @file CoupledLines.cpp
/// @brief Implementation of function for the S-parameter analysis of the ideal
/// coupled lines
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../SParameterCalculator.h"

vector<vector<Complex>>
SParameterCalculator::calculateCoupledLineYMatrix(double Z0e, double Z0o,
                                                  double length, double freq) {
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
  Y[0][0] = Y11;
  Y[0][1] = Y12;
  Y[0][2] = Y13;
  Y[0][3] = Y14;
  Y[1][0] = Y12;
  Y[1][1] = Y22;
  Y[1][2] = Y23;
  Y[1][3] = Y24;
  Y[2][0] = Y13;
  Y[2][1] = Y23;
  Y[2][2] = Y33;
  Y[2][3] = Y34;
  Y[3][0] = Y14;
  Y[3][1] = Y24;
  Y[3][2] = Y34;
  Y[3][3] = Y44;

  return Y;
}

void SParameterCalculator::addCoupledLineToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {
  if (comp.nodes.size() != 4) {
    cerr << "Error: Coupled line must have exactly 4 nodes" << endl;
    return;
  }

  double Z0e = comp.value["Z0e"];
  double Z0o = comp.value["Z0o"];
  double length = comp.value["Length"];

  // Calculate the 4x4 Y-matrix for the coupled line
  vector<vector<Complex>> coupledY =
      calculateCoupledLineYMatrix(Z0e, Z0o, length, frequency);

  // Add the coupled line Y-matrix to the global admittance matrix
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int node_i = comp.nodes[i];
      int node_j = comp.nodes[j];

      // Only add if both nodes are not ground (node 0)
      if (node_i > 0 && node_j > 0) {
        Y[node_i - 1][node_j - 1] += coupledY[i][j];
      }
    }
  }
}
