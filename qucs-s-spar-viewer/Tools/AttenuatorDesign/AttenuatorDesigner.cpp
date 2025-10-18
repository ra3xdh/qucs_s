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

#include "BridgedTeeAttenuator.h"
#include "L_pad_1st_series.h"
#include "L_pad_1st_shunt.h"
#include "PiAttenuator.h"
#include "QW_SeriesAttenuator.h"
#include "QW_ShuntAttenuator.h"
#include "RSeriesAttenuator.h"
#include "ReflectionAttenuator.h"
#include "RshuntAttenuator.h"
#include "TeeAttenuator.h"

AttenuatorDesigner::AttenuatorDesigner(AttenuatorDesignParameters SPC) {
  Specs = SPC;
}

AttenuatorDesigner::~AttenuatorDesigner() {}

QList<ComponentInfo> AttenuatorDesigner::getComponents() { return Components; }

QList<WireInfo> AttenuatorDesigner::getWires() { return Wires; }

QList<NodeInfo> AttenuatorDesigner::getNodes() { return Nodes; }

void AttenuatorDesigner::synthesize() {
  if (Specs.Topology == "Pi") {
    PiAttenuator *PI_AT;
    PI_AT = new PiAttenuator(Specs);
    PI_AT->synthesize();
    Schematic = PI_AT->Schematic;
    delete PI_AT;
  } else if (Specs.Topology == "Tee") {
    TeeAttenuator *TEE_AT;
    TEE_AT = new TeeAttenuator(Specs);
    TEE_AT->synthesize();
    Schematic = TEE_AT->Schematic;
    delete TEE_AT;
  } else if (Specs.Topology == "Bridged Tee") {
    BridgedTeeAttenuator *BT_AT;
    BT_AT = new BridgedTeeAttenuator(Specs);
    BT_AT->synthesize();
    Schematic = BT_AT->Schematic;
    delete BT_AT;
  } else if (Specs.Topology == "Reflection Attenuator") {
    ReflectionAttenuator *RFAT;
    RFAT = new ReflectionAttenuator(Specs);
    RFAT->synthesize();
    Schematic = RFAT->Schematic;
    delete RFAT;
  } else if (Specs.Topology == "Quarter-wave series") {

    QW_SeriesAttenuator *QWS;
    QWS = new QW_SeriesAttenuator(Specs);
    QWS->synthesize();
    Schematic = QWS->Schematic;
    delete QWS;

  } else if (Specs.Topology == "Quarter-wave shunt") {

    QW_ShuntAttenuator *QWS;
    QWS = new QW_ShuntAttenuator(Specs);
    QWS->synthesize();
    Schematic = QWS->Schematic;
    delete QWS;

  } else if (Specs.Topology == "L-pad 1st series") {
    LPadFirstSeries *LPFS;
    LPFS = new LPadFirstSeries(Specs);
    LPFS->synthesize();
    Schematic = LPFS->Schematic;
    delete LPFS;
  } else if (Specs.Topology == "L-pad 1st shunt") {
    LPadFirstShunt *LPFS;
    LPFS = new LPadFirstShunt(Specs);
    LPFS->synthesize();
    Schematic = LPFS->Schematic;
    delete LPFS;
  } else if (Specs.Topology == "Rseries") {
    RSeriesAttenuator *RS;
    RS = new RSeriesAttenuator(Specs);
    RS->synthesize();
    Schematic = RS->Schematic;
    delete RS;
  } else if (Specs.Topology == "Rshunt") {
    RShuntAttenuator *RS;
    RS = new RShuntAttenuator(Specs);
    RS->synthesize();
    Schematic = RS->Schematic;
    delete RS;
  }
}
