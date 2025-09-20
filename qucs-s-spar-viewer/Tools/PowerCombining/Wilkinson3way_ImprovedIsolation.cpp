/***************************************************************************
                                Wilkinson3way_ImprovedIsolation.cpp
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
 * II". Andrei Grebennikov. High Frequency Electronics. 2008 [2] "New 3N way
 * hybrid power dividers", IEEE Trans. Microwave Theory Tech., vol. MTT-25, Dec.
 * 1977, pp. 1008-1012
 */
void PowerCombinerDesigner::Wilkinson3Way_ImprovedIsolation() {
  double Z1 = 2.28 * Specs.Z0;
  double Z2 = 1.316 * Specs.Z0;
  double R1 = 1.319 * Specs.Z0;
  double R2 = 4 * Specs.Z0;

  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);

  ComponentInfo TermSpar1(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -25, 0);
  TermSpar1.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo N0(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              0, 0);
  Schematic.appendNode(N0);

  Schematic.appendWire(TermSpar1.ID, 0, N0.ID, 0);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 0);
  TL1.val["Z0"] = num2str(Z1, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 0);
  Schematic.appendNode(N1);

  Schematic.appendWire(TL1.ID, 0, N0.ID, 0);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, -100);
  TL2.val["Z0"] = num2str(Z1, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, -100);
  Schematic.appendNode(N2);

  Schematic.appendWire(TL2.ID, 0, N0.ID, 0);
  Schematic.appendWire(TL2.ID, 1, N2.ID, 0);

  // Isolation resistor
  ComponentInfo Ri1(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 100, -50);
  Ri1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Ri1);

  Schematic.appendWire(N2.ID, 0, Ri1.ID, 1);
  Schematic.appendWire(N1.ID, 0, Ri1.ID, 0);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 100);
  TL3.val["Z0"] = num2str(Z1, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);

  NodeInfo N3(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 100);
  Schematic.appendNode(N3);

  Schematic.appendWire(TL3.ID, 0, N0.ID, 0);
  Schematic.appendWire(TL3.ID, 1, N3.ID, 0);

  // Isolation resistor
  ComponentInfo Ri2(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 100, 50);
  Ri2.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Ri2);

  Schematic.appendWire(Ri2.ID, 1, N1.ID, 0);
  Schematic.appendWire(Ri2.ID, 0, N3.ID, 0);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 0);
  TL4.val["Z0"] = num2str(Z2, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL4);

  NodeInfo N4(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 0);
  Schematic.appendNode(N4);

  Schematic.appendWire(TL4.ID, 0, N1.ID, 0);
  Schematic.appendWire(TL4.ID, 1, N4.ID, 0);

  ComponentInfo TermSpar2(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 225, 0);
  TermSpar2.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSpar2.ID, 0, N4.ID, 0);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, -100);
  TL5.val["Z0"] = num2str(Z2, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL5);

  NodeInfo N5(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, -100);
  Schematic.appendNode(N5);

  Schematic.appendWire(TL5.ID, 0, N2.ID, 0);
  Schematic.appendWire(TL5.ID, 1, N5.ID, 0);

  // Isolation resistor
  ComponentInfo Ri3(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 200, -50);
  Ri3.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri3);

  ComponentInfo TermSpar3(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 225, -100);
  TermSpar3.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  Schematic.appendWire(Ri3.ID, 1, N5.ID, 0);
  Schematic.appendWire(Ri3.ID, 0, N4.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N5.ID, 0);

  ComponentInfo TL6(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 100);
  TL6.val["Z0"] = num2str(Z2, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL6);

  NodeInfo N6(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              200, 100);
  Schematic.appendNode(N6);

  Schematic.appendWire(TL6.ID, 0, N3.ID, 0);
  Schematic.appendWire(TL6.ID, 1, N6.ID, 0);

  // Isolation resistor
  ComponentInfo Ri4(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 200, 50);
  Ri4.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri4);

  ComponentInfo TermSpar4(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 225, 100);
  TermSpar4.val["Z0"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  Schematic.appendWire(N6.ID, 0, TermSpar4.ID, 0);
  Schematic.appendWire(Ri4.ID, 0, N6.ID, 0);
  Schematic.appendWire(Ri4.ID, 1, N4.ID, 0);
}
