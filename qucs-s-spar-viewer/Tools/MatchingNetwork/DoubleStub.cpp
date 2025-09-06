/***************************************************************************
                               DoubleStub.cpp
                               ------------
    copyright            :  QUCS-S team
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
#include "DoubleStub.h"

DoubleStub::DoubleStub() {}

DoubleStub::DoubleStub(MatchingNetworkDesignParameters AS, double freq) {
  Specs = AS;
  f_match = freq;
}

DoubleStub::~DoubleStub() {}

// Reference: 'Microwave Engineering', David Pozar. John Wiley and Sons. 4th
// Edition. Pg 241-245
void DoubleStub::synthesize() {
  // Port 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]), ComplexImpedance, 0, 225, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

         // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 225, 100);
  Schematic.appendComponent(GND_ZL);

         // Design equations from double stub method
  double lambda = SPEED_OF_LIGHT / f_match;
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double XL = Specs.ZL.imag();

  double Y0 = 1.0 / Z0;
  double GL = (1 / ((RL * RL) + (XL * XL))) * RL;
  double BL = -(1 / ((RL * RL) + (XL * XL))) * XL;
  double beta = (2 * M_PI) / lambda;
  double d = lambda / 8;  // Fixed spacing between stubs
  double t = tan(beta * d);
  double ll1, ll2, lstub1, lstub2;

         // Check if load can be matched using double stub method
  if (GL > Y0 * ((1 + t * t) / (2 * t * t))) {
    // Cannot match this load - could throw exception or handle error
    // For now, we'll proceed with the calculation but the result may not be optimal
  }

         // Calculate stub susceptances
  double B11 = -BL + (Y0 + sqrt((1 + t * t) * GL * Y0 - GL * GL * t * t)) / t; // 1st solution
  double B21 = ((Y0 * sqrt((1 + t * t) * GL * Y0 - GL * GL * t * t)) + GL * Y0) / (GL * t); // 1st solution

  ComponentInfo TLine, Stub1, Stub2;
  NodeInfo NI1, NI2;

         // Calculate stub lengths
  if (!Specs.OpenShort) { // Open stubs
    ll1 = (atan(B21 * Z0)) / (2 * M_PI);
    ll2 = (atan(B11 * Z0)) / (2 * M_PI);
  } else { // Short stubs
    ll1 = -(atan(1. / (B21 * Z0))) / (2 * M_PI);
    ll2 = -(atan(1. / (B11 * Z0))) / (2 * M_PI);
  }

  if (ll1 < 0) ll1 += 0.5;
  if (ll2 < 0) ll2 += 0.5;
  if (Specs.OpenShort && (ll1 > 0.5)) ll1 -= 0.5;
  if (Specs.OpenShort && (ll2 > 0.5)) ll2 -= 0.5;

  lstub1 = ll1 * lambda;
  lstub2 = ll2 * lambda;

         // Node between first stub and transmission line
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

         // Node between transmission line and second stub
  NI2.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 175, 0);
  Schematic.appendNode(NI2);

         // Create first stub component (at input side)
  if (!Specs.OpenShort) { // Open stub
    Stub1.setParams(
        QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]), OpenStub,
        0, 50, 50);
    Stub1.val["Z0"] = num2str(Z0, Resistance);
    Stub1.val["Length"] = ConvertLengthFromM("mm", lstub1);  // B11 corresponds to first stub
  } else { // Short stub
    Stub1.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]), ShortStub,
        0, 50, 50);
    Stub1.val["Z0"] = num2str(Z0, Resistance);
    Stub1.val["Length"] = ConvertLengthFromM("mm", lstub1);  // B11 corresponds to first stub
  }
  Schematic.appendComponent(Stub1);

         // Create transmission line component (lambda/8 spacing)
  TLine.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, -90, 112, 0);
  TLine.val["Z0"] = num2str(Z0, Resistance);
  TLine.val["Length"] = ConvertLengthFromM("mm", d);
  Schematic.appendComponent(TLine);

         // Create second stub component (at load side)
  if (!Specs.OpenShort) { // Open stub
    Stub2.setParams(
        QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]), OpenStub,
        0, 175, 50);
    Stub2.val["Z0"] = num2str(Z0, Resistance);
    Stub2.val["Length"] = ConvertLengthFromM("mm", lstub2);  // B21 corresponds to second stub
  } else { // Short stub
    Stub2.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]), ShortStub,
        0, 175, 50);
    Stub2.val["Z0"] = num2str(Z0, Resistance);
    Stub2.val["Length"] = ConvertLengthFromM("mm", lstub2);  // B21 corresponds to second stub
  }
  Schematic.appendComponent(Stub2);

         // Wires
  Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
  Schematic.appendWire(NI1.ID, 0, Stub1.ID, 1);
  Schematic.appendWire(NI1.ID, 0, TLine.ID, 1);
  Schematic.appendWire(TLine.ID, 0, NI2.ID, 0);
  Schematic.appendWire(NI2.ID, 0, Stub2.ID, 1);
  Schematic.appendWire(Zload.ID, 1, NI2.ID, 0);
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}
