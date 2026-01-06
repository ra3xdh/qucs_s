/// @file PiAttenuator.cpp
/// @brief Pi attenuator synthesis (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "PiAttenuator.h"

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995

void PiAttenuator::calculateParams() {
  // Design equations
  double L = pow(10, .1 * Specification.Attenuation);
  R2 = (.5 * (L - 1)) *
       sqrt(Specification.Zin * Specification.Zout / L); // Series resistor
  R1 = 1 / (((L + 1) / (Specification.Zin * (L - 1))) -
            (1 / R2)); // 1st shunt resistor
  R3 = 1 / (((L + 1) / (Specification.Zout * (L - 1))) -
            (1 / R2)); // 2ns shunt resistor

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
