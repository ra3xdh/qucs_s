/// @file Branchline.h
/// @brief Branch-line power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef BRANCHLINE_H
#define BRANCHLINE_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

/// @class Branchline
/// @brief Branch-line power combiner/divider network
class Branchline : public Network {
  public:
    /// @brief Class constructor
    Branchline() {}

    /// @brief Constructor with power combiner parameters
    /// @param params Power combiner specification parameters
    Branchline(PowerCombinerParams PS) { Specification = PS; }

    /// @brief Class destructor
    virtual ~Branchline() {}

    /// @brief Synthesize the Branchline network
    void synthesize();

  private:
    /// @brief Power combiner specifications
    struct PowerCombinerParams Specification;

    double lambda4; ///< Quarter wavelength
    double ZA;
    double ZB;

    /// @brief Calculate electrical parameters
    void calculateParams();

    /// @brief Set component locations for schematic layout
    void setComponentsLocation();

    /// @brief Build Branchline network using ideal transmission lines
    void buildBranchline_IdealTL();

    /// @brief Build Branchline network using microstrip transmission lines
    void buildBranchline_Microstrip();

    /// Component spacing parameters
    int x_spacing;  ///< Horizontal spacing between components
    int y_spacing;  ///< Vertical spacing between components

    /// Ports
    QPoint Port_in;   ///< Input port position
    QPoint Port_out1; ///< Output port 1 position
    QPoint Port_out2; ///< Output port 2 position

    /// Isolation resistor
    QPoint Riso_pos;  ///< Position of the isolation resistor
    QPoint GND_Riso;  ///< Position of the isolation resistor's GND

    /// Transmission line positions
    QPoint TL1_pos;
    QPoint TL2_pos;
    QPoint TL3_pos;
    QPoint TL4_pos;

    /// Node positions
    QPoint N1_pos;
    QPoint N2_pos;
    QPoint N3_pos;
    QPoint N4_pos;

};

#endif // BRANCHLINE_H
