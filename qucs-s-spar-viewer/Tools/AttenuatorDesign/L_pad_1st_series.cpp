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

// Reference: Based on the design equations from attenuatorfunc.cpp
// L_PAD_1ST_SERIES case
void AttenuatorDesigner::LPadFirstSeriesAttenuator() {
  ComponentInfo TermSpar1, TermSpar2;
  ComponentInfo Ground, Res1, Res2;
  NodeInfo NI;

  Components.clear();

  // Design equations (from attenuatorfunc.cpp L_PAD_1ST_SERIES case)
  double L  = pow(10, -Specs.Attenuation / 10);
  double R1 = -Specs.Zin * (L - 1) / (sqrt(L) + 1); // Series resistor
  double R2 = -Specs.Zin * (L + sqrt(L)) / (L - 1); // Shunt resistor
  double Zout =
      R2 * (R1 + Specs.Zin) / (R1 + R2 + Specs.Zin); // Output impedance

  // Power dissipation calculation (from attenuatorfunc.cpp)
  Pdiss.R1 = Specs.Pin * (1 - sqrt(L));               // Series resistor
  Pdiss.R2 = Specs.Pin * L * (1 - L) / (L + sqrt(L)); // Shunt resistor

  // Circuit implementation
  // Input terminal
  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Series resistor (R1)
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  // Node after series resistor
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150,
      0);
  Schematic.appendNode(NI);

  // Shunt resistor (R2)
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 150, 50);
  Res2.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Res2);

  // Ground for shunt resistor
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 100);
  Schematic.appendComponent(Ground);

  // Zout label
  QString Zout_label        = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem* label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(130, -20);
  Schematic.appendText(label2);

  // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Wiring connections
  // Input terminal to series resistor
  Schematic.appendWire(TermSpar1.ID, 0, Res1.ID, 0);

  // Series resistor to node
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);

  // Node to shunt resistor (top terminal)
  Schematic.appendWire(NI.ID, 0, Res2.ID, 1);

  // Shunt resistor to ground
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);

  // Node to output terminal
  Schematic.appendWire(NI.ID, 0, TermSpar2.ID, 0);
}
