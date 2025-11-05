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

#include "Wilkinson3way_ImprovedIsolation.h"

Wilkinson3Way_ImprovedIsolation::Wilkinson3Way_ImprovedIsolation() {}

Wilkinson3Way_ImprovedIsolation::Wilkinson3Way_ImprovedIsolation(
    PowerCombinerParams PS) {
  Specification = PS;
}

Wilkinson3Way_ImprovedIsolation::~Wilkinson3Way_ImprovedIsolation() {}

void Wilkinson3Way_ImprovedIsolation::calculateParams() {
  Z1 = 2.28 * Specification.Z0;
  Z2 = 1.316 * Specification.Z0;
  R1 = 1.319 * Specification.Z0;
  R2 = 4 * Specification.Z0;
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
}

void Wilkinson3Way_ImprovedIsolation::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildWilkinson3Way_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildWilkinson3Way_Microstrip();
  }
}

void Wilkinson3Way_ImprovedIsolation::buildWilkinson3Way_IdealTL() {
  // Define components' location
  setComponentsLocation();

  // Input port (T1)
  ComponentInfo TermSpar1(QString("T1"), Term, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // TL1: First stage, center branch
  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, 90, TL_pos[0]);
  TL1.val["Z0"] = num2str(Z1, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  // TL2: First stage, upper branch
  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, 90, TL_pos[1]);
  TL2.val["Z0"] = num2str(Z1, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  // Ri1: Isolation resistor between N2 and N1
  ComponentInfo Ri1(QString("R1"), Resistor, Riso_pos[0]);
  Ri1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Ri1);

  // TL3: First stage, lower branch
  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, 90, TL_pos[2]);
  TL3.val["Z0"] = num2str(Z1, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  // Ri2: Isolation resistor between N1 and N3
  ComponentInfo Ri2(QString("R2"), Resistor, Riso_pos[1]);
  Ri2.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Ri2);

  // TL4: Second stage, center branch
  ComponentInfo TL4(QString("TLIN4"), TransmissionLine, 90, TL_pos[3]);
  TL4.val["Z0"] = num2str(Z2, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);

  // T2: Center output port
  ComponentInfo TermSpar2(QString("T2"), Term, 180, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // TL5: Second stage, upper branch
  ComponentInfo TL5(QString("TLIN5"), TransmissionLine, 90, TL_pos[4]);
  TL5.val["Z0"] = num2str(Z2, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL5);

  // Ri3: Isolation resistor between N5 and N4
  ComponentInfo Ri3(QString("R3"), Resistor, Riso_pos[2]);
  Ri3.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri3);

  // T3: Upper output port
  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // TL6: Second stage, lower branch
  ComponentInfo TL6(QString("TLIN6"), TransmissionLine, 90, TL_pos[5]);
  TL6.val["Z0"] = num2str(Z2, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL6);

  // Ri4: Isolation resistor between N6 and N4
  ComponentInfo Ri4(QString("R4"), Resistor, Riso_pos[3]);
  Ri4.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri4);

  // T4: Lower output port
  ComponentInfo TermSpar4(QString("T4"), Term, 180, Ports_pos[3]);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  // Nodes

  // N0: Input node
  NodeInfo N0(QString("N0"), N_pos[0]);
  Schematic.appendNode(N0);

  // N1: Top-left corner
  NodeInfo N1(QString("N1"), N_pos[1]);
  N1.visible = false; // Virtual node. It's not needed, but helps routing,
                      // especially for export options
  Schematic.appendNode(N1);

  // N2: Top-center
  NodeInfo N2(QString("N2"), N_pos[2]);
  Schematic.appendNode(N2);

  // N3: Top-right
  NodeInfo N3(QString("N3"), N_pos[3]);
  Schematic.appendNode(N3);

  // N4: Center
  NodeInfo N4(QString("N4"), N_pos[4]);
  Schematic.appendNode(N4);

  // N5: Center-right
  NodeInfo N5(QString("N5"), N_pos[5]);
  Schematic.appendNode(N5);

  // N6: Bottom-left corner
  NodeInfo N6(QString("N6"), N_pos[6]);
  N6.visible = false; // Virtual node. It's not needed, but helps routing,
                      // especially for export options
  Schematic.appendNode(N6);

  // N7: Bottom-center)
  NodeInfo N7(QString("N7"), N_pos[7]);
  Schematic.appendNode(N7);

  // N8: Bottom-left corner
  NodeInfo N8(QString("N8"), N_pos[8]);
  Schematic.appendNode(N8);

  // Wires

  // Connected to N0
  Schematic.appendWire(N0.ID, 0, TermSpar1.ID, 0);
  Schematic.appendWire(N0.ID, 0, N1.ID, 0);
  Schematic.appendWire(N0.ID, 0, TL1.ID, 0);
  Schematic.appendWire(N0.ID, 0, N6.ID, 0);

  // Connected to N1
  Schematic.appendWire(N1.ID, 0, TL2.ID, 0);

  // Connected to N2
  Schematic.appendWire(N2.ID, 0, TL2.ID, 1);
  Schematic.appendWire(N2.ID, 0, TL5.ID, 0);
  Schematic.appendWire(N2.ID, 0, Ri1.ID, 1);

  // Connected to N3
  Schematic.appendWire(N3.ID, 0, TL5.ID, 1);
  Schematic.appendWire(N3.ID, 0, TermSpar2.ID, 0);
  Schematic.appendWire(N3.ID, 0, Ri3.ID, 1);

  // Connected to N4
  Schematic.appendWire(N4.ID, 0, TL1.ID, 1);
  Schematic.appendWire(N4.ID, 0, Ri1.ID, 0);
  Schematic.appendWire(N4.ID, 0, TL4.ID, 0);
  Schematic.appendWire(N4.ID, 0, Ri2.ID, 1);

  // Connected to N5
  Schematic.appendWire(N5.ID, 0, TL4.ID, 1);
  Schematic.appendWire(N5.ID, 0, Ri3.ID, 0);
  Schematic.appendWire(N5.ID, 0, Ri4.ID, 1);
  Schematic.appendWire(N5.ID, 0, TermSpar3.ID, 0);

  // Connected to N6
  Schematic.appendWire(N6.ID, 0, TL3.ID, 0);

  // Connected to N7
  Schematic.appendWire(N7.ID, 0, TL3.ID, 1);
  Schematic.appendWire(N7.ID, 0, Ri2.ID, 0);
  Schematic.appendWire(N7.ID, 0, TL6.ID, 0);

  // Connected to N8
  Schematic.appendWire(N8.ID, 0, TL6.ID, 1);
  Schematic.appendWire(N8.ID, 0, Ri4.ID, 0);
  Schematic.appendWire(N8.ID, 0, TermSpar4.ID, 0);
}

void Wilkinson3Way_ImprovedIsolation::buildWilkinson3Way_Microstrip() {
  // Define components' location
  setComponentsLocation();

  // Input port (T1)
  ComponentInfo TermSpar1(QString("T1"), Term, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Synthesize microstrip lines for different impedances
  // Z1 impedance lines (3 first-stage quarter-wave lines)
  MicrostripClass MSL_Z1;
  MSL_Z1.Substrate = Specification.MS_Subs;
  MSL_Z1.synthesizeMicrostrip(Z1, lambda4 * 1e3, Specification.freq);

  // MLIN1: First stage, center branch
  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, 90, TL_pos[0]);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  // MLIN2: First stage, upper branch
  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, 90, TL_pos[1]);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  // Ri1: Isolation resistor between N2 and N1
  ComponentInfo Ri1(QString("R1"), Resistor, Riso_pos[0]);
  Ri1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Ri1);

  // MLIN3: First stage, lower branch
  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, 90, TL_pos[2]);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  // Ri2: Isolation resistor between N1 and N3
  ComponentInfo Ri2(QString("R2"), Resistor, Riso_pos[1]);
  Ri2.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Ri2);

  // Z2 impedance lines (3 second-stage quarter-wave lines)
  MicrostripClass MSL_Z2;
  MSL_Z2.Substrate = Specification.MS_Subs;
  MSL_Z2.synthesizeMicrostrip(Z2, lambda4 * 1e3, Specification.freq);

  // MLIN4: Second stage, center branch
  ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, 90, TL_pos[3]);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  // T2: Center output port
  ComponentInfo TermSpar2(QString("T2"), Term, 180, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // MLIN5: Second stage, upper branch
  ComponentInfo MLIN5(QString("MLIN5"), MicrostripLine, 90, TL_pos[4]);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN5.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);

  // Ri3: Isolation resistor between N5 and N4
  ComponentInfo Ri3(QString("R3"), Resistor, Riso_pos[2]);
  Ri3.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri3);

  // T3: Upper output port
  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // MLIN6: Second stage, lower branch
  ComponentInfo MLIN6(QString("MLIN6"), MicrostripLine, 90, TL_pos[5]);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN6);

  // Ri4: Isolation resistor between N6 and N4
  ComponentInfo Ri4(QString("R4"), Resistor, Riso_pos[3]);
  Ri4.val["R"] = num2str(R2, Resistance);
  Schematic.appendComponent(Ri4);

  // T4: Lower output port
  ComponentInfo TermSpar4(QString("T4"), Term, 180, Ports_pos[3]);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  // Nodes

  // N0: Input node
  NodeInfo N0(QString("N0"), N_pos[0]);
  Schematic.appendNode(N0);

  // N1: Top-left corner
  NodeInfo N1(QString("N1"), N_pos[1]);
  N1.visible = false; // Virtual node. It's not needed, but helps routing,
                      // especially for export options
  Schematic.appendNode(N1);

  // N2: Top-center
  NodeInfo N2(QString("N2"), N_pos[2]);
  Schematic.appendNode(N2);

  // N3: Top-right
  NodeInfo N3(QString("N3"), N_pos[3]);
  Schematic.appendNode(N3);

  // N4: Center
  NodeInfo N4(QString("N4"), N_pos[4]);
  Schematic.appendNode(N4);

  // N5: Center-right
  NodeInfo N5(QString("N5"), N_pos[5]);
  Schematic.appendNode(N5);

  // N6: Bottom-left corner
  NodeInfo N6(QString("N6"), N_pos[6]);
  N6.visible = false; // Virtual node. It's not needed, but helps routing,
                      // especially for export options
  Schematic.appendNode(N6);

  // N7: Bottom-center)
  NodeInfo N7(QString("N7"), N_pos[7]);
  Schematic.appendNode(N7);

  // N8: Bottom-left corner
  NodeInfo N8(QString("N8"), N_pos[8]);
  Schematic.appendNode(N8);

  // Wires

  // Connected to N0
  Schematic.appendWire(N0.ID, 0, TermSpar1.ID, 0);
  Schematic.appendWire(N0.ID, 0, N1.ID, 0);
  Schematic.appendWire(N0.ID, 0, MLIN1.ID, 0);
  Schematic.appendWire(N0.ID, 0, N6.ID, 0);

  // Connected to N1
  Schematic.appendWire(N1.ID, 0, MLIN2.ID, 0);

  // Connected to N2
  Schematic.appendWire(N2.ID, 0, MLIN2.ID, 1);
  Schematic.appendWire(N2.ID, 0, MLIN5.ID, 0);
  Schematic.appendWire(N2.ID, 0, Ri1.ID, 1);

  // Connected to N3
  Schematic.appendWire(N3.ID, 0, MLIN5.ID, 1);
  Schematic.appendWire(N3.ID, 0, TermSpar2.ID, 0);
  Schematic.appendWire(N3.ID, 0, Ri3.ID, 1);

  // Connected to N4
  Schematic.appendWire(N4.ID, 0, MLIN1.ID, 1);
  Schematic.appendWire(N4.ID, 0, Ri1.ID, 0);
  Schematic.appendWire(N4.ID, 0, MLIN4.ID, 0);
  Schematic.appendWire(N4.ID, 0, Ri2.ID, 1);

  // Connected to N5
  Schematic.appendWire(N5.ID, 0, MLIN4.ID, 1);
  Schematic.appendWire(N5.ID, 0, Ri3.ID, 0);
  Schematic.appendWire(N5.ID, 0, Ri4.ID, 1);
  Schematic.appendWire(N5.ID, 0, TermSpar3.ID, 0);

  // Connected to N6
  Schematic.appendWire(N6.ID, 0, MLIN3.ID, 0);

  // Connected to N7
  Schematic.appendWire(N7.ID, 0, MLIN3.ID, 1);
  Schematic.appendWire(N7.ID, 0, Ri2.ID, 0);
  Schematic.appendWire(N7.ID, 0, MLIN6.ID, 0);

  // Connected to N8
  Schematic.appendWire(N8.ID, 0, MLIN6.ID, 1);
  Schematic.appendWire(N8.ID, 0, Ri4.ID, 0);
  Schematic.appendWire(N8.ID, 0, TermSpar4.ID, 0);
}

void Wilkinson3Way_ImprovedIsolation::setComponentsLocation() {
  // Define components' location
  //
  // Topology:
  //
  //         (N1) --- [TL2] --- (N2) --- [TL5] --- (N3) --> T2
  //          |                  |                   |
  //          |                  R1                  R3
  //          |                  |                   |
  //   IN -> (N0) --- [TL1] --- (N4) --- [TL4] --- (N5) --> T3
  //          |                  |                   |
  //          |                  R2                  R4
  //          |                  |                   |
  //         (N6) --- [TL3] --- (N7) --- [TL6] --- (N8) --> T4

  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Vector allocation
  Ports_pos.resize(4);
  TL_pos.resize(6);
  Riso_pos.resize(4);
  N_pos.resize(9);

  // Input port
  QPoint Port_in = QPoint(0, 0);
  Ports_pos[0] = Port_in;

  // First three nodes
  QPoint N0 = QPoint(Port_in.x() + x_spacing, Port_in.y());
  N_pos[0] = N0;

  QPoint N1 = QPoint(N0.x(), N0.y() - 2 * y_spacing);
  N_pos[1] = N1;

  QPoint N6 = QPoint(N0.x(), N0.y() + 2 * y_spacing);
  N_pos[6] = N6;

  // First three lines

  QPoint TL1 = QPoint(N0.x() + x_spacing, N0.y());
  TL_pos[0] = TL1;

  QPoint TL2 = QPoint(TL1.x(), N1.y()); // Upper first
  TL_pos[1] = TL2;

  QPoint TL3 = QPoint(TL1.x(), N6.y());
  TL_pos[2] = TL3;

  // Second column of nodes
  QPoint N2 = QPoint(TL2.x() + x_spacing, TL2.y());
  N_pos[2] = N2;

  QPoint N4 = QPoint(N2.x(), TL1.y());
  N_pos[4] = N4;

  QPoint N7 = QPoint(N2.x(), TL3.y());
  N_pos[7] = N7;

  // First stage isolation resistors
  QPoint R1 = QPoint(N2.x(), N2.y() + y_spacing);
  Riso_pos[0] = R1;

  QPoint R2 = QPoint(R1.x(), N4.y() + y_spacing);
  Riso_pos[1] = R2;

  // Second stage transmission lines positions

  QPoint TL5 = QPoint(N2.x() + x_spacing, N2.y());
  TL_pos[4] = TL5;

  QPoint TL4 = QPoint(N4.x() + x_spacing, N4.y());
  TL_pos[3] = TL4;

  QPoint TL6 = QPoint(N7.x() + x_spacing, N7.y());
  TL_pos[5] = TL6;

  // Output nodes

  QPoint N3 = QPoint(TL5.x() + x_spacing, TL5.y());
  N_pos[3] = N3;

  QPoint N5 = QPoint(TL4.x() + x_spacing, TL4.y());
  N_pos[5] = N5;

  QPoint N8 = QPoint(TL6.x() + x_spacing, TL6.y());
  N_pos[8] = N8;

  // Second stage isolation resistors
  QPoint R3 = QPoint(N3.x(), N3.y() + y_spacing);
  Riso_pos[2] = R3;

  QPoint R4 = QPoint(N5.x(), N5.y() + y_spacing);
  Riso_pos[3] = R4;

  // Output ports
  QPoint T2 = QPoint(N3.x() + x_spacing, N3.y());
  Ports_pos[1] = T2;

  QPoint T3 = QPoint(N5.x() + x_spacing, N5.y());
  Ports_pos[2] = T3;

  QPoint T4 = QPoint(N8.x() + x_spacing, N8.y());
  Ports_pos[3] = T4;
}
