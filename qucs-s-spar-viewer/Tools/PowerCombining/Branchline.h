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

  // Ports
  QPoint Port_in;
  QPoint Port_out1, Port_out2;

  // Isolation resistor
  QPoint Riso_pos;
  QPoint GND_Riso;

  // Transmission lines
  QPoint TL1_pos;
  QPoint TL2_pos;
  QPoint TL3_pos;
  QPoint TL4_pos;

  // Nodes
  QPoint N1_pos, N2_pos, N3_pos, N4_pos;

};

#endif // BRANCHLINE_H
