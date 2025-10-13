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

void PowerCombinerDesigner::TJunction() {
  double K = Specs.OutputRatio.at(0) * Specs.OutputRatio.at(0);
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildTJunction_IdealTL(lambda4, K);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildTJunction_Microstrip(lambda4, K);
  }
}

void PowerCombinerDesigner::buildTJunction_IdealTL(double lambda4, double K) {
  int index_t2 = 2;
  ComponentInfo TL4, TL5;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 50, 0);
  TL1.val["Z0"] = num2str(Specs.Z0, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 0);
  Schematic.appendNode(N1);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 1);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 125, -50);
  TL2.val["Z0"] = num2str(Specs.Z0 * (K + 1), Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL2.ID, 0, N1.ID, 1);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 125, 50);
  TL3.val["Z0"] = num2str(Specs.Z0 * (K + 1) / K, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);

  Schematic.appendWire(TL3.ID, 0, N1.ID, 1);

  int xpos_term = 175;

  if (K != 1) {
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

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
      xpos_term, -50);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  QString dst;
  (index_t2 == 5) ? dst = TL4.ID : dst = TL2.ID;
  Schematic.appendWire(TermSpar2.ID, 0, dst, 1);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
      xpos_term, 50);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  (index_t2 == 5) ? dst = TL5.ID : dst = TL3.ID;
  Schematic.appendWire(TermSpar3.ID, 0, dst, 1);
}

void PowerCombinerDesigner::buildTJunction_Microstrip(double lambda4,
                                                      double K) {
  int index_t2 = 2;
  ComponentInfo MLIN4, MLIN5;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Input line at Z0
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specs.MS_Subs;
  MSL_Input.synthesizeMicrostrip(Specs.Z0, lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, 0);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  Schematic.appendWire(TermSpar1.ID, 0, MLIN1.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 0);
  Schematic.appendNode(N1);
  Schematic.appendWire(MLIN1.ID, 1, N1.ID, 1);

  // Upper branch at Z0*(K+1)
  MicrostripClass MSL_Upper;
  MSL_Upper.Substrate = Specs.MS_Subs;
  MSL_Upper.synthesizeMicrostrip(Specs.Z0 * (K + 1), lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 125, -50);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Upper.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Upper.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  Schematic.appendWire(MLIN2.ID, 0, N1.ID, 1);

  // Lower branch at Z0*(K+1)/K
  MicrostripClass MSL_Lower;
  MSL_Lower.Substrate = Specs.MS_Subs;
  MSL_Lower.synthesizeMicrostrip(Specs.Z0 * (K + 1) / K, lambda4 * 1e3,
                                 Specs.freq);

  ComponentInfo MLIN3(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 125, 50);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Lower.Results.width);
  MLIN3.val["Length"] =
      ConvertLengthFromM("mm", MSL_Lower.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  Schematic.appendWire(MLIN3.ID, 0, N1.ID, 1);

  int xpos_term = 175;

  if (K != 1) {
    // Upper matching line
    MicrostripClass MSL_Match_Upper;
    MSL_Match_Upper.Substrate = Specs.MS_Subs;
    MSL_Match_Upper.synthesizeMicrostrip(
        sqrt(2 * Specs.Z0 * Specs.Z0 * (K + 1)), lambda4 * 1e3, Specs.freq);

    MLIN4.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, 200, -50);
    MLIN4.val["Width"] =
        ConvertLengthFromM("mm", MSL_Match_Upper.Results.width);
    MLIN4.val["Length"] =
        ConvertLengthFromM("mm", MSL_Match_Upper.Results.length * 1e-3);
    MLIN4.val["er"] = num2str(Specs.MS_Subs.er);
    MLIN4.val["h"] = num2str(Specs.MS_Subs.height);
    MLIN4.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MLIN4.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MLIN4.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MLIN4);

    Schematic.appendWire(MLIN4.ID, 0, MLIN2.ID, 1);

    // Lower matching line
    MicrostripClass MSL_Match_Lower;
    MSL_Match_Lower.Substrate = Specs.MS_Subs;
    MSL_Match_Lower.synthesizeMicrostrip(
        sqrt(2 * Specs.Z0 * Specs.Z0 * (K + 1) / K), lambda4 * 1e3, Specs.freq);

    MLIN5.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, 200, 50);
    MLIN5.val["Width"] =
        ConvertLengthFromM("mm", MSL_Match_Lower.Results.width);
    MLIN5.val["Length"] =
        ConvertLengthFromM("mm", MSL_Match_Lower.Results.length * 1e-3);
    MLIN5.val["er"] = num2str(Specs.MS_Subs.er);
    MLIN5.val["h"] = num2str(Specs.MS_Subs.height);
    MLIN5.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MLIN5.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MLIN5.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MLIN5);

    Schematic.appendWire(MLIN5.ID, 0, MLIN3.ID, 1);

    xpos_term += 100;
    index_t2 = 5;
  }

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
      xpos_term, -50);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  QString dst;
  (index_t2 == 5) ? dst = MLIN4.ID : dst = MLIN2.ID;
  Schematic.appendWire(TermSpar2.ID, 0, dst, 1);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
      xpos_term, 50);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  (index_t2 == 5) ? dst = MLIN5.ID : dst = MLIN3.ID;
  Schematic.appendWire(TermSpar3.ID, 0, dst, 1);
}
