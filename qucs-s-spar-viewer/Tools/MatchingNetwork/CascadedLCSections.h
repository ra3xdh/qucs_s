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

#ifndef CASCADEDLCSECTIONS_H
#define CASCADEDLCSECTIONS_H


#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"

class CascadedLCSections : public Network {
public:
  CascadedLCSections();
  virtual ~CascadedLCSections();
  CascadedLCSections(MatchingNetworkDesignParameters, double);
  void synthesize();

private:
  struct MatchingNetworkDesignParameters Specs;
  int NumberOfSections;
  void CreateLowpassSolution();
  void CreateHighpassSolution();
  double f_match;
};
#endif // CASCADEDLCSECTIONS_H
