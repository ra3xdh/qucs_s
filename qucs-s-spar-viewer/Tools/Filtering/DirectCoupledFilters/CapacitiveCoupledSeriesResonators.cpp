/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

/**
 * @file CapacitiveCoupledSeriesResonators.cpp
 * @brief Synthesis of capacitive-coupled series resonators filters
 * @author Andrés Martínez Mera
 * @date 2025
 */

#include "DirectCoupledFilters.h"

///
/// \internal
///
/// About the synthesis process:
/// 1) Given the resonator's inductance as design parameter, the
/// capacitance is calculated to tune the resonator at the center frequency.
///
/// 2) Then, the coupling parameters, Kxy, are calculated from the response's
/// lowpass prototype. After that, the shunt coupling capacitors are
/// synthesized (capacitive inverters).
///
/// 3) The negative capacitances from the inverters are absorbed into the
/// series resonator capacitances.
///
/// 4) For the source and load ports, series-to-parallel conversion is applied
/// to the first and last inverters, and impedance matching is achieved using
/// a reactive element (capacitor or inductor as specified by the user).
///
/// References:
/// [1] "Microwave Filters, Impedance-Matching Networks, and Coupling
/// Structures", George L. Matthaei, L. Young, E. M. Jones, Artech House pg. 484
/// [2] "Filter Handbook", Anatol I. Zverev. John Wiley and Sons, 1967,
/// pages 562-563

void DirectCoupledFilters::Synthesize_Capacitive_Coupled_Series_Resonators() {
  ComponentInfo Lseries, Cseries, Cshunt, Ground, MatchComponent;
  NodeInfo NI;

  int N = Specification.order;
  std::deque<double> Lres(N), Cres(N);
  double r = gi[N + 1];
  gi.pop_back();
  gi.pop_front();
  double fc = Specification.fc;
  double wc = 2 * M_PI * fc;
  double w0 = wc;
  double BW = Specification.bw;

  // Fixed inductors (design parameter)
  for (int i = 0; i < N; i++) {
    if (Specification.resonatorValues.size() == static_cast<unsigned int>(N)) {
      Lres[i] = Specification.resonatorValues[i];
    } else {
      Lres[i] = 10e-9; // Default fallback
    }
    // Calculate resonator capacitance - equation [1] Fig. 8.11-1 (1)
    Cres[i] = 1.0 / (Lres[i] * w0 * w0);
  }

  // Source and load impedances
  double RS = Specification.ZS;
  double RL = Specification.ZL;

  // Fractional bandwidth calculation
  double f1 = fc - BW / 2.0;
  double f2 = fc + BW / 2.0;
  double w1 = 2 * M_PI * f1;
  double w2 = 2 * M_PI * f2;
  double w0_calc = sqrt(w1 * w2); // Geometric mean
  double w = (w2 - w1) / w0_calc; // Fractional bandwidth

  // Calculate coupling coefficients K
  std::deque<double> K(N + 1);

  // K01 - First coupling coefficient - equation [1] Fig. 8.11-2 (2)
  K[0] = sqrt((RS * w0 * Lres[0] * w) / (gi[0] * gi[1]));

  // Intermediate coupling coefficients - equation [1] Fig. 8.11-2 (3)
  for (int i = 1; i < N; i++) {
    K[i] = (w * w0) * sqrt((Lres[i - 1] * Lres[i]) / (gi[i] * gi[i + 1]));
  }

  // Kn,n+1 - Last coupling coefficient - equation [1] Fig. 8.11-2 (4)
  K[N] = sqrt((RL * w0 * Lres[N - 1] * w) / (gi[N - 1] * r));

  // Calculate capacitive inverters - [2] Fig. 10.83 b)
  std::deque<double> Cinv(N + 1);
  for (int i = 0; i <= N; i++) {
    Cinv[i] = 1.0 / (w0 * K[i]);
  }

  // Absorb negative capacitances into resonator capacitances
  // Series equivalent of Cres[i] with two inverter capacitances
  for (int i = 0; i < N; i++) {
    Cres[i] = 1.0 / ((-1.0 / Cinv[i]) + (1.0 / Cres[i]) + (-1.0 / Cinv[i + 1]));
  }

  // Port matching components
  double Cmatch_source = 0.0;
  double Cmatch_load = 0.0;

  // Get port matching types (default to capacitor if not specified)
  std::string sourceMatchType = "C"; // Default
  std::string loadMatchType = "C";   // Default

  // 1. Source port - series to parallel conversion
  double Xseries_source = -1.0 / (w0 * Cinv[0]); // Negative reactance
  double Rp_source = (RS * RS + Xseries_source * Xseries_source) / RS;
  double Xp_source =
      (RS * RS + Xseries_source * Xseries_source) / Xseries_source;

  double C0 = 1.0 / (w0 * Xp_source); // Shunt equivalent (negative)
  Cinv[0] += C0;                      // Absorb into first shunt capacitor

  // Compensate impedance difference with reactive element
  Cmatch_source = 1.0 / (w0 * (Rp_source - RS));

  // 2. Load port - series to parallel conversion
  double Xseries_load = -1.0 / (w0 * Cinv[N]); // Negative reactance
  double Rp_load = (RL * RL + Xseries_load * Xseries_load) / RL;
  double Xp_load = (RL * RL + Xseries_load * Xseries_load) / Xseries_load;

  double Clast = 1.0 / (w0 * Xp_load); // Shunt equivalent (negative)
  Cinv[N] += Clast;                    // Absorb into last shunt capacitor

  // Compensate impedance difference with reactive element
  Cmatch_load = 1.0 / (w0 * (Rp_load - RL));

  // Build schematic
  int posx = 0;

  // Source port
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(RS, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 40;

  // Source matching component
  MatchComponent.setParams(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
      90, posx, 0);
  MatchComponent.val["C"] = num2str(Cmatch_source, Capacitance);
  Schematic.appendComponent(MatchComponent);

  // Wire from port to matching component
  Schematic.appendWire(TermSpar1.ID, 0, MatchComponent.ID, 0);

  posx += 25;

  // Main filter loop
  for (int k = 0; k < N; k++) {
    // Node position
    QPoint NodePos(posx, 0);
    QPoint ShuntPos(posx, 60);

    // Create node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        NodePos);
    Schematic.appendNode(NI);

    // Connect to previous component
    if (k == 0) {
      Schematic.appendWire(MatchComponent.ID, 1, NI.ID, 0);
    } else {
      Schematic.appendWire(Cseries.ID, 1, NI.ID, 0);
    }

    // Shunt coupling capacitor
    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        ShuntPos);
    Cshunt.val["C"] = num2str(Cinv[k], Capacitance);
    Schematic.appendComponent(Cshunt);

    // Ground
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, ShuntPos.x(), ShuntPos.y() + 50);
    Schematic.appendComponent(Ground);

    // Connect shunt capacitor
    Schematic.appendWire(NI.ID, 0, Cshunt.ID, 1);
    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    posx += 25;

    // Series resonator inductor
    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, 0);
    Lseries.val["L"] = num2str(Lres[k], Inductance);
    Schematic.appendComponent(Lseries);

    // Connect to node
    Schematic.appendWire(NI.ID, 0, Lseries.ID, 1);

    posx += 40;

    // Series resonator capacitor
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(Cres[k], Capacitance);
    Schematic.appendComponent(Cseries);

    // Connect to inductor
    Schematic.appendWire(Lseries.ID, 0, Cseries.ID, 0);

    posx += 25;
  }

  // Last shunt coupling capacitor
  QPoint LastNodePos(posx, 0);
  QPoint LastShuntPos(posx, 60);

  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
      LastNodePos);
  Schematic.appendNode(NI);

  // Connect to last series capacitor
  Schematic.appendWire(Cseries.ID, 1, NI.ID, 0);

  // Last shunt capacitor
  Cshunt.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                   Capacitor, LastShuntPos);
  Cshunt.val["C"] = num2str(Cinv[N], Capacitance);
  Schematic.appendComponent(Cshunt);

  // Ground
  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND,
                   0, LastShuntPos.x(), LastShuntPos.y() + 50);
  Schematic.appendComponent(Ground);

  // Connect last shunt capacitor
  Schematic.appendWire(NI.ID, 0, Cshunt.ID, 1);
  Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

  posx += 25;

  // Load matching component
  MatchComponent.setParams(
      QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
      90, posx, 0);
  MatchComponent.val["C"] = num2str(Cmatch_load, Capacitance);
  Schematic.appendComponent(MatchComponent);

  // Connect to last node
  Schematic.appendWire(NI.ID, 0, MatchComponent.ID, 0);

  posx += 50;

  // Load port
  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(RL, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Connect to load
  Schematic.appendWire(MatchComponent.ID, 1, TermSpar2.ID, 0);
}
