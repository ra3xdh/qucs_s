/***************************************************************************
                                BridgedTeeAttenuator.cpp
                                ----------
    copyright            :  QUCS team
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#include "AttenuatorDesigner.h"

// Reference: RF design guide. Systems, circuits, and equations. Peter
// Vizmuller. Artech House, 1995
void AttenuatorDesigner::BridgedTeeAttenuator() {
  ComponentInfo TermSpar1, TermSpar2;
  ComponentInfo Ground, Res1, Res2, Res3, Res4;
  NodeInfo NI;
  Components.clear();

  // Design equations
  double L = pow(10, .05 * Specs.Attenuation);    // Note: Bridged-T uses 0.05*Atten
  double R1 = Specs.Zin * (L - 1);
  double R4 = Specs.Zin / (L - 1);

  // Power dissipation calculation
  double K = R1 * R4 + Specs.Zin * (2 * R4 + Specs.Zin);

  K *= K;
  Pdiss.R1 = Specs.Pin * (4 * R1 * R4 * R4 * Specs.Zin) / K;
  Pdiss.R2 = Specs.Pin * (R1 * R4 + Specs.Zin * Specs.Zin) * (R1 * R4 + Specs.Zin * Specs.Zin) / K;
  Pdiss.R3 = 0;
  Pdiss.R4 = 4 * R4 * Specs.Zin * Specs.Zin / K;

  // Schematic implementation - match style of PiAttenuator()
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 0, 0);
  TermSparIN.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // Series resistor R1
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  // 1st Shunt Resistor R2
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 50, 50);
  Res2.val["R"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(Res2);

  // Node after input resistors
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);
  Schematic.appendWire(TermSparIN.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res1.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI.ID, 0);

  // 2nd Shunt Resistor R3
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 150, 50);
  Res3.val["R"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(Res3);

  // Node after series and shunt resistors
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 100, 80);
  Schematic.appendNode(NI);
  Schematic.appendWire(Res2.ID, 0, NI.ID, 0);

  // 3rd Shunt resistor R4
  Res4.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 100, 120);
  Res4.val["R"] = num2str(R4, Resistance);
  Schematic.appendComponent(Res4);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 100, 170);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res4.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res4.ID, 0, Ground.ID, 0);

  // Output node
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150, 0);
  Schematic.appendNode(NI);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 1, NI.ID, 0);
  // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 200, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zout, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, NI.ID, 0);
}

