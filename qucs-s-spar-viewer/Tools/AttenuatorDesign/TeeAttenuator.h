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

#ifndef TEEATTENUATOR_H
#define TEEATTENUATOR_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include <QPen>

class TeeAttenuator : public Network {
public:
  TeeAttenuator();
  virtual ~TeeAttenuator();
  TeeAttenuator(AttenuatorDesignParameters);
  void synthesize();
  QMap<QString, double> getPowerDissipation();

private:
  AttenuatorDesignParameters Specification;
  
  double R1, R2, R3;
  QMap<QString, double> Pdiss; // Power dissipation for each resistor
  
  void calculateParams();
  void buildTeeAttenuator();
};

#endif // TEEATTENUATOR_H
