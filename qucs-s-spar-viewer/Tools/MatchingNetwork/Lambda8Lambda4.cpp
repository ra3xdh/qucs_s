/***************************************************************************
                               Lambda8Lambda4.cpp
                               ----------------------
    copyright            :  QUCS team
    author               :  2025: Andrés Martínez Mera
    email                :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/

#include "Lambda8Lambda4.h"
#include <cmath>

Lambda8Lambda4::Lambda8Lambda4() {}
Lambda8Lambda4::~Lambda8Lambda4() {}
Lambda8Lambda4::Lambda8Lambda4(MatchingNetworkDesignParameters AS, double freq) {
  Specs = AS;
  f_match = freq;
}

void Lambda8Lambda4::synthesize() {
    // Port 1
    ComponentInfo TermSpar1(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 0, 0);
    TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
    Schematic.appendComponent(TermSpar1);


    // Design equations
    // Reference: Inder J. Bahl. "Fundamentals of RF and microwave transistor amplifiers".
    // John Wiley and Sons. 2009. Pages 159 - 160
    double Z0 = Specs.Z0;
    double RL = Specs.ZL.real();
    double XL = Specs.ZL.imag();

    double l4 = SPEED_OF_LIGHT / (4. * f_match);
    double l8 = 0.5 * l4;

    // Equivalent matching impedances
    double Zmm = std::sqrt(RL * RL + XL * XL);
    double Zm = std::sqrt((Z0 * RL * Zmm) / (Zmm - XL));

    int x_pos = 50; // x-axis position of the component

    ComponentInfo TL2;

    // First transmission line: Zm, length λ/4
    ComponentInfo TL1(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, x_pos, 0);
    TL1.val["Z0"] = num2str(Zm, Resistance);
    TL1.val["Length"] = ConvertLengthFromM("mm", l4);
    Schematic.appendComponent(TL1);

    if (XL != 0) {
      // Second transmission line: Zmm, length λ/8
      // It's needed as long as the load is complex to correct the imaginary part

      x_pos += 50;
      TL2.setParams(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, x_pos, 0);
      TL2.val["Z0"] = num2str(Zmm, Resistance);
      TL2.val["Length"] = ConvertLengthFromM("mm", l8);
      Schematic.appendComponent(TL2);
    }

    // Load

    x_pos += 50;
    ComponentInfo Zload(QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]), ComplexImpedance, 0, x_pos, 50);
    Zload.val["Z"] = num2str(Specs.ZL, Resistance);
    Schematic.appendComponent(Zload);

           // GND for load
    ComponentInfo GND_ZL;
    GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]), GND, 0, x_pos, 100);
    Schematic.appendComponent(GND_ZL);

    // --- Wiring ---
    Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);
    if (XL != 0) {
      Schematic.appendWire(TL1.ID, 1, TL2.ID, 0);
      Schematic.appendWire(Zload.ID, 1, TL2.ID, 1);
    } else {
      Schematic.appendWire(Zload.ID, 1, TL1.ID, 1);
    }
    Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}

