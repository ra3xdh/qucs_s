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

#ifndef ATTENUATORBASE_H
#define ATTENUATORBASE_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"

class AttenuatorBase : public Network {
    public:
        AttenuatorBase() = default;
        AttenuatorBase(AttenuatorDesignParameters AS) : Specification(AS) {}
        virtual ~AttenuatorBase() = default;

        // Common interface - pure virtual
        virtual void synthesize() = 0;

        // Common implementation
        QMap<QString, double> getPowerDissipation() { return Pdiss; }

    protected:
        AttenuatorDesignParameters Specification;
        QMap<QString, double> Pdiss;

        // Pure virtual for calculation - each attenuator type implements its own
        virtual void calculateParams() = 0;

        // Pure virtual for building - each attenuator type implements its own
        virtual void buildNetwork() = 0;
};

#endif // ATTENUATORBASE_H
