/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *  Based on msvia.cpp from Qucs - Copyright (C) 2004, 2005, 2008 Stefan Jahn
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

#include "./../../SParameterCalculator.h"

void SParameterCalculator::addMicrostripViaToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp) {
  // Extract microstrip via parameters
  int node1 = comp.nodes[0];
  int node2 = comp.nodes[1];

  double D = comp.value.value("D");        // Via diameter in meters
  double h = comp.value.value("h");        // Substrate height in meters
  double t = comp.value.value("t");        // Conductor thickness in meters
  double rho = comp.value.value("rho");    // Resistivity in Ohm*m

  // Calculate via impedance
  Complex Z = calcMicrostripViaImpedance(D, h, t, rho, frequency);

  // Check frequency validity
  if (frequency * h >= 0.03 * C0) {
    // Warning: Model defined for freq*h/C0 < 0.03
    // Model may be less accurate at higher frequencies
  }

  // Convert impedance to admittance
  Complex y = Complex(1.0, 0.0) / Z;

  // Add to admittance matrix (2-port network)
  if (node1 > 0) {
    Y[node1-1][node1-1] += y;
  }

  if (node2 > 0) {
    Y[node2-1][node2-1] += y;
  }

  if (node1 > 0 && node2 > 0) {
    Y[node1-1][node2-1] -= y;
    Y[node2-1][node1-1] -= y;
  }
}

Complex SParameterCalculator::calcMicrostripViaImpedance(double D, double h, double t, 
                                                         double rho, double frequency) {
  double r = D / 2.0;  // Via radius

  // Calculate DC resistance
  double v = h / M_PI / (r * r - (r - t) * (r - t));
  double R = rho * v;

  // Calculate frequency-dependent resistance (skin effect)
  double fs = M_PI * MU0 * t * t / rho;
  double res = R * sqrt(1.0 + frequency * fs);

  // Calculate inductance
  double a = sqrt(r * r + h * h);
  double ind = MU0 * (h * log((h + a) / r) + 1.5 * (r - a));

  // Return complex impedance Z = R(f) + j*omega*L
  return Complex(res, 2.0 * M_PI * frequency * ind);
}

double SParameterCalculator::calcMicrostripViaResistance(double D, double h, double t, double rho) {
  double r = D / 2.0;  // Via radius
  double v = h / M_PI / (r * r - (r - t) * (r - t));
  return rho * v;
}
