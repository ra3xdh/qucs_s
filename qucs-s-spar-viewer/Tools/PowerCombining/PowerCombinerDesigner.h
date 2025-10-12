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
  SchematicContent getSchematic() { return Schematic; }

private:
  PowerCombinerParams Specs;
  SchematicContent Schematic;

  // Power combiner design functions
  void Wilkinson();
  void buildWilkinson_LumpedLC(const TwoWayWilkinsonParams& WilkinsonParams);
  void buildWilkinson_IdealTL(const TwoWayWilkinsonParams& WilkinsonParams);
  void buildWilkinson_Microstrip(const TwoWayWilkinsonParams& WilkinsonParams);


  void MultistageWilkinson();
  void buildMultistageWilkinson_LumpedLC(const std::deque<double>& Zlines, const std::deque<double>& Risol);
  void buildMultistageWilkinson_IdealTL(const std::deque<double>& Zlines, const std::deque<double>& Risol, double lambda4);
  void buildMultistageWilkinson_Microstrip(const std::deque<double>& Zlines, const std::deque<double>& Risol, double lambda4);


  TwoWayWilkinsonParams CalculateWilkinson();
  void TJunction();
  void Branchline();
  void DoubleBoxBranchline();


  void Bagley();
  void buildBagley_IdealTL(double lambda4, double lambda2, double Zbranch);
  void buildBagley_Microstrip(double lambda4, double lambda2, double Zbranch);

  void Gysel();
  void Lim_Eom();
  void Wilkinson3Way_ImprovedIsolation();
  void Recombinant3WayWilkinson();
  void TravellingWave();
  void Tree();

  std::deque<double> ChebyshevTaper(double, double);
  std::deque<double> calcMultistageWilkinsonIsolators(std::deque<double>,
                                                      double,
                                                      std::complex<double>);
};

#endif // POWERCOMBINERDESIGNER_H
