/// /// @file Recombinant3WayWilkinson.cpp
/// @brief Recombinant 3-way Wilkinson-type power combiner/divider network
/// (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "Recombinant3WayWilkinson.h"

void Recombinant3WayWilkinson::calculateParams() {
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
  Z1 = Specification.Z0 * 0.72;
  Z2 = Specification.Z0 * 0.8;
  Z3 = Z2;
  Z4 = Specification.Z0 * 1.6;
  Z5 = Z3;
  Z6 = Z4;
  R1 = Specification.Z0;
  R2 = Specification.Z0 * 2;
}

void Recombinant3WayWilkinson::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildRecombinant3Way_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildRecombinant3Way_Microstrip();
  }
}

void Recombinant3WayWilkinson::buildRecombinant3Way_IdealTL() {
  // Define components' location
  setComponentsLocation();

  // Input port (T1)
  ComponentInfo TermSpar1(QString("T1"), Term, 0, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Line after the input port
  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, 90, TL_pos[0]);
  TL1.val["Z0"] = num2str(Z1, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  // Upper branch. First section
  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, 90, TL_pos[1]);
  TL2.val["Z0"] = num2str(Z2, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  // Lower branch. First section
  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, 90, TL_pos[2]);
  TL3.val["Z0"] = num2str(Z2, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  // First isolation resistor
  ComponentInfo R1(QString("R1"), Resistor, Riso_pos[0]);
  R1.val["R"] = num2str(this->R1, Resistance);
  Schematic.appendComponent(R1);

  // Upper branch. Last section, top
  ComponentInfo TL4(QString("TLIN4"), TransmissionLine, 90, TL_pos[3]);
  TL4.val["Z0"] = num2str(Z3, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);

  // Upper output transmission line
  ComponentInfo TL8(QString("TLIN8"), TransmissionLine, 90, TL_pos[7]);
  TL8.val["Z0"] = num2str(Z5, Resistance);
  TL8.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL8);

  // First output node
  ComponentInfo TermSpar2(QString("T2"), Term, 180, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Upper branch. Last section, bottom
  ComponentInfo TL5(QString("TLIN5"), TransmissionLine, 90, TL_pos[4]);
  TL5.val["Z0"] = num2str(Z4, Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL5);

  // Upper branch isolation resistor
  ComponentInfo R2(QString("R2"), Resistor, Riso_pos[1]);
  R2.val["R"] = num2str(this->R2, Resistance);
  Schematic.appendComponent(R2);

  // Central output transmission line
  ComponentInfo TL9(QString("TLIN9"), TransmissionLine, 90, TL_pos[8]);
  TL9.val["Z0"] = num2str(Z5, Resistance);
  TL9.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL9);

  // Central output port
  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Second section. Lower branch. Top.
  ComponentInfo TL6(QString("TLIN6"), TransmissionLine, 90, TL_pos[5]);
  TL6.val["Z0"] = num2str(Z4, Resistance);
  TL6.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL6);

  // Second section. Lower branch. Bottom
  ComponentInfo TL7(QString("TLIN7"), TransmissionLine, 90, TL_pos[6]);
  TL7.val["Z0"] = num2str(Z3, Resistance);
  TL7.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL7);

  // Bottom output isolation resistor
  ComponentInfo R3(QString("R3"), Resistor, Riso_pos[2]);
  R3.val["R"] = num2str(this->R2, Resistance);
  Schematic.appendComponent(R3);

  // Lower output transmission line
  ComponentInfo TL10(QString("TLIN10"), TransmissionLine, 90, TL_pos[9]);
  TL10.val["Z0"] = num2str(Z5, Resistance);
  TL10.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL10);

  // Bottom output port
  ComponentInfo TermSpar4(QString("T4"), Term, 180, Ports_pos[3]);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  // Nodes

  // First column of nodes. Central node
  NodeInfo N1(QString("N1"), N_pos[0]);
  Schematic.appendNode(N1);

  // First column of nodes. Upper node (virtual)
  NodeInfo N2(QString("N2"), N_pos[1]);
  N2.visible = false;
  Schematic.appendNode(N2);

  // First column of nodes. Bottom (virtual)
  NodeInfo N3(QString("N3"), N_pos[2]);
  N3.visible = false;
  Schematic.appendNode(N3);

  // Second column of nodes. Upper
  NodeInfo N4(QString("N4"), N_pos[3]);
  Schematic.appendNode(N4);

  // Second column of nodes. Bottom
  NodeInfo N5(QString("N5"), N_pos[4]);
  Schematic.appendNode(N5);

  // Third column of nodes. Central ndoe
  NodeInfo N6(QString("N6"), N_pos[5]);
  N6.visible = false;
  Schematic.appendNode(N6);

  // Third column of nodes. Upper branch. Top.
  NodeInfo N7(QString("N7"), N_pos[6]);
  Schematic.appendNode(N7);

  // Third column of nodes. Upper branch, bottom
  NodeInfo N8(QString("N8"), N_pos[7]);
  Schematic.appendNode(N8);

  NodeInfo N9(QString("N9"), N_pos[8]);
  N9.visible = false;
  Schematic.appendNode(N9);

  NodeInfo N10(QString("N10"), N_pos[9]);
  N10.visible = false;
  Schematic.appendNode(N10);

  NodeInfo N11(QString("N11"), N_pos[10]);
  N11.visible = false;
  Schematic.appendNode(N11);

  NodeInfo N12(QString("N12"), N_pos[11]);
  Schematic.appendNode(N12);

  NodeInfo N13(QString("N13"), N_pos[12]);
  Schematic.appendNode(N13);

  NodeInfo N14(QString("N14"), N_pos[13]);
  Schematic.appendNode(N14);

  NodeInfo N15(QString("N15"), N_pos[14]);
  Schematic.appendNode(N15);

  // Wiring

  // Input port to the first line
  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);

  // Connections to N1
  Schematic.appendWire(N1.ID, 1, TL1.ID, 1);
  Schematic.appendWire(N1.ID, 0, N2.ID, 0);
  Schematic.appendWire(N1.ID, 0, N3.ID, 0);

  // Connections to N2
  Schematic.appendWire(N2.ID, 0, TL2.ID, 0);

  // Connections to N3
  Schematic.appendWire(N3.ID, 0, TL3.ID, 0);

  // Connections to N4
  Schematic.appendWire(N4.ID, 0, TL2.ID, 1);
  Schematic.appendWire(N4.ID, 0, R1.ID, 1);
  Schematic.appendWire(N4.ID, 0, N7.ID, 0);

  // Connections to N5
  Schematic.appendWire(N5.ID, 0, TL3.ID, 1);
  Schematic.appendWire(N5.ID, 0, R1.ID, 0);
  Schematic.appendWire(N5.ID, 0, N8.ID, 0);

  // Connections to N6
  Schematic.appendWire(N6.ID, 0, N7.ID, 0);
  Schematic.appendWire(N6.ID, 0, TL4.ID, 0);

  // Connections to N7
  Schematic.appendWire(N7.ID, 0, TL5.ID, 0);

  // Connections to N8
  Schematic.appendWire(N8.ID, 0, TL6.ID, 0);
  Schematic.appendWire(N8.ID, 0, N9.ID, 0);

  // Connections to N9
  Schematic.appendWire(N9.ID, 0, TL7.ID, 0);

  // Connections to N10
  Schematic.appendWire(N10.ID, 0, TL4.ID, 1);
  Schematic.appendWire(N10.ID, 0, R2.ID, 1);
  Schematic.appendWire(N10.ID, 0, TL8.ID, 0);

  // Upper output
  Schematic.appendWire(TL8.ID, 1, TermSpar2.ID, 0);

  // Connections to N11
  Schematic.appendWire(N11.ID, 0, TL5.ID, 1);
  Schematic.appendWire(N11.ID, 0, R2.ID, 0);
  Schematic.appendWire(N11.ID, 0, N12.ID, 0);

  // Connections to N12
  Schematic.appendWire(N12.ID, 0, TL9.ID, 0);
  Schematic.appendWire(N12.ID, 0, N13.ID, 0);

  // Central output
  Schematic.appendWire(TL9.ID, 1, TermSpar3.ID, 0);

  // Connections to N13
  Schematic.appendWire(N13.ID, 0, TL6.ID, 1);
  Schematic.appendWire(N13.ID, 0, R3.ID, 1);

  // Connections to N14
  Schematic.appendWire(N14.ID, 0, TL10.ID, 0);
  Schematic.appendWire(N14.ID, 0, R3.ID, 0);
  Schematic.appendWire(N14.ID, 0, TL7.ID, 1);

  // Bottom output
  Schematic.appendWire(TL10.ID, 1, TermSpar4.ID, 0);
}

void Recombinant3WayWilkinson::buildRecombinant3Way_Microstrip() {
  // Define components' location
  setComponentsLocation();

  // Input port (T1)
  ComponentInfo TermSpar1(QString("T1"), Term, 0, Ports_pos[0]);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Synthesize microstrip lines for different impedances
  // Z1 impedance (input line)
  MicrostripClass MSL_Z1;
  MSL_Z1.Substrate = Specification.MS_Subs;
  MSL_Z1.synthesizeMicrostrip(Z1, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, 90, TL_pos[0]);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Z1.Results.width);
  MLIN1.val["Length"] = ConvertLengthFromM("mm", MSL_Z1.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  // Z2 impedance lines (4 lines)
  MicrostripClass MSL_Z2;
  MSL_Z2.Substrate = Specification.MS_Subs;
  MSL_Z2.synthesizeMicrostrip(Z2, lambda4 * 1e3, Specification.freq);

  // Upper branch. First section
  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, 90, TL_pos[1]);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN2.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  // Lower branch. First section
  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, 90, TL_pos[2]);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  // First isolation resistor
  ComponentInfo R1(QString("R1"), Resistor, 0, Riso_pos[0]);
  R1.val["R"] = num2str(this->R1, Resistance);
  Schematic.appendComponent(R1);

  // Upper branch. Last section, top
  ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, 90, TL_pos[3]);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Z2.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_Z2.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  // Upper output transmission line
  // Z2 impedance lines (4 lines)
  MicrostripClass MSL_Z5;
  MSL_Z5.Substrate = Specification.MS_Subs;
  MSL_Z5.synthesizeMicrostrip(Z5, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN8(QString("MLIN8"), MicrostripLine, 90, TL_pos[7]);
  MLIN8.val["Width"] = ConvertLengthFromM("mm", MSL_Z5.Results.width);
  MLIN8.val["Length"] = ConvertLengthFromM("mm", MSL_Z5.Results.length * 1e-3);
  MLIN8.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN8.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN8.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN8.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN8.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN8);

  // First output node
  ComponentInfo TermSpar2(QString("T2"), Term, 180, Ports_pos[1]);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Z4 impedance lines (2 lines)
  MicrostripClass MSL_Z4;
  MSL_Z4.Substrate = Specification.MS_Subs;
  MSL_Z4.synthesizeMicrostrip(Z4, lambda4 * 1e3, Specification.freq);

  // Upper branch. Last section, bottom
  ComponentInfo MLIN5(QString("MLIN5"), MicrostripLine, 90, TL_pos[4]);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_Z4.Results.width);
  MLIN5.val["Length"] = ConvertLengthFromM("mm", MSL_Z4.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);

  // Upper branch isolation resistor
  ComponentInfo R2(QString("R2"), Resistor, 0, Riso_pos[1]);
  R2.val["R"] = num2str(this->R2, Resistance);
  Schematic.appendComponent(R2);

  // Central output transmission line
  ComponentInfo MLIN9(QString("MLIN9"), MicrostripLine, 90, TL_pos[8]);
  MLIN9.val["Width"] = ConvertLengthFromM("mm", MSL_Z5.Results.width);
  MLIN9.val["Length"] = ConvertLengthFromM("mm", MSL_Z5.Results.length * 1e-3);
  MLIN9.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN9.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN9.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN9.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN9.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN9);

  // Central output port
  ComponentInfo TermSpar3(QString("T3"), Term, 180, Ports_pos[2]);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Second section. Lower branch. Top.
  ComponentInfo MLIN6(QString("MLIN6"), MicrostripLine, 90, TL_pos[5]);
  MLIN6.val["Width"] = ConvertLengthFromM("mm", MSL_Z4.Results.width);
  MLIN6.val["Length"] = ConvertLengthFromM("mm", MSL_Z4.Results.length * 1e-3);
  MLIN6.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN6.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN6.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN6.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN6.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN6);

  // Second section. Lower branch. Bottom
  MicrostripClass MSL_Z3;
  MSL_Z3.Substrate = Specification.MS_Subs;
  MSL_Z3.synthesizeMicrostrip(Z3, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN7(QString("MLIN7"), MicrostripLine, 90, TL_pos[6]);
  MLIN7.val["Width"] = ConvertLengthFromM("mm", MSL_Z3.Results.width);
  MLIN7.val["Length"] = ConvertLengthFromM("mm", MSL_Z3.Results.length * 1e-3);
  MLIN7.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN7.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN7.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN7.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN7.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN7);

  // Bottom output isolation resistor
  ComponentInfo R3(QString("R3"), Resistor, 0, Riso_pos[2]);
  R3.val["R"] = num2str(this->R2, Resistance);
  Schematic.appendComponent(R3);

  // Lower output transmission line
  ComponentInfo MLIN10(QString("MLIN10"), MicrostripLine, 90, TL_pos[9]);
  MLIN10.val["Width"] = ConvertLengthFromM("mm", MSL_Z5.Results.width);
  MLIN10.val["Length"] = ConvertLengthFromM("mm", MSL_Z5.Results.length * 1e-3);
  MLIN10.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN10.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN10.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN10.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN10.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN10);

  // Bottom output port
  ComponentInfo TermSpar4(QString("T4"), Term, 180, Ports_pos[3]);
  TermSpar4.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar4);

  // Nodes

  // First column of nodes. Central node
  NodeInfo N1(QString("N1"), N_pos[0]);
  Schematic.appendNode(N1);

  // First column of nodes. Upper node (virtual)
  NodeInfo N2(QString("N2"), N_pos[1]);
  N2.visible = false;
  Schematic.appendNode(N2);

  // First column of nodes. Bottom (virtual)
  NodeInfo N3(QString("N3"), N_pos[2]);
  N3.visible = false;
  Schematic.appendNode(N3);

  // Second column of nodes. Upper
  NodeInfo N4(QString("N4"), N_pos[3]);
  Schematic.appendNode(N4);

  // Second column of nodes. Bottom
  NodeInfo N5(QString("N5"), N_pos[4]);
  Schematic.appendNode(N5);

  // Third column of nodes. Central ndoe
  NodeInfo N6(QString("N6"), N_pos[5]);
  N6.visible = false;
  Schematic.appendNode(N6);

  // Third column of nodes. Upper branch. Top.
  NodeInfo N7(QString("N7"), N_pos[6]);
  Schematic.appendNode(N7);

  // Third column of nodes. Upper branch, bottom
  NodeInfo N8(QString("N8"), N_pos[7]);
  Schematic.appendNode(N8);

  NodeInfo N9(QString("N9"), N_pos[8]);
  N9.visible = false;
  Schematic.appendNode(N9);

  NodeInfo N10(QString("N10"), N_pos[9]);
  N10.visible = false;
  Schematic.appendNode(N10);

  NodeInfo N11(QString("N11"), N_pos[10]);
  N11.visible = false;
  Schematic.appendNode(N11);

  NodeInfo N12(QString("N12"), N_pos[11]);
  Schematic.appendNode(N12);

  NodeInfo N13(QString("N13"), N_pos[12]);
  Schematic.appendNode(N13);

  NodeInfo N14(QString("N14"), N_pos[13]);
  Schematic.appendNode(N14);

  NodeInfo N15(QString("N15"), N_pos[14]);
  Schematic.appendNode(N15);

  // Wiring

  // Input port to the first line
  Schematic.appendWire(TermSpar1.ID, 0, MLIN1.ID, 0);

  // Connections to N1
  Schematic.appendWire(N1.ID, 1, MLIN1.ID, 1);
  Schematic.appendWire(N1.ID, 0, N2.ID, 0);
  Schematic.appendWire(N1.ID, 0, N3.ID, 0);

  // Connections to N2
  Schematic.appendWire(N2.ID, 0, MLIN2.ID, 0);

  // Connections to N3
  Schematic.appendWire(N3.ID, 0, MLIN3.ID, 0);

  // Connections to N4
  Schematic.appendWire(N4.ID, 0, MLIN2.ID, 1);
  Schematic.appendWire(N4.ID, 0, R1.ID, 1);
  Schematic.appendWire(N4.ID, 0, N7.ID, 0);

  // Connections to N5
  Schematic.appendWire(N5.ID, 0, MLIN3.ID, 1);
  Schematic.appendWire(N5.ID, 0, R1.ID, 0);
  Schematic.appendWire(N5.ID, 0, N8.ID, 0);

  // Connections to N6
  Schematic.appendWire(N6.ID, 0, N7.ID, 0);
  Schematic.appendWire(N6.ID, 0, MLIN4.ID, 0);

  // Connections to N7
  Schematic.appendWire(N7.ID, 0, MLIN5.ID, 0);

  // Connections to N8
  Schematic.appendWire(N8.ID, 0, MLIN6.ID, 0);
  Schematic.appendWire(N8.ID, 0, N9.ID, 0);

  // Connections to N9
  Schematic.appendWire(N9.ID, 0, MLIN7.ID, 0);

  // Connections to N10
  Schematic.appendWire(N10.ID, 0, MLIN4.ID, 1);
  Schematic.appendWire(N10.ID, 0, R2.ID, 1);
  Schematic.appendWire(N10.ID, 0, MLIN8.ID, 0);

  // Upper output
  Schematic.appendWire(MLIN8.ID, 1, TermSpar2.ID, 0);

  // Connections to N11
  Schematic.appendWire(N11.ID, 0, MLIN5.ID, 1);
  Schematic.appendWire(N11.ID, 0, R2.ID, 0);
  Schematic.appendWire(N11.ID, 0, N12.ID, 0);

  // Connections to N12
  Schematic.appendWire(N12.ID, 0, MLIN9.ID, 0);
  Schematic.appendWire(N12.ID, 0, N13.ID, 0);

  // Central output
  Schematic.appendWire(MLIN9.ID, 1, TermSpar3.ID, 0);

  // Connections to N13
  Schematic.appendWire(N13.ID, 0, MLIN6.ID, 1);
  Schematic.appendWire(N13.ID, 0, R3.ID, 1);

  // Connections to N14
  Schematic.appendWire(N14.ID, 0, MLIN10.ID, 0);
  Schematic.appendWire(N14.ID, 0, R3.ID, 0);
  Schematic.appendWire(N14.ID, 0, MLIN7.ID, 1);

  // Bottom output
  Schematic.appendWire(MLIN10.ID, 1, TermSpar4.ID, 0);
}

void Recombinant3WayWilkinson::setComponentsLocation() {
  // Define components' location
  //
  // Topology:
  //
  //                                       (N6)--[TL4]--(N10)--[TL8]--> T2
  //                                         |             |
  //                                         |           [R2]
  //                                         |             |
  //                  (N2)--[TL2]---(N4)--- (N7)--[TL5]--(N11)
  //                   |              |                    |
  //                   |              |                    |
  //                   |              |                    |
  //                   |              |                    |
  //   T1 ->--[TL1]--(N1)            [R1]                (N12)--[TL9]--> T3
  //                   |              |                    |
  //                   |              |                    |
  //                   |              |                    |
  //                   |              |                    |
  //                  (N3)--[TL3]---(N5)---(N8)--[TL6]--(N13)
  //                                        |              |
  //                                        |             [R3]
  //                                        |              |
  //                                       (N9)--[TL7]--(N14)--[TL10]--> T4

  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Vector allocation
  Ports_pos.resize(4);
  TL_pos.resize(10);
  Riso_pos.resize(3);
  N_pos.resize(16);

  // Input port
  QPoint T1 = QPoint(0, 0);
  Ports_pos[0] = T1;

  // First line after the input port
  QPoint TL1 = QPoint(T1.x() + x_spacing, T1.y());
  TL_pos[0] = TL1;

  // First node column. Center node
  QPoint N1 = QPoint(TL1.x() + x_spacing, T1.y());
  N_pos[0] = N1;

  // First node column. Upper virtual node
  QPoint N2 = QPoint(N1.x(), N1.y() - y_spacing);
  N_pos[1] = N2;

  // First node column. Lower virtual node
  QPoint N3 = QPoint(N1.x(), N1.y() + y_spacing);
  N_pos[2] = N3;

  // Second column of TLs. Top
  QPoint TL2 = QPoint(N2.x() + x_spacing, N2.y());
  TL_pos[1] = TL2;

  // Second column of TLs. Bottom
  QPoint TL3 = QPoint(N3.x() + x_spacing, N3.y());
  TL_pos[2] = TL3;

  // Second column of nodes. Top
  QPoint N4 = QPoint(TL2.x() + x_spacing, TL2.y());
  N_pos[3] = N4;

  // First isolation resistor
  QPoint R1 = QPoint(N4.x(), N4.y() + y_spacing);
  Riso_pos[0] = R1;

  // Second column of nodes. Bottom
  QPoint N5 = QPoint(N4.x(), TL3.y());
  N_pos[4] = N5;

  // Third column of nodes. Upper branch, top.
  QPoint N6 = QPoint(N4.x() + x_spacing, N4.y() - 2 * y_spacing);
  N_pos[5] = N6;

  // Third column of nodes. Upper branch, top
  QPoint N7 = QPoint(N6.x(), N4.y());
  N_pos[6] = N7;

  // Third column of nodes. Upper branch, bottom.
  QPoint N8 = QPoint(N7.x(), N5.y());
  N_pos[7] = N8;

  // Third column of nodes. Upper branch, bottom.
  QPoint N9 = QPoint(N8.x(), N8.y() + 2 * y_spacing);
  N_pos[8] = N9;

  // Upper branch, top transmission line, before T2
  QPoint TL4 = QPoint(N6.x() + x_spacing, N6.y());
  TL_pos[3] = TL4;

  // Node between TL4 and T2
  QPoint N10 = QPoint(TL4.x() + x_spacing, TL4.y());
  N_pos[9] = N10;

  // Upper output TL
  QPoint TL8 = QPoint(N10.x() + x_spacing, N10.y());
  TL_pos[7] = TL8;

  // T2 output port
  QPoint T2 = QPoint(TL8.x() + x_spacing, N10.y());
  Ports_pos[1] = T2;

  // Upper branch output isolation resistor
  QPoint R2 = QPoint(N10.x(), N10.y() + y_spacing);
  Riso_pos[1] = R2;

  // Upper branch, top transmission line
  QPoint TL5 = QPoint(TL4.x(), N7.y());
  TL_pos[4] = TL5;

  // Node between the upper isolation resistor and TL5
  QPoint N11 = QPoint(R2.x(), TL5.y());
  N_pos[10] = N11;

  // T3 output node
  QPoint N12 = QPoint(N11.x(), T1.y());
  N_pos[11] = N12;

  // Central output TL
  QPoint TL9 = QPoint(TL8.x(), N12.y());
  TL_pos[8] = TL9;

  // Central output term
  QPoint T3 = QPoint(T2.x(), T1.y());
  Ports_pos[2] = T3;

  // Node between TL6 and the lower isolation resistor
  QPoint N13 = QPoint(N12.x(), N8.y());
  N_pos[12] = N13;

  // Node in front of T4
  QPoint N14 = QPoint(N13.x(), N9.y());
  N_pos[13] = N14;

  // Lower output TL
  QPoint TL10 = QPoint(TL9.x(), N14.y());
  TL_pos[9] = TL10;

  // Lower branch output term
  QPoint T4 = QPoint(T3.x(), N14.y());
  Ports_pos[3] = T4;

  // Lower branch, top transmission line
  QPoint TL6 = QPoint(TL4.x(), N8.y());
  TL_pos[5] = TL6;

  // Lower branch, bottom transmission line. Before T4
  QPoint TL7 = QPoint(TL6.x(), N9.y());
  TL_pos[6] = TL7;

  // Node in front of T4
  QPoint N15 = QPoint(N14.x(), TL7.y());
  N_pos[14] = N15;

  // Lower branch output isolation resistor
  QPoint R3 = QPoint(N15.x(), TL6.y() + y_spacing);
  Riso_pos[2] = R3;
}
