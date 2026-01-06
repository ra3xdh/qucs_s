/// @file TeeAttenuator.cpp
/// @brief Tee attenuator synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef TEEATTENUATOR_H
#define TEEATTENUATOR_H

#include "AttenuatorBase.h"

/// @class TeeAttenuator
/// @brief Tee attenuator synthesis
/// Reference: RF design guide. Systems, circuits, and equations. Peter Vizmuller. Artech House, 1995
class TeeAttenuator : public AttenuatorBase {
    public:
    /// @brief Class constructor
    TeeAttenuator() {}

    /// @brief Class constructor from specifications
    TeeAttenuator(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

    /// @brief Class destructor
    virtual ~TeeAttenuator() {}

    /// @brief Class constructor with parameters
    /// @param AS Design specifications
    void synthesize() override;

    private:
    double R1; ///< 1st series resistor
    double R2; ///< Shunt resistor
    double R3; ///< 2nd series resistor

    /// @brief Calculate R1, R2 and R3
    void calculateParams() override;

    /// @brief Build schematic with components, nodes, and wires
    void buildNetwork() override;
};

#endif // TEEATTENUATOR_H
