/// @file PiAttenuator.h
/// @brief Pi attenuator synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef PIATTENUATOR_H
#define PIATTENUATOR_H

#include "AttenuatorBase.h"

/// @class PiAttenuator
/// @brief Pi attenuator synthesis
/// Reference: RF design guide. Systems, circuits, and equations. Peter Vizmuller. Artech House, 1995
class PiAttenuator : public AttenuatorBase {
  public:
    /// @brief Class constructor
    PiAttenuator() {}

    /// @brief Class constructor from specifications
    PiAttenuator(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

    /// @brief Class destructor
    virtual ~PiAttenuator() {}

    /// @brief Class constructor with parameters
    /// @param AS Design specifications
    void synthesize() override;

  private:
    double R1; ///< 1st shunt resistor
    double R2; ///< Series resistor
    double R3; ///< 2nd shunt resistor

    /// @brief Calculate R1, R2 and R3
    void calculateParams() override;

    /// @brief Build schematic with components, nodes, and wires
    void buildNetwork() override;
};

#endif // PIATTENUATOR_H
