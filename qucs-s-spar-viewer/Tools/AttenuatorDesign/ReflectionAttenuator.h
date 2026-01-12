/// @file ReflectionAttenuator.h
/// @brief Reflection attenuator synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef REFLECTIONATTENUATOR_H
#define REFLECTIONATTENUATOR_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "AttenuatorBase.h"
#include <QPen>

/// @class ReflectionAttenuator
/// @brief Reflection attenuator synthesis
/// Reference: The PIN diode circuit designer's handbook. W.E. Doherty, Jr., R.D. Joos, Microsemi Corp., 1998
class ReflectionAttenuator : public AttenuatorBase {
    public:
      /// @brief Class constructor
      ReflectionAttenuator() {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      ReflectionAttenuator(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

      /// @brief Class destructor
      virtual ~ReflectionAttenuator() {}

      /// @brief Calculate component values and build schematic
      void synthesize() override;

    private:
      double Ri; ///< Shunt resistors

      /// @brief Calculate Ri depending on attenuation
      void calculateParams() override;

      /// @brief Build schematic with components, nodes, and wires
      void buildNetwork() override;
};

#endif // REFLECTIONATTENUATOR_H
