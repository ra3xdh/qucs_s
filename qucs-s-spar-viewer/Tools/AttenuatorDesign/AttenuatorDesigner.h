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

#ifndef ATTENUATORDESIGNER_H
#define ATTENUATORDESIGNER_H
#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "math.h"

struct PdissAtt {
  double R1, R2, R3, R4;
};

class AttenuatorDesigner : public Network {
public:
  AttenuatorDesigner(AttenuatorDesignParameters);
  ~AttenuatorDesigner();
  QList<ComponentInfo> getComponents();
  QList<WireInfo> getWires();
  QList<NodeInfo> getNodes();
  QMap<QString, QPen> displaygraphs;
  void synthesize();
  struct PdissAtt Pdiss; // Power dissipated by the resistors

private:
  AttenuatorDesignParameters Specs;
  QList<ComponentInfo> Components;
  QList<WireInfo> Wires;
  QList<NodeInfo> Nodes;

  QString QucsNetlist;
  QMap<ComponentType, int>
      NumberComponents; // List for assigning IDs to the filter components

  // Attenuator design functions
  void TeeAttenuator();
  void BridgedTeeAttenuator();

  void ReflectionAttenuator();

  // L-pads
  void LPadFirstSeriesAttenuator();
  void LPadFirstShuntAttenuator();

  // Single resistor attenuators
  void RSeriesAttenuator();
  void RShuntAttenuator();
};

#endif // ATTENUATORDESIGNER_H
