/***************************************************************************
                                MatchingNetworkDesignTool.cpp
                                ----------
    copyright            :  QUCS team
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
#ifndef MATCHINGNETWORKDESIGNER_H
#define MATCHINGNETWORKDESIGNER_H


#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"

#include "Lsection.h"
#include "SingleStub.h"
#include "DoubleStub.h"
#include "MultisectionQuarterWave.h"
#include "CascadedLCSections.h"
#include "Lambda8Lambda4.h"

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

  SchematicContent synthesize_One_Port(MatchingNetworkDesignParameters NetworkParams, double f_match);
  void synthesize_Two_Ports();

};

#endif // MATCHINGNETWORKDESIGNTOOL_H
