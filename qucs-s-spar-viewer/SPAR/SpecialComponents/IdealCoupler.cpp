/// @file IdealCoupler.cpp
/// @brief Implementation of function for the S-parameter analysis of the ideal
/// coupler
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../SParameterCalculator.h"

vector<vector<Complex>>
SParameterCalculator::calculateIdealCouplerYMatrix(double k, double phase_deg,
                                                   double Z0) {
  // k is the linear coupling coefficient (not dB)
  // phase_deg is the phase shift in degrees (180° for rat-race, 90° for
  // quadrature hybrid)
  double t = sqrt(1.0 - k * k); // Transmission coefficient

  // Convert phase from degrees to radians and create complex phase factor
  double phase_rad = phase_deg * M_PI / 180.0;
  Complex phase_factor = Complex(cos(phase_rad), sin(phase_rad));

  double G0 = 1.0 / Z0; // Characteristic conductance

  // Create the S-matrix for an ideal directional coupler with configurable
  // phase Port assignment: 1=input, 2=through, 3=isolated, 4=coupled For an
  // ideal coupler with phase shift φ: S = [0   t   0     k*e^(jφ)  ]
  //     [t   0   k*e^(jφ)  0     ]
  //     [0   k*e^(jφ)  0   t     ]
  //     [k*e^(jφ)  0   t   0     ]

  vector<vector<Complex>> S = createMatrix(4, 4);

  // Calculate coupling with phase shift
  Complex k_phase = k * phase_factor;

  // Fill the S-matrix according to ideal coupler theory with phase shift
  S[0][1] = Complex(t, 0); // S21 = t (through)
  S[0][3] = k_phase;       // S41 = k*e^(jφ) (coupled with phase shift)
  S[1][0] = Complex(t, 0); // S12 = t (reciprocity)
  S[1][2] = k_phase;       // S32 = k*e^(jφ) (coupled with phase shift)
  S[2][1] = k_phase;       // S23 = k*e^(jφ) (coupled with phase shift)
  S[2][3] = Complex(t, 0); // S43 = t (through)
  S[3][0] = k_phase;       // S14 = k*e^(jφ) (coupled with phase shift)
  S[3][2] = Complex(t, 0); // S34 = t (through)

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
  } catch (const exception &e) {
    cerr << "Error calculating coupler Y-matrix: " << e.what() << endl;
    // Return zero matrix on error
    return createMatrix(4, 4);
  }

  return Y;
}

void SParameterCalculator::addIdealCouplerToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {
  if (comp.nodes.size() != 4) {
    cerr << "Error: Ideal coupler must have exactly 4 nodes" << endl;
    return;
  }

  double k = comp.value["k"];                 // Linear coupling coefficient
  double phase_deg = comp.value["phase_deg"]; // Phase shift in degrees
  double Z0 = comp.value["Z0"];               // Characteristic impedance

  // Calculate the 4x4 Y-matrix for the ideal coupler
  vector<vector<Complex>> couplerY =
      calculateIdealCouplerYMatrix(k, phase_deg, Z0);

  // Add the coupler Y-matrix to the global admittance matrix
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int node_i = comp.nodes[i];
      int node_j = comp.nodes[j];

      // Only add if both nodes are not ground (node 0)
      if (node_i > 0 && node_j > 0) {
        Y[node_i - 1][node_j - 1] += couplerY[i][j];
      }
    }
  }
}
