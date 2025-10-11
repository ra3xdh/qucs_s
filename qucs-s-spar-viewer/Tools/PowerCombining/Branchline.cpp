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

void PowerCombinerDesigner::Branchline() {
  // Design equations
  double K       = Specs.OutputRatio.at(0);
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);
  double ZA      = Specs.Z0 * sqrt(K / (K + 1));
  double ZB      = Specs.Z0 * sqrt(K);

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20,
      -50);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 120,
      -50);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 120,
      50);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  ComponentInfo Riso(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                     Resistor, 0, 0, 75);
  Riso.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Riso);

  ComponentInfo Ground(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, 0, 120);
  Schematic.appendComponent(Ground);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, -50);
  TL1.val["Z0"]     = num2str(ZA, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 50);
  TL2.val["Z0"]     = num2str(ZA, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 0);
  TL3.val["Z0"]     = num2str(ZB, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 100, 0);
  TL4.val["Z0"]     = num2str(ZB, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL4);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, -50);
  Schematic.appendNode(N1);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, -50);
  Schematic.appendNode(N2);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 50);
  Schematic.appendNode(N3);

  NodeInfo N4(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 50);
  Schematic.appendNode(N4);

  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N3.ID, 0);

  Schematic.appendWire(N1.ID, 0, TL1.ID, 0);
  Schematic.appendWire(N2.ID, 0, TL1.ID, 1);

  Schematic.appendWire(N2.ID, 0, TL4.ID, 1);
  Schematic.appendWire(N3.ID, 0, TL4.ID, 0);
  Schematic.appendWire(N3.ID, 0, TL2.ID, 1);
  Schematic.appendWire(N1.ID, 0, TL3.ID, 1);

  Schematic.appendWire(Riso.ID, 1, N4.ID, 0);
  Schematic.appendWire(N4.ID, 0, TL2.ID, 0);
  Schematic.appendWire(N4.ID, 0, TL3.ID, 0);
  Schematic.appendWire(Riso.ID, 0, Ground.ID, 0);
}
