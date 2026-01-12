/// @file Microstrip.h
/// @brief Synthesize microstrip lines (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-laterng with this program.  If not, see
/// <https://www.gnu.org/licenses/>.

#include "Microstrip.h"

// Results struct implementation
MicrostripClass::SynthesisResults::SynthesisResults()
    : width(0), gap(0), er_eff(0), zl(0), zl_even(0), zl_odd(0), iterations(0) {
}

// Main implementation of microstrip calculation
void MicrostripClass::calcMicrostrip(double width, double freq, double &er_eff,
                                     double &zl) {
  double h = Substrate.height;
  double t = Substrate.MetalThickness;
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
  er_eff_term *= 1.0 + log((b_val + a_val / 2704.0) / (b_val + 0.432)) / 49.0 +
                 log(1.0 + a_val * Wh / 5929.741) / 18.7;
  er_eff =
      (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * pow(1.0 + 10.0 / Wh, er_eff_term);

  // Characteristic impedance
  double zl_term = 6.0 + 0.2831853 * exp(-pow(30.666 / Wh, 0.7528));
  zl = Z_FIELD / (2.0 * PI) * log(zl_term / Wh + sqrt(1.0 + 4.0 / (Wh * Wh)));

  // Characteristic impedance for width-corrected value
  double a_term = 6.0 + 0.2831853 * exp(-pow(30.666 / w1, 0.7528));
  double zl_w1 =
      Z_FIELD / (2.0 * PI) * log(a_term / w1 + sqrt(1.0 + 4.0 / (w1 * w1)));

  double correction_factor = zl_w1 / zl;
  zl /= sqrt(er_eff);
  er_eff *= correction_factor * correction_factor;

  // Dispersion models by Kirschning
  double freq_norm = freq * h / 1e6; // normalize frequency to GHz*mm

  // Relative effective permittivity with dispersion
  a_val = 0.0363 * exp(-4.6 * Wh) * (1.0 - exp(-pow(freq_norm / 38.7, 4.97)));
  a_val *= 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));
  a_val = pow((0.1844 + a_val) * freq_norm, 1.5763);
  a_val *= 0.27488 +
           Wh * (0.6315 + 0.525 / pow(1.0 + 0.0157 * freq_norm, 20.0)) -
           0.065683 * exp(-8.7513 * Wh);
  a_val *= 0.33622 * (1.0 - exp(-0.03442 * er));
  double er_freq = er - (er - er_eff) / (1.0 + a_val);

  // Characteristic impedance with dispersion
  a_val = -0.03891 * pow(er, 1.4);
  b_val = -0.267 * pow(Wh, 7.0);
  double R7 = 1.206 - 0.3144 * exp(a_val) * (1.0 - exp(b_val));

  a_val = 0.016 + pow(0.0514 * er, 4.524);
  b_val = pow(freq_norm / 28.843, 12.0);
  a_val =
      5.086 * a_val * b_val / (0.3838 + 0.386 * a_val) / (1.0 + 1.2992 * b_val);
  b_val = -22.2 * pow(Wh, 1.92);
  a_val *= exp(b_val);
  b_val = pow(er - 1.0, 6.0);
  double R9 = a_val * b_val / (1.0 + 10.0 * b_val);

  a_val = 4.766 * exp(-3.228 * pow(Wh, 0.641)); // R3
  a_val = 1.0 + 1.275 * (1.0 - exp(-0.004625 * a_val * pow(er, 1.674) *
                                   pow(freq_norm / 18.365, 2.745))); // R8

  b_val = 0.9408 * pow(er_freq, a_val) - 0.9603; // R13
  b_val /= (0.9408 - R9) * pow(er_eff, a_val) - 0.9603;
  R9 = b_val; // R13 / R14

  a_val = 0.00044 * pow(er, 2.136) + 0.0184;     // R10
  a_val *= 0.707 * pow(freq_norm / 12.3, 1.097); // R15
  a_val = exp(-0.026 * pow(freq_norm, 1.15656) - a_val);
  b_val = pow(freq_norm / 19.47, 6.0);
  b_val /= 1.0 + 0.0962 * b_val; // R11
  b_val =
      1.0 + 0.0503 * er * er * b_val * (1.0 - exp(-pow(Wh / 15, 6.0))); // R16
  R7 *= (1.0 - 1.1241 * a_val / b_val / (1.0 + 0.00245 * Wh * Wh));     // R17

  zl *= pow(R9, R7);
  er_eff = er_freq;
}

// Microstrip synthesis implementation
bool MicrostripClass::synthesizeMicrostrip(double Z0, double length,
                                           double freq) {
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
      Results.length = length / sqrt(er_eff);
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
  double Q1 =
      0.434907 * (Q4 + 0.26) / (Q4 - 0.189) * (Q5 + 0.236) / (Q5 + 0.87);

  double Q3 = 1.0 + pow(Wh, 0.371) / (2.358 * er + 1.0);
  Q3 = 1.0 + 0.5274 / pow(er_eff, 0.9236) * atan(0.084 * pow(Wh, 1.9413 / Q3));

  Q4 = 1.0 + 0.0377 * (6.0 - 5.0 * exp(0.036 * (1.0 - er))) *
                 atan(0.067 * pow(Wh, 1.456));

  Q5 = 1.0 - 0.218 * exp(-7.5 * Wh);

  return Q1 * Q3 * Q5 / Q4;
}

// Coupled microstrip synthesis implementation
bool MicrostripClass::synthesizeCoupledMicrostrip(double zl_even, double zl_odd,
                                                  double length, double freq) {
  Results.iterations = 0;
  double er = Substrate.er;
  double h = Substrate.height;

  // Wheeler formula for single microstrip synthesis (even mode)
  double a = exp(zl_even * sqrt(er + 1.0) / 84.8) - 1.0;
  double Wh_even =
      8.0 * sqrt(a * ((7.0 + 4.0 / er) / 11.0) + ((1.0 + 1.0 / er) / 0.81)) / a;

  // Wheeler formula for single microstrip synthesis (odd mode)
  a = exp(zl_odd * sqrt(er + 1.0) / 84.8) - 1.0;
  double Wh_odd =
      8.0 * sqrt(a * ((7.0 + 4.0 / er) / 11.0) + ((1.0 + 1.0 / er) / 0.81)) / a;

  // First rough estimation
  double ce = cosh(0.5 * PI * Wh_even);
  double co = cosh(0.5 * PI * Wh_odd);
  double gap = (2.0 / PI) * acosh((ce + co - 2.0) / (co - ce)) * h;
  double width = acosh((ce * co - 1.0) / (co - ce)) / PI - gap / 2.0;
  width *= h;

  double zl_even_current, zl_odd_current, zl_even_w, zl_odd_w, zl_even_g,
      zl_odd_g;
  double er_eff_e, er_eff_o;

  do {
    // Compute line parameters
    calcCoupledMicrostrip(width, gap, freq, zl_even_current, zl_odd_current,
                          er_eff_e, er_eff_o);

    if (fabs(zl_even - zl_even_current) < MAX_ERROR &&
        fabs(zl_odd - zl_odd_current) < MAX_ERROR) {
      Results.width = width;
      Results.gap = gap;

      // Calculate average effective permittivity
      Results.er_eff = 0.5 * (er_eff_e + er_eff_o);

      Results.zl_even = zl_even_current;
      Results.zl_odd = zl_odd_current;

      // Physical length of the coupled lines
      // For coupled lines, we use the average of even and odd mode effective
      // permittivities
      double er_eff_avg = Results.er_eff;

      // Physical length = Electrical length / sqrt(er_eff_avg)
      // This accounts for the wavelength shortening due to the dielectric
      // substrate
      Results.length = length / sqrt(er_eff_avg);

      return true;
    }

    // Compute derivatives
    double dw = width / 100.0;
    calcCoupledMicrostrip(width + dw, gap, freq, zl_even_w, zl_odd_w, er_eff_e,
                          er_eff_o);

    double dg = gap / 100.0;
    calcCoupledMicrostrip(width, gap + dg, freq, zl_even_g, zl_odd_g, er_eff_e,
                          er_eff_o);

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
    width -=
        (j22 * (zl_even_current - zl_even) - j12 * (zl_odd_current - zl_odd)) /
        det;
    gap -=
        (j11 * (zl_odd_current - zl_odd) - j21 * (zl_even_current - zl_even)) /
        det;

    if (width <= 0.0) {
      width = dw;
    }
    if (gap <= 0.0) {
      gap = dg;
    }

    Results.iterations++;
  } while (Results.iterations < 200);

  // If we get here, convergence failed
  Results.width = -1.0;
  Results.gap = -1.0;
  Results.er_eff = -1.0;
  Results.length = -1.0;
  return false;
}

// Coupled microstrip analysis implementation
void MicrostripClass::calcCoupledMicrostrip(double ms_width, double g,
                                            double freq, double &zl_e,
                                            double &zl_o, double &er_eff_e,
                                            double &er_eff_o) {
  double Wh_o, a, b, c, d;
  double Wh_e = ms_width;
  double h = Substrate.height;
  double t = Substrate.MetalThickness;
  double er = Substrate.er;

  // Width correction due to metal thickness
  a = 1.0;
  if (t > 1e-20) {
    if (Wh_e > (h / 2.0 / PI)) {
      if (h > (t * 4.0 * PI)) {
        a = t / PI * (1.0 + log(2.0 * h / t));
      }
    } else {
      if (Wh_e > (t * 2.0)) {
        a = t / PI * (1.0 + log(4.0 * PI * Wh_e / t));
      }
    }
  }
  if (g > 20.0 * t) {
    b = 2.0 * t / g / er;
    a *= 1.0 - 0.5 * exp(-0.69 * a / b);
  } else {
    a = b = 0.0;
  }
  Wh_e = (Wh_e + a) / h;
  Wh_o = Wh_e + b;

  g /= h;
  double fn = freq / 1e6 * h;

  // Static zero-thickness relative permittivity by Hammerstad and Jensen
  double er_e, er_o;
  a = Wh_e * Wh_e;
  b = a * a;
  er_e = -0.564 * pow((er - 0.9) / (er + 3.0), 0.053);
  er_e *= 1.0 + log((b + a / 2704.0) / (b + 0.432)) / 49.0 +
          log(1.0 + a * Wh_e / 5929.741) / 18.7;
  er_e = (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * pow(1.0 + 10.0 / Wh_e, er_e);

  a = Wh_o * Wh_o;
  b = a * a;
  er_o = -0.564 * pow((er - 0.9) / (er + 3.0), 0.053);
  er_o *= 1.0 + log((b + a / 2704.0) / (b + 0.432)) / 49.0 +
          log(1.0 + a * Wh_o / 5929.741) / 18.7;
  er_o = (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * pow(1.0 + 10.0 / Wh_o, er_o);

  // Static effective permittivity of even mode
  er_eff_e = 0.5 * (er + 1.0);
  b = Wh_e * (20.0 + g * g) / (10.0 + g * g) + g * exp(-g);
  a = -0.564 * pow((er - 0.9) / (er + 3.0), 0.053);
  a *= 1.0 + log(b * b * (b * b + 3.698e-4) / (b * b * b * b + 0.432)) / 49.0 +
       log(1.0 + pow(b / 18.1, 3.0)) / 18.7;
  er_eff_e += 0.5 * (er - 1.0) * pow(1.0 + 10.0 / b, a);

  // Static effective permittivity of odd mode
  b = 0.747 * er / (0.15 + er);
  b -= (b - 0.207) * exp(-0.414 * Wh_o);
  b *= pow(g, 0.593 + 0.694 * exp(-0.562 * Wh_o));
  a = 0.7287 * (er_o - 0.5 * (er + 1.0)) * (1.0 - exp(-0.179 * Wh_o));
  er_eff_o = er_o + (0.5 * (er + 1.0) + a - er_o) * exp(-b);

  // Static zero-thickness characteristic impedance by Hammerstad and Jensen
  zl_e = 6.0 + 0.2831853 * exp(-pow(30.666 / Wh_e, 0.7528));
  zl_e = Z_FIELD / sqrt(er_e) / 2.0 / PI *
         log(zl_e / Wh_e + sqrt(1.0 + 4.0 / Wh_e / Wh_e));

  // Static characteristic impedance of even mode
  b = 1.0 + 0.7519 * g + 0.189 * pow(g, 2.31); // Q2
  c = 0.1975 + pow(16.6 + pow(8.4 / g, 6.0), -0.387) +
      log(pow(g, 10.0) / (1.0 + pow(g / 3.4, 10.0))) / 241.0;         // Q3
  a = b * (exp(-g) * pow(Wh_e, c) + (2.0 - exp(-g)) * pow(Wh_e, -c)); // 1/Q4
  a = 1.739 * pow(Wh_e, 0.194) / a;                                   // Q1
  zl_e *= sqrt(er_e / er_eff_e) / (1.0 - zl_e / Z_FIELD * sqrt(er_e) * a);

  // Static zero-thickness characteristic impedance by Hammerstad and Jensen
  zl_o = 6.0 + 0.2831853 * exp(-pow(30.666 / Wh_o, 0.7528));
  zl_o = Z_FIELD / sqrt(er_o) / 2.0 / PI *
         log(zl_o / Wh_o + sqrt(1.0 + 4.0 / Wh_o / Wh_o));

  // Static characteristic impedance of odd mode
  a = exp(-6.5 - 0.95 * log(g) - pow(g / 0.15, 5.0));          // Q8
  a = a + 1.0 / 16.5;                                          // Q9
  a *= log((10.0 + 190.0 * g * g) / (1.0 + 82.3 * g * g * g)); // log(Q7)
  a = pow(Wh_o, -a);                                           // Q10_1
  a *= 0.2305 + log(pow(g, 10.0) / (1.0 + pow(g / 5.8, 10.0))) / 281.3 +
       log(1.0 + 0.598 * pow(g, 1.154)) / 5.1;                       // Q6
  a = exp(log(Wh_o) * a);                                            // Q10_2
  a *= 1.794 + 1.14 * log(1.0 + 0.638 / (g + 0.517 * pow(g, 2.43))); // Q5
  a /= b;                                                            // Q10_3
  a = 1.739 * pow(Wh_o, 0.194) / b /
          (exp(-g) * pow(Wh_o, c) + (2.0 - exp(-g)) * pow(Wh_o, -c)) -
      a; // Q4/Q10
  zl_o *= sqrt(er_o / er_eff_o) / (1.0 - zl_o / Z_FIELD * sqrt(er_o) * a);

  er_e = er_eff_e;
  er_o = er_eff_o;

  // Dispersive relative permittivity of even mode
  a = 0.0363 * exp(-4.6 * Wh_e) * (1.0 - exp(-pow(fn / 38.7, 4.97))); // P3
  d = 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));              // P4
  a *= d;
  b = 0.334 * exp(-3.3 * pow(er / 15.0, 3.0)) + 0.746; // P5
  b *= exp(-pow(fn / 18.0, 0.368));                    // P6
  b = 1.0 + 4.069 * b * pow(g, 0.479) *
                exp(-1.347 * pow(g, 0.595) - 0.17 * pow(g, 2.5)); // P7
  a = pow((a + 0.1844 * b) * fn, 1.5763);                         // Fe
  a *= 0.27488 + (0.6315 + 0.525 / pow(1.0 + 0.0157 * fn, 20.0)) * Wh_e -
       0.065683 * exp(-8.7513 * Wh_e);      // P1
  c = 0.33622 * (1.0 - exp(-0.03442 * er)); // P2
  er_eff_e = er - (er - er_eff_e) / (1.0 + a * c);

  // Dispersive relative permittivity of odd mode
  a = 0.7168 * (1.0 + 1.076 / (1.0 + 0.0576 * (er - 1.0))); // P8
  a -= 0.7913 * (1.0 - exp(-pow(fn / 20.0, 1.424))) *
       atan(2.481 * pow(er / 8.0, 0.946));                           // P9
  a += (1.0 - a) / (1.0 + 1.183 * pow(Wh_o, 1.376));                 // P12
  b = 0.242 * pow(er - 1.0, 0.55);                                   // P10
  b = -1.695 * b / (0.414 + 1.605 * b);                              // P13
  a *= exp(b * pow(g, 1.092));                                       // P15_1
  a /= 0.8928 + 0.1072 * (1.0 - exp(-0.42 * pow(fn / 20.0, 3.215))); // P14
  b = 0.6366 * (exp(-0.3401 * fn) - 1.0) *
      atan(1.263 * pow(Wh_o / 3.0, 1.629));                           // P11
  b = fabs(1.0 - 0.8928 * (1.0 + b) * a);                             // P15_2
  a = 0.0363 * exp(-4.6 * Wh_o) * (1.0 - exp(-pow(fn / 38.7, 4.97))); // P3
  a = pow((a * d + 0.1844) * fn * b, 1.5763);                         // Fo
  a *= 0.27488 + (0.6315 + 0.525 / pow(1.0 + 0.0157 * fn, 20.0)) * Wh_o -
       0.065683 * exp(-8.7513 * Wh_o); // P1
  er_eff_o = er - (er - er_eff_o) / (1.0 + a * c);

  // Dispersive characteristic impedance of even mode
  a = 0.893 * (1.0 - 0.3 / (1.0 + 0.7 * (er - 1.0))); // Q11
  a = -2.121 * (pow(fn / 20.0, 4.91) / (1.0 + a * pow(fn / 20.0, 4.91))) *
      exp(-2.87 * g) * pow(g, 0.902);                                  // Q12
  b = 1.0 + 0.038 * pow(er / 8.0, 5.1);                                // Q13
  c = 1.0 + 1.203 * pow(er / 15.0, 4.0) / (1.0 + pow(er / 15.0, 4.0)); // Q14
  b = 1.887 * exp(-1.5 * pow(g, 0.84)) * pow(g, c) /
      (1.0 + 0.41 * pow(fn / 15.0, 3.0) * pow(Wh_e, 2.0 / b) /
                 (0.125 + pow(Wh_e, 1.626 / b)));          // Q15
  a += b * (1.0 + 9.0 / (1.0 + 0.403 * pow(er - 1.0, 2))); // Q16
  a -= 0.394 * (1.0 - exp(-1.47 * pow(Wh_e / 7.0, 0.672))) *
       (1.0 - exp(-4.25 * pow(fn / 20.0, 1.87))); // Q17
  a += 0.61 * (1.0 - exp(-2.13 * pow(Wh_e / 8.0, 1.593))) /
       (1.0 + 6.544 * pow(g, 4.17)); // Q18
  b = 0.21 * g * g * g * g /
      ((1.0 + 0.18 * pow(g, 4.9)) * (1.0 + 0.1 * Wh_e * Wh_e) *
       (1.0 + pow(fn / 24.0, 3.0)));                      // Q19
  a += b * (0.09 + 1.0 / (1.0 + 0.1 * pow(er - 1, 2.7))); // Q20

  b = 4.766 * exp(-3.228 * pow(Wh_e, 0.641)); // pe
  a += 1.275 * (1.0 -
                exp(-0.004625 * b * pow(er, 1.674) * pow(fn / 18.365, 2.745))) +
       1.0; // Ce

  b = fabs(1.0 - 42.54 * pow(g, 0.133) * exp(-0.812 * g) * pow(Wh_e, 2.5) /
                     (1.0 + 0.033 * pow(Wh_e, 2.5))); // Q21
  b = 0.016 + pow(0.0514 * er * b, 4.524);            // qe
  c = pow(fn / 28.843, 12);                           // re
  b = 5.086 * b * c / (0.3838 + 0.386 * b) *
      (exp(-22.2 * pow(Wh_e, 1.92)) / (1.0 + 1.2992 * c)) *
      (pow(er - 1.0, 6.0) / (1.0 + 10.0 * pow(er - 1.0, 6.0))); // de

  d = 1;
  c = er_e;
  d = dispersionKirschning(er, Wh_e, fn, c, d);
  a = (0.9408 * pow(c, a) - 0.9603) / ((0.9408 - b) * pow(er_e, a) - 0.9603);
  zl_e *= pow(a, d);

  // Dispersive characteristic impedance of odd mode
  d = zl_o;
  c = er_o;
  dispersionKirschning(er, Wh_o, fn, c, d);

  a = pow((er - 1.0) / 13.0, 12.0);
  a = 30.0 - 22.2 * a / (1.0 + 3.0 * a);                                // Q26
  a -= 15.16 / (1.0 + 0.196 * pow(er - 1.0, 2.0));                      // Q29
  a = 0.925 * pow(fn / a, 1.536) / (1.0 + 0.3 * pow(fn / 30.0, 1.536)); // Q22
  zl_o *= pow(er_eff_o / er_o, a);

  a = pow(er - 1.0, 1.5);
  a = 0.4 * pow(g, 0.84) * (1.0 + 2.5 * a / (5.0 + a)); // Q27
  a = 1.0 + 0.005 * fn * a /
                ((1.0 + 0.812 * pow(fn / 15.0, 1.9)) *
                 (1.0 + 0.025 * Wh_o * Wh_o)); // Q23
  zl_o -= d * a;

  a = (er - 1.0) * (er - 1.0);
  a = (0.3 * fn * fn / (10.0 + fn * fn)) * (1.0 + 2.333 * a / (5.0 + a)); // Q25
  b = pow(er - 1.0, 3.0);
  b = 0.149 * b / (94.5 + 0.038 * b); // Q28
  c = pow(Wh_o, 0.894);
  b *= 2.506 * c * pow((1.0 + 1.3 * Wh_o) * fn / 99.25, 4.29) /
       (3.575 + c); // Q24
  zl_o = d + zl_o / (1.0 + b + pow(0.46 * g, 2.2) * a);
}

// Dispersion helper function implementation
double MicrostripClass::dispersionKirschning(double er, double Wh, double freq,
                                             double &er_eff, double &zl) {
  double a, b;

  // Relative effective permittivity
  a = 0.0363 * exp(-4.6 * Wh) * (1.0 - exp(-pow(freq / 38.7, 4.97)));
  a *= 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));
  a = pow((0.1844 + a) * freq, 1.5763);
  a *= 0.27488 + Wh * (0.6315 + 0.525 / pow(1.0 + 0.0157 * freq, 20.0)) -
       0.065683 * exp(-8.7513 * Wh);
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
  a = 1.0 + 1.275 * (1.0 - exp(-0.004625 * a * pow(er, 1.674) *
                               pow(freq / 18.365, 2.745)));

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
void MicrostripClass::printResults(const std::string &title) {
  std::cout << "=== " << title << " ===" << std::endl;
  std::cout << "Width: " << Results.width << " mm" << std::endl;
  if (Results.gap > 0) {
    std::cout << "Gap: " << Results.gap << " mm" << std::endl;
  }
  std::cout << "Effective Permittivity: " << Results.er_eff << std::endl;
  std::cout << "Characteristic Impedance: " << Results.zl << " ohms"
            << std::endl;
  if (Results.zl_even > 0) {
    std::cout << "Even Mode Impedance: " << Results.zl_even << " ohms"
              << std::endl;
  }
  if (Results.zl_odd > 0) {
    std::cout << "Odd Mode Impedance: " << Results.zl_odd << " ohms"
              << std::endl;
  }
  std::cout << "Iterations: " << Results.iterations << std::endl;
  std::cout << "==========================" << std::endl;
}
