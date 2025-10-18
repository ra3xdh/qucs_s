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

#ifndef RSERIESATTENUATOR_H
#define RSERIESATTENUATOR_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "AttenuatorBase.h"
#include <QPen>

class RSeriesAttenuator : public AttenuatorBase {
    public:
        RSeriesAttenuator();
        virtual ~RSeriesAttenuator();
        RSeriesAttenuator(AttenuatorDesignParameters);

        void synthesize() override;

    private:
        double R1, Zin, Zout;

        void calculateParams() override;
        void buildNetwork() override;
};

#endif // RSERIESATTENUATOR_H
