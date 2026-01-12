/// @file DoubleStub.cpp
/// @brief Double stub matching network synthesis (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "DoubleStub.h"

// Reference: 'Microwave Engineering', David Pozar. John Wiley and Sons. 4th
// Edition. Pg 241-245
void DoubleStub::synthesize() {
  double lambda = SPEED_OF_LIGHT / f_match;
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double XL = Specs.ZL.imag();

  // Calculate susceptances and stub lengths
  double d = lambda / 8; // Fixed spacing between stubs
  auto [lstub1, lstub2] = calculateStubLengths(lambda, Z0, RL, XL, d);

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildMatchingNetwork_Ideal(d, lstub1, lstub2);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildMatchingNetwork_Microstrip(d, lstub1, lstub2);
  }
}

std::pair<double, double> DoubleStub::calculateStubLengths(double lambda,
                                                           double Z0, double RL,
                                                           double XL,
                                                           double d) {
  double Y0 = 1.0 / Z0;
  double GL = (1 / ((RL * RL) + (XL * XL))) * RL;
  double BL = -(1 / ((RL * RL) + (XL * XL))) * XL;
  double beta = (2 * M_PI) / lambda;
  double t = tan(beta * d);

  // Check if load can be matched using double stub method
  if (GL > Y0 * ((1 + t * t) / (2 * t * t))) {
    // Cannot match this load - could throw exception or handle error
    // For now, we'll proceed with the calculation but the result may not be
    // optimal
  }

  // Calculate stub susceptances
  double B11 = -BL + (Y0 + sqrt((1 + t * t) * GL * Y0 - GL * GL * t * t)) / t;
  double B21 =
      ((Y0 * sqrt((1 + t * t) * GL * Y0 - GL * GL * t * t)) + GL * Y0) /
      (GL * t);

  // Calculate stub lengths
  double ll1, ll2;
  if (!Specs.OpenShort) { // Open stubs
    ll1 = (atan(B21 * Z0)) / (2 * M_PI);
    ll2 = (atan(B11 * Z0)) / (2 * M_PI);
  } else { // Short stubs
    ll1 = -(atan(1.0 / (B21 * Z0))) / (2 * M_PI);
    ll2 = -(atan(1.0 / (B11 * Z0))) / (2 * M_PI);
  }

  // Normalize lengths
  if (ll1 < 0) {
    ll1 += 0.5;
  }
  if (ll2 < 0) {
    ll2 += 0.5;
  }
  if (Specs.OpenShort && (ll1 > 0.5)) {
    ll1 -= 0.5;
  }
  if (Specs.OpenShort && (ll2 > 0.5)) {
    ll2 -= 0.5;
  }

  return {ll1 * lambda, ll2 * lambda};
}

void DoubleStub::buildMatchingNetwork_Ideal(double d, double lstub1,
                                            double lstub2) {
  double Z0 = Specs.Z0;

  // Port 1 termination
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Load impedance
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, 250, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, 250, 100);
  Schematic.appendComponent(GND_ZL);

  // Node between first stub and transmission line
  NodeInfo NI1;
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Node between transmission line and second stub
  NodeInfo NI2;
  NI2.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 175,
      0);
  Schematic.appendNode(NI2);

  // Create first stub component (at input side)
  ComponentInfo Stub1;
  if (!Specs.OpenShort) { // Open stub
    Stub1.setParams(
        QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]),
        OpenStub, 0, 50, 50);
  } else { // Short stub
    Stub1.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
        ShortStub, 0, 50, 50);
  }
  Stub1.val["Z0"] = num2str(Z0, Resistance);
  Stub1.val["Length"] = ConvertLengthFromM("mm", lstub1);
  Schematic.appendComponent(Stub1);

  // Create transmission line component (lambda/8 spacing)
  ComponentInfo TLine;
  TLine.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 112, 0);
  TLine.val["Z0"] = num2str(Z0, Resistance);
  TLine.val["Length"] = ConvertLengthFromM("mm", d);
  Schematic.appendComponent(TLine);

  // Create second stub component (at load side)
  ComponentInfo Stub2;
  if (!Specs.OpenShort) { // Open stub
    Stub2.setParams(
        QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]),
        OpenStub, 0, 175, 50);
  } else { // Short stub
    Stub2.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
        ShortStub, 0, 175, 50);
  }
  Stub2.val["Z0"] = num2str(Z0, Resistance);
  Stub2.val["Length"] = ConvertLengthFromM("mm", lstub2);
  Schematic.appendComponent(Stub2);

  // Wires
  Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
  Schematic.appendWire(NI1.ID, 0, Stub1.ID, 1);
  Schematic.appendWire(NI1.ID, 0, TLine.ID, 0);
  Schematic.appendWire(TLine.ID, 1, NI2.ID, 0);
  Schematic.appendWire(NI2.ID, 0, Stub2.ID, 1);
  Schematic.appendWire(Zload.ID, 1, NI2.ID, 0);
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}

void DoubleStub::buildMatchingNetwork_Microstrip(double d, double lstub1,
                                                 double lstub2) {
  double Z0 = Specs.Z0;

  // Port 1 termination
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Load impedance
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, 250, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, 250, 100);
  Schematic.appendComponent(GND_ZL);

  // Node between first stub and transmission line
  NodeInfo NI1;
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Node between transmission line and second stub
  NodeInfo NI2;
  NI2.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 175,
      0);
  Schematic.appendNode(NI2);

  // Synthesize microstrip transmission line
  MicrostripClass MSL_Line;
  MSL_Line.Substrate = Specs.MS_Subs;
  MSL_Line.synthesizeMicrostrip(Z0, d * 1e3, f_match);
  double MS_Line_Width = MSL_Line.Results.width;
  double MS_Line_Length = MSL_Line.Results.length * 1e-3;

  ComponentInfo TLine;
  TLine.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 112, 0);
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

  // Synthesize first microstrip stub
  MicrostripClass MSL_Stub1;
  MSL_Stub1.Substrate = Specs.MS_Subs;
  MSL_Stub1.synthesizeMicrostrip(Z0, lstub1 * 1e3, f_match);
  double MS_Stub1_Width = MSL_Stub1.Results.width;
  double MS_Stub1_Length = MSL_Stub1.Results.length * 1e-3;

  ComponentInfo Stub1;
  Stub1.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 50, 50);
  // Physical parameters
  Stub1.val["Width"] = ConvertLengthFromM("mm", MS_Stub1_Width);
  Stub1.val["Length"] = ConvertLengthFromM("mm", MS_Stub1_Length);
  // Substrate-related parameters
  Stub1.val["er"] = num2str(Specs.MS_Subs.er);
  Stub1.val["h"] = num2str(Specs.MS_Subs.height);
  Stub1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  Stub1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  Stub1.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(Stub1);

  // Synthesize second microstrip stub
  MicrostripClass MSL_Stub2;
  MSL_Stub2.Substrate = Specs.MS_Subs;
  MSL_Stub2.synthesizeMicrostrip(Z0, lstub2 * 1e3, f_match);
  double MS_Stub2_Width = MSL_Stub2.Results.width;
  double MS_Stub2_Length = MSL_Stub2.Results.length * 1e-3;

  ComponentInfo Stub2;
  Stub2.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 175, 50);
  // Physical parameters
  Stub2.val["Width"] = ConvertLengthFromM("mm", MS_Stub2_Width);
  Stub2.val["Length"] = ConvertLengthFromM("mm", MS_Stub2_Length);
  // Substrate-related parameters
  Stub2.val["er"] = num2str(Specs.MS_Subs.er);
  Stub2.val["h"] = num2str(Specs.MS_Subs.height);
  Stub2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  Stub2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  Stub2.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(Stub2);

  // Add stub terminations (open or short)
  if (!Specs.OpenShort) { // Open stubs
    ComponentInfo MSOPEN1;
    MSOPEN1.setParams(
        QString("MOPEN%1").arg(++Schematic.NumberComponents[MicrostripOpen]),
        MicrostripOpen, 0, 50, 100);
    // Physical parameters
    MSOPEN1.val["Width"] = ConvertLengthFromM("mm", MS_Stub1_Width);
    // Substrate-related parameters
    MSOPEN1.val["er"] = num2str(Specs.MS_Subs.er);
    MSOPEN1.val["h"] = num2str(Specs.MS_Subs.height);
    MSOPEN1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MSOPEN1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MSOPEN1.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MSOPEN1);

    ComponentInfo MSOPEN2;
    MSOPEN2.setParams(
        QString("MOPEN%1").arg(++Schematic.NumberComponents[MicrostripOpen]),
        MicrostripOpen, 0, 175, 100);
    // Physical parameters
    MSOPEN2.val["Width"] = ConvertLengthFromM("mm", MS_Stub2_Width);
    // Substrate-related parameters
    MSOPEN2.val["er"] = num2str(Specs.MS_Subs.er);
    MSOPEN2.val["h"] = num2str(Specs.MS_Subs.height);
    MSOPEN2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MSOPEN2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MSOPEN2.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MSOPEN2);

    // Wiring for open stubs
    Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
    Schematic.appendWire(NI1.ID, 0, Stub1.ID, 1);
    Schematic.appendWire(Stub1.ID, 0, MSOPEN1.ID, 0);
    Schematic.appendWire(NI1.ID, 0, TLine.ID, 0);
    Schematic.appendWire(TLine.ID, 1, NI2.ID, 0);
    Schematic.appendWire(NI2.ID, 0, Stub2.ID, 1);
    Schematic.appendWire(Stub2.ID, 0, MSOPEN2.ID, 0);
    Schematic.appendWire(Zload.ID, 1, NI2.ID, 0);
    Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
  } else { // Short stubs
    ComponentInfo MSVIA1;
    MSVIA1.setParams(
        QString("MSVIA%1").arg(++Schematic.NumberComponents[MicrostripVia]),
        MicrostripVia, 0, 50, 100);
    // Physical parameters
    MSVIA1.val["D"] = ConvertLengthFromM("mm", 0.5e-3); // Default: 0.5 mm
    MSVIA1.val["N"] = QString::number(4); // Number of vias in parallel (4 vias)
    // Substrate-related parameters
    MSVIA1.val["er"] = num2str(Specs.MS_Subs.er);
    MSVIA1.val["h"] = num2str(Specs.MS_Subs.height);
    MSVIA1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MSVIA1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MSVIA1.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MSVIA1);

    ComponentInfo MSVIA2;
    MSVIA2.setParams(
        QString("MSVIA%1").arg(++Schematic.NumberComponents[MicrostripVia]),
        MicrostripVia, 0, 175, 100);
    // Physical parameters
    MSVIA2.val["D"] = ConvertLengthFromM("mm", 0.5e-3); // Default: 0.5 mm
    MSVIA2.val["N"] = QString::number(4); // Number of vias in parallel (4 vias)
    // Substrate-related parameters
    MSVIA2.val["er"] = num2str(Specs.MS_Subs.er);
    MSVIA2.val["h"] = num2str(Specs.MS_Subs.height);
    MSVIA2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MSVIA2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MSVIA2.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MSVIA2);

    // Wiring for short stubs
    Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
    Schematic.appendWire(NI1.ID, 0, Stub1.ID, 1);
    Schematic.appendWire(Stub1.ID, 0, MSVIA1.ID, 0);
    Schematic.appendWire(NI1.ID, 0, TLine.ID, 0);
    Schematic.appendWire(TLine.ID, 1, NI2.ID, 0);
    Schematic.appendWire(NI2.ID, 0, Stub2.ID, 1);
    Schematic.appendWire(Stub2.ID, 0, MSVIA2.ID, 0);
    Schematic.appendWire(Zload.ID, 1, NI2.ID, 0);
    Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
  }
}
