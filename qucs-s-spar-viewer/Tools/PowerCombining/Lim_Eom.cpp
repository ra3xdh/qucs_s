/// @file Lim_Eom.cpp
/// @brief Lim-Eom power combiner/divider network (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "Lim_Eom.h"

void Lim_Eom::calculateParams() {
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);

  double M = Specification.OutputRatio.at(0);
  double N = Specification.OutputRatio.at(1);
  double K = Specification.OutputRatio.at(2);

  double d1 = M + N + K;
  double d2 = N + K;
  Z1 = Specification.Z0 * sqrt(d1 / d2);
  Z2 = Specification.Z0 * sqrt(d1 / M);
  Z3 = Specification.Z0;
  Z4 = Specification.Z0 * sqrt(d2 / N);
  Z5 = Specification.Z0 * sqrt(d2 / K);
}

void Lim_Eom::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildLimEom_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildLimEom_Microstrip();
  }
}

void Lim_Eom::buildLimEom_IdealTL() {
  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(QString("T1"), Term, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo N1(QString("N1"), N_pos[0]);
  Schematic.appendNode(N1);

  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, 90, TL_pos[0]);
  TL1.val["Z0"] = num2str(Z2, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  NodeInfo N2(QString("N2"), N_pos[1]);
  Schematic.appendNode(N2);

  ComponentInfo TermSpar2(QString("T2"), Term, 90, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, 90, TL_pos[1]);
  TL2.val["Z0"] = num2str(Z1, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  NodeInfo N3(QString("N3"), N_pos[2]);
  Schematic.appendNode(N3);

  ComponentInfo R1(QString("R1"), Resistor, Riso_pos[0]);
  R1.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(R1);

  ComponentInfo Ground1(QString("GND1"), GND, 0, GND_Riso_pos[0]);
  Schematic.appendComponent(Ground1);

  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, TL_pos[2]);
  TL3.val["Z0"] = num2str(Z2, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  NodeInfo N4(QString("N4"), N_pos[3]);
  Schematic.appendNode(N4);

  ComponentInfo TL4(QString("TLIN4"), TransmissionLine, TL_pos[3]);
  TL4.val["Z0"] = num2str(Z4, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);

  NodeInfo N5(QString("N5"), N_pos[4]);
  Schematic.appendNode(N5);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  ComponentInfo TL5(QString("TLIN5"), TransmissionLine, 90, TL_pos[4]);
  TL5.val["Z0"] = num2str(Z3, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specification.units, 2 * lambda4);
  Schematic.appendComponent(TL5);

  NodeInfo N6(QString("N6"), N_pos[5]);
  Schematic.appendNode(N6);

  ComponentInfo R2(QString("R2"), Resistor, Riso_pos[1]);
  R2.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(R2);

  ComponentInfo Ground2(QString("GND2"), GND, GND_Riso_pos[1]);
  Schematic.appendComponent(Ground2);

  ComponentInfo TL6(QString("TLIN6"), TransmissionLine, 90, TL_pos[5]);
  TL6.val["Z0"] = num2str(Z5, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL6);

  NodeInfo N7(QString("N7"), N_pos[6]);
  Schematic.appendNode(N7);

  ComponentInfo TermSpar4(QString("T4"), Term, Ports_pos[3]);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  ComponentInfo TL7(QString("TLIN7"), TransmissionLine, 90, TL_pos[6]);
  TL7.val["Z0"] = num2str(Z4, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL7);

  NodeInfo N8(QString("N8"), N_pos[7]);
  Schematic.appendNode(N8);

  ComponentInfo TL8(QString("TLIN8"), TransmissionLine, TL_pos[7]);
  TL8.val["Z0"] = num2str(Z5, Resistance);
  TL8.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL8);

  ComponentInfo TL9(QString("TLIN9"), TransmissionLine, TL_pos[8]);
  TL9.val["Z0"] = num2str(Z1, Resistance);
  TL9.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL9);

  // Wires
  // Connections to N1
  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 0);
  Schematic.appendWire(TL9.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL1.ID, 0, N1.ID, 0);

  // Connections to N2
  Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);
  Schematic.appendWire(TL1.ID, 1, N2.ID, 0);
  Schematic.appendWire(TL2.ID, 0, N2.ID, 0);

  // Connections to N3
  Schematic.appendWire(TL2.ID, 1, N3.ID, 0);
  Schematic.appendWire(R1.ID, 1, N3.ID, 0);
  Schematic.appendWire(TL3.ID, 1, N3.ID, 0);

  // Connections to N4
  Schematic.appendWire(TL3.ID, 0, N4.ID, 0);
  Schematic.appendWire(TL5.ID, 1, N4.ID, 0);
  Schematic.appendWire(TL4.ID, 1, N4.ID, 0);

  // Connections to N5
  Schematic.appendWire(TL4.ID, 0, N5.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N5.ID, 0);
  Schematic.appendWire(TL6.ID, 1, N5.ID, 0);

  // Connections to N6
  Schematic.appendWire(TL6.ID, 0, N6.ID, 0);
  Schematic.appendWire(R2.ID, 1, N6.ID, 0);
  Schematic.appendWire(TL7.ID, 1, N6.ID, 0);

  // Connections to N7
  Schematic.appendWire(TL7.ID, 0, N7.ID, 0);
  Schematic.appendWire(TL8.ID, 0, N7.ID, 0);
  Schematic.appendWire(TermSpar4.ID, 0, N7.ID, 0);

  // Connections to N8
  Schematic.appendWire(TL9.ID, 0, N8.ID, 0);
  Schematic.appendWire(TL5.ID, 0, N8.ID, 0);
  Schematic.appendWire(TL8.ID, 1, N8.ID, 0);

  // Ground <-> R1
  Schematic.appendWire(R1.ID, 0, Ground1.ID, 0);

  // Ground <-> R2
  Schematic.appendWire(R2.ID, 0, Ground2.ID, 0);
}

void Lim_Eom::buildLimEom_Microstrip() {
  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(QString("T1"), Term, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo N1(QString("N1"), N_pos[0]);
  Schematic.appendNode(N1);

  // Synthesize microstrip lines for different impedances
  // Z2 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z2;
  MSL_Z2.Substrate = Specification.MS_Subs;
  MSL_Z2.synthesizeMicrostrip(Z2, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, 90, TL_pos[0]);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  NodeInfo N2(QString("N2"), N_pos[1]);
  Schematic.appendNode(N2);

  ComponentInfo TermSpar2(QString("T2"), Term, 90, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Z1 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z1;
  MSL_Z1.Substrate = Specification.MS_Subs;
  MSL_Z1.synthesizeMicrostrip(Z1, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, 90, TL_pos[1]);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  NodeInfo N3(QString("N3"), N_pos[2]);
  Schematic.appendNode(N3);

  ComponentInfo R1(QString("R1"), Resistor, Riso_pos[0]);
  R1.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(R1);

  ComponentInfo Ground1(QString("GND1"), GND, 0, GND_Riso_pos[0]);
  Schematic.appendComponent(Ground1);

  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, TL_pos[2]);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  NodeInfo N4(QString("N4"), N_pos[3]);
  Schematic.appendNode(N4);

  // Z4 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z4;
  MSL_Z4.Substrate = Specification.MS_Subs;
  MSL_Z4.synthesizeMicrostrip(Z4, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, TL_pos[3]);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Z4.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_Z4.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  NodeInfo N5(QString("N5"), N_pos[4]);
  Schematic.appendNode(N5);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Z5 impedance lines (2 quarter-wave lines)
  // Half-wave line at Z0
  MicrostripClass MSL_Z0_Half;
  MSL_Z0_Half.Substrate = Specification.MS_Subs;
  MSL_Z0_Half.synthesizeMicrostrip(Specification.Z0, 2 * lambda4 * 1e3,
                                   Specification.freq);

  ComponentInfo MLIN5(QString("MLIN5"), MicrostripLine, 90, TL_pos[4]);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_Z0_Half.Results.width);
  MLIN5.val["Length"] =
      ConvertLengthFromM("mm", MSL_Z0_Half.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);

  NodeInfo N6(QString("N6"), N_pos[5]);
  Schematic.appendNode(N6);

  ComponentInfo R2(QString("R2"), Resistor, Riso_pos[1]);
  R2.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(R2);

  ComponentInfo Ground2(QString("GND2"), GND, GND_Riso_pos[1]);
  Schematic.appendComponent(Ground2);

  // Z4 impedance lines (2 quarter-wave lines)
  MicrostripClass MSL_Z5;
  MSL_Z5.Substrate = Specification.MS_Subs;
  MSL_Z5.synthesizeMicrostrip(Z5, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN6(QString("MLIN6"), MicrostripLine, 90, TL_pos[5]);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_Z5.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_Z5.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN6);

  NodeInfo N7(QString("N7"), N_pos[6]);
  Schematic.appendNode(N7);

  ComponentInfo TermSpar4(QString("T4"), Term, Ports_pos[3]);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  ComponentInfo MLIN7(QString("MLIN7"), MicrostripLine, 90, TL_pos[6]);
  MLIN7.val["Width"] = ConvertLengthFromM("mm", MSL_Z4.Results.width);
  MLIN7.val["Length"] = ConvertLengthFromM("mm", MSL_Z4.Results.length * 1e-3);
  MLIN7.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN7.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN7.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN7.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN7.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN7);

  NodeInfo N8(QString("N8"), N_pos[7]);
  Schematic.appendNode(N8);

  ComponentInfo MLIN8(QString("MLIN8"), MicrostripLine, TL_pos[7]);
  MLIN8.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN8.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN8.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN8.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN8.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN8.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN8.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN8);

  ComponentInfo MLIN9(QString("MLIN9"), MicrostripLine, TL_pos[8]);
  MLIN9.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN9.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN9.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN9.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN9.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN9.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN9.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN9);

  // Wires
  // Connections to N1
  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 0);
  Schematic.appendWire(MLIN9.ID, 1, N1.ID, 0);
  Schematic.appendWire(MLIN1.ID, 0, N1.ID, 0);

  // Connections to N2
  Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);
  Schematic.appendWire(MLIN1.ID, 1, N2.ID, 0);
  Schematic.appendWire(MLIN2.ID, 0, N2.ID, 0);

  // Connections to N3
  Schematic.appendWire(MLIN2.ID, 1, N3.ID, 0);
  Schematic.appendWire(R1.ID, 1, N3.ID, 0);
  Schematic.appendWire(MLIN3.ID, 1, N3.ID, 0);

  // Connections to N4
  Schematic.appendWire(MLIN3.ID, 0, N4.ID, 0);
  Schematic.appendWire(MLIN5.ID, 1, N4.ID, 0);
  Schematic.appendWire(MLIN4.ID, 1, N4.ID, 0);

  // Connections to N5
  Schematic.appendWire(MLIN4.ID, 0, N5.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N5.ID, 0);
  Schematic.appendWire(MLIN6.ID, 1, N5.ID, 0);

  // Connections to N6
  Schematic.appendWire(MLIN6.ID, 0, N6.ID, 0);
  Schematic.appendWire(R2.ID, 1, N6.ID, 0);
  Schematic.appendWire(MLIN7.ID, 1, N6.ID, 0);

  // Connections to N7
  Schematic.appendWire(MLIN7.ID, 0, N7.ID, 0);
  Schematic.appendWire(MLIN8.ID, 0, N7.ID, 0);
  Schematic.appendWire(TermSpar4.ID, 0, N7.ID, 0);

  // Connections to N8
  Schematic.appendWire(MLIN9.ID, 0, N8.ID, 0);
  Schematic.appendWire(MLIN5.ID, 0, N8.ID, 0);
  Schematic.appendWire(MLIN8.ID, 1, N8.ID, 0);

  // Ground <-> R1
  Schematic.appendWire(R1.ID, 0, Ground1.ID, 0);

  // Ground <-> R2
  Schematic.appendWire(R2.ID, 0, Ground2.ID, 0);
}

// Since the components' location is shared between TLIN and MLIN
// implementations, it makes sense to have a common function to set them up
void Lim_Eom::setComponentsLocation() {
  // Define components' location

  //                           T2
  //                            |
  // IN -- (N1) --- [TL1]  --- (N2) --- [TL2] --- (N3) --- R1
  //        |                                       |
  //      [TL9]                                   [TL3]
  //        |                                       |
  //      (N8)      ---        [TL5]     ---       (N4)
  //        |                                       |
  //      [TL8]                                   [TL4]
  //        |                                       |
  // T4 -- (N7) --- [TL7] ---  (N6) --- [TL6] --- (N5) --- T3
  //                            |
  //                            R2

  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Input port
  QPoint Port_in = QPoint(-25, -100);
  Ports_pos.push_back(Port_in); // [0]

  // N1: Node in front of the input port
  QPoint N1 = QPoint(Port_in.x() + x_spacing, Port_in.y());
  N_pos.append(N1); // [0]

  // Horizontal line in front of the input port
  QPoint TL1 = QPoint(N1.x() + x_spacing, Port_in.y());
  TL_pos.append(TL1);

  // N2: Node after TL1 (just below port 2)
  QPoint N2 = QPoint(TL1.x() + x_spacing, Port_in.y());
  N_pos.append(N2); // [1]

  // Port 2 (top center)
  QPoint T2 = QPoint(N2.x(), Port_in.y() - y_spacing);
  Ports_pos.append(T2);

  // TL2: at the right of TL1. In the "line" of the input port
  QPoint TL2 = QPoint(N2.x() + x_spacing, Port_in.y());
  TL_pos.append(TL2);

  // N3: Node after TL2 (top-right). It connects with the isolation resistor
  QPoint N3 = QPoint(TL2.x() + x_spacing, Port_in.y());
  N_pos.append(N3);

  // Isolation resistor
  QPoint R1 = QPoint(N3.x() + x_spacing, N2.y() + y_spacing);
  Riso_pos.append(R1);

  QPoint GND_R1 = QPoint(R1.x(), R1.y() + 50);
  GND_Riso_pos.append(GND_R1);

  // TL3: Vertical line on the right-side (top)
  QPoint TL3 = QPoint(N3.x(), N3.y() + y_spacing);
  TL_pos.append(TL3);

  // N4: Node connecting the two vertical lines on the right side
  QPoint N4 = QPoint(TL3.x(), TL3.y() + y_spacing);
  N_pos.append(N4);

  // TL4: Below TL3 (right-side vertical line, bottom)
  QPoint TL4 = QPoint(N4.x(), N4.y() + y_spacing);
  TL_pos.append(TL4);

  // TL5: Horizontal center branch
  QPoint TL5 = QPoint(N2.x(), N4.y());
  TL_pos.append(TL5);

  // N5: Bottom-right node. it connects term 3
  QPoint N5 = QPoint(N4.x(), TL4.y() + y_spacing);
  N_pos.append(N5);

  // Term 3
  QPoint T3 = QPoint(N5.x() + x_spacing, N5.y());
  Ports_pos.append(T3);

  // TL6: Between term 3 and Riso 2
  QPoint TL6 = QPoint(TL2.x(), N5.y());
  TL_pos.append(TL6);

  // N6: Node connecting the second isolation resistor
  QPoint N6 = QPoint(N2.x(), N5.y());
  N_pos.append(N6);

  // R2: Second isolation resistor
  QPoint R2 = QPoint(N6.x(), N6.y() + y_spacing);
  Riso_pos.append(R2);
  QPoint R2_GND = QPoint(R2.x(), R2.y() + 50);
  GND_Riso_pos.append(R2_GND);

  // TL7: Bottom left horizontal line
  QPoint TL7 = QPoint(TL1.x(), N6.y());
  TL_pos.append(TL7);

  // N7: Bottom-left corner
  QPoint N7 = QPoint(N1.x(), N6.y());
  N_pos.append(N7);

  // Term 4
  QPoint T4 = QPoint(Port_in.x(), N7.y());
  Ports_pos.append(T4);

  // TL8: Bottom-left vertical line
  QPoint TL8 = QPoint(N7.x(), TL4.y());
  TL_pos.append(TL8);

  // N8: Node in the middle of the first vertical lines
  QPoint N8 = QPoint(N1.x(), TL5.y());
  N_pos.append(N8);

  // TL9: First vertical line after the input port
  QPoint TL9 = QPoint(N1.x(), TL3.y());
  TL_pos.append(TL9);
}
