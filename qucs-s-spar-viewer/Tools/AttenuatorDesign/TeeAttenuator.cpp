/// @file TeeAttenuator.cpp
/// @brief Tee attenuator synthesis (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "TeeAttenuator.h"

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995

void TeeAttenuator::calculateParams() {
  // Design equations
  double L = pow(10, .1 * Specification.Attenuation);
  R2 = (2 * sqrt(Specification.Zin * Specification.Zout * L)) /
       (L - 1);                                       // Shunt resistor
  R1 = Specification.Zin * ((L + 1) / (L - 1)) - R2;  // 1st series resistor
  R3 = Specification.Zout * ((L + 1) / (L - 1)) - R2; // 2nd series resistor

  // Power dissipation
  Pdiss["R1"] = Specification.Pin * R1 / Specification.Zin;
  Pdiss["R2"] = Specification.Pin * (R1 - Specification.Zin) *
                (R1 - Specification.Zin) / (R2 * Specification.Zin);
  Pdiss["R3"] = Specification.Pin * R3 * (R1 + R2 - Specification.Zin) *
                (R1 + R2 - Specification.Zin) / (Specification.Zin * R2 * R2);
}

void TeeAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void TeeAttenuator::buildNetwork() {
  ComponentInfo Ground, Res1, Res2, Res3;
  NodeInfo NI;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
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
  Schematic.appendWire(TermSparIN.ID, 0, Res1.ID, 0);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);

  // Shunt resistor to ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 50);
  Res2.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Res2);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 100);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res2.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);

  // 2nd series resistor
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 150, 0);
  Res3.val["R"] = num2str(R3, Resistance);
  Schematic.appendComponent(Res3);

  Schematic.appendWire(Res3.ID, 0, NI.ID, 0);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specification.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Res3.ID, 1);
}
