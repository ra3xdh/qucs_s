/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *  Based on msstep.cpp from Qucs - Copyright (C) 2004, 2007, 2008 Stefan Jahn
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

void SParameterCalculator::addMicrostripStepToAdmittance(
    vector<vector<Complex>>& Y, const Component_SPAR& comp) {
  // Extract microstrip step parameters
  int node1 = comp.nodes[0];
  int node2 = comp.nodes[1];

  double W1 = comp.value.value("W1");      // Width of first section in meters
  double W2 = comp.value.value("W2");      // Width of second section in meters
  double h  = comp.value.value("h");       // Substrate height in meters
  double er = comp.value.value("er");      // Relative permittivity
  double t  = comp.value.value("th", 0.0); // Conductor thickness (optional)

  // Default model names (hardcoded for now, can be made configurable later)
  string SModel = "Hammerstad";
  string DModel = "Kirschning";

  // Calculate Z-parameters for the step discontinuity
  Complex z11, z12, z21, z22;
  calcMicrostripStepZ(W1, W2, h, er, t, frequency, SModel, DModel, z11, z12,
                      z21, z22);

  // Convert Z-parameters to Y-parameters
  // For a 2-port: Y = Z^(-1)
  Complex det = z11 * z22 - z12 * z21;

  // Avoid division by zero
  if (abs(det) < 1e-15) {
    return; // Skip this component if singular
  }

  Complex y11 = z22 / det;
  Complex y12 = -z12 / det;
  Complex y21 = -z21 / det;
  Complex y22 = z11 / det;

  // Add to admittance matrix
  if (node1 > 0) {
    Y[node1 - 1][node1 - 1] += y11;
  }

  if (node2 > 0) {
    Y[node2 - 1][node2 - 1] += y22;
  }

  if (node1 > 0 && node2 > 0) {
    Y[node1 - 1][node2 - 1] += y12;
    Y[node2 - 1][node1 - 1] += y21;
  }
}

void SParameterCalculator::calcMicrostripStepZ(
    double W1, double W2, double h, double er, double t, double frequency,
    const string& SModel, const string& DModel, Complex& z11, Complex& z12,
    Complex& z21, Complex& z22) {
  // Compute parallel capacitance
  double t1 = log10(er);
  double t2 = W1 / W2;
  double Cs = sqrt(W1 * W2) * (t2 * (10.1 * t1 + 2.33) - 12.6 * t1 - 3.17);

  // Compute series inductance
  t1        = log10(t2);
  t2        = t2 - 1.0;
  double Ls = h * (t2 * (40.5 + 0.2 * t2) - 75.0 * t1);

  // Calculate line parameters for W1
  double ZlEff1, ErEff1, WEff1, ZlEffFreq1, ErEffFreq1;
  analyseQuasiStatic(W1, h, t, er, SModel, ZlEff1, ErEff1, WEff1);
  analyseDispersion(W1, h, er, ZlEff1, ErEff1, frequency, DModel, ZlEffFreq1,
                    ErEffFreq1);
  double L1 = ZlEffFreq1 * sqrt(ErEffFreq1) / C0;

  // Calculate line parameters for W2
  double ZlEff2, ErEff2, WEff2, ZlEffFreq2, ErEffFreq2;
  analyseQuasiStatic(W2, h, t, er, SModel, ZlEff2, ErEff2, WEff2);
  analyseDispersion(W2, h, er, ZlEff2, ErEff2, frequency, DModel, ZlEffFreq2,
                    ErEffFreq2);
  double L2 = ZlEffFreq2 * sqrt(ErEffFreq2) / C0;

  // Normalize series inductance
  Ls /= (L1 + L2);
  L1 *= Ls;
  L2 *= Ls;

  // Build Z-parameter matrix
  // z21 = -j / (2*pi*f*Cs) * 0.5e12 (Cs is in fF)
  Complex z21_val = Complex(0.0, -0.5e12 / (M_PI * frequency * Cs));

  // z11 = j*2*pi*f*L1 + z21 (L1 is in nH)
  Complex z11_val = Complex(0.0, 2e-9 * M_PI * frequency * L1) + z21_val;

  // z22 = j*2*pi*f*L2 + z21 (L2 is in nH)
  Complex z22_val = Complex(0.0, 2e-9 * M_PI * frequency * L2) + z21_val;

  z11 = z11_val;
  z12 = z21_val;
  z21 = z21_val;
  z22 = z22_val;
}
