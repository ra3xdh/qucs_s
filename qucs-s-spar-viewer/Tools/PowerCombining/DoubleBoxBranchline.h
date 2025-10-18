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

#ifndef DOUBLE_BOX_BRANCHLINE_H
#define DOUBLE_BOX_BRANCHLINE_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

class DoubleBoxBranchline : public Network {
public:
  DoubleBoxBranchline();
  virtual ~DoubleBoxBranchline();
  DoubleBoxBranchline(PowerCombinerParams);
  void synthesize();

private:
  PowerCombinerParams Specification;
  
  double lambda4, ZA, ZB, ZD;
  
  void calculateParams();
  void buildDoubleBoxBranchline_IdealTL();
  void buildDoubleBoxBranchline_Microstrip();
};

#endif // DOUBLE_BOX_BRANCHLINE_H
