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

void AttenuatorDesigner::QW_SeriesAttenuator() {
  ComponentInfo TermSpar2;
  ComponentInfo Ground, Res1, Res2, Res3, TL;
  ComponentInfo Lseries, Cshunt;
  NodeInfo NI;
  Components.clear();

  // Design equations
  double R = Specs.Zin / (pow(10, .05 * Specs.Attenuation) - 1);
  double l4 = .25 * SPEED_OF_LIGHT / Specs.Frequency;
  double w0 = 2 * M_PI * Specs.Frequency;

  // Power dissipation
  double K = (R + Specs.Zin) * (R + Specs.Zin);
  Pdiss.R1 = Specs.Pin * Specs.Zin * R / K;
  Pdiss.R2 = Specs.Pin * Specs.Zin * Specs.Zin / K;
  Pdiss.R3 = Pdiss.R1;

  // Zout calculation
  double Zout = (R * R * Specs.Zin + 2 * R * Specs.Zin * Specs.Zin) / (R * R + 2 * R * Specs.Zin + 2 * Specs.Zin * Specs.Zin);

  // Schematic implementation
  // Input terminal
  ComponentInfo TermSparIN(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // 1st shunt resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  // First node after input and shunt
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);
  Schematic.appendWire(TermSparIN.ID, 0, NI.ID, 0);   // Input to node
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);         // Res1 to node

  // 2nd shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 50, 125);
  Res2.val["R"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(Res2);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 50, 175);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res1.ID, 0, Res2.ID, 1);       // Res1 to Res2
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);     // Res2 to ground

  if (Specs.Lumped_TL) {
    // Lumped transmission line: series L, shunt C
    Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0, 50, -50);
    Cshunt.val["C"] = num2str(1 / (Specs.Zin * w0), Capacitance);
    Schematic.appendComponent(Cshunt);
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 180, 50, -70);
    Schematic.appendComponent(Ground);
    Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor, -90, 100, 0);
    Lseries.val["L"] = num2str(Specs.Zin / w0, Inductance);
    Schematic.appendComponent(Lseries);
    Schematic.appendWire(Lseries.ID, 1, NI.ID, 0);    // Lseries to node
    Schematic.appendWire(Cshunt.ID, 0, NI.ID, 0);     // Cshunt to node
    Schematic.appendWire(Cshunt.ID, 1, Ground.ID, 0); // Cshunt to ground
  } else {
    // Distributed TL
    TL.setParams(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, 100, 0);
    TL.val["Z0"] = num2str(Specs.Zin, Resistance);
    TL.val["Length"] = ConvertLengthFromM("mm", l4);
    Schematic.appendComponent(TL);
    Schematic.appendWire(TL.ID, 0, NI.ID, 0);         // TL to node
  }

  // Second node (output side of TL, L, etc)
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150, 0);
  Schematic.appendNode(NI);
  if (Specs.Lumped_TL) {
    // Lumped TL output C and GND
    Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0, 150, -50);
    Cshunt.val["C"] = num2str(1 / (Specs.Zin * w0), Capacitance);
    Schematic.appendComponent(Cshunt);
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 180, 150, -70);
    Schematic.appendComponent(Ground);
    Schematic.appendWire(Lseries.ID, 0, NI.ID, 0);    // Lseries to second node
    Schematic.appendWire(Cshunt.ID, 0, NI.ID, 0);     // Cshunt to second node
    Schematic.appendWire(Cshunt.ID, 1, Ground.ID, 0); // Cshunt to ground
  } else {
    Schematic.appendWire(NI.ID, 0, TL.ID, 1);         // Node to output of TL
  }

  // 3rd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 150, 50);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 150, 100);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res3.ID, 1, NI.ID, 0);         // Res3 to node
  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);     // Res3 to ground


  // Zout label
  QString Zout_label = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem* label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(130, -20);
  Schematic.appendText(label2);

  // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, NI.ID, 0);    // Output terminal to node
}

