/// @file QuarterWaveCoupledShuntResonators.cpp
/// @brief Synthesis of quarter-wavelength coupled shunt resonators filters
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "DirectCoupledFilters.h"

///
/// \internal
///
/// About the synthesis process:
/// 1) Shunt resonators (LC parallel) are calculated to resonate at the center
/// frequency.
///
/// 2) The resonators are coupled using quarter-wavelength transmission lines
/// with characteristic impedance equal to the source/load impedance.
///
/// 3) The susceptance values (Bi/Y0) are calculated from the lowpass prototype
/// coefficients according to the formulas in reference [1].
///
/// References:
/// [1] "Microwave Filters, Impedance-Matching Networks, and Coupling
/// Structures", George L. Matthaei, L. Young, E. M. Jones, Artech House
/// pg. 478

void DirectCoupledFilters::Synthesize_QuarterWave_Coupled_Shunt_Resonators() {
  ComponentInfo QW_TL, Lshunt, Cshunt, Ground;
  NodeInfo NI, NLeft, NRight, Ncenter;
  QPoint Pos_L, Pos_C, PosCenter; // Position of the inductor and the capacitor
                                  // in the shunt resonator

  int N = Specification.order;
  gi.pop_back();
  gi.pop_front();

  double fc = Specification.fc;
  double wc = 2 * M_PI * fc;
  double w0 = wc;
  double BW = Specification.bw;

  // Source and load impedances
  double RS = Specification.ZS;
  double RL = Specification.ZL;
  double Y0 = 1.0 / RS;

  // Fractional bandwidth calculation
  double f1 = fc - BW / 2.0;
  double f2 = fc + BW / 2.0;
  double w1 = 2 * M_PI * f1;
  double w2 = 2 * M_PI * f2;
  double w0_calc = sqrt(w1 * w2); // Geometric mean
  double w = (w2 - w1) / w0_calc; // Fractional bandwidth

  // Calculate normalized susceptances Bi/Y0 for each resonator
  std::deque<double> by(N);
  std::deque<double> Cres(N);
  std::deque<double> Lres(N);

  // First resonator - equation [1] Fig. 8.10-1 (1)
  by[0] = gi[0] * gi[1] / w - M_PI / 4.0;
  Cres[0] = by[0] * Y0 / w0;
  Lres[0] = 1.0 / (w0 * w0 * Cres[0]);

  // Middle resonators
  for (int i = 1; i < N - 1; i++) {
    if (i % 2 == 1) { // Even resonator (index starts at 0)
      // equation [1] Fig. 8.10-1 (3)
      by[i] = gi[i + 1] / (w * gi[0]) - M_PI / 2.0;
    } else { // Odd resonator
      // equation [1] Fig. 8.10-1 (2)
      by[i] = gi[i + 1] * gi[0] / w - M_PI / 2.0;
    }
    Cres[i] = by[i] * Y0 / w0;
    Lres[i] = 1.0 / (w0 * w0 * Cres[i]);
  }

  // Last resonator - equation [1] Fig. 8.10-1 (4)
  by[N - 1] = by[0];
  Cres[N - 1] = Cres[0];
  Lres[N - 1] = Lres[0];

  // Quarter wavelength in meters
  double lambda4 = SPEED_OF_LIGHT / (4.0 * fc);

  // Build schematic
  int posx = 0;

  // Source port
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(RS, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 40;

  // Main filter loop
  for (int k = 0; k < N; k++) {

    // Set components' positions
    Pos_L = QPoint(posx - 25, 60); // Inductor
    PosCenter = QPoint(posx, 0);   // Central node on the main line
    Pos_C = QPoint(posx + 25, 60); // Capacitor

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

    // Connect to previous component
    if (k == 0) {
      Schematic.appendWire(TermSpar1.ID, 0, NI.ID, 0);
    } else {
      Schematic.appendWire(QW_TL.ID, 1, NI.ID, 0);
    }

    // Shunt capacitor (left side of resonator)
    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        Pos_C);
    Cshunt.val["C"] = num2str(Cres[k], Capacitance);
    Schematic.appendComponent(Cshunt);

    // Ground for capacitor
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, Pos_C.x(), Pos_C.y() + 50);
    Schematic.appendComponent(Ground);

    // Connect capacitor GND
    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    // Shunt inductor (right side of resonator)
    Lshunt.setParams(QString("L%1").arg(++Schematic.NumberComponents[Inductor]),
                     Inductor, Pos_L);
    Lshunt.val["L"] = num2str(Lres[k], Inductance);
    Schematic.appendComponent(Lshunt);

    // Ground for inductor
    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, Pos_L.x(), Pos_L.y() + 50);
    Schematic.appendComponent(Ground);

    // Connect inductor GND
    Schematic.appendWire(Lshunt.ID, 0, Ground.ID, 0);

    // Resonator connections
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

    posx += 50;

    if (k < N - 1) {
      // Quarter-wave transmission line (coupling for the next section)
      QW_TL.setParams(
          QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
          TransmissionLine, 90, posx, 0);
      QW_TL.val["Z0"] = num2str(RS, Resistance);
      QW_TL.val["Length"] = ConvertLengthFromM("mm", lambda4);
      Schematic.appendComponent(QW_TL);

      // Connect transmission line to node
      Schematic.appendWire(NI.ID, 0, QW_TL.ID, 0);

      posx += 50;
    }
  }

  // Load port
  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(RL, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Connect last transmission line to load
  Schematic.appendWire(NI.ID, 0, TermSpar2.ID, 0);
}
