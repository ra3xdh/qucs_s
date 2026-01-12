/// @file Bagley.h
/// @brief Bagley power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef BAGLEY_H
#define BAGLEY_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class Bagley
/// @brief Bagley power combiner/divider network
///
/// Implements the Bagley topology for N-way power combining/dividing.
/// Supports ideal transmission lines and microstrip implementations.
///
class Bagley : public Network {
  public:
    /// @brief Default constructor
    Bagley() {}

    /// @brief Constructor with power combiner parameters
    /// @param params Power combiner specification parameters
    Bagley(PowerCombinerParams PS) { Specification = PS; }

    /// @brief Class destructor
    virtual ~Bagley() {}

    /// @brief Synthesize the Bagley network
    void synthesize();

  private:
    /// @brief Power combiner specifications
    PowerCombinerParams Specification;

    double lambda4;  ///< Quarter wavelength at design frequency
    double lambda2;  ///< Half wavelength at design frequency
    double Zbranch;  ///< Branch line characteristic impedance

    /// @brief Calculate electrical parameters
    void calculateParams();

    /// @brief Build Bagley network using ideal transmission lines
    void buildBagley_IdealTL();

    /// @brief Build Bagley network using microstrip transmission lines
    void buildBagley_Microstrip();

    /// @brief Set component locations for schematic layout
    ///
    /// Calculates positions for all components before building
    /// the schematic based on number of outputs
    void setComponentsLocation();

    /// Component spacing parameters
    int x_spacing;  ///< Horizontal spacing between components
    int y_spacing;  ///< Vertical spacing between components

    /// Port locations
    QPoint Port_in;      ///< Input port position
    QPoint Port_1st_out; ///< First output port position

    /// Output node parameters
    int y_out;          ///< Y-coordinate of output line
    QPoint N_1st_out;   ///< First output node position

    /// Transmission line positions
    QPoint left_TL;   ///< Left vertical transmission line position
    QPoint right_TL;  ///< Right vertical transmission line position
};

#endif // BAGLEY_H
