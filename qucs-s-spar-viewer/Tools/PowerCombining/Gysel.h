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

#ifndef GYSEL_H
#define GYSEL_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

class Gysel : public Network {
public:
  Gysel();
  virtual ~Gysel();
  Gysel(PowerCombinerParams);
  void synthesize();

private:
  PowerCombinerParams Specification;
  
  double lambda4, lambda2;
  
  void calculateParams();
  void buildGysel_IdealTL();
  void buildGysel_Microstrip();

  private:

  // Components' locations

  // These variables are put as class private because these positions are shared between TLIN and MS implemenations

  // This function sets the component's location before the schematic is built
  void setComponentsLocation();

  // Private variables for components location
  int x_spacing, y_spacing; // General components spacing

  // Ports
  QPoint Port_in;
  QPoint Port_out_up;
  QPoint Port_out_bottom;

  // Nodes
  QPoint N1_pos; // Node in front of the input port
  QPoint N2_pos; // Node in front of the upper output port
  QPoint N3_pos; // Node in front of the lower output port
  QPoint N4_pos; // Node in front of the upper resistor
  QPoint N5_pos; // Node in front of the lower resistor

  // Transmission lines
  QPoint TL1_pos;
  QPoint TL2_pos;
  QPoint TL3_pos;
  QPoint TL4_pos;
  QPoint TL5_pos;

  // Resistors
  QPoint R_top, R_GND_top;
  QPoint R_bottom, R_GND_bottom;

};

#endif // GYSEL_H
