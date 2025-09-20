/***************************************************************************
                                SteppedImpedanceFilter.cpp
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

#include "SteppedImpedanceFilter.h"

SteppedImpedanceFilter::SteppedImpedanceFilter() {}

SteppedImpedanceFilter::SteppedImpedanceFilter(FilterSpecifications FS) {
  Specification = FS;
}

SteppedImpedanceFilter::~SteppedImpedanceFilter() {}

// This function synthesizes a lowpass filter using the stepped-impedance
// implementation Reference: Microwave Engineering. David M. Pozar. 4th Edition.
// 2012. John Wiley and Sons.Page 424.
void SteppedImpedanceFilter::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  std::deque<double> gi = LP_coeffs.getCoefficients();

  ComponentInfo TL;
  int N = Specification.order; // Number of elements
  int posx = 0;
  QString PreviousComponent;

  double TL_length;
  double Zlow = Specification.minZ;
  double Zhigh = Specification.maxZ;
  double Zline;
  double beta = 2 * M_PI * Specification.fc / SPEED_OF_LIGHT;

  // Add Term 1
  ComponentInfo TermSpar1(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComponent = TermSpar1.ID;

  int Kcontrol = 0;
  if (!Specification.UseZverevTables)
    Kcontrol = 0;
  if (Specification.UseZverevTables && (Specification.order % 2 == 0))
    Kcontrol = 1;
  posx += 50;
  for (int k = 0; k < N; k++) {
    if (((Specification.isCLC) && (k % 2 == Kcontrol)) ||
        ((!Specification.isCLC) && (k % 2 != Kcontrol))) {
      // Replace shunt capacitor
      Zline = Zlow;
      TL_length = gi[k + 1] * Zlow / (beta * Specification.ZS);
    } else {
      // Replace series inductor
      Zline = Zhigh;
      TL_length = gi[k + 1] * Specification.ZS / (beta * Zhigh);
    }

    // Short transmission line
    TL.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx, 0);
    TL.val["Z0"] = num2str(Zline, Resistance);
    TL.val["Length"] = ConvertLengthFromM("mm", TL_length);
    Schematic.appendComponent(TL);

    Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);

    PreviousComponent = TL.ID;
    posx += 50;
  }
  // Add Term 2
  double k = Specification.ZS;
  if (Specification.UseZverevTables)
    (!Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  else
    (Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];

  ComponentInfo TermSpar2(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx, 0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, PreviousComponent, 1);
}
