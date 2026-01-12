/// @file DirectCoupledFilters.h
/// @brief Synthesis of different types of direct-coupled resonator filters (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef DIRECTCOUPLEDFILTERS_H
#define DIRECTCOUPLEDFILTERS_H


#include "./../../../Schematic/Network.h"
#include "./../../../Schematic/component.h"
#include "./../LowpassPrototypeCoeffs.h"


/// @class DirectCoupledFilters
/// @brief Implements synthesis of various direct-coupled resonator filter topologies.
///
/// This class provides comprehensive design equations for direct-coupled bandpass
/// filters
///
/// Reference: "Microwave Filters, Impedance-Matching Networks, and Coupling Structures" by Matthaei, Young, and Jones.
class DirectCoupledFilters : public Network {
public:
  /// @brief Class constructor
  DirectCoupledFilters() {}

  /// @brief Class destructor
  virtual ~DirectCoupledFilters() {}

  /// @brief Class constructor initialized with the filter specifications
  DirectCoupledFilters(FilterSpecifications FS) {
    Specification = FS;
  }

  /// @brief Handles the direct-coupled filter implementation
  ///
  /// \internal 1) The lowpass prototype coefficients are obtained,
  /// depending on the desired response. 2) Then, depending on the topology
  /// specified, this function calls the appropiate synthesis function
  void synthesize();

private:
  struct FilterSpecifications Specification; /// Filter specifications (band, response type, ripple, etc.)
  std::deque<double> gi; /// Lowpass prototype. Initialized when the synthesize function is called.

  /// @brief Synthesis of capacitive-coupled shunt resonators filters
  void Synthesize_Capacitive_Coupled_Shunt_Resonators();

  /// @brief Synthesis of inductive-coupled shunt resonators filters
  void Synthesize_Inductive_Coupled_Shunt_Resonators();

  /// @brief Synthesis of capacitive-coupled series resonators filters
  void Synthesize_Capacitive_Coupled_Series_Resonators();

  /// @brief Synthesis of inductive-coupled series resonators filters
  void Synthesize_Inductive_Coupled_Series_Resonators();

  /// @brief Synthesis of (quarter-wavelength transmission line)-coupled shunt resonators filters
  void Synthesize_QuarterWave_Coupled_Shunt_Resonators();
};

#endif // DIRECTCOUPLED_H
