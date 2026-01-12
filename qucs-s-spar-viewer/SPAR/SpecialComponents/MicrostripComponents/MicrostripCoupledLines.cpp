/// @file MicrostripCoupledLines.cpp
/// @brief Implementation of function for the S-parameter analysis of the
/// microstrip coupled lines
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../SParameterCalculator.h"

void SParameterCalculator::addMicrostripCoupledLinesToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {
  // Extract microstrip coupled lines parameters
  int node1 = comp.nodes[0]; // Port 1 of line 1
  int node2 = comp.nodes[1]; // Port 2 of line 1
  int node3 = comp.nodes[2]; // Port 1 of line 2
  int node4 = comp.nodes[3]; // Port 2 of line 2

  double W = comp.value.value("W");       // Width of each line in meters
  double S = comp.value.value("S");       // Spacing between lines in meters
  double L = comp.value.value("L");       // Length in meters
  double h = comp.value.value("h");       // Substrate height in meters
  double er = comp.value.value("er");     // Relative permittivity
  double t = comp.value.value("th", 0.0); // Conductor thickness (optional)
  double tand = comp.value.value("tand", 0.0); // Loss tangent (optional)
  double rho = comp.value.value("rho", 1e-10); // Surface Resistivity (optional)

  // Calculate propagation characteristics for coupled lines
  double alpha_e, beta_e, zl_e, ereff_e; // Even mode
  double alpha_o, beta_o, zl_o, ereff_o; // Odd mode
  calcMicrostripCoupledPropagation(W, S, h, er, t, tand, rho, frequency,
                                   alpha_e, beta_e, zl_e, ereff_e, alpha_o,
                                   beta_o, zl_o, ereff_o);

  // Even mode calculations
  Complex gamma_e(alpha_e, beta_e);
  Complex gl_e = gamma_e * L;
  Complex sinh_gl_e = sinh(gl_e);
  Complex cosh_gl_e = cosh(gl_e);

  if (abs(gl_e) < 1e-10) {
    sinh_gl_e = gl_e;
    cosh_gl_e = Complex(1.0, 0.0);
  }

  // Odd mode calculations
  Complex gamma_o(alpha_o, beta_o);
  Complex gl_o = gamma_o * L;
  Complex sinh_gl_o = sinh(gl_o);
  Complex cosh_gl_o = cosh(gl_o);

  if (abs(gl_o) < 1e-10) {
    sinh_gl_o = gl_o;
    cosh_gl_o = Complex(1.0, 0.0);
  }

  Complex De = 0.5 / (zl_e * sinh_gl_e);
  Complex Do = 0.5 / (zl_o * sinh_gl_o);
  Complex y2 = -De - Do;
  Complex y3 = -De + Do;

  De = De * cosh_gl_e; // This is actually y1_e component
  Do = Do * cosh_gl_o; // This is actually y1_o component
  Complex y1 = De + Do;
  Complex y4 = De - Do;

  // The Y-matrix for coupled lines has this symmetric structure:
  // [y1  y2  y3  y4]
  // [y2  y1  y4  y3]
  // [y3  y4  y1  y2]
  // [y4  y3  y2  y1]

  // Add to admittance matrix
  if (node1 > 0) {
    Y[node1 - 1][node1 - 1] += y1;
    if (node2 > 0) {
      Y[node1 - 1][node2 - 1] += y2;
    }
    if (node3 > 0) {
      Y[node1 - 1][node3 - 1] += y3;
    }
    if (node4 > 0) {
      Y[node1 - 1][node4 - 1] += y4;
    }
  }

  if (node2 > 0) {
    if (node1 > 0) {
      Y[node2 - 1][node1 - 1] += y2;
    }
    Y[node2 - 1][node2 - 1] += y1;
    if (node3 > 0) {
      Y[node2 - 1][node3 - 1] += y4;
    }
    if (node4 > 0) {
      Y[node2 - 1][node4 - 1] += y3;
    }
  }

  if (node3 > 0) {
    if (node1 > 0) {
      Y[node3 - 1][node1 - 1] += y3;
    }
    if (node2 > 0) {
      Y[node3 - 1][node2 - 1] += y4;
    }
    Y[node3 - 1][node3 - 1] += y1;
    if (node4 > 0) {
      Y[node3 - 1][node4 - 1] += y2;
    }
  }

  if (node4 > 0) {
    if (node1 > 0) {
      Y[node4 - 1][node1 - 1] += y4;
    }
    if (node2 > 0) {
      Y[node4 - 1][node2 - 1] += y3;
    }
    if (node3 > 0) {
      Y[node4 - 1][node3 - 1] += y2;
    }
    Y[node4 - 1][node4 - 1] += y1;
  }
}

void SParameterCalculator::calcMicrostripCoupledPropagation(
    double W, double S, double h, double er, double t, double tand, double rho,
    double frequency, double &alpha_e, double &beta_e, double &zl_e,
    double &ereff_e, double &alpha_o, double &beta_o, double &zl_o,
    double &ereff_o) {

  double ZlEff_e, ErEff_e, ZlEffFreq_e, ErEffFreq_e;
  double ZlEff_o, ErEff_o, ZlEffFreq_o, ErEffFreq_o;
  double ac_e, ad_e, ac_o, ad_o;

  string SModel = "Kirschning"; // Default model
  string DModel = "Kirschning"; // Default dispersion model

  // Calculate BOTH even and odd modes together in quasi-static analysis
  analyseQuasiStaticCoupled(W, h, S, t, er, SModel, ZlEff_e, ZlEff_o, ErEff_e,
                            ErEff_o);

  // Analyse dispersion for even and odd modes
  analyseDispersionCoupled(W, h, S, t, er, ZlEff_e, ZlEff_o, ErEff_e, ErEff_o,
                           frequency, DModel, ZlEffFreq_e, ZlEffFreq_o,
                           ErEffFreq_e, ErEffFreq_o);

  // Analyse losses for even mode
  analyseLoss(W, t, er, rho, 0.0, tand, ZlEff_e, ZlEff_o, ErEff_e, frequency,
              "Hammerstad", ac_e, ad_e);

  // Analyse losses for odd mode
  analyseLoss(W, t, er, rho, 0.0, tand, ZlEff_o, ZlEff_e, ErEff_o, frequency,
              "Hammerstad", ac_o, ad_o);

  // Set output values
  zl_e = ZlEffFreq_e;
  ereff_e = ErEffFreq_e;
  alpha_e = ac_e + ad_e;
  beta_e = sqrt(ErEffFreq_e) * 2 * M_PI * frequency / C0;

  zl_o = ZlEffFreq_o;
  ereff_o = ErEffFreq_o;
  alpha_o = ac_o + ad_o;
  beta_o = sqrt(ErEffFreq_o) * 2 * M_PI * frequency / C0;
}

void SParameterCalculator::analyseQuasiStaticCoupled(
    double W, double h, double s, double t, double er, const string &Model,
    double &Zle, double &Zlo, double &ErEffe, double &ErEffo) {

  // Initialize default values
  ErEffe = ErEffo = er;
  Zlo = 42.2;
  Zle = 55.7;

  double u = W / h;
  double g = s / h;

  if (Model == "Hammerstad") {
    double Zl1, Fe, Fo, a, b, fo, Mu, Alpha, Beta, ErEff;
    double Pe, Po, r, fo1, q, p, n, Psi, Phi, m, Theta;

    // Modifying equations for even mode
    m = 0.2175 + pow(4.113 + pow(20.36 / g, 6.0), -0.251) +
        log(pow(g, 10.0) / (1.0 + pow(g / 13.8, 10.0))) / 323.0;
    Alpha = 0.5 * exp(-g);
    Psi = 1.0 + g / 1.45 + pow(g, 2.09) / 3.95;
    Phi = 0.8645 * pow(u, 0.172);
    Pe = Phi / (Psi * (Alpha * pow(u, m) + (1.0 - Alpha) * pow(u, -m)));

    // Modifying equations for odd mode
    n = (1.0 / 17.7 + exp(-6.424 - 0.76 * log(g) - pow(g / 0.23, 5.0))) *
        log((10.0 + 68.3 * pow(g, 2.0)) / (1.0 + 32.5 * pow(g, 3.093)));
    Beta = 0.2306 + log(pow(g, 10.0) / (1.0 + pow(g / 3.73, 10.0))) / 301.8 +
           log(1.0 + 0.646 * pow(g, 1.175)) / 5.3;
    Theta = 1.729 + 1.175 * log(1.0 + 0.627 / (g + 0.327 * pow(g, 2.17)));
    Po = Pe - Theta / Psi * exp(Beta * pow(u, -n) * log(u));

    // Further modifying equations
    r = 1.0 + 0.15 * (1.0 - exp(1.0 - pow(er - 1.0, 2.0) / 8.2) /
                                (1.0 + pow(g, -6.0)));
    fo1 = 1.0 - exp(-0.179 * pow(g, 0.15) -
                    0.328 * pow(g, r) / log(M_E + pow(g / 7.0, 2.8)));
    q = exp(-1.366 - g);
    p = exp(-0.745 * pow(g, 0.295)) / cosh(pow(g, 0.68));
    fo = fo1 * exp(p * log(u) + q * sin(M_PI * log10(u)));

    Mu = g * exp(-g) + u * (20.0 + pow(g, 2.0)) / (10.0 + pow(g, 2.0));
    Hammerstad_ab(Mu, er, a, b);
    Fe = pow(1.0 + 10.0 / Mu, -a * b);
    Hammerstad_ab(u, er, a, b);
    Fo = fo * pow(1.0 + 10.0 / u, -a * b);

    // Effective dielectric constants
    ErEffe = (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * Fe;
    ErEffo = (er + 1.0) / 2.0 + (er - 1.0) / 2.0 * Fo;

    // Single microstrip for reference
    Hammerstad_er(u, er, a, b, ErEff);
    Hammerstad_zl(u, Zl1);
    Zl1 /= sqrt(ErEff);

    // Characteristic impedances
    Zle = Zl1 / (1.0 - Zl1 * Pe / Z0);
    Zlo = Zl1 / (1.0 - Zl1 * Po / Z0);
  } else if (Model == "Kirschning") {
    double a, b, ae, be, ao, bo, v, co, d, ErEff, Zl1;
    double q1, q2, q3, q4, q5, q6, q7, q8, q9, q10;

    // Consider effect of finite strip thickness
    double ue = u;
    double uo = u;
    if (t != 0.0 && s > 10.0 * (2.0 * t)) {
      double dW = 0.0;
      if (u >= 1.0 / M_PI / 2.0 && 1.0 / M_PI / 2.0 > 2.0 * t / h) {
        dW = t * (1.0 + log(2.0 * h / t)) / M_PI;
      } else if (W > 2.0 * t) {
        dW = t * (1.0 + log(4.0 * M_PI * W / t)) / M_PI;
      }
      double dt = 2.0 * t * h / s / er;
      double We = W + dW * (1.0 - 0.5 * exp(-0.69 * dW / dt));
      double Wo = We + dt;
      ue = We / h;
      uo = Wo / h;
    }

    // Even relative dielectric constant
    v = ue * (20.0 + pow(g, 2.0)) / (10.0 + pow(g, 2.0)) + g * exp(-g);
    Hammerstad_ab(v, er, ae, be);
    Hammerstad_er(v, er, ae, be, ErEffe);

    // Odd relative dielectric constant
    Hammerstad_ab(uo, er, a, b);
    Hammerstad_er(uo, er, a, b, ErEff);
    d = 0.593 + 0.694 * exp(-0.562 * uo);
    bo = 0.747 * er / (0.15 + er);
    co = bo - (bo - 0.207) * exp(-0.414 * uo);
    ao = 0.7287 * (ErEff - (er + 1.0) / 2.0) * (1.0 - exp(-0.179 * uo));
    ErEffo = ((er + 1.0) / 2.0 + ao - ErEff) * exp(-co * pow(g, d)) + ErEff;

    // Characteristic impedance of single line
    Hammerstad_zl(u, Zl1);
    Zl1 /= sqrt(ErEff);

    // Even characteristic impedance
    q1 = 0.8695 * pow(ue, 0.194);
    q2 = 1.0 + 0.7519 * g + 0.189 * pow(g, 2.31);
    q3 = 0.1975 + pow(16.6 + pow(8.4 / g, 6.0), -0.387) +
         log(pow(g, 10.0) / (1.0 + pow(g / 3.4, 10.0))) / 241.0;
    q4 = q1 / q2 * 2.0 /
         (exp(-g) * pow(ue, q3) + (2.0 - exp(-g)) * pow(ue, -q3));
    Zle = sqrt(ErEff / ErEffe) * Zl1 / (1.0 - Zl1 * sqrt(ErEff) * q4 / Z0);

    // Odd characteristic impedance
    q5 = 1.794 + 1.14 * log(1.0 + 0.638 / (g + 0.517 * pow(g, 2.43)));
    q6 = 0.2305 + log(pow(g, 10.0) / (1.0 + pow(g / 5.8, 10.0))) / 281.3 +
         log(1.0 + 0.598 * pow(g, 1.154)) / 5.1;
    q7 = (10.0 + 190.0 * pow(g, 2.0)) / (1.0 + 82.3 * pow(g, 3.0));
    q8 = exp(-6.5 - 0.95 * log(g) - pow(g / 0.15, 5.0));
    q9 = log(q7) * (q8 + 1.0 / 16.5);
    q10 = (q2 * q4 - q5 * exp(log(uo) * q6 * pow(uo, -q9))) / q2;
    Zlo = sqrt(ErEff / ErEffo) * Zl1 / (1.0 - Zl1 * sqrt(ErEff) * q10 / Z0);
  }
}

void SParameterCalculator::analyseDispersionCoupled(
    double W, double h, double s, double t, double er, double Zle, double Zlo,
    double ErEffe, double ErEffo, double frequency, const string &DModel,
    double &ZleFreq, double &ZloFreq, double &ErEffeFreq, double &ErEffoFreq) {

  // Initialize default values
  ZleFreq = Zle;
  ErEffeFreq = ErEffe;
  ZloFreq = Zlo;
  ErEffoFreq = ErEffo;

  double u = W / h;
  double g = s / h;
  double ue, uo;
  double B, dW, dt;

  // Compute u_odd, u_even
  if (t > 0.0) {
    if (u < 0.1592) {
      B = 2.0 * M_PI * W;
    } else {
      B = h;
    }
    dW = t * (1.0 + log(2.0 * B / t)) / M_PI;
    dt = t / (er * g);
    ue = (W + dW * (1.0 - 0.5 * exp(-0.69 * dW / dt))) / h;
    uo = ue + dt / h;
  } else {
    ue = u;
    uo = u;
  }

  if (DModel == "Kirschning") {
    double p1, p2, p3, p4, p5, p6, p7, Fe;
    double fn = frequency * h * 1e-6;

    // Even relative dielectric constant dispersion
    p1 = 0.27488 * (0.6315 + 0.525 / pow(1.0 + 0.0157 * fn, 20.0)) * ue -
         0.065683 * exp(-8.7513 * ue);
    p2 = 0.33622 * (1.0 - exp(-0.03442 * er));
    p3 = 0.0363 * exp(-4.6 * ue) * (1.0 - exp(-pow(fn / 38.7, 4.97)));
    p4 = 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));
    p5 = 0.334 * exp(-3.3 * pow(er / 15.0, 3.0)) + 0.746;
    p6 = p5 * exp(-pow(fn / 18.0, 0.368));
    p7 = 1.0 + 4.069 * p6 * pow(g, 0.479) *
                   exp(-1.347 * pow(g, 0.595) - 0.17 * pow(g, 2.5));
    Fe = p1 * p2 * pow((p3 * p4 + 0.1844 * p7) * fn, 1.5763);
    ErEffeFreq = er - (er - ErEffe) / (1.0 + Fe);

    // Odd relative dielectric constant dispersion
    double p8, p9, p10, p11, p12, p13, p14, p15, Fo;
    p1 = 0.27488 * (0.6315 + 0.525 / pow(1.0 + 0.0157 * fn, 20.0)) * uo -
         0.065683 * exp(-8.7513 * uo);
    p3 = 0.0363 * exp(-4.6 * uo) * (1.0 - exp(-pow(fn / 38.7, 4.97)));
    p8 = 0.7168 * (1.0 + 1.076 / (1.0 + 0.0576 * (er - 1.0)));
    p9 = p8 - 0.7913 * (1.0 - exp(-pow(fn / 20.0, 1.424))) *
                  atan(2.481 * pow(er / 8.0, 0.946));
    p10 = 0.242 * pow(er - 1.0, 0.55);
    p11 =
        0.6366 * (exp(-0.3401 * fn) - 1.0) * atan(1.263 * pow(uo / 3.0, 1.629));
    p12 = p9 + (1.0 - p9) / (1.0 + 1.183 * pow(uo, 1.376));
    p13 = 1.695 * p10 / (0.414 + 1.605 * p10);
    p14 = 0.8928 + 0.1072 * (1.0 - exp(-0.42 * pow(fn / 20.0, 3.215)));
    p15 = fabs(1.0 -
               0.8928 * (1.0 + p11) * exp(-p13 * pow(g, 1.092)) * p12 / p14);
    Fo = p1 * p2 * pow((p3 * p4 + 0.1844) * fn * p15, 1.5763);
    ErEffoFreq = er - (er - ErEffo) / (1.0 + Fo);

    // Dispersion of even characteristic impedance
    double t, q11, q12, q13, q14, q15, q16, q17, q18, q19, q20, q21;
    q11 = 0.893 * (1.0 - 0.3 / (1.0 + 0.7 * (er - 1.0)));
    t = pow(fn / 20.0, 4.91);
    q12 = 2.121 * t / (1.0 + q11 * t) * exp(-2.87 * g) * pow(g, 0.902);
    q13 = 1.0 + 0.038 * pow(er / 8.0, 5.1);
    t = pow(er / 15.0, 4.0);
    q14 = 1.0 + 1.203 * t / (1.0 + t);
    q15 = 1.887 * exp(-1.5 * pow(g, 0.84)) * pow(g, q14) /
          (1.0 + 0.41 * pow(fn / 15.0, 3.0) * pow(u, 2.0 / q13) /
                     (0.125 + pow(u, 1.626 / q13)));
    q16 = q15 * (1.0 + 9.0 / (1.0 + 0.403 * pow(er - 1.0, 2.0)));
    q17 = 0.394 * (1.0 - exp(-1.47 * pow(u / 7.0, 0.672))) *
          (1.0 - exp(-4.25 * pow(fn / 20.0, 1.87)));
    q18 = 0.61 * (1.0 - exp(-2.31 * pow(u / 8.0, 1.593))) /
          (1.0 + 6.544 * pow(g, 4.17));
    q19 = 0.21 * pow(g, 4.0) / (1.0 + 0.18 * pow(g, 4.9)) /
          (1.0 + 0.1 * pow(u, 2.0)) / (1.0 + pow(fn / 24.0, 3.0));
    q20 = q19 * (0.09 + 1.0 / (1.0 + 0.1 * pow(er - 1.0, 2.7)));
    t = pow(u, 2.5);
    q21 = fabs(1.0 -
               42.54 * pow(g, 0.133) * exp(-0.812 * g) * t / (1.0 + 0.033 * t));

    double re, qe, pe, de, Ce, q0, ZlFreq, ErEffFreq;
    Kirschning_er(u, fn, er, ErEffe, ErEffFreq);
    Kirschning_zl(ErEffe, ErEffFreq, Zle, q0, ZlFreq);
    re = pow(fn / 28.843, 12.0);
    qe = 0.016 + pow(0.0514 * er * q21, 4.524);
    pe = 4.766 * exp(-3.228 * pow(u, 0.641));
    t = pow(er - 1.0, 6.0);
    de = 5.086 * qe * re / (0.3838 + 0.386 * qe) * exp(-22.2 * pow(u, 1.92)) /
         (1.0 + 1.2992 * re) * t / (1.0 + 10.0 * t);
    Ce = 1.0 +
         1.275 * (1.0 - exp(-0.004625 * pe * pow(er, 1.674) *
                            pow(fn / 18.365, 2.745))) -
         q12 + q16 - q17 + q18 + q20;
    ZleFreq = Zle * pow((0.9408 * pow(ErEffFreq, Ce) - 0.9603) /
                            ((0.9408 - de) * pow(ErEffe, Ce) - 0.9603),
                        q0);

    // Dispersion of odd characteristic impedance
    double q22, q23, q24, q25, q26, q27, q28, q29;
    Kirschning_er(u, fn, er, ErEffo, ErEffFreq);
    Kirschning_zl(ErEffo, ErEffFreq, Zlo, q0, ZlFreq);
    q29 = 15.16 / (1.0 + 0.196 * pow(er - 1.0, 2.0));
    t = pow(er - 1.0, 2.0);
    q25 = 0.3 * pow(fn, 2.0) / (10.0 + pow(fn, 2.0)) *
          (1.0 + 2.333 * t / (5.0 + t));
    t = pow((er - 1.0) / 13.0, 12.0);
    q26 = 30.0 - 22.2 * t / (1.0 + 3.0 * t) - q29;
    t = pow(er - 1.0, 1.5);
    q27 = 0.4 * pow(g, 0.84) * (1.0 + 2.5 * t / (5.0 + t));
    t = pow(er - 1.0, 3.0);
    q28 = 0.149 * t / (94.5 + 0.038 * t);
    q22 = 0.925 * pow(fn / q26, 1.536) / (1.0 + 0.3 * pow(fn / 30.0, 1.536));
    q23 = 1.0 + 0.005 * fn * q27 / (1.0 + 0.812 * pow(fn / 15.0, 1.9)) /
                    (1.0 + 0.025 * pow(u, 2.0));
    t = pow(u, 0.894);
    q24 =
        2.506 * q28 * t / (3.575 + t) * pow((1.0 + 1.3 * u) * fn / 99.25, 4.29);
    ZloFreq = ZlFreq + (Zlo * pow(ErEffoFreq / ErEffo, q22) - ZlFreq * q23) /
                           (1.0 + q24 + pow(0.46 * g, 2.2) * q25);
  }
}

void SParameterCalculator::analyseLossCoupled(
    double W, double t, double er, double rho, double D, double tand,
    double ZlEff1, double ZlEff2, double ErEff, double frequency,
    const string &Model, bool evenMode, double &ac, double &ad) {

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
