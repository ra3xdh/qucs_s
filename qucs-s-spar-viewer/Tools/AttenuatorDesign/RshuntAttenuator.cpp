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

#include "RshuntAttenuator.h"
// Reference: Based on design equations from attenuatorfunc.cpp R_SHUNT case

RShuntAttenuator::RShuntAttenuator() {}

RShuntAttenuator::RShuntAttenuator(AttenuatorDesignParameters AS)
    : AttenuatorBase(AS) {}

RShuntAttenuator::~RShuntAttenuator() {}

void RShuntAttenuator::calculateParams() {
  // Design equations
  double L = pow(10, -Specification.Attenuation / 10);

  R1 = (2 * sqrt(Specification.Zout * Specification.Zin * L) *
            Specification.Zout * Specification.Zin +
        (Specification.Zout * Specification.Zout * Specification.Zin +
         Specification.Zout * Specification.Zin * Specification.Zin) *
            L) /
       (4 * Specification.Zout * Specification.Zin -
        (Specification.Zout * Specification.Zout +
         2 * Specification.Zout * Specification.Zin +
         Specification.Zin * Specification.Zin) *
            L); // Shunt resistor

  // Input impedance
  Zin = (Specification.Zout * R1) / (Specification.Zout + R1);

  // Output impedance
  Zout = (Specification.Zin * R1) / (Specification.Zin + R1);

  // Power dissipation
  Pdiss["R1"] = Specification.Pin * (1 - L);
}

void RShuntAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void RShuntAttenuator::buildNetwork() {
  ComponentInfo Res1, Ground;
  NodeInfo NIin;

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
  label1->setPos(-20, -30);
  Schematic.appendText(label1);

  // Node before shunt
  NIin.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NIin);

  // Shunt resistor to ground
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 100);
  Schematic.appendComponent(Ground);

  // Output terminal
  ComponentInfo TermSpar2;
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, 100, 0);
  TermSpar2.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Wiring connections
  Schematic.appendWire(TermSpar1.ID, 0, NIin.ID, 0);
  Schematic.appendWire(NIin.ID, 0, Res1.ID, 1);
  Schematic.appendWire(Res1.ID, 0, Ground.ID, 0);
  Schematic.appendWire(NIin.ID, 0, TermSpar2.ID, 0);

  // Zout label
  QString Zout_label = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem *label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(80, -30);
  Schematic.appendText(label2);
}
