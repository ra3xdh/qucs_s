/***************************************************************************
                                Bagley.cpp
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

void PowerCombinerDesigner::Bagley() {

  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);
  double lambda2 = lambda4 * 2;
  double Zbranch = 2 * Specs.Z0 / sqrt(Specs.Noutputs);
  NodeInfo NI;

  ComponentInfo TermSpar(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 90, (Specs.Noutputs - 1) * 50, -30);
  TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              (Specs.Noutputs - 1) * 50, 0);
  Schematic.appendNode(N1);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, (Specs.Noutputs - 1) * 100, 50);
  TL1.val["Z0"] = num2str(Zbranch, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 50);
  TL2.val["Z0"] = num2str(Zbranch, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL2.ID, 1, N1.ID, 0);
  Schematic.appendWire(TermSpar.ID, 0, N1.ID, 0);

  TermSpar.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, -90, 0, 120);
  TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar);

  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0,
      100);
  Schematic.appendNode(NI);

  Schematic.appendWire(TermSpar.ID, 0, NI.ID, 0);
  Schematic.appendWire(TL2.ID, 0, NI.ID, 0);

  ComponentInfo TL;
  int posx = -50;
  for (int i = 1; i < Specs.Noutputs; i++) {
    posx += 100;
    TL.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx, 100);
    TL.val["Z0"] = num2str(Zbranch, Resistance);
    TL.val["Length"] = ConvertLengthFromM(Specs.units, lambda2);
    Schematic.appendComponent(TL);
    Schematic.appendWire(NI.ID, 0, TL.ID, 0);

    TermSpar.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, -90, posx + 50, 120);
    TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
    Schematic.appendComponent(TermSpar);

    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, 100);
    Schematic.appendNode(NI);

    Schematic.appendWire(NI.ID, 0, TL.ID, 1);
    Schematic.appendWire(NI.ID, 0, TermSpar.ID, 0);
  }

  Schematic.appendWire(TL1.ID, 0, NI.ID, 0);
}
