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

#ifndef BRANCHLINE_H
#define BRANCHLINE_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

class Branchline : public Network {
public:
  Branchline();
  virtual ~Branchline();
  Branchline(PowerCombinerParams);
  void synthesize();

private:
  struct PowerCombinerParams Specification;
  
  double lambda4, ZA, ZB;

  void calculateParams();
  void setComponentsLocation();
  void buildBranchline_IdealTL();
  void buildBranchline_Microstrip();

  private:
  // Private variables for components location
  int x_spacing, y_spacing; // General components spacing
  int x_P1, y_P1, x_P2, y_P2, x_P3, y_P3; // Ports
  int x_Riso, y_Riso; // Isolation resistor

  // Transmission lines
  int x_1s_vert_TL, y_1s_vert_TL; // First vertical
  int x_top_TL, y_top_TL; // Top horizontal
  int x_bottom_TL, y_bottom_TL; // Bottom horizontal
  int x_last_TL, y_last_TL; // 2nd vertical
};

#endif // BRANCHLINE_H
