/// @file CoupledLineBandpassFilter.h
/// @brief Synthesis of coupled-line bandpass filters (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later


#ifndef COUPLEDLINEBANDPASSFILTER_H
#define COUPLEDLINEBANDPASSFILTER_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "LowpassPrototypeCoeffs.h"

/// @class CoupledLineBandpassFilter
/// @brief Implements a coupled-line bandpass filter.
///
/// This class synthesizes bandpass filters using quarter-wave coupled
/// transmission line sections. The design is based on equations from
/// "Microwave Engineering" by David M. Pozar (4th Edition, pages 430-437).
/// @note Supports:
///   - Ideal transmission lines
///   - Microstrip transmission lines
class CoupledLineBandpassFilter : public Network {
    public:
      /// @brief Default constructor.
      CoupledLineBandpassFilter() {}

      /// @brief Virtual destructor.
      virtual ~CoupledLineBandpassFilter() {}

      /// @brief Constructor with filter specifications.
      /// @param FS Filter specifications including order, bandwidth, center frequency,
      ///           impedance, and transmission line implementation type.
      CoupledLineBandpassFilter(FilterSpecifications FS) {Specification = FS;}

      /// @brief Synthesizes the filter based on the provided specifications.
      ///
      /// Computes the lowpass prototype coefficients and calls the appropriate
      /// build method for either ideal or microstrip coupled line implementation.
      void synthesize();

    private:
      /// @brief Filter specifications structure containing all design parameters.
      struct FilterSpecifications Specification;

      /// @brief Builds the filter using ideal coupled transmission lines.
      /// @param gi Lowpass prototype element values (g-parameters).
      void buildFilter_IdealTL(const std::deque<double>& gi);

      /// @brief Builds the filter using microstrip coupled transmission lines.
      /// @param gi Lowpass prototype element values (g-parameters).
      void buildFilter_Microstrip(const std::deque<double>& gi);
};

#endif // COUPLEDLINEBANDPASSFILTER_H
