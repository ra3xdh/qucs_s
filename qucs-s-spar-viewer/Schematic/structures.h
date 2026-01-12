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

///
/// @brief Component type enumeration
///
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

///
/// @brief Convert component type to string
/// @param type Component type
/// @return String representation
///
inline QString ComponentTypeToString(ComponentType type){
  static const QMap<ComponentType, QString> names = {
                                                     {Capacitor, "Capacitor"},
                                                     {Inductor, "Inductor"},
                                                     {Term, "Term"},
                                                     {GND, "GND"},
                                                     {ConnectionNodes, "ConnectionNodes"},
                                                     {Resistor, "Resistor"},
                                                     {TransmissionLine, "TransmissionLine"},
                                                     {MicrostripLine, "MicrostripLine"},
                                                     {MicrostripStep, "MicrostripStep"},
                                                     {MicrostripOpen, "MicrostripOpen"},
                                                     {MicrostripVia, "MicrostripVia"},
                                                     {MicrostripCoupledLines, "MicrostripCoupledLines"},
                                                     {OpenStub, "OpenStub"},
                                                     {ShortStub, "ShortStub"},
                                                     {CoupledLines, "CoupledLines"},
                                                     {Coupler, "Coupler"},
                                                     {ComplexImpedance, "ComplexImpedance"},
                                                     {SPAR_Block, "SPAR_Block"}};

  return names.value(type, "Unknown");
}

///
/// @brief Filter response type enumeration
///
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

///
/// @brief Filter class enumeration
///
enum FilterClass { Lowpass, Highpass, Bandpass, Bandstop };

///
/// @brief Coupling type enumeration
///
enum Coupling {
  CapacitiveCoupledShuntResonators,
  InductiveCoupledShuntResonators,
  CapacitiveCoupledSeriesResonators,
  InductiveCoupledSeriesResonators,
  QWCoupledShuntResonators
};

///
/// @brief Semi-lumped implementation type
///
enum SemiLumpedImplementation { ONLY_INDUCTORS, INDUCTORS_AND_SHUNT_CAPS };


///
/// @brief Transmission line type enumeration
///
enum TransmissionLineType {
  Ideal,
  MLIN, // Microstrip
  SLIN,  // Stripline
  Lumped
};

static const double SPEED_OF_LIGHT = 299792458.0;

///
/// @brief Prototype table properties
///
struct PrototypeTableProperties {
  QString ID;                    ///< Table identifier
  std::vector<int> N;            ///< Filter orders
  std::vector<double> Ripple;    ///< Ripple values
  std::vector<double> RL;        ///< Return loss values
};

///
/// @brief Microstrip substrate parameters
///
struct MS_Substrate {
  double height;             ///< Substrate height (m)
  double er;                 ///< Relative permittivity
  double tand;               ///< Dissipation factor
  double MetalConductivity;  ///< Metal conductivity (S/m)
  double MetalThickness;     ///< Metal thickness (m)

         /// @brief Constructor with RO4003C defaults (20 mils)
  MS_Substrate(double h = 0.508e-3, double epsilon_r = 3.55,
               double tand = 0.0027, double MetalConductivity = 58e6,
               double MetalThickness = 32e-6)
      : height(h), er(epsilon_r), tand(tand),
        MetalConductivity(MetalConductivity), MetalThickness(MetalThickness) {}
};



/// @brief Compare two substrates for equality
/// @param a First substrate
/// @param b Second substrate
/// @return True if all parameters match
inline bool operator==(const MS_Substrate& a, const MS_Substrate& b) {
  return a.height == b.height &&
         a.er == b.er &&
         a.tand == b.tand &&
         a.MetalConductivity == b.MetalConductivity &&
         a.MetalThickness == b.MetalThickness;
}


/// @brief Remove duplicate substrates from list
/// @param list Input substrate list
/// @return List with duplicates removed
inline QList<MS_Substrate> removeDuplicates(const QList<MS_Substrate>& list) {
  QList<MS_Substrate> uniqueList;
  for (const auto& item : list) {
    if (!uniqueList.contains(item)) {
      uniqueList.append(item);
    }
  }
  return uniqueList;
}


///
/// @brief Filter design specifications
///
struct FilterSpecifications {
  FilterClass FilterType;                        ///< Lowpass, Highpass, Bandpass, Bandstop
  ResponseType FilterResponse;                   ///< Butterworth, Chebyshev, Cauer, etc.
  QString Implementation;                        ///< Implementation type
  TransmissionLineType TL_implementation;        ///< Ideal, microstrip, stripline, etc.
  Coupling DC_Coupling;                          ///< Bandpass direct coupled filters only
  bool isCLC;                                    ///< CLC or LCL implementation
  unsigned int order;                            ///< Filter order
  double Ripple = 0.05;                          ///< Ripple (Chebyshev and Cauer)
  double fc;                                     ///< Cutoff or center frequency (Hz)
  double bw;                                     ///< Filter bandwidth (Hz)
  double ZS;                                     ///< Source impedance (Ohm)
  double ZL;                                     ///< Load impedance (Ohm)
  double as;                                     ///< Stopband attenuation (dB)
  QString EllipticType;                          ///< Type of elliptic filter
  bool UseZverevTables;                          ///< Use Zverev tables flag
  double minZ;                                   ///< Min synthesizable impedance (Ohm)
  double maxZ;                                   ///< Max synthesizable impedance (Ohm)
  SemiLumpedImplementation SemiLumpedISettings;  ///< Semi-lumped settings
  double ImpedanceRatio;                         ///< Impedance ratio
  MS_Substrate MS_Subs;                          ///< Substrate settings
  ComponentType tunableComponent_DC_Filters;     ///< Adjustable component in DC filters
  std::vector<double> resonatorValues;           ///< Resonator component values
};

///
/// @brief Matching network design parameters
///
struct MatchingNetworkDesignParameters {
  double Z0;                              ///< Reference impedance (Ohm)
  int Topology;                           ///< Network topology
  TransmissionLineType TL_implementation; ///< TL type
  int Solution;                           ///< Solution number
  int OpenShort;                          ///< Open/short configuration
  int NSections;                          ///< Number of sections
  QString Weigthing;                      ///< Weighting function
  double gamma_MAX;                       ///< Maximum reflection coefficient
  double ZL_freq;                         ///< Load impedance frequency (Hz)
  std::complex<double> ZL;                ///< Load impedance at fmatch
  QList<double> freq;                     ///< Frequency list
  QList<std::complex<double>> ZL_data;    ///< Impedance vs frequency
  QString sim_path;                       ///< S-parameter file path
  MS_Substrate MS_Subs;                   ///< Substrate settings
};

///
/// @brief Matching network design data
///
struct MatchingData {
  MatchingNetworkDesignParameters InputNetworkParameters;   ///< Input network params
  MatchingNetworkDesignParameters OutputNetworkParameters;  ///< Output network params
  std::array<std::complex<double>, 4> sparams;              ///< S-parameters
  bool twoPortMode;                                         ///< Two-port mode flag
  double f_match;                                           ///< Target frequency (Hz)
};

///
/// @brief Network information
///
struct NetworkInfo {
  std::vector<std::complex<double>> ZS;  ///< Source impedances
  std::vector<std::complex<double>> ZL;  ///< Load impedances
  QStringList topology;                  ///< Topology descriptions
  QList<struct ComponentInfo> Ladder;    ///< Component IDs and properties
};

///
/// @brief Power combiner parameters
///
struct PowerCombinerParams {
  QString Type;                           ///< Wilkinson, branchlines, Bagley, etc.
  TransmissionLineType TL_implementation; ///< TL type
  int Noutputs;                           ///< Number of output branches
  int Nstages;                            ///< Number of combiner stages
  std::deque<double> OutputRatio;         ///< Splitting ratio
  double alpha;                           ///< Attenuation constant
  QString units;                          ///< mm, mil, um
  double freq;                            ///< Center frequency (Hz)
  double Z0;                              ///< Reference impedance (Ohm)
  MS_Substrate MS_Subs;                   ///< Substrate settings
};

///
/// @brief Attenuator design parameters
///
struct AttenuatorDesignParameters {
  QString Topology;                       ///< Attenuator topology
  TransmissionLineType TL_implementation; ///< TL type
  double Zin;                             ///< Input impedance (Ohm)
  double Zout;                            ///< Output impedance (Ohm)
  double Attenuation;                     ///< Attenuation (dB)
  double Frequency;                       ///< Central frequency (Hz)
  double Pin;                             ///< Input power (W)
  MS_Substrate MS_Subs;                   ///< Substrate settings
};

#endif // STRUCTURES_H
