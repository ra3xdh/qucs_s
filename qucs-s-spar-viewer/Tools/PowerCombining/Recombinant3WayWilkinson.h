/// @file Recombinant3WayWilkinson.cpp
/// @brief Recombinant 3-way Wilkinson-type power combiner/divider network (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef RECOMBINANT3WAYWILKINSON_H
#define RECOMBINANT3WAYWILKINSON_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class Recombinant3WayWilkinson
/// @brief Recombinant 3-way Wilkinson-type power combiner/divider network
/// References:
/// [1] "Power combiners, impedance transformers and directional couplers: part
/// II". Andrei Grebennikov. High Frequency Electronics. 2008
/// [2] "A recombinant in-phase power divider", IEEE Trans. Microwave Theory
/// Tech., vol. MTT-39, Aug. 1991, pp. 1438-1440
class Recombinant3WayWilkinson : public Network {
  public:
    /// @brief Class constructor
    Recombinant3WayWilkinson() {}

    /// @brief Constructor with power combiner parameters
    /// @param params Power combiner specification parameters
    Recombinant3WayWilkinson(PowerCombinerParams PS) {Specification = PS;}

    /// @brief Class destructor
    virtual ~Recombinant3WayWilkinson() {}

    /// @brief Synthesize the network
    void synthesize();

  private:
    /// @brief Power combiner specifications
    PowerCombinerParams Specification;

    double lambda4; ///< Quarter wavelength
    double Z1, Z2, Z3, Z4, Z5, Z6, R1, R2;

    /// @brief Calculate electrical parameters
    void calculateParams();

    /// @brief Build network using ideal transmission lines
    void buildRecombinant3Way_IdealTL();

    /// @brief Build network using microstrip transmission lines
    void buildRecombinant3Way_Microstrip();

    /// @brief Set component locations for schematic layout
    void setComponentsLocation();

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

#endif // RECOMBINANT3WAYWILKINSON_H
