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
  void setComponentsLocation();
  void buildDoubleBoxBranchline_IdealTL();
  void buildDoubleBoxBranchline_Microstrip();

private:
  // Private variables for components location
  int x_spacing, y_spacing; // General components spacing
  int x_P1, y_P1, x_P2, y_P2, x_P3, y_P3; // Ports
  int x_Riso, y_Riso; // Isolation resistor

         // Transmission lines
  int x_1st_vert_TL, y_1st_vert_TL; // 1st vertical
  int x_1st_top_TL, y_1st_top_TL; // 1st top horizontal
  int x_1st_bottom_TL, y_1st_bottom_TL; // 1st bottom horizontal
  int x_2nd_vert_TL, y_2nd_vert_TL; // 2nd vertical
  int x_2nd_top_TL, y_2nd_top_TL; // 2nd top horizontal
  int x_2nd_bottom_TL, y_2nd_bottom_TL; // 2nd bottom horizontal
  int x_3rd_top_TL, y_3rd_top_TL; // 3rd top horizontal
  int x_3rd_bottom_TL, y_3rd_bottom_TL; // 3rd bottom horizontal
  int x_3rd_vert_TL, y_3rd_vert_TL; // 3rd top horizontal
};

#endif // DOUBLE_BOX_BRANCHLINE_H
