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

#include "./../SParameterCalculator.h"

void SParameterCalculator::addTransmissionLineToAdmittance(
    vector<vector<Complex>>& Y, const Component_SPAR& comp) {
  // Extract TLIN parameters
  int node1     = comp.nodes[0];
  int node2     = comp.nodes[1];
  double Z0     = comp.value.value("Z0");
  double Length = comp.value.value("Length"); // mm

  double freq = frequency;
  double c   = 299792458.0; // speed of light [m/s], assume lossless line in air
  double l_m = Length;

  double beta  = 2 * M_PI * freq / c; // [rad/m]
  double theta = beta * l_m;          // [rad]
  Complex j(0, 1);
  Complex Z0c(Z0, 0);

  // Compute TLIN Y-matrix block
  Complex sinT = sin(theta);
  Complex cosT = cos(theta);

  // Handle the (rare) limit of sinT = 0 (open/short-circuit resonance)
  if (abs(sinT) < 1e-12) {
    // Pure open/short
    // Series open: Y=0 block; series short: Y=infinity block. Safer to just
    // skip adding anything.
    return;
  }

  // Standard 2-port, lossless TLIN block admittance matrix:
  // Y = (1 / (Z0 * sin(theta))) * [ -j cos(theta)   j;
  //                                   j         -j cos(theta)]
  Complex y11 = -j * cosT / (Z0c * sinT);
  Complex y12 = j / (Z0c * sinT);
  // y21 == y12; y22 == y11

  if (node1 > 0) {
    Y[node1 - 1][node1 - 1] += y11;
  }

  if (node2 > 0) {
    Y[node2 - 1][node2 - 1] += y11;
  }

  if (node1 > 0 && node2 > 0) {
    Y[node1 - 1][node2 - 1] += y12;
    Y[node2 - 1][node1 - 1] += y12;
  }
}
