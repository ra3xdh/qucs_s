/// @file RSeriesAttenuator.h
/// @brief Series resistor (unmatched) attenuator synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef RSERIESATTENUATOR_H
#define RSERIESATTENUATOR_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "AttenuatorBase.h"
#include <QPen>

/// @class RSeriesAttenuator
/// @brief Series resistor (unmatched) attenuator synthesis
class RSeriesAttenuator : public AttenuatorBase {
    public:
      /// @brief Class constructor
      RSeriesAttenuator() {}

      /// @brief Class constructor with parameters
      /// @param AS Design specifications
      RSeriesAttenuator(AttenuatorDesignParameters AS) : AttenuatorBase(AS) {}

      /// @brief Class destructor
      virtual ~RSeriesAttenuator() {}

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

#endif // RSERIESATTENUATOR_H
