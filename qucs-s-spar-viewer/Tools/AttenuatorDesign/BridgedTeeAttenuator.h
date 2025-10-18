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

#ifndef BRIDGEDTEEATTENUATOR_H
#define BRIDGEDTEEATTENUATOR_H

#include "AttenuatorBase.h"

class BridgedTeeAttenuator : public AttenuatorBase {
    public:
        BridgedTeeAttenuator();
        virtual ~BridgedTeeAttenuator();
        BridgedTeeAttenuator(AttenuatorDesignParameters);

        void synthesize() override;

    private:
        double R1, R4;

        void calculateParams() override;
        void buildNetwork() override;
};

#endif // BRIDGEDTEEATTENUATOR_H
