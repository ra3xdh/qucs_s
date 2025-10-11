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

#ifndef MICROSTRIP_H
#define MICROSTRIP_H

#include "../../Schematic/structures.h"
#include <cmath>
#include <iostream>
#include <string>

class MicrostripClass {
private:
  // Constants
  static constexpr double Z_FIELD =
      376.730313668; // Impedance of free space (ohms)
  static constexpr double PI        = 3.14159265358979323846;
  static constexpr double MAX_ERROR = 1e-7;

public:
  // Results structure for microstrip calculations
  struct SynthesisResults {
    double width;   // microstrip width (mm)
    double length;  // microstrip length (mm)
    double gap;     // for coupled lines (mm)
    double er_eff;  // effective permittivity
    double zl;      // characteristic impedance (ohms)
    double zl_even; // even mode impedance (ohms)
    double zl_odd;  // odd mode impedance (ohms)
    int iterations; // number of iterations used

    SynthesisResults();
  };

  MS_Substrate Substrate;
  SynthesisResults Results;

public:
  MicrostripClass()  = default;
  ~MicrostripClass() = default;

  // Calculates the impedance and relative effective permittivity of a
  // microstrip line.
  void calcMicrostrip(double width, double freq, double& er_eff, double& zl);

  // Synthesizes microstrip width for given characteristic impedance.
  bool synthesizeMicrostrip(double Z0, double e_length, double freq);

  // Calculates additional line length for microstrip open end.
  double getMicrostripOpen(double Wh, double er, double er_eff);

  // Synthesizes coupled microstrip dimensions for given even and odd mode
  // impedances.
  bool synthesizeCoupledMicrostrip(double zl_even, double zl_odd, double length,
                                   double freq);

  // Calculates even and odd mode parameters for coupled microstrip lines.
  void calcCoupledMicrostrip(double width, double gap, double freq,
                             double& zl_even, double& zl_odd,
                             double& er_eff_even, double& er_eff_odd);

  // Prints results in a formatted way
  void printResults(const std::string& title = "Results");

private:
  // Helper function for dispersion calculations (Kirschning model)
  static double dispersionKirschning(double er, double Wh, double freq,
                                     double& er_eff, double& zl);
};

#endif // MICROSTRIP_H
