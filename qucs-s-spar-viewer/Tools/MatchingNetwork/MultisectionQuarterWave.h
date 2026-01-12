/// @file MultisectionQuarterWave.h
/// @brief Quarterwave multisection transformer matching (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef MULTISECTIONQUARTERWAVE_H
#define MULTISECTIONQUARTERWAVE_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

/// @class MultisectionQuarterWave
/// @brief Quarterwave multisection transformer matching
class MultisectionQuarterWave : public Network {
  public:
    /// @brief Class constructor
    MultisectionQuarterWave() {}

    /// @brief Class constructor with parameters
    /// @param AS Design specifications
    /// @param freq Matching frequency
    MultisectionQuarterWave(
        MatchingNetworkDesignParameters AS, double freq) {
      Specs = AS;
      f_match = freq;
    }

    /// @brief Class destructor
    virtual ~MultisectionQuarterWave() {}

    /// @brief Calculate component values and build schematic
    void synthesize();

  private:
    /// @brief Matching network specifications
    struct MatchingNetworkDesignParameters Specs;

    /// @brief Calculate binomial coefficient
    /// @param n Total number
    /// @param k Selection number
    /// @return Binomial coefficient C(n,k)
    int BinomialCoeff(int n, int k);

    /// @brief Design transformer with binomial weighting
    /// @param Zs Output vector of section impedances
    void designBinomial(std::vector<double>& Zs);

    /// @brief Design transformer with Chebyshev weighting
    /// @param Zs Output vector of section impedances
    void designChebyshev(std::vector<double>& Zs);

    /// @brief Synthesize using ideal transmission lines
    /// @param Zi Vector of section impedances
    /// @param lambda4 Quarter wavelength in meters
    void synthesizeIdealTL(const std::vector<double>& Zi, double lambda4);

    /// @brief Synthesize using microstrip transmission lines
    /// @param Zi Vector of section impedances
    /// @param lambda4 Quarter wavelength in meters
    void synthesizeMicrostripTL(const std::vector<double>& Zi, double lambda4);

    double f_match; ///< Matching frequency [Hz]
};

#endif // MULTISECTIONQUARTERWAVE_H
