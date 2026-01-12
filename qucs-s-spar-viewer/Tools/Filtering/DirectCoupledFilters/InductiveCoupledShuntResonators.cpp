/// @file InductiveCoupledShuntResonators.cpp
/// @brief Synthesis of inductive-coupled shunt resonators BPF
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "DirectCoupledFilters.h"

void DirectCoupledFilters::Synthesize_Inductive_Coupled_Shunt_Resonators() {
  ComponentInfo Lseries, Cshunt, Ground, Lshunt;
  NodeInfo NI, NLeft, NRight, Ncenter;

  int N = Specification.order;
  std::deque<double> C(N), Lp(N);
  double r = gi[N + 1];
  gi.pop_back();
  gi.pop_front();
  double fc = Specification.fc;
  double wc = 2 * M_PI * fc;
  double w0 = wc;
  double BW = Specification.bw;
  double Z0 = Specification.ZS;

  std::deque<double> Lr(N); // Physical resonator inductance values

  // Fixed capacitors (design parameter)
  for (int i = 0; i < N; i++) {
    if (Specification.resonatorValues.size() == static_cast<unsigned int>(N)) {
      C[i] = Specification.resonatorValues[i];
    } else {
      C[i] = 1e-12; // Default fallback
    }
    // Calculate resonator inductance
    Lr[i] = 1.0 / (C[i] * w0 * w0);
  }

  // Source and load impedances
  double RS = Z0;               // Source impedance
  double RL = Specification.ZL; // Load impedance

  // Convert to admittances
  double GA = 1.0 / RS;
  double GB = 1.0 / RL;

  // Fractional bandwidth calculation
  double f1 = fc - BW / 2.0;
  double f2 = fc + BW / 2.0;
  double w1 = 2 * M_PI * f1;
  double w2 = 2 * M_PI * f2;
  double w0_calc = sqrt(w1 * w2); // Geometric mean
  double w = (w2 - w1) / w0_calc; // Fractional bandwidth

  // Calculate coupling coefficients J
  std::deque<double> J(N + 1);

  // J01 - First coupling coefficient - equation [1] Fig. 8.11-1 (2)
  J[0] = sqrt((GA * w0 * C[0] * w) / (gi[0] * gi[1]));

  // Intermediate coupling coefficients - equation [1] Fig. 8.11-1 (3)
  for (int i = 1; i < N; i++) {
    J[i] = (w * w0) * sqrt((C[i - 1] * C[i]) / (gi[i] * gi[i + 1]));
  }

  // Jn,n+1 - Last coupling coefficient - equation [1] Fig. 8.11-1 (4)
  J[N] = sqrt((GB * w0 * C[N - 1] * w) / (gi[N - 1] * r));

  // Calculate series coupling inductances
  std::deque<double> Ls(N + 1);

  // L01 - equation [1] Fig. 8.11-1 (5)
  Ls[0] = sqrt(1.0 - pow(J[0] / GA, 2)) / (w0 * J[0]);

  // Li,i+1 - equation [1] Fig. 8.11-1 (6)
  for (int i = 1; i < N; i++) {
    Ls[i] = 1.0 / (J[i] * w0);
  }

  // Ln,n+1 - equation [1] Fig. 8.11-1 (7)
  Ls[N] = sqrt(1.0 - pow(J[N] / GB, 2)) / (w0 * J[N]);

  // Excess inductance (note: these are ADDITIONS to resonator inductance)
  // L01e - equation [1] Fig. 8.11-1 (11)
  double L01e = Ls[0] * (1.0 + pow(1.0 / (GA * w0 * Ls[0]), 2));

  // Ln_np1e - equation [1] Fig. 8.11-1 (12)
  double Ln_np1e = Ls[N] * (1.0 + pow(1.0 / (GB * w0 * Ls[N]), 2));

  // Net shunt inductances (using reciprocal formula for parallel inductances)
  // Lp[0] - equation [1] Fig. 8.11-1 (8)
  Lp[0] = 1.0 / (1.0 / Lr[0] - 1.0 / L01e - 1.0 / Ls[1]);

  // Intermediate inductances - equation [1] Fig. 8.11-1 (9)
  for (int i = 1; i < N - 1; i++) {
    Lp[i] = 1.0 / (1.0 / Lr[i] - 1.0 / Ls[i] - 1.0 / Ls[i + 1]);
  }

  // Last inductance - equation [1] Fig. 8.11-1 (10)
  Lp[N - 1] = 1.0 / (1.0 / Lr[N - 1] - 1.0 / Ls[N - 1] - 1.0 / Ln_np1e);

  // Build schematic
  int posx = 0, Ni = 0;
  QString ConnectionAux = "";
  double k = Specification.ZS;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;
  // Series inductor
  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, posx, 0);
  Lseries.val["L"] = num2str(Ls[0], Inductance);
  Schematic.appendComponent(Lseries);
  Ni++;

  // Wires
  //***** Port to inductor *****
  Schematic.appendWire(TermSpar1.ID, 0, Lseries.ID, 1);

  QPoint Pos_L, Pos_C, PosCenter;

  for (int k = 0; k < N; k++) {
    // Advance the x-axis index to the new cell
    posx += 25;

    // Set components' positions
    Pos_C = QPoint(posx, 60);         // Capacitor
    PosCenter = QPoint(posx + 25, 0); // Central node on the main line
    Pos_L = QPoint(posx + 50, 60);    // Inductor

    //////////////////////////////////////////////////////////////////////
    // Create nodes
    // Virtual node above the capacitor
    NLeft.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        Pos_C.x(), Pos_C.y() - 40);
    NLeft.visible = false;
    Schematic.appendNode(NLeft);

    // Virtual node in between the capacitor and the inductor
    Ncenter.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        PosCenter.x(), Pos_C.y() - 40);
    Ncenter.visible = false;
    Schematic.appendNode(Ncenter);

    // Node in the main line
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        PosCenter);
    Schematic.appendNode(NI);

    // Virtual node above the inductor
    NRight.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        Pos_L.x(), Pos_L.y() - 40);
    NRight.visible = false;
    Schematic.appendNode(NRight);
    //////////////////////////////////////////////////////////////////////

    // Node to the previous series inductor
    Schematic.appendWire(NI.ID, 0, Lseries.ID, 0);

    // Shunt resonator
    // Shunt capacitor
    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        Pos_C);
    Cshunt.val["C"] = num2str(C[k], Capacitance);
    Schematic.appendComponent(Cshunt);

    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, Pos_C.x(), Pos_C.y() + 50);
    Schematic.appendComponent(Ground);

    // Shunt capacitor to GND
    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    posx += 25;
    // Shunt inductor
    Lshunt.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                     Inductor, Pos_L);
    Lshunt.val["L"] = num2str(Lp[k], Inductance);
    Schematic.appendComponent(Lshunt);

    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, Pos_L.x(), Pos_L.y() + 50);
    Schematic.appendComponent(Ground);

    // Shunt inductor to GND
    Schematic.appendWire(Lshunt.ID, 0, Ground.ID, 0);

    posx += 50;
    // Series inductor
    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, 0);
    Lseries.val["L"] = num2str(Ls[k + 1], Inductance);
    Schematic.appendComponent(Lseries);
    Ni++;

    // Cell wiring

    // Node to the series inductor
    Schematic.appendWire(NI.ID, 0, Lseries.ID, 1);

    // Central node in the main line to the central virtual line
    Schematic.appendWire(NI.ID, 0, Ncenter.ID, 0);

    // Central virtual node to the left virtual node
    Schematic.appendWire(Ncenter.ID, 0, NLeft.ID, 0);

    // Central virtual node to the right virtual node
    Schematic.appendWire(Ncenter.ID, 0, NRight.ID, 0);

    // Left virtual node to capacitor
    Schematic.appendWire(NLeft.ID, 0, Cshunt.ID, 1);

    // Right virtual node to inductor
    Schematic.appendWire(NRight.ID, 0, Lshunt.ID, 1);
  }

  posx += 40;

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(Specification.ZL, Resistance);
  Schematic.appendComponent(TermSpar2);
  ConnectionAux = TermSpar2.ID;

  Schematic.appendWire(Lseries.ID, 0, TermSpar2.ID, 0);
}
