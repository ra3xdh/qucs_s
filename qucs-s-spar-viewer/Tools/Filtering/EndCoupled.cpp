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

#include "EndCoupled.h"

EndCoupled::EndCoupled() {}

EndCoupled::EndCoupled(FilterSpecifications FS) { Specification = FS; }

EndCoupled::~EndCoupled() {}

// This function synthesizes an end-coupled bandpass filter
// implementation Reference: Microstrip filters for RF/Microwave Applications.
// Jia-Sheng Hong. M. J. Lancaster. 2001. John Wiley and Sons. Pages 121-123.
void EndCoupled::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  std::deque<double> gi = LP_coeffs.getCoefficients();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildFilter_IdealTL(gi);
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildFilter_Microstrip(gi);
  }
}

void EndCoupled::buildFilter_IdealTL(const std::deque<double> &gi) {
  ComponentInfo TL, Cseries;

  int N = Specification.order;
  int posx = 0;
  QString PreviousComponent;

  double TL_length, theta, Baux = 0;
  double bw = Specification.bw / Specification.fc;
  double w0 = 2 * M_PI * Specification.fc;
  double B, J, C, Z0 = Specification.ZS;
  double lambda_g0 = SPEED_OF_LIGHT / Specification.fc;

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComponent = TermSpar1.ID;

  posx += 50;
  for (int k = 0; k <= N; k++) {
    if (k == 0) {
      J = sqrt(.5 * M_PI * bw / (gi[0] * gi[1]));
    } else {
      if (k == N) {
        J = sqrt(.5 * M_PI * bw / (gi[N] * gi[N + 1]));
      } else {
        J = .5 * M_PI * bw / sqrt(gi[k] * gi[k + 1]);
      }
    }

    B = J / (1 - J * J);
    theta = M_PI - .5 * (atan(2 * Baux) + atan(2 * B));

    Baux = B;
    J /= Z0;
    B /= Z0;

    C = B / w0;
    TL_length = theta * lambda_g0 / (2 * M_PI);

    if (k > 0) {
      // Transmission line
      TL.setParams(
          QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
          TransmissionLine, 90, posx, 0);
      TL.val["Z0"] = num2str(Z0, Resistance);
      TL.val["Length"] = ConvertLengthFromM("mm", TL_length);

      Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);
      Schematic.appendComponent(TL);
      PreviousComponent = TL.ID;
      posx += 50;
    }

    // Series capacitor
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(C, Capacitance);
    Schematic.appendComponent(Cseries);

    Schematic.appendWire(Cseries.ID, 0, PreviousComponent, 1);

    PreviousComponent = Cseries.ID;
    posx += 50;
  }

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

  Schematic.appendWire(TermSpar2.ID, 0, PreviousComponent, 1);
}

void EndCoupled::buildFilter_Microstrip(const std::deque<double> &gi) {
  ComponentInfo TL, Cseries;

  int N = Specification.order;
  int posx = 0;
  QString PreviousComponent;

  double TL_length, theta, Baux = 0;
  double bw = Specification.bw / Specification.fc;
  double w0 = 2 * M_PI * Specification.fc;
  double B, J, C, Z0 = Specification.ZS;
  double lambda_g0 = SPEED_OF_LIGHT / Specification.fc;

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComponent = TermSpar1.ID;

  posx += 50;
  for (int k = 0; k <= N; k++) {
    if (k == 0) {
      J = sqrt(.5 * M_PI * bw / (gi[0] * gi[1]));
    } else {
      if (k == N) {
        J = sqrt(.5 * M_PI * bw / (gi[N] * gi[N + 1]));
      } else {
        J = .5 * M_PI * bw / sqrt(gi[k] * gi[k + 1]);
      }
    }

    B = J / (1 - J * J);
    theta = M_PI - .5 * (atan(2 * Baux) + atan(2 * B));

    Baux = B;
    J /= Z0;
    B /= Z0;

    C = B / w0;
    TL_length = theta * lambda_g0 / (2 * M_PI);

    if (k > 0) {
      // Microstrip transmission line
      MicrostripClass MSL;
      MSL.Substrate = Specification.MS_Subs;
      MSL.synthesizeMicrostrip(Z0, TL_length * 1e3, Specification.fc);

      double MS_Width = MSL.Results.width;
      double MS_Length = MSL.Results.length * 1e-3;

      TL.setParams(
          QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
          MicrostripLine, 90, posx, 0);
      TL.val["Width"] = ConvertLengthFromM("mm", MS_Width);
      TL.val["Length"] = ConvertLengthFromM("mm", MS_Length);
      TL.val["er"] = num2str(Specification.MS_Subs.er);
      TL.val["h"] = num2str(Specification.MS_Subs.height);
      TL.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
      TL.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
      TL.val["tand"] = num2str(Specification.MS_Subs.tand);

      Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);
      Schematic.appendComponent(TL);
      PreviousComponent = TL.ID;
      posx += 50;
    }

    // Series capacitor
    Cseries.setParams(
        QString("C%1").arg(++Schematic.NumberComponents[Capacitor]), Capacitor,
        90, posx, 0);
    Cseries.val["C"] = num2str(C, Capacitance);
    Schematic.appendComponent(Cseries);

    Schematic.appendWire(Cseries.ID, 0, PreviousComponent, 1);

    PreviousComponent = Cseries.ID;
    posx += 50;
  }

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

  Schematic.appendWire(TermSpar2.ID, 0, PreviousComponent, 1);
}
