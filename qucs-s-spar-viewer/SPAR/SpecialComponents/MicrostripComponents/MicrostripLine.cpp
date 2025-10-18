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

#include "./../../SParameterCalculator.h"

void SParameterCalculator::addMicrostripLineToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {
  // Extract microstrip parameters
  int node1 = comp.nodes[0];
  int node2 = comp.nodes[1];

  double W = comp.value.value("Width");        // Width in meters
  double L = comp.value.value("Length");       // Length in meters
  double h = comp.value.value("h");            // Substrate height in meters
  double er = comp.value.value("er");          // Relative permittivity
  double t = comp.value.value("th", 0.0);      // Conductor thickness (optional)
  double tand = comp.value.value("tand", 0.0); // Loss tangent (optional)
  double rho = comp.value.value("rho", 1e-10); // Surface Resistivity (optional)

  // Calculate propagation characteristics
  double alpha, beta, zl, ereff;
  calcMicrostripPropagation(W, h, er, t, tand, rho, frequency, alpha, beta, zl,
                            ereff);

  double z0 = 50.0;   // System impedance - make sure this matches your system
  double z = zl / z0; // normalized characteristic impedance
  double y = 1.0 / z; // normalized admittance

  Complex gamma(alpha, beta);
  Complex gl = gamma * L;

  // Complex characteristic impedance (can include loss effects if needed)
  Complex Z0c(zl, 0.0);

  // Calculate hyperbolic functions
  Complex sinh_gl = sinh(gl);
  Complex cosh_gl = cosh(gl);

  // Handle numerical issues for very small arguments
  if (abs(gl) < 1e-10) {
    sinh_gl = gl;
    cosh_gl = Complex(1.0, 0.0);
  }

  // Calculate exactly as in reference: n = 2*cosh(gl) + (z+y)*sinh(gl)
  Complex n = 2.0 * cosh_gl + (z + y) * sinh_gl;

  // Avoid division by zero
  if (abs(n) < 1e-15) {
    return; // Skip this component if singular
  }

  // Calculate S-parameters as in reference
  // These results may be used to check with qucsator-RF
  // Complex s11 = (z - y) * sinh_gl / n;
  // Complex s21 = 2.0 / n;

  // Y-parameters for lossy transmission line:
  // Y11 = Y22 = (1/Z0) * coth(gamma*L)
  // Y12 = Y21 = -(1/Z0) * csch(gamma*L)
  Complex coth_gl = cosh_gl / sinh_gl;
  Complex csch_gl = 1.0 / sinh_gl;

  Complex y11 = coth_gl / Z0c;
  Complex y12 = -csch_gl / Z0c;

  Complex y22 = y11;
  Complex y21 = y12;

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

void SParameterCalculator::calcMicrostripPropagation(
    double W, double h, double er, double t, double tand, double rho,
    double frequency, double &alpha, double &beta, double &zl, double &ereff) {
  // Local variables
  double ac, ad;
  double ZlEff, ErEff, WEff, ZlEffFreq, ErEffFreq;

  // Default model names
  string SModel = "Hammerstad";
  string DModel = "Kirschning";

  // Quasi-static effective dielectric constant and impedance
  analyseQuasiStatic(W, h, t, er, SModel, ZlEff, ErEff, WEff);

  // Analyse dispersion
  analyseDispersion(W, h, er, ZlEff, ErEff, frequency, DModel, ZlEffFreq,
                    ErEffFreq);

  // Analyse losses
  analyseLoss(W, t, er, rho, 0.0, tand, ZlEff, ZlEff, ErEff, frequency,
              "Hammerstad", ac, ad);

  // Set output values
  zl = ZlEffFreq;
  ereff = ErEffFreq;
  alpha = ac + ad;
  beta = sqrt(ErEffFreq) * 2 * M_PI * frequency / C0;
}

void SParameterCalculator::analyseQuasiStatic(double W, double h, double t,
                                              double er, const string &Model,
                                              double &ZlEff, double &ErEff,
                                              double &WEff) {
  double z, e;

  // Default values
  e = er;
  z = Z0;
  WEff = W;

  if (Model == "Hammerstad") {
    double a, b, du1, du, u, ur, u1, zr, z1;

    u = W / h; // normalized width
    t = t / h; // normalized thickness

    // Compute strip thickness effect
    if (t != 0) {
      du1 = t / M_PI *
            log(1 + 4 * euler / t / pow(1.0 / tanh(sqrt(6.517 * u)), 2));
    } else {
      du1 = 0;
    }

    du = du1 * (1 + 1.0 / cosh(sqrt(er - 1))) / 2;
    u1 = u + du1;
    ur = u + du;
    WEff = ur * h;

    // Compute impedances for homogeneous medium
    Hammerstad_zl(ur, zr);
    Hammerstad_zl(u1, z1);

    // Compute effective dielectric constant
    Hammerstad_ab(ur, er, a, b);
    Hammerstad_er(ur, er, a, b, e);

    // Compute final characteristic impedance and dielectric constant
    z = zr / sqrt(e);
    e = e * pow(z1 / zr, 2);
  }
  // Add other models (Wheeler, Schneider) as needed

  ZlEff = z;
  ErEff = e;
}

void SParameterCalculator::analyseDispersion(
    double W, double h, double er, double ZlEff, double ErEff, double frequency,
    const string &Model, double &ZlEffFreq, double &ErEffFreq) {
  double e, z;

  // Default values
  z = ZlEffFreq = ZlEff;
  e = ErEffFreq = ErEff;

  if (Model == "Kirschning") {
    double u = W / h, fn = frequency * h / 1e6;
    double r17;

    // Dispersion of dielectric constant
    Kirschning_er(u, fn, er, ErEff, e);

    // Dispersion of characteristic impedance
    Kirschning_zl(ErEff, e, ZlEff, r17, z);
  }
  // Add other dispersion models as needed

  ZlEffFreq = z;
  ErEffFreq = e;
}

void SParameterCalculator::analyseLoss(double W, double t, double er,
                                       double rho, double D, double tand,
                                       double ZlEff1, double ZlEff2,
                                       double ErEff, double frequency,
                                       const string &Model, double &ac,
                                       double &ad) {

  ac = ad = 0;

  // HAMMERSTAD and JENSEN
  if (Model == "Hammerstad") {
    double Rs, ds, l0, Kr, Ki;

    // Conductor losses
    if (t != 0.0) {
      Rs = sqrt(M_PI * frequency * MU0 * rho); // skin resistance
      ds = rho / Rs;                           // skin depth

      // Current distribution factor
      Ki = exp(-1.2 * pow((ZlEff1 + ZlEff2) / 2.0 / Z0, 0.7));

      // D is RMS surface roughness
      Kr = 1.0 + (2.0 / M_PI) * atan(1.4 * pow(D / ds, 2.0));

      ac = Rs / (ZlEff1 * W) * Ki * Kr;
    }

    // Dielectric losses
    l0 = C0 / frequency;
    ad = M_PI * er / (er - 1.0) * (ErEff - 1.0) / sqrt(ErEff) * tand / l0;
  }
}

// Helper function implementations (simplified versions)
void SParameterCalculator::Hammerstad_ab(double u, double er, double &a,
                                         double &b) {
  a = 1 + log((pow(u, 4) + pow(u / 52, 2)) / (pow(u, 4) + 0.432)) / 49 +
      log(1 + pow(u / 18.1, 3)) / 18.7;
  b = 0.564 * pow((er - 0.9) / (er + 3), 0.053);
}

void SParameterCalculator::Hammerstad_er(double u, double er, double a,
                                         double b, double &e) {
  e = (er + 1) / 2 + (er - 1) / 2 * pow(1 + 10 / u, -a * b);
}

void SParameterCalculator::Hammerstad_zl(double u, double &zl) {
  double fu = 6 + (2 * M_PI - 6) * exp(-pow(30.666 / u, 0.7528));
  zl = Z0 / 2 / M_PI * log(fu / u + sqrt(1 + pow(2 / u, 2)));
}

void SParameterCalculator::Kirschning_er(double u, double fn, double er,
                                         double ErEff, double &ErEffFreq) {
  double p, p1, p2, p3, p4;
  p1 = 0.27488 + (0.6315 + 0.525 / pow(1. + 0.0157 * fn, 20.)) * u -
       0.065683 * exp(-8.7513 * u);
  p2 = 0.33622 * (1 - exp(-0.03442 * er));
  p3 = 0.0363 * exp(-4.6 * u) * (1 - exp(-pow(fn / 38.7, 4.97)));
  p4 = 1 + 2.751 * (1 - exp(-pow(er / 15.916, 8.)));
  p = p1 * p2 * pow((0.1844 + p3 * p4) * fn, 1.5763);
  ErEffFreq = er - (er - ErEff) / (1 + p);
}

void SParameterCalculator::Kirschning_zl(double ErEff, double ErEffFreq,
                                         double ZlEff, double &r17,
                                         double &ZlEffFreq) {
  // Simplified implementation - full implementation would include all r1-r17
  // calculations
  r17 = 1.0; // Default value for now
  ZlEffFreq = ZlEff * sqrt(ErEff / ErEffFreq);
}
