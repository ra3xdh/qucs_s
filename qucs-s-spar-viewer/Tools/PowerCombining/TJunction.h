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

#ifndef TJUNCTION_H
#define TJUNCTION_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

class TJunction : public Network {
public:
  TJunction();
  virtual ~TJunction();
  TJunction(PowerCombinerParams);
  void synthesize();

private:
  PowerCombinerParams Specification;

  void buildTJunction_IdealTL(double lambda4, double K);
  void buildTJunction_Microstrip(double lambda4, double K);

private:

  // This function sets the component's location before the schematic is built
  void setComponentsLocation();

  // Private variables for components location
  int x_spacing, y_spacing; // General components spacing

  // Ports
  QVector<QPoint> Ports_pos;

  // Isolation resistor
  QPoint Riso_pos;
  QPoint GND_Riso_pos;

  // Transmission lines
  QVector<QPoint> TL_pos;

  // Nodes
  QVector<QPoint> N_pos;
};

#endif // TJUNCTION_H
