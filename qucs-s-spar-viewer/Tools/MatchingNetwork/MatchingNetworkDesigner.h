/// @file MatchingNetworkDesigner.h
/// @brief Matching network design management(definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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

/// @class MatchingNetworkDesigner
/// @brief Management of the matching network design
class MatchingNetworkDesigner : public Network {
public:
  /// @brief Class constructor
  MatchingNetworkDesigner(MatchingData Params) {
    Specs = Params;
  }

  /// @brief Class destructor
  ~MatchingNetworkDesigner() {}

  /// @brief Get component list
  /// @return List of components in the network
  QList<ComponentInfo> getComponents();

  /// @brief Get wire list
  /// @return List of wires in the network
  QList<WireInfo> getWires();

  /// @brief Get node list
  /// @return List of nodes in the network
  QList<NodeInfo> getNodes();

  /// @brief Display graphs
  QMap<QString, QPen> displaygraphs;

  /// @brief Synthesize the matching network
  void synthesize();

private:
  /// @brief Synthesize a one-port matching network
  /// @param NetworkParams Network design parameters
  /// @param f_match Matching frequency in Hz
  /// @return Schematic content of the synthesized network
  SchematicContent
  synthesize_One_Port(MatchingNetworkDesignParameters NetworkParams,
                      double f_match);

  /// @brief Synthesize a two-port matching network
  void synthesize_Two_Ports();

  MatchingData Specs;              ///< Design specifications
  QList<ComponentInfo> Components; ///< List of components
  QList<WireInfo> Wires;           ///< List of wires
  QList<NodeInfo> Nodes;           ///< List of nodes
};

#endif // MATCHINGNETWORKDESIGNTOOL_H
