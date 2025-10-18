/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MULTISTAGE_WILKINSON_H
#define MULTISTAGE_WILKINSON_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>
#include <complex>
#include <deque>

class MultistageWilkinson : public Network {
    public:
        MultistageWilkinson();
        virtual ~MultistageWilkinson();
        MultistageWilkinson(PowerCombinerParams);
        void synthesize();

    private:
        PowerCombinerParams Specification;

        std::deque<double> Zlines;
        std::deque<double> Risol;
        double lambda4;

        void calculateParams();
        std::deque<double> calcMultistageWilkinsonIsolators(
            const std::deque<double> &Zlines, double L, std::complex<double> gamma);
        std::deque<double> ChebyshevTaper(double ZL, double ripple);

        void buildMultistageWilkinson_LumpedLC();
        void buildMultistageWilkinson_IdealTL();
        void buildMultistageWilkinson_Microstrip();
};

#endif // MULTISTAGE_WILKINSON_H
