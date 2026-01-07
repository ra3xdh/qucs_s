/// @file LPadFirstSeries.h
/// @brief L-pad (first series) 1-port matched attenuator (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 5, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef L_PAD_1ST_SERIES_H
#define L_PAD_1ST_SERIES_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "AttenuatorBase.h"
#include <QPen>

/// @class LPadFirstSeries
/// @brief L-pad (first series) 1-port matched attenuator (definition)
class LPadFirstSeries : public AttenuatorBase {
    public:

      /// @brief Class constructor
      LPadFirstSeries(){}

      /// @brief Class destructor
      virtual ~LPadFirstSeries() {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      LPadFirstSeries(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      void synthesize() override;

    private:
      double R1;   ///< Series resistor
      double R2;   ///< Shunt resistor
      double Zout; ///< Output impedance

      /// @brief Calculate R1, R2 and Zout
      void calculateParams() override;

      /// @brief Build schematic with components, nodes, and wires
      void buildNetwork() override;
};

#endif // L_PAD_1ST_SERIES_H
