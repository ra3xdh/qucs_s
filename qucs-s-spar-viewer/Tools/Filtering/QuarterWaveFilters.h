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


///
/// @brief Implements quarter-wave stub bandpass and bandstop filters.
///
/// This class synthesizes filters using quarter-wave transmission line sections
/// with shunt stubs. For bandpass filters, short-circuited stubs are used, while
/// bandstop filters employ open-circuited stubs. The topology consists of cascaded
/// quarter-wave lines with alternating stub sections that provide the frequency
/// selectivity. Both ideal and microstrip implementations are supported.
///
class QuarterWaveFilters : public Network {
public:
  ///
  /// @brief Default constructor.
  ///
  /// Initializes component counters for the schematic.
  /// @todo See if it's necessary to update counters
  ///
  QuarterWaveFilters() {
    // Initialize list of components
    Schematic.NumberComponents[Capacitor] = 0;
    Schematic.NumberComponents[Inductor] = 0;
    Schematic.NumberComponents[Term] = 0;
    Schematic.NumberComponents[GND] = 0;
    Schematic.NumberComponents[ConnectionNodes] = 0;
  }

  ///
  /// @brief Virtual destructor.
  ///
  virtual ~QuarterWaveFilters() {}

  ///
  /// @brief Constructor with filter specifications.
  /// @param FS Filter specifications including order, bandwidth, center frequency,
  ///           filter type (bandpass/bandstop), impedance, and implementation type
  /// @todo See if it's necessary to update counters
  ///
  QuarterWaveFilters(FilterSpecifications FS) {
    Specification = FS;
    // Initialize list of components
    Schematic.NumberComponents[Capacitor] = 0;
    Schematic.NumberComponents[Inductor] = 0;
    Schematic.NumberComponents[Term] = 0;
    Schematic.NumberComponents[GND] = 0;
    Schematic.NumberComponents[ConnectionNodes] = 0;
  }

  ///
  /// @brief Synthesizes the filter based on the provided specifications.
  ///
  void synthesize();

private:

  ///
  /// @brief Filter specifications structure containing all design parameters.
  ///
  struct FilterSpecifications Specification;

  ///
  /// @brief Builds the filter using microstrip transmission lines.
  /// @param gi Lowpass prototype element values (g-parameters).
  ///
  void buildFilter_Microstrip(const std::deque<double>& gi);

  ///
  /// @brief Builds the filter using ideal transmission lines.
  /// @param gi Lowpass prototype element values (g-parameters).
  ///
  void buildFilter_IdealTL(const std::deque<double>& gi);
};

#endif // QUARTERWAVEFILTERS_H
