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

#ifndef SINGLESTUB_H
#define SINGLESTUB_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

class SingleStub : public Network {
public:
  SingleStub();
  virtual ~SingleStub();
  SingleStub(MatchingNetworkDesignParameters, double);
  void synthesize();

private:
  struct MatchingNetworkDesignParameters Specs;
  double f_match;

  void buildMatchingNetwork_IdealTL(double d, double lstub);
  void buildMatchingNetwork_Microstrip(double d, double lstub);

  double normalizePhase(double phase);
  double calculateSusceptance(double t, double RL, double XL, double Z0);
  double calculateT(double RL, double XL, double Z0);
  double calculateLineDistance(double t, double lambda);
  double calculateStubLength(double B, double Z0, double lambda);
};
#endif // SINGLESTUB_H
