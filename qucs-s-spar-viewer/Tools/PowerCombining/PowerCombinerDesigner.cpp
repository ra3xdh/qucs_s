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

#include "PowerCombinerDesigner.h"

PowerCombinerDesigner::PowerCombinerDesigner(PowerCombinerParams SPC) {
  Specs = SPC;
}

void PowerCombinerDesigner::synthesize() {
  if (Specs.Type == "Wilkinson") {
    Wilkinson();
  }
  if (Specs.Type == "Multistage Wilkinson") {
    MultistageWilkinson();
  }
  if (Specs.Type == "T-junction") {
    TJunction *TJ;
    TJ = new TJunction(Specs);
    TJ->synthesize();
    SchContent = TJ->Schematic;
    delete TJ;
  }
  if (Specs.Type == "Branchline") {
    Branchline();
  }
  if (Specs.Type == "Double box branchline") {
    DoubleBoxBranchline();
  }
  if (Specs.Type == "Bagley") {
    Bagley();
  }
  if (Specs.Type == "Gysel") {
    Gysel();
  }
  if (Specs.Type == "Lim-Eom") {
    Lim_Eom();
  }
  if (Specs.Type == "3 Way Wilkinson Improved Isolation") {
    Wilkinson3Way_ImprovedIsolation();
  }
  if (Specs.Type == "Recombinant 3 Way Wilkinson") {
    Recombinant3WayWilkinson();
  }
  /*if (Specs.Type == "Travelling Wave")       TravellingWave();
  if (Specs.Type == "Tree")                  Tree();*/
}

TwoWayWilkinsonParams PowerCombinerDesigner::CalculateWilkinson() {
  double K = Specs.OutputRatio.at(0);
  TwoWayWilkinsonParams WilkinsonParams;
  // Wilkinson divider design equations
  double K2 = K * K;
  WilkinsonParams.Z3 = Specs.Z0 * sqrt((K2 + 1) / (K * K * K));
  WilkinsonParams.Z2 = K2 * WilkinsonParams.Z3;
  WilkinsonParams.R = Specs.Z0 * ((K2 + 1) / K);
  WilkinsonParams.R2 = Specs.Z0 * K;
  WilkinsonParams.R3 = Specs.Z0 / K;
  return WilkinsonParams;
}
