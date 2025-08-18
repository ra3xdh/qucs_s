/***************************************************************************
                                QW_ShuntAttenuator.cpp
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

void AttenuatorDesigner::QW_ShuntAttenuator() {
  ComponentInfo TermSpar1, TermSpar2;
  ComponentInfo Ground, Res1, Res2, Res3, TL;
  ComponentInfo Lseries, Cshunt;
  NodeInfo NI;
  Components.clear();
  // Design equations
  double R = Specs.Zin * (pow(10, .05 * Specs.Attenuation) - 1);
  double l4 = .25 * SPEED_OF_LIGHT / Specs.Frequency;
  double w0 = 2 * M_PI * Specs.Frequency;
  // Power dissipation
  double K = (R + Specs.Zin) * (R + Specs.Zin);
  Pdiss.R1 = Specs.Pin * Specs.Zin * R / K;
  Pdiss.R2 = Specs.Pin * R * R / K;
  Pdiss.R3 = Pdiss.R1;

         // Schematic implementation - updated style

         // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 0, 0);
  TermSparIN.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

         // First node (input side)
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);

         // Lumped or distributed (TL) section
  if (Specs.Lumped_TL) {
    // Lumped: series L and shunt C to ground at each end
    Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0, 50, -50);
    Cshunt.val["C"] = num2str(1 / (Specs.Zin * w0), Capacitance);
    Schematic.appendComponent(Cshunt);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 180, 50, -75);
    Schematic.appendComponent(Ground);

    Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor, 0, 50, 50);
    Lseries.val["L"] = num2str(Specs.Zin / w0, Inductance);
    Schematic.appendComponent(Lseries);

    Schematic.appendWire(Cshunt.ID, 0, NI.ID, 0);     // C to node
    Schematic.appendWire(Cshunt.ID, 1, Ground.ID, 0); // C to ground
    Schematic.appendWire(Lseries.ID, 1, NI.ID, 0);    // Lseries to node
  } else {
    // Distributed: transmission line
    TL.setParams(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 0, 50, 50);
    TL.val["Z0"] = num2str(Specs.Zin, Resistance);
    TL.val["Length"] = ConvertLengthFromM("mm", l4);
    Schematic.appendComponent(TL);

    Schematic.appendWire(TL.ID, 1, NI.ID, 0);
  }

         // Input terminal to first node
  Schematic.appendWire(TermSparIN.ID, 0, NI.ID, 0);

         // 1st shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 50, 120);
  Res2.val["R"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 50, 175);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0); // R to ground

         // 2nd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 100, 120);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 100, 175);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0); // R to ground

         // How first node connects to rest
  if (Specs.Lumped_TL) {
    // Additional lumped section output shunt C and GND
    Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0, 150, 120);
    Cshunt.val["C"] = num2str(1 / (Specs.Zin * w0), Capacitance);
    Schematic.appendComponent(Cshunt);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 150, 175);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);
    Schematic.appendWire(Cshunt.ID, 1, Lseries.ID, 0);
    Schematic.appendWire(Res2.ID, 1, Lseries.ID, 0);
    Schematic.appendWire(Res3.ID, 1, Lseries.ID, 0);
  } else {
    Schematic.appendWire(Res2.ID, 1, TL.ID, 0);
    Schematic.appendWire(Res3.ID, 1, TL.ID, 0);
  }

         // Series resistor from input node to output node
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  Schematic.appendWire(Res1.ID, 0, NI.ID, 0);

         // Output node
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150, 0);
  Schematic.appendNode(NI);

         // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 200, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);
  Schematic.appendWire(NI.ID, 0, TermSpar2.ID, 0);
}

