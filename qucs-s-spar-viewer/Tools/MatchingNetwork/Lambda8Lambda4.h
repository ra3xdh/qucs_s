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

#ifndef LAMBDA8LAMBDA4_H
#define LAMBDA8LAMBDA4_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

class Lambda8Lambda4 : public Network {

public:
  Lambda8Lambda4();
  virtual ~Lambda8Lambda4();

  Lambda8Lambda4(MatchingNetworkDesignParameters, double freq);
  void synthesize();

private:
  struct MatchingNetworkDesignParameters Specs;
  double f_match;

  void buildMatchingNetwork_IdealTL(double Zm, double Zmm, double XL);
  void buildMatchingNetwork_Microstrip(double Zm, double Zmm, double XL);
};

#endif // LAMBDA8LAMBDA4_H
