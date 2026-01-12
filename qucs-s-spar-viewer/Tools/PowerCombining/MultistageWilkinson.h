/// @file MultistageWilkinson.h
/// @brief Multistage Wilkinson power combiner/divider network (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef MULTISTAGE_WILKINSON_H
#define MULTISTAGE_WILKINSON_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>
#include <complex>
#include <deque>

/// @class MultistageWilkinson
/// @brief Multistage Wilkinson power combiner/divider network
class MultistageWilkinson : public Network {
    public:
        /// @brief Class constructor
        MultistageWilkinson() {}

        /// @brief Constructor with power combiner parameters
        /// @param params Power combiner specification parameters
        MultistageWilkinson(PowerCombinerParams PS) {Specification = PS;}

        /// @brief Class destructor
        virtual ~MultistageWilkinson() {}

        /// @brief Synthesize the Multistage Wilkinson network
        void synthesize();

    private:
        /// @brief Power combiner specifications
        PowerCombinerParams Specification;

        /// @brief Vector containing the characteristic impedance of each section
        std::deque<double> Zlines;

        /// @brief Vector containing the value of the isolation resistors of each section
        std::deque<double> Risol;

        double lambda4; ///< Quarter wavelength

        /// @brief Calculate electrical parameters
        void calculateParams();

        /// @brief Calculate the value of the isolation resistors
        /// @param Zlines: Characteristic line impedance for each section
        /// @param L: Line length for each section
        /// @param gamma: Propagation constant
        /// @return Vector containing the value of the isolation resistors
        std::deque<double> calcMultistageWilkinsonIsolators(
            const std::deque<double> &Zlines, double L, std::complex<double> gamma);

        /// @brief Calculates the Chebyshev weigthing depending on the order
        /// @param ZL: Real value. Impedance of each section
        /// @param ripple: Chebyshev ripple
        /// @return Chebyshev weigthing
        std::deque<double> ChebyshevTaper(double ZL, double ripple);

        /// @brief Build network using lumped elements
        void buildMultistageWilkinson_LumpedLC();

        /// @brief Build network using ideal transmission lines
        void buildMultistageWilkinson_IdealTL();

        /// @brief Build network using microstrip transmission lines
        void buildMultistageWilkinson_Microstrip();
};

#endif // MULTISTAGE_WILKINSON_H
