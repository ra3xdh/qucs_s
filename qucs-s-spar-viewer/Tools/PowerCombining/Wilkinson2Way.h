/// @file Wilkinson2Way.h
/// @brief Wilkinson power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-laterng with this program.  If not, see
/// <https://www.gnu.org/licenses/>.

#ifndef WILKINSON2WAY_H
#define WILKINSON2WAY_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class Wilkinson2Way
/// @brief Wilkinson power combiner/divider network
class Wilkinson2Way : public Network {
    public:
      /// @brief Class constructor
      Wilkinson2Way() {}

      /// @brief Constructor with power combiner parameters
      /// @param params Power combiner specification parameters
      Wilkinson2Way(PowerCombinerParams PS) { Specification = PS; }

      /// @brief Class destructor
      virtual ~Wilkinson2Way() {}

      /// @brief Synthesize the Wilkinson network
      void synthesize();

    private:
      /// @brief Power combiner specifications
      PowerCombinerParams Specification;

      double Z2, Z3, R, R2, R3;

      /// @brief Calculate electrical parameters
      void calculateParams();

      /// @brief Set component locations for schematic layout
      void setComponentsLocation();

      /// @brief Build Wilkinson using lumped element transmission lines
      void buildWilkinson_LumpedLC();

      /// @brief Build Wilkinson network using ideal transmission lines
      void buildWilkinson_IdealTL();

      /// @brief Build Wilkinson network using microstrip transmission lines
      void buildWilkinson_Microstrip();

      /// Component spacing parameters
      int x_spacing;  ///< Horizontal spacing between components
      int y_spacing;  ///< Vertical spacing between components

      /// Ports
      QPoint Port_in;
      QPoint Port_out1, Port_out2;

      /// Isolation resistor
      QPoint Riso_pos;
      QPoint GND_Riso_pos;

      /// Transmission lines
      QPoint TL1_pos, TL2_pos, TL3_pos, TL4_pos, TL5_pos;

      /// Nodes
      QPoint N1_pos, N2_pos, N3_pos, N4_pos, N5_pos;
};

#endif // WILKINSON2WAY_H
