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
  double lambda = SPEED_OF_LIGHT / f_match;
  double Z0 = Specs.Z0;
  double RL = Specs.ZL.real();
  double XL = Specs.ZL.imag();

  double t = calculateT(RL, XL, Z0);
  double B = calculateSusceptance(t, RL, XL, Z0);

  double d = calculateLineDistance(t, lambda);
  double lstub = calculateStubLength(B, Z0, lambda);

  buildMatchingNetwork(d, lstub);
}

double SingleStub::calculateT(double RL, double XL, double Z0) {
  if (RL == Z0) {
    return -XL / (2 * Z0);
  }

  // Calculate both solutions
  double discriminant = sqrt((RL / Z0) * fabs((Z0 - RL) * (Z0 - RL) + XL * XL));
  double t1 = (XL + discriminant) / (RL - Z0);
  double t2 = (XL - discriminant) / (RL - Z0);

  // Prefer t1, fallback to t2 if t1 is zero
  return (t1 != 0) ? t1 : t2;
}

double SingleStub::calculateSusceptance(double t, double RL, double XL, double Z0) {
  return (RL * RL * t - (Z0 - XL * t) * (Z0 * t + XL)) /
         (Z0 * (RL * RL + (Z0 * t + XL) * (Z0 * t + XL)));
}

double SingleStub::normalizePhase(double phase) {
  return (phase < 0) ? (M_PI + phase) / (2 * M_PI) : phase / (2 * M_PI);
}

double SingleStub::calculateLineDistance(double t, double lambda) {
  double dl = normalizePhase(atan(t));
  return dl * lambda;
}

double SingleStub::calculateStubLength(double B, double Z0, double lambda) {
  double ll;

  if (!Specs.OpenShort) {  // Open stub
    ll = -atan(B * Z0) / (2 * M_PI);
    if (ll < 0) {
      ll += 0.5;
    }
  } else {  // Short stub
    ll = atan(1.0 / (B * Z0)) / (2 * M_PI);
    if (ll > 0.5) {
      ll -= 0.5;
    }
  }

  return ll * lambda;
}

void SingleStub::buildMatchingNetwork(double d, double lstub) {
  double Z0 = Specs.Z0;

  // Port 1 termination
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

         // Load impedance
  ComponentInfo Zload(
      QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]),
      ComplexImpedance, 0, 175, 50);
  Zload.val["Z"] = num2str(Specs.ZL, Resistance);
  Schematic.appendComponent(Zload);

         // GND for load
  ComponentInfo GND_ZL;
  GND_ZL.setParams(QString("GND_ZL%1").arg(++Schematic.NumberComponents[GND]),
                   GND, 0, 175, 100);
  Schematic.appendComponent(GND_ZL);

  // Node between the input port and stub
  NodeInfo NI1;
  NI1.setParams(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NI1);

         // Create stub component
  ComponentInfo Stub;
  if (!Specs.OpenShort) {  // Open stub
    Stub.setParams(
        QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]),
        OpenStub, 0, 50, 50);
  } else {  // Short stub
    Stub.setParams(
        QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
        ShortStub, 0, 50, 50);
  }
  Stub.val["Z0"] = num2str(Z0, Resistance);
  Stub.val["Length"] = ConvertLengthFromM("mm", lstub);
  Schematic.appendComponent(Stub);

         // Create transmission line component
  ComponentInfo TLine;
  TLine.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, -90, 100, 0);
  TLine.val["Z0"] = num2str(Z0, Resistance);
  TLine.val["Length"] = ConvertLengthFromM("mm", d);
  Schematic.appendComponent(TLine);

         // Connect components with wires
  Schematic.appendWire(TermSpar1.ID, 0, NI1.ID, 0);
  Schematic.appendWire(TLine.ID, 1, NI1.ID, 0);
  Schematic.appendWire(NI1.ID, 0, Stub.ID, 1);
  Schematic.appendWire(Zload.ID, 1, TLine.ID, 0);
  Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}
