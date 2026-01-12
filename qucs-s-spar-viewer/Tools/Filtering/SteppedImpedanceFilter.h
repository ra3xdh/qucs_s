/// @file SteppedImpedanceFilter.h
/// @brief Synthesis of stepped impdance filters (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef STEPPEDIMPEDANCEFILTER_H
#define STEPPEDIMPEDANCEFILTER_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "LowpassPrototypeCoeffs.h"

/// @class SteppedImpedanceFilter
/// @brief Implements a stepped-impedance lowpass filter.
///
/// This class synthesizes lowpass filters using the stepped-impedance technique,
/// where lumped inductors and capacitors from the prototype are replaced by
/// transmission line sections of alternating high and low characteristic impedances.
/// High-impedance lines approximate series inductors, while low-impedance
/// lines approximate shunt capacitors.
/// [1] "Microwave Engineering" by David M. Pozar. 4th Edition. page 424
class SteppedImpedanceFilter : public Network {
public:
  /// @brief Default constructor.
  SteppedImpedanceFilter(){}

  /// @brief Virtual destructor.
  virtual ~SteppedImpedanceFilter(){}

  /// @brief Constructor with filter specifications.
  /// @param FS Filter specifications including order, cutoff frequency, impedance,
  ///           minimum/maximum impedance values, and transmission line type.
  SteppedImpedanceFilter(FilterSpecifications FS) {Specification = FS;}

  /// @brief Synthesizes the filter based on the provided specifications.
  void synthesize();

private:

  /// @brief Filter specifications structure containing all design parameters.
  struct FilterSpecifications Specification;

  /// @brief Builds the filter using microstrip transmission lines.
  /// @param gi Lowpass prototype element values (g-parameters).
  void buildFilter_Microstrip(const std::deque<double>& gi);

  /// @brief Builds the filter using ideal transmission lines.
  /// @param gi Lowpass prototype element values (g-parameters).
  void buildFilter_IdealTL(const std::deque<double>& gi);
};

#endif // STEPPEDIMPEDANCEFILTER_H
