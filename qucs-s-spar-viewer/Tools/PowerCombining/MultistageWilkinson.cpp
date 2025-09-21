/***************************************************************************
                                MultistageWilkinson.cpp
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
#include "PowerCombinerDesigner.h"

void PowerCombinerDesigner::MultistageWilkinson() {
  int NStages = Specs.Nstages;
  std::vector<double> C(NStages, 0.0);
  std::vector<double> L(NStages, 0.0);
  std::deque<double> Zlines = ChebyshevTaper(2 * Specs.Z0, 0.05);
  ComponentInfo TermSpar1, TermSpar2, TermSpar3;
  ComponentInfo Ground;
  ComponentInfo Cshunt, Lseries;
  ComponentInfo TL, TL_Upper, TL_Lower;
  NodeInfo NI, Nupper, Nlower, Nupper_, Nlower_;

  if (Specs.Implementation == "Lumped LC") // CLC pi equivalent calculation
  {
    double w = 2 * M_PI * Specs.freq;
    for (int i = 0; i < NStages; i++) {
      L[i] = Zlines[i] / w;
      C[i] = 1. / (L[i] * w * w);
    }
  }

  double alpha = log(
      pow(0.1 * Specs.alpha, 10)); // Alpha is given in dB/m, then it is
                                   // necessary to convert it into Np/m units
  double lambda4 = SPEED_OF_LIGHT / (4. * Specs.freq);
  std::complex<double> gamma(
      alpha, 2 * M_PI * Specs.freq /
                 SPEED_OF_LIGHT); // It is only considered the attenation of the
                                  // metal conductor since it tends to be much
                                  // higher than the dielectric
  std::deque<double> Risol =
      calcMultistageWilkinsonIsolators(Zlines, lambda4, gamma);

  // Build the schematic
  int posx = 0;
  int posy;   // The position of the upper and the lower branches vary depending
              // on the type of implementation
  int Ni = 0; // Node index
  (Specs.Implementation == "Lumped LC") ? posy = 75 : posy = 50;

  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  posx += 50;
  if (Specs.Implementation ==
      "Lumped LC") // LC elements. Pi CLC equivalent of a lambda/4 line
  {
    // Shunt capacitor
    Cshunt.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        0, posx, 20);
    Cshunt.val["C"] = num2str(2 * C[0], Capacitance);
    Schematic.appendComponent(Cshunt);

    Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                     GND, 0, posx, 60);
    Schematic.appendComponent(Ground);

    Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 25, 0);
    Schematic.appendNode(NI);

    Schematic.appendWire(Cshunt.ID, 1, NI.ID, 0);
    Schematic.appendWire(NI.ID, 1, TermSpar1.ID, 0);

    posx += 20;
  } else { // Ideal transmission lines
    // 1st transmission line
    TL.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx, 0);
    TL.val["Z0"] = num2str(Specs.Z0, Resistance);
    TL.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
    Schematic.appendComponent(TL);

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 25, 0);
    Schematic.appendNode(NI);

    Schematic.appendWire(TermSpar1.ID, 0, TL.ID, 0);
    Schematic.appendWire(TL.ID, 1, NI.ID, 0);
    Ni++;
  }

  for (int i = 0; i < Specs.Nstages; i++) {
    if (Specs.Implementation ==
        "Lumped LC") // LC elements. Pi CLC equivalent of a lambda/4 line
    {
      double C_;
      if (i != Specs.Nstages - 1)
        C_ = C[i] + C[i + 1];
      else
        C_ = C[i];
      // Upper branch
      posx += 50;

      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, posx, -75);
      Lseries.val["L"] = num2str(L[i], Inductance);
      Schematic.appendComponent(Lseries);

      posx += 50;

      // Cshunt
      Cshunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0, posx, -50);
      Cshunt.val["C"] = num2str(C_, Capacitance);
      Schematic.appendComponent(Cshunt);

      Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, posx, -10);
      Schematic.appendComponent(Ground);

      // Capacitor to ground
      Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

      if (i > 0) { // Connect the current section to the previous one
        Schematic.appendWire(Lseries.ID, 1, Nupper.ID, 0);
      } else {
        if (i == 0) { // Connect to the common node
          Schematic.appendWire(Lseries.ID, 1, NI.ID, 0);
        }
      }

      // Node
      Nupper.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx + 50, -75);
      Schematic.appendNode(Nupper);

      // Node
      Nupper_.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, -75);
      Schematic.appendNode(Nupper_);

      // Capacitor to node
      Schematic.appendWire(Cshunt.ID, 1, Nupper_.ID, 0);

      // Inductor to node
      Schematic.appendWire(Nupper_.ID, 0, Lseries.ID, 0);

      // Nupper to Nupper_
      Schematic.appendWire(Nupper_.ID, 0, Nupper.ID, 0);

      posx -= 50;
      // Lower branch
      Lseries.setParams(
          QString("L%1").arg(++Schematic.NumberComponents[Inductor]), Inductor,
          -90, posx, 75);
      Lseries.val["L"] = num2str(L[i], Inductance);
      Schematic.appendComponent(Lseries);

      posx += 50;
      // Cshunt
      Cshunt.setParams(
          QString("C%1").arg(++Schematic.NumberComponents[Capacitor]),
          Capacitor, 0, posx, 100);
      Schematic.appendComponent(Cshunt);

      Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                       GND, 0, posx, 140);
      Schematic.appendComponent(Ground);

      // Capacitor to ground
      Schematic.appendWire(Cshunt.ID, 0, Ground.ID, 0);

      if (i > 0) { // Connect the current section to the previous one
        Schematic.appendWire(Lseries.ID, 1, Nlower.ID, 0);
      } else { // Connect to the common node
        Schematic.appendWire(Lseries.ID, 1, NI.ID, 0);
      }
      // Node
      Nlower.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx + 50, 75);
      Schematic.appendNode(Nlower);

      Nlower_.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx, 75);
      Schematic.appendNode(Nlower_);

      // Capacitor to node
      Schematic.appendWire(Cshunt.ID, 1, Nlower_.ID, 0);

      // Inductor to capacitor
      Schematic.appendWire(Nlower_.ID, 0, Lseries.ID, 0);

      // Nlower to Nlower_
      Schematic.appendWire(Nlower_.ID, 0, Nlower.ID, 0);

    } else { // Ideal TL
      posx += 50;
      // Upper branch TL
      // 1st transmission line
      TL_Upper.setParams(
          QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
          TransmissionLine, 90, posx + 15, -50);
      TL_Upper.val["Z0"] = num2str(Zlines[i], Resistance);
      TL_Upper.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
      Schematic.appendComponent(TL_Upper);

      if (i > 0) { // Connect the current section to the previous one
        Schematic.appendWire(TL_Upper.ID, 0, Nupper.ID, 0);
      }

      // Node
      Nupper.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx + 50, -50);
      Schematic.appendNode(Nupper);

      Schematic.appendWire(TL_Upper.ID, 1, Nupper.ID, 0);

      // Lower branch TL
      // 1st transmission line
      TL_Lower.setParams(
          QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
          TransmissionLine, 90, posx + 15, 50);
      TL_Lower.val["Z0"] = num2str(Zlines[i], Resistance);
      TL_Lower.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
      Schematic.appendComponent(TL_Lower);

      if (i > 0) { // Connect the current section to the previous one
        Schematic.appendWire(TL_Lower.ID, 0, Nlower.ID, 0);
      }

      // Node
      Nlower.setParams(
          QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
          posx + 50, 50);
      Schematic.appendNode(Nlower);

      Schematic.appendWire(TL_Lower.ID, 1, Nlower.ID, 0);

      if (i == 0) { // First section, the branches must be connected to the
                    // common node
        Schematic.appendWire(NI.ID, 0, TL_Upper.ID, 0);
        Schematic.appendWire(NI.ID, 0, TL_Lower.ID, 0);
      }
    }

    posx += 50;
    // Isolation resistor
    ComponentInfo Riso(
        QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0,
        posx, 0);
    Riso.val["R"] = num2str(Risol[i], Resistance);
    Schematic.appendComponent(Riso);

    Schematic.appendWire(Riso.ID, 1, Nupper.ID, 0);
    Schematic.appendWire(Riso.ID, 0, Nlower.ID, 0);
    Ni += 2;
  }

  posx += 50;

  // Add the output terminals
  // Upper branch term
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx, -posy);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  Schematic.appendWire(TermSpar2.ID, 0, Nupper.ID, 0);

  // Lower branch term
  TermSpar3.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx, posy);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);
  Schematic.appendWire(TermSpar3.ID, 0, Nlower.ID, 0);
}

// This function calculates the isolation resistors given the impedance of the
// quarter wave lines
std::deque<double> PowerCombinerDesigner::calcMultistageWilkinsonIsolators(
    std::deque<double> Zlines, double L, std::complex<double> gamma) {
  int NStages = Specs.Nstages;
  double Z_, R, Zaux = Zlines[NStages - 1];
  std::deque<double> Risol;

  for (int i = 0; i < NStages; i++) {
    Z_ = abs(Zaux * (Specs.Z0 + Zaux * tanh(gamma * L)) /
             (Zaux + Specs.Z0 * tanh(gamma * L)));
    Zaux = Zlines[i];
    R = Specs.Z0 * Z_ / (Z_ - Specs.Z0);
    Risol.push_front(2 * R);
  }
  return Risol;
}
