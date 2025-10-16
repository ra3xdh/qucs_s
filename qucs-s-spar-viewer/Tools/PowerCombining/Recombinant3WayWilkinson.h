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

#ifndef RECOMBINANT3WAYWILKINSON_H
#define RECOMBINANT3WAYWILKINSON_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/*
 * References:
 * [1] "Power combiners, impedance transformers and directional couplers: part
 * II". Andrei Grebennikov. High Frequency Electronics. 2008
 * [2] "A recombinant in-phase power divider", IEEE Trans. Microwave Theory
 * Tech., vol. MTT-39, Aug. 1991, pp. 1438-1440
 */

class Recombinant3WayWilkinson : public Network {
public:
  Recombinant3WayWilkinson();
  virtual ~Recombinant3WayWilkinson();
  Recombinant3WayWilkinson(PowerCombinerParams);
  void synthesize();

private:
  PowerCombinerParams Specification;
  
  double lambda4;
  double Z1, Z2, Z4, R1, R2;
  
  void calculateParams();
  void buildRecombinant3Way_IdealTL();
  void buildRecombinant3Way_Microstrip();
};

#endif // RECOMBINANT3WAYWILKINSON_H
