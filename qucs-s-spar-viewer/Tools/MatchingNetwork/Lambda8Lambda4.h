/// @file Lambda8Lambda4.h
/// @brief Lambda/8 + Lambda/4 matching network synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef LAMBDA8LAMBDA4_H
#define LAMBDA8LAMBDA4_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

/// @class Lambda8Lambda4
/// @brief Lambda/8 + Lambda/4 matching network synthesis
/// Reference: Inder J. Bahl. "Fundamentals of RF and microwave transistor
/// amplifiers". John Wiley and Sons. 2009. Pages 159 - 160
class Lambda8Lambda4 : public Network {
  public:
    /// @brief Class constructor
    Lambda8Lambda4() {}

    /// @brief Class constructor with parameters
    /// @param AS Design specifications
    /// @param freq Corner frequency for matching
    Lambda8Lambda4(MatchingNetworkDesignParameters AS,
                   double freq) {
      Specs = AS;
      f_match = freq;
    }

    /// @brief Class destructor
    virtual ~Lambda8Lambda4() {}

    /// @brief Calculate component values and build schematic
    void synthesize();

  private:
    /// @brief Matching network specifications
    struct MatchingNetworkDesignParameters Specs;

    double f_match; ///< Corner frequency for matching

    /// @brief Build schematic with ideal transmission lines
    void buildMatchingNetwork_IdealTL(double Zm, double Zmm, double XL);

    /// @brief Build schematic with microstrip transmission lines
    void buildMatchingNetwork_Microstrip(double Zm, double Zmm, double XL);
};

#endif // LAMBDA8LAMBDA4_H
