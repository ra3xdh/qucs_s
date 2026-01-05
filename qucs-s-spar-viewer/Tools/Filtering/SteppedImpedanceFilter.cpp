/// @file SteppedImpedanceFilter.cpp
/// @brief Synthesis of stepped impdance filters (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "SteppedImpedanceFilter.h"

// This function synthesizes a lowpass filter using the stepped-impedance
// implementation Reference: Microwave Engineering. David M. Pozar. 4th Edition.
// 2012. John Wiley and Sons.Page 424.
void SteppedImpedanceFilter::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  std::deque<double> gi = LP_coeffs.getCoefficients();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildFilter_IdealTL(gi);
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildFilter_Microstrip(gi);
  }
}

void SteppedImpedanceFilter::buildFilter_IdealTL(const std::deque<double> &gi) {
  int N = Specification.order; // Number of elements
  int posx = 0;
  double beta = 2 * M_PI * Specification.fc / SPEED_OF_LIGHT;
  double Zlow = Specification.minZ;
  double Zhigh = Specification.maxZ;

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);
  QString PreviousComponent = TermSpar1.ID;

  // Determine control parameter
  int Kcontrol = 0;
  if (Specification.UseZverevTables && (Specification.order % 2 == 0)) {
    Kcontrol = 1;
  }

  posx += 50;

  // Create transmission line sections
  for (int k = 0; k < N; k++) {
    double Zline, TL_length;

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

    // Create ideal transmission line
    ComponentInfo TL;
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

void SteppedImpedanceFilter::buildFilter_Microstrip(
    const std::deque<double> &gi) {
  int N = Specification.order; // Number of elements
  int posx = 0;
  double beta = 2 * M_PI * Specification.fc / SPEED_OF_LIGHT;
  double Zlow = Specification.minZ;
  double Zhigh = Specification.maxZ;

  // Add Term 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Specification.ZS, Resistance);
  Schematic.appendComponent(TermSpar1);
  QString PreviousComponent = TermSpar1.ID;

  // Determine control parameter
  int Kcontrol = 0;
  if (Specification.UseZverevTables && (Specification.order % 2 == 0)) {
    Kcontrol = 1;
  }

  posx += 50;
  ComponentInfo TL; // Declared outside loop to access last width

  // Create microstrip transmission line sections
  for (int k = 0; k < N; k++) {
    double Zline, TL_length;

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

    // Add step transition if not the first section
    ComponentInfo MStep;
    if (!PreviousComponent.startsWith("T")) {
      posx += 10; // Add some extra room

      MStep.ID =
          QString("MSTEP%1").arg(++Schematic.NumberComponents[MicrostripStep]);
      MStep.Type = MicrostripStep;
      MStep.Rotation = 0;
      MStep.Coordinates = {static_cast<double>(posx), 0};

      // Add its properties
      MStep.val["W1"] = TL.val["Width"]; // Last microstrip line props are still
                                         // in TL variable
      // MStep.val["W2"] will be set after next microstrip line is synthesized

      // Substrate-related parameters
      MStep.val["er"] = num2str(Specification.MS_Subs.er);
      MStep.val["h"] = num2str(Specification.MS_Subs.height);
      MStep.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
      MStep.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
      MStep.val["tand"] = num2str(Specification.MS_Subs.tand);

      posx += 60; // Advance the x-axis drawing index
    }

    // Synthesize microstrip parameters
    MicrostripClass MSL;
    MSL.Substrate = Specification.MS_Subs;
    MSL.synthesizeMicrostrip(Zline, TL_length * 1e3, Specification.fc);

    double MS_Width = MSL.Results.width;
    double MS_Length = MSL.Results.length * 1e-3;

    // Create microstrip line component
    TL.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx, 0);
    TL.val["Width"] = ConvertLengthFromM("mm", MS_Width);
    TL.val["Length"] = ConvertLengthFromM("mm", MS_Length);

    // Substrate-related parameters
    TL.val["er"] = num2str(Specification.MS_Subs.er);
    TL.val["h"] = num2str(Specification.MS_Subs.height);
    TL.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    TL.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    TL.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(TL);

    // Connect components
    if (!PreviousComponent.startsWith("T")) {
      // Complete step configuration and connect
      MStep.val["W2"] = TL.val["Width"];
      Schematic.appendComponent(MStep);

      // Connections
      Schematic.appendWire(PreviousComponent, 1, MStep.ID, 0);
      Schematic.appendWire(MStep.ID, 1, TL.ID, 0);
    } else {
      // Connect directly to port
      Schematic.appendWire(PreviousComponent, 1, TL.ID, 0);
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

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, posx,
      0);
  TermSpar2.val["Z"] = num2str(k, Resistance);
  Schematic.appendComponent(TermSpar2);
  Schematic.appendWire(TermSpar2.ID, 0, PreviousComponent, 1);
}
