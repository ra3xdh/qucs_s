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

#include "AttenuatorDesigner.h"

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995
void AttenuatorDesigner::TeeAttenuator() {
  ComponentInfo TermSpar1, TermSpar2;
  ComponentInfo Ground, Res1, Res2, Res3;
  NodeInfo NI;
  Components.clear();
  // Design equations
  double L  = pow(10, .1 * Specs.Attenuation);
  double R2 = (2 * sqrt(Specs.Zin * Specs.Zout * L)) / (L - 1);
  double R1 = Specs.Zin * ((L + 1) / (L - 1)) - R2;
  double R3 = Specs.Zout * ((L + 1) / (L - 1)) - R2;
  // Power dissipation
  Pdiss.R1 = Specs.Pin * R1 / Specs.Zin;
  Pdiss.R2 = Specs.Pin * (R1 - Specs.Zin) * (R1 - Specs.Zin) / (R2 * Specs.Zin);
  Pdiss.R3 = Specs.Pin * R3 * (R1 + R2 - Specs.Zin) * (R1 + R2 - Specs.Zin) /
             (Specs.Zin * R2 * R2);
  // Schematic implementation: follow new style
  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // 1st series resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 50, 0);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  // Node after R1
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 100,
      0);
  Schematic.appendNode(NI);
  Schematic.appendWire(TermSparIN.ID, 0, Res1.ID, 0); // Terminal to R1
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);         // R1 to node

  // Shunt resistor to ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 50);
  Res2.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Res2);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 100);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res2.ID, 1, NI.ID, 0);     // R2 to node
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0); // R2 to ground

  // 2nd series resistor
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 150, 0);
  Res3.val["R"] = num2str(R3, Resistance);
  Schematic.appendComponent(Res3);

  Schematic.appendWire(Res3.ID, 0, NI.ID, 0); // R3 to output node

  // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Res3.ID, 1); // Terminal to output node
}
