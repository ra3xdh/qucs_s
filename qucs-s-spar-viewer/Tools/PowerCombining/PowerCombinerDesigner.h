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

#ifndef POWERCOMBINERDESIGNER_H
#define POWERCOMBINERDESIGNER_H
#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/structures.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

#include "Wilkinson2Way.h"
#include "MultistageWilkinson.h"
#include "TJunction.h"
#include "Branchline.h"
#include "DoubleBoxBranchline.h"
#include "Bagley.h"
#include "Gysel.h"
#include "Lim_Eom.h"
#include "Wilkinson3way_ImprovedIsolation.h"
#include "Recombinant3WayWilkinson.h"

#include <QPen>

struct TwoWayWilkinsonParams {
  double Z2; // Branch 2 impedance
  double Z3; // Branch 3 impedance
  double R;  // Isolation resistor
  double R2; // Branch 2 terminating impedance
  double R3; // Branch 3 terminating impedance
};

class PowerCombinerDesigner {
public:
  PowerCombinerDesigner(PowerCombinerParams);
  void synthesize();
  SchematicContent getSchematic() { return SchContent; }

private:
  PowerCombinerParams Specs;
  SchematicContent SchContent;


  TwoWayWilkinsonParams CalculateWilkinson();


  void TravellingWave();
  void Tree();

  std::deque<double> ChebyshevTaper(double, double);
  std::deque<double> calcMultistageWilkinsonIsolators(std::deque<double>,
                                                      double,
                                                      std::complex<double>);
};

#endif // POWERCOMBINERDESIGNER_H
