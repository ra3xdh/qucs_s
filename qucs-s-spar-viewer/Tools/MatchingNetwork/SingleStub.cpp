/***************************************************************************
                               SingleStub.cpp
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
#include "SingleStub.h"

SingleStub::SingleStub() {}

SingleStub::SingleStub(MatchingNetworkDesignParameters AS, double freq) {
  Specs = AS;
  f_match = freq;
}

SingleStub::~SingleStub() {}

// Reference: 'Microwave Engineering'. David Pozar. John Wiley and Sons. 4th
// Edition. Pg 234-241
void SingleStub::synthesize() {
  // Port 1
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]), ComplexImpedance, 0, 175, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

  // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 175, 100);
  Schematic.appendComponent(GND_ZL);

  // Design equations from single stub method
  double lambda = SPEED_OF_LIGHT / f_match;
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double XL = Specs.ZL.imag();

  double t = 0, t1 = 0, t2 = 0;
  double dl, dl1, dl2, B;
  double B1, B2, d, lstub, ll;

  ComponentInfo TLine, Stub;
  NodeInfo NI1;

  // Stub+line method formulas
  if (RL == Z0) {
    t = -XL / (2 * Z0);
    (t < 0) ? dl = (M_PI + atan(t)) / (2 * M_PI) : dl = (atan(t)) / (2 * M_PI);
    B = (RL * RL * t - (Z0 - XL * t) * (Z0 * t + XL)) /
        (Z0 * (RL * RL + (Z0 * t + XL) * (Z0 * t + XL)));
  } else {
    t1 = (XL + sqrt(((RL / Z0) * fabs((Z0 - RL) * (Z0 - RL) + XL * XL)))) /
         (RL - Z0);
    (t1 < 0) ? dl1 = (M_PI + atan(t1)) / (2 * M_PI) : dl1 = (atan(t1)) / (2 * M_PI);
    B1 = (RL * RL * t1 - (Z0 - XL * t1) * (Z0 * t1 + XL)) /
         (Z0 * (RL * RL + (Z0 * t1 + XL) * (Z0 * t1 + XL)));

    t2 = (XL - sqrt((RL * fabs((Z0 - RL) * (Z0 - RL) + XL * XL)) / (Z0))) /
         (RL - Z0);
    (t2 < 0) ? dl2 = (M_PI + atan(t2)) / (2 * M_PI) : dl2 = (atan(t2)) / (2 * M_PI);
    B2 = (RL * RL * t2 - (Z0 - XL * t2) * (Z0 * t2 + XL)) /
         (Z0 * (RL * RL + (Z0 * t2 + XL) * (Z0 * t2 + XL)));
  }

  if (t != 0) {
    d = dl * lambda;
    (!Specs.OpenShort) ? ll = -(atan(B * Z0)) / (2 * M_PI)
                 : ll = (atan(1. / (B * Z0))) / (2 * M_PI);
    if ((!Specs.OpenShort) && (ll < 0))
      ll += 0.5;
    if ((Specs.OpenShort) && (ll > 0.5))
      ll -= 0.5;
    lstub = ll * lambda;
  }

  if (t1 != 0) {
    d = dl1 * lambda;
    (!Specs.OpenShort) ? ll = -(atan(B1 * Z0)) / (2 * M_PI)
                 : ll = (atan(1. / (1. * B1 * Z0))) / (2 * M_PI);
    if ((!Specs.OpenShort) && (ll < 0))
      ll += 0.5;
    if ((Specs.OpenShort) && (ll > 0.5))
      ll -= 0.5;
    lstub = ll * lambda;

  } else {
    if (t2 != 0) {
      d = dl2 * lambda;
      (!Specs.OpenShort) ? ll = -(atan(B2 * Z0)) / (2 * M_PI)
                   : ll = (atan(1. / (1. * B2 * Z0))) / (2 * M_PI);
      if ((!Specs.OpenShort) && (ll < 0))
        ll += 0.5;
      if ((Specs.OpenShort) && (ll > 0.5))
        ll -= 0.5;
      lstub = ll * lambda;
    }
  }

  // Node between the input port and stub
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

  // Create stub component
  if (!Specs.OpenShort) { // Open stub
    Stub.setParams(
        QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]), OpenStub,
        0, 50, 50);
    Stub.val["Z0"] = num2str(Z0, Resistance);
    Stub.val["Length"] = ConvertLengthFromM("mm", lstub);
  } else { // Short stub
    Stub.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]), ShortStub,
        0, 50, 50);
    Stub.val["Z0"] = num2str(Z0, Resistance);
    Stub.val["Length"] = ConvertLengthFromM("mm", lstub);
  }
  Schematic.appendComponent(Stub);


  // Create transmission line component
  TLine.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, -90, 100, 0);
  TLine.val["Z0"] = num2str(Z0, Resistance);
  TLine.val["Length"] = ConvertLengthFromM("mm", d);
  Schematic.appendComponent(TLine);

  // Wires
  Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
  Schematic.appendWire(TLine.ID, 1, NI1.ID, 0);
  Schematic.appendWire(NI1.ID, 0, Stub.ID, 1);
  Schematic.appendWire(Zload.ID, 1, TLine.ID, 0);
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}
