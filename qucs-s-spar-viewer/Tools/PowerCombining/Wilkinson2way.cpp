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

#include "PowerCombinerDesigner.h"

void PowerCombinerDesigner::Wilkinson() {
  TwoWayWilkinsonParams WilkinsonParams = CalculateWilkinson();
  double lambda4                        = SPEED_OF_LIGHT / (4 * Specs.freq);
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo Ground;
  if (Specs.Implementation ==
      "Lumped LC") { // This implementation is based on the quarter wavelength
                     // transmission line Pi LC equivalent
    // Design equations
    double Z4, Z5, L2_, C2_, L3_, C3_;
    double K  = Specs.OutputRatio.at(0);
    double w  = 2 * M_PI * Specs.freq;
    double L2 = WilkinsonParams.Z2 / w;
    double C2 = 1. / (L2 * w * w);
    double L3 = WilkinsonParams.Z3 / w;
    double C3 = 1. / (L3 * w * w);
    double CC = C2 + C3;
    if (WilkinsonParams.R2 !=
        WilkinsonParams
            .R3) // Unequal output power rate => requires matching to Z0
    {
      Z4  = Specs.Z0 * sqrt(K);
      Z5  = Specs.Z0 / sqrt(K);
      L2_ = Z4 / w;
      L3_ = Z5 / w;
      C2_ = 1. / (L2_ * w * w);
      C3_ = 1. / (L3_ * w * w);
      // Embed the first capacitor of the Pi quarter wave equivalent in the last
      // C of the Wilkinson structure
      C2 += C2_;
      C3 += C3_;
    }

    // Build the circuit and the netlist
    TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 0, 50, 0);
    TermSpar1.val["Z0"] = num2str(Specs.Z0, Resistance);
    Schematic.appendComponent(TermSpar1);

    // Shunt capacitor
    ComponentInfo Cshunt1(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, 100, 20);
    Cshunt1.val["C"] = num2str(CC, Capacitance);
    Schematic.appendComponent(Cshunt1);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, 100, 65);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Cshunt1.ID, 0, Ground.ID, 0);

    NodeInfo N1(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 100,
        0);
    Schematic.appendNode(N1);

    Schematic.appendWire(Cshunt1.ID, 1, N1.ID, 0);
    Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 1);

    // Upper branch
    // Series inductor
    ComponentInfo Lseries1(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, 150, -75);
    Lseries1.val["L"] = num2str(L2, Inductance);
    Schematic.appendComponent(Lseries1);

    // Cshunt
    ComponentInfo Cshunt2(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, 200, -55);
    Cshunt2.val["C"] = num2str(C2, Capacitance);
    Schematic.appendComponent(Cshunt2);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, 200, -10);
    Schematic.appendComponent(Ground);

    NodeInfo N2(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 250,
        -75);
    Schematic.appendNode(N2);

    NodeInfo N2_(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
        -75);
    Schematic.appendNode(N2_);

    Schematic.appendWire(N1.ID, 0, Lseries1.ID, 1);
    Schematic.appendWire(Lseries1.ID, 0, N2_.ID, 0);
    Schematic.appendWire(Cshunt2.ID, 0, Ground.ID, 0);
    Schematic.appendWire(Cshunt2.ID, 1, N2_.ID, 0);
    Schematic.appendWire(N2_.ID, 0, N2.ID, 0);

    // Lower branch
    // Series inductor
    ComponentInfo Lseries2(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, 150, 75);
    Lseries2.val["L"] = num2str(L3, Inductance);
    Schematic.appendComponent(Lseries2);

    // Cshunt
    ComponentInfo Cshunt3(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, 200, 95);
    Cshunt3.val["C"] = num2str(C3, Capacitance);
    Schematic.appendComponent(Cshunt3);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, 200, 130);
    Schematic.appendComponent(Ground);

    NodeInfo N3(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 250,
        75);
    Schematic.appendNode(N3);

    NodeInfo N3_(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
        75);
    Schematic.appendNode(N3_);

    Schematic.appendWire(N1.ID, 0, Lseries2.ID, 1);
    Schematic.appendWire(Lseries2.ID, 0, N3_.ID, 0);
    Schematic.appendWire(Cshunt3.ID, 0, Ground.ID, 0);
    Schematic.appendWire(Cshunt3.ID, 1, N3_.ID, 0);
    Schematic.appendWire(N3_.ID, 0, N3.ID, 0);

    ComponentInfo Risolation(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        250, 0);
    Risolation.val["R"] = num2str(WilkinsonParams.R, Resistance);
    Schematic.appendComponent(Risolation);

    Schematic.appendWire(Risolation.ID, 0, N3.ID, 1);
    Schematic.appendWire(Risolation.ID, 1, N2.ID, 0);

    ComponentInfo Ls3, Ls4, Cp4, Cp5;
    if (Specs.OutputRatio.at(0) !=
        1) { // An unequal power ratio implies that the load impedance != Z0, so
             // it requires matching

      // Upper branch
      Ls3.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, 285, -75);
      Ls3.val["L"] = num2str(L2_, Inductance);
      Schematic.appendComponent(Ls3);

      Schematic.appendWire(Ls3.ID, 1, N2.ID, 0);

      // Cshunt
      Cp4.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                    Capacitor, 0, 320, -50);
      Cp4.val["C"] = num2str(C2_, Capacitance);
      Schematic.appendComponent(Cp4);

      Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, 320, -10);
      Schematic.appendComponent(Ground);

      NodeInfo N4(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          320, -75);
      Schematic.appendNode(N4);

      Schematic.appendWire(Ls3.ID, 0, N4.ID, 0);
      Schematic.appendWire(Cp4.ID, 1, N4.ID, 0);
      Schematic.appendWire(Cp4.ID, 0, Ground.ID, 0);

      TermSpar2.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          370, -50);
      TermSpar2.val["Z0"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar2);

      Schematic.appendWire(N4.ID, 0, TermSpar2.ID, 0);

      // Lower branch
      Ls4.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, 285, 75);
      Ls4.val["L"] = num2str(L3_, Inductance);
      Schematic.appendComponent(Ls4);

      Schematic.appendWire(Ls4.ID, 1, N3.ID, 0);

      // Cshunt
      Cp5.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                    Capacitor, 0, 320, 100);
      Cp5.val["C"] = num2str(C3_, Capacitance);
      Schematic.appendComponent(Cp5);

      Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, 320, 140);
      Schematic.appendComponent(Ground);

      NodeInfo N5(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          320, 75);
      Schematic.appendNode(N5);

      Schematic.appendWire(Ls4.ID, 0, N5.ID, 0);
      Schematic.appendWire(Cp5.ID, 1, N5.ID, 0);
      Schematic.appendWire(Cp5.ID, 0, Ground.ID, 0);

      TermSpar3.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          370, 75);
      TermSpar3.val["Z0"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar3);

      Schematic.appendWire(N5.ID, 0, TermSpar3.ID, 0);
    } else { // Just put the output terms
      TermSpar2.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          280, -75);
      TermSpar2.val["Z0"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar2);
      Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);

      TermSpar3.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          280, 75);
      TermSpar3.val["Z0"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar3);
      Schematic.appendWire(TermSpar3.ID, 0, N3.ID, 0);
    }
  }

  if (Specs.Implementation == "Ideal TL") {
    TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 0, 0, 0);
    TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
    Schematic.appendComponent(TermSpar1);

    // 1st transmission line
    ComponentInfo TL1(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, 50, 0);
    TL1.val["Z0"]     = num2str(Specs.Z0, Resistance);
    TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL1);

    Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);

    // Node
    NodeInfo N1(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 100,
        0);
    Schematic.appendNode(N1);
    Schematic.appendWire(TL1.ID, 1, N1.ID, 1);

    // Upper branch TL
    // 1st transmission line
    ComponentInfo TL2(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, 135, -50);
    TL2.val["Z0"]     = num2str(WilkinsonParams.Z2, Resistance);
    TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL2);

    Schematic.appendWire(TL2.ID, 0, N1.ID, 0);
    NodeInfo N2(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
        -50);
    Schematic.appendNode(N2);

    Schematic.appendWire(TL2.ID, 1, N2.ID, 0);
    // Lower branch TL
    // 1st transmission line
    ComponentInfo TL3(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, 135, 50);
    TL3.val["Z0"]     = num2str(WilkinsonParams.Z3, Resistance);
    TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL3);

    Schematic.appendWire(TL3.ID, 0, N1.ID, 0);

    NodeInfo N3(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
        50);
    Schematic.appendNode(N3);
    Schematic.appendWire(TL3.ID, 1, N3.ID, 0);
    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        200, 0);
    Riso.val["R"] = num2str(WilkinsonParams.R, Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, N2.ID, 0);
    Schematic.appendWire(Riso.ID, 0, N3.ID, 0);

    if (Specs.OutputRatio.at(0) !=
        1) { // An unequal power ratio implies that the load impedance != Z0, so
             // it requires matching

      // Upper branch matching transmission line
      ComponentInfo TL4(
          QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
          TransmissionLine, 90, 275, -50);
      TL4.val["Z0"] = num2str(sqrt(Specs.Z0 * WilkinsonParams.R2), Resistance);
      TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
      Schematic.appendComponent(TL4);

      // Upper branch term
      TermSpar2.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          325, -50);
      TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar2);

      Schematic.appendWire(TL4.ID, 0, N2.ID, 0);
      Schematic.appendWire(TL4.ID, 1, TermSpar2.ID, 0);

      // Lower branch matching transmission line
      ComponentInfo TL5(
          QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
          TransmissionLine, 90, 275, 50);
      TL5.val["Z0"] = num2str(sqrt(Specs.Z0 * WilkinsonParams.R3), Resistance);
      TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
      Schematic.appendComponent(TL5);

      // Lower branch term
      TermSpar3.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          325, 50);
      TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar3);

      Schematic.appendWire(TL5.ID, 0, N3.ID, 0);
      Schematic.appendWire(TL5.ID, 1, TermSpar3.ID, 0);

    } else { // Just put the output terms
      // Upper branch term
      TermSpar2.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          250, -50);
      TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar2);
      Schematic.appendWire(N2.ID, 1, TermSpar2.ID, 0);

      // Lower branch term
      TermSpar3.setParams(
          QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
          250, 50);
      TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
      Schematic.appendComponent(TermSpar3);
      Schematic.appendWire(N3.ID, 1, TermSpar3.ID, 0);
    }
  }
}
