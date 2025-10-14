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

#include "PowerCombinerDesigner.h"

void PowerCombinerDesigner::DoubleBoxBranchline() {
  double K = Specs.OutputRatio.at(0);
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);
  double r = 1;
  double t = sqrt((1 + K) * r);
  double ZA = Specs.Z0 * sqrt(r * (t * t - r)) / (t - r);
  double ZD = Specs.Z0 * sqrt(r * (t * t - r)) / (t - 1);
  double ZB = Specs.Z0 * sqrt(r - (r * r) / (t * t));

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildDoubleBoxBranchline_IdealTL(lambda4, ZA, ZB, ZD);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildDoubleBoxBranchline_Microstrip(lambda4, ZA, ZB, ZD);
  }
}

void PowerCombinerDesigner::buildDoubleBoxBranchline_IdealTL(double lambda4,
                                                             double ZA,
                                                             double ZB,
                                                             double ZD) {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 0, -20,
      -50);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar1);

  NodeInfo NSP1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0,
      -50);
  SchContent.appendNode(NSP1);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 220,
      -50);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar2);

  NodeInfo NSP2(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      -50);
  SchContent.appendNode(NSP2);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 220,
      50);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar3);

  NodeInfo NSP3(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      50);
  SchContent.appendNode(NSP3);

  ComponentInfo Riso(
      QString("R%1").arg(++SchContent.NumberComponents[Resistor]), Resistor, 0,
      0, 75);
  Riso.val["R"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(Riso);

  NodeInfo NIso(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0,
      50);
  SchContent.appendNode(NIso);

  ComponentInfo Ground(QString("GND%1").arg(++SchContent.NumberComponents[GND]),
                       GND, 0, 0, 120);
  SchContent.appendComponent(Ground);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, -50);
  TL1.val["Z0"] = num2str(ZB, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL1);

  NodeInfo N1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      -50);
  SchContent.appendNode(N1);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 50);
  TL2.val["Z0"] = num2str(ZB, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL2);

  NodeInfo N2(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      50);
  SchContent.appendNode(N2);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 0);
  TL3.val["Z0"] = num2str(ZA, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL3);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 100, 0);
  TL4.val["Z0"] = num2str(ZB, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL4);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, -50);
  TL5.val["Z0"] = num2str(ZB, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL5);

  ComponentInfo TL6(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 50);
  TL6.val["Z0"] = num2str(ZB, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL6);

  ComponentInfo TL7(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 200, 0);
  TL7.val["Z0"] = num2str(ZD, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL7);

  SchContent.appendWire(TermSpar1.ID, 0, NSP1.ID, 0);
  SchContent.appendWire(TermSpar2.ID, 0, NSP2.ID, 0);
  SchContent.appendWire(TermSpar3.ID, 0, NSP3.ID, 0);
  SchContent.appendWire(Riso.ID, 1, NIso.ID, 0);

  SchContent.appendWire(NSP1.ID, 0, TL1.ID, 0);
  SchContent.appendWire(NSP2.ID, 0, TL5.ID, 1);
  SchContent.appendWire(NSP2.ID, 0, TL7.ID, 1);
  SchContent.appendWire(NSP3.ID, 0, TL7.ID, 0);
  SchContent.appendWire(NSP3.ID, 0, TL6.ID, 1);
  SchContent.appendWire(NSP1.ID, 0, TL3.ID, 1);
  SchContent.appendWire(NIso.ID, 1, TL2.ID, 0);
  SchContent.appendWire(NIso.ID, 1, TL3.ID, 0);
  SchContent.appendWire(Riso.ID, 0, Ground.ID, 0);

  SchContent.appendWire(TL1.ID, 1, N1.ID, 0);
  SchContent.appendWire(TL5.ID, 0, N1.ID, 0);
  SchContent.appendWire(TL4.ID, 1, N1.ID, 0);
  SchContent.appendWire(TL2.ID, 1, N2.ID, 0);
  SchContent.appendWire(TL4.ID, 0, N2.ID, 0);
  SchContent.appendWire(TL6.ID, 0, N2.ID, 0);
}

void PowerCombinerDesigner::buildDoubleBoxBranchline_Microstrip(double lambda4,
                                                                double ZA,
                                                                double ZB,
                                                                double ZD) {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 0, -20,
      -50);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar1);

  NodeInfo NSP1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0,
      -50);
  SchContent.appendNode(NSP1);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 220,
      -50);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar2);

  NodeInfo NSP2(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      -50);
  SchContent.appendNode(NSP2);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 220,
      50);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar3);

  NodeInfo NSP3(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      50);
  SchContent.appendNode(NSP3);

  ComponentInfo Riso(
      QString("R%1").arg(++SchContent.NumberComponents[Resistor]), Resistor, 0,
      0, 75);
  Riso.val["R"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(Riso);

  NodeInfo NIso(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0,
      50);
  SchContent.appendNode(NIso);

  ComponentInfo Ground(QString("GND%1").arg(++SchContent.NumberComponents[GND]),
                       GND, 0, 0, 120);
  SchContent.appendComponent(Ground);

  // Synthesize microstrip lines for each impedance
  // ZB impedance lines (most of the structure)
  MicrostripClass MSL_ZB;
  MSL_ZB.Substrate = Specs.MS_Subs;
  MSL_ZB.synthesizeMicrostrip(ZB, lambda4 * 1e3, Specs.freq);

  // ZA impedance line
  MicrostripClass MSL_ZA;
  MSL_ZA.Substrate = Specs.MS_Subs;
  MSL_ZA.synthesizeMicrostrip(ZA, lambda4 * 1e3, Specs.freq);

  // ZD impedance line
  MicrostripClass MSL_ZD;
  MSL_ZD.Substrate = Specs.MS_Subs;
  MSL_ZD.synthesizeMicrostrip(ZD, lambda4 * 1e3, Specs.freq);

  // Left box - Top vertical line (ZB)
  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, -50);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN1);

  NodeInfo N1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      -50);
  SchContent.appendNode(N1);

  // Left box - Bottom vertical line (ZB)
  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, 50);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN2);

  NodeInfo N2(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      50);
  SchContent.appendNode(N2);

  // Left box - Left horizontal line (ZA)
  ComponentInfo MLIN3(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 0, 0);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_ZA.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_ZA.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN3);

  // Center horizontal line (ZB)
  ComponentInfo MLIN4(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 100, 0);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN4);

  // Right box - Top vertical line (ZB)
  ComponentInfo MLIN5(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, -50);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN5.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN5);

  // Right box - Bottom vertical line (ZB)
  ComponentInfo MLIN6(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, 50);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN6);

  // Right box - Right horizontal line (ZD)
  ComponentInfo MLIN7(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 200, 0);
  MLIN7.val["Width"] = ConvertLengthFromM("mm", MSL_ZD.Results.width);
  MLIN7.val["Length"] = ConvertLengthFromM("mm", MSL_ZD.Results.length * 1e-3);
  MLIN7.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN7.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN7.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN7.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN7.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN7);

  // Wire connections
  SchContent.appendWire(TermSpar1.ID, 0, NSP1.ID, 0);
  SchContent.appendWire(TermSpar2.ID, 0, NSP2.ID, 0);
  SchContent.appendWire(TermSpar3.ID, 0, NSP3.ID, 0);
  SchContent.appendWire(Riso.ID, 1, NIso.ID, 0);

  SchContent.appendWire(NSP1.ID, 0, MLIN1.ID, 0);
  SchContent.appendWire(NSP2.ID, 0, MLIN5.ID, 1);
  SchContent.appendWire(NSP2.ID, 0, MLIN7.ID, 1);
  SchContent.appendWire(NSP3.ID, 0, MLIN7.ID, 0);
  SchContent.appendWire(NSP3.ID, 0, MLIN6.ID, 1);
  SchContent.appendWire(NSP1.ID, 0, MLIN3.ID, 1);
  SchContent.appendWire(NIso.ID, 1, MLIN2.ID, 0);
  SchContent.appendWire(NIso.ID, 1, MLIN3.ID, 0);
  SchContent.appendWire(Riso.ID, 0, Ground.ID, 0);

  SchContent.appendWire(MLIN1.ID, 1, N1.ID, 0);
  SchContent.appendWire(MLIN5.ID, 0, N1.ID, 0);
  SchContent.appendWire(MLIN4.ID, 1, N1.ID, 0);
  SchContent.appendWire(MLIN2.ID, 1, N2.ID, 0);
  SchContent.appendWire(MLIN4.ID, 0, N2.ID, 0);
  SchContent.appendWire(MLIN6.ID, 0, N2.ID, 0);
}
