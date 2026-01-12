/// @file BridgedTeeAttenuator.cpp
/// @brief Bridged-Tee attenuator (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "BridgedTeeAttenuator.h"

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995
void BridgedTeeAttenuator::calculateParams() {
  // Design equations
  // Note: Bridged-T uses 0.05*Attenuation (not 0.1 like Pi/Tee)
  double L = pow(10, .05 * Specification.Attenuation);
  R1 = Specification.Zin * (L - 1);
  R4 = Specification.Zin / (L - 1);

  // Power dissipation calculation
  double K = R1 * R4 + Specification.Zin * (2 * R4 + Specification.Zin);

  K *= K;
  Pdiss["R1"] = Specification.Pin * (4 * R1 * R4 * R4 * Specification.Zin) / K;
  Pdiss["R2"] = Specification.Pin *
                (R1 * R4 + Specification.Zin * Specification.Zin) *
                (R1 * R4 + Specification.Zin * Specification.Zin) / K;
  Pdiss["R3"] = 0;
  Pdiss["R4"] = 4 * R4 * Specification.Zin * Specification.Zin / K;
}

void BridgedTeeAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void BridgedTeeAttenuator::buildNetwork() {
  ComponentInfo Ground, Res1, Res2, Res3, Res4;
  NodeInfo N1, N2, N3;    // Middle node
  NodeInfo NLeft, NRight; // Virtual nodes to aid the export process

  //
  //  T1 -- (N1) -- [R1] -- (N3) -- T2
  //          |              |
  //         [R2]           [R3]
  //          |              |
  //       (NLeft)--(N2)--(NRight)
  //                  |
  //                 [R4]
  //                  |
  //                 ---

  // Positions
  // Components
  QPoint posTerm1 = QPoint(0, 0);
  QPoint posR1 = QPoint(100, 0);
  QPoint posR2 = QPoint(50, 50);
  QPoint posR3 = QPoint(150, 50);
  QPoint posR4 = QPoint(100, 140);
  QPoint posGND = QPoint(posR4.x(), posR4.y() + 50);
  QPoint posTerm2 = QPoint(posR3.x() + 50, posTerm1.y());

  // Nodes
  QPoint posN1 = QPoint(posR2.x(), posTerm1.y());   // Input node
  QPoint posN2 = QPoint(posR1.x(), posR2.y() + 50); // Center node
  QPoint posN3 = QPoint(posR3.x(), posR3.y() - 50); // Output node

  // Virtual nodes
  QPoint posNLeft = QPoint(posR2.x(), posN2.y());
  QPoint posNRight = QPoint(posR3.x(), posN2.y());

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, posTerm1);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // Series resistor R1
  Res1.setParams(QString("R1"), Resistor, 90, posR1);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  // 1st Shunt Resistor R2
  Res2.setParams(QString("R2"), Resistor, posR2);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);

  // Input node
  N1.setParams(QString("N1"), posN1);
  Schematic.appendNode(N1);
  Schematic.appendWire(TermSparIN.ID, 0, N1.ID, 0);
  Schematic.appendWire(Res1.ID, 0, N1.ID, 0);
  Schematic.appendWire(Res2.ID, 1, N1.ID, 0);

  // 2nd Shunt Resistor R3
  Res3.setParams(QString("R3"), Resistor, posR3);
  Res3.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res3);

  // Middle node (bridge connection)
  N2.setParams(QString("N2"), posN2);
  Schematic.appendNode(N2);

  // 3rd Shunt resistor R4 (bridge to ground)
  Res4.setParams(QString("R4"), Resistor, posR4);
  Res4.val["R"] = num2str(R4, Resistance);
  Schematic.appendComponent(Res4);

  Ground.setParams(QString("GND1"), GND, posGND);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res4.ID, 0, Ground.ID, 0);

  // Virtual nodes: They are very useful for creating wires in the export
  // process Left
  NLeft.setParams(QString("N4"), posNLeft);
  NLeft.visible = false;
  Schematic.appendNode(NLeft);

  NRight.setParams(QString("N5"), posNRight);
  NRight.visible = false;
  Schematic.appendNode(NRight);

  Schematic.appendWire(Res2.ID, 0, NLeft.ID, 0);
  Schematic.appendWire(Res3.ID, 0, NRight.ID, 0);
  Schematic.appendWire(NLeft.ID, 0, N2.ID, 0);
  Schematic.appendWire(NRight.ID, 0, N2.ID, 0);
  Schematic.appendWire(Res4.ID, 1, N2.ID, 0);

  // Output node
  N3.setParams(QString("N3"), posN3);
  Schematic.appendNode(N3);
  Schematic.appendWire(Res1.ID, 1, N3.ID, 0);
  Schematic.appendWire(Res3.ID, 1, N3.ID, 0);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T2"), Term, 180, posTerm2);
  TermSpar2.val["Z"] = num2str(Specification.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, N3.ID, 0);
}
