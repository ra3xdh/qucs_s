#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QStringList>
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
  OpenStub,
  ShortStub,
  CoupledLines,
  Coupler,
  ComplexImpedance
};

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
  CapacitativeCoupledShuntResonators,
  InductiveCoupledSeriesResonators
};
enum SemiLumpedImplementation { ONLY_INDUCTORS, INDUCTORS_AND_SHUNT_CAPS };

static const double SPEED_OF_LIGHT =
    299792458.0; // REMOVE THIS WHEN THIS TOOL BECOMES INTEGRATED IN QUCS

struct PrototypeTableProperties {
  QString ID;
  std::vector<int> N;
  std::vector<double> Ripple;
  std::vector<double> RL;
};

struct FilterSpecifications {
  FilterClass FilterType;      // Lowpass, Highpass, Bandpass, Bandstop
  ResponseType FilterResponse; // Butterworth, Chebyshev, Cauer, etc...
  QString Implementation;
  Coupling DC_Coupling; // Only for bandpass direct coupled filters
  bool isCLC;           // CLC or LCL implementation
  unsigned int order;   // Filter order
  double Ripple = 0.05;        // Ripple (Chebyshev and Cauer)
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
};

struct MatchingNetworkDesignParameters {
  QString Topology;          // Attenuator topology
  std::complex<double> Zin;  // Input impedance
  std::complex<double> Zout; // Output impedance
  int Solution; // Some matching techniques yield several valid solutions to the
                // matching problem
  int OpenShort; // This is applicable to topologies with stubs
  double freqStart;
  double freqEnd;
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
  int Noutputs; // Number of output branches
  int Nstages;  // Number of combiner stages (broadband Wilkinson)
  std::deque<double> OutputRatio; // Splitting ratio
  QString Implementation;         // LC, microstrip, ideal TL
  double alpha;                   // Attenuation constant of the ideal TL
  QString units;                  // mm, mil, um
  double freq;                    // Center freq
  double Z0;                      // Reference impedance
};

struct AttenuatorDesignParameters {
  QString Topology;   // Attenuator topology
  double Zin;         // Input impedance
  double Zout;        // Output impedance
  double Attenuation; // Attenuation in dB
  double Frequency;   // Central frequency of tuned attenuators
  double Pin;         // Input power in W
  bool Lumped_TL;     // Use the lumped equivalent of a QW transmission line
};

#endif // STRUCTURES_H
