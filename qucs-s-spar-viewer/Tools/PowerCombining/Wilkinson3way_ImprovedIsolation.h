/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

#ifndef WILKINSON3WAY_IMPROVEDISOLATION_H
#define WILKINSON3WAY_IMPROVEDISOLATION_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/*
 * References:
 * [1] "Power combiners, impedance transformers and directional couplers: part
 * II". Andrei Grebennikov. High Frequency Electronics. 2008
 * [2] "New 3N way hybrid power dividers", IEEE Trans. Microwave Theory Tech.,
 * vol. MTT-25, Dec. 1977, pp. 1008-1012
 */

class Wilkinson3Way_ImprovedIsolation : public Network {
    public:
        Wilkinson3Way_ImprovedIsolation();
        virtual ~Wilkinson3Way_ImprovedIsolation();
        Wilkinson3Way_ImprovedIsolation(PowerCombinerParams);
        void synthesize();

    private:
        PowerCombinerParams Specification;

        double lambda4;
        double Z1, Z2, R1, R2;

        void calculateParams();
        void buildWilkinson3Way_IdealTL();
        void buildWilkinson3Way_Microstrip();

    // Private variables for components location
    private:
        // This function sets the component's location before the schematic is built
        void setComponentsLocation();

        // General components spacing
        int x_spacing, y_spacing;

        // Ports
        QVector<QPoint> Ports_pos;

        // Isolation resistors
        QVector<QPoint> Riso_pos;

        // Transmission lines
        QVector<QPoint> TL_pos;

        // Nodes
        QVector<QPoint> N_pos;
};

#endif // WILKINSON3WAY_IMPROVEDISOLATION_H
