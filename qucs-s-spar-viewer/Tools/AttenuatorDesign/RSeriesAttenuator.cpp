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

#include "RSeriesAttenuator.h"

// Reference: Based on design equations from attenuatorfunc.cpp R_SERIES case

RSeriesAttenuator::RSeriesAttenuator() {}

RSeriesAttenuator::RSeriesAttenuator(AttenuatorDesignParameters AS)
    : AttenuatorBase(AS) {}

RSeriesAttenuator::~RSeriesAttenuator() {}

void RSeriesAttenuator::calculateParams() {
  // Design equations
  double L = pow(10, -Specification.Attenuation / 10);

  R1 = -((Specification.Zout + Specification.Zin) * L -
         2 * sqrt(Specification.Zout * Specification.Zin * L)) /
       L;                        // Series resistor
  Zin = Specification.Zout + R1; // Input impedance
  Zout = Specification.Zin + R1; // Output impedance

  // Power dissipation
  Pdiss["R1"] = Specification.Pin * (1 - L);
}

void RSeriesAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void RSeriesAttenuator::buildNetwork() {
  ComponentInfo Res1;

  // Input terminal
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Zin label
  QString Zin_label = QString("Zin = %1 \u03A9").arg(num2str(Zin));
  QGraphicsTextItem *label1 = new QGraphicsTextItem(Zin_label);
  label1->setDefaultTextColor(Qt::red);
  label1->setFont(QFont("Arial", 6, QFont::Bold));
  label1->setPos(10, -20);
  Schematic.appendText(label1);

  // Series resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 200, 0);
  TermSpar2.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Wiring connections
  Schematic.appendWire(TermSpar1.ID, 0, Res1.ID, 0);
  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);

  // Zout label
  QString Zout_label = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem *label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(130, -20);
  Schematic.appendText(label2);
}
