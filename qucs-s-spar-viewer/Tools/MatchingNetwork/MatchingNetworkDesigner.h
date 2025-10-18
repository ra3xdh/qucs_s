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

#ifndef MATCHINGNETWORKDESIGNER_H
#define MATCHINGNETWORKDESIGNER_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"

#include "CascadedLCSections.h"
#include "DoubleStub.h"
#include "Lambda8Lambda4.h"
#include "Lsection.h"
#include "MultisectionQuarterWave.h"
#include "SingleStub.h"

class MatchingNetworkDesigner : public Network {
public:
  MatchingNetworkDesigner(MatchingData);
  ~MatchingNetworkDesigner();
  QList<ComponentInfo> getComponents();
  QList<WireInfo> getWires();
  QList<NodeInfo> getNodes();
  QMap<QString, QPen> displaygraphs;
  void synthesize();

private:
  MatchingData Specs;
  QList<ComponentInfo> Components;
  QList<WireInfo> Wires;
  QList<NodeInfo> Nodes;

  SchematicContent
  synthesize_One_Port(MatchingNetworkDesignParameters NetworkParams,
                      double f_match);
  void synthesize_Two_Ports();
};

#endif // MATCHINGNETWORKDESIGNTOOL_H
