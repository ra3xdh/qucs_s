/// @file Gysel.h
/// @brief Gysel power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef GYSEL_H
#define GYSEL_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class Gysel
/// @brief Gysel power combiner/divider network
class Gysel : public Network {
  public:
    /// @brief Class constructor
    Gysel() {}

    /// @brief Constructor with power combiner parameters
    /// @param params Power combiner specification parameters
    Gysel(PowerCombinerParams PS) { Specification = PS; }

    /// @brief Class destructor
    virtual ~Gysel() {}

    /// @brief Synthesize the double-box branchline network
    void synthesize();

  private:
    /// @brief Power combiner specifications
    PowerCombinerParams Specification;

    double lambda4; ///< Quarter wavelength length
    double lambda2; ///< Half wavelength length

    /// @brief Calculate electrical parameters
    void calculateParams();

    /// @brief Set component locations for schematic layout
    void setComponentsLocation();

    /// @brief Build Gysel network using ideal transmission lines
    void buildGysel_IdealTL();

    /// @brief Build Gysel network using microstrip transmission lines
    void buildGysel_Microstrip();

    /// Component spacing parameters
    int x_spacing;  ///< Horizontal spacing between components
    int y_spacing;  ///< Vertical spacing between components

    /// Ports
    QPoint Port_in;   ///< Input port position
    QPoint Port_out_up; ///< Output port 1 position
    QPoint Port_out_bottom; ///< Output port 2 position

    /// Node positions
    QPoint N1_pos; // Node in front of the input port
    QPoint N2_pos; // Node in front of the upper output port
    QPoint N3_pos; // Node in front of the lower output port
    QPoint N4_pos; // Node in front of the upper resistor
    QPoint N5_pos; // Node in front of the lower resistor

    /// Transmission line positions
    QPoint TL1_pos;
    QPoint TL2_pos;
    QPoint TL3_pos;
    QPoint TL4_pos;
    QPoint TL5_pos;

    /// Top resistor
    QPoint R_top;      ///< Position of the top resistor
    QPoint R_GND_top;  ///< Position of the top resistor's GND

    /// Bottom resistor
    QPoint R_bottom;      ///< Position of the top resistor
    QPoint R_GND_bottom;  ///< Position of the top resistor's GND

};

#endif // GYSEL_H
