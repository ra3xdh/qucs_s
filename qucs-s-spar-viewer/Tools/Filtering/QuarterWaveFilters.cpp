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

#include "QuarterWaveFilters.h"

QuarterWaveFilters::QuarterWaveFilters() {
  // Initialize list of components
  Schematic.NumberComponents[Capacitor] = 0;
  Schematic.NumberComponents[Inductor] = 0;
  Schematic.NumberComponents[Term] = 0;
  Schematic.NumberComponents[GND] = 0;
  Schematic.NumberComponents[ConnectionNodes] = 0;
}

QuarterWaveFilters::QuarterWaveFilters(FilterSpecifications FS) {
  Specification = FS;
  // Initialize list of components
  Schematic.NumberComponents[Capacitor] = 0;
  Schematic.NumberComponents[Inductor] = 0;
  Schematic.NumberComponents[Term] = 0;
  Schematic.NumberComponents[GND] = 0;
  Schematic.NumberComponents[ConnectionNodes] = 0;
}

QuarterWaveFilters::~QuarterWaveFilters() {}

void QuarterWaveFilters::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  std::deque<double> gi = LP_coeffs.getCoefficients();

  gi.pop_back();
  gi.pop_front();

  // Dispatch to appropriate implementation
  if (Specification.TL_implementation == TransmissionLineType::Ideal) {
    buildFilter_IdealTL(gi);
  } else if (Specification.TL_implementation == TransmissionLineType::MLIN) {
    buildFilter_Microstrip(gi);
  }
}

void QuarterWaveFilters::buildFilter_IdealTL(const std::deque<double> &gi) {
  ComponentInfo QW_TL, OC_Stub, SC_Stub;
  NodeInfo NI;

  double lambda4 = SPEED_OF_LIGHT / (4. * Specification.fc);
  int N = Specification.order;
  double fc = Specification.fc;
  double BW = Specification.bw;
  double bw = BW / fc;
  double Z0 = Specification.ZS;

  int posx = 0;
  QString PreviousComp;

  // Input port
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Z0, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComp = TermSpar1.ID;
  posx -= 50;

  // Build filter sections
  for (int k = 0; k < N; k++) {
    posx += 100;

    // Quarter-wave transmission line
    QW_TL.setParams(
        QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
        TransmissionLine, 90, posx, 0);
    QW_TL.val["Z0"] = num2str(Z0, Resistance);
    QW_TL.val["Length"] = ConvertLengthFromM("mm", lambda4);
    Schematic.appendComponent(QW_TL);

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, 0);
    Schematic.appendNode(NI);

    // Wires
    Schematic.appendWire(PreviousComp, 0, QW_TL.ID, 0);
    Schematic.appendWire(NI.ID, 0, QW_TL.ID, 1);

    // Stubs
    double Z;
    if (Specification.FilterType == Bandpass) {
      Z = (M_PI * Z0 * bw) / (4 * gi[k]);

      SC_Stub.setParams(
          QString("SSTUB%1").arg(++Schematic.NumberComponents[ShortStub]),
          ShortStub, 0, posx + 50, 50);
      SC_Stub.val["Z0"] = num2str(Z, Resistance);
      SC_Stub.val["Length"] = ConvertLengthFromM("mm", lambda4);
      Schematic.appendComponent(SC_Stub);
      Schematic.appendWire(NI.ID, 0, SC_Stub.ID, 1);

    } else if (Specification.FilterType == Bandstop) {
      Z = (4 * Z0) / (M_PI * bw * gi[k]);

      OC_Stub.setParams(
          QString("OSTUB%1").arg(++Schematic.NumberComponents[OpenStub]),
          OpenStub, 0, posx + 50, 50);
      OC_Stub.val["Z0"] = num2str(Z, Resistance);
      OC_Stub.val["Length"] = ConvertLengthFromM("mm", lambda4);
      Schematic.appendComponent(OC_Stub);
      Schematic.appendWire(NI.ID, 0, OC_Stub.ID, 1);
    }

    PreviousComp = NI.ID;
  }

  // Final quarter-wave line
  posx += 100;
  QW_TL.setParams(
      QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]),
      TransmissionLine, 90, posx, 0);
  QW_TL.val["Z0"] = num2str(Z0, Resistance);
  QW_TL.val["Length"] = ConvertLengthFromM("mm", lambda4);
  Schematic.appendComponent(QW_TL);

  // Output port
  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
      posx + 50, 0);
  TermSpar2.val["Z"] = num2str(Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Final wires
  Schematic.appendWire(NI.ID, 0, QW_TL.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, QW_TL.ID, 1);
}

void QuarterWaveFilters::buildFilter_Microstrip(const std::deque<double> &gi) {
  ComponentInfo QW_TL, OC_Stub, SC_Stub;
  ComponentInfo MSVIA, MSOPEN;
  NodeInfo NI;

  double lambda4 = SPEED_OF_LIGHT / (4. * Specification.fc);
  int N = Specification.order;
  double fc = Specification.fc;
  double BW = Specification.bw;
  double bw = BW / fc;
  double Z0 = Specification.ZS;

  int posx = 0;
  QString PreviousComp;

  // Input port
  ComponentInfo TermSpar1(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, posx, 0);
  TermSpar1.val["Z"] = num2str(Z0, Resistance);
  Schematic.appendComponent(TermSpar1);
  PreviousComp = TermSpar1.ID;
  posx -= 50;

  // Build filter sections
  for (int k = 0; k < N; k++) {
    posx += 100;

    // Quarter-wave microstrip line
    MicrostripClass MSL;
    MSL.Substrate = Specification.MS_Subs;
    MSL.synthesizeMicrostrip(Z0, lambda4 * 1e3, Specification.fc);

    double MS_Width = MSL.Results.width;
    double MS_Length = MSL.Results.length * 1e-3;

    QW_TL.setParams(
        QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
        MicrostripLine, 90, posx, 0);
    QW_TL.val["Width"] = ConvertLengthFromM("mm", MS_Width);
    QW_TL.val["Length"] = ConvertLengthFromM("mm", MS_Length);
    QW_TL.val["er"] = num2str(Specification.MS_Subs.er);
    QW_TL.val["h"] = num2str(Specification.MS_Subs.height);
    QW_TL.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
    QW_TL.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
    QW_TL.val["tand"] = num2str(Specification.MS_Subs.tand);
    Schematic.appendComponent(QW_TL);

    // Node
    NI.setParams(
        QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]),
        posx + 50, 0);
    Schematic.appendNode(NI);

    // Wires
    Schematic.appendWire(PreviousComp, 0, QW_TL.ID, 0);
    Schematic.appendWire(NI.ID, 0, QW_TL.ID, 1);

    // Stubs
    double Z;
    if (Specification.FilterType == Bandpass) {
      Z = (M_PI * Z0 * bw) / (4 * gi[k]);

      MicrostripClass MSL_Stub;
      MSL_Stub.Substrate = Specification.MS_Subs;
      MSL_Stub.synthesizeMicrostrip(Z, lambda4 * 1e3, Specification.fc);

      double MS_Width_Stub = MSL_Stub.Results.width;
      double MS_Length_Stub = MSL_Stub.Results.length * 1e-3;

      SC_Stub.setParams(
          QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
          MicrostripLine, 0, posx + 50, 50);
      SC_Stub.val["Width"] = ConvertLengthFromM("mm", MS_Width_Stub);
      SC_Stub.val["Length"] = ConvertLengthFromM("mm", MS_Length_Stub);
      SC_Stub.val["er"] = num2str(Specification.MS_Subs.er);
      SC_Stub.val["h"] = num2str(Specification.MS_Subs.height);
      SC_Stub.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
      SC_Stub.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
      SC_Stub.val["tand"] = num2str(Specification.MS_Subs.tand);
      Schematic.appendComponent(SC_Stub);

      // Via
      MSVIA.setParams(
          QString("MSVIA%1").arg(++Schematic.NumberComponents[MicrostripVia]),
          MicrostripVia, 0, posx + 50, 100);
      MSVIA.val["D"] = ConvertLengthFromM("mm", 0.5e-3);
      MSVIA.val["N"] = QString::number(4);
      MSVIA.val["er"] = num2str(Specification.MS_Subs.er);
      MSVIA.val["h"] = num2str(Specification.MS_Subs.height);
      MSVIA.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
      MSVIA.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
      MSVIA.val["tand"] = num2str(Specification.MS_Subs.tand);
      Schematic.appendComponent(MSVIA);

      Schematic.appendWire(NI.ID, 0, SC_Stub.ID, 1);
      Schematic.appendWire(SC_Stub.ID, 0, MSVIA.ID, 0);

    } else if (Specification.FilterType == Bandstop) {
      Z = (4 * Z0) / (M_PI * bw * gi[k]);

      MicrostripClass MSL_Stub;
      MSL_Stub.Substrate = Specification.MS_Subs;
      MSL_Stub.synthesizeMicrostrip(Z, lambda4 * 1e3, Specification.fc);

      double MS_Width_Stub = MSL_Stub.Results.width;
      double MS_Length_Stub = MSL_Stub.Results.length * 1e-3;

      OC_Stub.setParams(
          QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
          MicrostripLine, 0, posx + 50, 50);
      OC_Stub.val["Width"] = ConvertLengthFromM("mm", MS_Width_Stub);
      OC_Stub.val["Length"] = ConvertLengthFromM("mm", MS_Length_Stub);
      OC_Stub.val["er"] = num2str(Specification.MS_Subs.er);
      OC_Stub.val["h"] = num2str(Specification.MS_Subs.height);
      OC_Stub.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
      OC_Stub.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
      OC_Stub.val["tand"] = num2str(Specification.MS_Subs.tand);
      Schematic.appendComponent(OC_Stub);

      // Microstrip open
      MSOPEN.setParams(
          QString("MOPEN%1").arg(++Schematic.NumberComponents[MicrostripOpen]),
          MicrostripOpen, 0, posx + 50, 100);
      MSOPEN.val["Width"] = ConvertLengthFromM("mm", MS_Width_Stub);
      MSOPEN.val["er"] = num2str(Specification.MS_Subs.er);
      MSOPEN.val["h"] = num2str(Specification.MS_Subs.height);
      MSOPEN.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
      MSOPEN.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
      MSOPEN.val["tand"] = num2str(Specification.MS_Subs.tand);
      Schematic.appendComponent(MSOPEN);

      Schematic.appendWire(NI.ID, 0, OC_Stub.ID, 1);
      Schematic.appendWire(OC_Stub.ID, 0, MSOPEN.ID, 0);
    }

    PreviousComp = NI.ID;
  }

  // Final quarter-wave microstrip line
  posx += 100;
  MicrostripClass MSL_Final;
  MSL_Final.Substrate = Specification.MS_Subs;
  MSL_Final.synthesizeMicrostrip(Z0, lambda4 * 1e3, Specification.fc);

  double MS_Width_Final = MSL_Final.Results.width;
  double MS_Length_Final = MSL_Final.Results.length * 1e-3;

  QW_TL.setParams(
      QString("MLIN%1").arg(++Schematic.NumberComponents[MicrostripLine]),
      MicrostripLine, 90, posx, 0);
  QW_TL.val["Width"] = ConvertLengthFromM("mm", MS_Width_Final);
  QW_TL.val["Length"] = ConvertLengthFromM("mm", MS_Length_Final);
  QW_TL.val["er"] = num2str(Specification.MS_Subs.er);
  QW_TL.val["h"] = num2str(Specification.MS_Subs.height);
  QW_TL.val["cond"] = num2str(Specification.MS_Subs.MetalConductivity);
  QW_TL.val["th"] = num2str(Specification.MS_Subs.MetalThickness);
  QW_TL.val["tand"] = num2str(Specification.MS_Subs.tand);
  Schematic.appendComponent(QW_TL);

  // Output port
  ComponentInfo TermSpar2(
      QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180,
      posx + 50, 0);
  TermSpar2.val["Z"] = num2str(Z0, Resistance);
  Schematic.appendComponent(TermSpar2);

  // Final wires
  Schematic.appendWire(NI.ID, 0, QW_TL.ID, 0);
  Schematic.appendWire(TermSpar2.ID, 0, QW_TL.ID, 1);
}
