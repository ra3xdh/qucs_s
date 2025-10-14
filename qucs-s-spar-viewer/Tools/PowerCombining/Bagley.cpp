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

void PowerCombinerDesigner::Bagley() {
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);
  double lambda2 = lambda4 * 2;
  double Zbranch = 2 * Specs.Z0 / sqrt(Specs.Noutputs);

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildBagley_IdealTL(lambda4, lambda2, Zbranch);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildBagley_Microstrip(lambda4, lambda2, Zbranch);
  }
}

void PowerCombinerDesigner::buildBagley_IdealTL(double lambda4, double lambda2,
                                                double Zbranch) {
  NodeInfo NI;

  ComponentInfo TermSpar(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 90,
      (Specs.Noutputs - 1) * 50, -30);
  TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar);

  NodeInfo N1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]),
      (Specs.Noutputs - 1) * 50, 0);
  SchContent.appendNode(N1);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, (Specs.Noutputs - 1) * 100, 50);
  TL1.val["Z0"] = num2str(Zbranch, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL1);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 50);
  TL2.val["Z0"] = num2str(Zbranch, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  SchContent.appendComponent(TL2);

  SchContent.appendWire(TL1.ID, 1, N1.ID, 0);
  SchContent.appendWire(TL2.ID, 1, N1.ID, 0);
  SchContent.appendWire(TermSpar.ID, 0, N1.ID, 0);

  TermSpar.setParams(QString("T%1").arg(++SchContent.NumberComponents[Term]),
                     Term, -90, 0, 120);
  TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar);

  NI.setParams(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0,
      100);
  SchContent.appendNode(NI);

  SchContent.appendWire(TermSpar.ID, 0, NI.ID, 0);
  SchContent.appendWire(TL2.ID, 0, NI.ID, 0);

  ComponentInfo TL;
  int posx = -50;
  for (int i = 1; i < Specs.Noutputs; i++) {
    posx += 100;
    TL.setParams(
        QString("TLIN%1").arg(++SchContent.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx, 100);
    TL.val["Z0"] = num2str(Zbranch, Resistance);
    TL.val["Length"] = ConvertLengthFromM(Specs.units, lambda2);
    SchContent.appendComponent(TL);
    SchContent.appendWire(NI.ID, 0, TL.ID, 0);

    TermSpar.setParams(QString("T%1").arg(++SchContent.NumberComponents[Term]),
                       Term, -90, posx + 50, 120);
    TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
    SchContent.appendComponent(TermSpar);

    NI.setParams(
        QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]),
        posx + 50, 100);
    SchContent.appendNode(NI);

    SchContent.appendWire(NI.ID, 0, TL.ID, 1);
    SchContent.appendWire(NI.ID, 0, TermSpar.ID, 0);
  }

  SchContent.appendWire(TL1.ID, 0, NI.ID, 0);
}

void PowerCombinerDesigner::buildBagley_Microstrip(double lambda4,
                                                   double lambda2,
                                                   double Zbranch) {
  NodeInfo NI;

  ComponentInfo TermSpar(
      QString("T%1").arg(++SchContent.NumberComponents[Term]), Term, 90,
      (Specs.Noutputs - 1) * 50, -30);
  TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar);

  NodeInfo N1(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]),
      (Specs.Noutputs - 1) * 50, 0);
  SchContent.appendNode(N1);

  // Right quarter-wave branch microstrip line
  MicrostripClass MSL_Branch1;
  MSL_Branch1.Substrate = Specs.MS_Subs;
  MSL_Branch1.synthesizeMicrostrip(Zbranch, lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, (Specs.Noutputs - 1) * 100, 50);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Branch1.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Branch1.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN1);

  // Left quarter-wave branch microstrip line
  MicrostripClass MSL_Branch2;
  MSL_Branch2.Substrate = Specs.MS_Subs;
  MSL_Branch2.synthesizeMicrostrip(Zbranch, lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 0, 50);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Branch2.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Branch2.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specs.MS_Subs.tand);
  SchContent.appendComponent(MLIN2);

  SchContent.appendWire(MLIN1.ID, 1, N1.ID, 0);
  SchContent.appendWire(MLIN2.ID, 1, N1.ID, 0);
  SchContent.appendWire(TermSpar.ID, 0, N1.ID, 0);

  TermSpar.setParams(QString("T%1").arg(++SchContent.NumberComponents[Term]),
                     Term, -90, 0, 120);
  TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
  SchContent.appendComponent(TermSpar);

  NI.setParams(
      QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]), 0,
      100);
  SchContent.appendNode(NI);

  SchContent.appendWire(TermSpar.ID, 0, NI.ID, 0);
  SchContent.appendWire(MLIN2.ID, 0, NI.ID, 0);

  // Half-wave microstrip line sections
  MicrostripClass MSL_HalfWave;
  MSL_HalfWave.Substrate = Specs.MS_Subs;
  MSL_HalfWave.synthesizeMicrostrip(Zbranch, lambda2 * 1e3, Specs.freq);

  ComponentInfo MLIN;
  int posx = -50;
  for (int i = 1; i < Specs.Noutputs; i++) {
    posx += 100;

    MLIN.setParams(
        QString("MLIN%1").arg(++SchContent.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx, 100);
    MLIN.val["Width"] = ConvertLengthFromM("mm", MSL_HalfWave.Results.width);
    MLIN.val["Length"] =
        ConvertLengthFromM("mm", MSL_HalfWave.Results.length * 1e-3);
    MLIN.val["er"] = num2str(Specs.MS_Subs.er);
    MLIN.val["h"] = num2str(Specs.MS_Subs.height);
    MLIN.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MLIN.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MLIN.val["tand"] = num2str(Specs.MS_Subs.tand);
    SchContent.appendComponent(MLIN);

    SchContent.appendWire(NI.ID, 0, MLIN.ID, 0);

    TermSpar.setParams(QString("T%1").arg(++SchContent.NumberComponents[Term]),
                       Term, -90, posx + 50, 120);
    TermSpar.val["Z"] = num2str(Specs.Z0, Resistance);
    SchContent.appendComponent(TermSpar);

    NI.setParams(
        QString("N%1").arg(++SchContent.NumberComponents[ConnectionNodes]),
        posx + 50, 100);
    SchContent.appendNode(NI);

    SchContent.appendWire(NI.ID, 0, MLIN.ID, 1);
    SchContent.appendWire(NI.ID, 0, TermSpar.ID, 0);
  }

  SchContent.appendWire(MLIN1.ID, 0, NI.ID, 0);
}
