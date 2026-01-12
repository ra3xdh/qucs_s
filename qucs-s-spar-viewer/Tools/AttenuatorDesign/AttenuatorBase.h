/// @file AttenuatorBase.h
/// @brief Abstract base class for RF attenuator implementations
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef ATTENUATORBASE_H
#define ATTENUATORBASE_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"

///
/// @brief Abstract base class for RF attenuator implementations
///
class AttenuatorBase : public Network {
    public:
        ///
        /// @brief Class constructor
        ///
        AttenuatorBase() = default;

        ///
        /// @brief Construct with design parameters
        /// @param AS Design specifications (attenuation, impedances, frequency, power, etc.)
        ///
        AttenuatorBase(AttenuatorDesignParameters AS) : Specification(AS) {}

        ///
        /// @brief Class destructor
        ///
        virtual ~AttenuatorBase() = default;

        ///
        /// @brief Synthesize the attenuator: Calculates topology parameters and build the circuit
        ///
        virtual void synthesize() = 0;

        ///
        /// @brief Get power dissipation for all resistors in the attenuator
        /// @return Map of resistor names to power values in Watts
        ///
        QMap<QString, double> getPowerDissipation() { return Pdiss; }

    protected:
        AttenuatorDesignParameters Specification; ///< Design specifications
        QMap<QString, double> Pdiss; ///< Power dissipation per resistor (Watts)

        ///
        /// @brief Calculate component values and power dissipation
        /// @note Pure virtual - each attenuator type implements its own
        ///
        virtual void calculateParams() = 0;

        ///
        /// @brief Build the schematic network
        /// @note Pure virtual - each attenuator type implements its own
        virtual void buildNetwork() = 0;
};

#endif // ATTENUATORBASE_H
