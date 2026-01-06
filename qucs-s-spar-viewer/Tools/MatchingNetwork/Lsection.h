/// @file Lsection.h
/// @brief L-section matching network synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef LSECTION_H
#define LSECTION_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"

/// @class Lsection
/// @brief L-section matching network synthesis
/// Reference: RF design guide. Systems, circuits, and equations. Peter Vizmuller.
/// Artech House, 1995
class Lsection : public Network {
public:
  /// @brief Class constructor
  Lsection() {}

  /// @brief Class constructor with parameters
  /// @param AS Design specifications
  /// @param freq Matching frequency
  Lsection(MatchingNetworkDesignParameters AS, double freq) {
    Specs = AS;
    f_match = freq;
  }

  /// @brief Class destructor
  virtual ~Lsection() {}

  /// @brief Calculate component values and build schematic
  void synthesize();

private:
  /// @brief Matching network specifications
  struct MatchingNetworkDesignParameters Specs;

  double f_match; ///< Matching frequency
};
#endif // LSECTION_H
