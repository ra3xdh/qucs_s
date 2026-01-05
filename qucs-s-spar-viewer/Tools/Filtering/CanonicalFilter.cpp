/// @file CanonicalFilter.cpp
/// @brief Synthesis of canonical filters (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "CanonicalFilter.h"

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
  ComponentInfo Cshunt, Lseries, Ground, MSOPEN;
  NodeInfo NI;
  double L_ci, L_li, lambda0 = SPEED_OF_LIGHT / Specification.fc;

  // Synthesize CLC of LCL network
  int N = Specification.order; // Number of elements
  int posx = 0;
  QString ConnectionAux = "";

  // Add Term 1
  double k = Specification.ZS;

  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar1);

  ConnectionAux = TermSpar1.ID;

  int Kcontrol = 0;
  if (!Specification.UseZverevTables) {
    Kcontrol = 0;
  }
  if (Specification.UseZverevTables && (Specification.order % 2 == 0)) {
    Kcontrol = 1;
  }
  posx += 50;
  for (int k = 0; k < N; k++) {

    if (((Specification.isCLC) && (k % 2 == Kcontrol)) ||
        ((!Specification.isCLC) && (k % 2 != Kcontrol))) {

      // Node
      NI.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(NI);

      // Shunt capacitor
      gi[k + 1] *= 1 / (2 * M_PI * Specification.fc *
                        Specification.ZS); // Lowpass to highpass transformation
      if ((semilumped == true) &&
          (Specification.SemiLumpedISettings == INDUCTORS_AND_SHUNT_CAPS)) {
        // Microstrip Filters for RF/Microwave Applications. JIA-SHENG HONG. M.
        // J. LANCASTER. JOHN WILEY & SONS, INC. 2001. page 119. Eq. 5.9
        L_ci =
            lambda0 / (2 * M_PI) *
            asin(2 * M_PI * Specification.fc * Specification.minZ * gi[k + 1]);

        if (Specification.TL_implementation == TransmissionLineType::Ideal) {
          // Ideal transmission line
          Cshunt.setParams(
              QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]),
              OpenStub, 0.0, posx, 50);
          Cshunt.val["Z0"] = num2str(Specification.minZ, Resistance);
          Cshunt.val["Length"] = ConvertLengthFromM("mm", L_ci);
          Schematic.appendComponent(Cshunt);

          Schematic.appendWire(NI.ID, 1, ConnectionAux, 1);

        } else if (Specification.TL_implementation ==
                   TransmissionLineType::MLIN) {
          // Microstrip transmission line
          MicrostripClass MSL; // Synthesize MS parameters

          MSL.Substrate = Specification.MS_Subs;
          MSL.synthesizeMicrostrip(Specification.minZ, L_ci * 1e3,
                                   Specification.fc);

          double MS_Width =
              MSL.Results.width; // MicrostripClass calculations are in mm. It's
                                 // needed to convert to m
          double MS_Length = MSL.Results.length * 1e-3;

          // Instantiate component
          Cshunt.setParams(QString("MLIN%1").arg(
                               ++Schematic.NumberComponents[MicrostripLine]),
                           MicrostripLine, 0, posx, 50);

          // Physical parameters
          Cshunt.val["Width"] = ConvertLengthFromM("mm", MS_Width);
          Cshunt.val["Length"] = ConvertLengthFromM("mm", MS_Length);

          // Substrate-related parameters
          Cshunt.val["er"] = num2str(Specification.MS_Subs.er);
          Cshunt.val["h"] = num2str(Specification.MS_Subs.height);
          Cshunt.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
          Cshunt.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
          Cshunt.val["tand"] = num2str(Specification.MS_Subs.tand);
          Schematic.appendComponent(Cshunt);

          // Microstrip open
          MSOPEN.setParams(QString("MOPEN%1").arg(
                               ++Schematic.NumberComponents[MicrostripOpen]),
                           MicrostripOpen, 0, posx, 100);

          // Physical parameters
          MSOPEN.val["Width"] = ConvertLengthFromM("mm", MS_Width);

          // Substrate-related parameters
          MSOPEN.val["er"] = num2str(Specification.MS_Subs.er);
          MSOPEN.val["h"] = num2str(Specification.MS_Subs.height);
          MSOPEN.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
          MSOPEN.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
          MSOPEN.val["tand"] = num2str(Specification.MS_Subs.tand);
          Schematic.appendComponent(MSOPEN);

          Schematic.appendWire(Cshunt.ID, 0, MSOPEN.ID,
                               0); // Wire: Stub to open circuit model

          Schematic.appendWire(NI.ID, 1, ConnectionAux, 1);
        }
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
        Schematic.appendComponent(Cshunt);

        Schematic.appendWire(NI.ID, 1, ConnectionAux, 0);
      }

      // Wires
      //***** Capacitor to node *****
      Schematic.appendWire(NI.ID, 0, Cshunt.ID, 1);

      ConnectionAux = NI.ID; // The series inductor of the next section must
                             // be connected to this node
    } else {
      // Series inductor
      gi[k + 1] *= Specification.ZS / (2 * M_PI * Specification.fc);
      if (semilumped == true) {
        // Microstrip Filters for RF/Microwave Applications. JIA-SHENG HONG. M.
        // J. LANCASTER. JOHN WILEY & SONS, INC. 2001. page 119. Eq. 5.9
        L_li =
            lambda0 / (2 * M_PI) *
            asin(2 * M_PI * Specification.fc * gi[k + 1] / Specification.maxZ);

        if (Specification.TL_implementation == TransmissionLineType::Ideal) {
          Lseries.setParams(QString("TLIN%1").arg(
                                ++Schematic.NumberComponents[TransmissionLine]),
                            TransmissionLine, 90, posx, 0);
          Lseries.val["Z0"] = num2str(Specification.maxZ, Resistance);
          Lseries.val["Length"] = ConvertLengthFromM("mm", L_li);
          Schematic.appendWire(ConnectionAux, 0, Lseries.ID, 0);

        } else if (Specification.TL_implementation ==
                   TransmissionLineType::MLIN) {
          // Microstrip transmission line

          MicrostripClass MSL; // Synthesize MS parameters

          MSL.Substrate = Specification.MS_Subs;
          MSL.synthesizeMicrostrip(Specification.maxZ, L_li * 1e3,
                                   Specification.fc);

          double MS_Width =
              MSL.Results.width; // MicrostripClass calculations are in mm. It's
                                 // needed to convert to m
          double MS_Length = MSL.Results.length * 1e-3;

          // Instantiate component

          // Physical parameters
          Lseries.setParams(QString("MLIN%1").arg(
                                ++Schematic.NumberComponents[MicrostripLine]),
                            MicrostripLine, 90, posx, 0);
          Lseries.val["Width"] = ConvertLengthFromM("mm", MS_Width);
          Lseries.val["Length"] = ConvertLengthFromM("mm", MS_Length);

          // Substrate-related parameters
          Lseries.val["er"] = num2str(Specification.MS_Subs.er);
          Lseries.val["h"] = num2str(Specification.MS_Subs.height);
          Lseries.val["cond"] =
              num2str(Specification.MS_Subs.MetalConductivity);
          Lseries.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
          Lseries.val["tand"] = num2str(Specification.MS_Subs.tand);

          Schematic.appendWire(ConnectionAux, 0, Lseries.ID, 0);
        }
      } else {
        Lseries.setParams(
            QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
            Inductor, -90, posx, 0);

        Lseries.val["L"] = num2str(gi[k + 1], Inductance);
        Schematic.appendWire(ConnectionAux, 0, Lseries.ID, 1);
      }
      Schematic.appendComponent(Lseries);
      // Wiring

      ConnectionAux = Lseries.ID;
    }
    posx += 50;
  }
  // Add Term 2
  k = Specification.ZS;
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

  Schematic.appendWire(TermSpar2.ID, 0, ConnectionAux, 0);
}

// Synthesis of highpass filters
void CanonicalFilter::SynthesizeHPF() {
  ComponentInfo Lshunt, Cseries, Ground, MSVIA;
  // Synthesize CLC of LCL network
  int N = Specification.order; // Number of elements
  int posx = 0;
  double L_li, lambda0 = SPEED_OF_LIGHT / Specification.fc;
  QString ConnectionAux = "";

  NodeInfo NI;

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);

  ConnectionAux = TermSpar1.ID;
  posx += 50;
  for (int k = 0; k < N; k++) {

    if (((Specification.isCLC) && (k % 2 == 0)) ||
        ((!Specification.isCLC) && (k % 2 != 0))) {

      // Node
      NI.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(NI);

      // Wires
      //***** Shunt inductor to the previous Lseries/Term *****
      Schematic.appendWire(ConnectionAux, 1, NI.ID, 1);

      // Shunt inductor
      gi[k + 1] = Specification.ZS / (2 * M_PI * Specification.fc * gi[k + 1]);

      if (semilumped == true) {
        Lshunt.setParams(
            QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
            ShortStub, 0, posx, 50);
        // Microstrip Filters for RF/Microwave Applications. JIA-SHENG HONG. M.
        // J. LANCASTER. JOHN WILEY & SONS, INC. 2001. page 119. Eq. 5.9
        L_li =
            lambda0 / (2 * M_PI) *
            asin(2 * M_PI * Specification.fc * gi[k + 1] / Specification.maxZ);

        if (Specification.TL_implementation == TransmissionLineType::Ideal) {
          // Ideal transmission line
          Lshunt.val["Z0"] = num2str(Specification.maxZ, Resistance);
          Lshunt.val["Length"] = ConvertLengthFromM("mm", L_li);
          Schematic.appendComponent(Lshunt);

          // Wires
          //***** Shunt inductor to node *****
          Schematic.appendWire(NI.ID, 1, Lshunt.ID, 1);

        } else if (Specification.TL_implementation ==
                   TransmissionLineType::MLIN) {
          // Microstrip transmission line
          MicrostripClass MSL; // Synthesize MS parameters

          MSL.Substrate = Specification.MS_Subs;
          MSL.synthesizeMicrostrip(Specification.maxZ, L_li * 1e3,
                                   Specification.fc);

          double MS_Width =
              MSL.Results.width; // MicrostripClass calculations are in mm. It's
                                 // needed to convert to m
          double MS_Length = MSL.Results.length * 1e-3;

          // Instantiate component
          Lshunt.setParams(QString("MLIN%1").arg(
                               ++Schematic.NumberComponents[MicrostripLine]),
                           MicrostripLine, 0, posx, 50);

          // Physical parameters
          Lshunt.val["Width"] = ConvertLengthFromM("mm", MS_Width);
          Lshunt.val["Length"] = ConvertLengthFromM("mm", MS_Length);

          // Substrate-related parameters
          Lshunt.val["er"] = num2str(Specification.MS_Subs.er);
          Lshunt.val["h"] = num2str(Specification.MS_Subs.height);
          Lshunt.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
          Lshunt.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
          Lshunt.val["tand"] = num2str(Specification.MS_Subs.tand);
          Schematic.appendComponent(Lshunt);

          // GND
          MSVIA.setParams(QString("MSVIA%1").arg(
                              ++Schematic.NumberComponents[MicrostripVia]),
                          MicrostripVia, 0, posx, 100);

          // Physical parameters
          MSVIA.val["D"] = ConvertLengthFromM("mm", 0.5e-3); // Default: 0.5 mm
          MSVIA.val["N"] = QString::number(4);
          ; // Number of vias in parallel (4 vias)

          // Substrate-related parameters
          MSVIA.val["er"] = num2str(Specification.MS_Subs.er);
          MSVIA.val["h"] = num2str(Specification.MS_Subs.height);
          MSVIA.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
          MSVIA.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
          MSVIA.val["tand"] = num2str(Specification.MS_Subs.tand);

          Schematic.appendComponent(MSVIA);

          // Connections
          Schematic.appendWire(NI.ID, 1, Lshunt.ID, 1);
          Schematic.appendWire(Lshunt.ID, 0, MSVIA.ID, 0); // Wire: Stub to gnd
        }
      } else {
        Lshunt.setParams(
            QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
            Inductor, 0, posx, 50);
        Lshunt.val["L"] = num2str(gi[k + 1], Inductance);
        Schematic.appendComponent(Lshunt);

        // GND
        Ground.setParams(
            QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0,
            posx, 100);
        Schematic.appendComponent(Ground);

        //***** GND to capacitor *****
        Schematic.appendWire(NI.ID, 1, Lshunt.ID, 1);
        Schematic.appendWire(Ground.ID, 0, Lshunt.ID, 0);
      }

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
  double k = Specification.ZS;
  Specification.isCLC ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(ConnectionAux, 1, TermSpar2.ID, 0);
}

// Synthesis of bandpass filters
void CanonicalFilter::SynthesizeBPF() {
  ComponentInfo Cshunt, Lshunt, Ground1, Ground2, Cseries, Lseries;
  NodeInfo NI; // Main line node.
  NodeInfo NLeft, NCenter,
      NRight; // Virtual nodes, they make easier the export logic for Qucs-S

  QPoint PosC, PosL;

  // Synthesize CLC of LCL network
  int N = Specification.order; // Number of elements
  int posx = 0;
  QString ConnectionAux = "";

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);

  ConnectionAux = TermSpar1.ID;

  double wc = 2 * M_PI * Specification.fc;
  double delta = 2 * M_PI * Specification.bw;
  double w0 = sqrt(wc * wc - .25 * delta * delta);

  if (Specification.isCLC) {
    posx += 50;
  }

  for (int k = 0; k < N; k++) {

    if (((Specification.isCLC) && (k % 2 == 0)) ||
        ((!Specification.isCLC) && (k % 2 != 0))) {

      // Set up component's position
      PosC = QPoint(posx - 25, 60); // Shunt capacitor
      PosL = QPoint(posx + 25, 60); // Shunt inductor

      // Shunt capacitor
      Cshunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, PosC);
      Cshunt.val["C"] =
          num2str(gi[k + 1] / (delta * Specification.ZS), Capacitance);
      Schematic.appendComponent(Cshunt);

      // GND
      Ground1.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, PosC.x(), PosC.y() + 50);
      Schematic.appendComponent(Ground1);

      // Shunt inductor
      Lshunt.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          PosL);
      Lshunt.val["L"] =
          num2str(Specification.ZS * delta / (w0 * w0 * gi[k + 1]), Inductance);
      Schematic.appendComponent(Lshunt);

      // GND
      Ground2.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, PosL.x(), PosL.y() + 50);
      Schematic.appendComponent(Ground2);

      // Main line node
      NI.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 0);
      Schematic.appendNode(NI);

      // Resonator left virtual node
      NLeft.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          PosC.x(), PosC.y() - 40);
      NLeft.visible = false;
      Schematic.appendNode(NLeft);

      // Resonator center virtual node
      NCenter.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, PosC.y() - 40);
      NCenter.visible = false;
      Schematic.appendNode(NCenter);

      // Resonator right virtual node
      NRight.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          PosL.x(), PosL.y() - 40);
      NRight.visible = false;
      Schematic.appendNode(NRight);

      // Wires
      // Main line node to resonator center node
      Schematic.appendWire(NI.ID, 0, NCenter.ID, 0);

      // Resonator center node to capacitor node
      Schematic.appendWire(NCenter.ID, 0, NLeft.ID, 0);

      // Node above the capacitor to the capacitor
      Schematic.appendWire(NLeft.ID, 0, Cshunt.ID, 1);

      // Resonator center node to inductor node
      Schematic.appendWire(NCenter.ID, 0, NRight.ID, 0);

      // Node above the capacitor to the capacitor
      Schematic.appendWire(NRight.ID, 0, Lshunt.ID, 1);

      //***** GND to capacitor *****
      Schematic.appendWire(Ground1.ID, 0, Cshunt.ID, 0);

      //***** GND to inductor *****
      Schematic.appendWire(Ground2.ID, 0, Lshunt.ID, 0);

      //***** Capacitor to the previous Lseries/Term *****
      Schematic.appendWire(ConnectionAux, 1, NI.ID, 1);

      ConnectionAux = NI.ID; // The series inductor of the next section must be
                             // connected to this node
    } else {
      if (k == 0) {
        posx += 50; // First element
      }
      // Series inductor
      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, posx - 20, 0);
      Lseries.val["L"] =
          num2str(gi[k + 1] * Specification.ZS / (delta), Inductance);
      Schematic.appendComponent(Lseries);

      // Series capacitor
      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 90, posx + 20, 0);
      Cseries.val["C"] = num2str(
          delta / (w0 * w0 * Specification.ZS * gi[k + 1]), Capacitance);
      Schematic.appendComponent(Cseries);

      // Wiring
      Schematic.appendWire(ConnectionAux, 0, Lseries.ID, 1);
      Schematic.appendWire(Lseries.ID, 0, Cseries.ID, 0);
      ConnectionAux = Cseries.ID;
    }
    posx += 80;
  }
  // Add Term 2
  double k = Specification.ZS;
  Specification.isCLC ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
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
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);

  QMap<QString, unsigned int> UnconnectedComponents;
  UnconnectedComponents[TermSpar1.ID] = 0;

  double wc = 2 * M_PI * Specification.fc;
  double delta = 2 * M_PI * Specification.bw;
  double w0 = sqrt(wc * wc - .25 * delta * delta);

  if (Specification.isCLC) {
    posx += 50;
  }

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
      if (k == 0) {
        posx += 50; // First element
      }

      // Node
      Node1.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx - 20, 0);
      Schematic.appendNode(Node1);
      posx += 20;

      // Series inductor
      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, posx, 0);
      Lseries.val["L"] =
          num2str(gi[k + 1] * Specification.ZS * delta / (w0 * w0), Inductance);
      Schematic.appendComponent(Lseries);

      // Series capacitor

      Cseries.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 90, posx, -60);
      Cseries.val["C"] =
          num2str(1 / (gi[k + 1] * delta * Specification.ZS), Capacitance);
      Schematic.appendComponent(Cseries);

      // Node
      posx += 20;
      Node2.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx + 20, 0);
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
    posx += 50;
  }
  // Add Term 2
  double k = Specification.ZS;
  Specification.isCLC ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);

  QMap<QString, unsigned int>::const_iterator i =
      UnconnectedComponents.constBegin();
  while (i != UnconnectedComponents.constEnd()) {
    Schematic.appendWire(i.key(), i.value(), TermSpar2.ID, 0);
    ++i;
  }
}
