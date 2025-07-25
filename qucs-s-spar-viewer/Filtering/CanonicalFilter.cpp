/***************************************************************************
                                CanonicalFilter.cpp
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
#include "CanonicalFilter.h"

CanonicalFilter::CanonicalFilter() {}

CanonicalFilter::CanonicalFilter(FilterSpecifications FS) {
  Specification = FS;
}

CanonicalFilter::~CanonicalFilter() {}

void CanonicalFilter::setSemilumpedMode(bool mode) { this->semilumped = mode; }
void CanonicalFilter::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  gi = LP_coeffs.getCoefficients();

  if ((Specification.FilterResponse == Chebyshev) &&
      (!Specification
            .UseZverevTables)) { // Correct cutoff according to the ripple
    double epsilon = sqrt(pow(10.0, Specification.Ripple / 10.0) - 1.0);

    switch (Specification.FilterType) {
    case Lowpass:
      Specification.fc /= cosh(acosh(1.0 / epsilon) / Specification.order);
      break;
    case Highpass:
      Specification.fc *= cosh(acosh(1.0 / epsilon) / Specification.order);
      break;
    case Bandpass:
    case Bandstop:
      break;
    }
  }

  switch (Specification.FilterType) {
  case Lowpass:
    SynthesizeLPF();
    break;
  case Highpass:
    SynthesizeHPF();
    break;
  case Bandpass:
    SynthesizeBPF();
    break;
  case Bandstop:
    SynthesizeBSF();
    break;
  }
}

// Synthesis of lowpass filters
void CanonicalFilter::SynthesizeLPF() {
  ComponentInfo Cshunt, Lseries, Ground;
  NodeInfo NI;
  double L_ci, L_li, lambda0 = SPEED_OF_LIGHT / Specification.fc;

  // Synthesize CLC of LCL network
  int N = Specification.order; // Number of elements
  int posx = 0;
  QString ConnectionAux = "";

  // Add Term 1
  double k = Specification.ZS;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar1.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar1);

  ConnectionAux = TermSpar1.ID;

  int Kcontrol = 0;
  if (!Specification.UseZverevTables)
    Kcontrol = 0;
  if (Specification.UseZverevTables && (Specification.order % 2 == 0))
    Kcontrol = 1;
  posx += 50;
  for (int k = 0; k < N; k++) {

    if (((Specification.isCLC) && (k % 2 == Kcontrol)) ||
        ((!Specification.isCLC) && (k % 2 != Kcontrol))) {
      // Shunt capacitor
      gi[k + 1] *= 1 / (2 * M_PI * Specification.fc *
                        Specification.ZS); // Lowpass to highpass transformation
      if ((semilumped == true) &&
          (Specification.SemiLumpedISettings == INDUCTORS_AND_SHUNT_CAPS)) {
        Cshunt.setParams(QString("TLIN%1").arg(
                             ++Schematic.NumberComponents[TransmissionLine]),
                         OpenStub, 0.0, posx, 50);
        // Microstrip Filters for RF/Microwave Applications. JIA-SHENG HONG. M.
        // J. LANCASTER. JOHN WILEY & SONS, INC. 2001. page 119. Eq. 5.9
        L_ci =
            lambda0 / (2 * M_PI) *
            asin(2 * M_PI * Specification.fc * Specification.minZ * gi[k + 1]);
        Cshunt.val["Z0"] = num2str(Specification.minZ, Resistance);
        Cshunt.val["Length"] = ConvertLengthFromM("mm", L_ci);
      } else {
        // Lumped capacitor
        Cshunt.setParams(
            QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
            Capacitor, 0.0, posx, 50);
        Cshunt.val["C"] = num2str(gi[k + 1], Capacitance);

        // GND
        Ground.setParams(
            QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0,
            posx, 100);
        Schematic.appendComponent(Ground);

        //***** GND to capacitor *****
        Schematic.appendWire(Ground.ID, 0, Cshunt.ID, 0);
      }
      Schematic.appendComponent(Cshunt);

      // Node
      NI.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(NI);

      // Wires
      //***** Capacitor to node *****
      Schematic.appendWire(NI.ID, 0, Cshunt.ID, 1);

      //***** Capacitor to the previous Lseries/Term *****
      Schematic.appendWire(NI.ID, 1, ConnectionAux, 0);

      ConnectionAux = NI.ID; // The series inductor of the next section must be
                             // connected to this node
    } else {
      // Series inductor
      gi[k + 1] *= Specification.ZS / (2 * M_PI * Specification.fc);
      if (semilumped == true) {
        // Microstrip Filters for RF/Microwave Applications. JIA-SHENG HONG. M.
        // J. LANCASTER. JOHN WILEY & SONS, INC. 2001. page 119. Eq. 5.9
        L_li =
            lambda0 / (2 * M_PI) *
            asin(2 * M_PI * Specification.fc * gi[k + 1] / Specification.maxZ);
        Lseries.setParams(QString("TLIN%1").arg(
                              ++Schematic.NumberComponents[TransmissionLine]),
                          TransmissionLine, -90, posx, 0);
        Lseries.val["Z0"] = num2str(Specification.maxZ, Resistance);
        Lseries.val["Length"] = ConvertLengthFromM("mm", L_li);
      } else {
        Lseries.setParams(
            QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
            Inductor, -90, posx, 0);

        Lseries.val["L"] = num2str(gi[k + 1], Inductance);
      }
      Schematic.appendComponent(Lseries);
      // Wiring
      Schematic.appendWire(ConnectionAux, 0, Lseries.ID, 1);
      ConnectionAux = Lseries.ID;
    }
    posx += 50;
  }
  // Add Term 2
  k = Specification.ZS;
  if (Specification.UseZverevTables)
    (!Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  else
    (Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSpar2.ID, 0, ConnectionAux, 0);
}

// Synthesis of highpass filters
void CanonicalFilter::SynthesizeHPF() {
  ComponentInfo Lshunt, Cseries, Ground;
  // Synthesize CLC of LCL network
  int N = Specification.order; // Number of elements
  int posx = 0;
  double L_li, lambda0 = SPEED_OF_LIGHT / Specification.fc;
  QString ConnectionAux = "";

  NodeInfo NI;

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, -180, posx,
      0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);

  ConnectionAux = TermSpar1.ID;
  posx += 50;
  for (int k = 0; k < N; k++) {

    if (((Specification.isCLC) && (k % 2 == 0)) ||
        ((!Specification.isCLC) && (k % 2 != 0))) {
      // Shunt inductor
      gi[k + 1] = Specification.ZS / (2 * M_PI * Specification.fc * gi[k + 1]);
      if (semilumped == true) {
        Lshunt.setParams(QString("TLIN%1").arg(
                             ++Schematic.NumberComponents[TransmissionLine]),
                         ShortStub, 0, posx, 50);
        // Microstrip Filters for RF/Microwave Applications. JIA-SHENG HONG. M.
        // J. LANCASTER. JOHN WILEY & SONS, INC. 2001. page 119. Eq. 5.9
        L_li =
            lambda0 / (2 * M_PI) *
            asin(2 * M_PI * Specification.fc * gi[k + 1] / Specification.maxZ);
        Lshunt.val["Z0"] = num2str(Specification.maxZ, Resistance);
        Lshunt.val["Length"] = ConvertLengthFromM("mm", L_li);
      } else {
        Lshunt.setParams(
            QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
            Inductor, 0, posx, 50);
        Lshunt.val["L"] = num2str(gi[k + 1], Inductance);

        // GND
        Ground.setParams(
            QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0,
            posx, 100);
        Schematic.appendComponent(Ground);

        //***** GND to capacitor *****
        Schematic.appendWire(Ground.ID, 0, Lshunt.ID, 0);
      }
      Schematic.appendComponent(Lshunt);

      // Node
      NI.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(NI);

      // Wires
      //***** Capacitor to node *****
      Schematic.appendWire(NI.ID, 1, Lshunt.ID, 1);

      //***** Capacitor to the previous Lseries/Term *****
      Schematic.appendWire(ConnectionAux, 1, NI.ID, 1);

      ConnectionAux = NI.ID; // The series inductor of the next section must be
                             // connected to this node
    } else {
      // Series capacitor
      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 90, posx, 0);
      gi[k + 1] =
          1 / (2 * M_PI * Specification.fc * gi[k + 1] * Specification.ZS);
      Cseries.val["C"] = num2str(gi[k + 1], Capacitance);
      Schematic.appendComponent(Cseries);

      // Wiring
      Schematic.appendWire(ConnectionAux, 0, Cseries.ID, 0);

      ConnectionAux = Cseries.ID;
    }
    posx += 50;
  }
  // Add Term 2
  double k = Specification.ZL;
  Specification.isCLC ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(ConnectionAux, 1, TermSpar2.ID, 0);
}

// Synthesis of bandpass filters
void CanonicalFilter::SynthesizeBPF() {
  ComponentInfo Cshunt, Lshunt, Ground1, Ground2, Cseries, Lseries;
  NodeInfo NI;
  // Synthesize CLC of LCL network
  int N = Specification.order; // Number of elements
  int posx = 0;
  QString ConnectionAux = "";

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, -180, posx,
      0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);

  ConnectionAux = TermSpar1.ID;

  double wc = 2 * M_PI * Specification.fc;
  double delta = 2 * M_PI * Specification.bw;
  double w0 = sqrt(wc * wc - .25 * delta * delta);

  posx += 50;
  for (int k = 0; k < N; k++) {

    if (((Specification.isCLC) && (k % 2 == 0)) ||
        ((!Specification.isCLC) && (k % 2 != 0))) {
      // Shunt capacitor
      Cshunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0, posx - 25, 50);
      Cshunt.val["C"] =
          num2str(gi[k + 1] / (delta * Specification.ZS), Capacitance);
      Schematic.appendComponent(Cshunt);

      // GND
      Ground1.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, posx - 25, 100);
      Schematic.appendComponent(Ground1);

      // Shunt inductor
      Lshunt.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          0, posx + 25, 50);
      Lshunt.val["L"] =
          num2str(Specification.ZS * delta / (w0 * w0 * gi[k + 1]), Inductance);
      Schematic.appendComponent(Lshunt);

      // GND
      Ground2.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, posx + 25, 100);
      Schematic.appendComponent(Ground2);

      // Node
      NI.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(NI);

      // Wires
      //***** Capacitor to node *****
      Schematic.appendWire(NI.ID, 1, Cshunt.ID, 1);

      //***** Inductor to node *****
      Schematic.appendWire(NI.ID, 1, Lshunt.ID, 1);

      //***** GND to capacitor *****
      Schematic.appendWire(Ground1.ID, 0, Cshunt.ID, 0);

      //***** GND to inductor *****
      Schematic.appendWire(Ground2.ID, 0, Lshunt.ID, 0);

      //***** Capacitor to the previous Lseries/Term *****
      Schematic.appendWire(ConnectionAux, 1, NI.ID, 1);

      ConnectionAux = NI.ID; // The series inductor of the next section must be
                             // connected to this node
    } else {
      if (k == 0)
        posx += 50; // First element
      // Series inductor
      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, posx - 30, 0);
      Lseries.val["L"] =
          num2str(gi[k + 1] * Specification.ZS / (delta), Inductance);
      Schematic.appendComponent(Lseries);

      // Series capacitor
      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 90, posx + 30, 0);
      Cseries.val["C"] = num2str(
          delta / (w0 * w0 * Specification.ZS * gi[k + 1]), Capacitance);
      Schematic.appendComponent(Cseries);

      // Wiring
      Schematic.appendWire(ConnectionAux, 0, Lseries.ID, 1);
      Schematic.appendWire(Lseries.ID, 0, Cseries.ID, 0);
      ConnectionAux = Cseries.ID;
    }
    posx += 100;
  }
  // Add Term 2
  double k = Specification.ZL;
  Specification.isCLC ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(ConnectionAux, 1, TermSpar2.ID, 0);
}

// Synthesis of bandstop filters
void CanonicalFilter::SynthesizeBSF() {
  ComponentInfo Cshunt, Lshunt, Ground1, Lseries, Cseries;
  NodeInfo NI, Node1, Node2;
  // Synthesize CLC of LCL network
  int N = Specification.order; // Number of elements
  int posx = 0;

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, -180, posx,
      0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);

  QMap<QString, unsigned int> UnconnectedComponents;
  UnconnectedComponents[TermSpar1.ID] = 0;

  double wc = 2 * M_PI * Specification.fc;
  double delta = 2 * M_PI * Specification.bw;
  double w0 = sqrt(wc * wc - .25 * delta * delta);

  posx += 50;
  for (int k = 0; k < N; k++) {

    if (((Specification.isCLC) && (k % 2 == 0)) ||
        ((!Specification.isCLC) && (k % 2 != 0))) {
      // Shunt capacitor
      Cshunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0, posx, 100);
      Cshunt.val["C"] = num2str(
          gi[k + 1] * delta / (w0 * w0 * Specification.ZS), Capacitance);
      Schematic.appendComponent(Cshunt);

      // GND
      Ground1.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, posx, 150);
      Schematic.appendComponent(Ground1);

      // Shunt inductor
      Lshunt.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          0, posx, 50);
      Lshunt.val["L"] =
          num2str(Specification.ZS / (delta * gi[k + 1]), Inductance);
      Schematic.appendComponent(Lshunt);

      // Node
      NI.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(NI);

      // Wires
      //***** Inductor to node *****
      Schematic.appendWire(NI.ID, 1, Lshunt.ID, 1);

      //***** Capacitor to inductor *****
      Schematic.appendWire(Lshunt.ID, 0, Cshunt.ID, 1);

      //***** GND to capacitor *****
      Schematic.appendWire(Ground1.ID, 0, Cshunt.ID, 0);

      //***** Capacitor to the previous Lseries *****
      QMap<QString, unsigned int>::const_iterator i =
          UnconnectedComponents.constBegin();
      while (i != UnconnectedComponents.constEnd()) {
        Schematic.appendWire(i.key(), i.value(), NI.ID, 1);
        ++i;
      }

      UnconnectedComponents.clear();
      UnconnectedComponents[NI.ID] = 0;

      // The series inductor of the next section
      //   must be
      // connected to this node
    } else {
      if (k == 0)
        posx += 50; // First element

      // Node
      Node1.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(Node1);
      posx += 50;

      // Series inductor
      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, posx, 30);
      Lseries.val["L"] =
          num2str(gi[k + 1] * Specification.ZS * delta / (w0 * w0), Inductance);
      Schematic.appendComponent(Lseries);

      // Series capacitor

      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 90, posx, -30);
      Cseries.val["C"] =
          num2str(1 / (gi[k + 1] * delta * Specification.ZS), Capacitance);
      Schematic.appendComponent(Cseries);

      // Node
      posx += 50;
      Node2.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(Node2);

      // Wiring
      // Intermediate series inductance => Connect port 0 to the previous
      // Cshunt and port 1 to the next Cshunt

      QMap<QString, unsigned int>::const_iterator i =
          UnconnectedComponents.constBegin();
      while (i != UnconnectedComponents.constEnd()) {
        Schematic.appendWire(i.key(), i.value(), Node1.ID, 0);
        ++i;
      }

      Schematic.appendWire(Node1.ID, 0, Lseries.ID, 1);
      Schematic.appendWire(Node1.ID, 0, Cseries.ID, 0);
      Schematic.appendWire(Node2.ID, 0, Lseries.ID, 0);
      Schematic.appendWire(Node2.ID, 0, Cseries.ID, 1);

      UnconnectedComponents.clear();
      UnconnectedComponents[Node2.ID] = 0;
    }
    posx += 100;
  }
  // Add Term 2
  double k = Specification.ZL;
  Specification.isCLC ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);

  QMap<QString, unsigned int>::const_iterator i =
      UnconnectedComponents.constBegin();
  while (i != UnconnectedComponents.constEnd()) {
    Schematic.appendWire(i.key(), i.value(), TermSpar2.ID, 0);
    ++i;
  }
}
