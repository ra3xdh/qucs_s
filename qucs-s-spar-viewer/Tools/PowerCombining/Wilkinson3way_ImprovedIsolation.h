/// @file Wilkinson3way_ImprovedIsolation.h
/// @brief 3-way Wilkinson with improved isolation network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-laterng with this program.  If not, see
/// <https://www.gnu.org/licenses/>.

#ifndef WILKINSON3WAY_IMPROVEDISOLATION_H
#define WILKINSON3WAY_IMPROVEDISOLATION_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class Wilkinson3Way_ImprovedIsolation
/// @brief 3-way Wilkinson with improved isolation network
/// References:
/// [1] "Power combiners, impedance transformers and directional couplers: part
/// II". Andrei Grebennikov. High Frequency Electronics. 2008
/// [2] "New 3N way hybrid power dividers", IEEE Trans. Microwave Theory Tech.,
/// vol. MTT-25, Dec. 1977, pp. 1008-1012
class Wilkinson3Way_ImprovedIsolation : public Network {
    public:
      /// @brief Class constructor
      Wilkinson3Way_ImprovedIsolation() {}

      /// @brief Constructor with power combiner parameters
      /// @param params Power combiner specification parameters
      Wilkinson3Way_ImprovedIsolation(PowerCombinerParams PS) {Specification = PS;}

      /// @brief Class destructor
      virtual ~Wilkinson3Way_ImprovedIsolation() {}

      /// @brief Synthesize the network
      void synthesize();

    private:
      /// @brief Power combiner specifications
      PowerCombinerParams Specification;

      double lambda4; ///< Quarter wavelength
      double Z1, Z2;
      double R1; ///< 1st isolation resistor
      double R2; ///< 2nd isolation resistor

      /// @brief Calculate electrical parameters
      void calculateParams();

      /// @brief Set component locations for schematic layout
      void setComponentsLocation();

      /// @brief Build network using ideal transmission lines
      void buildWilkinson3Way_IdealTL();

      /// @brief Build network using microstrip transmission lines
      void buildWilkinson3Way_Microstrip();

      /// Component spacing parameters
      int x_spacing;  ///< Horizontal spacing between components
      int y_spacing;  ///< Vertical spacing between components

      /// Ports
      QVector<QPoint> Ports_pos;

      /// Isolation resistors
      QVector<QPoint> Riso_pos;

      /// Transmission lines
      QVector<QPoint> TL_pos;

      /// Nodes
      QVector<QPoint> N_pos;
};

#endif // WILKINSON3WAY_IMPROVEDISOLATION_H
