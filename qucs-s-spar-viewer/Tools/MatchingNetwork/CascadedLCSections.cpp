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

#include "CascadedLCSections.h"

CascadedLCSections::CascadedLCSections() {
  NumberOfSections = 3; // Default number of sections
}

CascadedLCSections::CascadedLCSections(MatchingNetworkDesignParameters AS,
                                       double freq) {
  Specs            = AS;
  NumberOfSections = 3; // Default, should be configurable
  f_match          = freq;
}

CascadedLCSections::~CascadedLCSections() {}

// Reference: Inder J. Bahl. "Fundamentals of RF and microwave transistor
// amplifiers". John Wiley and Sons. 2009. Pages 169 - 170
void CascadedLCSections::synthesize() {

  if (Specs.Solution == 1) {
    // Lowpass approach
    CreateLowpassSolution();
  } else {
    // Highpass approach
    CreateHighpassSolution();
  }
}

void CascadedLCSections::CreateHighpassSolution() {

  // Port 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Calculate load impedance from reflection coefficient
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double RS = Z0;
  double w  = 2 * M_PI * f_match;

  // Check if load has resistive part
  if (RL == 0) {
    // Cannot match reactive-only loads - could throw exception or handle error
    // For now, we'll return without creating components
    return;
  }

  // Warning: Only real part will be matched if XL != 0
  // In practice, you might want to add a series reactive component first

  double R1, R2;

  // Design equations are for RS > RL case
  if (RL > RS) {
    R1 = RL;
    R2 = RS;
  } else {
    R1 = RS;
    R2 = RL;
  }

  int N = Specs.NSections;
  std::vector<double> Q(N), C(N), L(N), R(N);

  ComponentInfo Lshunt, Cseries, gnd;
  NodeInfo node;

  int x_pos     = 50; // Starting x position
  int x_spacing = 80; // Spacing between sections

  QString previous_node;

  // Highpass approach
  for (int i = 0; i < N; i++) {
    R[i] = pow(R1, (double)(N - i - 1) / N) * pow(R2, (double)(i + 1) / N);

    if (i == 0) {
      Q[i] = sqrt(R1 / R[0] - 1);
      L[i] = R1 / (w * Q[i]);
    } else {
      Q[i] = sqrt(R[i - 1] / R[i] - 1);
      L[i] = R[i - 1] / (w * Q[i]);
    }
    C[i] = 1 / (Q[i] * R[i] * w);
  }

  // Generate cascaded LC sections
  for (int i = 0; i < N; i++) {

    if (RS > RL) {
      // Lshunt
      Lshunt.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          0, x_pos + i * x_spacing, 50);
      Lshunt.val["L"] = num2str(L[i], Inductance);
      Schematic.appendComponent(Lshunt);

      gnd.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                    GND, 0, x_pos + i * x_spacing, 100);
      Schematic.appendComponent(gnd);

      // Cseries
      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, -90, x_pos + i * x_spacing + x_spacing / 2, 0);
      Cseries.val["C"] = num2str(C[i], Capacitance);
      Schematic.appendComponent(Cseries);

      node.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          x_pos + i * x_spacing, 0);
      Schematic.appendNode(node);

      // Connections
      if (previous_node.isEmpty()) {
        Schematic.appendWire(TermSpar1.ID, 0, node.ID, 0);
      } else { // Then it's the source port
        Schematic.appendWire(previous_node, 0, node.ID, 0);
      }
      Schematic.appendWire(node.ID, 0, Lshunt.ID, 1);
      Schematic.appendWire(Lshunt.ID, 0, gnd.ID, 0);
      Schematic.appendWire(node.ID, 0, Cseries.ID, 1);
      previous_node = Cseries.ID;

    } else {
      // RS < RL

      // Cseries
      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, -90, x_pos + i * x_spacing + x_spacing / 2, 0);
      Cseries.val["C"] = num2str(C[N - i - 1], Capacitance);
      Schematic.appendComponent(Cseries);

      node.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          x_pos + (i + 1) * x_spacing, 0);
      Schematic.appendNode(node);

      // Cshunt
      Lshunt.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          0, x_pos + (i + 1) * x_spacing, 50);
      Lshunt.val["L"] = num2str(L[N - i - 1], Inductance);
      Schematic.appendComponent(Lshunt);

      gnd.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                    GND, 0, x_pos + (i + 1) * x_spacing, 100);
      Schematic.appendComponent(gnd);

      Schematic.appendWire(Lshunt.ID, 0, gnd.ID, 0);

      // Connections
      if (previous_node.isEmpty()) {
        Schematic.appendWire(TermSpar1.ID, 0, Cseries.ID, 1);
      } else { // Then it's the source port
        Schematic.appendWire(previous_node, 0, Cseries.ID, 1);
      }
      Schematic.appendWire(Cseries.ID, 0, node.ID, 0);
      Schematic.appendWire(node.ID, 0, Lshunt.ID, 1);
      Schematic.appendWire(Lshunt.ID, 0, gnd.ID, 0);

      previous_node = node.ID;
    }
  }

  // Create load resistor at the end
  ComponentInfo Zload;
  Zload.setParams(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, x_pos + (N + 1) * x_spacing, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, x_pos + (N + 1) * x_spacing, 100);
  Schematic.appendComponent(GND_ZL);

  // Connect input and output
  if (RS < RL) {
    Schematic.appendWire(Zload.ID, 1, previous_node, 1);
  } else {
    Schematic.appendWire(Zload.ID, 1, previous_node, 0);
  }

  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}

void CascadedLCSections::CreateLowpassSolution() {
  // Port 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  // Calculate load impedance from reflection coefficient
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double RS = Z0;
  double w  = 2 * M_PI * f_match;

  // Check if load has resistive part
  if (RL == 0) {
    // Cannot match reactive-only loads - could throw exception or handle error
    // For now, we'll return without creating components
    return;
  }

  // Warning: Only real part will be matched if XL != 0
  // In practice, you might want to add a series reactive component first

  double R1, R2;

  // Design equations are for RS > RL case
  if (RL > RS) {
    R1 = RL;
    R2 = RS;
  } else {
    R1 = RS;
    R2 = RL;
  }

  int N = Specs.NSections;
  std::vector<double> Q(N), C(N), L(N), R(N);

  ComponentInfo Cshunt, Lseries, gnd;
  NodeInfo node;

  int x_pos     = 50; // Starting x position
  int x_spacing = 80; // Spacing between sections

  QString previous_node;

  // Lowpass approach
  for (int i = 0; i < N; i++) {
    R[i] = pow(R1, (double)(N - i - 1) / N) * pow(R2, (double)(i + 1) / N);

    if (i == 0) {
      Q[i] = sqrt(R1 / R[0] - 1);
      C[i] = Q[i] / (w * R1);
    } else {
      Q[i] = sqrt(R[i - 1] / R[i] - 1);
      C[i] = Q[i] / (w * R[i - 1]);
    }

    L[i] = Q[i] * R[i] / w;
  }

  // Generate cascaded LC sections
  for (int i = 0; i < N; i++) {

    if (RS > RL) {
      // Cshunt
      Cshunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0, x_pos + i * x_spacing, 50);
      Cshunt.val["C"] = num2str(C[i], Capacitance);
      Schematic.appendComponent(Cshunt);

      gnd.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                    GND, 0, x_pos + i * x_spacing, 100);
      Schematic.appendComponent(gnd);

      // Lseries
      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, x_pos + i * x_spacing + x_spacing / 2, 0);
      Lseries.val["L"] = num2str(L[i], Inductance);
      Schematic.appendComponent(Lseries);

      node.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          x_pos + i * x_spacing, 0);
      Schematic.appendNode(node);

      // Connections
      if (previous_node.isEmpty()) {
        Schematic.appendWire(TermSpar1.ID, 0, node.ID, 0);
      } else { // Then it's the source port
        Schematic.appendWire(previous_node, 0, node.ID, 0);
      }
      Schematic.appendWire(node.ID, 0, Cshunt.ID, 1);
      Schematic.appendWire(Cshunt.ID, 0, gnd.ID, 0);
      Schematic.appendWire(node.ID, 0, Lseries.ID, 1);
      previous_node = Lseries.ID;

    } else {
      // RS < RL

      // Lseries
      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, x_pos + i * x_spacing + x_spacing / 2, 0);
      Lseries.val["L"] = num2str(L[N - i - 1], Inductance);
      Schematic.appendComponent(Lseries);

      node.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          x_pos + (i + 1) * x_spacing, 0);
      Schematic.appendNode(node);

      // Cshunt
      Cshunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0, x_pos + (i + 1) * x_spacing, 50);
      Cshunt.val["C"] = num2str(C[N - i - 1], Capacitance);
      Schematic.appendComponent(Cshunt);

      gnd.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                    GND, 0, x_pos + (i + 1) * x_spacing, 100);
      Schematic.appendComponent(gnd);

      Schematic.appendWire(Cshunt.ID, 0, gnd.ID, 0);

      // Connections
      if (previous_node.isEmpty()) {
        Schematic.appendWire(TermSpar1.ID, 0, Lseries.ID, 1);
      } else { // Then it's the source port
        Schematic.appendWire(previous_node, 0, Lseries.ID, 1);
      }
      Schematic.appendWire(Lseries.ID, 0, node.ID, 0);
      Schematic.appendWire(node.ID, 0, Cshunt.ID, 1);
      Schematic.appendWire(Cshunt.ID, 0, gnd.ID, 0);

      previous_node = node.ID;
    }
  }

  // Create load resistor at the end
  ComponentInfo Zload;
  Zload.setParams(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, x_pos + (N + 1) * x_spacing, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, x_pos + (N + 1) * x_spacing, 100);
  Schematic.appendComponent(GND_ZL);

  // Connect input and output
  if (RS < RL) {
    Schematic.appendWire(Zload.ID, 1, previous_node, 1);
  } else {
    Schematic.appendWire(Zload.ID, 1, previous_node, 0);
  }

  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}
