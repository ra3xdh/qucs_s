/// @file MultisectionQuarterWave.cpp
/// @brief Quarterwave multisection transformer matching (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "MultisectionQuarterWave.h"

// Binomial coefficient
int MultisectionQuarterWave::BinomialCoeff(int n, int k) {
  double coeff = 1.0;
  for (int i = 1; i <= k; i++) {
    coeff *= (n + 1 - i) / double(i);
  }
  return int(coeff);
}

// Binomial weighting design
void MultisectionQuarterWave::designBinomial(std::vector<double> &Zs) {
  double RL = Specs.ZL.real(); // Load resistance only
  double Z0 = Specs.Z0;        // Source port reference
  int N = Specs.NSections + 1; // Number of sections
  double Zaux = Z0;

  for (int i = 1; i < N; i++) {
    double Ci = BinomialCoeff(N - 1, i - 1);
    double Zi = std::exp(std::log(Zaux) +
                         (Ci / std::pow(2, N - 1)) * std::log(RL / Z0));
    Zaux = Zi;
    Zs.push_back(Zi);
  }
}

// Chebyshev weighting design
void MultisectionQuarterWave::designChebyshev(std::vector<double> &Zs) {
  double RL = Specs.ZL.real();
  double Z0 = Specs.Z0;
  int N = Specs.NSections; // number of sections
  double gamma = Specs.gamma_MAX;

  double log_ratio = std::log(RL / Z0) / (2.0 * gamma);
  if (fabs(log_ratio) < 1) {
    return;
  }

  double sec_theta_m = std::cosh((1.0 / N) * std::acosh(fabs(log_ratio)));
  std::vector<double> w(N, 0.0);

  switch (N) // The weights are calculated by equating the reflection coeffient
             // formula to the N-th Chebyshev polinomial
  {
  case 1:
    w[0] = sec_theta_m;
    break;
  case 2:
    w[0] = sec_theta_m * sec_theta_m;
    w[1] = 2 * (sec_theta_m * sec_theta_m - 1);
    break;
  case 3:
    w[0] = pow(sec_theta_m, 3);
    w[1] = 3 * (pow(sec_theta_m, 3) - sec_theta_m);
    w[2] = w[1];
    break;
  case 4:
    w[0] = pow(sec_theta_m, 4);
    w[1] = 4 * sec_theta_m * sec_theta_m * (sec_theta_m * sec_theta_m - 1);
    w[2] = 2 * (1 - 4 * sec_theta_m * sec_theta_m + 3 * pow(sec_theta_m, 4));
    w[3] = w[1];
    break;
  case 5:
    w[0] = pow(sec_theta_m, 5);
    w[1] = 5 * (pow(sec_theta_m, 5) - pow(sec_theta_m, 3));
    w[2] =
        10 * pow(sec_theta_m, 5) - 15 * pow(sec_theta_m, 3) + 5 * sec_theta_m;
    w[3] = w[2];
    w[4] = w[1];
    break;
  case 6:
    w[0] = pow(sec_theta_m, 6);
    w[1] = 6 * pow(sec_theta_m, 4) * (sec_theta_m * sec_theta_m - 1);
    w[2] = 15 * pow(sec_theta_m, 6) - 24 * pow(sec_theta_m, 4) +
           9 * sec_theta_m * sec_theta_m;
    w[3] = 2 * (10 * pow(sec_theta_m, 6) - 18 * pow(sec_theta_m, 4) +
                9 * sec_theta_m * sec_theta_m - 1);
    w[4] = w[2];
    w[5] = w[1];
    break;
  case 7:
    w[0] = pow(sec_theta_m, 7);
    w[1] = 7 * pow(sec_theta_m, 5) * (sec_theta_m * sec_theta_m - 1);
    w[2] = 21 * pow(sec_theta_m, 7) - 35 * pow(sec_theta_m, 5) +
           14 * pow(sec_theta_m, 3);
    w[3] = 35 * pow(sec_theta_m, 7) - 70 * pow(sec_theta_m, 5) +
           42 * pow(sec_theta_m, 3) - 7 * sec_theta_m;
    w[4] = w[3];
    w[5] = w[2];
    w[6] = w[1];
    break;
  }

  double Zaux = Z0, Zi;

  for (int i = 0; i < N; i++) {
    Zi = (RL < Z0) ? std::exp(std::log(Zaux) - gamma * w[i])
                   : std::exp(std::log(Zaux) + gamma * w[i]);
    Zaux = Zi;
    Zs.push_back(Zi);
  }
}

void MultisectionQuarterWave::synthesize() {

  // Calculate impedance weighting
  double lambda4 = SPEED_OF_LIGHT / (4 * f_match);
  std::vector<double> Zi;
  if (Specs.Weigthing == QString("Chebyshev")) {
    designChebyshev(Zi);
  } else {
    designBinomial(Zi);
  }

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    synthesizeIdealTL(Zi, lambda4);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    synthesizeMicrostripTL(Zi, lambda4);
  }
}

void MultisectionQuarterWave::synthesizeIdealTL(const std::vector<double> &Zi,
                                                double lambda4) {
  // Create source termination
  ComponentInfo TermSrc(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 0, 0, 0);
  TermSrc.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSrc);

  ComponentInfo TL;
  int xpos = 50;
  QString PreviousComponent = TermSrc.ID;

  // Place transmission line sections
  for (size_t i = 0; i < Zi.size(); i++) {
    // Ideal transmission line
    TL.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, xpos, 0);
    TL.val["Z0"] = num2str(Zi[i], Resistance);
    TL.val["Length"] = ConvertLengthFromM("mm", lambda4);
    Schematic.appendComponent(TL);

    // Wire previous to this
    if (i == 0) {
      // The previous component is the port
      Schematic.appendWire(PreviousComponent, 0, TL.ID, 0);
    } else {
      // The previous component is another TLIN
      Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);
    }

    xpos += 50;
    PreviousComponent = TL.ID;
  }

  // Add load components
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, xpos, 50);
  Zload.val["Z"] = num2str(Specs.ZL.real(), Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, xpos, 100);
  Schematic.appendComponent(GND_ZL);

  // Connect last TL to load
  Schematic.appendWire(Zload.ID, 1, PreviousComponent, 1);
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}

void MultisectionQuarterWave::synthesizeMicrostripTL(
    const std::vector<double> &Zi, double lambda4) {
  // Create source termination
  ComponentInfo TermSrc(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 0, 0, 0);
  TermSrc.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSrc);

  ComponentInfo MStep, TL;
  int xpos = 50;
  QString PreviousComponent = TermSrc.ID;

  // Place transmission line sections
  for (size_t i = 0; i < Zi.size(); i++) {
    // Microstrip transmission line

    // Check that the previous component is not a term. In that case, it was a
    // microstrip line of different width and a step needs to be added to model
    // the transition
    if (!PreviousComponent.startsWith("T")) {
      xpos += 10; // Add some extra room

      MStep.ID =
          QString("MSTEP%1").arg(++Schematic.NumberComponents[MicrostripStep]);
      MStep.Type = MicrostripStep;
      MStep.Rotation = 0;
      MStep.Coordinates = {static_cast<double>(xpos), 0};

      // Add its properties
      MStep.val["W1"] = TL.val["Width"]; // Last microstrip line props are still
                                         // in TL variable
      // MStep.val["W2"] = <This needs to be calculated later>

      // Substrate-related parameters
      MStep.val["er"] = num2str(Specs.MS_Subs.er);
      MStep.val["h"] = num2str(Specs.MS_Subs.height);
      MStep.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
      MStep.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
      MStep.val["tand"] = num2str(Specs.MS_Subs.tand);

      xpos += 60; // Advance the x-axis drawing index
    }

    // Microstrip line
    // Synthesize MS parameters
    MicrostripClass MSL;
    MSL.Substrate = Specs.MS_Subs;
    MSL.synthesizeMicrostrip(Zi[i], lambda4 * 1e3, f_match);

    double MS_Width = MSL.Results.width; // MicrostripClass calculations are in
                                         // mm. It's needed to convert to m
    double MS_Length = MSL.Results.length * 1e-3;

    // Instantiate component
    TL.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, xpos, 0);
    TL.val["Width"] = ConvertLengthFromM("mm", MS_Width);
    TL.val["Length"] = ConvertLengthFromM("mm", MS_Length);

    // Substrate-related parameters
    TL.val["er"] = num2str(Specs.MS_Subs.er);
    TL.val["h"] = num2str(Specs.MS_Subs.height);
    TL.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    TL.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    TL.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(TL);
    xpos += 60;

    if (!PreviousComponent.startsWith("T")) {
      // At this point, the width of the microstrip line is calculated, so the
      // second width of the step can be assigned. At this point the component
      // is also added to the schematic and routed
      MStep.val["W2"] = TL.val["Width"];
      Schematic.appendComponent(MStep);

      // Connections
      Schematic.appendWire(PreviousComponent, 1, MStep.ID, 0);
      Schematic.appendWire(MStep.ID, 1, TL.ID, 0);
    } else {
      // The microstrip line is connected to the port
      Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);
    }

    PreviousComponent = TL.ID;
  }

  // Add load components
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, xpos, 50);
  Zload.val["Z"] = num2str(Specs.ZL.real(), Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, xpos, 100);
  Schematic.appendComponent(GND_ZL);

  // Connect last TL to load
  Schematic.appendWire(Zload.ID, 1, PreviousComponent, 1);
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}
