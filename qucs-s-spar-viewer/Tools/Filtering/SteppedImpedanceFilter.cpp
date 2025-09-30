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
  ComponentInfo MStep;

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

           // Transmission line
    if (Specification.TL_implementation == TransmissionLineType::Ideal){
      // Ideal
      TL.setParams(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, posx, 0);
      TL.val["Z0"] = num2str(Zline, Resistance);
      TL.val["Length"] = ConvertLengthFromM("mm", TL_length);

      Schematic.appendComponent(TL);
      Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);

    } else {
      if (Specification.TL_implementation == TransmissionLineType::MLIN){

        // Check that the previous component is not a term. In that case, it was a microstrip line of different width
        // and a step need to be added to model the transition
        if (!PreviousComponent.startsWith("T")) {

          posx += 10; // Add some extra room

          MStep.ID = QString("MSTEP%1").arg(++Schematic.NumberComponents[MicrostripStep]);
          MStep.Type = MicrostripStep;
          MStep.Rotation = 0;
          MStep.Coordinates = {static_cast<double>(posx), 0};

          // Add its properties
          MStep.val["W1"] = TL.val["Width"]; // Last microstrip line props are still in TL variable
          // MStep.val["W2"] = <This needs to be calculated later>

          // Substrate-related parameters
          MStep.val["er"] = num2str(Specification.MS_Subs.er);
          MStep.val["h"] = num2str(Specification.MS_Subs.height);
          MStep.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
          MStep.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
          MStep.val["tand"] = num2str(Specification.MS_Subs.tand);

          posx += 60; // Advance the x-axis drawing index
        }


        // Microstrip line
        // Synthesize MS parameters
        MicrostripClass MSL;

        MSL.Substrate = Specification.MS_Subs;
        MSL.synthesizeMicrostrip(Zline, TL_length*1e3, Specification.fc);

        double MS_Width = MSL.Results.width; // MicrostripClass calculations are in mm. It's needed to convert to m
        double MS_Length = MSL.Results.length*1e-3;

        // Instantiate component
        TL.setParams(QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]), MicrostripLine, 90, posx, 0);
        TL.val["Width"] = ConvertLengthFromM("mm", MS_Width);
        TL.val["Length"] = ConvertLengthFromM("mm", MS_Length);

        // Substrate-related parameters
        TL.val["er"] = num2str(Specification.MS_Subs.er);
        TL.val["h"] = num2str(Specification.MS_Subs.height);
        TL.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
        TL.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
        TL.val["tand"] = num2str(Specification.MS_Subs.tand);
        Schematic.appendComponent(TL);

        if (!PreviousComponent.startsWith("T")) {
          // At this point, the width of the microstrip line is calculated, so the second width of the step can be
          // assigned. At this point the component is also added to the schematic and routed
          MStep.val["W2"] = TL.val["Width"];
          Schematic.appendComponent(MStep);

          // Connections
          Schematic.appendWire(PreviousComponent, 1, MStep.ID, 0);
          Schematic.appendWire(MStep.ID, 1, TL.ID, 0);

        } else {
          // The microstrip line is connected to the port
          Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);
        }
      }
    }

    PreviousComponent = TL.ID;
    posx += 50;
  }
  // Add Term 2
  double k = Specification.ZS;
  if (Specification.UseZverevTables) {
    (!Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  } else {
    (Specification.isCLC) ? k /= gi[N + 1] : k *= gi[N + 1];
  }

  ComponentInfo TermSpar2(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx, 0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, PreviousComponent, 1);
}
