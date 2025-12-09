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

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <array>
#include <complex.h>
#include <complex>
#include <deque>
#include <vector>

enum ComponentType {
  Capacitor,
  Inductor,
  Term,
  GND,
  ConnectionNodes,
  Resistor,
  TransmissionLine,
  MicrostripLine,
  MicrostripStep,
  MicrostripOpen,
  MicrostripVia,
  MicrostripCoupledLines,
  OpenStub,
  ShortStub,
  CoupledLines,
  Coupler,
  ComplexImpedance,
  SPAR_Block
};

// Function for getting the component name as text
QString ComponentTypeToString(ComponentType type);

enum ResponseType {
  Butterworth,
  Chebyshev,
  Legendre,
  Gaussian,
  Elliptic,
  Blichinkoff,
  Bessel,
  LinearPhaseEqError,
  Gegenbauer
};
enum FilterClass { Lowpass, Highpass, Bandpass, Bandstop };
enum Coupling {
  CapacitiveCoupledShuntResonators,
  InductiveCoupledShuntResonators,
  CapacitiveCoupledSeriesResonators,
  InductiveCoupledSeriesResonators,
  QWCoupledShuntResonators
};
enum SemiLumpedImplementation { ONLY_INDUCTORS, INDUCTORS_AND_SHUNT_CAPS };

enum TransmissionLineType {
  Ideal,
  MLIN, // Microstrip
  SLIN,  // Stripline
  Lumped
};

static const double SPEED_OF_LIGHT = 299792458.0;

struct PrototypeTableProperties {
  QString ID;
  std::vector<int> N;
  std::vector<double> Ripple;
  std::vector<double> RL;
};

// Microstrip substrate struct
struct MS_Substrate {
  // Substrate
  double height; // substrate height (mm)
  double er;     // relative permittivity
  double tand;   // dissipation factor

  // Metal properties
  double MetalConductivity; // Metal conductivity
  double MetalThickness;    // Metal thickness

  MS_Substrate(double h = 0.508e-3, double epsilon_r = 3.55,
               double tand = 0.0027, double MetalConductivity = 58e6,
               double MetalThickness = 32e-6)
      : height(h), er(epsilon_r), tand(tand),
        MetalConductivity(MetalConductivity), MetalThickness(MetalThickness) {
  } // RO4003C, 20 mils
};


bool operator==(const MS_Substrate& a, const MS_Substrate& b);
QList<MS_Substrate> removeDuplicates(const QList<MS_Substrate>& list);


struct FilterSpecifications {
  FilterClass FilterType;      // Lowpass, Highpass, Bandpass, Bandstop
  ResponseType FilterResponse; // Butterworth, Chebyshev, Cauer, etc...
  QString Implementation;
  TransmissionLineType TL_implementation; // Ideal, microstrip, stripline, etc.
  Coupling DC_Coupling; // Only for bandpass direct coupled filters
  bool isCLC;           // CLC or LCL implementation
  unsigned int order;   // Filter order
  double Ripple = 0.05; // Ripple (Chebyshev and Cauer)
  double fc; // Cutoff frequency (lowpass and highpass filters) or center
             // frequency (bandpass and bandstop)
  double bw; // Filter bandwidth
  double ZS; // Source impedance
  double ZL; // Load impedance
  double as; // Stopband attenuation
  QString EllipticType; // Type of elliptic filter
  bool UseZverevTables;
  double minZ; // Minimum synthesizable impedance (only for stepped impedance
               // filters)
  double maxZ; // Maximum synthesizable impedance (only for stepped impedance
               // filters)
  SemiLumpedImplementation SemiLumpedISettings;
  double ImpedanceRatio;

  MS_Substrate MS_Subs; // Substrate Settings

  ComponentType tunableComponent_DC_Filters; /// Direct-Coupled filters only. It indicates which component in the resonator can be adjusted by the user
  std::vector<double> resonatorValues; /// Direct-coupled filters only. The vector size must match the order of the filter. Each position indicates the value of the adjustable component in the resonator
};

// Contains the information of the matching network topology
struct MatchingNetworkDesignParameters {
  double Z0;
  int Topology;
  TransmissionLineType TL_implementation; // Ideal, microstrip, stripline, etc.
  int Solution;
  int OpenShort;
  int NSections;
  QString Weigthing;
  double gamma_MAX;

  // Impedance data
  double ZL_freq;
  std::complex<double> ZL; // Load impedance at fmatch
  QList<double> freq;
  QList<std::complex<double>>
      ZL_data;      // Impedance vs freq. This is will need to be fixed
  QString sim_path; // Path to the S-parameter file that models the load.
                    // Required for the simulation engine

  // Substrate Settings
  MS_Substrate MS_Subs;
};

// Contains all the information for the matching network design problem
struct MatchingData {
  struct MatchingNetworkDesignParameters InputNetworkParameters;
  struct MatchingNetworkDesignParameters OutputNetworkParameters;
  std::array<std::complex<double>, 4> sparams;

  bool twoPortMode;

  // Match band
  double f_match; // Target frequency
};

struct NetworkInfo {
  std::vector<std::complex<double>> ZS;
  std::vector<std::complex<double>> ZL;
  QStringList topology;
  QList<struct ComponentInfo>
      Ladder; // Contains the ID of the components and their properties
};

struct PowerCombinerParams {
  QString Type; // Wilkinson, branchlines, Bagley, etc.
  TransmissionLineType TL_implementation; // Ideal, microstrip, stripline, etc.
  int Noutputs;                           // Number of output branches
  int Nstages; // Number of combiner stages (broadband Wilkinson)
  std::deque<double> OutputRatio; // Splitting ratio
  double alpha;                   // Attenuation constant of the ideal TL
  QString units;                  // mm, mil, um
  double freq;                    // Center freq
  double Z0;                      // Reference impedance

  // Substrate Settings
  MS_Substrate MS_Subs;
};

struct AttenuatorDesignParameters {
  QString Topology;                       // Attenuator topology
  TransmissionLineType TL_implementation; // Ideal, microstrip, stripline, etc.
  double Zin;                             // Input impedance
  double Zout;                            // Output impedance
  double Attenuation;                     // Attenuation in dB
  double Frequency; // Central frequency of tuned attenuators
  double Pin;       // Input power in W

  // Substrate Settings
  MS_Substrate MS_Subs;
};

#endif // STRUCTURES_H
