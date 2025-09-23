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

#include <string>
#include <cmath>
#include <iostream>

class Microstrip {
private:
  // Constants
  static constexpr double Z_FIELD = 376.730313668; // Impedance of free space (ohms)
  static constexpr double PI = 3.14159265358979323846;
  static constexpr double MAX_ERROR = 1e-7;

public:
  // Substrate parameters structure
  struct Substrate {
    double height;      // substrate height (mm)
    double thickness;   // conductor thickness (mm)
    double er;          // relative permittivity

    Substrate(double h = 1.6, double t = 0.035, double epsilon_r = 4.4);
  };

         // Results structure for microstrip calculations
  struct Results {
    double width;       // microstrip width (mm)
    double gap;         // for coupled lines (mm)
    double er_eff;      // effective permittivity
    double zl;          // characteristic impedance (ohms)
    double zl_even;     // even mode impedance (ohms)
    double zl_odd;      // odd mode impedance (ohms)
    int iterations;     // number of iterations used

    Results();
  };

public:
  Microstrip() = default;
  ~Microstrip() = default;

  //Calculates the impedance and relative effective permittivity of a microstrip line.
  static void calcMicrostrip(const Substrate& substrate, double width, double freq, double& er_eff, double& zl);

  //Synthesizes microstrip width for given characteristic impedance.
  static bool synthesizeMicrostrip(double Z0, double freq, const Substrate& substrate, Results& results);

  //Calculates additional line length for microstrip open end.
  static double getMicrostripOpen(double Wh, double er, double er_eff);

  //Synthesizes coupled microstrip dimensions for given even and odd mode impedances.
  static bool synthesizeCoupledMicrostrip(double zl_even, double zl_odd, double freq, const Substrate& substrate, Results& results);

  //Calculates even and odd mode parameters for coupled microstrip lines.
  static void calcCoupledMicrostrip(double width, double gap, double freq,
                                    const Substrate& substrate, double& zl_even, double& zl_odd,
                                    double& er_eff_even, double& er_eff_odd);

  //Prints results in a formatted way
  static void printResults(const Results& results, const std::string& title = "Results");

private:
  // Helper function for dispersion calculations (Kirschning model)
  static double dispersionKirschning(double er, double Wh, double freq,
                                     double& er_eff, double& zl);
};

#endif // MICROSTRIP_H
