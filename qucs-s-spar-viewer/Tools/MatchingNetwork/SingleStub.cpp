/// @file SingleStub.cpp
/// @brief Single stub matching network synthesis (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "SingleStub.h"

// Reference: 'Microwave Engineering'. David Pozar. John Wiley and Sons. 4th
// Edition. Pg 234-241
void SingleStub::synthesize() {
  double lambda = SPEED_OF_LIGHT / f_match;
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double XL = Specs.ZL.imag();

  double t = calculateT(RL, XL, Z0);
  double B = calculateSusceptance(t, RL, XL, Z0);

  double d = calculateLineDistance(t, lambda);
  double lstub = calculateStubLength(B, Z0, lambda);

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildMatchingNetwork_IdealTL(d, lstub);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildMatchingNetwork_Microstrip(d, lstub);
  }
}

double SingleStub::calculateT(double RL, double XL, double Z0) {
  if (RL == Z0) {
    return -XL / (2 * Z0);
  }

  // Calculate both solutions
  double discriminant = sqrt((RL / Z0) * fabs((Z0 - RL) * (Z0 - RL) + XL * XL));
  double t1 = (XL + discriminant) / (RL - Z0);
  double t2 = (XL - discriminant) / (RL - Z0);

  // Prefer t1, fallback to t2 if t1 is zero
  return (t1 != 0) ? t1 : t2;
}

double SingleStub::calculateSusceptance(double t, double RL, double XL,
                                        double Z0) {
  return (RL * RL * t - (Z0 - XL * t) * (Z0 * t + XL)) /
         (Z0 * (RL * RL + (Z0 * t + XL) * (Z0 * t + XL)));
}

double SingleStub::normalizePhase(double phase) {
  return (phase < 0) ? (M_PI + phase) / (2 * M_PI) : phase / (2 * M_PI);
}

double SingleStub::calculateLineDistance(double t, double lambda) {
  double dl = normalizePhase(atan(t));
  return dl * lambda;
}

double SingleStub::calculateStubLength(double B, double Z0, double lambda) {
  double ll;

  if (!Specs.OpenShort) { // Open stub
    ll = -atan(B * Z0) / (2 * M_PI);
    if (ll < 0) {
      ll += 0.5;
    }
  } else { // Short stub
    ll = atan(1.0 / (B * Z0)) / (2 * M_PI);
    if (ll > 0.5) {
      ll -= 0.5;
    }
  }

  return ll * lambda;
}

void SingleStub::buildMatchingNetwork_IdealTL(double d, double lstub) {
  double Z0 = Specs.Z0;

  // Port 1 termination
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Load impedance
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, 175, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, 175, 100);
  Schematic.appendComponent(GND_ZL);

  // Node between the input port and stub
  NodeInfo NI1;
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Create stub component
  ComponentInfo Stub;
  if (!Specs.OpenShort) { // Open stub
    Stub.setParams(
        QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]),
        OpenStub, 0, 50, 50);
  } else { // Short stub
    Stub.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
        ShortStub, 0, 50, 50);
  }
  Stub.val["Z0"] = num2str(Z0, Resistance);
  Stub.val["Length"] = ConvertLengthFromM("mm", lstub);
  Schematic.appendComponent(Stub);

  // Create transmission line component
  ComponentInfo TLine;
  TLine.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 100, 0);
  TLine.val["Z0"] = num2str(Z0, Resistance);
  TLine.val["Length"] = ConvertLengthFromM("mm", d);
  Schematic.appendComponent(TLine);

  // Connect components with wires
  Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
  Schematic.appendWire(TLine.ID, 0, NI1.ID, 0);
  Schematic.appendWire(NI1.ID, 0, Stub.ID, 1);
  Schematic.appendWire(Zload.ID, 1, TLine.ID, 1);
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}

void SingleStub::buildMatchingNetwork_Microstrip(double d, double lstub) {
  double Z0 = Specs.Z0;

  // Port 1 termination
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Load impedance
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, 175, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, 175, 100);
  Schematic.appendComponent(GND_ZL);

  // Node between the input port and stub
  NodeInfo NI1;
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Synthesize microstrip transmission line to load
  MicrostripClass MSL_Line;
  MSL_Line.Substrate = Specs.MS_Subs;
  MSL_Line.synthesizeMicrostrip(Z0, d * 1e3, f_match);
  double MS_Line_Width = MSL_Line.Results.width;
  double MS_Line_Length = MSL_Line.Results.length * 1e-3;

  ComponentInfo TLine;
  TLine.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 100, 0);
  // Physical parameters
  TLine.val["Width"] = ConvertLengthFromM("mm", MS_Line_Width);
  TLine.val["Length"] = ConvertLengthFromM("mm", MS_Line_Length);
  // Substrate-related parameters
  TLine.val["er"] = num2str(Specs.MS_Subs.er);
  TLine.val["h"] = num2str(Specs.MS_Subs.height);
  TLine.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  TLine.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  TLine.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(TLine);

  // Synthesize microstrip stub
  MicrostripClass MSL_Stub;
  MSL_Stub.Substrate = Specs.MS_Subs;
  MSL_Stub.synthesizeMicrostrip(Z0, lstub * 1e3, f_match);
  double MS_Stub_Width = MSL_Stub.Results.width;
  double MS_Stub_Length = MSL_Stub.Results.length * 1e-3;

  ComponentInfo Stub;
  Stub.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 50, 50);
  // Physical parameters
  Stub.val["Width"] = ConvertLengthFromM("mm", MS_Stub_Width);
  Stub.val["Length"] = ConvertLengthFromM("mm", MS_Stub_Length);
  // Substrate-related parameters
  Stub.val["er"] = num2str(Specs.MS_Subs.er);
  Stub.val["h"] = num2str(Specs.MS_Subs.height);
  Stub.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  Stub.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  Stub.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(Stub);

  // Add stub termination (open or short)
  if (!Specs.OpenShort) { // Open stub
    ComponentInfo MSOPEN;
    MSOPEN.setParams(
        QString("MOPEN%1").arg(++Schematic.NumberComponents[MicrostripOpen]),
        MicrostripOpen, 0, 50, 100);
    // Physical parameters
    MSOPEN.val["Width"] = ConvertLengthFromM("mm", MS_Stub_Width);
    // Substrate-related parameters
    MSOPEN.val["er"] = num2str(Specs.MS_Subs.er);
    MSOPEN.val["h"] = num2str(Specs.MS_Subs.height);
    MSOPEN.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MSOPEN.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MSOPEN.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MSOPEN);

    // Wiring for open stub
    Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
    Schematic.appendWire(TLine.ID, 0, NI1.ID, 0);
    Schematic.appendWire(NI1.ID, 0, Stub.ID, 1);
    Schematic.appendWire(Stub.ID, 0, MSOPEN.ID, 0);
    Schematic.appendWire(Zload.ID, 1, TLine.ID, 1);
    Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
  } else { // Short stub
    ComponentInfo MSVIA;
    MSVIA.setParams(
        QString("MSVIA%1").arg(++Schematic.NumberComponents[MicrostripVia]),
        MicrostripVia, 0, 50, 100);
    // Physical parameters
    MSVIA.val["D"] = ConvertLengthFromM("mm", 0.5e-3); // Default: 0.5 mm
    MSVIA.val["N"] = QString::number(4); // Number of vias in parallel (4 vias)
    // Substrate-related parameters
    MSVIA.val["er"] = num2str(Specs.MS_Subs.er);
    MSVIA.val["h"] = num2str(Specs.MS_Subs.height);
    MSVIA.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MSVIA.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MSVIA.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MSVIA);

    // Wiring for short stub
    Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
    Schematic.appendWire(TLine.ID, 0, NI1.ID, 0);
    Schematic.appendWire(NI1.ID, 0, Stub.ID, 1);
    Schematic.appendWire(Stub.ID, 0, MSVIA.ID, 0);
    Schematic.appendWire(Zload.ID, 1, TLine.ID, 1);
    Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
  }
}
