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

#include "Gysel.h"

Gysel::Gysel() {}

Gysel::Gysel(PowerCombinerParams PS) { Specification = PS; }

Gysel::~Gysel() {}

void Gysel::calculateParams() {
  lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);
  lambda2 = lambda4 * 2;
}

void Gysel::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildGysel_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildGysel_Microstrip();
  }
}

void Gysel::buildGysel_IdealTL() {
  Schematic.Comps.clear();

  // Define components' location
  setComponentsLocation();

  // Input S-par term
  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Node in front of TermSpar1
  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);

  // Transmission line above N1
  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, TL1_pos);
  TL1.val["Z0"] = num2str(sqrt(2) * Specification.Z0, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(N1.ID, 0, TermSpar1.ID, 0);
  Schematic.appendWire(TL1.ID, 0, N1.ID, 0);

  // Transmission line below N1
  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, TL2_pos);
  TL2.val["Z0"] = num2str(sqrt(2) * Specification.Z0, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL2.ID, 1, N1.ID, 0);

  // Top-left node
  NodeInfo N2(QString("N2"), N2_pos);
  Schematic.appendNode(N2);

  // Top horizontal line
  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, 90, TL3_pos);
  TL3.val["Z0"] = num2str(Specification.Z0, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  // Top-left term
  ComponentInfo TermSpar2(QString("T2"), Term, Port_out_up);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(N2.ID, 0, TermSpar2.ID, 0);
  Schematic.appendWire(TL1.ID, 1, N2.ID, 0);
  Schematic.appendWire(TL3.ID, 0, N2.ID, 0);

  // Bottom horizontal line
  ComponentInfo TL4(QString("TLIN4"), TransmissionLine, 90, TL4_pos);
  TL4.val["Z0"] = num2str(Specification.Z0, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL4);

  // Bottom S-parameter term
  ComponentInfo TermSpar3(QString("T3"), Term, Port_out_bottom);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Bottom-left node
  NodeInfo N3(QString("N3"), N3_pos);
  Schematic.appendNode(N3);

  Schematic.appendWire(N3.ID, 0, TermSpar3.ID, 0);
  Schematic.appendWire(TL2.ID, 0, N3.ID, 0);
  Schematic.appendWire(TL4.ID, 0, N3.ID, 0);

  // Top right node (upper R connection)
  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);

  // Center vertical line
  ComponentInfo TL5(QString("TLIN5"), TransmissionLine, TL5_pos);
  TL5.val["Z0"] = num2str(Specification.Z0 / sqrt(2), Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda2);
  Schematic.appendComponent(TL5);

  NodeInfo N5(QString("N5"), N5_pos);
  Schematic.appendNode(N5);

  // Bottom resistor
  ComponentInfo Rbottom(QString("R1"), Resistor, R_bottom);
  Rbottom.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Rbottom);

  ComponentInfo Ground_Rbottom(QString("GND1"), GND, R_GND_bottom);
  Schematic.appendComponent(Ground_Rbottom);

  Schematic.appendWire(Rbottom.ID, 0, Ground_Rbottom.ID, 0);
  Schematic.appendWire(Rbottom.ID, 1, N5.ID, 0);
  Schematic.appendWire(TL5.ID, 0, N5.ID, 0);
  Schematic.appendWire(TL4.ID, 1, N5.ID, 0);
  Schematic.appendWire(TL5.ID, 1, N4.ID, 1);

  // Top resistor
  ComponentInfo Rtop(QString("R2"), Resistor, R_top);
  Rtop.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Rtop);

  ComponentInfo Ground_Rtop(QString("GND2"), GND, R_GND_top);
  Schematic.appendComponent(Ground_Rtop);

  Schematic.appendWire(Rtop.ID, 0, Ground_Rtop.ID, 0);
  Schematic.appendWire(Rtop.ID, 1, N4.ID, 0);
  Schematic.appendWire(TL3.ID, 1, N4.ID, 0);
}

void Gysel::buildGysel_Microstrip() {
  Schematic.Comps.clear();

  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Node in front of TermSpar1
  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);

  // Synthesize microstrip lines for different impedances
  // Quarter-wave lines at sqrt(2)*Z0
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specification.MS_Subs;
  MSL_Input.synthesizeMicrostrip(sqrt(2) * Specification.Z0, lambda4 * 1e3,
                                 Specification.freq);

  // Transmission line above N1
  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, TL1_pos);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  Schematic.appendWire(N1.ID, 0, TermSpar1.ID, 0);
  Schematic.appendWire(MLIN1.ID, 0, N1.ID, 0);

  // Transmission line below N1
  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, TL2_pos);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  Schematic.appendWire(MLIN2.ID, 1, N1.ID, 0);

  // Top-left node
  NodeInfo N2(QString("N2"), N2_pos);
  Schematic.appendNode(N2);

  // Quarter-wave lines at Z0
  MicrostripClass MSL_Z0;
  MSL_Z0.Substrate = Specification.MS_Subs;
  MSL_Z0.synthesizeMicrostrip(Specification.Z0, lambda4 * 1e3,
                              Specification.freq);

  // Top horizontal line
  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, 90, TL3_pos);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Z0.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_Z0.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  // Top-left term
  ComponentInfo TermSpar2(QString("T2"), Term, Port_out_up);
  TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(N2.ID, 0, TermSpar2.ID, 0);
  Schematic.appendWire(MLIN1.ID, 1, N2.ID, 0);
  Schematic.appendWire(MLIN3.ID, 0, N2.ID, 0);

  // Bottom horizontal line
  ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, 90, TL4_pos);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Z0.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_Z0.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  // Bottom S-parameter term
  ComponentInfo TermSpar3(QString("T3"), Term, Port_out_bottom);
  TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  // Bottom-left node
  NodeInfo N3(QString("N3"), N3_pos);
  Schematic.appendNode(N3);

  Schematic.appendWire(N3.ID, 0, TermSpar3.ID, 0);
  Schematic.appendWire(MLIN2.ID, 0, N3.ID, 0);
  Schematic.appendWire(MLIN4.ID, 0, N3.ID, 0);

  // Top right node (upper R connection)
  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);

  // Half-wave line at Z0/sqrt(2)
  MicrostripClass MSL_HalfWave;
  MSL_HalfWave.Substrate = Specification.MS_Subs;
  MSL_HalfWave.synthesizeMicrostrip(Specification.Z0 / sqrt(2), lambda2 * 1e3,
                                    Specification.freq);

  // Center vertical line
  ComponentInfo MLIN5(QString("MLIN5"), MicrostripLine, TL5_pos);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_HalfWave.Results.width);
  MLIN5.val["Length"] =
      ConvertLengthFromM("mm", MSL_HalfWave.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);

  NodeInfo N5(QString("N5"), N5_pos);
  Schematic.appendNode(N5);

  // Bottom resistor
  ComponentInfo Rbottom(QString("R1"), Resistor, R_bottom);
  Rbottom.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Rbottom);

  ComponentInfo Ground_Rbottom(QString("GND1"), GND, R_GND_bottom);
  Schematic.appendComponent(Ground_Rbottom);

  Schematic.appendWire(Rbottom.ID, 0, Ground_Rbottom.ID, 0);
  Schematic.appendWire(Rbottom.ID, 1, N5.ID, 0);
  Schematic.appendWire(MLIN5.ID, 0, N5.ID, 0);
  Schematic.appendWire(MLIN4.ID, 1, N5.ID, 0);
  Schematic.appendWire(MLIN5.ID, 1, N4.ID, 1);

  // Top resistor
  ComponentInfo Rtop(QString("R2"), Resistor, R_top);
  Rtop.val["R"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(Rtop);

  ComponentInfo Ground_Rtop(QString("GND2"), GND, R_GND_top);
  Schematic.appendComponent(Ground_Rtop);

  Schematic.appendWire(Rtop.ID, 0, Ground_Rtop.ID, 0);
  Schematic.appendWire(Rtop.ID, 1, N4.ID, 0);
  Schematic.appendWire(MLIN3.ID, 1, N4.ID, 0);
}

// Set the components's location for the TLIN and MS implementations
void Gysel::setComponentsLocation() {
  // Spacing between components
  x_spacing = 60;
  y_spacing = 60;

  // Input port
  Port_in = QPoint(0, 0);

  // Node in front of the input port
  N1_pos = QPoint(Port_in.x() + x_spacing,
                  Port_in.y()); // Shift 20 points away from the input port

  // Top vertical line (TL1)
  TL1_pos = QPoint(N1_pos.x(), N1_pos.y() - y_spacing);

  // Bottom vertical line (TL2)
  TL2_pos = QPoint(N1_pos.x(), N1_pos.y() + y_spacing);

  // Top left node
  N2_pos = QPoint(N1_pos.x(), TL1_pos.y() - y_spacing);

  // Top horizontal (TL3)
  TL3_pos = QPoint(N2_pos.x() + x_spacing, N2_pos.y());

  // Top-left S-parameter term
  Port_out_up = QPoint(Port_in.x(), N2_pos.y());

  // Bottom-left node
  N3_pos = QPoint(N1_pos.x(), TL2_pos.y() + y_spacing);

  // Bottom horizontal line
  TL4_pos = QPoint(TL3_pos.x(), N3_pos.y());

  // Bottom-left S-parameter term
  Port_out_bottom = QPoint(Port_in.x(), N3_pos.y());

  // Line between resistors
  TL5_pos = QPoint(TL3_pos.x() + x_spacing, N1_pos.y());

  // Top right node (upper R connection)
  N4_pos = QPoint(TL5_pos.x(), TL3_pos.y());

  // Bottom right node (lower R connection)
  N5_pos = QPoint(N4_pos.x(), TL4_pos.y());

  // Top resistor
  R_top = QPoint(N4_pos.x() + x_spacing, N4_pos.y() + y_spacing / 2);
  R_GND_top = QPoint(R_top.x(), R_top.y() + 50);

  // Bottom resistor
  R_bottom = QPoint(R_top.x(), N5_pos.y() + y_spacing / 2);
  R_GND_bottom = QPoint(R_bottom.x(), R_bottom.y() + 50);
}
