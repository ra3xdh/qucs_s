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
#include "MatchingNetworkDesigner.h"

MatchingNetworkDesigner::MatchingNetworkDesigner(MatchingData Params) {
  Specs = Params;
}

MatchingNetworkDesigner::~MatchingNetworkDesigner() {

}

void MatchingNetworkDesigner::synthesize(){

  if (Specs.twoPortMode){
    // Two-port matching
    synthesize_Two_Ports();
  } else {
    // One-port matching
    MatchingNetworkDesignParameters NetworkParams = Specs.InputNetworkParameters;
    double f_match = Specs.f_match; // Frequency at which the network will be matched
    synthesize_One_Port(NetworkParams, f_match);
  }
}


// Handle 1-port matching
void MatchingNetworkDesigner::synthesize_One_Port(MatchingNetworkDesignParameters NetworkParams, double f_match){

  switch (NetworkParams.Topology){

    case 0: {// L-section
      Lsection *L = new Lsection(NetworkParams, f_match);
      L->synthesize();
      Schematic = L->Schematic;
      delete L;
      break;
    }

    case 1: { // Single-stub
      SingleStub *SSM = new SingleStub(NetworkParams, f_match);
      SSM->synthesize();
      Schematic = SSM->Schematic;
      delete SSM;
      break;
    }

    case 2: { // Double-stub
      DoubleStub *DSM = new DoubleStub(NetworkParams, f_match);
      DSM->synthesize();
      Schematic = DSM->Schematic;
      delete DSM;
      break;
    }

    case 3: { // Multisection lambda/4
      MultisectionQuarterWave *MSL4 = new MultisectionQuarterWave(NetworkParams, f_match);
      MSL4->synthesize();
      Schematic = MSL4->Schematic;
      delete MSL4;
      break;
    }

    case 4: { // Cascaded LC sections
      CascadedLCSections *CLCM = new CascadedLCSections(NetworkParams, f_match);
      CLCM->synthesize();
      Schematic = CLCM->Schematic;
      delete CLCM;
      break;
    }

    case 5: { //lambda/8 + lambda/4
      Lambda8Lambda4 *L8L4 = new Lambda8Lambda4(NetworkParams, f_match);
      L8L4->synthesize();
      Schematic = L8L4->Schematic;
      delete L8L4;
      break;
    }

  }
}


// Handle 2-ports matching
void MatchingNetworkDesigner::synthesize_Two_Ports(){

  // 1) Get the frequency at which the network will be matched
  double f_match = Specs.f_match;

  // 2) Design the input matching network
  MatchingNetworkDesignParameters NetworkParams = Specs.InputNetworkParameters;
  synthesize_One_Port(NetworkParams, f_match);
  SchematicContent IMN_Schematic = this->Schematic;

  // 3) Design the output matching network
  NetworkParams = Specs.OutputNetworkParameters;
  synthesize_One_Port(NetworkParams, f_match);
  SchematicContent OMN_Schematic = this->Schematic;

  // 4) Flip vertically the output matching network with respect to the load

  // 5) Compose the final network

  // 5.1) Remove the load from the input matching network

  // 5.2) Add and connect a SPAR component

  // 5.3) Add and connect the output matching network

  // 5.4) Add the output port

}
