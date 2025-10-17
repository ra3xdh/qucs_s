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

#include "BridgedTeeAttenuator.h"

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995

BridgedTeeAttenuator::BridgedTeeAttenuator() {}

BridgedTeeAttenuator::BridgedTeeAttenuator(AttenuatorDesignParameters AS) {
  Specification = AS;
}

BridgedTeeAttenuator::~BridgedTeeAttenuator() {}

void BridgedTeeAttenuator::calculateParams() {
  // Design equations
  // Note: Bridged-T uses 0.05*Attenuation (not 0.1 like Pi/Tee)
  double L = pow(10, .05 * Specification.Attenuation);
  R1 = Specification.Zin * (L - 1);
  R4 = Specification.Zin / (L - 1);
}

void BridgedTeeAttenuator::synthesize() {
  calculateParams();
  buildBridgedTeeAttenuator();
}

void BridgedTeeAttenuator::buildBridgedTeeAttenuator() {
  ComponentInfo Ground, Res1, Res2, Res3, Res4;
  NodeInfo NI;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // Series resistor R1
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  // 1st Shunt Resistor R2
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 50);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);

  // Input node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);
  Schematic.appendWire(TermSparIN.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res1.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI.ID, 0);

  // 2nd Shunt Resistor R3
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 150, 50);
  Res3.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res3);

  // Middle node (bridge connection)
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 100,
      80);
  Schematic.appendNode(NI);
  Schematic.appendWire(Res2.ID, 0, NI.ID, 0);

  // 3rd Shunt resistor R4 (bridge to ground)
  Res4.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 120);
  Res4.val["R"] = num2str(R4, Resistance);
  Schematic.appendComponent(Res4);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 170);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res3.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res4.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res4.ID, 0, Ground.ID, 0);

  // Output node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150,
      0);
  Schematic.appendNode(NI);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 1, NI.ID, 0);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specification.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, NI.ID, 0);
}
