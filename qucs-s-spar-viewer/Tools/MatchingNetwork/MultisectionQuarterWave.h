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

#ifndef MULTISECTIONQUARTERWAVE_H
#define MULTISECTIONQUARTERWAVE_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

class MultisectionQuarterWave : public Network {

public:
  MultisectionQuarterWave();
  virtual ~MultisectionQuarterWave();

  MultisectionQuarterWave(MatchingNetworkDesignParameters, double);
  void synthesize();

private:
  struct MatchingNetworkDesignParameters Specs;

  // Helper functions
  int BinomialCoeff(int n, int k);
  void designBinomial(std::vector<double>& Zs);
  void designChebyshev(std::vector<double>& Zs);

  void synthesizeIdealTL(const std::vector<double>& Zi, double lambda4);
  void synthesizeMicrostripTL(const std::vector<double>& Zi, double lambda4);

  double f_match;
};

#endif // MULTISECTIONQUARTERWAVE_H
