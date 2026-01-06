/// @file SchematicContent.cpp
/// @brief Container for schematic circuit data (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "SchematicContent.h"

SchematicContent::SchematicContent() {
  Comps.clear();
  Nodes.clear();
  Wires.clear();
  // Initialize list of components
  NumberComponents[Capacitor] = 0;
  NumberComponents[Inductor] = 0;
  NumberComponents[Term] = 0;
  NumberComponents[GND] = 0;
  NumberComponents[ConnectionNodes] = 0;
  NumberComponents[Resistor] = 0;
  NumberComponents[TransmissionLine] = 0;
  NumberComponents[CoupledLines] = 0;
  NumberComponents[Coupler] = 0;
  NumberComponents[OpenStub] = 0;
  NumberComponents[ShortStub] = 0;
  NumberComponents[SPAR_Block] = 0;
  NumberWires = 0;
}

void SchematicContent::assignNetToWiresConnectedToNode(QString NodeID,
                                                       QString net) {
  for (int i = 0; i < Wires.length(); i++) {
    if (!Wires[i].getNet().isEmpty()) {
      continue;
    }
    if ((Wires[i].OriginID == NodeID) || (Wires[i].DestinationID == NodeID)) {
      Wires[i].setNet(net);
    }
  }
}

QString SchematicContent::getSParameterNetlist() {
  if (Comps.isEmpty()) {
    return netlist; // This is used in case the network is defined using a plain
                    // text netlist
  }

  netlist.clear(); // Clear previous netlist

  // First, assign nets to wires (same logic as original getQucsNetlist)

  // Find wires connected to ground
  for (int i = 0; i < Wires.length(); i++) {
    if ((Wires[i].OriginID.contains("GND")) ||
        (Wires[i].DestinationID.contains("GND"))) {
      Wires[i].setNet("0"); // Use node 0 for ground in SPICE format
    }
  }

  // Find wires connecting two nodes and assign same net
  for (int i = 0; i < Wires.length(); i++) {
    if ((Wires[i].OriginID.at(0) == 'N') &&
        (Wires[i].DestinationID.at(0) == 'N')) {
      QString net_name;
      if (Wires[i].getNet().isEmpty()) {
        net_name = Wires[i].OriginID;
      } else {
        net_name = Wires[i].getNet();
      }
      assignNetToWiresConnectedToNode(Wires[i].OriginID, net_name);
      assignNetToWiresConnectedToNode(Wires[i].DestinationID, net_name);
    }
  }

  // Assign nets for wires connected to nodes
  for (int i = 0; i < Wires.length(); i++) {
    if (!Wires[i].getNet().isEmpty()) {
      continue;
    }

    QString net_name;
    if (Wires[i].OriginID.at(0) == 'N') {
      net_name = Wires[i].OriginID;
      Wires[i].setNet(net_name);
      assignNetToWiresConnectedToNode(Wires[i].OriginID, net_name);
    }
    if (Wires[i].DestinationID.at(0) == 'N') {
      net_name = Wires[i].DestinationID;
      Wires[i].setNet(net_name);
      assignNetToWiresConnectedToNode(Wires[i].DestinationID, net_name);
    }
  }

  // Set remaining wires net = Wire ID
  for (int i = 0; i < Wires.length(); i++) {
    if (!Wires[i].getNet().isEmpty()) {
      continue;
    }
    Wires[i].setNet(Wires[i].getID());
  }

  // Create a mapping from net names to node numbers
  QMap<QString, int> netToNodeMap;
  int nodeCounter = 1; // Start from 1, 0 is reserved for ground

  // Reserve node 0 for ground
  netToNodeMap["0"] = 0;
  netToNodeMap["gnd"] = 0;

  // Assign node numbers to all nets
  for (int i = 0; i < Wires.length(); i++) {
    QString net = Wires[i].getNet();
    if (!netToNodeMap.contains(net) && net != "0" && net != "gnd") {
      netToNodeMap[net] = nodeCounter++;
    }
  }

  // Process components and generate SPICE-like netlist
  for (int i = 0; i < Comps.length(); i++) {
    if (Comps[i].Type == GND) {
      continue;
    }

    // Get component connections
    QVector<QString> connections;
    int connection_counter = 0;
    connections.resize(Comps[i].getNumberOfPorts());

    // Handle special cases for grounded components
    if (Comps[i].Type == Term) {
      connections[1] = QString("0"); // Ground
    }
    if (Comps[i].Type == ShortStub) {
      connections[0] = QString("0"); // Ground
    }

    int num_ports = Comps[i].getNumberOfPorts();

    // Find wire connections for this component
    for (int k = 0; k < Wires.length(); k++) {
      if (connection_counter == num_ports) {
        break;
      }
      if (Wires[k].DestinationID == Comps[i].ID) {
        connections[Wires[k].PortDestination] = Wires[k].getNet();
        connection_counter++;
      }
      if (Wires[k].OriginID == Comps[i].ID) {
        connections[Wires[k].PortOrigin] = Wires[k].getNet();
        connection_counter++;
      }
    }

    // Convert component to SPICE format
    QString componentLine;

    switch (Comps[i].Type) {
    case Resistor:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);
        QString value = Comps[i].val.contains("R") ? Comps[i].val["R"] : "50";
        componentLine = QString("%1 %2 %3 %4\n")
                            .arg(Comps[i].ID, QString::number(node1),
                                 QString::number(node2), value);
      }
      break;

    case Capacitor:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);
        QString value =
            Comps[i].val.contains("C") ? Comps[i].val["C"] : "1e-12";
        componentLine = QString("%1 %2 %3 %4\n")
                            .arg(Comps[i].ID, QString::number(node1),
                                 QString::number(node2), value);
      }
      break;

    case Inductor:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);
        QString value = Comps[i].val.contains("L") ? Comps[i].val["L"] : "1e-9";
        componentLine = QString("%1 %2 %3 %4\n")
                            .arg(Comps[i].ID, QString::number(node1),
                                 QString::number(node2), value);
      }
      break;

    case ComplexImpedance:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);
        QString value = Comps[i].val.contains("Z") ? Comps[i].val["Z"] : "50";
        static const QRegularExpression whitespaceRegex("\\s");
        value = value.remove(whitespaceRegex); // Remove blank spaces

        componentLine = QString("%1 %2 %3 %4\n")
                            .arg(Comps[i].ID, QString::number(node1),
                                 QString::number(node2), value);
      }
      break;

    case Term:
      // Terminal/Port
      if (connections.size() >= 1) {
        int node = netToNodeMap.value(connections[0], 1);
        QString impedance =
            Comps[i].val.contains("Z") ? Comps[i].val["Z"] : "50";
        // Change component ID to start with 'P' for port
        QString portID = Comps[i].ID;
        if (portID.at(0) != 'P') {
          portID = "P" + portID.mid(1);
        }
        componentLine =
            QString("%1 %2 %3\n").arg(portID, QString::number(node), impedance);
      }
      break;

    case OpenStub:
    case ShortStub:
    case TransmissionLine:
      // Treat as resistor with characteristic impedance for now
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);

        QString Z0 = Comps[i].val["Z0"];
        QString Length = Comps[i].val["Length"];

        QString TL_ID = Comps[i].ID;
        componentLine = QString("%1 %2 %3 %4 %5\n")
                            .arg(TL_ID, QString::number(node1),
                                 QString::number(node2), Z0, Length);
      }
      break;

    case MicrostripLine:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);

        QString Width = Comps[i].val["Width"];
        QString Length = Comps[i].val["Length"];
        QString er = Comps[i].val["er"];
        QString h = Comps[i].val["h"];
        QString cond = Comps[i].val["cond"];
        QString th = Comps[i].val["th"];
        QString tand = Comps[i].val["tand"];

        QString MLIN_ID = Comps[i].ID;
        componentLine =
            QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n")
                .arg(MLIN_ID, QString::number(node1), QString::number(node2),
                     Width, Length, er, h, cond, th, tand);
      }

      break;

    case MicrostripCoupledLines:
      if (connections.size() >= 2) {

        // This is needed to avoid connecting open ends to GND
        int node1 = netToNodeMap.value(connections[0], -1);
        if (node1 == -1) {
          nodeCounter++;
          node1 = nodeCounter;
        }

        int node2 = netToNodeMap.value(connections[1], -1);
        if (node2 == -1) {
          nodeCounter++;
          node2 = nodeCounter;
        }

        int node3 = netToNodeMap.value(connections[2], -1);
        if (node3 == -1) {
          nodeCounter++;
          node3 = nodeCounter;
        }

        int node4 = netToNodeMap.value(connections[3], -1);
        if (node4 == -1) {
          nodeCounter++;
          node4 = nodeCounter;
        }

        QString W = Comps[i].val["W"];
        QString L = Comps[i].val["L"];
        QString S = Comps[i].val["S"];
        QString er = Comps[i].val["er"];
        QString h = Comps[i].val["h"];
        QString cond = Comps[i].val["cond"];
        QString th = Comps[i].val["th"];
        QString tand = Comps[i].val["tand"];

        QString MLIN_ID = Comps[i].ID;
        componentLine =
            QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13\n")
                .arg(MLIN_ID, QString::number(node1), QString::number(node2),
                     QString::number(node3), QString::number(node4), W, L, S,
                     er, h, cond, th, tand);
      }

      break;

    case MicrostripOpen:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);

        QString Width = Comps[i].val["Width"];
        QString er = Comps[i].val["er"];
        QString h = Comps[i].val["h"];
        QString cond = Comps[i].val["cond"];
        QString th = Comps[i].val["th"];
        QString tand = Comps[i].val["tand"];

        QString MLIN_ID = Comps[i].ID;
        componentLine = QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
                            .arg(MLIN_ID, QString::number(node1), Width, er, h,
                                 cond, th, tand);
      }

      break;

    case MicrostripStep:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);

        QString W1 = Comps[i].val["W1"];
        QString W2 = Comps[i].val["W2"];
        QString er = Comps[i].val["er"];
        QString h = Comps[i].val["h"];
        QString cond = Comps[i].val["cond"];
        QString th = Comps[i].val["th"];
        QString tand = Comps[i].val["tand"];

        QString MLIN_ID = Comps[i].ID;
        componentLine =
            QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10\n")
                .arg(MLIN_ID, QString::number(node1), QString::number(node2),
                     W1, W2, er, h, cond, th, tand);
      }

      break;

    case MicrostripVia:
      if (connections.size() >= 1) {
        int node1 = netToNodeMap.value(connections[0], 0);

        QString D = Comps[i].val["D"];
        QString N = Comps[i].val["N"];
        QString er = Comps[i].val["er"];
        QString h = Comps[i].val["h"];
        QString cond = Comps[i].val["cond"];
        QString th = Comps[i].val["th"];
        QString tand = Comps[i].val["tand"];

        QString MLIN_ID = Comps[i].ID;
        componentLine = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9\n")
                            .arg(MLIN_ID, QString::number(node1), D, N, er, h,
                                 cond, th, tand);
      }

      break;

    case SPAR_Block:
      // Treat as resistor with characteristic impedance for now
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);

        QString path = Comps[i].val["Path"];
        QString SPAR_ID = Comps[i].ID;

        if (path.isEmpty()) {
          // Hardcoded S-par data
          QString S11r = Comps[i].val["S11r"];
          QString S11i = Comps[i].val["S11i"];

          QString S12r = Comps[i].val["S12r"];
          QString S12i = Comps[i].val["S12i"];

          QString S21r = Comps[i].val["S21r"];
          QString S21i = Comps[i].val["S21i"];

          QString S22r = Comps[i].val["S22r"];
          QString S22i = Comps[i].val["S22i"];

          componentLine =
              QString("%1 %2 %3 (%4,%5) (%6,%7); (%8,%9) (%10,%11)\n")
                  .arg(SPAR_ID, QString::number(node1), QString::number(node2),
                       S11r, S11i, S12r, S12i, S21r, S21i, S22r, S22i);
        } else {
          // S-parameter file
          componentLine = QString("%1 %2 %3 %4\n")
                              .arg(SPAR_ID)
                              .arg(node1)
                              .arg(node2)
                              .arg(path);
        }
      }
      break;

    case Coupler: {

      // This is needed to avoid connecting open ends to GND
      int node1 = netToNodeMap.value(connections[0], -1);
      if (node1 == -1) {
        nodeCounter++;
        node1 = nodeCounter;
      }
      int node2 = netToNodeMap.value(connections[1], -1);
      if (node2 == -1) {
        nodeCounter++;
        node2 = nodeCounter;
      }
      int node3 = netToNodeMap.value(connections[2], -1);
      if (node3 == -1) {
        nodeCounter++;
        node3 = nodeCounter;
      }
      int node4 = netToNodeMap.value(connections[3], -1);
      if (node4 == -1) {
        nodeCounter++;
        node4 = nodeCounter;
      }
      QString Z0 = Comps[i].val["Z0"];       // Reference impedance
      QString k = Comps[i].val["k"];         // Coupling factor
      QString Phase = Comps[i].val["phase"]; // Coupler phase shift
      QString COUP_ID = Comps[i].ID;
      componentLine = QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
                          .arg(COUP_ID, QString::number(node1),
                               QString::number(node2), QString::number(node4),
                               QString::number(node3), k, Phase, Z0);

      break;
    }
    case CoupledLines: {

      // This is needed to avoid connecting open ends to GND
      int node1 = netToNodeMap.value(connections[0], -1);
      if (node1 == -1) {
        nodeCounter++;
        node1 = nodeCounter;
      }

      int node2 = netToNodeMap.value(connections[1], -1);
      if (node2 == -1) {
        nodeCounter++;
        node2 = nodeCounter;
      }

      int node3 = netToNodeMap.value(connections[2], -1);
      if (node3 == -1) {
        nodeCounter++;
        node3 = nodeCounter;
      }

      int node4 = netToNodeMap.value(connections[3], -1);
      if (node4 == -1) {
        nodeCounter++;
        node4 = nodeCounter;
      }

      QString Z0e = Comps[i].val["Ze"];
      QString Z0o = Comps[i].val["Zo"];
      QString Length = Comps[i].val["Length"];

      QString CLIN_ID = Comps[i].ID;
      componentLine = QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
                          .arg(CLIN_ID, QString::number(node1),
                               QString::number(node2), QString::number(node4),
                               QString::number(node3), Z0e, Z0o, Length);
    } break;

    default:
      // Skip unsupported components
      componentLine = QString("* Unsupported component: %1\n").arg(Comps[i].ID);
      break;
    }

    if (!componentLine.isEmpty()) {
      netlist += componentLine;
    }
  }

  return netlist;
}

void SchematicContent::appendWire(WireInfo WI) {
  // Set wire ID
  QString ID = QString("W%1").arg(++NumberWires);
  WI.setID(ID);
  Wires.append(WI);
}

void SchematicContent::appendWire(QString O, int ON, QString D, int DN) {
  WireInfo WI(O, ON, D, DN);
  // Set wire ID
  QString ID = QString("W%1").arg(++NumberWires);
  WI.setID(ID);
  Wires.append(WI);
}

void SchematicContent::appendWire(QString O, int ON, QString D, int DN,
                                  QColor c) {
  WireInfo WI(O, ON, D, DN);
  WI.WireColor = c;
  // Set wire ID
  QString ID = QString("W%1").arg(++NumberWires);
  WI.setID(ID);
  Wires.append(WI);
}

int SchematicContent::getComponentCounter(ComponentType Comp) {
  // If NumberComponents map is empty, build it from Comps list
  if (NumberComponents.isEmpty() && !Comps.isEmpty()) {
    // Initialize counters for all component types to 0
    NumberComponents.clear();

    // Count each component type in Comps
    for (const ComponentInfo &comp : std::as_const(Comps)) {
      NumberComponents[comp.Type]++;
    }
  }

  // Return the count for the requested component type
  // Returns 0 if the type doesn't exist in the map
  return NumberComponents.value(Comp, 0);
}

QString SchematicContent::exportSchematic(QString environment,
                                          QString backend) {

  QString schematic;
  if (environment == QString("Qucs-S")) {
    // Qucs-S frontend
    QucsSExporter QE(*this);
    QE.backend_simulator = backend;
    schematic = QE.exportSchematic();
  }
  return schematic;
}
