/// @file InductiveCoupledSeriesResonators.cpp
/// @brief Synthesis of inductive-coupled series resonators filters
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "DirectCoupledFilters.h"

///
/// \internal
///
/// About the synthesis process:
/// 1) Given the resonantor's inductance (capacitance) as design parameter, the
/// capacitance (inductance) is calculated to be tune the resonator at the
/// center frequency.
///
/// 2) Then, the coupling parameters, Jxy, are calculated from the response's
/// lowpass prototype. After that, the shunt coupling capacitors are
/// synthesized (this depends on these Jxy values obtained and the impedance of
/// the terminations).
///
/// 3) Finally, the value of the inductors in the resonators is adjusted
/// depending on the inverters needed.
///
/// Reference: [1] "Microwave Filters, Impedance-Matching Networks, and
/// Coupling Structures", George L. Matthaei, L. Young, E. M. Jones, Artech
/// House pg. 484
///
void DirectCoupledFilters::Synthesize_Inductive_Coupled_Series_Resonators() {
  int N = Specification.order;
  std::deque<double> L(N), C(N), Lr(N), M(N + 1), Lp(N + 1), Ls(N + 2);
  double r = gi[N + 1];
  gi.pop_back();
  gi.pop_front();
  double fc = Specification.fc;
  double wc = 2 * M_PI * fc;
  double w0 = wc; // Changed from 1.0 to wc
  double BW = Specification.bw;
  double Z0 = Specification.ZS;

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

  // Normalized impedances
  double RA = Z0;               // Source impedance
  double RB = Specification.ZL; // Load impedance

  // Fractional bandwidth
  double f1 = fc - BW / 2.0;
  double f2 = fc + BW / 2.0;
  double w1 = 2 * M_PI * f1;
  double w2 = 2 * M_PI * f2;
  double w0_calc = sqrt(w1 * w2); // Geometric mean
  double w = (w2 - w1) / w0_calc; // Fractional bandwidth

  // Calculate coupling coefficients K (similar to J for capacitive case)
  std::deque<double> K(N + 1);

  // K01 - First coupling coefficient
  K[0] = sqrt((RA * w0 * Lr[0] * w) / (gi[0] * gi[1]));

  // Intermediate coupling coefficients
  for (int i = 1; i < N; i++) {
    K[i] = (w * w0) * sqrt((Lr[i - 1] * Lr[i]) / (gi[i] * gi[i + 1]));
  }

  // Kn,n+1 - Last coupling coefficient
  K[N] = sqrt((RB * w0 * Lr[N - 1] * w) / (gi[N - 1] * r));

  // Calculate shunt coupling inductances (mutual inductances)
  Lp[0] = K[0] / (w0 * sqrt(1.0 - pow(K[0] / RA, 2)));

  for (int i = 1; i < N; i++) {
    Lp[i] = K[i] / w0;
  }

  Lp[N] = K[N] / (w0 * sqrt(1.0 - pow(K[N] / RB, 2)));

  // Excess inductance
  double L01e = Lp[0] / (1.0 + pow(w0 * Lp[0] / RA, 2));
  double Ln_np1e = Lp[N] / (1.0 + pow(w0 * Lp[N] / RB, 2));

  // Calculate net series inductances
  // First section (before first resonator)
  Ls[0] = L01e;

  // Series inductance of first resonator
  L[0] = Lr[0] - L01e - Lp[1];

  // Middle resonators
  for (int i = 1; i < N - 1; i++) {
    L[i] = Lr[i] - Lp[i] - Lp[i + 1];
  }

  // Last resonator
  L[N - 1] = Lr[N - 1] - Lp[N - 1] - Ln_np1e;

  // Last section (after last resonator)
  Ls[N + 1] = Ln_np1e;

  // Intermediate series inductances (between resonators)
  for (int i = 1; i <= N; i++) {
    Ls[i] = 0.0; // In the direct-coupled topology, these are effectively zero
  }

  // Create schematic and Qucs netlist
  int posx = 0, Ni = 0;
  QString ConnectionAux = "";
  double k = Specification.ZS;
  ComponentInfo Lseries, Lshunt, Cseries, Ground;
  NodeInfo NI;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar1);
  ConnectionAux = TermSpar1.ID;

  posx += 40;
  // First series inductor (excess inductance from first coupling)
  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, posx, 0);
  Lseries.val["L"] = num2str(Ls[0], Inductance);
  Schematic.appendComponent(Lseries);
  Ni++;

  // Wires
  //***** Port to inductor *****
  Schematic.appendWire(TermSpar1.ID, 0, Lseries.ID, 1);

  posx += 25;
  // First shunt coupling inductor
  Lshunt.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                   Inductor, 0, posx, 50);
  Lshunt.val["L"] = num2str(Lp[0], Inductance);
  Schematic.appendComponent(Lshunt);

  // GND
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, posx, 100);
  Schematic.appendComponent(Ground);

  // Node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      0);
  Schematic.appendNode(NI);

  Schematic.appendWire(Lseries.ID, 0, NI.ID, 0);
  Schematic.appendWire(NI.ID, 0, Lshunt.ID, 1);
  Schematic.appendWire(Lshunt.ID, 0, Ground.ID, 0);

  for (int k = 0; k < N; k++) {
    // Series inductor (resonator inductance)
    posx += 25;
    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, 0);
    Lseries.val["L"] = num2str(L[k], Inductance);
    Schematic.appendComponent(Lseries);
    Ni++;

    // Series capacitor (resonator capacitance)
    posx += 40;
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(C[k], Capacitance);
    Schematic.appendComponent(Cseries);
    Ni++;

    posx += 25;
    // Shunt inductor coupling
    Lshunt.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                     Inductor, 0, posx, 50);
    Lshunt.val["L"] = num2str(Lp[k + 1], Inductance);
    Schematic.appendComponent(Lshunt);

    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, 100);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(NI.ID, 0, Lseries.ID,
                         1); // Series inductor to the previous section
    Schematic.appendWire(Lseries.ID, 0, Cseries.ID,
                         0); // Series inductor to the series capacitor

    // Node to connect the shunt coupling inductor to the series resonator
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        0);
    Schematic.appendNode(NI);

    Schematic.appendWire(NI.ID, 0, Cseries.ID,
                         1); // Series capacitor to node
    Schematic.appendWire(NI.ID, 0, Lshunt.ID,
                         1); // Node to the shunt coupling inductor
    Schematic.appendWire(Ground.ID, 0, Lshunt.ID,
                         0); // Shunt inductor to ground
  }

  posx += 25;
  // Last series inductor (excess inductance from last coupling)
  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, posx, 0);
  Lseries.val["L"] = num2str(Ls[N + 1], Inductance);
  Schematic.appendComponent(Lseries);

  Schematic.appendWire(NI.ID, 0, Lseries.ID,
                       1); // Last series inductor to the previous node

  posx += 50;
  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(Specification.ZL, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Last series inductor to the port
  Schematic.appendWire(TermSpar2.ID, 0, Lseries.ID, 0);
}
