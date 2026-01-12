/// @file Lim_Eom.h
/// @brief Lim-Eom power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef LIM_EOM_H
#define LIM_EOM_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class Lim_Eom
/// @brief Lim-Eom power combiner/divider network
/// References:
/// [1] "Power combiners, impedance transformers and directional couplers: part II".
/// Andrei Grebennikov. High Frequency Electronics. 2008
/// [2] "A New 3-Way Power Divider with Various Output Power Ratios," J.-S. Lim
/// and S.-Y. Eom, 1996 IEEE MTT-S Int. Microwave Symp. Dig., pp. 785-788."
class Lim_Eom : public Network {
  public:
    /// @brief Default constructor
    Lim_Eom() {}

    /// @brief Constructor with power combiner parameters
    /// @param params Power combiner specification parameters
    Lim_Eom(PowerCombinerParams PS) { Specification = PS; }

    /// @brief Class destructor
    virtual ~Lim_Eom() {}

    /// @brief Synthesize the Bagley network
    void synthesize();

  private:
    /// @brief Power combiner specifications
    PowerCombinerParams Specification;

    double lambda4;  ///< Quarter wavelength at design frequency
    double Z1;
    double Z2;
    double Z3;
    double Z4;
    double Z5;

    /// @brief Calculate electrical parameters
    void calculateParams();

    /// @brief Build Lim-Eom network using ideal transmission lines
    void buildLimEom_IdealTL();

    /// @brief Build Lim-Eom network using microstrip transmission lines
    void buildLimEom_Microstrip();

    /// @brief Set component locations for schematic layout
    void setComponentsLocation();

    /// Component spacing parameters
    int x_spacing;  ///< Horizontal spacing between components
    int y_spacing;  ///< Vertical spacing between components

    /// Port locations
    QVector<QPoint> Ports_pos;

    /// Isolation resistor
    QVector<QPoint> Riso_pos;
    QVector<QPoint> GND_Riso_pos;

    /// Transmission line positions
    QVector<QPoint> TL_pos;

    /// Nodes positions
    QVector<QPoint> N_pos;
};

#endif // LIM_EOM_H
