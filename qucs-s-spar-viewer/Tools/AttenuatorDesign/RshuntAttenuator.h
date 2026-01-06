/// @file RshuntAttenuator.h
/// @brief Shunt resistor (unmatched) attenuator synthesi0s (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef RSHUNTATTENUATOR_H
#define RSHUNTATTENUATOR_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "AttenuatorBase.h"
#include <QPen>

/// @class RShuntAttenuator
/// @brief Shunt resistor (unmatched) attenuator synthesi0
class RShuntAttenuator : public AttenuatorBase {
    public:
      /// @brief Class constructor
      RShuntAttenuator() {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      RShuntAttenuator(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

      /// @brief Class destructor
      virtual ~RShuntAttenuator() {}

      /// @brief Calculate component values and build schematic
      void synthesize() override;

    private:
      double R1;    ///< Resistor value
      double Zin;   ///< Input impedance [Ohm]
      double Zout;  ///< Output impedance [Ohm]

      /// @brief Calculate R1, Zin and Zout
      void calculateParams() override;

      /// @brief Build schematic with components, nodes, and wires
      void buildNetwork() override;
};

#endif // RSHUNTATTENUATOR_H
