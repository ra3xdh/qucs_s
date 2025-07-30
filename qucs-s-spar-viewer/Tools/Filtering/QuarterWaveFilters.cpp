/***************************************************************************
                                QuarterWaveFilters.cpp
                                ----------
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/

#include "QuarterWaveFilters.h"

QuarterWaveFilters::QuarterWaveFilters() {
  // Initialize list of components
  Schematic.NumberComponents[Capacitor] = 0;
  Schematic.NumberComponents[Inductor] = 0;
  Schematic.NumberComponents[Term] = 0;
  Schematic.NumberComponents[GND] = 0;
  Schematic.NumberComponents[ConnectionNodes] = 0;
}

QuarterWaveFilters::QuarterWaveFilters(FilterSpecifications FS) {
  Specification = FS;
  // Initialize list of components
  Schematic.NumberComponents[Capacitor] = 0;
  Schematic.NumberComponents[Inductor] = 0;
  Schematic.NumberComponents[Term] = 0;
  Schematic.NumberComponents[GND] = 0;
  Schematic.NumberComponents[ConnectionNodes] = 0;
}

QuarterWaveFilters::~QuarterWaveFilters() {}

void QuarterWaveFilters::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  std::deque<double> gi = LP_coeffs.getCoefficients();

  ComponentInfo QW_TL, OC_Stub, SC_Stub;
  NodeInfo NI;
  double lambda4 = SPEED_OF_LIGHT / (4. * Specification.fc);

  int N = Specification.order;
  double Z;
  gi.pop_back();
  gi.pop_front();
  double fc = Specification.fc;
  double BW = Specification.bw;
  double bw = BW / fc;
  double Z0 = Specification.ZS;

  // Build schematic
  int posx = 0;
  QString PreviousComp;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar1.val["Z"] = num2str(Z0, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComp = TermSpar1.ID;
  posx -= 50;

  for (int k = 0; k < N; k++) {
    posx += 100;
    // Quarter-wave transmission line
    QW_TL.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx, 0);
    QW_TL.val["Z0"] = num2str(Z0, Resistance);
    QW_TL.val["Length"] = ConvertLengthFromM("mm", lambda4);
    Schematic.appendComponent(QW_TL);

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, 0);
    Schematic.appendNode(NI);

    // Wire: Connect the QW transmission line to the previous element (Term1 or
    // a node)
    Schematic.appendWire(PreviousComp, 0, QW_TL.ID, 0);

    // Wire: Connect the QW transmission line to the node
    Schematic.appendWire(NI.ID, 0, QW_TL.ID, 1);

    // Stubs
    switch (Specification.FilterType) {
    default:
    case Bandpass:
      Z = (M_PI * Z0 * bw) / (4 * gi[k]);
      SC_Stub.setParams(
          QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
          ShortStub, 0, posx + 50, 50);
      SC_Stub.val["Z0"] = num2str(Z, Resistance);
      SC_Stub.val["Length"] = ConvertLengthFromM("mm", lambda4);
      Schematic.appendComponent(SC_Stub);

      // Wire: Node to stub
      Schematic.appendWire(NI.ID, 0, SC_Stub.ID, 1);
      break;

    case Bandstop:
      Z = (4 * Z0) / (M_PI * bw * gi[k]);
      OC_Stub.setParams(
          QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]),
          OpenStub, 0, posx + 50, 50);
      OC_Stub.val["Z0"] = num2str(Z, Resistance);
      OC_Stub.val["Length"] = ConvertLengthFromM("mm", lambda4);
      Schematic.appendComponent(OC_Stub);

      // Wire: Node to stub
      Schematic.appendWire(NI.ID, 0, OC_Stub.ID, 1);
      break;
    }
    PreviousComp = NI.ID;
  }
  posx += 100;
  // Quarter-wave transmission line
  QW_TL.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, posx, 0);
  QW_TL.val["Z0"] = num2str(Z0, Resistance);
  QW_TL.val["Length"] = ConvertLengthFromM("mm", lambda4);
  Schematic.appendComponent(QW_TL);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0,
      posx + 50, 0);
  TermSpar2.val["Z"] = num2str(Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Wire: Connect  QW line to the previous node
  Schematic.appendWire(NI.ID, 0, QW_TL.ID, 0);

  // Wire: Connect QW line to the SPAR term
  Schematic.appendWire(TermSpar2.ID, 0, QW_TL.ID, 1);
}
