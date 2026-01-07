/// @file BridgedTeeAttenuator.h
/// @brief Bridged-Tee attenuator (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef BRIDGEDTEEATTENUATOR_H
#define BRIDGEDTEEATTENUATOR_H

#include "AttenuatorBase.h"

/// @class BridgedTeeAttenuator
/// @brief Bridged-Tee attenuator implementation
/// @details Four-resistor topology: R1 (series), R2/R3 (shunt to ground = Z0), R4 (bridge to ground)
/// Reference: RF Design Guide. Systems, Circuits, and Equations. Peter Vizmuller. Artech House, 1995
class BridgedTeeAttenuator : public AttenuatorBase {
    public:
      /// @brief Class constructor
      BridgedTeeAttenuator(){}

      /// @brief Class destructor
      virtual ~BridgedTeeAttenuator() {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      BridgedTeeAttenuator(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

      /// @brief Calculate component values and build schematic
      void synthesize() override;

    private:
      double R1; ///< Series resistor (Ohms)
      double R4; ///< Bridge resistor to ground (Ohms)

      /// @brief Calculate R1, R4 values and power dissipation
      void calculateParams() override;

      /// @brief Build schematic with components, nodes, and wires
      void buildNetwork() override;
};

#endif // BRIDGEDTEEATTENUATOR_H
