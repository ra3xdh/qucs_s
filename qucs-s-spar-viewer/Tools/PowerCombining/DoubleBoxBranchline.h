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

private:

  // This function sets the component's location before the schematic is built
  void setComponentsLocation();

  // Private variables for components location
  int x_spacing, y_spacing; // General components spacing

    // Ports
  QPoint Port_in;
  QPoint Port_out1, Port_out2;

  // Isolation resistor
  QPoint Riso_pos;
  QPoint GND_Riso_pos;

  // Transmission lines
  QPoint TL1_pos, TL2_pos, TL3_pos, TL4_pos, TL5_pos, TL6_pos, TL7_pos;

  // Nodes
  QPoint N1_pos, N2_pos, N3_pos, N4_pos, N5_pos, N6_pos;
};

#endif // DOUBLE_BOX_BRANCHLINE_H
