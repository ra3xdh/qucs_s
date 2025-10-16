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

#include "Lim_Eom.h"

Lim_Eom::Lim_Eom() {}

Lim_Eom::Lim_Eom(PowerCombinerParams PS) { Specification = PS; }

Lim_Eom::~Lim_Eom() {}

void Lim_Eom::calculateParams() {
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);

  double M = Specification.OutputRatio.at(0);
  double N = Specification.OutputRatio.at(1);
  double K = Specification.OutputRatio.at(2);

  double d1 = M + N + K;
  double d2 = N + K;
  Z1 = Specification.Z0 * sqrt(d1 / d2);
  Z2 = Specification.Z0 * sqrt(d1 / M);
  Z4 = Specification.Z0 * sqrt(d2 / N);
  Z5 = Specification.Z0 * sqrt(d2 / K);
}

void Lim_Eom::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildLimEom_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildLimEom_Microstrip();
  }
}

void Lim_Eom::buildLimEom_IdealTL() {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -25,
      -100);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo N0(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, -100);
  Schematic.appendNode(N0);
  Schematic.appendWire(TermSpar1.ID, 0, N0.ID, 0);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, -100);
  TL1.val["Z0"] = num2str(Z2, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);
  Schematic.appendWire(TL1.ID, 0, N0.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, -100);
  Schematic.appendNode(N1);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 90, 100,
      -130);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, N1.ID, 0);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, -100);
  TL2.val["Z0"] = num2str(Z1, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);
  Schematic.appendWire(TL2.ID, 0, N1.ID, 0);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, -100);
  Schematic.appendNode(N2);
  Schematic.appendWire(TL2.ID, 1, N2.ID, 0);

  ComponentInfo Ri1(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 260, -60);
  Ri1.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Ri1);

  ComponentInfo Ground1(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 260, -20);
  Schematic.appendComponent(Ground1);
  Schematic.appendWire(Ri1.ID, 1, N2.ID, 0);
  Schematic.appendWire(Ri1.ID, 0, Ground1.ID, 0);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 200, -50);
  TL3.val["Z0"] = num2str(Z2, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);
  Schematic.appendWire(TL3.ID, 1, N2.ID, 0);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 0);
  Schematic.appendNode(N3);
  Schematic.appendWire(TL3.ID, 0, N3.ID, 0);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 200, 50);
  TL4.val["Z0"] = num2str(Z4, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);
  Schematic.appendWire(TL4.ID, 1, N3.ID, 0);

  NodeInfo N4(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 100);
  Schematic.appendNode(N4);
  Schematic.appendWire(TL4.ID, 0, N4.ID, 0);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 220,
      100);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, N4.ID, 0);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 100);
  TL5.val["Z0"] = num2str(Z5, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL5);
  Schematic.appendWire(TL5.ID, 1, N4.ID, 0);

  NodeInfo N5(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 100);
  Schematic.appendNode(N5);
  Schematic.appendWire(TL5.ID, 0, N5.ID, 0);

  ComponentInfo Ri2(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 100, 150);
  Ri2.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Ri2);

  ComponentInfo Ground2(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 100, 200);
  Schematic.appendComponent(Ground2);
  Schematic.appendWire(Ri2.ID, 1, N5.ID, 0);
  Schematic.appendWire(Ri2.ID, 0, Ground2.ID, 0);

  ComponentInfo TL6(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 100);
  TL6.val["Z0"] = num2str(Z4, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL6);
  Schematic.appendWire(TL6.ID, 1, N5.ID, 0);

  NodeInfo N6(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 100);
  Schematic.appendNode(N6);
  Schematic.appendWire(TL6.ID, 0, N6.ID, 0);

  ComponentInfo TermSpar4(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20,
      100);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);
  Schematic.appendWire(TermSpar4.ID, 0, N6.ID, 0);

  ComponentInfo TL7(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 50);
  TL7.val["Z0"] = num2str(Z5, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL7);
  Schematic.appendWire(TL7.ID, 0, N6.ID, 0);

  NodeInfo N7(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 0);
  Schematic.appendNode(N7);
  Schematic.appendWire(TL7.ID, 1, N7.ID, 0);

  ComponentInfo TL8(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, -50);
  TL8.val["Z0"] = num2str(Z1, Resistance);
  TL8.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL8);
  Schematic.appendWire(TL8.ID, 0, N7.ID, 0);
  Schematic.appendWire(TL8.ID, 1, N0.ID, 0);

  ComponentInfo TL9(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 100, 0);
  TL9.val["Z0"] = num2str(Specification.Z0, Resistance);
  TL9.val["Length"] = ConvertLengthFromM(Specification.units, 2 * lambda4);
  Schematic.appendComponent(TL9);
  Schematic.appendWire(TL9.ID, 0, N7.ID, 0);
  Schematic.appendWire(TL9.ID, 1, N3.ID, 0);
}

void Lim_Eom::buildLimEom_Microstrip() {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -25,
      -100);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo N0(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, -100);
  Schematic.appendNode(N0);
  Schematic.appendWire(TermSpar1.ID, 0, N0.ID, 0);

  // Synthesize microstrip lines for different impedances
  // Z2 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z2;
  MSL_Z2.Substrate = Specification.MS_Subs;
  MSL_Z2.synthesizeMicrostrip(Z2, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, -100);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);
  Schematic.appendWire(MLIN1.ID, 0, N0.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, -100);
  Schematic.appendNode(N1);
  Schematic.appendWire(MLIN1.ID, 1, N1.ID, 0);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 90, 100,
      -130);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, N1.ID, 0);

  // Z1 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z1;
  MSL_Z1.Substrate = Specification.MS_Subs;
  MSL_Z1.synthesizeMicrostrip(Z1, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, -100);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);
  Schematic.appendWire(MLIN2.ID, 0, N1.ID, 0);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, -100);
  Schematic.appendNode(N2);
  Schematic.appendWire(MLIN2.ID, 1, N2.ID, 0);

  ComponentInfo Ri1(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 260, -60);
  Ri1.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Ri1);

  ComponentInfo Ground1(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 260, -20);
  Schematic.appendComponent(Ground1);
  Schematic.appendWire(Ri1.ID, 1, N2.ID, 0);
  Schematic.appendWire(Ri1.ID, 0, Ground1.ID, 0);

  ComponentInfo MLIN3(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 200, -50);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);
  Schematic.appendWire(MLIN3.ID, 1, N2.ID, 0);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 0);
  Schematic.appendNode(N3);
  Schematic.appendWire(MLIN3.ID, 0, N3.ID, 0);

  // Z4 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z4;
  MSL_Z4.Substrate = Specification.MS_Subs;
  MSL_Z4.synthesizeMicrostrip(Z4, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN4(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 200, 50);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Z4.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_Z4.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);
  Schematic.appendWire(MLIN4.ID, 1, N3.ID, 0);

  NodeInfo N4(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 100);
  Schematic.appendNode(N4);
  Schematic.appendWire(MLIN4.ID, 0, N4.ID, 0);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 220,
      100);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, N4.ID, 0);

  // Z5 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z5;
  MSL_Z5.Substrate = Specification.MS_Subs;
  MSL_Z5.synthesizeMicrostrip(Z5, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN5(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, 100);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_Z5.Results.width);
  MLIN5.val["Length"] = ConvertLengthFromM("mm", MSL_Z5.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);
  Schematic.appendWire(MLIN5.ID, 1, N4.ID, 0);

  NodeInfo N5(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 100);
  Schematic.appendNode(N5);
  Schematic.appendWire(MLIN5.ID, 0, N5.ID, 0);

  ComponentInfo Ri2(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 100, 150);
  Ri2.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Ri2);

  ComponentInfo Ground2(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 100, 200);
  Schematic.appendComponent(Ground2);
  Schematic.appendWire(Ri2.ID, 1, N5.ID, 0);
  Schematic.appendWire(Ri2.ID, 0, Ground2.ID, 0);

  ComponentInfo MLIN6(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, 100);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_Z4.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_Z4.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN6);
  Schematic.appendWire(MLIN6.ID, 1, N5.ID, 0);

  NodeInfo N6(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 100);
  Schematic.appendNode(N6);
  Schematic.appendWire(MLIN6.ID, 0, N6.ID, 0);

  ComponentInfo TermSpar4(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20,
      100);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);
  Schematic.appendWire(TermSpar4.ID, 0, N6.ID, 0);

  ComponentInfo MLIN7(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 0, 50);
  MLIN7.val["Width"] = ConvertLengthFromM("mm", MSL_Z5.Results.width);
  MLIN7.val["Length"] = ConvertLengthFromM("mm", MSL_Z5.Results.length * 1e-3);
  MLIN7.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN7.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN7.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN7.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN7.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN7);
  Schematic.appendWire(MLIN7.ID, 0, N6.ID, 0);

  NodeInfo N7(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 0);
  Schematic.appendNode(N7);
  Schematic.appendWire(MLIN7.ID, 1, N7.ID, 0);

  ComponentInfo MLIN8(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 0, -50);
  MLIN8.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN8.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN8.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN8.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN8.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN8.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN8.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN8);
  Schematic.appendWire(MLIN8.ID, 0, N7.ID, 0);
  Schematic.appendWire(MLIN8.ID, 1, N0.ID, 0);

  // Half-wave line at Z0
  MicrostripClass MSL_Z0_Half;
  MSL_Z0_Half.Substrate = Specification.MS_Subs;
  MSL_Z0_Half.synthesizeMicrostrip(Specification.Z0, 2 * lambda4 * 1e3,
                                   Specification.freq);

  ComponentInfo MLIN9(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 100, 0);
  MLIN9.val["Width"] = ConvertLengthFromM("mm", MSL_Z0_Half.Results.width);
  MLIN9.val["Length"] =
      ConvertLengthFromM("mm", MSL_Z0_Half.Results.length * 1e-3);
  MLIN9.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN9.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN9.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN9.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN9.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN9);
  Schematic.appendWire(MLIN9.ID, 0, N7.ID, 0);
  Schematic.appendWire(MLIN9.ID, 1, N3.ID, 0);
}
