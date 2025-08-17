/***************************************************************************
                                Lim_Eom.cpp
                                ----------
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#include "PowerCombinerDesigner.h"

/*
 * References:
 * [1] "Power combiners, impedance transformers and directional couplers: part
 * II". Andrei Grebennikov. High Frequency Electronics. 2008 [2] “A New 3-Way
 * Power Divider with Various Output Power Ratios,” J.-S. Lim and S.-Y. Eom,
 * 1996 IEEE MTT-S Int. Microwave Symp. Dig., pp. 785-788."
 */
void PowerCombinerDesigner::Lim_Eom() {
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);

  double M = Specs.OutputRatio.at(0);
  double N = Specs.OutputRatio.at(1);
  double K = Specs.OutputRatio.at(2);

  double d1 = M + N + K;
  double d2 = N + K;
  double Z1 = Specs.Z0 * sqrt(d1 / d2);
  double Z2 = Specs.Z0 * sqrt(d1 / M);

  double Z4 = Specs.Z0 * sqrt(d2 / N);
  double Z5 = Specs.Z0 * sqrt(d2 / K);

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, -25,
      -100);
  TermSpar1.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo N0(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, -100);
  Schematic.appendNode(N0);

  Schematic.appendWire(TermSpar1.ID, 0, N0.ID, 0);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, -100);
  TL1.val["Z0"] = num2str(Z2, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(TL1.ID, 0, N0.ID, 0);

  // Node
  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, -100);
  Schematic.appendNode(N1);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, -90, 100,
      -130);
  TermSpar2.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSpar2.ID, 0, N1.ID, 0);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, -100);
  TL2.val["Z0"] = num2str(Z1, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);
  Schematic.appendWire(TL2.ID, 0, N1.ID, 0);

  // Node
  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, -100);
  Schematic.appendNode(N2);
  Schematic.appendWire(TL2.ID, 1, N2.ID, 0);

  // Isolation resistor
  ComponentInfo Ri1(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 260, -60);
  Ri1.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Ri1);

  ComponentInfo Ground1(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 260, -20);
  Schematic.appendComponent(Ground1);
  Schematic.appendWire(Ri1.ID, 1, N2.ID, 0);
  Schematic.appendWire(Ri1.ID, 0, Ground1.ID, 0);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 200, -50);
  TL3.val["Z0"] = num2str(Z2, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);
  Schematic.appendWire(TL3.ID, 1, N2.ID, 0);

  // Node
  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 0);
  Schematic.appendNode(N3);
  Schematic.appendWire(TL3.ID, 0, N3.ID, 0);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 200, 50);
  TL4.val["Z0"] = num2str(Z4, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL4);

  Schematic.appendWire(TL4.ID, 1, N3.ID, 0);

  // Node
  NodeInfo N4(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 100);
  Schematic.appendNode(N4);
  Schematic.appendWire(TL4.ID, 0, N4.ID, 0);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 220,
      100);
  TermSpar3.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, N4.ID, 0);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 100);
  TL5.val["Z0"] = num2str(Z5, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL5);
  Schematic.appendWire(TL5.ID, 1, N4.ID, 0);

  // Node
  NodeInfo N5(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 100);
  Schematic.appendNode(N5);
  Schematic.appendWire(TL5.ID, 0, N5.ID, 0);

  // Isolation resistor
  ComponentInfo Ri2(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 100, 150);
  Ri2.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Ri2);

  ComponentInfo Ground2(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 100, 200);
  Schematic.appendComponent(Ground2);
  Schematic.appendWire(Ri2.ID, 1, N5.ID, 0);
  Schematic.appendWire(Ri2.ID, 0, Ground2.ID, 0);

  ComponentInfo TL6(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 100);
  TL6.val["Z0"] = num2str(Z4, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL6);
  Schematic.appendWire(TL6.ID, 1, N5.ID, 0);

  // Node
  NodeInfo N6(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 100);
  Schematic.appendNode(N6);
  Schematic.appendWire(TL6.ID, 0, N6.ID, 0);

  ComponentInfo TermSpar4(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, -20,
      100);
  TermSpar4.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);
  Schematic.appendWire(TermSpar4.ID, 0, N6.ID, 0);

  ComponentInfo TL7(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 50);
  TL7.val["Z0"] = num2str(Z5, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL7);
  Schematic.appendWire(TL7.ID, 0, N6.ID, 0);

  // Node
  NodeInfo N7(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 0);
  Schematic.appendNode(N7);
  Schematic.appendWire(TL7.ID, 1, N7.ID, 0);

  ComponentInfo TL8(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, -50);
  TL8.val["Z0"] = num2str(Z1, Resistance);
  TL8.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL8);
  Schematic.appendWire(TL8.ID, 0, N7.ID, 0);
  Schematic.appendWire(TL8.ID, 1, N0.ID, 0);

  ComponentInfo TL9(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 100, 0);
  TL9.val["Z0"] = num2str(Specs.Z0, Resistance);
  TL9.val["Length"] = ConvertLengthFromM(Specs.units, 2 * lambda4);
  Schematic.appendComponent(TL9);
  Schematic.appendWire(TL9.ID, 0, N7.ID, 0);
  Schematic.appendWire(TL9.ID, 1, N3.ID, 0);
}
