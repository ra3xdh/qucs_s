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

void PowerCombinerDesigner::MultistageWilkinson() {
  std::deque<double> Zlines = ChebyshevTaper(2 * Specs.Z0, 0.05);

  double alpha = log(pow(0.1 * Specs.alpha, 10));
  double lambda4 = SPEED_OF_LIGHT / (4. * Specs.freq);
  std::complex<double> gamma(alpha, 2 * M_PI * Specs.freq / SPEED_OF_LIGHT);
  std::deque<double> Risol =
      calcMultistageWilkinsonIsolators(Zlines, lambda4, gamma);

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Lumped) {
    buildMultistageWilkinson_LumpedLC(Zlines, Risol);
  } else if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildMultistageWilkinson_IdealTL(Zlines, Risol, lambda4);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildMultistageWilkinson_Microstrip(Zlines, Risol, lambda4);
  }
}

void PowerCombinerDesigner::buildMultistageWilkinson_LumpedLC(
    const std::deque<double> &Zlines, const std::deque<double> &Risol) {
  int NStages = Specs.Nstages;
  std::vector<double> C(NStages, 0.0);
  std::vector<double> L(NStages, 0.0);

  double w = 2 * M_PI * Specs.freq;
  for (int i = 0; i < NStages; i++) {
    L[i] = Zlines[i] / w;
    C[i] = 1. / (L[i] * w * w);
  }

  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo Ground;
  ComponentInfo Cshunt, Lseries;
  NodeInfo NI, Nupper, Nlower, Nupper_, Nlower_;

  int posx = 0;
  int posy = 75;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;

  // Shunt capacitor
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, 0, posx, 20);
  Cshunt.val["C"] = num2str(2 * C[0], Capacitance);
  Schematic.appendComponent(Cshunt);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, posx, 60);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      posx + 25, 0);
  Schematic.appendNode(NI);

  Schematic.appendWire(Cshunt.ID, 1, NI.ID, 0);
  Schematic.appendWire(NI.ID, 1, TermSpar1.ID, 0);

  posx += 20;

  for (int i = 0; i < Specs.Nstages; i++) {
    double C_;
    if (i != Specs.Nstages - 1) {
      C_ = C[i] + C[i + 1];
    } else {
      C_ = C[i];
    }

    // Upper branch
    posx += 50;

    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, -75);
    Lseries.val["L"] = num2str(L[i], Inductance);
    Schematic.appendComponent(Lseries);

    posx += 50;

    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, posx, -50);
    Cshunt.val["C"] = num2str(C_, Capacitance);
    Schematic.appendComponent(Cshunt);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, -10);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    if (i > 0) {
      Schematic.appendWire(Lseries.ID, 1, Nupper.ID, 0);
    } else {
      Schematic.appendWire(Lseries.ID, 1, NI.ID, 0);
    }

    Nupper.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, -75);
    Schematic.appendNode(Nupper);

    Nupper_.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        -75);
    Schematic.appendNode(Nupper_);

    Schematic.appendWire(Cshunt.ID, 1, Nupper_.ID, 0);
    Schematic.appendWire(Nupper_.ID, 0, Lseries.ID, 0);
    Schematic.appendWire(Nupper_.ID, 0, Nupper.ID, 0);

    posx -= 50;

    // Lower branch
    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, 75);
    Lseries.val["L"] = num2str(L[i], Inductance);
    Schematic.appendComponent(Lseries);

    posx += 50;

    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, posx, 100);
    Schematic.appendComponent(Cshunt);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, 140);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    if (i > 0) {
      Schematic.appendWire(Lseries.ID, 1, Nlower.ID, 0);
    } else {
      Schematic.appendWire(Lseries.ID, 1, NI.ID, 0);
    }

    Nlower.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, 75);
    Schematic.appendNode(Nlower);

    Nlower_.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        75);
    Schematic.appendNode(Nlower_);

    Schematic.appendWire(Cshunt.ID, 1, Nlower_.ID, 0);
    Schematic.appendWire(Nlower_.ID, 0, Lseries.ID, 0);
    Schematic.appendWire(Nlower_.ID, 0, Nlower.ID, 0);

    posx += 50;

    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        posx, 0);
    Riso.val["R"] = num2str(Risol[i], Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, Nupper.ID, 0);
    Schematic.appendWire(Riso.ID, 0, Nlower.ID, 0);
  }

  posx += 50;

  // Output terminals
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, -posy);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Nupper.ID, 0);

  TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, posy);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, Nlower.ID, 0);
}

void PowerCombinerDesigner::buildMultistageWilkinson_IdealTL(
    const std::deque<double> &Zlines, const std::deque<double> &Risol,
    double lambda4) {
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo TL, TL_Upper, TL_Lower;
  NodeInfo NI, Nupper, Nlower;

  int posx = 0;
  int posy = 50;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;

  // Input transmission line
  TL.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, posx, 0);
  TL.val["Z0"] = num2str(Specs.Z0, Resistance);
  TL.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL);

  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      posx + 25, 0);
  Schematic.appendNode(NI);

  Schematic.appendWire(TermSpar1.ID, 0, TL.ID, 0);
  Schematic.appendWire(TL.ID, 1, NI.ID, 0);

  for (int i = 0; i < Specs.Nstages; i++) {
    posx += 50;

    // Upper branch TL
    TL_Upper.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx + 15, -50);
    TL_Upper.val["Z0"] = num2str(Zlines[i], Resistance);
    TL_Upper.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL_Upper);

    if (i > 0) {
      Schematic.appendWire(TL_Upper.ID, 0, Nupper.ID, 0);
    }

    Nupper.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, -50);
    Schematic.appendNode(Nupper);
    Schematic.appendWire(TL_Upper.ID, 1, Nupper.ID, 0);

    // Lower branch TL
    TL_Lower.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx + 15, 50);
    TL_Lower.val["Z0"] = num2str(Zlines[i], Resistance);
    TL_Lower.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL_Lower);

    if (i > 0) {
      Schematic.appendWire(TL_Lower.ID, 0, Nlower.ID, 0);
    }

    Nlower.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, 50);
    Schematic.appendNode(Nlower);
    Schematic.appendWire(TL_Lower.ID, 1, Nlower.ID, 0);

    if (i == 0) {
      Schematic.appendWire(NI.ID, 0, TL_Upper.ID, 0);
      Schematic.appendWire(NI.ID, 0, TL_Lower.ID, 0);
    }

    posx += 50;

    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        posx, 0);
    Riso.val["R"] = num2str(Risol[i], Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, Nupper.ID, 0);
    Schematic.appendWire(Riso.ID, 0, Nlower.ID, 0);
  }

  posx += 50;

  // Output terminals
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, -posy);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Nupper.ID, 0);

  TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, posy);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, Nlower.ID, 0);
}

void PowerCombinerDesigner::buildMultistageWilkinson_Microstrip(
    const std::deque<double> &Zlines, const std::deque<double> &Risol,
    double lambda4) {
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo MLIN, MLIN_Upper, MLIN_Lower;
  NodeInfo NI, Nupper, Nlower;

  int posx = 0;
  int posy = 50;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;

  // Input microstrip line
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specs.MS_Subs;
  MSL_Input.synthesizeMicrostrip(Specs.Z0, lambda4 * 1e3, Specs.freq);

  MLIN.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, posx, 0);
  MLIN.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN);

  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      posx + 25, 0);
  Schematic.appendNode(NI);

  Schematic.appendWire(TermSpar1.ID, 0, MLIN.ID, 0);
  Schematic.appendWire(MLIN.ID, 1, NI.ID, 0);

  for (int i = 0; i < Specs.Nstages; i++) {
    posx += 50;

    // Upper branch microstrip line
    MicrostripClass MSL_Upper;
    MSL_Upper.Substrate = Specs.MS_Subs;
    MSL_Upper.synthesizeMicrostrip(Zlines[i], lambda4 * 1e3, Specs.freq);

    MLIN_Upper.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx + 15, -50);
    MLIN_Upper.val["Width"] = ConvertLengthFromM("mm", MSL_Upper.Results.width);
    MLIN_Upper.val["Length"] =
        ConvertLengthFromM("mm", MSL_Upper.Results.length * 1e-3);
    MLIN_Upper.val["er"] = num2str(Specs.MS_Subs.er);
    MLIN_Upper.val["h"] = num2str(Specs.MS_Subs.height);
    MLIN_Upper.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MLIN_Upper.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MLIN_Upper.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MLIN_Upper);

    if (i > 0) {
      Schematic.appendWire(MLIN_Upper.ID, 0, Nupper.ID, 0);
    }

    Nupper.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, -50);
    Schematic.appendNode(Nupper);
    Schematic.appendWire(MLIN_Upper.ID, 1, Nupper.ID, 0);

    // Lower branch microstrip line
    MicrostripClass MSL_Lower;
    MSL_Lower.Substrate = Specs.MS_Subs;
    MSL_Lower.synthesizeMicrostrip(Zlines[i], lambda4 * 1e3, Specs.freq);

    MLIN_Lower.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx + 15, 50);
    MLIN_Lower.val["Width"] = ConvertLengthFromM("mm", MSL_Lower.Results.width);
    MLIN_Lower.val["Length"] =
        ConvertLengthFromM("mm", MSL_Lower.Results.length * 1e-3);
    MLIN_Lower.val["er"] = num2str(Specs.MS_Subs.er);
    MLIN_Lower.val["h"] = num2str(Specs.MS_Subs.height);
    MLIN_Lower.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
    MLIN_Lower.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
    MLIN_Lower.val["tand"] = num2str(Specs.MS_Subs.tand);
    Schematic.appendComponent(MLIN_Lower);

    if (i > 0) {
      Schematic.appendWire(MLIN_Lower.ID, 0, Nlower.ID, 0);
    }

    Nlower.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, 50);
    Schematic.appendNode(Nlower);
    Schematic.appendWire(MLIN_Lower.ID, 1, Nlower.ID, 0);

    if (i == 0) {
      Schematic.appendWire(NI.ID, 0, MLIN_Upper.ID, 0);
      Schematic.appendWire(NI.ID, 0, MLIN_Lower.ID, 0);
    }

    posx += 50;

    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        posx, 0);
    Riso.val["R"] = num2str(Risol[i], Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, Nupper.ID, 0);
    Schematic.appendWire(Riso.ID, 0, Nlower.ID, 0);
  }

  posx += 50;

  // Output terminals
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, -posy);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, Nupper.ID, 0);

  TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 180, posx, posy);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, Nlower.ID, 0);
}

std::deque<double> PowerCombinerDesigner::calcMultistageWilkinsonIsolators(
    std::deque<double> Zlines, double L, std::complex<double> gamma) {
  int NStages = Specs.Nstages;
  double Z_, R, Zaux = Zlines[NStages - 1];
  std::deque<double> Risol;

  for (int i = 0; i < NStages; i++) {
    Z_ = abs(Zaux * (Specs.Z0 + Zaux * tanh(gamma * L)) /
             (Zaux + Specs.Z0 * tanh(gamma * L)));
    Zaux = Zlines[i];
    R = Specs.Z0 * Z_ / (Z_ - Specs.Z0);
    Risol.push_front(2 * R);
  }
  return Risol;
}
