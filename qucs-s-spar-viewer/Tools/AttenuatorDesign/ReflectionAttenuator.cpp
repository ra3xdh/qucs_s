/// @file ReflectionAttenuator.cpp
/// @brief Reflection attenuator synthesis (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "ReflectionAttenuator.h"

/// Reference: The PIN diode circuit designer's handbook. W.E. Doherty, Jr.,
/// R.D. Joos, Microsemi Corp., 1998
void ReflectionAttenuator::calculateParams() {
  // Design equations
  double L = pow(10, -.05 * Specification.Attenuation);
  Ri = Specification.Zin * (1 - L) / (1 + L);

  // Power dissipation calculation
  Pdiss["R1"] =
      .5 * Specification.Pin * (1 - pow(10, -0.1 * Specification.Attenuation));
  Pdiss["R2"] = Pdiss["R1"];
}

void ReflectionAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void ReflectionAttenuator::buildNetwork() {
  ComponentInfo Ground, Res1, Res2, Coup;

  //
  // T1 ----    ----- T2
  //        |   |
  //       -------
  //       |     |
  //       |     |
  //       -------
  //        |   |
  //       R1   R2

  // Define components' position
  QPoint posT1 = QPoint(0, 0);
  QPoint posCoupler = QPoint(posT1.x() + 60, 50);
  QPoint posR1 = QPoint(posCoupler.x() - 30, posCoupler.y() + 75);
  QPoint posR2 = QPoint(posCoupler.x() + 30, posR1.y());
  QPoint posT2 = QPoint(posCoupler.x() + 60, posT1.y());

  // Nodes (all virtual)

  // Input terminal
  ComponentInfo TermSparIN(QString("T1"), Term, posT1);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // First shunt resistor
  Res1.setParams(QString("R1"), Resistor, posR1);
  Res1.val["R"] = num2str(Ri, Resistance);
  Schematic.appendComponent(Res1);

  Ground.setParams(QString("GND1"), GND, 0, posR1.x(), posR1.y() + 50);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res1.ID, 0, Ground.ID, 0);

  // Coupler (3 dB hybrid coupler)
  QStringList ConnectionNodes;
  ConnectionNodes.append(QString("N0"));
  ConnectionNodes.append(QString("NR1"));
  ConnectionNodes.append(QString("NR2"));
  ConnectionNodes.append(QString("N1"));
  Coup.setParams(QString("COUPLER1"), Coupler, posCoupler);
  Coup.val["k"] = num2str(0.7071, NoUnits);
  Coup.val["Z0"] = num2str(Specification.Zin, Resistance);
  Coup.val["phase"] = num2str(90, NoUnits);
  Schematic.appendComponent(Coup);

  Schematic.appendWire(Res1.ID, 1, Coup.ID, 0);

  // Second shunt resistor
  Res2.setParams(QString("R2"), Resistor, posR2);
  Res2.val["R"] = num2str(Ri, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND2"), GND, 0, posR2.x(), posR2.y() + 50);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 1, Coup.ID, 3);
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T2"), Term, 180, posT2);
  TermSpar2.val["Z"] = num2str(Specification.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSparIN.ID, 0, Coup.ID, 1);
  Schematic.appendWire(TermSpar2.ID, 0, Coup.ID, 2);
}
