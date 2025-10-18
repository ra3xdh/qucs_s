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

#ifndef DOUBLESTUB_H
#define DOUBLESTUB_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

class DoubleStub : public Network {
public:
  DoubleStub();
  virtual ~DoubleStub();
  DoubleStub(MatchingNetworkDesignParameters, double);
  void synthesize();

private:
  struct MatchingNetworkDesignParameters Specs;
  double f_match;

  void buildMatchingNetwork_Ideal(double d, double lstub1, double lstub2);
  void buildMatchingNetwork_Microstrip(double d, double lstub1, double lstub2);

  std::pair<double, double> calculateStubLengths(double lambda, double Z0,
                                                 double RL, double XL,
                                                 double d);
};
#endif // DOUBLESTUB_H
