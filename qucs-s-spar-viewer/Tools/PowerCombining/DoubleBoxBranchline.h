/// @file DoubleBoxBranchline.h
/// @brief Double-box branch-line power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef DOUBLE_BOX_BRANCHLINE_H
#define DOUBLE_BOX_BRANCHLINE_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/// @class DoubleBoxBranchline
/// @brief Double-box branch-line power combiner/divider network
class DoubleBoxBranchline : public Network {
  public:
    /// @brief Class constructor
    DoubleBoxBranchline() {}

    /// @brief Constructor with power combiner parameters
    /// @param params Power combiner specification parameters
    DoubleBoxBranchline(PowerCombinerParams PS) {
      Specification = PS;
    }

    /// @brief Class destructor
    virtual ~DoubleBoxBranchline() {}

    /// @brief Synthesize the double-box branchline network
    void synthesize();

  private:
    /// @brief Power combiner specifications
    PowerCombinerParams Specification;

    double lambda4; ///< Quarter wavelength length
    double ZA;
    double ZB;
    double ZD;

    /// @brief Calculate electrical parameters
    void calculateParams();

    /// @brief Set component locations for schematic layout
    void setComponentsLocation();

    /// @brief Build double-box Branchline network using ideal transmission lines
    void buildDoubleBoxBranchline_IdealTL();

    /// @brief Build double-box Branchline network using microstrip transmission lines
    void buildDoubleBoxBranchline_Microstrip();

    /// Component spacing parameters
    int x_spacing;  ///< Horizontal spacing between components
    int y_spacing;  ///< Vertical spacing between components

    /// Ports
    QPoint Port_in;   ///< Input port position
    QPoint Port_out1; ///< Output port 1 position
    QPoint Port_out2; ///< Output port 2 position

    /// Isolation resistor
    QPoint Riso_pos;  ///< Position of the isolation resistor
    QPoint GND_Riso_pos;  ///< Position of the isolation resistor's GND

    /// Transmission line positions
    QPoint TL1_pos;
    QPoint TL2_pos;
    QPoint TL3_pos;
    QPoint TL4_pos;
    QPoint TL5_pos;
    QPoint TL6_pos;
    QPoint TL7_pos;

    /// Node positions
    QPoint N1_pos;
    QPoint N2_pos;
    QPoint N3_pos;
    QPoint N4_pos;
    QPoint N5_pos;
    QPoint N6_pos;

};

#endif // DOUBLE_BOX_BRANCHLINE_H
