/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

// This is needed for catching the current Qucs-S version
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "./../../SchematicContent.h"

QString SchematicContent::export2QucsS() {
  if (Comps.isEmpty()) {
    return QString("");
  }

  QString qucsNetlist;

  // Qucs-S header
  qucsNetlist += QString("<Qucs Schematic %1>\n").arg(PACKAGE_VERSION);

  // Process components
  qucsNetlist += processComponents_QucsS();

  // Process nodes. They are needed for the wiring
  processNodes_QucsS();

  // Process wires
  qucsNetlist += processWires_QucsS();

  qDebug() << qucsNetlist;
  return qucsNetlist;
}

QString SchematicContent::processComponents_QucsS() {

  QString qucs_S_Components_Netlist = QString("");
  qucs_S_Components_Netlist += QString("<Components>\n");

  // Coordinates of the bottom left (needed for putting the simulation box, etc.
  // there)
  int x_bottom = 1e6, y_bottom = -1e6;

  for (int i = 0; i < Comps.length(); i++) {

    // Update circuit bottom-left coordinate
    std::vector<double> coordinates = Comps[i].Coordinates;
    if (coordinates[0] < x_bottom) {
      x_bottom = coordinates[0];
    }
    if (coordinates[1] > y_bottom) {
      y_bottom = coordinates[1];
    }

    // Convert component to Qucs format
    QString componentLine;

    ComponentPinMap[Comps[i].ID].resize(Comps[i].getNumberOfPorts());
    switch (Comps[i].Type) {
    case Resistor:
      componentLine = parseResistor_QucsS(Comps[i]);
      break;

    case Capacitor:
      componentLine = parseCapacitor_QucsS(Comps[i]);
      break;

    case Inductor:
      componentLine = parseInductor_QucsS(Comps[i]);
      break;

    case TransmissionLine:
      componentLine = parseIdealTransmissionLine_QucsS(Comps[i]);
      break;

    case Term:
      componentLine = parseTerm_QucsS(Comps[i]);
      break;

    case GND:
      componentLine = parseGND_QucsS(Comps[i]);
      break;

      ///////////////////////////////////////////////
      // Microstrip components
    case MicrostripLine:
      componentLine = parseMicrostripLine_QucsS(Comps[i]);
      break;
      ///////////////////////////////////////////////

    default:
      componentLine =
          QString(".* Unsupported component: %1\n").arg(Comps[i].ID);
      break;
    }

    if (!componentLine.isEmpty()) {
      qucs_S_Components_Netlist += componentLine;
    }
  }

  // Add S-parameter simulation box
  y_bottom += 50;

  if (y_bottom < 120) {
    y_bottom = 120;
  }
  qucs_S_Components_Netlist +=
      QString("<.SP SP1 1 %1 %2 0 60 0 0 \"log\" 1 \"%3\" 1 \"%4\" 1 "
              "\"%5\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n")
          .arg(x_bottom)
          .arg(y_bottom)
          .arg(f_start)
          .arg(f_stop)
          .arg(n_points);

  x_bottom += 200;

  // Add equations
  if (this->Type == QString("Power Combiner")) {
    qucs_S_Components_Netlist +=
        QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S11_dB=dB(S[1,1])\" "
                "1 \"S21_dB=dB(S[2,1])\" 1 \"S31_dB=dB(S[3,1])\" 1 "
                "\"S32_dB=dB(S[3,2])\" 1 \"yes\" 0>\n")
            .arg(x_bottom)
            .arg(y_bottom);

  } else if (this->Type == QString("Matching-1-port")) {
    qucs_S_Components_Netlist +=
        QString(
            "<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n")
            .arg(x_bottom)
            .arg(y_bottom);

  } else if (this->Type == QString("Matching-2-ports")) {
    qucs_S_Components_Netlist +=
        QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S21_dB=dB(S[2,1])\" "
                "1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n")
            .arg(x_bottom)
            .arg(y_bottom);
  } else {
    // Filter
    qucs_S_Components_Netlist +=
        QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S21_dB=dB(S[2,1])\" "
                "1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n")
            .arg(x_bottom)
            .arg(y_bottom);
  }

  qucs_S_Components_Netlist +=
      QString("</Components>\n"); // Close the components section

  return qucs_S_Components_Netlist;
}

// Get the position of the internal nodes. This will be needed later for tracing
// the wires
void SchematicContent::processNodes_QucsS() {
  for (int i = 0; i < Nodes.length(); i++) {

    QString NodeName = Nodes[i].ID;

    int x_pos = Nodes[i].Coordinates.at(0) * scale_x_QucsS_export;
    int y_pos = Nodes[i].Coordinates.at(1) * scale_y_QucsS_export;

    ComponentPinMap[NodeName].resize(1); // Allocate memory for 1 point
    ComponentPinMap[NodeName][0] = QPoint(x_pos, y_pos);
  }
}

QString SchematicContent::processWires_QucsS() {
  QString qucsWires = "<Wires>\n";

  for (int i = 0; i < Wires.size(); i++) {
    WireInfo wire = Wires.at(i);

    // Start point
    QString OriginID = wire.OriginID;
    if (OriginID.startsWith("GND"))
      continue; // Omit grounds. They don't need to be routed

    int portOrigin;
    if (OriginID.startsWith("N") || OriginID.startsWith("T")) {
      // Node
      portOrigin = 0;
    } else {
      // Component
      portOrigin = wire.PortOrigin;
    }
    QPointF start = ComponentPinMap[OriginID].at(portOrigin);

    // End point
    QString DestinationID = wire.DestinationID;
    if (DestinationID.startsWith("GND"))
      continue; // Omit grounds. They don't need to be routed

    int portDestination;
    if (DestinationID.startsWith("N") || DestinationID.startsWith("T")) {
      // Node and terms are treated as 1 port devices
      portDestination = 0;
    } else {
      // Component
      portDestination = wire.PortDestination;
    }

    QPointF end = ComponentPinMap[DestinationID].at(portDestination);

    int x_start = qRound(start.x());
    int y_start = qRound(start.y());
    int x_end = qRound(end.x());
    int y_end = qRound(end.y());

    if (x_end < x_start) {
      // Flip.
      int x_temp = x_start;
      int y_temp = y_start;

      x_start = x_end;
      y_start = y_end;

      x_end = x_temp;
      y_end = y_temp;
    }

    // Qucs-S requires wires to be either horizontal or vertical
    // If the wire is diagonal, we need to split it into segments
    if (x_start != x_end && y_start != y_end) {
      // Create a L-shaped path
      // First horizontal segment

      if (y_end < y_start) {

        // Do this:
        //       ------(end)
        //      |
        //      |
        // (start)

        // Horizontal wire
        qucsWires += QString("<%1 %2 %3 %4 \"\" 0 0 0 \"\">\n")
                         .arg(x_start)
                         .arg(y_end)
                         .arg(x_end)
                         .arg(y_end);

        // Vertical segment
        qucsWires += QString("<%1 %2 %3 %4 \"\" 0 0 0 \"\">\n")
                         .arg(x_start)
                         .arg(y_start)
                         .arg(x_start)
                         .arg(y_end);

      } else {
        // y_end < y_start

        // Do this:
        //   (start)  ----
        //                |
        //                |
        //               (end)

        // Horizontal wire
        qucsWires += QString("<%1 %2 %3 %4 \"\" 0 0 0 \"\">\n")
                         .arg(x_start)
                         .arg(y_start)
                         .arg(x_end)
                         .arg(y_start);

        // Vertical segment
        qucsWires += QString("<%1 %2 %3 %4 \"\" 0 0 0 \"\">\n")
                         .arg(x_end)
                         .arg(y_start)
                         .arg(x_end)
                         .arg(y_end);
      }

    } else {
      // Wire is already horizontal or vertical
      qucsWires += QString("<%1 %2 %3 %4 \"\" 0 0 0 \"\">\n")
                       .arg(x_start)
                       .arg(y_start)
                       .arg(x_end)
                       .arg(y_end);
    }
  }

  qucsWires += "</Wires>\n";
  return qucsWires;
}
