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

  // 4.1 Find Zload component
  double z1_x = 0;
  for (auto& comp : OMN_Schematic.Comps) {
    if (comp.ID == "Z1") {
      z1_x = comp.Coordinates[0];
      break;
    }
  }

  // 4.2 Flip all components
  double x_pos = 0, distance = 0; // Component x-axis
  double x_offset = 50; // Additional x-axis offset
  for (auto& comp : OMN_Schematic.Comps) {

    if (comp.ID == "Z1") {
      // The load component is just offseted
      // The name (for now) need to be changed
      comp.ID = "Z2";
      comp.Coordinates[0] = z1_x + x_offset;
      continue;
    }

    x_pos = comp.Coordinates[0];
    distance = z1_x - x_pos; // z1_x > x_pos (The load is always on the right)
    comp.Coordinates[0] = z1_x + distance + x_offset; // Update the component's x-axis position

    if (comp.ID == "T1"){
      comp.Rotation = 0;
      comp.ID = "T2";
    } else {


      if (comp.Rotation == -90){
        // If the component is on a series branch, it need to be mirrored
        for (auto& wire : OMN_Schematic.Wires) {
          // Inspect all wires. If the beginning or the end of the wire matches with the current component (to be mirrored), then change the connection pins

          // Origin port
          if(wire.OriginID == comp.ID){
            if (wire.PortOrigin == 0){
              wire.PortOrigin = 1;
            } else {
              wire.PortOrigin = 0;
            }
          }
          // Destination port
          if(wire.DestinationID == comp.ID){
            if (wire.PortDestination == 0){
              wire.PortDestination = 1;
            } else {
              wire.PortDestination = 0;
            }
          }
        }
      }

      // Change name
      comp.ID = QString("%1out").arg(comp.ID);
    }
  }

  for (auto& wire : OMN_Schematic.Wires) {
    // Update name (otherwise it'll be a mesh when composing the final network)
    wire.ID = QString("%1out").arg(wire.ID);

    // The components the wire connects need also be renanmed (add +out suffix)

    if (wire.DestinationID == "T1"){
      wire.DestinationID = "T2";
    } else{
      if (wire.DestinationID == "Z1"){
        wire.DestinationID = "Z2";
      } else {
        wire.DestinationID = QString("%1out").arg(wire.DestinationID);
      }
    }

    if (wire.OriginID == "T1"){
      wire.OriginID = "T2";
    } else{
      if (wire.OriginID == "Z1"){
        wire.OriginID = "Z2";
      } else {
        wire.OriginID = QString("%1out").arg(wire.OriginID);
      }
    }

  }

  // 4.3 Flip all nodes
  for (auto& node : OMN_Schematic.Nodes) {
    x_pos = node.Coordinates[0];
    distance = z1_x - x_pos; // z1_x > x_pos (The load is always on the right)
    node.Coordinates[0] = z1_x + distance + x_offset; // Update the component's x-axis position

    // Update name (otherwise it'll be a mesh when composing the final network)
    node.ID = QString("%1out").arg(node.ID);
  }

  // 5) Compose the final network
  // Clear data
  Schematic.Comps.clear();
  Schematic.Nodes.clear();
  Schematic.Wires.clear();

  Schematic.Comps.append(IMN_Schematic.Comps);
  Schematic.Comps.append(OMN_Schematic.Comps);

  Schematic.Nodes.append(IMN_Schematic.Nodes);
  Schematic.Nodes.append(OMN_Schematic.Nodes);

  Schematic.Wires.append(IMN_Schematic.Wires);
  Schematic.Wires.append(OMN_Schematic.Wires);

  // 5.1) Remove the load from the input matching network

  // 5.2) Add and connect a SPAR component

  // 5.3) Add and connect the output matching network

  // 5.4) Add the output port

}
