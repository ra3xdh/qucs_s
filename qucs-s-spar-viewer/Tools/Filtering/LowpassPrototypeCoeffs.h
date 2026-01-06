/// @file LowpassPrototypeCoeffs.h
/// @brief Calculation of the normalized lowpass filter coefficients (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef LOWPASSPROTOTYPECOEFFS_H
#define LOWPASSPROTOTYPECOEFFS_H
#include "../../Schematic/Network.h"
#include "../../Schematic/structures.h"
#include <QDebug>
#include <deque>

/// @class LowpassPrototypeCoeffs
/// @brief Calculates normalized element values for lowpass filter prototypes
///
/// Computes g-parameters (normalized element values) for various filter approximations
class LowpassPrototypeCoeffs {
public:
  /// @brief Constructor initializes with filter specifications
  /// @param specs Filter specifications (order, ripple, response type)
  LowpassPrototypeCoeffs(FilterSpecifications);

  /// @brief Calculates prototype coefficients based on filter response type
  /// @return Deque of g-parameters: [g0, g1, g2, ..., gN, gN+1]
  ///         where g0 = source impedance (normalized to 1)
  ///               g1...gN = element values (L or C)
  ///               gN+1 = load impedance
  std::deque<double> getCoefficients();

private:
  FilterSpecifications Specification; ///< Filter design parameters

  /// @brief Calculates Butterworth prototype coefficients
  /// @return g-parameters for maximally flat magnitude response
  /// @details Reference: Matthaei, Young, Jones - "Microwave Filters" (1964)
  std::deque<double> calcButterworth_gi();

  /// @brief Calculates Chebyshev prototype coefficients
  /// @return g-parameters for equiripple passband response
  /// @details Iterative calculation based on passband ripple specification
  ///          Reference: Matthaei, Young, Jones - "Microwave Filters" (1964)
  std::deque<double> calcChebyshev_gi();

  /// @brief Retrieves Bessel prototype coefficients from lookup table
  /// @return g-parameters for maximally flat group delay
  /// @details Pre-calculated values for orders 2-10
  ///          Reference: Zverev - "Handbook of Filter Synthesis" (1967)
  std::deque<double> calcBessel_gi();

  /// @brief Retrieves Gaussian prototype coefficients from lookup table
  /// @return g-parameters for Gaussian magnitude response
  /// @details Pre-calculated values for orders 3-10, optimized for minimal overshoot
  ///          Reference: Zverev - "Handbook of Filter Synthesis" (1967)
  std::deque<double> calcGaussian_gi();

  /// @brief Retrieves Legendre prototype coefficients from lookup table
  /// @return g-parameters for monotonic response with steep roll-off
  /// @details Pre-calculated values for orders 4-10, steeper than Butterworth
  ///          Reference: Zverev - "Handbook of Filter Synthesis" (1967)
  std::deque<double> calcLegendre_gi();
};

#endif // LOWPASSPROTOTYPECOEFFS_H
