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

// Inherited by the network implementation classes
class Network {
public:
  virtual ~Network() {}
  virtual void synthesize() = 0;
  SchematicContent Schematic; // This object contains all the circuit data of
  // the filter, i.e. components and nets and it
  // returns data for the simulation
};
#endif
