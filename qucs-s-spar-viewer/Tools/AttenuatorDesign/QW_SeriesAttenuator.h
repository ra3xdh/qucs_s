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

#ifndef QW_SERIESATTENUATOR_H
#define QW_SERIESATTENUATOR_H

#include "../../Misc/general.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "AttenuatorBase.h"
#include <QPen>

class QW_SeriesAttenuator : public AttenuatorBase {
    public:
        QW_SeriesAttenuator();
        virtual ~QW_SeriesAttenuator();
        QW_SeriesAttenuator(AttenuatorDesignParameters);

        void synthesize() override;

    private:
        double R, l4, Zout;
        double w0;

        void calculateParams() override;
        void buildNetwork() override;
        void buildQW_Series_Lumped();
        void buildQW_Series_IdealTL();
        void buildQW_Series_Microstrip();
};

#endif // QW_SERIESATTENUATOR_H
