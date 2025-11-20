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

#include "MatchingNetworkDesigner.h"

MatchingNetworkDesigner::MatchingNetworkDesigner(MatchingData Params) {
  Specs = Params;
}

MatchingNetworkDesigner::~MatchingNetworkDesigner() {}

void MatchingNetworkDesigner::synthesize() {

  if (Specs.twoPortMode) {
    // Two-port matching
    synthesize_Two_Ports();
  } else {
    // One-port matching
    MatchingNetworkDesignParameters NetworkParams =
        Specs.InputNetworkParameters;
    double f_match =
        Specs.f_match; // Frequency at which the network will be matched
    Schematic = synthesize_One_Port(NetworkParams, f_match);
  }
}

// Handle 1-port matching
SchematicContent MatchingNetworkDesigner::synthesize_One_Port(
    MatchingNetworkDesignParameters NetworkParams, double f_match) {

  SchematicContent Schematic;
  switch (NetworkParams.Topology) {

  case 0: { // L-section
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
    MultisectionQuarterWave *MSL4 =
        new MultisectionQuarterWave(NetworkParams, f_match);
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

  case 5: { // lambda/8 + lambda/4
    Lambda8Lambda4 *L8L4 = new Lambda8Lambda4(NetworkParams, f_match);
    L8L4->synthesize();
    Schematic = L8L4->Schematic;
    delete L8L4;
    break;
  }
  }
  return Schematic;
}

// Handle 2-ports matching
void MatchingNetworkDesigner::synthesize_Two_Ports() {

  // 1) Get the frequency at which the network will be matched
  double f_match = Specs.f_match;

  // 2) Design the input matching network
  MatchingNetworkDesignParameters NetworkParams = Specs.InputNetworkParameters;
  SchematicContent IMN_Schematic = synthesize_One_Port(NetworkParams, f_match);

  // 3) Design the output matching network
  NetworkParams = Specs.OutputNetworkParameters;
  SchematicContent OMN_Schematic = synthesize_One_Port(NetworkParams, f_match);

  // 4) Flip vertically the output matching network with respect to the load

  // 4.1 Find Zloads of the input/output matching networks and get the x-axis
  // position
  double zl_input = 0;
  for (auto &comp : IMN_Schematic.Comps) {
    if (comp.ID == "Z1") {
      zl_input = comp.Coordinates[0];
      break;
    }
  }

  double zl_output = 0;
  for (auto &comp : OMN_Schematic.Comps) {
    if (comp.ID == "Z1") {
      zl_output = comp.Coordinates[0];
      break;
    }
  }

  // 4.2 Flip all components
  double OMN_Start = zl_input;    // End of the IMN
  double x_pos = 0, distance = 0; // Component x-axis
  double x_offset = 0;            // Additional x-axis offset
  QMap<QString, QString>
      replace_ID; // A map is needed to assign the new component IDs once the
                  // output matching network is created

  for (auto &comp : OMN_Schematic.Comps) {

    if (comp.ID == "Z1") {
      // This component need to be removed
      comp.ID = "Z2";
      comp.Coordinates[0] = zl_output + x_offset;
      comp.Rotation = 180;
      continue;
    }

    x_pos = comp.Coordinates[0];
    distance = zl_output -
               x_pos; // zl_output > x_pos (The load is always on the right)
    comp.Coordinates[0] = OMN_Start + distance +
                          x_offset; // Update the component's x-axis position

    if (comp.ID == "T1") {
      comp.Rotation = 180;
      comp.ID = "T2";
    } else {

      if ((comp.Rotation == -90) || (comp.Rotation == 90)) {
        // If the component is on a series branch, it need to be mirrored
        for (auto &wire : OMN_Schematic.Wires) {
          // Inspect all wires. If the beginning or the end of the wire matches
          // with the current component (to be mirrored), then change the
          // connection pins

          // Origin port
          if (wire.OriginID == comp.ID) {
            if (wire.PortOrigin == 0) {
              wire.PortOrigin = 1;
            } else {
              wire.PortOrigin = 0;
            }
          }
          // Destination port
          if (wire.DestinationID == comp.ID) {
            if (wire.PortDestination == 0) {
              wire.PortDestination = 1;
            } else {
              wire.PortDestination = 0;
            }
          }
        }
      }

      // Change name
      int new_comp_number =
          IMN_Schematic.NumberComponents.value(comp.Type, 0) + 1;
      IMN_Schematic.NumberComponents.insert(comp.Type, new_comp_number);

      if (!comp.ID.isEmpty()) {
        // Find where the trailing number starts
        int i = comp.ID.length() - 1;
        while (i >= 0 && comp.ID[i].isDigit()) {
          i--;
        }

        if (i < comp.ID.length() - 1) {
          QString prefix = comp.ID.left(i + 1); // Everything before the number
          replace_ID[comp.ID] = prefix + QString::number(new_comp_number);
          comp.ID = replace_ID[comp.ID];
        }
      }
    }
  }

  for (auto &wire : OMN_Schematic.Wires) {
    // Update name (otherwise it'll be a mesh when composing the final network)
    wire.ID = QString("%1out").arg(wire.ID);

    // The components the wire connects need also be renanmed (add +out suffix)

    if (wire.DestinationID == "T1") {
      wire.DestinationID = "T2";
    } else {
      if (wire.DestinationID == "Z1") {
        wire.DestinationID = "Z2";
      } else {
        if (!wire.DestinationID.startsWith("N")) {
          // This only applies to component, not to nodes
          wire.DestinationID = replace_ID[wire.DestinationID];
        } else {
          wire.DestinationID = QString("%1out").arg(wire.DestinationID);
        }
      }
    }

    if (wire.OriginID == "T1") {
      wire.OriginID = "T2";
    } else {
      if (wire.OriginID == "Z1") {
        wire.OriginID = "Z2";
      } else {
        if (!wire.OriginID.startsWith("N")) {
          // This only applies to component, not to nodes
          wire.OriginID = replace_ID[wire.OriginID];
        } else {
          wire.OriginID = QString("%1out").arg(wire.OriginID);
        }
      }
    }
  }

  // 4.3 Flip all nodes
  for (auto &node : OMN_Schematic.Nodes) {
    x_pos = node.Coordinates[0];
    distance =
        zl_output - x_pos; // z1_x > x_pos (The load is always on the right)
    node.Coordinates[0] = OMN_Start + distance +
                          x_offset; // Update the component's x-axis position

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

  // 5.1) Remove the load from the matching networks. Remove their associeated
  // grounds as well
  for (auto it = Schematic.Comps.begin(); it != Schematic.Comps.end();) {
    if ((it->ID == "Z1") || (it->ID == "Z2") || (it->ID.contains("GND_ZL"))) {
      it = Schematic.Comps.erase(it); // Erase and return next iterator[3][10]
    } else {
      ++it;
    }

    // Also needed to remove the associated GNDs
  }

  // 5.2) Add the SPAR component

  ComponentInfo SPAR(
      QString("SPAR%1").arg(++Schematic.NumberComponents[SPAR_Block]),
      SPAR_Block, 0, zl_input, 0);
  SPAR.val["S11r"] = num2str(Specs.sparams[0].real());
  SPAR.val["S11i"] = num2str(Specs.sparams[0].imag());

  SPAR.val["S12r"] = num2str(Specs.sparams[1].real());
  SPAR.val["S12i"] = num2str(Specs.sparams[1].imag());

  SPAR.val["S21r"] = num2str(Specs.sparams[2].real());
  SPAR.val["S21i"] = num2str(Specs.sparams[2].imag());

  SPAR.val["S22r"] = num2str(Specs.sparams[3].real());
  SPAR.val["S22i"] = num2str(Specs.sparams[3].imag());

  Schematic.Comps.append(SPAR);

  // 5.3) Replace load connections

  for (auto it = Schematic.Wires.begin(); it != Schematic.Wires.end();) {
    // Change connection pins if matching the component to be mirrored
    if (it->OriginID == "Z1") {
      it->OriginID = "SPAR1";
      it->PortOrigin = 0;
    }
    if (it->OriginID == "Z2") {
      it->OriginID = "SPAR1";
      it->PortOrigin = 1;
    }
    if (it->DestinationID == "Z1") {
      it->DestinationID = "SPAR1";
      it->PortOrigin = 0;
    }
    if (it->DestinationID == "Z2") {
      it->DestinationID = "SPAR1";
      it->PortOrigin = 0;
    }

    // Remove wires if OriginID or DestinationID contains "GND_ZL"
    if (it->OriginID.contains("GND_ZL") ||
        it->DestinationID.contains("GND_ZL")) {
      it = Schematic.Wires.erase(it); // Erase and get next iterator
    } else {
      ++it;
    }
  }
}
