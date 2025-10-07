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

void SParameterCalculator::addMicrostripCoupledLinesToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp) {
  // Extract microstrip coupled lines parameters
  int node1 = comp.nodes[0];  // Port 1 of line 1
  int node2 = comp.nodes[1];  // Port 2 of line 1
  int node3 = comp.nodes[2];  // Port 1 of line 2
  int node4 = comp.nodes[3];  // Port 2 of line 2

  double W = comp.value.value("Width");       // Width of each line in meters
  double S = comp.value.value("Spacing");     // Spacing between lines in meters
  double L = comp.value.value("Length");      // Length in meters
  double h = comp.value.value("h");           // Substrate height in meters
  double er = comp.value.value("er");         // Relative permittivity
  double t = comp.value.value("th", 0.0);     // Conductor thickness (optional)
  double tand = comp.value.value("tand", 0.0); // Loss tangent (optional)
  double rho = comp.value.value("rho", 1e-10); // Surface Resistivity (optional)

  // Calculate propagation characteristics for coupled lines
  double alpha_e, beta_e, zl_e, ereff_e;  // Even mode
  double alpha_o, beta_o, zl_o, ereff_o;  // Odd mode
  calcMicrostripCoupledPropagation(W, S, L, h, er, t, tand, rho, frequency,
                                   alpha_e, beta_e, zl_e, ereff_e,
                                   alpha_o, beta_o, zl_o, ereff_o);

  double z0 = 50.0;  // System impedance

  // Even mode calculations
  Complex gamma_e(alpha_e, beta_e);
  Complex gl_e = gamma_e * L;
  Complex sinh_gl_e = sinh(gl_e);
  Complex cosh_gl_e = cosh(gl_e);
  
  if (abs(gl_e) < 1e-10) {
    sinh_gl_e = gl_e;
    cosh_gl_e = Complex(1.0, 0.0);
  }

  Complex coth_gl_e = cosh_gl_e / sinh_gl_e;
  Complex csch_gl_e = 1.0 / sinh_gl_e;
  Complex Z0c_e(zl_e, 0.0);

  // Odd mode calculations
  Complex gamma_o(alpha_o, beta_o);
  Complex gl_o = gamma_o * L;
  Complex sinh_gl_o = sinh(gl_o);
  Complex cosh_gl_o = cosh(gl_o);
  
  if (abs(gl_o) < 1e-10) {
    sinh_gl_o = gl_o;
    cosh_gl_o = Complex(1.0, 0.0);
  }

  Complex coth_gl_o = cosh_gl_o / sinh_gl_o;
  Complex csch_gl_o = 1.0 / sinh_gl_o;
  Complex Z0c_o(zl_o, 0.0);

  // Calculate Y-parameters from even and odd mode impedances
  // For coupled lines: Y = 1/2 * (Y_even + Y_odd)
  Complex y11_e = coth_gl_e / Z0c_e;
  Complex y12_e = -csch_gl_e / Z0c_e;
  Complex y11_o = coth_gl_o / Z0c_o;
  Complex y12_o = -csch_gl_o / Z0c_o;

  // Transform to coupled line Y-parameters
  Complex y11 = 0.5 * (y11_e + y11_o);
  Complex y12 = 0.5 * (y12_e + y12_o);
  Complex y13 = 0.5 * (y11_e - y11_o);
  Complex y14 = 0.5 * (y12_e - y12_o);

  Complex y21 = y12;
  Complex y22 = y11;
  Complex y23 = y14;
  Complex y24 = y13;

  Complex y31 = y13;
  Complex y32 = y23;
  Complex y33 = y11;
  Complex y34 = y12;

  Complex y41 = y14;
  Complex y42 = y24;
  Complex y43 = y34;
  Complex y44 = y22;

  // Add to admittance matrix
  if (node1 > 0) {
    Y[node1-1][node1-1] += y11;
    if (node2 > 0) Y[node1-1][node2-1] += y12;
    if (node3 > 0) Y[node1-1][node3-1] += y13;
    if (node4 > 0) Y[node1-1][node4-1] += y14;
  }

  if (node2 > 0) {
    if (node1 > 0) Y[node2-1][node1-1] += y21;
    Y[node2-1][node2-1] += y22;
    if (node3 > 0) Y[node2-1][node3-1] += y23;
    if (node4 > 0) Y[node2-1][node4-1] += y24;
  }

  if (node3 > 0) {
    if (node1 > 0) Y[node3-1][node1-1] += y31;
    if (node2 > 0) Y[node3-1][node2-1] += y32;
    Y[node3-1][node3-1] += y33;
    if (node4 > 0) Y[node3-1][node4-1] += y34;
  }

  if (node4 > 0) {
    if (node1 > 0) Y[node4-1][node1-1] += y41;
    if (node2 > 0) Y[node4-1][node2-1] += y42;
    if (node3 > 0) Y[node4-1][node3-1] += y43;
    Y[node4-1][node4-1] += y44;
  }
}

void SParameterCalculator::calcMicrostripCoupledPropagation(
    double W, double S, double L, double h, double er, double t,
    double tand, double rho, double frequency,
    double& alpha_e, double& beta_e, double& zl_e, double& ereff_e,
    double& alpha_o, double& beta_o, double& zl_o, double& ereff_o) {

  // Local variables
  double ac_e, ad_e, ac_o, ad_o;
  double ZlEff_e, ErEff_e, ZlEffFreq_e, ErEffFreq_e;
  double ZlEff_o, ErEff_o, ZlEffFreq_o, ErEffFreq_o;
  double WEff;

  string SModel = "Hammerstad";
  string DModel = "Kirschning";

  // Calculate even mode characteristics
  analyseQuasiStaticCoupled(W, S, h, t, er, SModel, true, ZlEff_e, ErEff_e, WEff);
  analyseDispersionCoupled(W, S, h, er, ZlEff_e, ErEff_e, frequency, DModel, true,
                          ZlEffFreq_e, ErEffFreq_e);
  analyseLossCoupled(W, S, t, er, rho, 0.0, tand, ZlEff_e, ZlEff_e, ErEff_e,
                    frequency, "Hammerstad", true, ac_e, ad_e);

  // Calculate odd mode characteristics
  analyseQuasiStaticCoupled(W, S, h, t, er, SModel, false, ZlEff_o, ErEff_o, WEff);
  analyseDispersionCoupled(W, S, h, er, ZlEff_o, ErEff_o, frequency, DModel, false,
                          ZlEffFreq_o, ErEffFreq_o);
  analyseLossCoupled(W, S, t, er, rho, 0.0, tand, ZlEff_o, ZlEff_o, ErEff_o,
                    frequency, "Hammerstad", false, ac_o, ad_o);

  // Set output values for even mode
  zl_e = ZlEffFreq_e;
  ereff_e = ErEffFreq_e;
  alpha_e = ac_e + ad_e;
  beta_e = sqrt(ErEffFreq_e) * 2 * M_PI * frequency / C0;

  // Set output values for odd mode
  zl_o = ZlEffFreq_o;
  ereff_o = ErEffFreq_o;
  alpha_o = ac_o + ad_o;
  beta_o = sqrt(ErEffFreq_o) * 2 * M_PI * frequency / C0;
}

void SParameterCalculator::analyseQuasiStaticCoupled(
    double W, double S, double h, double t, double er, const string& Model,
    bool evenMode, double& ZlEff, double& ErEff, double& WEff) {

  double z, e;

  // Default values
  e = er;
  z = Z0;
  WEff = W;

  if (Model == "Hammerstad") {
    double a, b, u, ur, zr;
    double ae, be, ao, bo;
    double q1, q2, q3, q4, q5, q6, q7, q8, q9, q10;

    u = W / h;
    t = t / h;

    // Calculate even and odd mode parameters
    Hammerstad_ab(u, er, a, b);
    
    if (evenMode) {
      // Even mode calculation
      q1 = 0.8695 * pow(u, 0.194);
      q2 = 1 + 0.7519 * S / h + 0.189 * pow(S / h, 2.31);
      q3 = 0.1975 + pow(16.6 + pow(8.4 / (S / h), 6), -0.387) +
           log(pow(S / h, 10) / (1 + pow(S / h / 3.4, 10))) / 241;
      q4 = 2 * q1 / q2 / (exp(-S / h) * pow(u, q3) + (2 - exp(-S / h)) * pow(u, -q3));
      
      Hammerstad_er(u, er, a, b, e);
      ae = (e + 1) / 2 + (e - 1) / 2 * pow(1 + 10 / (u + q4), -a * b);
      
      // Even mode impedance
      Hammerstad_zl(u, zr);
      q5 = 1.794 + 1.14 * log(1 + 0.638 / (S / h + 0.517 * pow(S / h, 2.43)));
      q6 = 0.2305 + log(pow(S / h, 10) / (1 + pow(S / h / 5.8, 10))) / 281.3 +
           log(1 + 0.598 * pow(S / h, 1.154)) / 5.1;
      q7 = (10 + 190 * pow(S / h, 2)) / (1 + 82.3 * pow(S / h, 3));
      q8 = exp(-6.5 - 0.95 * log(S / h) - pow(S / h / 0.15, 5));
      q9 = log(q7) * (q8 + 1.0 / 16.5);
      q10 = q2 / q1 * (q9 * pow(u, -q6) + (2 - q9) * pow(u, q6));
      
      z = zr / sqrt(e) / (1 - zr / Z0 / sqrt(e) * q4 / q10);
      e = ae;
    } else {
      // Odd mode calculation
      q1 = 0.8695 * pow(u, 0.194);
      q2 = 1 + 0.7519 * S / h + 0.189 * pow(S / h, 2.31);
      q3 = 0.1975 + pow(16.6 + pow(8.4 / (S / h), 6), -0.387) +
           log(pow(S / h, 10) / (1 + pow(S / h / 3.4, 10))) / 241;
      
      Hammerstad_er(u, er, a, b, e);
      ao = (e + 1) / 2 + (e - 1) / 2 * pow(1 + 10 / u, -a * b);
      
      // Odd mode additional terms
      double q11, q12, q13;
      q11 = 0.2528 + 0.078 * S / h / (1 + 0.157 * S / h) - 
            0.01737 * pow(S / h, 2) / (1 + 0.0325 * pow(S / h, 2));
      q12 = pow(30 - 22.2 * pow((er - 1) / (er + 1), 1.92), q11);
      q13 = 1 + 0.598 * pow(q12 - 1, 2.1) * 
            pow(S / h, 0.38 - 1.44 * log(er) / pow(er, 0.15));
      
      ao = 0.5 * (e + 1) + 0.5 * (e - 1) * pow(1 + 10 / u, -a * b) * q13;
      
      // Odd mode impedance
      Hammerstad_zl(u, zr);
      double q14, q15, q16, q17, q18, q19, q20;
      q14 = (S / h / 0.15 + S / h) / (S / h / 0.15 + 0.6 + 0.009 * S / h);
      q15 = 0.2528 + 0.078 * S / h / (1 + 0.157 * S / h) - 
            0.01737 * pow(S / h, 2) / (1 + 0.0325 * pow(S / h, 2));
      q16 = pow(30 - 22.2 * pow((er - 1) / (er + 1), 1.92), q15);
      q17 = 1.206 - 0.3144 * exp(-1.03891 * pow(er, 1.4)) * 
            (1 - exp(-0.267 * pow(u, 7)));
      q18 = 14 + 8 / er + 11 * sqrt(u / (u + 10));
      q19 = q14 * exp(-1.27 * sqrt(u) / sqrt(1 + u / 200)) * 
            pow(u, 1.2 / q18);
      q20 = q14 * (1 - exp(-0.15 * sqrt(S / h))) / (1 + 0.149 * pow(u, 1.051));
      
      z = zr / sqrt(e) * pow(q17 - q19, -1) * pow(q17 - q20, -1) * 
          (1 - zr / Z0 / sqrt(e) * tanh(sqrt(er) * S / h) / 2);
      e = ao;
    }
  }

  ZlEff = z;
  ErEff = e;
}

void SParameterCalculator::analyseDispersionCoupled(
    double W, double S, double h, double er, double ZlEff, double ErEff,
    double frequency, const string& Model, bool evenMode,
    double& ZlEffFreq, double& ErEffFreq) {

  double e, z;
  double u = W / h;
  double fn = frequency * h / 1e6;

  // Default values
  z = ZlEffFreq = ZlEff;
  e = ErEffFreq = ErEff;

  if (Model == "Kirschning") {
    // Use single line dispersion as approximation
    Kirschning_er(u, fn, er, ErEff, e);
    
    double r17;
    Kirschning_zl(u, fn, er, ErEff, e, ZlEff, r17, z);
  }

  ZlEffFreq = z;
  ErEffFreq = e;
}

void SParameterCalculator::analyseLossCoupled(
    double W, double S, double t, double er, double rho, double D, double tand,
    double ZlEff1, double ZlEff2, double ErEff, double frequency,
    const string& Model, bool evenMode, double& ac, double& ad) {

  ac = ad = 0;

  if (Model == "Hammerstad") {
    double Rs, ds, l0, Kr, Ki;

    // Conductor losses
    if (t != 0.0 && rho != 0.0) {
      Rs = sqrt(M_PI * frequency * MU0 * rho);
      ds = rho / Rs;

      Ki = exp(-1.2 * pow((ZlEff1 + ZlEff2) / 2 / Z0, 0.7));
      Kr = 1 + two_over_pi * atan(1.4 * pow(D / ds, 2));

      // Adjust for coupling effects
      double coupling_factor = evenMode ? 1.0 : 1.2;
      ac = Rs / (ZlEff1 * W) * Ki * Kr * coupling_factor;
    }

    // Dielectric losses
    if (tand != 0.0) {
      l0 = C0 / frequency;
      ad = M_PI * er / (er - 1) * (ErEff - 1) / sqrt(ErEff) * tand / l0;
    }
  }
}
