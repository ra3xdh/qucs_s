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

#include "QW_SeriesAttenuator.h"

QW_SeriesAttenuator::QW_SeriesAttenuator() {}

QW_SeriesAttenuator::QW_SeriesAttenuator(AttenuatorDesignParameters AS)
    : AttenuatorBase(AS) {}

QW_SeriesAttenuator::~QW_SeriesAttenuator() {}

void QW_SeriesAttenuator::calculateParams() {
  // Design equations
  R = Specification.Zin / (pow(10, .05 * Specification.Attenuation) - 1);
  l4 = .25 * SPEED_OF_LIGHT / Specification.Frequency;

  // Zout calculation
  Zout = (R * R * Specification.Zin +
          2 * R * Specification.Zin * Specification.Zin) /
         (R * R + 2 * R * Specification.Zin +
          2 * Specification.Zin * Specification.Zin);

  // Power dissipation
  double K = (R + Specification.Zin) * (R + Specification.Zin);
  Pdiss["R1"] = Specification.Pin * Specification.Zin * R / K;
  Pdiss["R2"] = Specification.Pin * Specification.Zin * Specification.Zin / K;
  Pdiss["R3"] = Pdiss["R1"];

  // For lumped implementation
  w0 = 2 * M_PI * Specification.Frequency;
}

void QW_SeriesAttenuator::synthesize() {
  calculateParams();
  buildNetwork();
}

void QW_SeriesAttenuator::buildNetwork() {
  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Lumped) {
    buildQW_Series_Lumped();
  } else if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildQW_Series_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildQW_Series_Microstrip();
  }
}

void QW_SeriesAttenuator::buildQW_Series_Lumped() {
  ComponentInfo Ground, Res1, Res2, Res3;
  ComponentInfo Lseries, Cshunt;
  NodeInfo NI;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // 1st shunt resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  // First node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);
  Schematic.appendWire(TermSparIN.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);

  // 2nd shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 125);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 175);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res1.ID, 0, Res2.ID, 1);
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);

  // Lumped transmission line: series L, shunt C (input side)
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, 50, -50);
  Cshunt.val["C"] = num2str(1 / (Specification.Zin * w0), Capacitance);
  Schematic.appendComponent(Cshunt);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   180, 50, -80);
  Schematic.appendComponent(Ground);
  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, 100, 0);
  Lseries.val["L"] = num2str(Specification.Zin / w0, Inductance);
  Schematic.appendComponent(Lseries);
  Schematic.appendWire(Lseries.ID, 1, NI.ID, 0);
  Schematic.appendWire(Cshunt.ID, 0, NI.ID, 0);
  Schematic.appendWire(Cshunt.ID, 1, Ground.ID, 0);

  // Second node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150,
      0);
  Schematic.appendNode(NI);

  // Lumped TL output C and GND
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, 150, -50);
  Cshunt.val["C"] = num2str(1 / (Specification.Zin * w0), Capacitance);
  Schematic.appendComponent(Cshunt);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   180, 150, -80);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Lseries.ID, 0, NI.ID, 0);
  Schematic.appendWire(Cshunt.ID, 0, NI.ID, 0);
  Schematic.appendWire(Cshunt.ID, 1, Ground.ID, 0);

  // 3rd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 150, 50);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 100);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res3.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);

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
  Schematic.appendWire(TermSpar2.ID, 0, NI.ID, 0);
}

void QW_SeriesAttenuator::buildQW_Series_IdealTL() {
  ComponentInfo Ground, Res1, Res2, Res3, TL;
  NodeInfo NI;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // 1st shunt resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  // First node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);
  Schematic.appendWire(TermSparIN.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);

  // 2nd shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 125);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 175);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res1.ID, 0, Res2.ID, 1);
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);

  // Ideal transmission line
  TL.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 100, 0);
  TL.val["Z0"] = num2str(Specification.Zin, Resistance);
  TL.val["Length"] = ConvertLengthFromM("mm", l4);
  Schematic.appendComponent(TL);
  Schematic.appendWire(TL.ID, 0, NI.ID, 0);

  // Second node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150,
      0);
  Schematic.appendNode(NI);
  Schematic.appendWire(NI.ID, 0, TL.ID, 1);

  // 3rd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 150, 50);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 100);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res3.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);

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
  Schematic.appendWire(TermSpar2.ID, 0, NI.ID, 0);
}

void QW_SeriesAttenuator::buildQW_Series_Microstrip() {
  ComponentInfo Ground, Res1, Res2, Res3, MLIN;
  NodeInfo NI;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // 1st shunt resistor
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res1);

  // First node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI);
  Schematic.appendWire(TermSparIN.ID, 0, NI.ID, 0);
  Schematic.appendWire(Res1.ID, 1, NI.ID, 0);

  // 2nd shunt resistor and ground
  Res2.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 50, 125);
  Res2.val["R"] = num2str(Specification.Zin, Resistance);
  Schematic.appendComponent(Res2);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 175);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res1.ID, 0, Res2.ID, 1);
  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);

  // Microstrip transmission line
  MicrostripClass MSL;
  MSL.Substrate = Specification.MS_Subs;
  MSL.synthesizeMicrostrip(Specification.Zin, l4 * 1e3,
                           Specification.Frequency);

  MLIN.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 100, 0);
  MLIN.val["Width"] = ConvertLengthFromM("mm", MSL.Results.width);
  MLIN.val["Length"] = ConvertLengthFromM("mm", MSL.Results.length * 1e-3);
  MLIN.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN);
  Schematic.appendWire(MLIN.ID, 0, NI.ID, 0);

  // Second node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 150,
      0);
  Schematic.appendNode(NI);
  Schematic.appendWire(NI.ID, 0, MLIN.ID, 1);

  // 3rd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 150, 50);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 100);
  Schematic.appendComponent(Ground);
  Schematic.appendWire(Res3.ID, 1, NI.ID, 0);
  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);

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
  Schematic.appendWire(TermSpar2.ID, 0, NI.ID, 0);
}
