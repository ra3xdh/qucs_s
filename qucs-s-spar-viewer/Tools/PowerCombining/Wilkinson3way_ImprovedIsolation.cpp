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

/*
 * References:
 * [1] "Power combiners, impedance transformers and directional couplers: part
 * II". Andrei Grebennikov. High Frequency Electronics. 2008
 * [2] "New 3N way hybrid power dividers", IEEE Trans. Microwave Theory Tech.,
 * vol. MTT-25, Dec. 1977, pp. 1008-1012
 */
void PowerCombinerDesigner::Wilkinson3Way_ImprovedIsolation() {
  double Z1 = 2.28 * Specs.Z0;
  double Z2 = 1.316 * Specs.Z0;
  double R1 = 1.319 * Specs.Z0;
  double R2 = 4 * Specs.Z0;
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildWilkinson3Way_IdealTL(lambda4, Z1, Z2, R1, R2);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildWilkinson3Way_Microstrip(lambda4, Z1, Z2, R1, R2);
  }
}

void PowerCombinerDesigner::buildWilkinson3Way_IdealTL(double lambda4,
                                                       double Z1, double Z2,
                                                       double R1, double R2) {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 0, -25, 0);
  TermSpar1.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar1);

  NodeInfo N0(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0, 0);
  SchContent.appendNode(N0);
  SchContent.appendWire(TermSpar1.ID, 0, N0.ID, 0);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 0);
  TL1.val["Z0"] = num2str(Z1, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL1);

  NodeInfo N1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      0);
  SchContent.appendNode(N1);
  SchContent.appendWire(TL1.ID, 0, N0.ID, 0);
  SchContent.appendWire(TL1.ID, 1, N1.ID, 0);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, -100);
  TL2.val["Z0"] = num2str(Z1, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL2);

  NodeInfo N2(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      -100);
  SchContent.appendNode(N2);
  SchContent.appendWire(TL2.ID, 0, N0.ID, 0);
  SchContent.appendWire(TL2.ID, 1, N2.ID, 0);

  ComponentInfo Ri1(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 100, -50);
  Ri1.val["R"] = num2str(R1, Resistance);
  SchContent.appendComponent(Ri1);
  SchContent.appendWire(N2.ID, 0, Ri1.ID, 1);
  SchContent.appendWire(N1.ID, 0, Ri1.ID, 0);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 100);
  TL3.val["Z0"] = num2str(Z1, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL3);

  NodeInfo N3(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      100);
  SchContent.appendNode(N3);
  SchContent.appendWire(TL3.ID, 0, N0.ID, 0);
  SchContent.appendWire(TL3.ID, 1, N3.ID, 0);

  ComponentInfo Ri2(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 100, 50);
  Ri2.val["R"] = num2str(R1, Resistance);
  SchContent.appendComponent(Ri2);
  SchContent.appendWire(Ri2.ID, 1, N1.ID, 0);
  SchContent.appendWire(Ri2.ID, 0, N3.ID, 0);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 0);
  TL4.val["Z0"] = num2str(Z2, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL4);

  NodeInfo N4(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      0);
  SchContent.appendNode(N4);
  SchContent.appendWire(TL4.ID, 0, N1.ID, 0);
  SchContent.appendWire(TL4.ID, 1, N4.ID, 0);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 225,
      0);
  TermSpar2.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar2);
  SchContent.appendWire(TermSpar2.ID, 0, N4.ID, 0);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, -100);
  TL5.val["Z0"] = num2str(Z2, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL5);

  NodeInfo N5(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      -100);
  SchContent.appendNode(N5);
  SchContent.appendWire(TL5.ID, 0, N2.ID, 0);
  SchContent.appendWire(TL5.ID, 1, N5.ID, 0);

  ComponentInfo Ri3(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 200, -50);
  Ri3.val["R"] = num2str(R2, Resistance);
  SchContent.appendComponent(Ri3);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 225,
      -100);
  TermSpar3.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar3);
  SchContent.appendWire(Ri3.ID, 1, N5.ID, 0);
  SchContent.appendWire(Ri3.ID, 0, N4.ID, 0);
  SchContent.appendWire(TermSpar3.ID, 0, N5.ID, 0);

  ComponentInfo TL6(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 100);
  TL6.val["Z0"] = num2str(Z2, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL6);

  NodeInfo N6(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      100);
  SchContent.appendNode(N6);
  SchContent.appendWire(TL6.ID, 0, N3.ID, 0);
  SchContent.appendWire(TL6.ID, 1, N6.ID, 0);

  ComponentInfo Ri4(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 200, 50);
  Ri4.val["R"] = num2str(R2, Resistance);
  SchContent.appendComponent(Ri4);

  ComponentInfo TermSpar4(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 225,
      100);
  TermSpar4.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar4);
  SchContent.appendWire(N6.ID, 0, TermSpar4.ID, 0);
  SchContent.appendWire(Ri4.ID, 0, N6.ID, 0);
  SchContent.appendWire(Ri4.ID, 1, N4.ID, 0);
}

void PowerCombinerDesigner::buildWilkinson3Way_Microstrip(double lambda4,
                                                          double Z1, double Z2,
                                                          double R1,
                                                          double R2) {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 0, -25, 0);
  TermSpar1.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar1);

  NodeInfo N0(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0, 0);
  SchContent.appendNode(N0);
  SchContent.appendWire(TermSpar1.ID, 0, N0.ID, 0);

  // Synthesize microstrip lines for different impedances
  // Z1 impedance lines (3 first-stage quarter-wave lines)
  MicrostripClass MSL_Z1;
  MSL_Z1.Substrate = Specs.MS_Subs;
  MSL_Z1.synthesizeMicrostrip(Z1, lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, 0);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN1);

  NodeInfo N1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      0);
  SchContent.appendNode(N1);
  SchContent.appendWire(MLIN1.ID, 0, N0.ID, 0);
  SchContent.appendWire(MLIN1.ID, 1, N1.ID, 0);

  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, -100);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN2);

  NodeInfo N2(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      -100);
  SchContent.appendNode(N2);
  SchContent.appendWire(MLIN2.ID, 0, N0.ID, 0);
  SchContent.appendWire(MLIN2.ID, 1, N2.ID, 0);

  ComponentInfo Ri1(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 100, -50);
  Ri1.val["R"] = num2str(R1, Resistance);
  SchContent.appendComponent(Ri1);
  SchContent.appendWire(N2.ID, 0, Ri1.ID, 1);
  SchContent.appendWire(N1.ID, 0, Ri1.ID, 0);

  ComponentInfo MLIN3(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, 100);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN3);

  NodeInfo N3(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 100,
      100);
  SchContent.appendNode(N3);
  SchContent.appendWire(MLIN3.ID, 0, N0.ID, 0);
  SchContent.appendWire(MLIN3.ID, 1, N3.ID, 0);

  ComponentInfo Ri2(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 100, 50);
  Ri2.val["R"] = num2str(R1, Resistance);
  SchContent.appendComponent(Ri2);
  SchContent.appendWire(Ri2.ID, 1, N1.ID, 0);
  SchContent.appendWire(Ri2.ID, 0, N3.ID, 0);

  // Z2 impedance lines (3 second-stage quarter-wave lines)
  MicrostripClass MSL_Z2;
  MSL_Z2.Substrate = Specs.MS_Subs;
  MSL_Z2.synthesizeMicrostrip(Z2, lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN4(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, 0);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN4);

  NodeInfo N4(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      0);
  SchContent.appendNode(N4);
  SchContent.appendWire(MLIN4.ID, 0, N1.ID, 0);
  SchContent.appendWire(MLIN4.ID, 1, N4.ID, 0);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 225,
      0);
  TermSpar2.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar2);
  SchContent.appendWire(TermSpar2.ID, 0, N4.ID, 0);

  ComponentInfo MLIN5(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, -100);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN5.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN5);

  NodeInfo N5(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      -100);
  SchContent.appendNode(N5);
  SchContent.appendWire(MLIN5.ID, 0, N2.ID, 0);
  SchContent.appendWire(MLIN5.ID, 1, N5.ID, 0);

  ComponentInfo Ri3(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 200, -50);
  Ri3.val["R"] = num2str(R2, Resistance);
  SchContent.appendComponent(Ri3);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 225,
      -100);
  TermSpar3.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar3);
  SchContent.appendWire(Ri3.ID, 1, N5.ID, 0);
  SchContent.appendWire(Ri3.ID, 0, N4.ID, 0);
  SchContent.appendWire(TermSpar3.ID, 0, N5.ID, 0);

  ComponentInfo MLIN6(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, 100);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN6);

  NodeInfo N6(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 200,
      100);
  SchContent.appendNode(N6);
  SchContent.appendWire(MLIN6.ID, 0, N3.ID, 0);
  SchContent.appendWire(MLIN6.ID, 1, N6.ID, 0);

  ComponentInfo Ri4(QString("R%1").arg(++SchContent.NumberComponents[Resistor]),
                    Resistor, 0, 200, 50);
  Ri4.val["R"] = num2str(R2, Resistance);
  SchContent.appendComponent(Ri4);

  ComponentInfo TermSpar4(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 180, 225,
      100);
  TermSpar4.val["Z0"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar4);
  SchContent.appendWire(N6.ID, 0, TermSpar4.ID, 0);
  SchContent.appendWire(Ri4.ID, 0, N6.ID, 0);
  SchContent.appendWire(Ri4.ID, 1, N4.ID, 0);
}
