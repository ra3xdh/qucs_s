/// @file Bagley.cpp
/// @brief Bagley power combiner/divider network (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "Bagley.h"

void Bagley::calculateParams() {
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
  lambda2 = lambda4 * 2;
  Zbranch = 2 * Specification.Z0 / sqrt(Specification.Noutputs);
}

void Bagley::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildBagley_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildBagley_Microstrip();
  }
}

void Bagley::buildBagley_IdealTL() {

  // Define components' location
  setComponentsLocation();

  NodeInfo NI;

  // Input port (top)
  ComponentInfo TermSpar(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                         Term, 90, Port_in.x(), Port_in.y());
  TermSpar.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              Port_in.x(), Port_in.y() + 30);
  Schematic.appendNode(N1);

  // First vertical line
  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, left_TL.x(), left_TL.y());
  TL1.val["Z0"] = num2str(Zbranch, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  // Second vertical line
  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, right_TL.x(), right_TL.y());
  TL2.val["Z0"] = num2str(Zbranch, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL2.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);
  Schematic.appendWire(TermSpar.ID, 0, N1.ID, 0);

  TermSpar.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                     Term, -90, Port_1st_out.x(), Port_1st_out.y());
  TermSpar.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar);

  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      N_1st_out.x(), N_1st_out.y());
  Schematic.appendNode(NI);

  Schematic.appendWire(TermSpar.ID, 0, NI.ID, 0);
  Schematic.appendWire(TL1.ID, 0, NI.ID, 0);

  ComponentInfo TL;
  int posx = -50;
  for (int i = 1; i < Specification.Noutputs; i++) {
    posx += 100;
    TL.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx, y_out);
    TL.val["Z0"] = num2str(Zbranch, Resistance);
    TL.val["Length"] = ConvertLengthFromM(Specification.units, lambda2);
    Schematic.appendComponent(TL);
    Schematic.appendWire(NI.ID, 0, TL.ID, 0);

    TermSpar.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                       Term, -90, posx + 50, y_out + 20);
    TermSpar.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar);

    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_out);
    Schematic.appendNode(NI);

    Schematic.appendWire(NI.ID, 0, TL.ID, 1);
    Schematic.appendWire(NI.ID, 0, TermSpar.ID, 0);
  }

  Schematic.appendWire(TL2.ID, 0, NI.ID, 0);
}

void Bagley::buildBagley_Microstrip() {

  // Define components' location
  setComponentsLocation();

  NodeInfo NI;

  // Input port (top)
  ComponentInfo TermSpar(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                         Term, 90, Port_in.x(), Port_in.y());
  TermSpar.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              Port_in.x(), Port_in.y() + 30);
  Schematic.appendNode(N1);

  // Right quarter-wave branch microstrip line
  MicrostripClass MSL_Branch1;
  MSL_Branch1.Substrate = Specification.MS_Subs;
  MSL_Branch1.synthesizeMicrostrip(Zbranch, lambda4 * 1e3, Specification.freq);

  // First vertical line
  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, left_TL.x(), left_TL.y());
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Branch1.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Branch1.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  // Left quarter-wave branch microstrip line
  MicrostripClass MSL_Branch2;
  MSL_Branch2.Substrate = Specification.MS_Subs;
  MSL_Branch2.synthesizeMicrostrip(Zbranch, lambda4 * 1e3, Specification.freq);

  // Second vertical line
  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, right_TL.x(), right_TL.y());
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Branch2.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Branch2.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  Schematic.appendWire(MLIN2.ID, 1, N1.ID, 0);
  Schematic.appendWire(MLIN1.ID, 1, N1.ID, 0);
  Schematic.appendWire(TermSpar.ID, 0, N1.ID, 0);

  TermSpar.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                     Term, -90, Port_1st_out.x(), Port_1st_out.y());
  TermSpar.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar);

  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      N_1st_out.x(), N_1st_out.y());
  Schematic.appendNode(NI);

  Schematic.appendWire(TermSpar.ID, 0, NI.ID, 0);
  Schematic.appendWire(MLIN1.ID, 0, NI.ID, 0);

  // Half-wave microstrip line sections
  MicrostripClass MSL_HalfWave;
  MSL_HalfWave.Substrate = Specification.MS_Subs;
  MSL_HalfWave.synthesizeMicrostrip(Zbranch, lambda2 * 1e3, Specification.freq);

  ComponentInfo MLIN;
  int posx = -50;
  for (int i = 1; i < Specification.Noutputs; i++) {
    posx += 100;

    MLIN.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx, y_out);
    MLIN.val["Width"] = ConvertLengthFromM("mm", MSL_HalfWave.Results.width);
    MLIN.val["Length"] =
        ConvertLengthFromM("mm", MSL_HalfWave.Results.length * 1e-3);
    MLIN.val["er"] = num2str(Specification.MS_Subs.er);
    MLIN.val["h"] = num2str(Specification.MS_Subs.height);
    MLIN.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MLIN.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MLIN.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MLIN);

    Schematic.appendWire(NI.ID, 0, MLIN.ID, 0);

    TermSpar.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                       Term, -90, posx + 50, y_out + 20);
    TermSpar.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar);

    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, y_out);
    Schematic.appendNode(NI);

    Schematic.appendWire(NI.ID, 0, MLIN.ID, 1);
    Schematic.appendWire(NI.ID, 0, TermSpar.ID, 0);
  }

  Schematic.appendWire(MLIN2.ID, 0, NI.ID, 0);
}

void Bagley::setComponentsLocation() {

  // Input port
  Port_in = QPoint((Specification.Noutputs - 1) * 50, -20);

  // Left vertical TL
  left_TL.setX(0);
  left_TL.setY(Port_in.y() + 80);

  // Right vertical TL
  right_TL.setX((Specification.Noutputs - 1) * 100);
  right_TL.setY(left_TL.y());

  y_out = left_TL.y() + 50; // y-axis coordinate of the output lines

  // 1st output node
  N_1st_out.setX(left_TL.x());
  N_1st_out.setY(y_out);

  // First output port
  Port_1st_out = QPoint(0, y_out + 20);
}
