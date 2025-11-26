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

#ifndef LIM_EOM_H
#define LIM_EOM_H

#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include <QPen>

/*
 * References:
 * [1] "Power combiners, impedance transformers and directional couplers: part
 * II". Andrei Grebennikov. High Frequency Electronics. 2008
 * [2] "A New 3-Way Power Divider with Various Output Power Ratios," J.-S. Lim
 * and S.-Y. Eom, 1996 IEEE MTT-S Int. Microwave Symp. Dig., pp. 785-788."
 */

class Lim_Eom : public Network {
public:
  Lim_Eom();
  virtual ~Lim_Eom();
  Lim_Eom(PowerCombinerParams);
  void synthesize();

private:
  PowerCombinerParams Specification;
  
  double lambda4;
  double Z1, Z2, Z3, Z4, Z5;
  
  void calculateParams();
  void buildLimEom_IdealTL();
  void buildLimEom_Microstrip();

private:
  // This function sets the component's location before the schematic is built
  void setComponentsLocation();

         // Private variables for components location
  int x_spacing, y_spacing; // General components spacing

         // Ports
  QVector<QPoint> Ports_pos;

         // Isolation resistor
  QVector<QPoint> Riso_pos;
  QVector<QPoint> GND_Riso_pos;

         // Transmission lines
  QVector<QPoint> TL_pos;

         // Nodes
  QVector<QPoint> N_pos;
};

#endif // LIM_EOM_H
