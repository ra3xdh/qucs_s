/// @file LPadFirstShunt.h
/// @brief L-pad (first shunt) 1-port matched attenuator (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef L_PAD_1ST_SHUNT_H
#define L_PAD_1ST_SHUNT_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "AttenuatorBase.h"
#include <QPen>

/// @class LPadFirstShunt
/// @brief L-pad (first shunt) 1-port matched attenuator (definition)
class LPadFirstShunt : public AttenuatorBase {
    public:
      /// @brief Class constructor
      LPadFirstShunt() {}

      /// @brief Class destructor
      virtual ~LPadFirstShunt(){}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      LPadFirstShunt(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      void synthesize() override;

    private:
      double R1;   ///< Shunt resistor
      double R2;   ///< Series resistor
      double Zout; ///< Output impedance

      /// @brief Calculate R1, R2 and Zout
      void calculateParams() override;

      /// @brief Build schematic with components, nodes, and wires
      void buildNetwork() override;
};

#endif // L_PAD_1ST_SHUNT_H
