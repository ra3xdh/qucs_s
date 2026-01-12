/// @file DoubleStub.h
/// @brief Double stub matching network synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later


#ifndef DOUBLESTUB_H
#define DOUBLESTUB_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

/// @class DoubleStub
/// @brief Double stub matching network synthesis
/// Reference: 'Microwave Engineering', David Pozar. John Wiley and Sons. 4th
/// Edition. Pg 241-245
class DoubleStub : public Network {
  public:
    /// @brief Class constructor
    DoubleStub() {}

    /// @brief Class constructor with parameters
    /// @param AS Design specifications
    /// @param freq Matching frequency
    DoubleStub(MatchingNetworkDesignParameters AS, double freq) {
              Specs = AS;
              f_match = freq;
              }

    /// @brief Class destructor
    virtual ~DoubleStub(){}

    /// @brief Calculate component values and build schematic
    void synthesize();

private:
  /// @brief Matching network specifications
  struct MatchingNetworkDesignParameters Specs;

  double f_match; ///< Matching frequency

  /// @brief Build schematic with ideal transmission lines
  void buildMatchingNetwork_Ideal(double d, double lstub1, double lstub2);

  /// @brief Build schematic with microstrip transmission lines
  void buildMatchingNetwork_Microstrip(double d, double lstub1, double lstub2);

  /// @brief Apply the design equations
  std::pair<double, double> calculateStubLengths(double lambda, double Z0,
                                                 double RL, double XL,
                                                 double d);
};
#endif // DOUBLESTUB_H
