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

#include "ReflectionAttenuator.h"

ReflectionAttenuator::ReflectionAttenuator() {}

ReflectionAttenuator::ReflectionAttenuator(AttenuatorDesignParameters AS) {
  Specification = AS;
}

ReflectionAttenuator::~ReflectionAttenuator() {}

void ReflectionAttenuator::calculateParams() {
  // Design equations
  double L = pow(10, -.05 * Specification.Attenuation);
  Ri = Specification.Zin * (1 - L) / (1 + L);
}

void ReflectionAttenuator::synthesize() {
  calculateParams();
  buildReflectionAttenuator();
}

void ReflectionAttenuator::buildReflectionAttenuator() {
  ComponentInfo Ground, Res1, Res2, Coup;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // First shunt resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 100);
  Res1.val["R"] = num2str(Ri, Resistance);
  Schematic.appendComponent(Res1);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 150);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res1.ID, 0, Ground.ID, 0);

  // Coupler (3 dB hybrid coupler)
  QStringList ConnectionNodes;
  ConnectionNodes.append(QString("N0"));
  ConnectionNodes.append(QString("NR1"));
  ConnectionNodes.append(QString("NR2"));
  ConnectionNodes.append(QString("N1"));
  Coup.setParams(
      QString("COUPLER%1").arg(++Schematic.NumberComponents[Coupler]), Coupler,
      0, 100, 50);
  Coup.val["k"] = num2str(0.7071, NoUnits);
  Coup.val["Z0"] = num2str(Specification.Zin, Resistance);
  Coup.val["phase"] = num2str(90, NoUnits);
  Schematic.appendComponent(Coup);

  Schematic.appendWire(Res1.ID, 1, Coup.ID, 0);

  // Second shunt resistor
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 150, 100);
  Res2.val["R"] = num2str(Ri, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 150);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 1, Coup.ID, 3);
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specification.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSparIN.ID, 0, Coup.ID, 1);
  Schematic.appendWire(TermSpar2.ID, 0, Coup.ID, 2);
}
