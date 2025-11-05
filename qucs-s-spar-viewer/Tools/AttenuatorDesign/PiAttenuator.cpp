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

#include "PiAttenuator.h"

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995

PiAttenuator::PiAttenuator() {}

PiAttenuator::PiAttenuator(AttenuatorDesignParameters AS)
    : AttenuatorBase(AS) {}

PiAttenuator::~PiAttenuator() {}

void PiAttenuator::calculateParams() {
  // Design equations
  double L = pow(10, .1 * Specification.Attenuation);
  R2 = (.5 * (L - 1)) * sqrt(Specification.Zin * Specification.Zout / L);
  R1 = 1 / (((L + 1) / (Specification.Zin * (L - 1))) - (1 / R2));
  R3 = 1 / (((L + 1) / (Specification.Zout * (L - 1))) - (1 / R2));

  // Calculate power dissipation
  Pdiss["R1"] = Specification.Pin * Specification.Zin / R1;
  Pdiss["R2"] = Specification.Pin * R2 * (R1 - Specification.Zin) *
                (R1 - Specification.Zin) / (R1 * R1 * Specification.Zin);
  double K = R1 * R2 - Specification.Zin * (R1 + R2);
  Pdiss["R3"] = Specification.Pin * K * K / (R1 * R1 * R3 * Specification.Zin);
}

void PiAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void PiAttenuator::buildNetwork() {
  ComponentInfo Ground, Res1, Res2, Res3;
  NodeInfo NI;

  // Input terminal
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSpar1);

  // 1st shunt resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 100);
  Schematic.appendComponent(Ground);

  // First node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);

  Schematic.appendWire(TermSpar1.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res1.ID, 0, Ground.ID, 0);

  // Series resistor
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 100, 0);
  Res2.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Res2);

  Schematic.appendWire(Res2.ID, 0, NI.ID, 0);

  // 2nd shunt resistor
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 150, 50);
  Res3.val["R"] = num2str(R3, Resistance);
  Schematic.appendComponent(Res3);

  // Second node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150,
      0);
  Schematic.appendNode(NI);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 100);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specification.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSpar2.ID, 0, NI.ID, 0);
}
