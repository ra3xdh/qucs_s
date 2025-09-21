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
 * II". Andrei Grebennikov. High Frequency Electronics. 2008 [2] "A recombinant
 * in-phase power divider", IEEE Trans. Microwave Theory Tech., vol. MTT-39,
 * Aug. 1991, pp. 1438-1440
 */
void PowerCombinerDesigner::Recombinant3WayWilkinson() {
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);
  double Z1 = Specs.Z0 * 0.72;
  double Z2 = Specs.Z0 * 0.8;
  double Z4 = Specs.Z0 * 1.6;
  double R1 = Specs.Z0, R2 = Specs.Z0 * 2;

  ComponentInfo TermSpar1(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 0);
  TL1.val["Z0"] = num2str(Z1, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(TL1.ID, 0, TermSpar1.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 0);
  Schematic.appendNode(N1);

  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 125, -75);
  TL2.val["Z0"] = num2str(Z2, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              175, -75);
  Schematic.appendNode(N2);

  Schematic.appendWire(TL2.ID, 0, N1.ID, 0);
  Schematic.appendWire(TL2.ID, 1, N2.ID, 0);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 125, 75);
  TL3.val["Z0"] = num2str(Z2, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              175, 75);
  Schematic.appendNode(N3);

  Schematic.appendWire(TL3.ID, 0, N1.ID, 0);
  Schematic.appendWire(TL3.ID, 1, N3.ID, 0);

  // Isolation resistor
  ComponentInfo Ri1(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 175, 0);
  Ri1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Ri1);

  Schematic.appendWire(Ri1.ID, 1, N2.ID, 0);
  Schematic.appendWire(Ri1.ID, 0, N3.ID, 0);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 225, -125);
  TL4.val["Z0"] = num2str(Z2, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL4);

  Schematic.appendWire(TL4.ID, 0, N2.ID, 0);

  NodeInfo N4(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              275, -125);
  Schematic.appendNode(N4);

  ComponentInfo TermSpar2(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 300, -125);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TL4.ID, 1, N4.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, N4.ID, 0);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 225, -50);
  TL5.val["Z0"] = num2str(Z4, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL5);

  NodeInfo N5(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              275, -50);
  Schematic.appendNode(N5);

  Schematic.appendWire(TL5.ID, 0, N2.ID, 0);
  Schematic.appendWire(TL5.ID, 1, N5.ID, 0);

  // Isolation resistor
  ComponentInfo Ri2(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 275, -85);
  Ri2.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri2);

  Schematic.appendWire(Ri2.ID, 1, N4.ID, 0);
  Schematic.appendWire(Ri2.ID, 0, N5.ID, 0);

  NodeInfo N8(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              275, 0);
  Schematic.appendNode(N8);

  ComponentInfo TermSpar3(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 300, 0);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  Schematic.appendWire(N5.ID, 0, N8.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N8.ID, 0);

  ComponentInfo TL6(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 225, 125);
  TL6.val["Z0"] = num2str(Z2, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL6);

  NodeInfo N6(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              275, 125);
  Schematic.appendNode(N6);

  Schematic.appendWire(TL6.ID, 0, N3.ID, 0);
  Schematic.appendWire(TL6.ID, 1, N6.ID, 0);

  ComponentInfo TL7(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 225, 50);
  TL7.val["Z0"] = num2str(Z4, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL7);

  NodeInfo N7(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              275, 50);
  Schematic.appendNode(N7);

  Schematic.appendWire(TL7.ID, 0, N3.ID, 0);
  Schematic.appendWire(TL7.ID, 1, N7.ID, 0);

  // Isolation resistor
  ComponentInfo Ri3(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 275, 85);
  Ri3.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri3);

  Schematic.appendWire(Ri3.ID, 1, N7.ID, 0);
  Schematic.appendWire(Ri3.ID, 0, N6.ID, 0);
  Schematic.appendWire(N7.ID, 0, N8.ID, 0);

  ComponentInfo TermSpar4(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 300,
      125);
  TermSpar4.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  Schematic.appendWire(TermSpar4.ID, 0, N6.ID, 0);
}
