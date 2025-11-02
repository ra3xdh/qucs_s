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

#include "TJunction.h"

TJunction::TJunction() {}

TJunction::TJunction(PowerCombinerParams PS) { Specification = PS; }

TJunction::~TJunction() {}

void TJunction::synthesize() {
  double K = Specification.OutputRatio.at(0) * Specification.OutputRatio.at(0);
  double lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildTJunction_IdealTL(lambda4, K);
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildTJunction_Microstrip(lambda4, K);
  }
}

void TJunction::buildTJunction_IdealTL(double lambda4, double K) {

  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(QString("T1"), Term, 0, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // TL in front of the input port
  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, 90, TL_pos[0]);
  TL1.val["Z0"] = num2str(Specification.Z0, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);

  NodeInfo N1(QString("N1"), N_pos[0]);
  Schematic.appendNode(N1);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 1);

  NodeInfo N2(QString("N2"), N_pos[1]);
  N2.visible = false;
  Schematic.appendNode(N2);

  NodeInfo N3(QString("N3"), N_pos[2]);
  N3.visible = false;
  Schematic.appendNode(N3);

  // Upper branch
  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, 90, TL_pos[1]);
  TL2.val["Z0"] = num2str(Specification.Z0 * (K + 1), Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(N1.ID, 0, N2.ID, 0);
  Schematic.appendWire(TL2.ID, 0, N2.ID, 0);

  // Lower branch
  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, 90, TL_pos[2]);
  TL3.val["Z0"] = num2str(Specification.Z0 * (K + 1) / K, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  Schematic.appendWire(N1.ID, 0, N3.ID, 0);
  Schematic.appendWire(TL3.ID, 0, N3.ID, 0);

  // Output ports
  ComponentInfo TermSpar2(QString("T2"), Term, 180, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  if (K != 1) {
    // Uneven power split
    // Upper matching line
    ComponentInfo TL4(QString("TLIN4"), TransmissionLine, 90, TL_pos[3]);
    TL4.val["Z0"] = num2str(
        sqrt(2 * Specification.Z0 * Specification.Z0 * (K + 1)), Resistance);
    TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL4);

    Schematic.appendWire(TL4.ID, 0, TL2.ID, 1);

    ComponentInfo TL5(QString("TLIN5"), TransmissionLine, 90, TL_pos[4]);
    TL5.val["Z0"] =
        num2str(sqrt(2 * Specification.Z0 * Specification.Z0 * (K + 1) / K),
                Resistance);
    TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL5);

    Schematic.appendWire(TL5.ID, 0, TL3.ID, 1);

    // Route the output ports
    Schematic.appendWire(TermSpar2.ID, 0, TL4.ID, 1);
    Schematic.appendWire(TermSpar3.ID, 0, TL5.ID, 1);
  } else {
    // Equal power split
    Schematic.appendWire(TermSpar2.ID, 0, TL2.ID, 1);
    Schematic.appendWire(TermSpar3.ID, 0, TL3.ID, 1);
  }
}

void TJunction::buildTJunction_Microstrip(double lambda4, double K) {
  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(QString("T1"), Term, 0, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Input line at Z0
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specification.MS_Subs;
  MSL_Input.synthesizeMicrostrip(Specification.Z0, lambda4 * 1e3,
                                 Specification.freq);

  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, 90, TL_pos[0]);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  Schematic.appendWire(TermSpar1.ID, 0, MLIN1.ID, 0);

  NodeInfo N1(QString("N1"), N_pos[0]);
  Schematic.appendNode(N1);
  Schematic.appendWire(MLIN1.ID, 1, N1.ID, 1);

  NodeInfo N2(QString("N2"), N_pos[1]);
  N2.visible = false;
  Schematic.appendNode(N2);

  NodeInfo N3(QString("N3"), N_pos[2]);
  N3.visible = false;
  Schematic.appendNode(N3);

  // Upper branch at Z0*(K+1)
  MicrostripClass MSL_Upper;
  MSL_Upper.Substrate = Specification.MS_Subs;
  MSL_Upper.synthesizeMicrostrip(Specification.Z0 * (K + 1), lambda4 * 1e3,
                                 Specification.freq);

  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, 90, TL_pos[1]);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Upper.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Upper.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  Schematic.appendWire(N1.ID, 0, N2.ID, 0);
  Schematic.appendWire(MLIN2.ID, 0, N2.ID, 0);

  // Lower branch at Z0*(K+1)/K
  MicrostripClass MSL_Lower;
  MSL_Lower.Substrate = Specification.MS_Subs;
  MSL_Lower.synthesizeMicrostrip(Specification.Z0 * (K + 1) / K, lambda4 * 1e3,
                                 Specification.freq);

  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, 90, TL_pos[2]);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Lower.Results.width);
  MLIN3.val["Length"] =
      ConvertLengthFromM("mm", MSL_Lower.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  Schematic.appendWire(N1.ID, 0, N3.ID, 0);
  Schematic.appendWire(MLIN3.ID, 0, N3.ID, 0);

  // Output ports
  ComponentInfo TermSpar2(QString("T2"), Term, 180, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  if (K != 1) {
    // Uneven power split
    // Upper matching line
    MicrostripClass MSL_Match_Upper;
    MSL_Match_Upper.Substrate = Specification.MS_Subs;
    MSL_Match_Upper.synthesizeMicrostrip(
        sqrt(2 * Specification.Z0 * Specification.Z0 * (K + 1)), lambda4 * 1e3,
        Specification.freq);

    ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, 90, TL_pos[3]);
    MLIN4.val["Width"] =
        ConvertLengthFromM("mm", MSL_Match_Upper.Results.width);
    MLIN4.val["Length"] =
        ConvertLengthFromM("mm", MSL_Match_Upper.Results.length * 1e-3);
    MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
    MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
    MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MLIN4);

    Schematic.appendWire(MLIN4.ID, 0, MLIN2.ID, 1);

    // Lower matching line
    MicrostripClass MSL_Match_Lower;
    MSL_Match_Lower.Substrate = Specification.MS_Subs;
    MSL_Match_Lower.synthesizeMicrostrip(
        sqrt(2 * Specification.Z0 * Specification.Z0 * (K + 1) / K),
        lambda4 * 1e3, Specification.freq);

    ComponentInfo MLIN5(QString("MLIN5"), MicrostripLine, 90, TL_pos[4]);
    MLIN5.val["Width"] =
        ConvertLengthFromM("mm", MSL_Match_Lower.Results.width);
    MLIN5.val["Length"] =
        ConvertLengthFromM("mm", MSL_Match_Lower.Results.length * 1e-3);
    MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
    MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
    MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MLIN5);

    Schematic.appendWire(MLIN5.ID, 0, MLIN3.ID, 1);

    // Route the output ports
    Schematic.appendWire(TermSpar2.ID, 0, MLIN4.ID, 1);
    Schematic.appendWire(TermSpar3.ID, 0, MLIN5.ID, 1);

  } else {
    // Equal power split
    Schematic.appendWire(TermSpar2.ID, 0, MLIN2.ID, 1);
    Schematic.appendWire(TermSpar3.ID, 0, MLIN3.ID, 1);
  }
}

// Since the components' location is shared between TLIN and MLIN
// implementations, it makes sense to have a common function to set them up
void TJunction::setComponentsLocation() {
  // Define components' location

  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Input port
  QPoint Port_in = QPoint(0, 0);
  Ports_pos.push_back(Port_in); // [0]

  // TL1: in front of the input port
  QPoint TL1 = QPoint(Port_in.x() + x_spacing, Port_in.y());
  TL_pos.push_back(TL1); // [0]

  // N1: Node in front of the first line
  QPoint N1 = QPoint(TL1.x() + x_spacing / 2, Port_in.y());
  N_pos.push_back(N1); // [0]

  // N2: Virtual node in front of the upper branch
  QPoint N2 = QPoint(N1.x(), N1.y() - y_spacing);
  N_pos.push_back(N2); // [1]

  // TL2: Upper branch
  QPoint TL2 = QPoint(N1.x() + x_spacing / 2, TL1.y() - y_spacing);
  TL_pos.push_back(TL2); // [1]

  // N3: Virtual node in front of the upper branch
  QPoint N3 = QPoint(N1.x(), N1.y() + y_spacing);
  N_pos.push_back(N3); // [2]

  // TL3: Lower branch
  QPoint TL3 = QPoint(TL2.x(), TL1.y() + y_spacing);
  TL_pos.push_back(TL3); // [2]

  if (Specification.OutputRatio.at(0) != 1) {
    // Uneven output ratio
    // TL4: Upper branch, uneven power split ratio
    QPoint TL4 = QPoint(TL2.x() + x_spacing, TL2.y());
    TL_pos.push_back(TL4); // [3]

    // TL5: Lower branch, uneven power split ratio
    QPoint TL5 = QPoint(TL3.x() + x_spacing, TL3.y());
    TL_pos.push_back(TL5); // [4]

    // Upper output term
    QPoint T2 = QPoint(TL4.x() + x_spacing, TL4.y());
    Ports_pos.push_back(T2); // [1]

    // Lower output term
    QPoint T3 = QPoint(TL5.x() + x_spacing, TL5.y());
    Ports_pos.push_back(T3); // [2]

  } else {
    // Equal output ratio
    // Upper output term
    QPoint T2 = QPoint(TL2.x() + x_spacing, TL2.y());
    Ports_pos.push_back(T2); // [1]

    // Lower output term
    QPoint T3 = QPoint(TL3.x() + x_spacing, TL3.y());
    Ports_pos.push_back(T3); // [2]
  }
}
