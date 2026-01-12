/// @file QuarterWaveFilter.h
/// @brief Synthesis of quarter wavelength BPF/BSF (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef QUARTERWAVEFILTERS_H
#define QUARTERWAVEFILTERS_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../Filtering/LowpassPrototypeCoeffs.h"
#include "../TransmissionLineSynthesis/Microstrip.h"


/// @class QuarterWaveFilters
/// @brief Implements quarter-wave stub bandpass and bandstop filters.
///
/// This class synthesizes filters using quarter-wave transmission line sections
/// with shunt stubs.
/// Bandpass filters: short-circuited stubs
/// Bandstop filters: open-circuited stubs
/// Both ideal and microstrip implementations are supported.
/// Reference: "Microwave Engineering", David M. Pozar. 4th edition. Page 437 - 440
class QuarterWaveFilters : public Network {
public:
  /// @brief Default constructor.
  /// Initializes component counters for the schematic.
  QuarterWaveFilters() {}

  /// @brief Virtual destructor.
  virtual ~QuarterWaveFilters() {}

  /// @brief Constructor with filter specifications.
  /// @param FS Filter specifications including order, bandwidth, center frequency,
  ///           filter type (bandpass/bandstop), impedance, and implementation type
  QuarterWaveFilters(FilterSpecifications FS) {Specification = FS;}

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

#endif // QUARTERWAVEFILTERS_H
