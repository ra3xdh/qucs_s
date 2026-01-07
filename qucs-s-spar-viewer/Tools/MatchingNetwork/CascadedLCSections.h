/// @file CascadedLCSections.h
/// @brief Cascaded L-section matching network synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later


#ifndef CASCADEDLCSECTIONS_H
#define CASCADEDLCSECTIONS_H


#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"


/// @class CascadedLCSections
/// @brief Cascaded L-section matching network synthesis
/// Reference: Inder J. Bahl. "Fundamentals of RF and microwave transistor
/// amplifiers". John Wiley and Sons. 2009. Pages 169 - 170
class CascadedLCSections : public Network {
public:
  /// @brief Class constructor
  CascadedLCSections() {
    NumberOfSections = 3; // Default number of sections
  }

  /// @brief Class constructor with parameters
  /// @param AS Design specifications
  /// @param freq Corner frequency for matching
  CascadedLCSections(MatchingNetworkDesignParameters AS, double freq) {
    Specs            = AS;
    NumberOfSections = 3; // Default, should be configurable
    f_match          = freq;
  }

  /// @brief Class destructor
  virtual ~CascadedLCSections() {}

  /// @brief Calculate component values and build schematic
  void synthesize();

private:
  /// @brief Matching network specifications
  struct MatchingNetworkDesignParameters Specs;

  /// @brief Number of L-sections
  int NumberOfSections;

  /// @brief Build schematic for the lowpass solution
  void CreateLowpassSolution();

  /// @brief Build schematic for the highpass solution
  void CreateHighpassSolution();

  double f_match; ///< Corner frequency for matching
};
#endif // CASCADEDLCSECTIONS_H
