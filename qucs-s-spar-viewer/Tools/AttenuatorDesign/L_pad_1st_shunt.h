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

#ifndef L_PAD_1ST_SHUNT_H
#define L_PAD_1ST_SHUNT_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "AttenuatorBase.h"
#include <QPen>

class LPadFirstShunt : public AttenuatorBase {
    public:
        LPadFirstShunt();
        virtual ~LPadFirstShunt();
        LPadFirstShunt(AttenuatorDesignParameters);

        void synthesize() override;

    private:
        double R1, R2, Zout;

        void calculateParams() override;
        void buildNetwork() override;
};

#endif // L_PAD_1ST_SHUNT_H
