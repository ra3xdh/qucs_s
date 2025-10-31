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

#include "DoubleBoxBranchline.h"

DoubleBoxBranchline::DoubleBoxBranchline() {}

DoubleBoxBranchline::DoubleBoxBranchline(PowerCombinerParams PS) {
  Specification = PS;
}

DoubleBoxBranchline::~DoubleBoxBranchline() {}

void DoubleBoxBranchline::calculateParams() {
  double K = Specification.OutputRatio.at(0);
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
  double r = 1;
  double t = sqrt((1 + K) * r);
  ZA = Specification.Z0 * sqrt(r * (t * t - r)) / (t - r);
  ZD = Specification.Z0 * sqrt(r * (t * t - r)) / (t - 1);
  ZB = Specification.Z0 * sqrt(r - (r * r) / (t * t));
}

void DoubleBoxBranchline::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildDoubleBoxBranchline_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildDoubleBoxBranchline_Microstrip();
  }
}

void DoubleBoxBranchline::buildDoubleBoxBranchline_IdealTL() {

  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, x_P1,
      y_P1);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Node next to P1
  NodeInfo NSP1(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_1st_vert_TL, y_P1);
  Schematic.appendNode(NSP1);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, x_P2,
      y_P2);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Node next to P2
  NodeInfo NSP2(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_3rd_vert_TL, y_P2);
  Schematic.appendNode(NSP2);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, x_P3,
      y_P3);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Node next to P3
  NodeInfo NSP3(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_3rd_vert_TL, y_P3);
  Schematic.appendNode(NSP3);

  ComponentInfo Riso(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                     Resistor, 0, x_Riso, y_Riso);
  Riso.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Riso);

  // Node next Riso
  NodeInfo NIso(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), x_Riso,
      y_1st_bottom_TL);
  Schematic.appendNode(NIso);

  ComponentInfo Ground(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, x_Riso, y_Riso + 50);
  Schematic.appendComponent(Ground);

  // 1st top horizontal
  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, x_1st_top_TL, y_1st_top_TL);
  TL1.val["Z0"] = num2str(ZB, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  // Node above the 2nd vertical line
  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              x_2nd_vert_TL, y_2nd_top_TL);
  Schematic.appendNode(N1);

  // 1st bottom horizontal line
  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, x_1st_bottom_TL, y_1st_bottom_TL);
  TL2.val["Z0"] = num2str(ZB, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  // Node below the 2nd vertical line
  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              x_2nd_vert_TL, y_2nd_bottom_TL);
  Schematic.appendNode(N2);

  // 1st vertical TL
  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, x_1st_vert_TL, y_1st_vert_TL);
  TL3.val["Z0"] = num2str(ZA, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  // 2nd vertical line
  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, x_2nd_vert_TL, y_2nd_vert_TL);
  TL4.val["Z0"] = num2str(ZB, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);

  // 2nd top horizontal line
  ComponentInfo TL5(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, x_2nd_top_TL, y_2nd_top_TL);
  TL5.val["Z0"] = num2str(ZB, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL5);

  // 2nd bottom horizontal line
  ComponentInfo TL6(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, x_2nd_bottom_TL, y_2nd_bottom_TL);
  TL6.val["Z0"] = num2str(ZB, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL6);

  // 3rd vertical line
  ComponentInfo TL7(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, x_3rd_vert_TL, y_3rd_vert_TL);
  TL7.val["Z0"] = num2str(ZD, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
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

  Schematic.appendWire(TL1.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL5.ID, 0, N1.ID, 0);
  Schematic.appendWire(TL4.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL2.ID, 1, N2.ID, 0);
  Schematic.appendWire(TL4.ID, 0, N2.ID, 0);
  Schematic.appendWire(TL6.ID, 0, N2.ID, 0);
}

void DoubleBoxBranchline::buildDoubleBoxBranchline_Microstrip() {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20,
      -50);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo NSP1(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0,
      -50);
  Schematic.appendNode(NSP1);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 220,
      -50);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  NodeInfo NSP2(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
      -50);
  Schematic.appendNode(NSP2);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 220,
      50);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  NodeInfo NSP3(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 200,
      50);
  Schematic.appendNode(NSP3);

  ComponentInfo Riso(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                     Resistor, 0, 0, 75);
  Riso.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Riso);

  NodeInfo NIso(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0, 50);
  Schematic.appendNode(NIso);

  ComponentInfo Ground(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, 0, 120);
  Schematic.appendComponent(Ground);

  // Synthesize microstrip lines for each impedance
  // ZB impedance lines (most of the structure)
  MicrostripClass MSL_ZB;
  MSL_ZB.Substrate = Specification.MS_Subs;
  MSL_ZB.synthesizeMicrostrip(ZB, lambda4 * 1e3, Specification.freq);

  // ZA impedance line
  MicrostripClass MSL_ZA;
  MSL_ZA.Substrate = Specification.MS_Subs;
  MSL_ZA.synthesizeMicrostrip(ZA, lambda4 * 1e3, Specification.freq);

  // ZD impedance line
  MicrostripClass MSL_ZD;
  MSL_ZD.Substrate = Specification.MS_Subs;
  MSL_ZD.synthesizeMicrostrip(ZD, lambda4 * 1e3, Specification.freq);

  // Left box - Top vertical line (ZB)
  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, -50);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, -50);
  Schematic.appendNode(N1);

  // Left box - Bottom vertical line (ZB)
  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 50, 50);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              100, 50);
  Schematic.appendNode(N2);

  // Left box - Left horizontal line (ZA)
  ComponentInfo MLIN3(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 0, 0);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_ZA.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_ZA.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  // Center horizontal line (ZB)
  ComponentInfo MLIN4(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 100, 0);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  // Right box - Top vertical line (ZB)
  ComponentInfo MLIN5(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, -50);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN5.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);

  // Right box - Bottom vertical line (ZB)
  ComponentInfo MLIN6(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 150, 50);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN6);

  // Right box - Right horizontal line (ZD)
  ComponentInfo MLIN7(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 200, 0);
  MLIN7.val["Width"] = ConvertLengthFromM("mm", MSL_ZD.Results.width);
  MLIN7.val["Length"] = ConvertLengthFromM("mm", MSL_ZD.Results.length * 1e-3);
  MLIN7.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN7.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN7.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN7.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN7.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN7);

  // Wire connections
  Schematic.appendWire(TermSpar1.ID, 0, NSP1.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, NSP2.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, NSP3.ID, 0);
  Schematic.appendWire(Riso.ID, 1, NIso.ID, 0);

  Schematic.appendWire(NSP1.ID, 0, MLIN1.ID, 0);
  Schematic.appendWire(NSP2.ID, 0, MLIN5.ID, 1);
  Schematic.appendWire(NSP2.ID, 0, MLIN7.ID, 1);
  Schematic.appendWire(NSP3.ID, 0, MLIN7.ID, 0);
  Schematic.appendWire(NSP3.ID, 0, MLIN6.ID, 1);
  Schematic.appendWire(NSP1.ID, 0, MLIN3.ID, 1);
  Schematic.appendWire(NIso.ID, 1, MLIN2.ID, 0);
  Schematic.appendWire(NIso.ID, 1, MLIN3.ID, 0);
  Schematic.appendWire(Riso.ID, 0, Ground.ID, 0);

  Schematic.appendWire(MLIN1.ID, 1, N1.ID, 0);
  Schematic.appendWire(MLIN5.ID, 0, N1.ID, 0);
  Schematic.appendWire(MLIN4.ID, 1, N1.ID, 0);
  Schematic.appendWire(MLIN2.ID, 1, N2.ID, 0);
  Schematic.appendWire(MLIN4.ID, 0, N2.ID, 0);
  Schematic.appendWire(MLIN6.ID, 0, N2.ID, 0);
}

// Since the components' location is shared between TLIN and MLIN
// implementations, it makes sense to have a common function to set them up
void DoubleBoxBranchline::setComponentsLocation() {
  // Define components' location

  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Input port
  x_P1 = 0;
  y_P1 = 0;

  // 1st vertical TL (TLIN3)
  x_1st_vert_TL = x_P1 + x_spacing;
  y_1st_vert_TL = y_P1 + y_spacing;

  // 1st top horizontal line (TLIN1)
  x_1st_top_TL = x_1st_vert_TL + x_spacing;
  y_1st_top_TL = y_P1;

  // 1st bottom horizontal line (TLIN2)
  x_1st_bottom_TL = x_1st_top_TL;
  y_1st_bottom_TL = y_1st_vert_TL + y_spacing;

  // Isolation resistor
  x_Riso = x_1st_vert_TL;
  y_Riso = y_1st_bottom_TL + y_spacing;

  // 2nd vertical line (TLIN4)
  x_2nd_vert_TL = x_1st_bottom_TL + x_spacing;
  y_2nd_vert_TL = y_1st_vert_TL;

  // 2nd top horizontal line (TLIN5)
  x_2nd_top_TL = x_2nd_vert_TL + x_spacing;
  y_2nd_top_TL = y_P1;

  // 2nd bottom horizontal line (TLIN6)
  x_2nd_bottom_TL = x_2nd_top_TL;
  y_2nd_bottom_TL = y_2nd_vert_TL + y_spacing;

  // 3rd vertical line (TLIN7)
  x_3rd_vert_TL = x_2nd_bottom_TL + x_spacing;
  y_3rd_vert_TL = y_1st_vert_TL;

  // Top output port
  x_P2 = x_3rd_vert_TL + x_spacing;
  y_P2 = y_P1;

  // Bottom output port
  x_P3 = x_P2;
  y_P3 = y_1st_bottom_TL;
}
