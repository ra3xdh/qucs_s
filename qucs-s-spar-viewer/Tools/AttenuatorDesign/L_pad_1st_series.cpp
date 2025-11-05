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

#include "L_pad_1st_series.h"

// Reference: Based on the design equations from attenuatorfunc.cpp
// L_PAD_1ST_SERIES case

LPadFirstSeries::LPadFirstSeries() {}

LPadFirstSeries::LPadFirstSeries(AttenuatorDesignParameters AS)
    : AttenuatorBase(AS) {}

LPadFirstSeries::~LPadFirstSeries() {}

void LPadFirstSeries::calculateParams() {
  // Design equations
  double L = pow(10, -Specification.Attenuation / 10);
  R1 = -Specification.Zin * (L - 1) / (sqrt(L) + 1); // Series resistor
  R2 = -Specification.Zin * (L + sqrt(L)) / (L - 1); // Shunt resistor
  Zout = R2 * (R1 + Specification.Zin) /
         (R1 + R2 + Specification.Zin); // Output impedance

  // Power dissipation calculation (from attenuatorfunc.cpp)
  Pdiss["R1"] = Specification.Pin * (1 - sqrt(L)); // Series resistor
  Pdiss["R2"] =
      Specification.Pin * L * (1 - L) / (L + sqrt(L)); // Shunt resistor
}

void LPadFirstSeries::synthesize() {
  calculateParams();
  buildNetwork();
}

void LPadFirstSeries::buildNetwork() {
  ComponentInfo Ground, Res1, Res2;
  NodeInfo NI;

  // Input terminal
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specification.Zin, Resistance);
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
  QString Zout_label = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem *label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(130, -20);
  Schematic.appendText(label2);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Wiring connections
  Schematic.appendWire(TermSpar1.ID, 0, Res1.ID, 0);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);
  Schematic.appendWire(NI.ID, 0, Res2.ID, 1);
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(NI.ID, 0, TermSpar2.ID, 0);
}
