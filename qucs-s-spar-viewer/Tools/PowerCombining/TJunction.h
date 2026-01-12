/// @file Tjunction.h
/// @brief T-Junction power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-laterng with this program.  If not, see
/// <https://www.gnu.org/licenses/>.

#ifndef TJUNCTION_H
#define TJUNCTION_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class TJunction
/// @brief T-Junction power combiner/divider network
class TJunction : public Network {
  public:
    /// @brief Class constructor
    TJunction() {}

    /// @brief Constructor with power combiner parameters
    /// @param params Power combiner specification parameters
    TJunction(PowerCombinerParams PS) { Specification = PS; }

    /// @brief Class destructor
    virtual ~TJunction() {}

    /// @brief Synthesize the T-Junction network
    void synthesize();

  private:
    /// @brief Power combiner specifications
    PowerCombinerParams Specification;

    /// @brief Build T-Junction network using ideal transmission lines
    void buildTJunction_IdealTL(double lambda4, double K);

    /// @brief Build T-Junction network using microstrip transmission lines
    void buildTJunction_Microstrip(double lambda4, double K);

    /// @brief Set component locations for schematic layout
    void setComponentsLocation();

    /// Component spacing parameters
    int x_spacing;  ///< Horizontal spacing between components
    int y_spacing;  ///< Vertical spacing between components

    /// Ports
    QVector<QPoint> Ports_pos;

    /// Transmission lines
    QVector<QPoint> TL_pos;

    /// Nodes
    QVector<QPoint> N_pos;
};

#endif // TJUNCTION_H
