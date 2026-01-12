/// @file Wilkinson2Way.cpp
/// @brief Wilkinson power combiner/divider network (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-laterng with this program.  If not, see
/// <https://www.gnu.org/licenses/>.

#include "Wilkinson2Way.h"

void Wilkinson2Way::calculateParams() {
  double K = Specification.OutputRatio.at(0);
  Z2 = Specification.Z0 * sqrt(2 * (1 + K) / K);
  Z3 = Specification.Z0 * sqrt(2 * (1 + K));
  R2 = Specification.Z0 * K;
  R3 = Specification.Z0 / K;
  R = R2 + R3;
}

void Wilkinson2Way::synthesize() {
  calculateParams();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Lumped) {
    buildWilkinson_LumpedLC();
  } else if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildWilkinson_IdealTL();
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildWilkinson_Microstrip();
  }
}

void Wilkinson2Way::buildWilkinson_LumpedLC() {
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo Ground;

  // In the schematic of a Wilkinson power combiner there are two output
  // branches and a central feed line. These variables set the y-axis coordinate
  // of these branches;

  int y_upper_branch = 0;
  int y_central_branch = -60;
  int y_lower_branch = -120;

  int x_1st_term = 50;
  int x_1st_shunt_C = 100;
  int x_split = 150;
  int x_1st_series_L = 200;
  int x_2nd_shunt_C = 250;
  int x_Riso = 300;

  // For unequal power split ratio
  int x_2nd_series_L = 350;
  int x_3rd_shunt_C = 400;

  // Design equations
  double Z4, Z5, L2_, C2_, L3_, C3_;
  double K = Specification.OutputRatio.at(0);
  double w = 2 * M_PI * Specification.freq;
  double L2 = Z2 / w;
  double C2 = 1. / (L2 * w * w);
  double L3 = Z3 / w;
  double C3 = 1. / (L3 * w * w);
  double CC = C2 + C3;

  if (R2 != R3) {
    Z4 = Specification.Z0 * sqrt(K);
    Z5 = Specification.Z0 / sqrt(K);
    L2_ = Z4 / w;
    L3_ = Z5 / w;
    C2_ = 1. / (L2_ * w * w);
    C3_ = 1. / (L3_ * w * w);
    C2 += C2_;
    C3 += C3_;
  }

  // Build the circuit and the netlist
  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                      Term, 0, x_1st_term, y_central_branch);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Shunt capacitor
  ComponentInfo Cshunt1(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0,
      x_1st_shunt_C, y_central_branch + 30);
  Cshunt1.val["C"] = num2str(CC, Capacitance);
  Schematic.appendComponent(Cshunt1);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, x_1st_shunt_C, y_central_branch + 80);
  Schematic.appendComponent(Ground);

  Schematic.appendWire(Cshunt1.ID, 0, Ground.ID, 0);

  NodeInfo N_1st_shunt_C(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_1st_shunt_C, y_central_branch);
  Schematic.appendNode(N_1st_shunt_C);

  Schematic.appendWire(Cshunt1.ID, 1, N_1st_shunt_C.ID, 0);
  Schematic.appendWire(TermSpar1.ID, 0, N_1st_shunt_C.ID, 1);

  NodeInfo Nlower(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_split, y_lower_branch);
  Nlower.visible = false; // Not visible. This node is not really needed, but
                          // makes routing easier
  Schematic.appendNode(Nlower);

  NodeInfo Ncentral(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_split, y_central_branch);
  Schematic.appendNode(Ncentral);

  NodeInfo Nupper(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_split, y_upper_branch);
  Nupper.visible = false; // Not visible. This node is not really needed, but
                          // makes routing easier

  Schematic.appendNode(Nupper);

  // Connect the central node with the first shunt capacitor
  Schematic.appendWire(N_1st_shunt_C.ID, 0, Ncentral.ID, 0);

  // Upper branch - Series inductor
  ComponentInfo Lseries1(
      QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor, -90,
      x_1st_series_L, y_upper_branch);
  Lseries1.val["L"] = num2str(L2, Inductance);
  Schematic.appendComponent(Lseries1);

  // Cshunt
  ComponentInfo Cshunt2(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0,
      x_2nd_shunt_C, y_upper_branch + 30);
  Cshunt2.val["C"] = num2str(C2, Capacitance);
  Schematic.appendComponent(Cshunt2);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, x_2nd_shunt_C, y_upper_branch + 80);
  Schematic.appendComponent(Ground);

  NodeInfo N_2nd_Cshunt_up(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_2nd_shunt_C, y_upper_branch);
  Schematic.appendNode(N_2nd_Cshunt_up);

  NodeInfo N_Riso_up(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), x_Riso,
      y_upper_branch);
  Schematic.appendNode(N_Riso_up);

  Schematic.appendWire(Ncentral.ID, 0, Nupper.ID, 0);
  Schematic.appendWire(Nupper.ID, 0, Lseries1.ID, 1);
  Schematic.appendWire(Lseries1.ID, 0, N_2nd_Cshunt_up.ID, 0);
  Schematic.appendWire(Cshunt2.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Cshunt2.ID, 1, N_2nd_Cshunt_up.ID, 0);
  Schematic.appendWire(N_Riso_up.ID, 0, N_2nd_Cshunt_up.ID, 0);

  // Lower branch - Series inductor
  ComponentInfo Lseries2(
      QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor, -90,
      x_1st_series_L, y_lower_branch);
  Lseries2.val["L"] = num2str(L3, Inductance);
  Schematic.appendComponent(Lseries2);

  // Cshunt
  ComponentInfo Cshunt3(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor, 0,
      x_2nd_shunt_C, y_lower_branch + 30);
  Cshunt3.val["C"] = num2str(C3, Capacitance);
  Schematic.appendComponent(Cshunt3);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, x_2nd_shunt_C, y_lower_branch + 80);
  Schematic.appendComponent(Ground);

  NodeInfo N_2nd_Cshunt_down(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      x_2nd_shunt_C, y_lower_branch);
  Schematic.appendNode(N_2nd_Cshunt_down);

  NodeInfo N_Riso_down(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), x_Riso,
      y_lower_branch);
  Schematic.appendNode(N_Riso_down);

  Schematic.appendWire(Nlower.ID, 0, Ncentral.ID, 1);
  Schematic.appendWire(Nlower.ID, 0, Lseries2.ID, 1);
  Schematic.appendWire(Lseries2.ID, 0, N_2nd_Cshunt_down.ID, 0);
  Schematic.appendWire(Cshunt3.ID, 0, Ground.ID, 0);
  Schematic.appendWire(Cshunt3.ID, 1, N_2nd_Cshunt_down.ID, 0);
  Schematic.appendWire(N_Riso_down.ID, 0, N_2nd_Cshunt_down.ID, 0);

  ComponentInfo Risolation(
      QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
      x_Riso, y_central_branch);
  Risolation.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Risolation);

  Schematic.appendWire(Risolation.ID, 0, N_Riso_up.ID, 0);
  Schematic.appendWire(Risolation.ID, 1, N_Riso_down.ID, 0);

  ComponentInfo Ls3, Ls4, Cp4, Cp5;
  if (Specification.OutputRatio.at(0) != 1) {
    // Upper branch
    Ls3.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                  Inductor, -90, x_2nd_series_L, y_upper_branch);
    Ls3.val["L"] = num2str(L2_, Inductance);
    Schematic.appendComponent(Ls3);

    Schematic.appendWire(Ls3.ID, 1, N_2nd_Cshunt_up.ID, 0);

    Cp4.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                  Capacitor, 0, x_3rd_shunt_C, y_upper_branch + 30);
    Cp4.val["C"] = num2str(C2_, Capacitance);
    Schematic.appendComponent(Cp4);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, x_3rd_shunt_C, y_upper_branch + 80);
    Schematic.appendComponent(Ground);

    NodeInfo N_3rd_Shunt_C_up(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        x_3rd_shunt_C, y_upper_branch);
    Schematic.appendNode(N_3rd_Shunt_C_up);

    Schematic.appendWire(Ls3.ID, 0, N_3rd_Shunt_C_up.ID, 0);
    Schematic.appendWire(Cp4.ID, 1, N_3rd_Shunt_C_up.ID, 0);
    Schematic.appendWire(Cp4.ID, 0, Ground.ID, 0);

    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, x_3rd_shunt_C + 50, y_upper_branch);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);

    Schematic.appendWire(N_3rd_Shunt_C_up.ID, 0, TermSpar2.ID, 0);

    // Lower branch
    Ls4.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                  Inductor, -90, x_2nd_series_L, y_lower_branch);
    Ls4.val["L"] = num2str(L3_, Inductance);
    Schematic.appendComponent(Ls4);

    Schematic.appendWire(Ls4.ID, 1, N_2nd_Cshunt_down.ID, 0);

    Cp5.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                  Capacitor, 0, x_3rd_shunt_C, y_lower_branch + 30);
    Cp5.val["C"] = num2str(C3_, Capacitance);
    Schematic.appendComponent(Cp5);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, x_3rd_shunt_C, y_lower_branch + 80);
    Schematic.appendComponent(Ground);

    NodeInfo N_3rd_Shunt_C_down(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        x_3rd_shunt_C, y_lower_branch);
    Schematic.appendNode(N_3rd_Shunt_C_down);

    Schematic.appendWire(Ls4.ID, 0, N_3rd_Shunt_C_down.ID, 0);
    Schematic.appendWire(Cp5.ID, 1, N_3rd_Shunt_C_down.ID, 0);
    Schematic.appendWire(Cp5.ID, 0, Ground.ID, 0);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, x_3rd_shunt_C + 50, y_lower_branch);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);

    Schematic.appendWire(N_3rd_Shunt_C_down.ID, 0, TermSpar3.ID, 0);
  } else {
    // Equal power split
    TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, x_Riso + 50, y_upper_branch);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);
    Schematic.appendWire(TermSpar2.ID, 0, N_2nd_Cshunt_up.ID, 0);

    TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]),
                        Term, 180, x_Riso + 50, y_lower_branch);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);
    Schematic.appendWire(TermSpar3.ID, 0, N_2nd_Cshunt_down.ID, 0);
  }
}

void Wilkinson2Way::buildWilkinson_IdealTL() {

  ComponentInfo TermSpar2, TermSpar3;

  // Define components' location
  setComponentsLocation();

  double lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);

  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // 1st transmission line
  ComponentInfo TL1(QString("TLIN1"), TransmissionLine, 90, TL1_pos);
  TL1.val["Z0"] = num2str(Specification.Z0, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);

  // Central node
  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);
  Schematic.appendWire(TL1.ID, 1, N1.ID, 1);

  // Upper node
  NodeInfo N2(QString("N2"), N2_pos);
  N2.visible = false;
  Schematic.appendNode(N2);
  Schematic.appendWire(N2.ID, 1, N1.ID, 1);

  // Lower node
  NodeInfo N3(QString("N3"), N3_pos);
  N3.visible = false;
  Schematic.appendNode(N3);
  Schematic.appendWire(N3.ID, 1, N1.ID, 1);

  // Upper branch TL
  ComponentInfo TL2(QString("TLIN2"), TransmissionLine, 90, TL2_pos);
  TL2.val["Z0"] = num2str(Z2, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL2.ID, 0, N2.ID, 0);

  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);
  Schematic.appendWire(TL2.ID, 1, N4.ID, 0);

  // Lower branch TL
  ComponentInfo TL3(QString("TLIN3"), TransmissionLine, 90, TL3_pos);
  TL3.val["Z0"] = num2str(Z3, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
  Schematic.appendComponent(TL3);

  Schematic.appendWire(TL3.ID, 0, N3.ID, 0);

  NodeInfo N5(QString("N5"), N5_pos);
  Schematic.appendNode(N5);
  Schematic.appendWire(TL3.ID, 1, N5.ID, 0);

  // Isolation resistor
  ComponentInfo Riso(QString("R1"), Resistor, Riso_pos);
  Riso.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Riso);

  Schematic.appendWire(Riso.ID, 1, N4.ID, 0);
  Schematic.appendWire(Riso.ID, 0, N5.ID, 0);

  if (Specification.OutputRatio.at(0) != 1) {
    // Upper branch matching transmission line
    ComponentInfo TL4(QString("TLIN4"), TransmissionLine, 90, TL4_pos);
    TL4.val["Z0"] = num2str(sqrt(Specification.Z0 * R2), Resistance);
    TL4.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL4);

    TermSpar2.setParams(QString("T2"), Term, 180, Port_out1);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);

    Schematic.appendWire(TL4.ID, 0, N4.ID, 0);
    Schematic.appendWire(TL4.ID, 1, TermSpar2.ID, 0);

    // Lower branch matching transmission line
    ComponentInfo TL5(QString("TLIN5"), TransmissionLine, 90, TL5_pos);
    TL5.val["Z0"] = num2str(sqrt(Specification.Z0 * R3), Resistance);
    TL5.val["Length"] = ConvertLengthFromM(Specification.units, lambda4);
    Schematic.appendComponent(TL5);

    TermSpar3.setParams(QString("T3"), Term, 180, Port_out2);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);

    Schematic.appendWire(TL5.ID, 0, N5.ID, 0);
    Schematic.appendWire(TL5.ID, 1, TermSpar3.ID, 0);

  } else {
    // Equal power split
    TermSpar2.setParams(QString("T2"), Term, 180, Port_out1);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);
    Schematic.appendWire(N4.ID, 1, TermSpar2.ID, 0);

    TermSpar3.setParams(QString("T3"), Term, 180, Port_out2);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);
    Schematic.appendWire(N5.ID, 1, TermSpar3.ID, 0);
  }
}

void Wilkinson2Way::buildWilkinson_Microstrip() {

  ComponentInfo TermSpar2, TermSpar3;

  double lambda4 = SPEED_OF_LIGHT / (4 * Specification.freq);

  // Define components' location
  setComponentsLocation();

  ComponentInfo TermSpar1(QString("T1"), Term, Port_in);
  TermSpar1.val["Z"] = num2str(Specification.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Input quarter-wave microstrip line
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specification.MS_Subs;
  MSL_Input.synthesizeMicrostrip(Specification.Z0, lambda4 * 1e3,
                                 Specification.freq);

  ComponentInfo MLIN1(QString("MLIN1"), MicrostripLine, 90, TL1_pos);
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

  // Central node
  NodeInfo N1(QString("N1"), N1_pos);
  Schematic.appendNode(N1);
  Schematic.appendWire(MLIN1.ID, 1, N1.ID, 1);

  // Upper node
  NodeInfo N2(QString("N2"), N2_pos);
  N2.visible = false;
  Schematic.appendNode(N2);
  Schematic.appendWire(N2.ID, 1, N1.ID, 1);

  // Lower node
  NodeInfo N3(QString("N3"), N3_pos);
  N3.visible = false;
  Schematic.appendNode(N3);
  Schematic.appendWire(N3.ID, 1, N1.ID, 1);

  // Upper branch microstrip line
  MicrostripClass MSL_Upper;
  MSL_Upper.Substrate = Specification.MS_Subs;
  MSL_Upper.synthesizeMicrostrip(Z2, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN2(QString("MLIN2"), MicrostripLine, 90, TL2_pos);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Upper.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Upper.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  Schematic.appendWire(MLIN2.ID, 0, N2.ID, 0);

  NodeInfo N4(QString("N4"), N4_pos);
  Schematic.appendNode(N4);
  Schematic.appendWire(MLIN2.ID, 1, N4.ID, 0);

  // Lower branch microstrip line
  MicrostripClass MSL_Lower;
  MSL_Lower.Substrate = Specification.MS_Subs;
  MSL_Lower.synthesizeMicrostrip(Z3, lambda4 * 1e3, Specification.freq);

  ComponentInfo MLIN3(QString("MLIN3"), MicrostripLine, 90, TL3_pos);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Lower.Results.width);
  MLIN3.val["Length"] =
      ConvertLengthFromM("mm", MSL_Lower.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specification.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specification.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  Schematic.appendWire(MLIN3.ID, 0, N3.ID, 0);

  NodeInfo N5(QString("N5"), N5_pos);
  Schematic.appendNode(N5);
  Schematic.appendWire(MLIN3.ID, 1, N5.ID, 0);

  // Isolation resistor
  ComponentInfo Riso(QString("R1"), Resistor, 0, Riso_pos);
  Riso.val["R"] = num2str(R, Resistance);
  Schematic.appendComponent(Riso);

  Schematic.appendWire(Riso.ID, 1, N4.ID, 0);
  Schematic.appendWire(Riso.ID, 0, N5.ID, 0);

  if (Specification.OutputRatio.at(0) != 1) {
    // Upper branch matching microstrip line
    MicrostripClass MSL_Match_Upper;
    MSL_Match_Upper.Substrate = Specification.MS_Subs;
    double Z_match_upper = sqrt(Specification.Z0 * R2);
    MSL_Match_Upper.synthesizeMicrostrip(Z_match_upper, lambda4 * 1e3,
                                         Specification.freq);

    ComponentInfo MLIN4(QString("MLIN4"), MicrostripLine, 90, TL4_pos);
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

    TermSpar2.setParams(QString("T2"), Term, 180, Port_out1);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);

    Schematic.appendWire(MLIN4.ID, 0, N4.ID, 0);
    Schematic.appendWire(MLIN4.ID, 1, TermSpar2.ID, 0);

    // Lower branch matching microstrip line
    MicrostripClass MSL_Match_Lower;
    MSL_Match_Lower.Substrate = Specification.MS_Subs;
    double Z_match_lower = sqrt(Specification.Z0 * R3);
    MSL_Match_Lower.synthesizeMicrostrip(Z_match_lower, lambda4 * 1e3,
                                         Specification.freq);

    ComponentInfo MLIN5(QString("MLIN5"), MicrostripLine, 90, TL5_pos);
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

    TermSpar3.setParams(QString("T3"), Term, 180, Port_out2);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);

    Schematic.appendWire(MLIN5.ID, 0, N5.ID, 0);
    Schematic.appendWire(MLIN5.ID, 1, TermSpar3.ID, 0);

  } else {
    // Equal power split
    TermSpar2.setParams(QString("T2"), Term, 180, Port_out1);
    TermSpar2.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar2);
    Schematic.appendWire(N4.ID, 1, TermSpar2.ID, 0);

    TermSpar3.setParams(QString("T3"), Term, 180, Port_out2);
    TermSpar3.val["Z"] = num2str(Specification.Z0, Resistance);
    Schematic.appendComponent(TermSpar3);
    Schematic.appendWire(N5.ID, 1, TermSpar3.ID, 0);
  }
}

// Since the components' location is shared between TLIN and MLIN
// implementations, it makes sense to have a common function to set them up
void Wilkinson2Way::setComponentsLocation() {
  // Define components' location

  // Spacing between components
  x_spacing = 50;
  y_spacing = 50;

  // In the schematic of a Wilkinson power combiner there are two output
  // branches and a central feed line. These variables set the y-axis coordinate
  // of these branches;
  int y_central_branch = 0;
  int y_lower_branch = 60;
  int y_upper_branch = -60;

  // Input port
  Port_in = QPoint(0, y_central_branch);

  // TL in front of the input port
  TL1_pos = QPoint(Port_in.x() + x_spacing, Port_in.y());

  // Node at the split
  N1_pos = QPoint(TL1_pos.x() + x_spacing, Port_in.y());

  // Virtual node in front of the upper line
  N2_pos = QPoint(N1_pos.x(), y_upper_branch);

  // Upper TL
  TL2_pos = QPoint(N2_pos.x() + x_spacing, y_upper_branch);

  // Virtual node in front of the bottom line
  N3_pos = QPoint(N1_pos.x(), y_lower_branch);

  // Lower TL
  TL3_pos = QPoint(TL2_pos.x(), y_lower_branch);

  // Node at the output of the upper line
  N4_pos = QPoint(TL2_pos.x() + x_spacing, y_upper_branch);

  // Isolation resistor
  Riso_pos = QPoint(N4_pos.x(), y_central_branch);

  // Node at the output of the lower line
  N5_pos = QPoint(N4_pos.x(), y_lower_branch);

  if (Specification.OutputRatio.at(0) != 1) {
    // In case of unequal power division, the next positions are needed
    // Upper branch TL
    TL4_pos = QPoint(N4_pos.x() + x_spacing, y_upper_branch);

    // Lower branch TL
    TL5_pos = QPoint(TL4_pos.x(), y_lower_branch);

    // Upper S-par term
    Port_out1 = QPoint(TL4_pos.x() + x_spacing, y_upper_branch);

    // Lower S-par term
    Port_out2 = QPoint(Port_out1.x(), y_lower_branch);

  } else {
    // Equal power split

    // Upper S-par term
    Port_out1 = QPoint(N4_pos.x() + x_spacing, y_upper_branch);

    // Lower S-par term
    Port_out2 = QPoint(Port_out1.x(), y_lower_branch);
  }
}
