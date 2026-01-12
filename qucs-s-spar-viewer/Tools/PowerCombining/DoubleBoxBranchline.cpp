/// @file DoubleBoxBranchline.cpp
/// @brief Double-box branch-line power combiner/divider network
/// (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "DoubleBoxBranchline.h"

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

  // Input port
  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Node next to P1
  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);

  ComponentInfo TermSpar2(QString("T2"), Term, 180, Port_out1);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Node next to P2
  NodeInfo N2(QString("N2"), N2_pos);
  Schematic.appendNode(N2);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Port_out2);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Node next to P3
  NodeInfo N3(QString("N3"), N3_pos);
  Schematic.appendNode(N3);

  // Isolation resistor
  ComponentInfo Riso(QString("R1"), Resistor, Riso_pos);
  Riso.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Riso);

  ComponentInfo Ground(QString("GND1"), GND, GND_Riso_pos);
  Schematic.appendComponent(Ground);

  // Node next Riso
  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);

  // 1st top horizontal
  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, 90, TL1_pos);
  TL1.val["Z0"] = num2str(ZB, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  // Node above the 2nd vertical line
  NodeInfo N5(QString("N5"), N5_pos);
  Schematic.appendNode(N5);

  // 1st bottom horizontal line
  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, 90, TL2_pos);
  TL2.val["Z0"] = num2str(ZB, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  // Node below the 2nd vertical line
  NodeInfo N6(QString("N6"), N6_pos);
  Schematic.appendNode(N6);

  // 1st vertical TL
  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, TL3_pos);
  TL3.val["Z0"] = num2str(ZA, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  // 2nd vertical line
  ComponentInfo TL4(QString("TLIN4"), TransmissionLine, TL4_pos);
  TL4.val["Z0"] = num2str(ZB, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);

  // 2nd top horizontal line
  ComponentInfo TL5(QString("TLIN5"), TransmissionLine, 90, TL5_pos);
  TL5.val["Z0"] = num2str(ZB, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL5);

  // 2nd bottom horizontal line
  ComponentInfo TL6(QString("TLIN6"), TransmissionLine, 90, TL6_pos);
  TL6.val["Z0"] = num2str(ZB, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL6);

  // 3rd vertical line
  ComponentInfo TL7(QString("TLIN7"), TransmissionLine, TL7_pos);
  TL7.val["Z0"] = num2str(ZD, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL7);

  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N3.ID, 0);
  Schematic.appendWire(Riso.ID, 1, N4.ID, 0);

  Schematic.appendWire(N1.ID, 0, TL1.ID, 0);
  Schematic.appendWire(N2.ID, 0, TL5.ID, 1);
  Schematic.appendWire(N2.ID, 0, TL7.ID, 1);
  Schematic.appendWire(N3.ID, 0, TL7.ID, 0);
  Schematic.appendWire(N3.ID, 0, TL6.ID, 1);
  Schematic.appendWire(N1.ID, 0, TL3.ID, 1);
  Schematic.appendWire(N4.ID, 1, TL2.ID, 0);
  Schematic.appendWire(N4.ID, 1, TL3.ID, 0);
  Schematic.appendWire(Riso.ID, 0, Ground.ID, 0);

  Schematic.appendWire(TL1.ID, 1, N5.ID, 0);
  Schematic.appendWire(TL5.ID, 0, N5.ID, 0);
  Schematic.appendWire(TL4.ID, 1, N5.ID, 0);
  Schematic.appendWire(TL2.ID, 1, N6.ID, 0);
  Schematic.appendWire(TL4.ID, 0, N6.ID, 0);
  Schematic.appendWire(TL6.ID, 0, N6.ID, 0);
}

void DoubleBoxBranchline::buildDoubleBoxBranchline_Microstrip() {

  // Define components' location
  setComponentsLocation();

  // Input port
  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Node next to P1
  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);

  ComponentInfo TermSpar2(QString("T2"), Term, 180, Port_out1);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Node next to P2
  NodeInfo N2(QString("N2"), N2_pos);
  Schematic.appendNode(N2);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Port_out2);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Node next to P3
  NodeInfo N3(QString("N3"), N3_pos);
  Schematic.appendNode(N3);

  // Isolation resistor
  ComponentInfo Riso(QString("R1"), Resistor, Riso_pos);
  Riso.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Riso);

  ComponentInfo Ground(QString("GND1"), GND, GND_Riso_pos);
  Schematic.appendComponent(Ground);

  // Node next Riso
  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);

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
  // 1st top horizontal
  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, 90, TL1_pos);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  // Node above the 2nd vertical line
  NodeInfo N5(QString("N5"), N5_pos);
  Schematic.appendNode(N5);

  // Left box - Bottom vertical line (ZB)
  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, 90, TL2_pos);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  // Node below the 2nd vertical line
  NodeInfo N6(QString("N6"), N6_pos);
  Schematic.appendNode(N6);

  // Left box - Left horizontal line (ZA)
  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, TL3_pos);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_ZA.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_ZA.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  // Center horizontal line (ZB)
  ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, TL4_pos);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  // Right box - Top vertical line (ZB)
  ComponentInfo MLIN5(QString("MLIN5"), MicrostripLine, 90, TL5_pos);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN5.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);

  // Right box - Bottom vertical line (ZB)
  ComponentInfo MLIN6(QString("MLIN6"), MicrostripLine, 90, TL6_pos);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_ZB.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_ZB.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN6);

  // Right box - Right horizontal line (ZD)
  ComponentInfo MLIN7(QString("MLIN7"), MicrostripLine, TL7_pos);
  MLIN7.val["Width"] = ConvertLengthFromM("mm", MSL_ZD.Results.width);
  MLIN7.val["Length"] = ConvertLengthFromM("mm", MSL_ZD.Results.length * 1e-3);
  MLIN7.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN7.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN7.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN7.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN7.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN7);

  // Wire connections
  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N3.ID, 0);
  Schematic.appendWire(Riso.ID, 1, N4.ID, 0);

  Schematic.appendWire(N1.ID, 0, MLIN1.ID, 0);
  Schematic.appendWire(N2.ID, 0, MLIN5.ID, 1);
  Schematic.appendWire(N2.ID, 0, MLIN7.ID, 1);
  Schematic.appendWire(N3.ID, 0, MLIN7.ID, 0);
  Schematic.appendWire(N3.ID, 0, MLIN6.ID, 1);
  Schematic.appendWire(N1.ID, 0, MLIN3.ID, 1);
  Schematic.appendWire(N4.ID, 1, MLIN2.ID, 0);
  Schematic.appendWire(N4.ID, 1, MLIN3.ID, 0);
  Schematic.appendWire(Riso.ID, 0, Ground.ID, 0);

  Schematic.appendWire(MLIN1.ID, 1, N5.ID, 0);
  Schematic.appendWire(MLIN5.ID, 0, N5.ID, 0);
  Schematic.appendWire(MLIN4.ID, 1, N5.ID, 0);
  Schematic.appendWire(MLIN2.ID, 1, N6.ID, 0);
  Schematic.appendWire(MLIN4.ID, 0, N6.ID, 0);
  Schematic.appendWire(MLIN6.ID, 0, N6.ID, 0);
}

// Since the components' location is shared between TLIN and MLIN
// implementations, it makes sense to have a common function to set them up
void DoubleBoxBranchline::setComponentsLocation() {
  // Define components' location

  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Input port
  Port_in = QPoint(0, 0);

  // 1st vertical TL (TLIN3)
  TL3_pos = QPoint(Port_in.x() + x_spacing, Port_in.y() + y_spacing);

  // 1st top horizontal line (TLIN1)
  TL1_pos = QPoint(TL3_pos.x() + x_spacing, Port_in.y());

  // 1st bottom horizontal line (TLIN2)
  TL2_pos = QPoint(TL1_pos.x(), TL3_pos.y() + y_spacing);

  // Isolation resistor
  Riso_pos = QPoint(TL3_pos.x(), TL2_pos.y() + y_spacing);
  GND_Riso_pos = QPoint(Riso_pos.x(), Riso_pos.y() + 50);

  // 2nd vertical line (TLIN4)
  TL4_pos = QPoint(TL2_pos.x() + x_spacing, TL3_pos.y());

  // 2nd top horizontal line (TLIN5)
  TL5_pos = QPoint(TL4_pos.x() + x_spacing, Port_in.y());

  // 2nd bottom horizontal line (TLIN6)
  TL6_pos = QPoint(TL5_pos.x(), TL4_pos.y() + y_spacing);

  // 3rd vertical line (TLIN7)
  TL7_pos = QPoint(TL6_pos.x() + x_spacing, TL3_pos.y());

  // Top output port
  Port_out1 = QPoint(TL7_pos.x() + x_spacing, Port_in.y());

  // Bottom output port
  Port_out2 = QPoint(Port_out1.x(), TL2_pos.y());

  // Node1: Next to the input port
  N1_pos = QPoint(TL3_pos.x(), Port_in.y());

  // Node2: Next to the first output port
  N2_pos = QPoint(TL7_pos.x(), Port_out1.y());

  // Node3: Next to the second output port
  N3_pos = QPoint(TL7_pos.x(), Port_out2.y());

  // Node4: Next to isolation resistor
  N4_pos = QPoint(Riso_pos.x(), TL2_pos.y());

  // Node5: Above the 2nd vertical line
  N5_pos = QPoint(TL4_pos.x(), TL5_pos.y());

  // Node6: Below the 2nd vertical line
  N6_pos = QPoint(TL4_pos.x(), TL6_pos.y());
}
