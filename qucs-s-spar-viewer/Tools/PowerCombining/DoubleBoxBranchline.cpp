/***************************************************************************
                                DoubleBoxBranchline.cpp
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

void PowerCombinerDesigner::DoubleBoxBranchline() {

  double K = Specs.OutputRatio.at(0);
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);
  double r = 1;
  double t = sqrt((1 + K) * r);
  double ZA = Specs.Z0 * sqrt(r * (t * t - r)) / (t - r);
  double ZD = Specs.Z0 * sqrt(r * (t * t - r)) / (t - 1);
  double ZB = Specs.Z0 * sqrt(r - (r * r) / (t * t));

  ComponentInfo TermSpar1(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20, -50);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo NSP1(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0,
      -50);
  Schematic.appendNode(NSP1);

  ComponentInfo TermSpar2(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 220, -50);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  NodeInfo NSP2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200, -50);
  Schematic.appendNode(NSP2);

  ComponentInfo TermSpar3(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 220, 50);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  NodeInfo NSP3(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
      50);
  Schematic.appendNode(NSP3);

  ComponentInfo Riso(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                     Resistor, 0, 0, 75);
  Riso.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Riso);

  NodeInfo NIso(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0, 50);
  Schematic.appendNode(NIso);

  ComponentInfo Ground(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, 0, 120);
  Schematic.appendComponent(Ground);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, -50);
  TL1.val["Z0"] = num2str(ZB, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  // Node
  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, -50);
  Schematic.appendNode(N1);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 50);
  TL2.val["Z0"] = num2str(ZB, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  // Node
  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 50);
  Schematic.appendNode(N2);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 0);
  TL3.val["Z0"] = num2str(ZA, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 100, 0);
  TL4.val["Z0"] = num2str(ZB, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL4);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, -50);
  TL5.val["Z0"] = num2str(ZB, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL5);

  ComponentInfo TL6(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 150, 50);
  TL6.val["Z0"] = num2str(ZB, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL6);

  ComponentInfo TL7(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 200, 0);
  TL7.val["Z0"] = num2str(ZD, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL7);

  Schematic.appendWire(TermSpar1.ID, 0, NSP1.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, NSP2.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, NSP3.ID, 0);
  Schematic.appendWire(Riso.ID, 1, NIso.ID, 0);

  Schematic.appendWire(NSP1.ID, 0, TL1.ID, 0);
  Schematic.appendWire(NSP2.ID, 0, TL5.ID, 1);
  Schematic.appendWire(NSP2.ID, 0, TL7.ID, 1);
  Schematic.appendWire(NSP3.ID, 0, TL7.ID, 0);
  Schematic.appendWire(NSP3.ID, 0, TL6.ID, 1);
  Schematic.appendWire(NSP1.ID, 0, TL3.ID, 1);
  Schematic.appendWire(NIso.ID, 1, TL2.ID, 0);
  Schematic.appendWire(NIso.ID, 1, TL3.ID, 0);
  Schematic.appendWire(Riso.ID, 0, Ground.ID, 0);

  // Connect transmission lines to the central nodes
  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL5.ID, 0, N1.ID, 0);
  Schematic.appendWire(TL4.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL2.ID, 1, N2.ID, 0);
  Schematic.appendWire(TL4.ID, 0, N2.ID, 0);
  Schematic.appendWire(TL6.ID, 0, N2.ID, 0);
}
