/// @file CapacitivelyCoupledShuntResonatorsFilter.h
/// @brief Synthesis of capacitive-coupled shunt resonantors bandpass filters (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef CAPACITIVELYCOUPLEDSHUNTRESONATORS_H
#define CAPACITIVELYCOUPLEDSHUNTRESONATORS_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "LowpassPrototypeCoeffs.h"


/// @class CapacitivelyCoupledShuntResonatorsFilter
/// @brief Implements an end-coupled resonator bandpass filter.
/// This class synthesizes bandpass filters using end-coupled resonator topology,
/// where resonators are coupled through series capacitors at their ends.
/// Reference: Microwave Engineering. David M. Pozar. 4th Edition.
/// 2012. John Wiley and Sons.Page 443-448.
/// @note Supports:
///   - Ideal transmission lines
///   - Microstrip transmission lines
class CapacitivelyCoupledShuntResonatorsFilter : public Network {
public:
  /// @brief Default constructor.
  CapacitivelyCoupledShuntResonatorsFilter() {}

  /// @brief Virtual destructor.
  virtual ~CapacitivelyCoupledShuntResonatorsFilter() {}

  /// @brief Constructor with filter specifications.
  /// @param FS Filter specifications including order, bandwidth, center frequency,
  ///           impedance, and transmission line implementation type.
  CapacitivelyCoupledShuntResonatorsFilter(FilterSpecifications FS) {Specification = FS;}

  /// @brief Synthesizes the filter based on the provided specifications.
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

#endif // CAPACITIVELYCOUPLEDSHUNTRESONATORS_H
