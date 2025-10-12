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

void AttenuatorDesigner::QW_ShuntAttenuator() {
  Components.clear();

  // Design equations
  double R = Specs.Zin * (pow(10, .05 * Specs.Attenuation) - 1);
  double l4 = .25 * SPEED_OF_LIGHT / Specs.Frequency;

  // Power dissipation
  double K = (R + Specs.Zin) * (R + Specs.Zin);
  Pdiss.R1 = Specs.Pin * Specs.Zin * R / K;
  Pdiss.R2 = Specs.Pin * R * R / K;
  Pdiss.R3 = Pdiss.R1;

  // Zout calculation
  double Zout = R + Specs.Zin * (R + Specs.Zin) / (2 * R + Specs.Zin);

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Lumped) {
    buildAttenuator_QW_Shunt_Lumped(R, Zout);
  } else {
    if (Specs.TL_implementation == TransmissionLineType::Ideal) {
      buildAttenuator_QW_Shunt_IdealTL(R, l4, Zout);
    } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
      buildAttenuator_QW_Shunt_Microstrip(R, l4, Zout);
    }
  }
}

void AttenuatorDesigner::buildAttenuator_QW_Shunt_Lumped(double R,
                                                         double Zout) {
  ComponentInfo Ground, Res1, Res2, Res3;
  ComponentInfo Lseries, Cshunt;
  NodeInfo NI1, NI2;

  double w0 = 2 * M_PI * Specs.Frequency;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // First node (input side)
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Input terminal to first node
  Schematic.appendWire(TermSparIN.ID, 0, NI1.ID, 0);

  // Lumped transmission line: shunt C at input
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, 50, -50);
  Cshunt.val["C"] = num2str(1 / (Specs.Zin * w0), Capacitance);
  Schematic.appendComponent(Cshunt);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   180, 50, -75);
  Schematic.appendComponent(Ground);

  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, 0, 50, 50);
  Lseries.val["L"] = num2str(Specs.Zin / w0, Inductance);
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
  Res2.val["R"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 205);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI2.ID, 0);

  // 2nd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 150);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 205);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res3.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res3.ID, 1, NI2.ID, 0);

  // Lumped TL output shunt C and GND
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, 150, 150);
  Cshunt.val["C"] = num2str(1 / (Specs.Zin * w0), Capacitance);
  Schematic.appendComponent(Cshunt);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 150, 205);
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
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);
}

void AttenuatorDesigner::buildAttenuator_QW_Shunt_IdealTL(double R, double l4,
                                                          double Zout) {
  ComponentInfo Ground, Res1, Res2, Res3, TL;
  NodeInfo NI1, NI2;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specs.Zin, Resistance);
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
  TL.val["Z0"] = num2str(Specs.Zin, Resistance);
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
  Res2.val["R"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 205);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI2.ID, 0);

  // 2nd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 150);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 205);
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
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);
}

void AttenuatorDesigner::buildAttenuator_QW_Shunt_Microstrip(double R,
                                                             double l4,
                                                             double Zout) {
  ComponentInfo Ground, Res1, Res2, Res3, TL;
  NodeInfo NI1, NI2;

  // Input terminal
  ComponentInfo TermSparIN(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSparIN.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSparIN);

  // First node (input side)
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Input terminal to first node
  Schematic.appendWire(TermSparIN.ID, 0, NI1.ID, 0);

  // Microstrip transmission line
  MicrostripClass MSL;
  MSL.Substrate = Specs.MS_Subs;
  MSL.synthesizeMicrostrip(Specs.Zin, l4 * 1e3, Specs.Frequency);

  double MS_Width = MSL.Results.width;
  double MS_Length = MSL.Results.length * 1e-3;

  TL.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 50, 50);
  TL.val["Width"] = ConvertLengthFromM("mm", MS_Width);
  TL.val["Length"] = ConvertLengthFromM("mm", MS_Length);
  TL.val["er"] = num2str(Specs.MS_Subs.er);
  TL.val["h"] = num2str(Specs.MS_Subs.height);
  TL.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  TL.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  TL.val["tand"] = num2str(Specs.MS_Subs.tand);
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
  Res2.val["R"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(Res2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 50, 205);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Res2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Res2.ID, 1, NI2.ID, 0);

  // 2nd shunt resistor and ground
  Res3.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                 Resistor, 0, 100, 150);
  Res3.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Res3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, 100, 205);
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
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(Res1.ID, 1, TermSpar2.ID, 0);
}
