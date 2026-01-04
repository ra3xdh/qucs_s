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

///
/// @brief Abstract base class for network implementations
///
class Network {
public:
  virtual ~Network() {}
  ///
  /// @brief Synthesize the network
  ///
  virtual void synthesize() = 0;
  SchematicContent Schematic; ///< Circuit data including components, nets, and simulation info
};
#endif
