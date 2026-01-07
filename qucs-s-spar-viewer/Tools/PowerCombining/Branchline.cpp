/// @file Branchline.cpp
/// @brief Branch-line power combiner/divider network (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "Branchline.h"

void Branchline::calculateParams() {
  double K = Specification.OutputRatio.at(0);
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
  ZA = Specification.Z0 * sqrt(K / (K + 1));
  ZB = Specification.Z0 * sqrt(K);
}

void Branchline::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildBranchline_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildBranchline_Microstrip();
  }
}

void Branchline::buildBranchline_IdealTL() {

  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo TermSpar2(QString("T2"), Term, 180, Port_out1);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Port_out2);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  ComponentInfo Riso(QString("R1"), Resistor, Riso_pos);
  Riso.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Riso);

  ComponentInfo Ground(QString("GND1"), GND, GND_Riso);
  Schematic.appendComponent(Ground);

  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, 90, TL1_pos);
  TL1.val["Z0"] = num2str(ZA, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, 90, TL2_pos);
  TL2.val["Z0"] = num2str(ZA, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, TL3_pos);
  TL3.val["Z0"] = num2str(ZB, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  ComponentInfo TL4(QString("TLIN4"), TransmissionLine, TL4_pos);
  TL4.val["Z0"] = num2str(ZB, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);

  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);

  NodeInfo N2(QString("N2"), N2_pos);
  Schematic.appendNode(N2);

  NodeInfo N3(QString("N3"), N3_pos);
  Schematic.appendNode(N3);

  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);

  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N3.ID, 0);

  Schematic.appendWire(N1.ID, 0, TL1.ID, 0);
  Schematic.appendWire(N2.ID, 0, TL1.ID, 1);

  Schematic.appendWire(N2.ID, 0, TL4.ID, 1);
  Schematic.appendWire(N3.ID, 0, TL4.ID, 0);
  Schematic.appendWire(N3.ID, 0, TL2.ID, 1);
  Schematic.appendWire(N1.ID, 0, TL3.ID, 1);

  Schematic.appendWire(Riso.ID, 1, N4.ID, 0);
  Schematic.appendWire(N4.ID, 0, TL2.ID, 0);
  Schematic.appendWire(N4.ID, 0, TL3.ID, 0);
  Schematic.appendWire(Riso.ID, 0, Ground.ID, 0);
}

void Branchline::buildBranchline_Microstrip() {

  // Define components' location
  setComponentsLocation();

  Schematic.Comps.clear();

  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo TermSpar2(QString("T2"), Term, 180, Port_out1);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  ComponentInfo TermSpar3(QString("T3"), Term, 180, Port_out2);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  ComponentInfo Riso(QString("R1"), Resistor, Riso_pos);
  Riso.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Riso);

  ComponentInfo Ground(QString("GND1"), GND, GND_Riso);
  Schematic.appendComponent(Ground);

  // Vertical series arms (ZA impedance)
  MicrostripClass MSL_Series;
  MSL_Series.Substrate = Specification.MS_Subs;
  MSL_Series.synthesizeMicrostrip(ZA, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, 90, TL1_pos);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Series.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Series.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, 90, TL2_pos);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Series.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Series.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  // Horizontal shunt arms (ZB impedance)
  MicrostripClass MSL_Shunt;
  MSL_Shunt.Substrate = Specification.MS_Subs;
  MSL_Shunt.synthesizeMicrostrip(ZB, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, TL3_pos);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Shunt.Results.width);
  MLIN3.val["Length"] =
      ConvertLengthFromM("mm", MSL_Shunt.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, TL4_pos);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Shunt.Results.width);
  MLIN4.val["Length"] =
      ConvertLengthFromM("mm", MSL_Shunt.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);

  NodeInfo N2(QString("N2"), N2_pos);
  Schematic.appendNode(N2);

  NodeInfo N3(QString("N3"), N3_pos);
  Schematic.appendNode(N3);

  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);

  Schematic.appendWire(TermSpar1.ID, 0, N1.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, N2.ID, 0);
  Schematic.appendWire(TermSpar3.ID, 0, N3.ID, 0);

  Schematic.appendWire(N1.ID, 0, MLIN1.ID, 0);
  Schematic.appendWire(N2.ID, 0, MLIN1.ID, 1);

  Schematic.appendWire(N2.ID, 0, MLIN4.ID, 1);
  Schematic.appendWire(N3.ID, 0, MLIN4.ID, 0);
  Schematic.appendWire(N3.ID, 0, MLIN2.ID, 1);
  Schematic.appendWire(N1.ID, 0, MLIN3.ID, 1);

  Schematic.appendWire(Riso.ID, 1, N4.ID, 0);
  Schematic.appendWire(N4.ID, 0, MLIN2.ID, 0);
  Schematic.appendWire(N4.ID, 0, MLIN3.ID, 0);
  Schematic.appendWire(Riso.ID, 0, Ground.ID, 0);
}

// Since the components' location is shared between TLIN and MLIN
// implementations, it makes sense to have a common function to set them up
void Branchline::setComponentsLocation() {
  // Define components' location

  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Input port
  Port_in = QPoint(0, 0);

  // First vertical (TL3)
  TL3_pos = QPoint(Port_in.x() + x_spacing, Port_in.y() + y_spacing);

  // Top horizontal line (TL1)
  TL1_pos = QPoint(TL3_pos.x() + x_spacing, Port_in.y());

  // Bottom horizontal line
  TL2_pos = QPoint(TL1_pos.x(), TL3_pos.y() + y_spacing);

  // Isolation resistor
  Riso_pos = QPoint(TL3_pos.x(), TL2_pos.y() + y_spacing);
  GND_Riso = QPoint(Riso_pos.x(), Riso_pos.y() + 50);

  // Last vertical line
  TL4_pos = QPoint(TL2_pos.x() + x_spacing, TL3_pos.y());

  // Top output port
  Port_out1 = QPoint(TL4_pos.x() + x_spacing, Port_in.y());

  // Bottom output port
  Port_out2 = QPoint(Port_out1.x(), TL2_pos.y());

  // Node in front of the input port
  N1_pos = QPoint(TL3_pos.x(), Port_in.y());

  // Node in front of the first output port
  N2_pos = QPoint(TL4_pos.x(), Port_out1.y());

  // Node in front of the second output port
  N3_pos = QPoint(N2_pos.x(), Port_out2.y());

  // Node above the isolation resistor
  N4_pos = QPoint(N1_pos.x(), Port_out2.y());
}
