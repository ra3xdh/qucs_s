/// @file MicrostripOpen.cpp
/// @brief Implementation of function for the S-parameter analysis of the
/// microstrip open line
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../SParameterCalculator.h"

void SParameterCalculator::addMicrostripOpenToAdmittance(
    vector<vector<Complex>> &Y, const Component_SPAR &comp) {
  // Extract microstrip open end parameters
  int node1 = comp.nodes[0];

  double W = comp.value.value("W");       // Width in meters
  double h = comp.value.value("h");       // Substrate height in meters
  double er = comp.value.value("er");     // Relative permittivity
  double t = comp.value.value("th", 0.0); // Conductor thickness (optional)

  // Get model type (default: Kirschning)
  string Model = "Kirschning"; // Default model

  // Default model names (hardcoded for now, can be made configurable later)
  string SModel = "Hammerstad";
  string DModel = "Kirschning";

  // Calculate admittance for the open end
  Complex y =
      calcMicrostripOpenY(W, h, er, t, frequency, Model, SModel, DModel);

  // Add to admittance matrix
  if (node1 > 0) {
    Y[node1 - 1][node1 - 1] += y;
  }
}

Complex SParameterCalculator::calcMicrostripOpenY(double W, double h, double er,
                                                  double t, double frequency,
                                                  const string &Model,
                                                  const string &SModel,
                                                  const string &DModel) {
  double omega = 2.0 * M_PI * frequency;
  Complex y;

  // Alexopoulos and Wu model
  if (Model == "Alexopoulos") {
    double ZlEff, ErEff, WEff, ZlEffFreq, ErEffFreq;
    analyseQuasiStatic(W, h, t, er, SModel, ZlEff, ErEff, WEff);
    analyseDispersion(W, h, er, ZlEff, ErEff, frequency, DModel, ZlEffFreq,
                      ErEffFreq);

    // Warning: Model defined for er = 9.9
    if (fabs(er - 9.9) > 0.2) {
      // Model may be less accurate for er far from 9.9
    }

    double W_h = W / h;
    double c1, c2, l2, r2;

    // Capacitance c1 (in pF, convert from original formula)
    c1 = (1.125 * tanh(1.358 * W_h) - 0.315) * h / 2.54e-5 / 25.0 / ZlEffFreq *
         1e-12;

    // Capacitance c2 (in pF)
    c2 = (6.832 * tanh(0.0109 * W_h) + 0.919) * h / 2.54e-5 / 25.0 / ZlEffFreq *
         1e-12;

    // Inductance l2 (in nH)
    l2 = (0.008285 * tanh(0.5665 * W_h) + 0.0103) * h / 2.54e-5 / 25.0 *
         ZlEffFreq * 1e-9;

    // Resistance r2 (in Ohms)
    r2 = (1.024 * tanh(2.025 * W_h)) * ZlEffFreq;

    // Admittance: Y = j*omega*c1 + 1/(r2 + j*(omega*l2 - 1/(omega*c2)))
    Complex y_c1 = Complex(0.0, omega * c1);
    Complex z_parallel = Complex(r2, omega * l2 - 1.0 / (omega * c2));
    y = y_c1 + Complex(1.0, 0.0) / z_parallel;
  } else {
    // Kirschning or Hammerstad model - simple capacitive end effect
    double c =
        calcMicrostripOpenCend(W, h, er, t, frequency, Model, SModel, DModel);
    y = Complex(0.0, omega * c);
  }

  return y;
}

double SParameterCalculator::calcMicrostripOpenCend(
    double W, double h, double er, double t, double frequency,
    const string &Model, const string &SModel, const string &DModel) {
  // Calculate line parameters
  double ZlEff, ErEff, WEff, ZlEffFreq, ErEffFreq;
  analyseQuasiStatic(W, h, t, er, SModel, ZlEff, ErEff, WEff);
  analyseDispersion(W, h, er, ZlEff, ErEff, frequency, DModel, ZlEffFreq,
                    ErEffFreq);

  double W_h = W / h;
  double dl = 0.0;

  // Kirschning, Jansen and Koster model
  if (Model == "Kirschning") {
    double Q6 = pow(ErEffFreq, 0.81);
    double Q7 = pow(W_h, 0.8544);
    double Q1 =
        0.434907 * (Q6 + 0.26) / (Q6 - 0.189) * (Q7 + 0.236) / (Q7 + 0.87);
    double Q2 = pow(W_h, 0.371) / (2.358 * er + 1.0) + 1.0;
    double Q3 =
        atan(0.084 * pow(W_h, 1.9413 / Q2)) * 0.5274 / pow(ErEffFreq, 0.9236) +
        1.0;
    double Q4 = 0.0377 * (6.0 - 5.0 * exp(0.036 * (1.0 - er))) *
                    atan(0.067 * pow(W_h, 1.456)) +
                1.0;
    double Q5 = 1.0 - 0.218 * exp(-7.5 * W_h);
    dl = Q1 * Q3 * Q5 / Q4;
  }
  // Hammerstad model
  else if (Model == "Hammerstad") {
    dl = 0.102 * (W_h + 0.106) / (W_h + 0.264) *
         (1.166 + (er + 1.0) / er * (0.9 + log(W_h + 2.475)));
  }

  // Return capacitance: C = dl * h * sqrt(ErEffFreq) / (c0 * ZlEffFreq)
  return dl * h * sqrt(ErEffFreq) / C0 / ZlEffFreq;
}
