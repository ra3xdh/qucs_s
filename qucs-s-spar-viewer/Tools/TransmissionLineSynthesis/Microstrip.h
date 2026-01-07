/// @file Microstrip.h
/// @brief Synthesize microstrip lines (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-laterng with this program.  If not, see
/// <https://www.gnu.org/licenses/>.

#ifndef MICROSTRIP_H
#define MICROSTRIP_H

#include "../../Schematic/structures.h"
#include <cmath>
#include <iostream>
#include <string>

/// @class MicrostripClass
/// @brief Synthesize microstrip lines
class MicrostripClass {
  private:
    // Constants
    static constexpr double Z_FIELD =
        376.730313668; // Impedance of free space (ohms)
    static constexpr double PI        = 3.14159265358979323846;
    static constexpr double MAX_ERROR = 1e-7;

  public:

    /// @struct SynthesisResults
    /// @brief Structure for microstrip calculations
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

    /// @brief Substrate definition
    MS_Substrate Substrate;

    /// @brief Synthesis results
    SynthesisResults Results;

  public:
    /// @brieg Class constructor
    MicrostripClass()  = default;

    /// @brief Class destructor
    ~MicrostripClass() = default;

    /// @brief Calculates the impedance and relative effective permittivity of a
    /// microstrip line.
    /// @param width: Trace width (mm).
    /// @param freq:  Frequency (Hz).
    /// @param er_eff:  Effective relative permittivity (output).
    /// @param[out] zl      Characteristic impedance (Ω, output).
    void calcMicrostrip(double width, double freq, double& er_eff, double& zl);

    /// @brief Synthesizes microstrip width for given characteristic impedance.
    bool synthesizeMicrostrip(double Z0, double e_length, double freq);

    /// @brief Calculates additional line length for microstrip open end.
    double getMicrostripOpen(double Wh, double er, double er_eff);

    /// @brief Synthesizes coupled microstrip dimensions for given even and odd mode
    /// impedances.
    bool synthesizeCoupledMicrostrip(double zl_even, double zl_odd, double length,
                                     double freq);

    /// @brief Calculates even and odd mode parameters for coupled microstrip lines.
    void calcCoupledMicrostrip(double width, double gap, double freq,
                               double& zl_even, double& zl_odd,
                               double& er_eff_even, double& er_eff_odd);

    /// @brief Prints results in a formatted way
    void printResults(const std::string& title = "Results");

  private:
    /// @brief Helper function for dispersion calculations (Kirschning model)
    static double dispersionKirschning(double er, double Wh, double freq,
                                       double& er_eff, double& zl);
};

#endif // MICROSTRIP_H
