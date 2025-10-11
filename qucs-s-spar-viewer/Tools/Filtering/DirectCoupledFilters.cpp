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

#include "DirectCoupledFilters.h"

DirectCoupledFilters::DirectCoupledFilters() {}

DirectCoupledFilters::DirectCoupledFilters(FilterSpecifications FS) {
  Specification = FS;
}

DirectCoupledFilters::~DirectCoupledFilters() {}

void DirectCoupledFilters::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  gi = LP_coeffs.getCoefficients();
  if (Specification.DC_Coupling == CapacitativeCoupledShuntResonators) {
    Synthesize_Capacitative_Coupled_Shunt_Resonators();
  }
  if (Specification.DC_Coupling == InductiveCoupledSeriesResonators) {
    Synthesize_Inductive_Coupled_Series_Resonators();
  }
}

void DirectCoupledFilters::Synthesize_Capacitative_Coupled_Shunt_Resonators() {
  WireInfo WI;
  ComponentInfo Cseries, Lshunt, Ground, Cshunt;
  NodeInfo NI;

  int N = Specification.order;
  std::deque<double> L(N), Cp(N);
  double r = gi[N + 1];
  gi.pop_back();
  gi.pop_front();
  double fc = Specification.fc;
  double wc = 2 * M_PI * fc;
  double BW = Specification.bw;
  double Z0 = Specification.ZS;

  for (int i = 0; i < N; i++) {
    L[i] = 10e-9;
  }

  double R1  = 1;
  double RN  = Specification.ZL / Z0;
  double w0  = 1.0;
  double f1  = (fc - BW) / (2 * M_PI * fc);
  double f2  = (fc + BW) / (2 * M_PI * fc);
  double f1d = w0 / (2 * M_PI);
  double f0  = w0 / (2 * M_PI);
  double wd  = ((f0 / f1) - (f0 / f2)) * (f0 / f1d);

  std::deque<double> Lrk(N), Crk(N), Cs(N + 1);

  for (int i = 0; i < N; i++) {
    Lrk[i] = ((wc * L[i]) / Z0);
    Crk[i] = 1. / ((w0 * w0) * Lrk[i]);
  }

  // Series capacitors
  Cs[0] = (1 / w0) *
          sqrt((wd * Crk[0] / (R1 * gi[0]) / (1 - (wd * Crk[0] * R1 / gi[0]))));
  for (int i = 1; i < N; i++) {
    Cs[i] = wd * sqrt((Crk[i - 1] * Crk[i]) / (gi[i - 1] * gi[i]));
  }
  Cs[N] = (1 / w0) * sqrt((wd * Crk[N - 1] * r / (RN * gi[N - 1])) /
                          (1 - (wd * Crk[N - 1] * RN / gi[N - 1])));

  double Cs1_   = Cs[0] / (1 + pow(w0 * Cs[0] * R1, 2));
  double Csn_n1 = Cs[N] / (1 + pow(w0 * Cs[N] * RN, 2));

  Cp[0] = Crk[0] - Cs1_ - Cs[1];
  for (int i = 1; i < N - 1; i++) {
    Cp[i] = Crk[i] - Cs[i] - Cs[i + 1];
  }
  Cp[N - 1] = Crk[N - 1] - Csn_n1 - Cs[N - 1];

  // Scale
  for (int i = 0; i < N + 1; i++) {
    if (i < N) {
      Cp[i] = Cp[i] / (2 * M_PI * fc * Z0);
    }
    Cs[i] = Cs[i] / (2 * M_PI * fc * Z0);
  }

  // Build schematic
  int posx = 0, Ni = 0;
  QString ConnectionAux = "";
  double k              = Specification.ZS;

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

  for (int k = 0; k < N; k++) {
    posx += 50;
    // Shunt resonator
    // Shunt inductor
    Lshunt.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                     Inductor, 0, posx, 50);
    Lshunt.val["L"] = num2str(L[k], Inductance);
    Schematic.appendComponent(Lshunt);

    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, 100);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Lshunt.ID, 0, Ground.ID, 0);

    posx += 25;

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        0);
    Schematic.appendNode(NI);

    // Node to the previous series capacitor
    Schematic.appendWire(NI.ID, 0, Cseries.ID, 1);

    posx += 25;
    // Shunt capacitor
    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, posx, 50);
    Cshunt.val["C"] = num2str(Cp[k], Capacitance);
    Schematic.appendComponent(Cshunt);

    // GND
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, 100);
    Schematic.appendComponent(Ground);

    posx += 50;
    // Series capacitor
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(Cs[k + 1], Capacitance);
    Schematic.appendComponent(Cseries);
    Ni++;

    Schematic.appendWire(NI.ID, 0, Lshunt.ID, 1);
    Schematic.appendWire(NI.ID, 0, Cshunt.ID, 1);
    Schematic.appendWire(NI.ID, 0, Cseries.ID, 0);
    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);
  }

  posx += 50;

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(Specification.ZL, Resistance);
  Schematic.appendComponent(TermSpar2);
  ConnectionAux = TermSpar2.ID;

  Schematic.appendWire(Cseries.ID, 1, TermSpar2.ID, 0);
}

void DirectCoupledFilters::Synthesize_Inductive_Coupled_Series_Resonators() {
  int N = Specification.order;
  std::deque<double> L(N + 2), Lrk(N + 2), Crk(N), M(N + 1), Lp(N + 1);
  double r = gi[N + 1];
  gi.pop_back();
  gi.pop_front();
  double fc = Specification.fc;
  double wc = 2 * M_PI * fc;
  double BW = Specification.bw;
  double Z0 = Specification.ZS;
  double R1 = 1;
  double RN = Specification.ZL / Z0;

  for (int i = 0; i < N + 2; i++) {
    L[i] = 10e-9;
  }

  double w0  = 1.0;
  double f1  = (fc - BW) / (2 * M_PI * fc);
  double f2  = (fc + BW) / (2 * M_PI * fc);
  double f1d = 1 / (2 * M_PI);
  double f0  = w0 / (2 * M_PI);
  double w_  = ((f0 / f1) - (f0 / f2)) * (f0 / f1d);

  for (int i = 0; i < N + 2; i++) {
    Lrk[i] = (wc * L[i]) / Z0;
  }

  Crk[0] = (1 + (w_ * Lrk[0]) / (gi[0] * R1)) / (Lrk[1] * w0 * w0);
  for (int i = 1; i < N - 1; i++) {
    Crk[i] = 1 / (Lrk[i] * w0 * w0);
  }
  Crk[N - 1] =
      (1 + (w_ * Lrk[N] * r) / (gi[N - 1] * RN)) / (Lrk[N - 1] * w0 * w0);

  M[0] = sqrt((w_ * (R1 * R1 + pow(w0 * Lrk[0], 2))) /
              ((Crk[0] * gi[0] * R1 * w0 * w0))) /
         w0;
  for (int i = 1; i < N; i++) {
    M[i] = (w_ * sqrt((Lrk[i - 1] * Lrk[i]) / (gi[i - 1] * gi[i]))) / w0;
  }
  M[N] = sqrt((w_ * r * (RN * RN + pow(w0 * Lrk[N + 1], 2))) /
              ((Crk[N - 1] * gi[N - 1] * RN * w0 * w0))) /
         w0;

  std::deque<double> Ls(N + 2);
  Ls[0] = Lrk[0] - M[0];
  Ls[1] = Lrk[0] - M[0] - M[1];

  for (int i = 2; i < N; i++) {
    Ls[i] = Lrk[i - 2] - M[i - 1] - M[i];
  }
  Ls[N]     = Lrk[N] - M[N - 1] - M[N];
  Ls[N + 1] = Lrk[N + 1] - M[N];

  // Impedance and frequency scaling
  for (int i = 0; i < N + 2; i++) {
    Ls[i] = Ls[i] * Z0 / (2 * M_PI * fc);
    if (i < N + 1) {
      Lp[i] = M[i] * Z0 / (2 * M_PI * fc);
      if (i < N) {
        Crk[i] = Crk[i] / (2 * M_PI * fc * Z0);
      }
    }
  }

  // Create schematic and Qucs netlist
  int posx = 0, Ni = 0;
  QString ConnectionAux = "";
  double k              = Specification.ZS;
  ComponentInfo Lseries, Lshunt, Cseries, Ground;
  NodeInfo NI;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar1.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar1);
  ConnectionAux = TermSpar1.ID;

  posx += 50;
  // Series inductor
  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, posx, 0);
  Lseries.val["L"] = num2str(Ls[0], Inductance);
  Schematic.appendComponent(Lseries);
  Ni++;

  // Wires
  //***** Port to capacitor *****
  Schematic.appendWire(TermSpar1.ID, 0, Lseries.ID, 1);

  posx += 50;
  // Shunt inductor
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
    // Series inductor
    posx += 50;
    Lseries.setParams(
        QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
        -90, posx, 0);
    Lseries.val["L"] = num2str(Ls[k + 1], Inductance);
    Schematic.appendComponent(Lseries);
    Ni++;

    // Series capacitor
    posx += 75;
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(Crk[k], Capacitance);
    Schematic.appendComponent(Cseries);
    Ni++;

    posx += 50;
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
                         1); // Series inductor to the previous section
    Schematic.appendWire(NI.ID, 0, Lshunt.ID,
                         1); // Node to the shunt coupling inductor
    Schematic.appendWire(Ground.ID, 0, Lshunt.ID,
                         0); // Shunt inductor to ground
  }

  posx += 50;
  // Series inductor
  Lseries.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                    Inductor, -90, posx, 0);
  Lseries.val["L"] = num2str(Ls[N + 1], Inductance);
  Schematic.appendComponent(Lseries);

  Schematic.appendWire(NI.ID, 0, Lseries.ID,
                       1); // Last series inductor to the previous node

  posx += 50;
  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar2.val["Z"] = num2str(Specification.ZL, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Last series inductor to the previous node
  Schematic.appendWire(TermSpar2.ID, 0, Lseries.ID, 0);
}
