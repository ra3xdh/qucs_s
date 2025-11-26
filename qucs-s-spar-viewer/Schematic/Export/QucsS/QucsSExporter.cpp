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

#include "QucsSExporter.h"
#include <QMap>
#include <QRegularExpression>

QucsSExporter::QucsSExporter(SchematicContent &sch)
    : schematic(sch), // store the reference
      x_offset(0.0), y_offset(0.0), scale_x(2.0), scale_y(1.2) {
  // Start with a clean map – Qt containers are empty by default,
  // but clearing makes the intention obvious.
  Export_Blacklists.clear();

  // -------------------------- NGSpice ------------------------------------
  // NGSpice cannot handle some microstrip components.
  Export_Blacklists["NGspice"].append(MicrostripVia);
  Export_Blacklists["NGspice"].append(MicrostripStep);
  Export_Blacklists["NGspice"].append(MicrostripCoupledLines);
  Export_Blacklists["NGspice"].append(Coupler);

  // ---------------------------- Xyce ------------------------------------
  // Xyce cannot handle several microstrip components.
  Export_Blacklists["Xyce"].append(MicrostripLine);
  Export_Blacklists["Xyce"].append(MicrostripOpen);
  Export_Blacklists["Xyce"].append(MicrostripVia);
  Export_Blacklists["Xyce"].append(MicrostripStep);
  Export_Blacklists["Xyce"].append(MicrostripCoupledLines);
  Export_Blacklists["Xyce"].append(Coupler);
}

QString QucsSExporter::exportSchematic() {
  if (schematic.Comps.isEmpty()) {
    return QString("");
  }

  QString qucsNetlist;

  // Qucs-S header
  qucsNetlist += QString("<Qucs Schematic %1>\n").arg(PACKAGE_VERSION);

  // Process components
  QString components = processComponents_QucsS(backend_simulator);

  // Was component processing ok?
  if (components == QString("-1")) {
    // Unsupported components were found -> Abort export
    return QString("-1");
  }
  qucsNetlist += components;

  // Process nodes. They are needed for the wiring
  processNodes_QucsS();

  // Process wires
  qucsNetlist += processWires_QucsS();

  qDebug() << qucsNetlist;
  return qucsNetlist;
}

QucsSExporter::~QucsSExporter() {}

QString QucsSExporter::processComponents_QucsS(QString backend_simulator) {
  QString qucs_S_Components_Netlist = QString("");
  qucs_S_Components_Netlist += QString("<Components>\n");

  // Coordinates of the bottom left (needed for putting the simulation box, etc.
  // there)
  int x_bottom = 1e6, y_bottom = -1e6;

  // System impedance
  // This is used for the complex impedance component (RFEDD), which requires a
  // Z0. This value is initialized as 50 Ohm (the most common case) and updated
  // when the port component is found.
  double Z0 = 50;

  QList<MS_Substrate>
      MS_Substrate_List; // Contains all substrates used in the design. So far
                         // the tool doesn't synthesize designs with multiple
                         // substrates, but at least, this handles the
                         // possibility

  // Unsupported components list. When the parser founds something it can't
  // parse, it put's the ID here to show that to the user at the end of the
  // export process
  QList<QPair<QString, ComponentType>> unsupported_components_list;

  // Get the blacklist for this backend
  const QList<ComponentType> &blacklist = Export_Blacklists[backend_simulator];

  for (int i = 0; i < schematic.Comps.length(); i++) {
    // Check if the component if supported by the simulator back-end
    ComponentType type = schematic.Comps[i].Type;
    if (blacklist.contains(type)) {
      unsupported_components_list.append(
          qMakePair(schematic.Comps[i].ID, type));
      continue;
    }
    // Update circuit bottom-left coordinate
    std::vector<double> coordinates = schematic.Comps[i].Coordinates;
    if (coordinates[0] < x_bottom) {
      x_bottom = coordinates[0];
    }
    if (coordinates[1] > y_bottom) {
      if (schematic.Comps[i].Type == Term) {
        y_bottom =
            coordinates[1] + y_offset +
            90; //  Terms in the internal schematic have no y-axis
                //  dimmension. It's needed to add some extra room accounting
                //  for the port's y-axis dimmension in Qucs-S and its GND
      } else {
        y_bottom = coordinates[1];
      }
    }

    // Convert component to Qucs format
    QString componentLine;

    ComponentPinMap[schematic.Comps[i].ID].resize(
        schematic.Comps[i].getNumberOfPorts());
    switch (schematic.Comps[i].Type) {
    case Resistor:
      componentLine = parseResistor_QucsS(schematic.Comps[i]);
      break;

    case ComplexImpedance:
      componentLine = parseComplexImpedance_QucsS(schematic.Comps[i], Z0);
      break;

    case Capacitor:
      componentLine = parseCapacitor_QucsS(schematic.Comps[i]);
      break;

    case Inductor:
      componentLine = parseInductor_QucsS(schematic.Comps[i]);
      break;

    case OpenStub:
    case TransmissionLine:
      componentLine = parseIdealTransmissionLine_QucsS(schematic.Comps[i]);
      break;

    case CoupledLines:
      componentLine =
          parseIdealCoupledTransmissionLines_QucsS(schematic.Comps[i]);
      break;

    case ShortStub:
      componentLine = parseShortStub_QucsS(schematic.Comps[i]);
      break;

    case Term:
      componentLine = parseTerm_QucsS(schematic.Comps[i]);
      break;

    case SPAR_Block:
      componentLine = parseSPAR_Block_QucsS(schematic.Comps[i]);
      break;

    case GND:
      componentLine = parseGND_QucsS(schematic.Comps[i]);
      break;

    case Coupler:
      componentLine = parseIdealCoupler_QucsS(schematic.Comps[i]);
      break;

      ///////////////////////////////////////////////
      // Microstrip components
    case MicrostripLine: {
      MS_Substrate subs = get_MS_Substrate(schematic.Comps[i]);
      MS_Substrate_List.append(subs);
      componentLine = parseMicrostripLine_QucsS(schematic.Comps[i]);
      break;
    }

    case MicrostripCoupledLines: {
      MS_Substrate subs = get_MS_Substrate(schematic.Comps[i]);
      MS_Substrate_List.append(subs);
      componentLine = parseMicrostripCoupledLines_QucsS(schematic.Comps[i]);
      break;
    }

    case MicrostripStep: {
      MS_Substrate subs = get_MS_Substrate(schematic.Comps[i]);
      MS_Substrate_List.append(subs);
      componentLine = parseMicrostripStep_QucsS(schematic.Comps[i]);
      break;
    }

    case MicrostripOpen: {
      MS_Substrate subs = get_MS_Substrate(schematic.Comps[i]);
      MS_Substrate_List.append(subs);
      componentLine = parseMicrostripOpen_QucsS(schematic.Comps[i]);
      break;
    }
    case MicrostripVia: {
      MS_Substrate subs = get_MS_Substrate(schematic.Comps[i]);
      MS_Substrate_List.append(subs);
      componentLine = parseMicrostripVia_QucsS(schematic.Comps[i]);
      break;
    }

      ///////////////////////////////////////////////

    default:
      unsupported_components_list.append(
          qMakePair(schematic.Comps[i].ID, schematic.Comps[i].Type));

      break;
    }

    if (!componentLine.isEmpty()) {
      qucs_S_Components_Netlist += componentLine;
    }
  }

  // Show components with parsing problems
  if (!unsupported_components_list.isEmpty()) {
    QStringList messageLines;
    messageLines << "Unsupported components found:";

    for (const auto &pair : unsupported_components_list) {
      QString id = pair.first;
      ComponentType type = pair.second;

      QString line =
          QString("• %1 (Type: %2)").arg(id).arg(ComponentTypeToString(type));
      messageLines << line;
    }

    QString message = messageLines.join("\n");
    QMessageBox::information(nullptr, "Parsing error", message);

    // Unsupported components found. Abort export process
    return QString("-1");
  }

  // Add extra room for the S-parameter simulation box, substrate and equations
  y_bottom += 100;

  // S-parameter simulation box
  qucs_S_Components_Netlist +=
      QString("<.SP SP1 1 %1 %2 0 60 0 0 \"log\" 1 \"%3\" 1 \"%4\" 1 "
              "\"%5\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n")
          .arg(x_bottom)
          .arg(y_bottom)
          .arg(schematic.f_start)
          .arg(schematic.f_stop)
          .arg(schematic.n_points);

  // Substrate box
  x_bottom += 170;
  QString SubstrateNetlist =
      addSubstrateBox(MS_Substrate_List, x_bottom, y_bottom + 30);

  if (SubstrateNetlist.isEmpty()) {
    // No substrate
    x_bottom -= 170; // Restore the x-axis coordinate
  } else {
    // One or more substrate definitions. Add them to the netlist
    qucs_S_Components_Netlist += SubstrateNetlist;
  }

  // Add equations
  x_bottom += 170;
  if (schematic.Type == QString("Power Combiner")) {
    qucs_S_Components_Netlist +=
        QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S11_dB=dB(S[1,1])\" "
                "1 \"S21_dB=dB(S[2,1])\" 1 \"S31_dB=dB(S[3,1])\" 1 "
                "\"S32_dB=dB(S[3,2])\" 1 \"yes\" 0>\n")
            .arg(x_bottom)
            .arg(y_bottom);

  } else if (schematic.Type == QString("Matching-1-port")) {
    qucs_S_Components_Netlist +=
        QString(
            "<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n")
            .arg(x_bottom)
            .arg(y_bottom);

  } else if (schematic.Type == QString("Matching-2-ports")) {
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

// This function extracts the substrate properties from a component
MS_Substrate QucsSExporter::get_MS_Substrate(ComponentInfo Comp) {
  MS_Substrate subs;
  subs.er = Comp.val["er"].toDouble();
  subs.height = Comp.val["h"].toDouble();
  subs.MetalConductivity = Comp.val["cond"].toDouble();
  subs.MetalThickness = Comp.val["th"].toDouble();
  subs.tand = Comp.val["tand"].toDouble();

  return subs;
}

QString QucsSExporter::addSubstrateBox(QList<MS_Substrate> subs_list,
                                       int x_bottom, int y_bottom) {
  QString netlist_subs_box;
  QList<MS_Substrate> subs_list_simplified = removeDuplicates(subs_list);

  for (int i = 0; i < subs_list_simplified.size(); ++i) {
    const MS_Substrate &s = subs_list_simplified[i];
    // Format fields as requested (use mm and um for better readability)
    QString line =
        QString("<SUBST Subst%1 1 %2 %3 -30 24 0 0 \"%2\" 1 \"%3 mm\" 1 "
                "\"%4 um\" 1 \"%5\" 1 \"%6\" 1 \"%7\" 1>")
            .arg(i + 1)
            .arg(x_bottom)
            .arg(y_bottom)
            .arg(QString::number(s.er, 'g', 8))
            .arg(QString::number(s.height * 1000.0, 'g', 8)) // convert m to mm
            .arg(QString::number(s.MetalThickness * 1e6, 'g',
                                 8)) // convert m to um
            .arg(QString::number(s.tand, 'g', 8))
            .arg(QString::number(s.MetalConductivity, 'g', 8))
            .arg(QString::number(0.15e-6, 'g',
                                 8)); // Roughness
    netlist_subs_box += line + "\n";
  }
  return netlist_subs_box;
}

// Get the position of the internal nodes. This will be needed later for tracing
// the wires
void QucsSExporter::processNodes_QucsS() {
  for (int i = 0; i < schematic.Nodes.length(); i++) {

    QString NodeName = schematic.Nodes[i].ID;

    int x_pos = schematic.Nodes[i].Coordinates.at(0) * scale_x;
    int y_pos = schematic.Nodes[i].Coordinates.at(1) * scale_y;

    ComponentPinMap[NodeName].resize(1); // Allocate memory for 1 point
    ComponentPinMap[NodeName][0] = QPoint(x_pos, y_pos);
  }
}

QString QucsSExporter::processWires_QucsS() {
  QString qucsWires = "<Wires>\n";

  for (int i = 0; i < schematic.Wires.size(); i++) {
    WireInfo wire = schematic.Wires.at(i);

    // Start point
    QString OriginID = wire.OriginID;
    if (OriginID.startsWith("GND"))
      continue; // Omit grounds. They don't need to be routed

    int portOrigin;
    if (OriginID.startsWith("N") ||
        (OriginID.startsWith("T") && !OriginID.startsWith("TLIN"))) {
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
    if (DestinationID.startsWith("N") ||
        (DestinationID.startsWith("T") && !DestinationID.startsWith("TLIN"))) {
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

    if ((x_start == x_end) && (y_start == y_end)) {
      continue;
    }

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
