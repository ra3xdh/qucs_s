/***************************************************************************
                                TJunction.cpp
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

void PowerCombinerDesigner::TJunction() {
  // Design equations
  double K = Specs.OutputRatio.at(0) * Specs.OutputRatio.at(0);
  int index_t2 = 2;
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);

  ComponentInfo TL4, TL5; // Auxiliar lines for matching in case of K!=1

  ComponentInfo TermSpar1(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo TL1(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),  TransmissionLine, 90, 50, 0);
  TL1.val["Z0"] = num2str(Specs.Z0, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);

  // Node
  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 100, 0);
  Schematic.appendNode(N1);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 1);


  ComponentInfo TL2(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, 125, -50);
  TL2.val["Z0"] = num2str(Specs.Z0 * (K + 1), Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL2.ID, 0, N1.ID, 1);

  ComponentInfo TL3(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, 125, 50);
  TL3.val["Z0"] = num2str(Specs.Z0 * (K + 1) / K, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);

  Schematic.appendWire(TL3.ID, 0, N1.ID, 1);

  int xpos_term = 175;

  if (K != 1) { // Requires matching
    TL4.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, 200, -50);
    TL4.val["Z0"] =
        num2str(sqrt(2 * Specs.Z0 * Specs.Z0 * (K + 1)), Resistance);
    TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL4);

    Schematic.appendWire(TL4.ID, 0, TL2.ID, 1);

    TL5.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, 200, 50);
    TL5.val["Z0"] =
        num2str(sqrt(2 * Specs.Z0 * Specs.Z0 * (K + 1) / K), Resistance);
    TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL5);

    Schematic.appendWire(TL5.ID, 0, TL3.ID, 1);

    xpos_term += 100;
    index_t2 = 5;
  }

  ComponentInfo TermSpar2(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, xpos_term, -50);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  QString dst;
  (index_t2 == 5) ? dst = TL4.ID : dst = TL2.ID;
  Schematic.appendWire(TermSpar2.ID, 0, dst, 1);

  ComponentInfo TermSpar3( QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, xpos_term, 50);

  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  (index_t2 == 5) ? dst = TL5.ID : dst = TL3.ID;
  Schematic.appendWire(TermSpar3.ID, 0, dst, 1);
}
