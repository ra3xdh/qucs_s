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

#include "Lsection.h"

Lsection::Lsection() {}

Lsection::Lsection(MatchingNetworkDesignParameters AS, double freq) {
  Specs   = AS;
  f_match = freq;
}

Lsection::~Lsection() {}

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995
void Lsection::synthesize() {
  // Port 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo Shunt, Series, Ground;
  NodeInfo NI;

  // Complex load
  ComponentInfo Zload;

  if (Specs.sim_path.isEmpty()) {
    // Constant ZL over frequency
    Zload.setParams(
        QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
        ComplexImpedance, 0, 175, 50);
    Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  } else {
    // There's a path to a S-parameter file
    Zload.setParams(
        QString("SPAR%1").arg(++Schematic.NumberComponents[SPAR_Block]),
        SPAR_Block, -90, 175, 50);
    Zload.val["Path"] = Specs.sim_path;
  }
  Schematic.appendComponent(Zload);

  // GND_ZL
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, 175, 100);
  Schematic.appendComponent(GND_ZL);

  // Design equations
  double w0 = 2.0 * M_PI * f_match;
  double L, C, X, B;

  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double XL = Specs.ZL.imag();

  if (Z0 > RL) {
    // ZS -------- X -- ZL
    //       |
    //       B
    //       |
    //      ---

    // Solution 1
    if (Specs.Solution == 1) {
      X = sqrt(RL * (Z0 - RL)) - XL;
      B = sqrt((Z0 - RL) / RL) / Z0;
    } else {
      // Solution 2
      X = -sqrt(RL * (Z0 - RL)) - XL;
      B = -sqrt((Z0 - RL) / RL) / Z0;
    }

    // Shunt element
    if (B > 0) // Capacitor
    {
      C = B / w0;
      // Lumped capacitor
      Shunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0.0, 50, 50);
      Shunt.val["C"] = num2str(C, Capacitance);
    } else { // Inductor
      L = -1 / (w0 * B);
      Shunt.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          0, 50, 50);
      Shunt.val["L"] = num2str(L, Inductance);
    }
    Schematic.appendComponent(Shunt);
    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, 50, 100);
    Schematic.appendComponent(Ground);

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50,
        0);
    Schematic.appendNode(NI);

    // Series element
    if (X < 0) // Capacitor
    {
      C = -1 / (w0 * X);
      Series.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, -90, 100, 0);
      Series.val["C"] = num2str(C, Capacitance);
    } else { // Inductor
      L = X / w0;
      Series.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, 100, 0);
      Series.val["L"] = num2str(L, Inductance);
    }
    Schematic.appendComponent(Series);

    // Wires
    Schematic.appendWire(TermSpar1.ID, 0, NI.ID, 0);
    Schematic.appendWire(Shunt.ID, 1, NI.ID, 0);
    Schematic.appendWire(Shunt.ID, 0, Ground.ID, 0);
    Schematic.appendWire(NI.ID, 0, Series.ID, 1);
    Schematic.appendWire(Zload.ID, 1, Series.ID, 0);

  } else {
    // Z0 < RL
    // ZS --- X  ------- ZL
    //              |
    //              B
    //              |
    //             ---

    // Solution 1
    if (Specs.Solution == 1) {
      B = (XL + sqrt(RL / Z0) * sqrt(RL * RL + XL * XL - Z0 * RL)) /
          (RL * RL + XL * XL);
      X = 1 / B + XL * Z0 / RL - Z0 / (B * RL);
    } else {
      // Solution 2
      B = (XL - sqrt(RL / Z0) * sqrt(RL * RL + XL * XL - Z0 * RL)) /
          (RL * RL + XL * XL);
      X = 1 / B + XL * Z0 / RL - Z0 / (B * RL);
    }

    // Series element
    if (X < 0) { // Capacitor
      C = -1 / (w0 * X);
      Series.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, -90, 50, 0);
      Series.val["C"] = num2str(C, Capacitance);
    } else { // Inductor
      L = X / w0;
      Series.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, 50, 0);
      Series.val["L"] = num2str(L, Inductance);
    }
    Schematic.appendComponent(Series);

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 100,
        0);
    Schematic.appendNode(NI);

    // Shunt element
    if (B > 0) { // Capacitor
      C = B / w0;
      Shunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0.0, 100, 50);
      Shunt.val["C"] = num2str(C, Capacitance);
    } else { // Inductor
      L = -1 / (w0 * B);
      Shunt.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          0, 100, 50);
      Shunt.val["L"] = num2str(L, Inductance);
    }
    Schematic.appendComponent(Shunt);
    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, 100, 100);
    Schematic.appendComponent(Ground);

    // Wires
    Schematic.appendWire(TermSpar1.ID, 0, Series.ID, 1);
    Schematic.appendWire(Series.ID, 0, NI.ID, 0);
    Schematic.appendWire(NI.ID, 0, Shunt.ID, 1);
    Schematic.appendWire(Shunt.ID, 0, Ground.ID, 0);
    Schematic.appendWire(Zload.ID, 1, NI.ID, 0);
  }

  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}
