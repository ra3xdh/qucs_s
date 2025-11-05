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

#include "CapacitivelyCoupledShuntResonatorsFilter.h"

CapacitivelyCoupledShuntResonatorsFilter::
    CapacitivelyCoupledShuntResonatorsFilter() {}

CapacitivelyCoupledShuntResonatorsFilter::
    CapacitivelyCoupledShuntResonatorsFilter(FilterSpecifications FS) {
  Specification = FS;
}

CapacitivelyCoupledShuntResonatorsFilter::
    ~CapacitivelyCoupledShuntResonatorsFilter() {}

// This function synthesizes a capacitively coupled resonators bandpass filter
// implementation Reference: Microwave Engineering. David M. Pozar. 4th Edition.
// 2012. John Wiley and Sons.Page 443-448.
void CapacitivelyCoupledShuntResonatorsFilter::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  std::deque<double> gi = LP_coeffs.getCoefficients();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildFilter_IdealTL(gi);
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildFilter_Microstrip(gi);
  }
}

void CapacitivelyCoupledShuntResonatorsFilter::buildFilter_IdealTL(
    const std::deque<double> &gi) {
  ComponentInfo SC_Stub, Cseries;
  NodeInfo NI;

  int N = Specification.order;
  int posx = 0, posy = 50;
  QString PreviousComponent;

  double delta = Specification.bw / Specification.fc;
  double w0 = 2 * M_PI * Specification.fc;
  double Z0 = Specification.ZS;
  double lambda0 = SPEED_OF_LIGHT / Specification.fc;
  std::vector<double> J(N + 1), C(N + 1), deltaC(N), l(N);

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComponent = TermSpar1.ID;

  posx += 50;

  for (int k = 0; k < N; k++) {
    if (k == 0) {
      J[k] = sqrt(M_PI * delta / (4 * gi[k + 1])) / Z0;
      C[k] = J[k] / (w0 * sqrt(1.0 - Z0 * Z0 * J[k] * J[k]));

      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 90, posx, 0);
      Cseries.val["C"] = num2str(C[k], Capacitance);
      Schematic.appendComponent(Cseries);
      posx += 50;

      Schematic.appendWire(Cseries.ID, 0, TermSpar1.ID, 0);
      PreviousComponent = Cseries.ID;
      continue;
    }

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        0);
    Schematic.appendNode(NI);
    Schematic.appendWire(PreviousComponent, 1, NI.ID, 0);

    J[k] = (0.25 * M_PI * delta / sqrt(gi[k] * gi[k + 1])) / Z0;
    C[k] = J[k] / w0;
    deltaC[k - 1] = -C[k - 1] - C[k];
    l[k - 1] = lambda0 / 4 + (Z0 * w0 * deltaC[k - 1] / (2 * M_PI)) * lambda0;
    if (l[k - 1] < 0) {
      l[k - 1] += lambda0 / 4;
    }

    // Short stub
    SC_Stub.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
        ShortStub, 0, posx, posy);
    SC_Stub.val["Z0"] = num2str(Z0, Resistance);
    SC_Stub.val["Length"] = ConvertLengthFromM("mm", l[k - 1]);
    Schematic.appendComponent(SC_Stub);

    Schematic.appendWire(NI.ID, 0, SC_Stub.ID, 1);

    posx += 50;

    // Series capacitor
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(C[k], Capacitance);
    Schematic.appendComponent(Cseries);

    Schematic.appendWire(NI.ID, 1, Cseries.ID, 0);

    PreviousComponent = Cseries.ID;
    posx += 50;
  }

  // Last node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      0);
  Schematic.appendNode(NI);
  Schematic.appendWire(PreviousComponent, 1, NI.ID, 0);

  // Last short stub + C series section
  J[N] = sqrt(M_PI * delta / (4 * gi[N + 1] * gi[N])) / Z0;
  C[N] = J[N] / (w0 * sqrt(1 - Z0 * Z0 * J[N] * J[N]));
  deltaC[N - 1] = -C[N] - C[N - 1];
  l[N - 1] = lambda0 / 4 + (Z0 * w0 * deltaC[N - 1] / (2 * M_PI)) * lambda0;
  if (l[N - 1] < 0) {
    l[N - 1] += lambda0 / 4;
  }

  // Short stub
  SC_Stub.setParams(
      QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
      ShortStub, 0, posx, posy);
  SC_Stub.val["Z0"] = num2str(Z0, Resistance);
  SC_Stub.val["Length"] = ConvertLengthFromM("mm", l[N - 1]);
  Schematic.appendComponent(SC_Stub);

  posx += 50;

  // Series capacitor
  Cseries.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                    Capacitor, 90, posx, 0);
  Cseries.val["C"] = num2str(C[N], Capacitance);
  Schematic.appendComponent(Cseries);
  posx += 50;

  Schematic.appendWire(SC_Stub.ID, 1, NI.ID, 0);
  Schematic.appendWire(NI.ID, 0, Cseries.ID, 0);

  // Add Term 2
  double k = Specification.ZS;
  if (Specification.UseZverevTables) {
    (!Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  } else {
    (Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  }

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSpar2.ID, 0, Cseries.ID, 1);
}

void CapacitivelyCoupledShuntResonatorsFilter::buildFilter_Microstrip(
    const std::deque<double> &gi) {
  ComponentInfo SC_Stub, Cseries, MSVIA;
  NodeInfo NI;

  int N = Specification.order;
  int posx = 0, posy = 50;
  QString PreviousComponent;

  double delta = Specification.bw / Specification.fc;
  double w0 = 2 * M_PI * Specification.fc;
  double Z0 = Specification.ZS;
  double lambda0 = SPEED_OF_LIGHT / Specification.fc;
  std::vector<double> J(N + 1), C(N + 1), deltaC(N), l(N);

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComponent = TermSpar1.ID;

  posx += 50;

  for (int k = 0; k < N; k++) {
    if (k == 0) {
      J[k] = sqrt(M_PI * delta / (4 * gi[k + 1])) / Z0;
      C[k] = J[k] / (w0 * sqrt(1.0 - Z0 * Z0 * J[k] * J[k]));

      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 90, posx, 0);
      Cseries.val["C"] = num2str(C[k], Capacitance);
      Schematic.appendComponent(Cseries);
      posx += 50;

      Schematic.appendWire(Cseries.ID, 0, TermSpar1.ID, 0);
      PreviousComponent = Cseries.ID;
      continue;
    }

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
        0);
    Schematic.appendNode(NI);
    Schematic.appendWire(PreviousComponent, 1, NI.ID, 0);

    J[k] = (0.25 * M_PI * delta / sqrt(gi[k] * gi[k + 1])) / Z0;
    C[k] = J[k] / w0;
    deltaC[k - 1] = -C[k - 1] - C[k];
    l[k - 1] = lambda0 / 4 + (Z0 * w0 * deltaC[k - 1] / (2 * M_PI)) * lambda0;
    if (l[k - 1] < 0) {
      l[k - 1] += lambda0 / 4;
    }

    // Microstrip short stub
    MicrostripClass MSL;
    MSL.Substrate = Specification.MS_Subs;
    MSL.synthesizeMicrostrip(Z0, l[k - 1] * 1e3, Specification.fc);

    double MS_Width = MSL.Results.width;
    double MS_Length = MSL.Results.length * 1e-3;

    SC_Stub.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 0, posx, posy);
    SC_Stub.val["Width"] = ConvertLengthFromM("mm", MS_Width);
    SC_Stub.val["Length"] = ConvertLengthFromM("mm", MS_Length);
    SC_Stub.val["er"] = num2str(Specification.MS_Subs.er);
    SC_Stub.val["h"] = num2str(Specification.MS_Subs.height);
    SC_Stub.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    SC_Stub.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    SC_Stub.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(SC_Stub);

    // Via
    MSVIA.setParams(
        QString("MSVIA%1").arg(++Schematic.NumberComponents[MicrostripVia]),
        MicrostripVia, 0, posx, posy + 50);
    MSVIA.val["D"] = ConvertLengthFromM("mm", 0.5e-3);
    MSVIA.val["N"] = QString::number(4);
    MSVIA.val["er"] = num2str(Specification.MS_Subs.er);
    MSVIA.val["h"] = num2str(Specification.MS_Subs.height);
    MSVIA.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    MSVIA.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    MSVIA.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(MSVIA);

    Schematic.appendWire(NI.ID, 0, SC_Stub.ID, 1);
    Schematic.appendWire(SC_Stub.ID, 0, MSVIA.ID, 0);

    posx += 50;

    // Series capacitor
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(C[k], Capacitance);
    Schematic.appendComponent(Cseries);

    Schematic.appendWire(NI.ID, 1, Cseries.ID, 0);

    PreviousComponent = Cseries.ID;
    posx += 50;
  }

  // Last node
  NI.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), posx,
      0);
  Schematic.appendNode(NI);
  Schematic.appendWire(PreviousComponent, 1, NI.ID, 0);

  // Last short stub + C series section
  J[N] = sqrt(M_PI * delta / (4 * gi[N + 1] * gi[N])) / Z0;
  C[N] = J[N] / (w0 * sqrt(1 - Z0 * Z0 * J[N] * J[N]));
  deltaC[N - 1] = -C[N] - C[N - 1];
  l[N - 1] = lambda0 / 4 + (Z0 * w0 * deltaC[N - 1] / (2 * M_PI)) * lambda0;
  if (l[N - 1] < 0) {
    l[N - 1] += lambda0 / 4;
  }

  // Microstrip short stub
  MicrostripClass MSL;
  MSL.Substrate = Specification.MS_Subs;
  MSL.synthesizeMicrostrip(Z0, l[N - 1] * 1e3, Specification.fc);

  double MS_Width = MSL.Results.width;
  double MS_Length = MSL.Results.length * 1e-3;

  SC_Stub.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, posx, posy);
  SC_Stub.val["Width"] = ConvertLengthFromM("mm", MS_Width);
  SC_Stub.val["Length"] = ConvertLengthFromM("mm", MS_Length);
  SC_Stub.val["er"] = num2str(Specification.MS_Subs.er);
  SC_Stub.val["h"] = num2str(Specification.MS_Subs.height);
  SC_Stub.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  SC_Stub.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  SC_Stub.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(SC_Stub);

  // Via
  MSVIA.setParams(
      QString("MSVIA%1").arg(++Schematic.NumberComponents[MicrostripVia]),
      MicrostripVia, 0, posx, posy + 50);
  MSVIA.val["D"] = ConvertLengthFromM("mm", 0.5e-3);
  MSVIA.val["N"] = QString::number(4);
  MSVIA.val["er"] = num2str(Specification.MS_Subs.er);
  MSVIA.val["h"] = num2str(Specification.MS_Subs.height);
  MSVIA.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  MSVIA.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  MSVIA.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(MSVIA);

  Schematic.appendWire(NI.ID, 0, SC_Stub.ID, 1);
  Schematic.appendWire(SC_Stub.ID, 0, MSVIA.ID, 0);

  posx += 50;

  // Series capacitor
  Cseries.setParams(QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
                    Capacitor, 90, posx, 0);
  Cseries.val["C"] = num2str(C[N], Capacitance);
  Schematic.appendComponent(Cseries);
  posx += 50;

  Schematic.appendWire(SC_Stub.ID, 1, NI.ID, 0);
  Schematic.appendWire(NI.ID, 0, Cseries.ID, 0);

  // Add Term 2
  double k = Specification.ZS;
  if (Specification.UseZverevTables) {
    (!Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  } else {
    (Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  }

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSpar2.ID, 0, Cseries.ID, 1);
}
