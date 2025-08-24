/***************************************************************************
Lambda8Lambda4.cpp
------------------
author : 2025: Andrés Martínez Mera
***************************************************************************/

#include "Lambda8Lambda4.h"
#include <cmath>

Lambda8Lambda4::Lambda8Lambda4() {}
Lambda8Lambda4::~Lambda8Lambda4() {}
Lambda8Lambda4::Lambda8Lambda4(MatchingNetworkDesignParameters AS) { Specs = AS; }

void Lambda8Lambda4::synthesize() {
    // Port 1
    ComponentInfo TermSpar1(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 0, 0);
    TermSpar1.val["Z"] = num2str(Specs.Zin.real(), Resistance);
    Schematic.appendComponent(TermSpar1);


    // --- Design equations
    double Z0 = Specs.Zin.real();
    double RL = Specs.Zout.real();
    double XL = Specs.Zout.imag();

    double l4 = SPEED_OF_LIGHT / (4. * Specs.freqStart);
    double l8 = 0.5 * l4;

    // Equivalent matching impedances
    double Zmm = std::sqrt(RL * RL + XL * XL);
    double Zm = std::sqrt((Z0 * RL * Zmm) / (Zmm - XL));

    // First transmission line: Zm, length λ/4
    ComponentInfo TL1(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, 50, 0);
    TL1.val["Z0"] = num2str(Zm, Resistance);
    TL1.val["Length"] = ConvertLengthFromM("mm", l4);
    Schematic.appendComponent(TL1);

    // Second transmission line: Zmm, length λ/8
    ComponentInfo TL2(QString("TLIN%1").arg(++Schematic.NumberComponents[TransmissionLine]), TransmissionLine, 90, 100, 0);
    TL2.val["Z0"] = num2str(Zmm, Resistance);
    TL2.val["Length"] = ConvertLengthFromM("mm", l8);
    Schematic.appendComponent(TL2);

    // Load
    ComponentInfo Zload(QString("Z%1").arg(++Schematic.NumberComponents[ComplexImpedance]), ComplexImpedance, 0, 150, 50);
    Zload.val["Z"] = num2str(Specs.Zout, Resistance);
    Schematic.appendComponent(Zload);

           // GND for load
    ComponentInfo GND_ZL;
    GND_ZL.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 150, 100);
    Schematic.appendComponent(GND_ZL);

    // --- Wiring ---
    Schematic.appendWire(TermSpar1.ID, 0, TL1.ID, 0);
    Schematic.appendWire(TL1.ID, 1, TL2.ID, 0);
    Schematic.appendWire(Zload.ID, 1, TL2.ID, 1);
    Schematic.appendWire(Zload.ID, 0, GND_ZL.ID, 0);
}

