/// @file Lambda8Lambda4.cpp
/// @brief Lambda/8 + Lambda/4 matching network synthesis (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "Lambda8Lambda4.h"

void Lambda8Lambda4::synthesize() {
  // Calculate matching impedances
  // Reference: Inder J. Bahl. "Fundamentals of RF and microwave transistor
  // amplifiers". John Wiley and Sons. 2009. Pages 159 - 160
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double XL = Specs.ZL.imag();

  double Zmm = std::sqrt(RL * RL + XL * XL);
  double Zm = std::sqrt((Z0 * RL * Zmm) / (Zmm - XL));

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildMatchingNetwork_IdealTL(Zm, Zmm, XL);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildMatchingNetwork_Microstrip(Zm, Zmm, XL);
  }
}

void Lambda8Lambda4::buildMatchingNetwork_IdealTL(double Zm, double Zmm,
                                                  double XL) {
  double l4 = SPEED_OF_LIGHT / (4. * f_match);
  double l8 = 0.5 * l4;

  // Port 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  int x_pos = 50;
  ComponentInfo TL1, TL2;

  // First transmission line: Zm, length λ/4
  TL1.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, x_pos, 0);
  TL1.val["Z0"] = num2str(Zm, Resistance);
  TL1.val["Length"] = ConvertLengthFromM("mm", l4);
  Schematic.appendComponent(TL1);

  if (XL != 0) {
    // Second transmission line: Zmm, length λ/8
    x_pos += 50;
    TL2.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, x_pos, 0);
    TL2.val["Z0"] = num2str(Zmm, Resistance);
    TL2.val["Length"] = ConvertLengthFromM("mm", l8);
    Schematic.appendComponent(TL2);
  }

  // Load
  x_pos += 50;
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, x_pos, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, x_pos, 100);
  Schematic.appendComponent(GND_ZL);

  // Wiring
  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);
  if (XL != 0) {
    Schematic.appendWire(TL1.ID, 1, TL2.ID, 0);
    Schematic.appendWire(Zload.ID, 1, TL2.ID, 1);
  } else {
    Schematic.appendWire(Zload.ID, 1, TL1.ID, 1);
  }
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}

void Lambda8Lambda4::buildMatchingNetwork_Microstrip(double Zm, double Zmm,
                                                     double XL) {
  double l4 = SPEED_OF_LIGHT / (4. * f_match);
  double l8 = 0.5 * l4;

  // Port 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  int x_pos = 50;
  ComponentInfo TL1, TL2, MStep;
  MicrostripClass MSL;

  // First microstrip line: Zm, length λ/4
  MSL.Substrate = Specs.MS_Subs;
  MSL.synthesizeMicrostrip(Zm, l4 * 1e3, f_match);

  double MS_Width = MSL.Results.width;
  double MS_Length = MSL.Results.length * 1e-3;

  TL1.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, -90, x_pos, 0);
  TL1.val["Width"] = ConvertLengthFromM("mm", MS_Width);
  TL1.val["Length"] = ConvertLengthFromM("mm", MS_Length);
  TL1.val["er"] = num2str(Specs.MS_Subs.er);
  TL1.val["h"] = num2str(Specs.MS_Subs.height);
  TL1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  TL1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  TL1.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(TL1);

  if (XL != 0) {
    // Microstrip step
    x_pos += 60;
    MStep.ID =
        QString("MSTEP%1").arg(++Schematic.NumberComponents[MicrostripStep]);
    MStep.Type = MicrostripStep;
    MStep.Rotation = 0;
    MStep.Coordinates = {static_cast<double>(x_pos), 0};
    MStep.val["W1"] = TL1.val["Width"];
    MStep.val["er"] = num2str(Specs.MS_Subs.er);
    MStep.val["h"] = num2str(Specs.MS_Subs.height);
    MStep.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MStep.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MStep.val["tand"] = num2str(Specs.MS_Subs.tand);

    // Second microstrip line: Zmm, length λ/8
    x_pos += 60;
    MSL.Substrate = Specs.MS_Subs;
    MSL.synthesizeMicrostrip(Zmm, l8 * 1e3, f_match);

    MS_Width = MSL.Results.width;
    MS_Length = MSL.Results.length * 1e-3;

    TL2.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, -90, x_pos, 0);
    TL2.val["Width"] = ConvertLengthFromM("mm", MS_Width);
    TL2.val["Length"] = ConvertLengthFromM("mm", MS_Length);
    TL2.val["er"] = num2str(Specs.MS_Subs.er);
    TL2.val["h"] = num2str(Specs.MS_Subs.height);
    TL2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    TL2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    TL2.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(TL2);

    MStep.val["W2"] = TL2.val["Width"];
    Schematic.appendComponent(MStep);
  }

  // Load
  x_pos += 50;
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, x_pos, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, x_pos, 100);
  Schematic.appendComponent(GND_ZL);

  // Wiring
  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 1);
  if (XL != 0) {
    Schematic.appendWire(TL1.ID, 0, MStep.ID, 0);
    Schematic.appendWire(MStep.ID, 1, TL2.ID, 1);
    Schematic.appendWire(Zload.ID, 1, TL2.ID, 0);
  } else {
    Schematic.appendWire(Zload.ID, 1, TL1.ID, 0);
  }
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}
