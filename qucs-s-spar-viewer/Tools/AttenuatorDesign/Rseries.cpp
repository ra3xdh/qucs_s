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

// Reference: Based on design equations from attenuatorfunc.cpp R_SERIES case

void AttenuatorDesigner::RSeriesAttenuator() {

  ComponentInfo TermSpar1, TermSpar2;
  ComponentInfo Res1;
  Components.clear();

         // Design equations
  double L = pow(10, -Specs.Attenuation / 10);

  double R1 = -((Specs.Zout + Specs.Zin)*L - 2*sqrt(Specs.Zout*Specs.Zin*L)) / L; // Series resistor
  double Zin = Specs.Zout + R1;  // Input impedance
  double Zout = Specs.Zin + R1;   // Output impedance

         // Power dissipation
  Pdiss.R1 = Specs.Pin * (1 - L);

         // Circuit implementation

         // Input terminal
  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Zin, Resistance); // input impedance is R2
  Schematic.appendComponent(TermSpar1);

  // Zin label
  QString Zin_label = QString("Zin = %1 \u03A9").arg(num2str(Zin));
  QGraphicsTextItem* label1 = new QGraphicsTextItem(Zin_label);
  label1->setDefaultTextColor(Qt::red);
  label1->setFont(QFont("Arial", 6, QFont::Bold));
  label1->setPos(10, -20);
  Schematic.appendText(label1);


         // Series resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

         // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

         // Wiring connections
         // Input terminal to series resistor
  Schematic.appendWire(TermSpar1.ID, 0, Res1.ID, 0);

         // Series resistor to output terminal
  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);

  // Zout label
  QString Zout_label = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem* label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(130, -20);
  Schematic.appendText(label2);

}
