/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

#include "Wilkinson2Way.h"

Wilkinson2Way::Wilkinson2Way() {}

Wilkinson2Way::Wilkinson2Way(PowerCombinerParams PS) { Specification = PS; }

Wilkinson2Way::~Wilkinson2Way() {}

void Wilkinson2Way::calculateParams() {
  double K = Specification.OutputRatio.at(0);
  Z2 = Specification.Z0 * sqrt(2 * (1 + K) / K);
  Z3 = Specification.Z0 * sqrt(2 * (1 + K));
  R2 = Specification.Z0 * K;
  R3 = Specification.Z0 / K;
  R = R2 + R3;
}

void Wilkinson2Way::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Lumped) {
    buildWilkinson_LumpedLC();
  } else if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildWilkinson_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildWilkinson_Microstrip();
  }
}

void Wilkinson2Way::buildWilkinson_LumpedLC() {
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo Ground;

  // Design equations
  double Z4, Z5, L2_, C2_, L3_, C3_;
  double K = Specification.OutputRatio.at(0);
  double w = 2 * M_PI * Specification.freq;
  double L2 = Z2 / w;
  double C2 = 1. / (L2 * w * w);
  double L3 = Z3 / w;
  double C3 = 1. / (L3 * w * w);
  double CC = C2 + C3;

  if (R2 != R3) {
    Z4 = Specification.Z0 * sqrt(K);
    Z5 = Specification.Z0 / sqrt(K);
    L2_ = Z4 / w;
    L3_ = Z5 / w;
    C2_ = 1. / (L2_ * w * w);
    C3_ = 1. / (L3_ * w * w);
    C2 += C2_;
    C3 += C3_;
  }

  // Build the circuit and the netlist
  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, 50, 0);
  TermSpar1.val["Z0"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Shunt capacitor
  ComponentInfo Cshunt1(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0,
      100, 20);
  Cshunt1.val["C"] = num2str(CC, Capacitance);
  Schematic.appendComponent(Cshunt1);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 65);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Cshunt1.ID, 0, Ground.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 0);
  Schematic.appendNode(N1);

  Schematic.appendWire(Cshunt1.ID, 1, N1.ID, 0);
  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 1);

  // Upper branch - Series inductor
  ComponentInfo Lseries1(
      QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor, -90,
      150, -75);
  Lseries1.val["L"] = num2str(L2, Inductance);
  Schematic.appendComponent(Lseries1);

  // Cshunt
  ComponentInfo Cshunt2(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0,
      200, -55);
  Cshunt2.val["C"] = num2str(C2, Capacitance);
  Schematic.appendComponent(Cshunt2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 200, -10);
  Schematic.appendComponent(Ground);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              250, -75);
  Schematic.appendNode(N2);

  NodeInfo N2_(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
      -75);
  Schematic.appendNode(N2_);

  Schematic.appendWire(N1.ID, 0, Lseries1.ID, 1);
  Schematic.appendWire(Lseries1.ID, 0, N2_.ID, 0);
  Schematic.appendWire(Cshunt2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Cshunt2.ID, 1, N2_.ID, 0);
  Schematic.appendWire(N2_.ID, 0, N2.ID, 0);

  // Lower branch - Series inductor
  ComponentInfo Lseries2(
      QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor, -90,
      150, 75);
  Lseries2.val["L"] = num2str(L3, Inductance);
  Schematic.appendComponent(Lseries2);

  // Cshunt
  ComponentInfo Cshunt3(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0,
      200, 95);
  Cshunt3.val["C"] = num2str(C3, Capacitance);
  Schematic.appendComponent(Cshunt3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 200, 130);
  Schematic.appendComponent(Ground);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              250, 75);
  Schematic.appendNode(N3);

  NodeInfo N3_(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
      75);
  Schematic.appendNode(N3_);

  Schematic.appendWire(N1.ID, 0, Lseries2.ID, 1);
  Schematic.appendWire(Lseries2.ID, 0, N3_.ID, 0);
  Schematic.appendWire(Cshunt3.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Cshunt3.ID, 1, N3_.ID, 0);
  Schematic.appendWire(N3_.ID, 0, N3.ID, 0);

  ComponentInfo Risolation(
      QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
      250, 0);
  Risolation.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Risolation);

  Schematic.appendWire(Risolation.ID, 0, N3.ID, 1);
  Schematic.appendWire(Risolation.ID, 1, N2.ID, 0);

  ComponentInfo Ls3, Ls4, Cp4, Cp5;
  if (Specification.OutputRatio.at(0) != 1) {
    // Upper branch
    Ls3.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                  Inductor, -90, 285, -75);
    Ls3.val["L"] = num2str(L2_, Inductance);
    Schematic.appendComponent(Ls3);

    Schematic.appendWire(Ls3.ID, 1, N2.ID, 0);

    Cp4.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                  Capacitor, 0, 320, -50);
    Cp4.val["C"] = num2str(C2_, Capacitance);
    Schematic.appendComponent(Cp4);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, 320, -10);
    Schematic.appendComponent(Ground);

    NodeInfo N4(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 320,
        -75);
    Schematic.appendNode(N4);

    Schematic.appendWire(Ls3.ID, 0, N4.ID, 0);
    Schematic.appendWire(Cp4.ID, 1, N4.ID, 0);
    Schematic.appendWire(Cp4.ID, 0, Ground.ID, 0);

    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 370, -50);
    TermSpar2.val["Z0"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);

    Schematic.appendWire(N4.ID, 0, TermSpar2.ID, 0);

    // Lower branch
    Ls4.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                  Inductor, -90, 285, 75);
    Ls4.val["L"] = num2str(L3_, Inductance);
    Schematic.appendComponent(Ls4);

    Schematic.appendWire(Ls4.ID, 1, N3.ID, 0);

    Cp5.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                  Capacitor, 0, 320, 100);
    Cp5.val["C"] = num2str(C3_, Capacitance);
    Schematic.appendComponent(Cp5);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, 320, 140);
    Schematic.appendComponent(Ground);

    NodeInfo N5(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 320,
        75);
    Schematic.appendNode(N5);

    Schematic.appendWire(Ls4.ID, 0, N5.ID, 0);
    Schematic.appendWire(Cp5.ID, 1, N5.ID, 0);
    Schematic.appendWire(Cp5.ID, 0, Ground.ID, 0);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 370, 75);
    TermSpar3.val["Z0"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);

    Schematic.appendWire(N5.ID, 0, TermSpar3.ID, 0);
  } else {
    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 280, -75);
    TermSpar2.val["Z0"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);
    Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 280, 75);
    TermSpar3.val["Z0"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);
    Schematic.appendWire(TermSpar3.ID, 0, N3.ID, 0);
  }
}

void Wilkinson2Way::buildWilkinson_IdealTL() {
  double lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // 1st transmission line
  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 0);
  TL1.val["Z0"] = num2str(Specification.Z0, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);

  // Node
  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 0);
  Schematic.appendNode(N1);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 1);

  // Upper branch TL
  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 135, -50);
  TL2.val["Z0"] = num2str(Z2, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL2.ID, 0, N1.ID, 0);
  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, -50);
  Schematic.appendNode(N2);

  Schematic.appendWire(TL2.ID, 1, N2.ID, 0);

  // Lower branch TL
  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 135, 50);
  TL3.val["Z0"] = num2str(Z3, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  Schematic.appendWire(TL3.ID, 0, N1.ID, 0);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 50);
  Schematic.appendNode(N3);
  Schematic.appendWire(TL3.ID, 1, N3.ID, 0);

  // Isolation resistor
  ComponentInfo Riso(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                     Resistor, 0, 200, 0);
  Riso.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Riso);

  Schematic.appendWire(Riso.ID, 1, N2.ID, 0);
  Schematic.appendWire(Riso.ID, 0, N3.ID, 0);

  if (Specification.OutputRatio.at(0) != 1) {
    // Upper branch matching transmission line
    ComponentInfo TL4(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, 275, -50);
    TL4.val["Z0"] = num2str(sqrt(Specification.Z0 * R2), Resistance);
    TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL4);

    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 325, -50);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);

    Schematic.appendWire(TL4.ID, 0, N2.ID, 0);
    Schematic.appendWire(TL4.ID, 1, TermSpar2.ID, 0);

    // Lower branch matching transmission line
    ComponentInfo TL5(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, 275, 50);
    TL5.val["Z0"] = num2str(sqrt(Specification.Z0 * R3), Resistance);
    TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL5);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 325, 50);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);

    Schematic.appendWire(TL5.ID, 0, N3.ID, 0);
    Schematic.appendWire(TL5.ID, 1, TermSpar3.ID, 0);

  } else {
    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 250, -50);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);
    Schematic.appendWire(N2.ID, 1, TermSpar2.ID, 0);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 250, 50);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);
    Schematic.appendWire(N3.ID, 1, TermSpar3.ID, 0);
  }
}

void Wilkinson2Way::buildWilkinson_Microstrip() {
  double lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Input quarter-wave microstrip line
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specification.MS_Subs;
  MSL_Input.synthesizeMicrostrip(Specification.Z0, lambda4 * 1e3,
                                 Specification.freq);

  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, 0);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  Schematic.appendWire(TermSpar1.ID, 0, MLIN1.ID, 0);

  // Node
  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 0);
  Schematic.appendNode(N1);
  Schematic.appendWire(MLIN1.ID, 1, N1.ID, 1);

  // Upper branch microstrip line
  MicrostripClass MSL_Upper;
  MSL_Upper.Substrate = Specification.MS_Subs;
  MSL_Upper.synthesizeMicrostrip(Z2, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 135, -50);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Upper.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Upper.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  Schematic.appendWire(MLIN2.ID, 0, N1.ID, 0);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, -50);
  Schematic.appendNode(N2);
  Schematic.appendWire(MLIN2.ID, 1, N2.ID, 0);

  // Lower branch microstrip line
  MicrostripClass MSL_Lower;
  MSL_Lower.Substrate = Specification.MS_Subs;
  MSL_Lower.synthesizeMicrostrip(Z3, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN3(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 135, 50);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Lower.Results.width);
  MLIN3.val["Length"] =
      ConvertLengthFromM("mm", MSL_Lower.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  Schematic.appendWire(MLIN3.ID, 0, N1.ID, 0);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 50);
  Schematic.appendNode(N3);
  Schematic.appendWire(MLIN3.ID, 1, N3.ID, 0);

  // Isolation resistor
  ComponentInfo Riso(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                     Resistor, 0, 200, 0);
  Riso.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Riso);

  Schematic.appendWire(Riso.ID, 1, N2.ID, 0);
  Schematic.appendWire(Riso.ID, 0, N3.ID, 0);

  if (Specification.OutputRatio.at(0) != 1) {
    // Upper branch matching microstrip line
    MicrostripClass MSL_Match_Upper;
    MSL_Match_Upper.Substrate = Specification.MS_Subs;
    double Z_match_upper = sqrt(Specification.Z0 * R2);
    MSL_Match_Upper.synthesizeMicrostrip(Z_match_upper, lambda4 * 1e3,
                                         Specification.freq);

    ComponentInfo MLIN4(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, 275, -50);
    MLIN4.val["Width"] =
        ConvertLengthFromM("mm", MSL_Match_Upper.Results.width);
    MLIN4.val["Length"] =
        ConvertLengthFromM("mm", MSL_Match_Upper.Results.length * 1e-3);
    MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
    MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
    MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MLIN4);

    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 325, -50);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);

    Schematic.appendWire(MLIN4.ID, 0, N2.ID, 0);
    Schematic.appendWire(MLIN4.ID, 1, TermSpar2.ID, 0);

    // Lower branch matching microstrip line
    MicrostripClass MSL_Match_Lower;
    MSL_Match_Lower.Substrate = Specification.MS_Subs;
    double Z_match_lower = sqrt(Specification.Z0 * R3);
    MSL_Match_Lower.synthesizeMicrostrip(Z_match_lower, lambda4 * 1e3,
                                         Specification.freq);

    ComponentInfo MLIN5(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, 275, 50);
    MLIN5.val["Width"] =
        ConvertLengthFromM("mm", MSL_Match_Lower.Results.width);
    MLIN5.val["Length"] =
        ConvertLengthFromM("mm", MSL_Match_Lower.Results.length * 1e-3);
    MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
    MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
    MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MLIN5);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 325, 50);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);

    Schematic.appendWire(MLIN5.ID, 0, N3.ID, 0);
    Schematic.appendWire(MLIN5.ID, 1, TermSpar3.ID, 0);

  } else {
    // Equal power split - no matching required
    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 250, -50);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);
    Schematic.appendWire(N2.ID, 1, TermSpar2.ID, 0);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, 250, 50);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);
    Schematic.appendWire(N3.ID, 1, TermSpar3.ID, 0);
  }
}
