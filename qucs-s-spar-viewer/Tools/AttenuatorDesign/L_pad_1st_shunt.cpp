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

// Reference: Based on design equations from attenuatorfunc.cpp L_PAD_1ST_SHUNT case

void AttenuatorDesigner::LPadFirstShuntAttenuator() {

  ComponentInfo TermSpar1, TermSpar2;
  ComponentInfo Ground, Res1, Res2;
  NodeInfo NI;
  Components.clear();

         // Design equations (from attenuatorfunc.cpp, L_PAD_1ST_SHUNT case)
  double L = pow(10, -Specs.Attenuation / 10);

  double R1 = Specs.Zin / (1 - sqrt(L));             // Shunt resistor
  double R2 = Specs.Zin * (1 - sqrt(L)) / sqrt(L);   // Series resistor
  double Zout = -Specs.Zin * (L - 2*sqrt(L) + 2) / (L - 2*sqrt(L)); // Output impedance

         // Power dissipation calculation
  Pdiss.R1 = Specs.Pin * sqrt(L) * (1 - 2*sqrt(L) + L) / (1 - sqrt(L)); // Shunt resistor
  Pdiss.R2 = Specs.Pin * (1 - sqrt(L));                                 // Series resistor

         // Circuit implementation

         // Input terminal
  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar1);

         // Node between series resistor and shunt
  NI.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);

         // Shunt resistor (R1) to ground at input node
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 50, 100);
  Schematic.appendComponent(Ground);

         // Series resistor (R2)
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 90, 100, 0);
  Res2.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Res2);

  // Zout label
  QString Zout_label = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem* label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(130, -20);
  Schematic.appendText(label2);

         // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 150, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

         // Wiring connections
         // Input terminal to node
  Schematic.appendWire(TermSpar1.ID, 0, NI.ID, 0);
        // Node to shunt resistor
  Schematic.appendWire(NI.ID, 0, Res1.ID, 1);

         // Shunt resistor to ground
  Schematic.appendWire(Res1.ID, 0, Ground.ID, 0);

         // Input terminal to series resistor
  Schematic.appendWire(NI.ID, 0, Res2.ID, 0);

         // Series resistor to output terminal
  Schematic.appendWire(Res2.ID, 1, TermSpar2.ID, 0);

}
