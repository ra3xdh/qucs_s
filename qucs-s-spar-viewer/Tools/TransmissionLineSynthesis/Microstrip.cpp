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

#include "Microstrip.h"

// Results struct implementation
MicrostripClass::SynthesisResults::SynthesisResults()
    : width(0), gap(0), er_eff(0), zl(0), zl_even(0), zl_odd(0), iterations(0) {}

// Main implementation of microstrip calculation
void MicrostripClass::calcMicrostrip(double width, double freq,
                                double& er_eff, double& zl) {
  double h = Substrate.height;
  double t = Substrate.thickness;
  double er = Substrate.er;
  double Wh = width / h;
  double t_norm = t / h;

  // Quasi-static models by Hammerstad
  double w1 = Wh;
  if (t_norm > 1e-100) { // width correction due to metal thickness
    double a = 1.0 / tanh(sqrt(6.517 * Wh)); // coth
    double b = t_norm / PI * log(1.0 + 10.873127 / (t_norm * a * a));
    w1 += b;
    Wh += 0.5 * b * (1.0 + 1.0 / cosh(sqrt(er - 1.0))); // sech
  }

  // Relative effective permittivity
  double a_val = Wh * Wh;
  double b_val = a_val * a_val;
  double er_eff_term = -0.564 * pow((er - 0.9) / (er + 3.0), 0.053);
  er_eff_term *= 1.0 + log((b_val + a_val / 2704.0) / (b_val + 0.432)) / 49.0
                 + log(1.0 + a_val * Wh / 5929.741) / 18.7;
  er_eff = (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * pow(1.0 + 10.0 / Wh, er_eff_term);

  // Characteristic impedance
  double zl_term = 6.0 + 0.2831853 * exp(-pow(30.666 / Wh, 0.7528));
  zl = Z_FIELD / (2.0 * PI) * log(zl_term / Wh + sqrt(1.0 + 4.0 / (Wh * Wh)));

  // Characteristic impedance for width-corrected value
  double a_term = 6.0 + 0.2831853 * exp(-pow(30.666 / w1, 0.7528));
  double zl_w1 = Z_FIELD / (2.0 * PI) * log(a_term / w1 + sqrt(1.0 + 4.0 / (w1 * w1)));

  double correction_factor = zl_w1 / zl;
  zl /= sqrt(er_eff);
  er_eff *= correction_factor * correction_factor;

  // Dispersion models by Kirschning
  double freq_norm = freq * h / 1e6; // normalize frequency to GHz*mm

  // Relative effective permittivity with dispersion
  a_val = 0.0363 * exp(-4.6 * Wh) * (1.0 - exp(-pow(freq_norm / 38.7, 4.97)));
  a_val *= 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));
  a_val = pow((0.1844 + a_val) * freq_norm, 1.5763);
  a_val *= 0.27488 + Wh * (0.6315 + 0.525 / pow(1.0 + 0.0157 * freq_norm, 20.0))
           - 0.065683 * exp(-8.7513 * Wh);
  a_val *= 0.33622 * (1.0 - exp(-0.03442 * er));
  double er_freq = er - (er - er_eff) / (1.0 + a_val);

  // Characteristic impedance with dispersion
  a_val = -0.03891 * pow(er, 1.4);
  b_val = -0.267 * pow(Wh, 7.0);
  double R7 = 1.206 - 0.3144 * exp(a_val) * (1.0 - exp(b_val));

  a_val = 0.016 + pow(0.0514 * er, 4.524);
  b_val = pow(freq_norm / 28.843, 12.0);
  a_val = 5.086 * a_val * b_val / (0.3838 + 0.386 * a_val) / (1.0 + 1.2992 * b_val);
  b_val = -22.2 * pow(Wh, 1.92);
  a_val *= exp(b_val);
  b_val = pow(er - 1.0, 6.0);
  double R9 = a_val * b_val / (1.0 + 10.0 * b_val);

  a_val = 4.766 * exp(-3.228 * pow(Wh, 0.641)); // R3
  a_val = 1.0 + 1.275 * (1.0 - exp(-0.004625 * a_val * pow(er, 1.674)
                                   * pow(freq_norm / 18.365, 2.745))); // R8

  b_val = 0.9408 * pow(er_freq, a_val) - 0.9603; // R13
  b_val /= (0.9408 - R9) * pow(er_eff, a_val) - 0.9603;
  R9 = b_val; // R13 / R14

  a_val = 0.00044 * pow(er, 2.136) + 0.0184; // R10
  a_val *= 0.707 * pow(freq_norm / 12.3, 1.097); // R15
  a_val = exp(-0.026 * pow(freq_norm, 1.15656) - a_val);
  b_val = pow(freq_norm / 19.47, 6.0);
  b_val /= 1.0 + 0.0962 * b_val; // R11
  b_val = 1.0 + 0.0503 * er * er * b_val * (1.0 - exp(-pow(Wh / 15, 6.0))); // R16
  R7 *= (1.0 - 1.1241 * a_val / b_val / (1.0 + 0.00245 * Wh * Wh)); // R17

  zl *= pow(R9, R7);
  er_eff = er_freq;
}

// Microstrip synthesis implementation
bool MicrostripClass::synthesizeMicrostrip(double Z0, double length, double freq) {
  Results.iterations = 0;
  double width = 1.0; // start with 1mm
  double er_eff;
  double Z0_current, Z0_result;

  do {
    // Compute line parameters
    calcMicrostrip(width, freq, er_eff, Z0_current);

    if (fabs(Z0 - Z0_current) < MAX_ERROR) {
      Results.width = width;
      Results.er_eff = er_eff;
      Results.zl = Z0_current;
      Results.length = length/sqrt(er_eff);
      return true;
    }

    double increment = width / 100.0;
    double width_new = width + increment;

    // Compute line parameters with new width
    calcMicrostrip(width_new, freq, er_eff, Z0_result);

    // Newton iteration
    double derivative = (Z0_result - Z0_current) / increment;
    width -= (Z0_current - Z0) / derivative;

    if (width <= 0.0) {
      width = increment;
    }

    Results.iterations++;
  } while (Results.iterations < 150);

  // If we get here, convergence failed
  Results.width = -1.0;
  Results.er_eff = -1.0;
  Results.length = -1;
  return false;
}

// Open end correction implementation
double MicrostripClass::getMicrostripOpen(double Wh, double er, double er_eff) {
  // Model by Kirschning
  double Q4 = pow(er_eff, 0.81);
  double Q5 = pow(Wh, 0.8544);
  double Q1 = 0.434907 * (Q4 + 0.26) / (Q4 - 0.189) * (Q5 + 0.236) / (Q5 + 0.87);

  double Q3 = 1.0 + pow(Wh, 0.371) / (2.358 * er + 1.0);
  Q3 = 1.0 + 0.5274 / pow(er_eff, 0.9236) * atan(0.084 * pow(Wh, 1.9413 / Q3));

  Q4 = 1.0 + 0.0377 * (6.0 - 5.0 * exp(0.036 * (1.0 - er))) * atan(0.067 * pow(Wh, 1.456));

  Q5 = 1.0 - 0.218 * exp(-7.5 * Wh);

  return Q1 * Q3 * Q5 / Q4;
}

// Coupled microstrip synthesis implementation
bool MicrostripClass::synthesizeCoupledMicrostrip(double zl_even, double zl_odd, double freq) {
  Results.iterations = 0;
  double er = Substrate.er;
  double h = Substrate.height;

  // Wheeler formula for single microstrip synthesis (even mode)
  double a = exp(zl_even * sqrt(er + 1.0) / 84.8) - 1.0;
  double Wh_even = 8.0 * sqrt(a * ((7.0 + 4.0 / er) / 11.0) + ((1.0 + 1.0 / er) / 0.81)) / a;

  // Wheeler formula for single microstrip synthesis (odd mode)
  a = exp(zl_odd * sqrt(er + 1.0) / 84.8) - 1.0;
  double Wh_odd = 8.0 * sqrt(a * ((7.0 + 4.0 / er) / 11.0) + ((1.0 + 1.0 / er) / 0.81)) / a;

  // First rough estimation
  double ce = cosh(0.5 * PI * Wh_even);
  double co = cosh(0.5 * PI * Wh_odd);
  double gap = (2.0 / PI) * acosh((ce + co - 2.0) / (co - ce)) * h;
  double width = acosh((ce * co - 1.0) / (co - ce)) / PI - gap / 2.0;
  width *= h;

  double zl_even_current, zl_odd_current, zl_even_w, zl_odd_w, zl_even_g, zl_odd_g;
  double er_eff_e, er_eff_o;

  do {
    // Compute line parameters
    calcCoupledMicrostrip(width, gap, freq, zl_even_current, zl_odd_current, er_eff_e, er_eff_o);

    if (fabs(zl_even - zl_even_current) < MAX_ERROR &&
        fabs(zl_odd - zl_odd_current) < MAX_ERROR) {
      Results.width = width;
      Results.gap = gap;
      Results.er_eff = 0.5 * (er_eff_e + er_eff_o);
      Results.zl_even = zl_even_current;
      Results.zl_odd = zl_odd_current;
      return true;
    }

    // Compute derivatives
    double dw = width / 100.0;
    calcCoupledMicrostrip(width + dw, gap, freq, zl_even_w, zl_odd_w, er_eff_e, er_eff_o);

    double dg = gap / 100.0;
    calcCoupledMicrostrip(width, gap + dg, freq, zl_even_g, zl_odd_g, er_eff_e, er_eff_o);

    // Jacobian matrix
    double j11 = (zl_even_w - zl_even_current) / dw;
    double j12 = (zl_even_g - zl_even_current) / dg;
    double j21 = (zl_odd_w - zl_odd_current) / dw;
    double j22 = (zl_odd_g - zl_odd_current) / dg;

    // Determinant
    double det = j11 * j22 - j12 * j21;

    if (fabs(det) < 1e-15) {
      det = 1e-15; // Avoid division by zero
    }

    // Newton iteration
    width -= (j22 * (zl_even_current - zl_even) - j12 * (zl_odd_current - zl_odd)) / det;
    gap -= (j11 * (zl_odd_current - zl_odd) - j21 * (zl_even_current - zl_even)) / det;

    if (width <= 0.0) width = dw;
        if (gap <= 0.0) gap = dg;

    Results.iterations++;
  } while (Results.iterations < 200);

  // If we get here, convergence failed
  Results.width = -1.0;
  Results.gap = -1.0;
  Results.er_eff = -1.0;
  return false;
}

// Coupled microstrip analysis implementation
void MicrostripClass::calcCoupledMicrostrip(double width, double gap, double freq, double& zl_even, double& zl_odd,double& er_eff_even, double& er_eff_odd) {
  // Simplified calculation - full implementation would be very complex
  double h = Substrate.height;
  double er = Substrate.er;

  double Wh = width / h;
  double g = gap / h;

  // Very rough approximations for demonstration
  er_eff_even = (er + 1) / 2 + (er - 1) / 2 * pow(1 + 12/Wh, -0.5);
  er_eff_odd = er_eff_even * 0.9; // Simplified

  zl_even = 100 * sqrt(er_eff_even) / (Wh + 0.5*g); // Simplified
  zl_odd = 80 * sqrt(er_eff_odd) / (Wh - 0.3*g);   // Simplified
}

// Dispersion helper function implementation
double MicrostripClass::dispersionKirschning(double er, double Wh, double freq, double& er_eff, double& zl) {
  double a, b;

         // Relative effective permittivity
  a = 0.0363 * exp(-4.6 * Wh) * (1.0 - exp(-pow(freq / 38.7, 4.97)));
  a *= 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));
  a = pow((0.1844 + a) * freq, 1.5763);
  a *= 0.27488 + Wh * (0.6315 + 0.525 / pow(1.0 + 0.0157 * freq, 20.0))
       - 0.065683 * exp(-8.7513 * Wh);
  a *= 0.33622 * (1.0 - exp(-0.03442 * er));
  double er_freq = er - (er - er_eff) / (1.0 + a);

         // Characteristic impedance
  a = -0.03891 * pow(er, 1.4);
  b = -0.267 * pow(Wh, 7.0);
  double R7 = 1.206 - 0.3144 * exp(a) * (1.0 - exp(b));

  a = 0.016 + pow(0.0514 * er, 4.524);
  b = pow(freq / 28.843, 12.0);
  a = 5.086 * a * b / (0.3838 + 0.386 * a) / (1.0 + 1.2992 * b);
  b = -22.2 * pow(Wh, 1.92);
  a *= exp(b);
  b = pow(er - 1.0, 6.0);
  double R9 = a * b / (1.0 + 10.0 * b);

  a = 4.766 * exp(-3.228 * pow(Wh, 0.641));
  a = 1.0 + 1.275 * (1.0 - exp(-0.004625 * a * pow(er, 1.674) * pow(freq / 18.365, 2.745)));

  b = 0.9408 * pow(er_freq, a) - 0.9603;
  b /= (0.9408 - R9) * pow(er_eff, a) - 0.9603;
  R9 = b;

  a = 0.00044 * pow(er, 2.136) + 0.0184;
  a *= 0.707 * pow(freq / 12.3, 1.097);
  a = exp(-0.026 * pow(freq, 1.15656) - a);
  b = pow(freq / 19.47, 6.0);
  b /= 1.0 + 0.0962 * b;
  b = 1.0 + 0.0503 * er * er * b * (1.0 - exp(-pow(Wh / 15, 6.0)));
  R7 *= (1.0 - 1.1241 * a / b / (1.0 + 0.00245 * Wh * Wh));

  zl *= pow(R9, R7);
  er_eff = er_freq;
  return R7;
}

// Results printing implementation
void MicrostripClass::printResults(const std::string& title) {
  std::cout << "=== " << title << " ===" << std::endl;
  std::cout << "Width: " << Results.width << " mm" << std::endl;
  if (Results.gap > 0) {
    std::cout << "Gap: " << Results.gap << " mm" << std::endl;
  }
  std::cout << "Effective Permittivity: " << Results.er_eff << std::endl;
  std::cout << "Characteristic Impedance: " << Results.zl << " ohms" << std::endl;
  if (Results.zl_even > 0) {
    std::cout << "Even Mode Impedance: " << Results.zl_even << " ohms" << std::endl;
  }
  if (Results.zl_odd > 0) {
    std::cout << "Odd Mode Impedance: " << Results.zl_odd << " ohms" << std::endl;
  }
  std::cout << "Iterations: " << Results.iterations << std::endl;
  std::cout << "==========================" << std::endl;
}
