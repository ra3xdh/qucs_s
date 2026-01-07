/// @file QW_ShuntAttenuator.h
/// @brief Quarter wavelength first-shunt attenuator synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef QW_SHUNTATTENUATOR_H
#define QW_SHUNTATTENUATOR_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "AttenuatorBase.h"
#include <QPen>

/// @class QW_ShuntAttenuator
/// @brief Quarter wavelength first-shunt attenuator synthesis
/// Reference: The PIN diode circuit designer's handbook. W.E. Doherty, Jr., R.D. Joos, Microsemi Corp., 1998
class QW_ShuntAttenuator : public AttenuatorBase {
    public:
      /// @brief Class constructor
      QW_ShuntAttenuator() {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      QW_ShuntAttenuator(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

      /// @brief Class destructor
      virtual ~QW_ShuntAttenuator() {}

      /// @brief Calculate component values and build schematic
      void synthesize() override;

    private:
      double R;    ///< Resistor
      double l4;   ///< Quarte wavelength [m]
      double Zout; ///< Output impedance [Ohm]
      double w0;   ///< Radial central frequency = 2*pi*f0 [rad/s]

      /// @brief Calculate R, l4 and Zout values and power dissipation
      void calculateParams() override;

      /// @brief Build schematic with components, nodes, and wires
      /// @details Calls different synthesis functions depending on the transmission line implementation
      void buildNetwork() override;

      /// @brief Build schematic (lumped version) with components, nodes, and wires
      void buildQW_Shunt_Lumped();

      /// @brief Build schematic (ideal transmission line version) with components, nodes, and wires
      void buildQW_Shunt_IdealTL();

      /// @brief Build schematic (microstrip transmission line version) with components, nodes, and wires
      void buildQW_Shunt_Microstrip();
};

#endif // QW_SHUNTATTENUATOR_H
