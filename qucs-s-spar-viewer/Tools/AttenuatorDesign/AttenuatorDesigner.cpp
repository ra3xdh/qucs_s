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

#include "AttenuatorDesigner.h"

AttenuatorDesigner::AttenuatorDesigner(AttenuatorDesignParameters SPC) {
  Specs = SPC;
}

AttenuatorDesigner::~AttenuatorDesigner() {}

QList<ComponentInfo> AttenuatorDesigner::getComponents() {
  return Components;
}

QList<WireInfo> AttenuatorDesigner::getWires() {
  return Wires;
}

QList<NodeInfo> AttenuatorDesigner::getNodes() {
  return Nodes;
}

void AttenuatorDesigner::synthesize() {
  if (Specs.Topology == "Pi") {
    PiAttenuator();
  } else if (Specs.Topology == "Tee") {
    TeeAttenuator();
  } else if (Specs.Topology == "Bridged Tee") {
    BridgedTeeAttenuator();
  } else if (Specs.Topology == "Reflection Attenuator") {
    ReflectionAttenuator();
  } else if (Specs.Topology == "Quarter-wave series") {
    QW_SeriesAttenuator();
  } else if (Specs.Topology == "Quarter-wave shunt") {
    QW_ShuntAttenuator();
  } else if (Specs.Topology == "L-pad 1st series") {
    LPadFirstSeriesAttenuator();
  } else if (Specs.Topology == "L-pad 1st shunt") {
    LPadFirstShuntAttenuator();
  } else if (Specs.Topology == "Rseries") {
    RSeriesAttenuator();
  } else if (Specs.Topology == "Rshunt") {
    RShuntAttenuator();
  }
}
