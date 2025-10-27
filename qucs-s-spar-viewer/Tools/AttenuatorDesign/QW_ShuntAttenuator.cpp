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

#include "QW_ShuntAttenuator.h"

QW_ShuntAttenuator::QW_ShuntAttenuator() {}

QW_ShuntAttenuator::QW_ShuntAttenuator(AttenuatorDesignParameters AS)
    : AttenuatorBase(AS) {}

QW_ShuntAttenuator::~QW_ShuntAttenuator() {}

void QW_ShuntAttenuator::calculateParams() {
  // Design equations
  R = Specification.Zin * (pow(10, .05 * Specification.Attenuation) - 1);
  l4 = .25 * SPEED_OF_LIGHT / Specification.Frequency;

  // Zout calculation
  Zout = R + Specification.Zin * (R + Specification.Zin) /
                 (2 * R + Specification.Zin);

  // Power dissipation
  double K = (R + Specification.Zin) * (R + Specification.Zin);
  Pdiss["R1"] = Specification.Pin * Specification.Zin * R / K;
  Pdiss["R2"] = Specification.Pin * R * R / K;
  Pdiss["R3"] = Pdiss["R1"];

  // For lumped implementation
  w0 = 2 * M_PI * Specification.Frequency;
}

void QW_ShuntAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void QW_ShuntAttenuator::buildNetwork() {
  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Lumped) {
    buildQW_Shunt_Lumped();
  } else if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildQW_Shunt_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildQW_Shunt_Microstrip();
  }
}

void QW_ShuntAttenuator::buildQW_Shunt_Lumped() {
  ComponentInfo Ground, Res1, Res2, Res3;
  ComponentInfo Lseries, Cshunt;
  NodeInfo NI1, NI2;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // First node (input side)
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Input terminal to first node
  Schematic.appendWire(TermSparIN.ID, 0, NI1.ID, 0);

  // Lumped transmission line: shunt C at input
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, 50, -40);
  Cshunt.val["C"] = num2str(1 / (Specification.Zin * w0), Capacitance);
  Schematic.appendComponent(Cshunt);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, -70);
  Schematic.appendComponent(Ground);

  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, 0, 50, 50);
  Lseries.val["L"] = num2str(Specification.Zin / w0, Inductance);
  Schematic.appendComponent(Lseries);

  Schematic.appendWire(Cshunt.ID, 0, NI1.ID, 0);
  Schematic.appendWire(Cshunt.ID, 1, Ground.ID, 0);
  Schematic.appendWire(Lseries.ID, 1, NI1.ID, 0);

  // Second node (bottom side of lumped TL)
  NI2.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50,
      100);
  Schematic.appendNode(NI2);
  Schematic.appendWire(Lseries.ID, 0, NI2.ID, 0);

  // 1st shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 150);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 200);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI2.ID, 0);

  // 2nd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 150);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 200);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res3.ID, 1, NI2.ID, 0);

  // Lumped TL output shunt C and GND
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, 150, 150);
  Cshunt.val["C"] = num2str(1 / (Specification.Zin * w0), Capacitance);
  Schematic.appendComponent(Cshunt);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 200);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Cshunt.ID, 1, NI2.ID, 0);

  // Series resistor from input node to output node
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  Schematic.appendWire(Res1.ID, 0, NI1.ID, 0);

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

  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);
}

void QW_ShuntAttenuator::buildQW_Shunt_IdealTL() {
  ComponentInfo Ground, Res1, Res2, Res3, TL;
  NodeInfo NI1, NI2;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // First node (input side)
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Input terminal to first node
  Schematic.appendWire(TermSparIN.ID, 0, NI1.ID, 0);

  // Ideal transmission line
  TL.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 50, 50);
  TL.val["Z0"] = num2str(Specification.Zin, Resistance);
  TL.val["Length"] = ConvertLengthFromM("mm", l4);
  Schematic.appendComponent(TL);
  Schematic.appendWire(TL.ID, 1, NI1.ID, 0);

  // Second node (bottom side of TL)
  NI2.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50,
      100);
  Schematic.appendNode(NI2);
  Schematic.appendWire(TL.ID, 0, NI2.ID, 0);

  // 1st shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 150);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 200);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI2.ID, 0);

  // 2nd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 150);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 200);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res3.ID, 1, NI2.ID, 0);

  // Series resistor from input node to output node
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  Schematic.appendWire(Res1.ID, 0, NI1.ID, 0);

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

  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);
}

void QW_ShuntAttenuator::buildQW_Shunt_Microstrip() {
  ComponentInfo Ground, Res1, Res2, Res3, MLIN;
  NodeInfo NI1, NI2;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // First node (input side)
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Input terminal to first node
  Schematic.appendWire(TermSparIN.ID, 0, NI1.ID, 0);

  // Microstrip transmission line
  MicrostripClass MSL;
  MSL.Substrate = Specification.MS_Subs;
  MSL.synthesizeMicrostrip(Specification.Zin, l4 * 1e3,
                           Specification.Frequency);

  MLIN.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 50, 50);
  MLIN.val["Width"] = ConvertLengthFromM("mm", MSL.Results.width);
  MLIN.val["Length"] = ConvertLengthFromM("mm", MSL.Results.length * 1e-3);
  MLIN.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN);
  Schematic.appendWire(MLIN.ID, 1, NI1.ID, 0);

  // Second node (bottom side of TL)
  NI2.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50,
      100);
  Schematic.appendNode(NI2);
  Schematic.appendWire(MLIN.ID, 0, NI2.ID, 0);

  // 1st shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 150);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 200);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI2.ID, 0);

  // 2nd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 150);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 200);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res3.ID, 1, NI2.ID, 0);

  // Series resistor from input node to output node
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 90, 100, 0);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  Schematic.appendWire(Res1.ID, 0, NI1.ID, 0);

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

  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);
}
