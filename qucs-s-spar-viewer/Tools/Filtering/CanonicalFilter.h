/// @file CanonicalFilter.h
/// @brief Synthesis of canonical filters (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef CANONICALFILTER_H
#define CANONICALFILTER_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "LowpassPrototypeCoeffs.h"

/// @class CanonicalFilter
/// @brief Synthesizes classical ladder filter topologies from normalized prototypes
///
/// Converts lowpass prototypes (g-parameters) to complete filter implementations:
/// - Lowpass: Direct synthesis with frequency/impedance scaling
/// - Highpass: Lowpass-to-highpass transformation (L↔C)
/// - Bandpass: Series/parallel resonator transformations
/// - Bandstop: Dual resonator transformations
///
/// Supports lumped (LC) and semi-lumped (transmission line) realizations
class CanonicalFilter : public Network {
public:
  /// @brief Default constructor
  CanonicalFilter() {}

  /// @brief Destructor
  virtual ~CanonicalFilter() {}

  /// @brief Constructor with filter specifications
  /// @param specs Filter parameters (order, type, frequency, ripple, etc.)
  CanonicalFilter(FilterSpecifications FS) {Specification = FS;}

  /// @brief Performs complete filter synthesis from specifications
  void synthesize();

  /// @brief Enables semi-lumped implementation mode
  /// @param mode true to use transmission line stubs for inductors/capacitors
  void setSemilumpedMode(bool mode) { this->semilumped = mode; }

private:
  struct FilterSpecifications Specification; ///< Design parameters
  bool semilumped = false; ///< Enable transmission line stub replacement
  std::deque<double> gi; ///< Normalized lowpass prototype element values

  //***********  Schematic synthesis ********************

  /// @brief Synthesizes lowpass filter schematic
  void SynthesizeLPF();

  /// @brief Synthesizes semi-lumped lowpass filter (TLINs replace L/C)
  void SynthesizeSemilumpedLPF();

  /// @brief Synthesizes highpass filter schematic
  void SynthesizeHPF();

  /// @brief Synthesizes bandpass filter schematic
  void SynthesizeBPF();

  /// @brief Synthesizes bandstop filter schematic
  void SynthesizeBSF();
};

#endif // CANONICALFILTER_H
