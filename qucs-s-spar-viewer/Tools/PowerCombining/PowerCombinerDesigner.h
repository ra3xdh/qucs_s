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


  void Bagley();
  void buildBagley_IdealTL(double lambda4, double lambda2, double Zbranch);
  void buildBagley_Microstrip(double lambda4, double lambda2, double Zbranch);

  void Gysel();
  void buildGysel_IdealTL(double lambda4, double lambda2);
  void buildGysel_Microstrip(double lambda4, double lambda2);


  void Lim_Eom();
  void buildLimEom_IdealTL(double lambda4, double Z1, double Z2, double Z4, double Z5);
  void buildLimEom_Microstrip(double lambda4, double Z1, double Z2, double Z4, double Z5);

  void Wilkinson3Way_ImprovedIsolation();
  void buildWilkinson3Way_IdealTL(double lambda4, double Z1, double Z2, double R1, double R2);
  void buildWilkinson3Way_Microstrip(double lambda4, double Z1, double Z2, double R1, double R2);

  void Recombinant3WayWilkinson();
  void buildRecombinant3Way_Microstrip(double lambda4, double Z1, double Z2, double Z4, double R1, double R2);
  void buildRecombinant3Way_IdealTL(double lambda4, double Z1, double Z2, double Z4, double R1, double R2);

  void TravellingWave();
  void Tree();

  std::deque<double> ChebyshevTaper(double, double);
  std::deque<double> calcMultistageWilkinsonIsolators(std::deque<double>,
                                                      double,
                                                      std::complex<double>);
};

#endif // POWERCOMBINERDESIGNER_H
