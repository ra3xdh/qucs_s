/// @file MicrostripVia.cpp
/// @brief Implementation of function for the S-parameter analysis of the
/// microstrip via
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../SParameterCalculator.h"

void SParameterCalculator::addMicrostripViaToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {
  // Extract microstrip via parameters
  int node1 = comp.nodes[0];

  int N = comp.value.value("N");               // Number of vias in parallel
  double D = comp.value.value("D");            // Via diameter in meters
  double h = comp.value.value("h");            // Substrate height in meters
  double t = comp.value.value("th");           // Conductor thickness in meters
  double rho = comp.value.value("rho", 1e-10); // Resistivity in Ohm*m

  // Calculate via impedance
  Complex Z = calcMicrostripViaImpedance(D, h, t, rho, frequency);

  // Multiple vias effect
  Z /= N;

  // Check frequency validity
  if (frequency * h >= 0.03 * C0) {
    // Warning: Model defined for freq*h/C0 < 0.03
    // Model may be less accurate at higher frequencies
  }

  // Convert impedance to admittance
  Complex y = Complex(1.0, 0.0) / Z;

  // Add to admittance matrix (1-port to ground)
  if (node1 > 0) {
    Y[node1 - 1][node1 - 1] += y;
  }
}

Complex SParameterCalculator::calcMicrostripViaImpedance(double D, double h,
                                                         double t, double rho,
                                                         double frequency) {
  double r = D / 2.0; // Via radius

  // Calculate DC resistance
  double v = h / M_PI / (r * r - (r - t) * (r - t));
  double R = rho * v;

  // Calculate frequency-dependent resistance (skin effect)
  double fs = M_PI * MU0 * t * t / rho;
  double res = R * sqrt(1.0 + frequency * fs);

  // Calculate inductance
  double a = sqrt(r * r + h * h);
  double ind = MU0 * (h * log((h + a) / r) + 1.5 * (r - a));
  double X = 2.0 * M_PI * frequency * ind;

  // Return complex impedance Z = R(f) + j*omega*L
  return Complex(res, X);
}

double SParameterCalculator::calcMicrostripViaResistance(double D, double h,
                                                         double t, double rho) {
  double r = D / 2.0; // Via radius
  double v = h / M_PI / (r * r - (r - t) * (r - t));
  return rho * v;
}
