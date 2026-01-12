/// @file MultistageWilkinson.cpp
/// @brief Multistage Wilkinson power combiner/divider network (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "MultistageWilkinson.h"

void MultistageWilkinson::calculateParams() {
  Zlines = ChebyshevTaper(2 * Specification.Z0, 0.05);

  double alpha = log(pow(0.1 * Specification.alpha, 10));
  lambda4 = SPEED_OF_LIGHT / (4. * Specification.freq);
  std::complex<double> gamma(alpha,
                             2 * M_PI * Specification.freq / SPEED_OF_LIGHT);

  Risol = calcMultistageWilkinsonIsolators(Zlines, lambda4, gamma);
}

void MultistageWilkinson::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Lumped) {
    buildMultistageWilkinson_LumpedLC();
  } else if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildMultistageWilkinson_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildMultistageWilkinson_Microstrip();
  }
}

void MultistageWilkinson::buildMultistageWilkinson_LumpedLC() {
  int NStages = Specification.Nstages;
  std::vector<double> C(NStages, 0.0);
  std::vector<double> L(NStages, 0.0);

  double w = 2 * M_PI * Specification.freq;
  for (int i = 0; i < NStages; i++) {
    L[i] = Zlines[i] / w;
    C[i] = 1. / (L[i] * w * w);
  }

  // In the schematic of a Wilkinson power combiner there are two output
  // branches and a central feed line. These variables set the y-axis coordinate
  // of these branches;

  int y_upper_branch = -60;
  int y_central_branch = 0;
  int y_lower_branch = 60;

  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo Ground;
  ComponentInfo Cshunt, Lseries;
  NodeInfo Ncentral, Nupper, Nlower, Nupper_, Nlower_;

  int posx = 0; // x-axis position. It varies as the loop grows

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, posx, y_central_branch);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;

  // Shunt capacitor
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, posx, y_central_branch + 30);
  Cshunt.val["C"] = num2str(2 * C[0], Capacitance);
  Schematic.appendComponent(Cshunt);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, posx, y_central_branch + 80);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

  NodeInfo N_1st_shunt_C(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_central_branch);
  Schematic.appendNode(N_1st_shunt_C);

  Schematic.appendWire(N_1st_shunt_C.ID, 0, TermSpar1.ID, 0);
  Schematic.appendWire(Cshunt.ID, 1, N_1st_shunt_C.ID, 0);

  posx += 50; // Split zone
  /////////////////////////////////
  // Nodes needed in the split zone
  Nlower.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_lower_branch);
  Nlower.visible = false; // Not visible. This node is not really needed, but
                          // makes routing easier
  Schematic.appendNode(Nlower);

  Ncentral.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_central_branch);
  Schematic.appendNode(Ncentral);

  Nupper.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_upper_branch);
  Nupper.visible = false;
  ; // Not visible. This node is not really needed, but
    // makes routing easier
  Schematic.appendNode(Nupper);

  Schematic.appendWire(Ncentral.ID, 0, Nlower.ID, 0);
  Schematic.appendWire(Ncentral.ID, 0, Nupper.ID, 0);
  /////////////////////////////////

  Schematic.appendWire(N_1st_shunt_C.ID, 0, Ncentral.ID, 0);

  // The next instantiations of these nodes need to be visible
  Nlower.visible = true;
  Nupper.visible = true;

  for (int i = 0; i < Specification.Nstages; i++) {
    double C_;
    if (i != Specification.Nstages - 1) {
      C_ = C[i] + C[i + 1];
    } else {
      C_ = C[i];
    }

    // Upper branch
    posx += 50;

    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, y_upper_branch);
    Lseries.val["L"] = num2str(L[i], Inductance);
    Schematic.appendComponent(Lseries);

    posx += 50;

    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, posx, y_upper_branch + 30);
    Cshunt.val["C"] = num2str(C_, Capacitance);
    Schematic.appendComponent(Cshunt);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, y_upper_branch + 80);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    Schematic.appendWire(Lseries.ID, 1, Nupper.ID, 0);

    Nupper.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_upper_branch);
    Schematic.appendNode(Nupper);

    Nupper_.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        y_upper_branch);
    Schematic.appendNode(Nupper_);

    Schematic.appendWire(Cshunt.ID, 1, Nupper_.ID, 0);
    Schematic.appendWire(Nupper_.ID, 0, Lseries.ID, 0);
    Schematic.appendWire(Nupper_.ID, 0, Nupper.ID, 0);

    posx -= 50;

    // Lower branch
    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, y_lower_branch);
    Lseries.val["L"] = num2str(L[i], Inductance);
    Schematic.appendComponent(Lseries);

    posx += 50;

    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, posx, y_lower_branch + 30);
    Schematic.appendComponent(Cshunt);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, y_lower_branch + 80);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    Schematic.appendWire(Lseries.ID, 1, Nlower.ID, 0);

    Nlower.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_lower_branch);
    Schematic.appendNode(Nlower);

    Nlower_.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        y_lower_branch);
    Schematic.appendNode(Nlower_);

    Schematic.appendWire(Cshunt.ID, 1, Nlower_.ID, 0);
    Schematic.appendWire(Nlower_.ID, 0, Lseries.ID, 0);
    Schematic.appendWire(Nlower_.ID, 0, Nlower.ID, 0);

    posx += 50;

    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        posx, y_central_branch);
    Riso.val["R"] = num2str(Risol[i], Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, Nupper.ID, 0);
    Schematic.appendWire(Riso.ID, 0, Nlower.ID, 0);
  }

  posx += 50;

  // Output terminals
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, y_upper_branch);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Nupper.ID, 0);

  TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, y_lower_branch);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, Nlower.ID, 0);
}

void MultistageWilkinson::buildMultistageWilkinson_IdealTL() {
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo TL, TL_Upper, TL_Lower;
  NodeInfo Ncentral, Nupper, Nlower;

  int posx = 0;

  int y_upper_branch = -60;
  int y_central_branch = 0;
  int y_lower_branch = 60;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, posx, y_central_branch);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;

  // Input transmission line
  TL.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, posx, y_central_branch);
  TL.val["Z0"] = num2str(Specification.Z0, Resistance);
  TL.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL);

  posx += 50;
  /////////////////////////////////
  // Nodes needed in the split zone
  Nlower.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_lower_branch);
  Nlower.visible = false; // Not visible. This node is not really needed, but
                          // makes routing easier
  Schematic.appendNode(Nlower);

  Ncentral.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_central_branch);
  Schematic.appendNode(Ncentral);

  Nupper.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_upper_branch);
  Nupper.visible = false; // Not visible. This node is not really needed, but
                          // makes routing easier
  Schematic.appendNode(Nupper);

  Schematic.appendWire(Ncentral.ID, 0, Nlower.ID, 0);
  Schematic.appendWire(Ncentral.ID, 0, Nupper.ID, 0);
  /////////////////////////////////

  Schematic.appendWire(TermSpar1.ID, 0, TL.ID, 0);
  Schematic.appendWire(TL.ID, 1, Ncentral.ID, 0);

  // The next instantiations of these nodes need to be visible
  Nlower.visible = true;
  Nupper.visible = true;

  for (int i = 0; i < Specification.Nstages; i++) {
    posx += 30;

    // Upper branch TL
    TL_Upper.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx + 15, y_upper_branch);
    TL_Upper.val["Z0"] = num2str(Zlines[i], Resistance);
    TL_Upper.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL_Upper);

    Schematic.appendWire(TL_Upper.ID, 0, Nupper.ID, 0);

    Nupper.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_upper_branch);
    Schematic.appendNode(Nupper);
    Schematic.appendWire(TL_Upper.ID, 1, Nupper.ID, 0);

    // Lower branch TL
    TL_Lower.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx + 15, y_lower_branch);
    TL_Lower.val["Z0"] = num2str(Zlines[i], Resistance);
    TL_Lower.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL_Lower);

    Schematic.appendWire(TL_Lower.ID, 0, Nlower.ID, 0);

    Nlower.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_lower_branch);
    Schematic.appendNode(Nlower);
    Schematic.appendWire(TL_Lower.ID, 1, Nlower.ID, 0);

    posx += 50;

    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        posx, y_central_branch);
    Riso.val["R"] = num2str(Risol[i], Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, Nupper.ID, 0);
    Schematic.appendWire(Riso.ID, 0, Nlower.ID, 0);
  }

  posx += 50;

  // Output terminals
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, y_upper_branch);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Nupper.ID, 0);

  TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, y_lower_branch);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, Nlower.ID, 0);
}

void MultistageWilkinson::buildMultistageWilkinson_Microstrip() {
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo MLIN, MLIN_Upper, MLIN_Lower;
  NodeInfo Ncentral, Nupper, Nlower;

  int posx = 0;

  int y_upper_branch = -60;
  int y_central_branch = 0;
  int y_lower_branch = 60;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, posx, y_central_branch);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;

  // Input microstrip line
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specification.MS_Subs;
  MSL_Input.synthesizeMicrostrip(Specification.Z0, lambda4 * 1e3,
                                 Specification.freq);

  MLIN.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, posx, y_central_branch);
  MLIN.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN);

  posx += 50;
  /////////////////////////////////
  // Nodes needed in the split zone
  Nlower.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_lower_branch);
  Nlower.visible = false; // Not visible. This node is not really needed, but
                          // makes routing easier
  Schematic.appendNode(Nlower);

  Ncentral.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_central_branch);
  Schematic.appendNode(Ncentral);

  Nupper.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      y_upper_branch);
  Nupper.visible = false; // Not visible. This node is not really needed, but
                          // makes routing easier
  Schematic.appendNode(Nupper);

  Schematic.appendWire(Ncentral.ID, 0, Nlower.ID, 0);
  Schematic.appendWire(Ncentral.ID, 0, Nupper.ID, 0);
  /////////////////////////////////

  Schematic.appendWire(TermSpar1.ID, 0, MLIN.ID, 0);
  Schematic.appendWire(MLIN.ID, 1, Ncentral.ID, 0);

  // The next instantiations of these nodes need to be visible
  Nlower.visible = true;
  Nupper.visible = true;

  for (int i = 0; i < Specification.Nstages; i++) {
    posx += 30;

    // Upper branch microstrip line
    MicrostripClass MSL_Upper;
    MSL_Upper.Substrate = Specification.MS_Subs;
    MSL_Upper.synthesizeMicrostrip(Zlines[i], lambda4 * 1e3,
                                   Specification.freq);

    MLIN_Upper.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx + 15, y_upper_branch);
    MLIN_Upper.val["Width"] = ConvertLengthFromM("mm", MSL_Upper.Results.width);
    MLIN_Upper.val["Length"] =
        ConvertLengthFromM("mm", MSL_Upper.Results.length * 1e-3);
    MLIN_Upper.val["er"] = num2str(Specification.MS_Subs.er);
    MLIN_Upper.val["h"] = num2str(Specification.MS_Subs.height);
    MLIN_Upper.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MLIN_Upper.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MLIN_Upper.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MLIN_Upper);

    Schematic.appendWire(MLIN_Upper.ID, 0, Nupper.ID, 0);

    Nupper.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_upper_branch);
    Schematic.appendNode(Nupper);
    Schematic.appendWire(MLIN_Upper.ID, 1, Nupper.ID, 0);

    // Lower branch microstrip line
    MicrostripClass MSL_Lower;
    MSL_Lower.Substrate = Specification.MS_Subs;
    MSL_Lower.synthesizeMicrostrip(Zlines[i], lambda4 * 1e3,
                                   Specification.freq);

    MLIN_Lower.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx + 15, y_lower_branch);
    MLIN_Lower.val["Width"] = ConvertLengthFromM("mm", MSL_Lower.Results.width);
    MLIN_Lower.val["Length"] =
        ConvertLengthFromM("mm", MSL_Lower.Results.length * 1e-3);
    MLIN_Lower.val["er"] = num2str(Specification.MS_Subs.er);
    MLIN_Lower.val["h"] = num2str(Specification.MS_Subs.height);
    MLIN_Lower.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MLIN_Lower.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MLIN_Lower.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MLIN_Lower);

    Schematic.appendWire(MLIN_Lower.ID, 0, Nlower.ID, 0);

    Nlower.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_lower_branch);
    Schematic.appendNode(Nlower);
    Schematic.appendWire(MLIN_Lower.ID, 1, Nlower.ID, 0);

    posx += 50;

    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        posx, y_central_branch);
    Riso.val["R"] = num2str(Risol[i], Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, Nupper.ID, 0);
    Schematic.appendWire(Riso.ID, 0, Nlower.ID, 0);
  }

  posx += 50;

  // Output terminals
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, y_upper_branch);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Nupper.ID, 0);

  TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, y_lower_branch);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, Nlower.ID, 0);
}

std::deque<double> MultistageWilkinson::calcMultistageWilkinsonIsolators(
    const std::deque<double> &Zlines, double L, std::complex<double> gamma) {
  int NStages = Specification.Nstages;
  double Z_, R, Zaux = Zlines[NStages - 1];
  std::deque<double> Risol_temp;

  for (int i = 0; i < NStages; i++) {
    Z_ = abs(Zaux * (Specification.Z0 + Zaux * tanh(gamma * L)) /
             (Zaux + Specification.Z0 * tanh(gamma * L)));
    Zaux = Zlines[i];
    R = Specification.Z0 * Z_ / (Z_ - Specification.Z0);
    Risol_temp.push_front(2 * R);
  }
  return Risol_temp;
}

//-----------------------------------------------------------------------------------
// This function calculates a multistage lambda/4 matching using the Chebyshev
// weigthing. See Microwave Engineering. David Pozar. John Wiley and Sons. 4th
// Edition. Pg 256-261
std::deque<double> MultistageWilkinson::ChebyshevTaper(double RL,
                                                       double gamma) {
  double Z0 = Specification.Z0;
  int N = Specification.Nstages;
  double sec_theta_m;
  (fabs(log(RL / Z0) / (2 * gamma)) < 1)
      ? sec_theta_m = 0
      : sec_theta_m =
            cosh((1 / (1. * N)) * acosh(fabs(log(RL / Z0) / (2 * gamma))));

  std::deque<double> Taper;
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
    (RL < Z0) ? Zi = exp(log(Zaux) - gamma * w[i])
              : Zi = exp(log(Zaux) + gamma * w[i]); // When RL<Z0, Z_{i}<Z_{i-1}
    Zaux = Zi;
    Taper.push_front(Zi);
  }
  return Taper;
}
