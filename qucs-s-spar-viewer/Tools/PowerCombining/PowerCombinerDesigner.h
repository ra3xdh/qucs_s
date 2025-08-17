/***************************************************************************
                                PowerCombinerDesigner.h
                                ----------
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#ifndef POWERCOMBINERDESIGNER_H
#define POWERCOMBINERDESIGNER_H
#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/structures.h"
#include "../../Misc/general.h"
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
  void MultistageWilkinson();
  TwoWayWilkinsonParams CalculateWilkinson();
  void TJunction();
  void Branchline();
  void DoubleBoxBranchline();
  void Bagley();
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
