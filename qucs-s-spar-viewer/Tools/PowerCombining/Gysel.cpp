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

#include "PowerCombinerDesigner.h"

void PowerCombinerDesigner::Gysel() {
  double lambda4 = SPEED_OF_LIGHT / (4 * Specs.freq);
  double lambda2 = lambda4 * 2;

  // Dispatch to appropriate implementation
  if (Specs.TL_implementation == TransmissionLineType::Ideal) {
    buildGysel_IdealTL(lambda4, lambda2);
  } else if (Specs.TL_implementation == TransmissionLineType::MLIN) {
    buildGysel_Microstrip(lambda4, lambda2);
  }
}

void PowerCombinerDesigner::buildGysel_IdealTL(double lambda4, double lambda2) {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo NSP1(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0, 0);
  Schematic.appendNode(NSP1);

  ComponentInfo TL1(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, -50);
  TL1.ID =
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]);
  TL1.val["Z0"] = num2str(sqrt(2) * Specs.Z0, Resistance);
  TL1.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL1);

  Schematic.appendWire(NSP1.ID, 0, TermSpar1.ID, 0);
  Schematic.appendWire(TL1.ID, 0, NSP1.ID, 0);

  ComponentInfo TL2(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 0, 50);
  TL2.ID =
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]);
  TL2.val["Z0"] = num2str(sqrt(2) * Specs.Z0, Resistance);
  TL2.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL2);

  Schematic.appendWire(TL2.ID, 1, NSP1.ID, 0);

  ComponentInfo TL3(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 100, -75);
  TL3.val["Z0"] = num2str(Specs.Z0, Resistance);
  TL3.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL3);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20,
      -75);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  NodeInfo NSP2(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0,
      -75);
  Schematic.appendNode(NSP2);

  Schematic.appendWire(NSP2.ID, 0, TermSpar2.ID, 0);
  Schematic.appendWire(TL1.ID, 1, NSP2.ID, 0);
  Schematic.appendWire(TL3.ID, 0, NSP2.ID, 0);

  ComponentInfo TL4(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, 100, 75);
  TL4.val["Z0"] = num2str(Specs.Z0, Resistance);
  TL4.val["Length"] = ConvertLengthFromM(Specs.units, lambda4);
  Schematic.appendComponent(TL4);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20, 75);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  NodeInfo NSP3(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0, 75);
  Schematic.appendNode(NSP3);

  Schematic.appendWire(NSP3.ID, 0, TermSpar3.ID, 0);
  Schematic.appendWire(TL2.ID, 0, NSP3.ID, 0);
  Schematic.appendWire(TL4.ID, 0, NSP3.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              150, -75);
  Schematic.appendNode(N1);

  ComponentInfo TL5(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 0, 150, 0);
  TL5.val["Z0"] = num2str(Specs.Z0 / sqrt(2), Resistance);
  TL5.val["Length"] = ConvertLengthFromM(Specs.units, lambda2);
  Schematic.appendComponent(TL5);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              150, 75);
  Schematic.appendNode(N2);

  ComponentInfo Ri1(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 200, 100);
  Ri1.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Ri1);

  ComponentInfo Ground1(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 200, 125);
  Schematic.appendComponent(Ground1);

  Schematic.appendWire(Ri1.ID, 0, Ground1.ID, 0);
  Schematic.appendWire(Ri1.ID, 1, N2.ID, 0);
  Schematic.appendWire(TL5.ID, 0, N2.ID, 0);
  Schematic.appendWire(TL4.ID, 1, N2.ID, 0);
  Schematic.appendWire(TL5.ID, 1, N1.ID, 1);

  ComponentInfo Ri2(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 200, -50);
  Ri2.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Ri2);

  ComponentInfo Ground2(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 200, 0);
  Schematic.appendComponent(Ground2);
  Schematic.appendWire(Ri2.ID, 0, Ground2.ID, 0);
  Schematic.appendWire(Ri2.ID, 1, N1.ID, 0);
  Schematic.appendWire(TL3.ID, 1, N1.ID, 0);
}

void PowerCombinerDesigner::buildGysel_Microstrip(double lambda4,
                                                  double lambda2) {
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20, 0);
  TermSpar1.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar1);

  NodeInfo NSP1(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0, 0);
  Schematic.appendNode(NSP1);

  // Synthesize microstrip lines for different impedances
  // Quarter-wave lines at sqrt(2)*Z0
  MicrostripClass MSL_Input;
  MSL_Input.Substrate = Specs.MS_Subs;
  MSL_Input.synthesizeMicrostrip(sqrt(2) * Specs.Z0, lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN1(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 0, -50);
  MLIN1.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN1.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN1.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN1.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN1.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN1.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN1.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN1);

  Schematic.appendWire(NSP1.ID, 0, TermSpar1.ID, 0);
  Schematic.appendWire(MLIN1.ID, 0, NSP1.ID, 0);

  ComponentInfo MLIN2(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 0, 50);
  MLIN2.val["Width"] = ConvertLengthFromM("mm", MSL_Input.Results.width);
  MLIN2.val["Length"] =
      ConvertLengthFromM("mm", MSL_Input.Results.length * 1e-3);
  MLIN2.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN2.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN2.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN2.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN2.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN2);

  Schematic.appendWire(MLIN2.ID, 1, NSP1.ID, 0);

  // Quarter-wave lines at Z0
  MicrostripClass MSL_Z0;
  MSL_Z0.Substrate = Specs.MS_Subs;
  MSL_Z0.synthesizeMicrostrip(Specs.Z0, lambda4 * 1e3, Specs.freq);

  ComponentInfo MLIN3(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 100, -75);
  MLIN3.val["Width"] = ConvertLengthFromM("mm", MSL_Z0.Results.width);
  MLIN3.val["Length"] = ConvertLengthFromM("mm", MSL_Z0.Results.length * 1e-3);
  MLIN3.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN3.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN3.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN3.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN3.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN3);

  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20,
      -75);
  TermSpar2.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  NodeInfo NSP2(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0,
      -75);
  Schematic.appendNode(NSP2);

  Schematic.appendWire(NSP2.ID, 0, TermSpar2.ID, 0);
  Schematic.appendWire(MLIN1.ID, 1, NSP2.ID, 0);
  Schematic.appendWire(MLIN3.ID, 0, NSP2.ID, 0);

  ComponentInfo MLIN4(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, 100, 75);
  MLIN4.val["Width"] = ConvertLengthFromM("mm", MSL_Z0.Results.width);
  MLIN4.val["Length"] = ConvertLengthFromM("mm", MSL_Z0.Results.length * 1e-3);
  MLIN4.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN4.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN4.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN4.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN4.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN4);

  ComponentInfo TermSpar3(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, -20, 75);
  TermSpar3.val["Z"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(TermSpar3);

  NodeInfo NSP3(
      QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 0, 75);
  Schematic.appendNode(NSP3);

  Schematic.appendWire(NSP3.ID, 0, TermSpar3.ID, 0);
  Schematic.appendWire(MLIN2.ID, 0, NSP3.ID, 0);
  Schematic.appendWire(MLIN4.ID, 0, NSP3.ID, 0);

  NodeInfo N1(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              150, -75);
  Schematic.appendNode(N1);

  // Half-wave line at Z0/sqrt(2)
  MicrostripClass MSL_HalfWave;
  MSL_HalfWave.Substrate = Specs.MS_Subs;
  MSL_HalfWave.synthesizeMicrostrip(Specs.Z0 / sqrt(2), lambda2 * 1e3,
                                    Specs.freq);

  ComponentInfo MLIN5(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 0, 150, 0);
  MLIN5.val["Width"] = ConvertLengthFromM("mm", MSL_HalfWave.Results.width);
  MLIN5.val["Length"] =
      ConvertLengthFromM("mm", MSL_HalfWave.Results.length * 1e-3);
  MLIN5.val["er"] = num2str(Specs.MS_Subs.er);
  MLIN5.val["h"] = num2str(Specs.MS_Subs.height);
  MLIN5.val["cond"] = num2str(Specs.MS_Subs.MetalConductivity);
  MLIN5.val["th"] = num2str(Specs.MS_Subs.MetalThickness);
  MLIN5.val["tand"] = num2str(Specs.MS_Subs.tand);
  Schematic.appendComponent(MLIN5);

  NodeInfo N2(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
              150, 75);
  Schematic.appendNode(N2);

  ComponentInfo Ri1(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 200, 100);
  Ri1.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Ri1);

  ComponentInfo Ground1(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 200, 125);
  Schematic.appendComponent(Ground1);

  Schematic.appendWire(Ri1.ID, 0, Ground1.ID, 0);
  Schematic.appendWire(Ri1.ID, 1, N2.ID, 0);
  Schematic.appendWire(MLIN5.ID, 0, N2.ID, 0);
  Schematic.appendWire(MLIN4.ID, 1, N2.ID, 0);
  Schematic.appendWire(MLIN5.ID, 1, N1.ID, 1);

  ComponentInfo Ri2(QString("R%1").arg(++Schematic.NumberComponents[Resistor]),
                    Resistor, 0, 200, -50);
  Ri2.val["R"] = num2str(Specs.Z0, Resistance);
  Schematic.appendComponent(Ri2);

  ComponentInfo Ground2(QString("GND%1").arg(++Schematic.NumberComponents[GND]),
                        GND, 0, 200, 0);
  Schematic.appendComponent(Ground2);
  Schematic.appendWire(Ri2.ID, 0, Ground2.ID, 0);
  Schematic.appendWire(Ri2.ID, 1, N1.ID, 0);
  Schematic.appendWire(MLIN3.ID, 1, N1.ID, 0);
}
