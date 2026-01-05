/// @file CapacitativeCoupledShuntResonators.cpp
/// @brief Synthesis of capacitive-coupled shunt resonator BPF
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "DirectCoupledFilters.h"

void DirectCoupledFilters::Synthesize_Capacitive_Coupled_Shunt_Resonators() {
  ComponentInfo Cseries, Lshunt, Ground, Cshunt;
  NodeInfo NI, NLeft, NRight, Ncenter;

  int N = Specification.order;
  std::deque<double> L(N), Cp(N);
  double r = gi[N + 1];
  gi.pop_back();
  gi.pop_front();
  double fc = Specification.fc;
  double wc = 2 * M_PI * fc;
  double w0 = wc; // Changed from 1.0 to wc to match Python
  double BW = Specification.bw;
  double Z0 = Specification.ZS;

  std::deque<double> Cr(N); // Starting resonantor capacitances. Calculated from
                            // the fixed inductances (design parameter)

  // Fixed inductors (design parameter)
  for (int i = 0; i < N; i++) {
    if (Specification.resonatorValues.size() == static_cast<unsigned int>(N)) {
      L[i] = Specification.resonatorValues[i];
    } else {
      L[i] = 10e-9; // Default fallback
    }
    // Calculate resonator capacitance - equation [1] Fig. 8.11-1 (1)
    Cr[i] = 1.0 / (L[i] * w0 * w0);
  }

  // Normalized admittances
  double GA = 1.0 / Z0;               // Source admittance
  double GB = 1.0 / Specification.ZL; // Load admittance

  // Fractional bandwidth
  double f1 = fc - BW / 2.0;
  double f2 = fc + BW / 2.0;
  double w1 = 2 * M_PI * f1;
  double w2 = 2 * M_PI * f2;
  double w0_calc = sqrt(w1 * w2); // Geometric mean
  double w = (w2 - w1) / w0_calc; // Fractional bandwidth

  // Calculate coupling coefficients J
  std::deque<double> J(N + 1);

  // J01 - First coupling coefficient - equation [1] Fig. 8.11-1 (2)
  J[0] = sqrt((GA * w0 * Cr[0] * w) / (gi[0] * gi[1]));

  // Intermediate coupling coefficients - equation [1] Fig. 8.11-1 (3)
  for (int i = 1; i < N; i++) {
    J[i] = (w * w0) * sqrt((Cr[i - 1] * Cr[i]) / (gi[i] * gi[i + 1]));
  }

  // Jn,n+1 - Last coupling coefficient - equation [1] Fig. 8.11-1 (4)
  J[N] = sqrt((GB * w0 * Cr[N - 1] * w) / (gi[N - 1] * r));

  // Calculate series coupling capacitances
  std::deque<double> Cs(N + 1);

  // C01 - equation [1] Fig. 8.11-1 (5)
  Cs[0] = J[0] / (w0 * sqrt(1.0 - pow(J[0] / GA, 2)));

  // Ci,i+1 - equation [1] Fig. 8.11-1 (6)
  for (int i = 1; i < N; i++) {
    Cs[i] = J[i] / w0;
  }

  // Cn,n+1 - equation [1] Fig. 8.11-1 (7)
  Cs[N] = J[N] / (w0 * sqrt(1.0 - pow(J[N] / GB, 2)));

  // Excess capacitance
  double C01e =
      Cs[0] / (1.0 + pow(w0 * Cs[0] / GA, 2)); // equation [1] Fig. 8.11-1 (11)
  double Cn_np1e =
      Cs[N] / (1.0 + pow(w0 * Cs[N] / GB, 2)); // equation [1] Fig. 8.11-1 (12)

  // Net shunt capacitances
  Cp[0] = Cr[0] - C01e - Cs[1]; // equation [1] Fig. 8.11-1 (8)

  for (int i = 1; i < N - 1; i++) {
    Cp[i] = Cr[i] - Cs[i] - Cs[i + 1]; // equation [1] Fig. 8.11-1 (9)
  }

  Cp[N - 1] = Cr[N - 1] - Cs[N - 1] - Cn_np1e; // equation [1] Fig. 8.11-1 (10)

  // Build schematic
  int posx = 0, Ni = 0;
  QString ConnectionAux = "";
  double k = Specification.ZS;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;
  // Series capacitor
  Cseries.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                    Capacitor, 90, posx, 0);
  Cseries.val["C"] = num2str(Cs[0], Capacitance);
  Schematic.appendComponent(Cseries);
  Ni++;

  // Wires
  //***** Port to capacitor *****
  Schematic.appendWire(TermSpar1.ID, 0, Cseries.ID, 0);

  QPoint Pos_L, Pos_C, PosCenter;

  for (int k = 0; k < N; k++) {
    // Advance the x-axis index to the new cell
    posx += 25;

    // Set components' positions
    Pos_L = QPoint(posx, 60);         // Inductor
    PosCenter = QPoint(posx + 25, 0); // Central node on the main line
    Pos_C = QPoint(posx + 50, 60);    // Capacitor

    //////////////////////////////////////////////////////////////////////
    // Create nodes
    // Virtual node above the inductor
    NLeft.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        Pos_L.x(), Pos_L.y() - 40);
    NLeft.visible = false;
    Schematic.appendNode(NLeft);

    // Virtual node in between the inductor and the capacitor
    Ncenter.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        PosCenter.x(), Pos_L.y() - 40);
    Ncenter.visible = false;
    Schematic.appendNode(Ncenter);

    // Node in the main line
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        PosCenter);
    Schematic.appendNode(NI);

    // Virtual node above the capacitor
    NRight.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        Pos_C.x(), Pos_C.y() - 40);
    NRight.visible = false;
    Schematic.appendNode(NRight);
    //////////////////////////////////////////////////////////////////////

    // Node to the previous series capacitor
    Schematic.appendWire(NI.ID, 0, Cseries.ID, 1);

    // Shunt resonator
    // Shunt inductor
    Lshunt.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                     Inductor, Pos_L);
    Lshunt.val["L"] = num2str(L[k], Inductance);
    Schematic.appendComponent(Lshunt);

    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, Pos_L.x(), Pos_L.y() + 50);
    Schematic.appendComponent(Ground);

    // Shunt inductor to GND
    Schematic.appendWire(Lshunt.ID, 0, Ground.ID, 0);

    posx += 25;
    // Shunt capacitor
    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        Pos_C);
    Cshunt.val["C"] = num2str(Cp[k], Capacitance);
    Schematic.appendComponent(Cshunt);

    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, Pos_C.x(), Pos_C.y() + 50);
    Schematic.appendComponent(Ground);

    // Shunt capacitor to GND
    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    posx += 50;
    // Series capacitor
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(Cs[k + 1], Capacitance);
    Schematic.appendComponent(Cseries);
    Ni++;

    // Cell wiring

    // Node to the series capacitor
    Schematic.appendWire(NI.ID, 0, Cseries.ID, 0);

    // Central node in th main line to the central virtual line
    Schematic.appendWire(NI.ID, 0, Ncenter.ID, 0);

    // Central virtual node to the left virtual node
    Schematic.appendWire(Ncenter.ID, 0, NLeft.ID, 0);

    // Central virtual node to the right virtual node
    Schematic.appendWire(Ncenter.ID, 0, NRight.ID, 0);

    // Left virtual node to inductor
    Schematic.appendWire(NLeft.ID, 0, Lshunt.ID, 1);

    // Right virtual node to capacitor
    Schematic.appendWire(NRight.ID, 0, Cshunt.ID, 1);
  }

  posx += 40;

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(Specification.ZL, Resistance);
  Schematic.appendComponent(TermSpar2);
  ConnectionAux = TermSpar2.ID;

  Schematic.appendWire(Cseries.ID, 1, TermSpar2.ID, 0);
}
