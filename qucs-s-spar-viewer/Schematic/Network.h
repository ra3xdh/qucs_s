/// @file Network.h
/// @brief Abstract base class for network implementations
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 7, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef NETWORK_H
#define NETWORK_H

#include "../Schematic/SchematicContent.h"
#include <QMap>
#include <QPen>
#include <QString>
#include <QStringList>
#include <complex>
#include <deque>
#include <vector>

class SchematicContent;
class WireInfo;
class NodeInfo;
class ComponentInfo;

/// @class Network
/// @brief Abstract base class for network implementations
class Network {
public:
  /// @brief Destructor
  virtual ~Network() {}
  /// @brief Synthesize the network
  virtual void synthesize() = 0;
  SchematicContent Schematic; ///< Circuit data including components, nets, and simulation info
};
#endif
