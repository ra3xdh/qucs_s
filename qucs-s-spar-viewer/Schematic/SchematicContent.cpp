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
  NumberWires = 0;
}

// This function returns a structure for doing a simple (internal) ladder
// simulation
NetworkInfo SchematicContent::getLadder() {
  NetworkInfo NWI;
  std::vector<std::complex<double>> ZS(1), ZL(1);
  ZS[0] = Str2Complex(Comps[0].val["Z"]);                // Port 1 impedance
  ZL[0] = Str2Complex(Comps[Comps.size() - 1].val["Z"]); // Port 2 impedance
  NWI.ZS = ZS;
  NWI.ZL = ZL;
  NWI.Ladder = Comps;
  return NWI;
}

void SchematicContent::assignNetToWiresConnectedToNode(QString NodeID,
                                                       QString net) {
  for (int i = 0; i < Wires.length(); i++) {
    if (!Wires[i].getNet().isEmpty())
      continue;
    if ((Wires[i].OriginID == NodeID) || (Wires[i].DestinationID == NodeID))
      Wires[i].setNet(net);
  }
}

QString SchematicContent::getQucsNetlist() {
  // Build Qucs netlist
  /* QString QucsNetlist;
   QString codestr;
   for (int i = 0; i < Comps.length(); i++) {
     codestr = Comps[i].getQucs();
     if (!codestr.isEmpty())
       QucsNetlist += codestr;
   }
   return QucsNetlist;*/

  // Firstly, find those wires connected to ground
  // Check if the wire is connected to ground
  for (int i = 0; i < Wires.length(); i++) {
    if ((Wires[i].OriginID.contains("GND")) ||
        (Wires[i].DestinationID.contains("GND")))
      Wires[i].setNet("gnd");
  }

  // We need to find those wires which connect two nodes and
  // assign them the same net
  for (int i = 0; i < Wires.length(); i++) {
    if ((Wires[i].OriginID.at(0) == 'N') &&
        (Wires[i].DestinationID.at(0) == 'N')) {
      // Then both nodes must share the same net. In other words, every wire
      // connected to these nodes must share the same net
      QString net_name;
      if (Wires[i].getNet().isEmpty())
        net_name = Wires[i].OriginID;
      else
        net_name = Wires[i].getNet();
      // Propagate this net to the wires connected to the above nodes
      assignNetToWiresConnectedToNode(Wires[i].OriginID, net_name);
      assignNetToWiresConnectedToNode(Wires[i].DestinationID, net_name);
    }
  }

  for (int i = 0; i < Wires.length(); i++) {

    if (!Wires[i].getNet().isEmpty())
      continue; // The net was already set

    // A wire has two endings. If one of them is connected to a node, then the
    // wire ID takes the name of that node
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

  // Finally, we hace to look for those wires not connected to a node. Let's set
  // the net = Wire ID
  for (int i = 0; i < Wires.length(); i++) {
    if (!Wires[i].getNet().isEmpty())
      continue; // The net was already set

    Wires[i].setNet(Wires[i].getID());
  }

  QString codestr;
  QVector<QString> connections;
  int connection_counter;
  int open_counter = 0; // Counts the number of open nodes;
  // Now, iterate through the components list. The component name and its
  // connections are recorded in a line in the netlist
  for (int i = 0; i < Comps.length(); i++) {
    if (Comps[i].Type == GND)
      continue;
    codestr += Comps[i].getQucsCode(); // Get component code

    // Set nodes according to the information updated in the above loop. We need
    // to loop again the Wires list update the connections
    connections.clear(); // Clear the data of a previous component
    connection_counter = 0;
    connections.resize(Comps[i].getNumberOfPorts());

    // Check if the component is a term. In that case, this a 2-port device, but
    // it one of its ports is intrinsically connected to ground
    if (Comps[i].Type == Term)
      connections[1] = QString("gnd");
    if (Comps[i].Type == ShortStub)
      connections[0] = QString("gnd");
    if (Comps[i].Type == OpenStub)
      connections[0] = QString("NOPEN%1").arg(++open_counter);

    int num_ports = Comps[i].getNumberOfPorts();
    for (int k = 0; k < Wires.length(); k++) {
      if (connection_counter == num_ports)
        break;
      if (Wires[k].DestinationID == Comps[i].ID) {
        connections[Wires[k].PortDestination] = Wires[k].getNet();
        connection_counter++;
      }
      if (Wires[k].OriginID == Comps[i].ID) {
        connections[Wires[k].PortOrigin] = Wires[k].getNet();
        connection_counter++;
      }
    }
    // In case not every component port is connected to a wire, we must assign
    // the other ports are opened.
    if (connection_counter < num_ports) {
      for (int i = 0; i < num_ports; i++) {
        if (connections[i].isEmpty())
          connections[i] = QString("NOPEN%1").arg(++open_counter);
      }
    }

    // Now put the content of the connections vector into the netlist
    for (int k = 0; k < connections.size(); k++) {
      codestr += QString(" %1").arg(connections[k]);
    }

    // Set properties
    codestr += " ";
    codestr += Comps[i].getQucsProperties();
  }
  return codestr;
}


QString SchematicContent::getSParameterNetlist() {
  if (Comps.isEmpty()) {
    return netlist; // This is used in case the network is defined using a plain text netlist
  }

  netlist.clear(); // Clear previous netlist

  // First, assign nets to wires (same logic as original getQucsNetlist)

  // Find wires connected to ground
  for (int i = 0; i < Wires.length(); i++) {
    if ((Wires[i].OriginID.contains("GND")) || (Wires[i].DestinationID.contains("GND"))) {
      Wires[i].setNet("0"); // Use node 0 for ground in SPICE format
    }
  }

  // Find wires connecting two nodes and assign same net
  for (int i = 0; i < Wires.length(); i++) {
    if ((Wires[i].OriginID.at(0) == 'N') && (Wires[i].DestinationID.at(0) == 'N')) {
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
                            .arg(Comps[i].ID)
                            .arg(node1)
                            .arg(node2)
                            .arg(value);
      }
      break;

    case Capacitor:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);
        QString value = Comps[i].val.contains("C") ? Comps[i].val["C"] : "1e-12";
        componentLine = QString("%1 %2 %3 %4\n")
                            .arg(Comps[i].ID)
                            .arg(node1)
                            .arg(node2)
                            .arg(value);
      }
      break;

    case Inductor:
      if (connections.size() >= 2) {
        int node1 = netToNodeMap.value(connections[0], 0);
        int node2 = netToNodeMap.value(connections[1], 0);
        QString value = Comps[i].val.contains("L") ? Comps[i].val["L"] : "1e-9";
        componentLine = QString("%1 %2 %3 %4\n")
                            .arg(Comps[i].ID)
                            .arg(node1)
                            .arg(node2)
                            .arg(value);
      }
      break;

    case Term:
      // Terminal/Port
      if (connections.size() >= 1) {
        int node = netToNodeMap.value(connections[0], 1);
        QString impedance = Comps[i].val.contains("Z") ? Comps[i].val["Z"] : "50";
        // Change component ID to start with 'P' for port
        QString portID = Comps[i].ID;
        if (portID.at(0) != 'P') {
          portID = "P" + portID.mid(1);
        }
        componentLine = QString("%1 %2 %3\n")
                            .arg(portID)
                            .arg(node)
                            .arg(impedance);
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
                            .arg(TL_ID)
                            .arg(node1)
                            .arg(node2)
                            .arg(Z0)
                            .arg(Length);
      }
      break;

    case CoupledLines: {

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
                          .arg(CLIN_ID)
                          .arg(node1)
                          .arg(node2)
                          .arg(node4)
                          .arg(node3)
                          .arg(Z0e)
                          .arg(Z0o)
                          .arg(Length);
    }
      break;

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


void SchematicContent::setNetlist(QString netlist) {
  this->netlist = netlist;
}

void SchematicContent::appendComponent(struct ComponentInfo C) {
  Comps.append(C);
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

void SchematicContent::appendNode(struct NodeInfo N) { Nodes.append(N); }

QList<struct ComponentInfo> SchematicContent::getComponents() { return Comps; }
QList<struct WireInfo> SchematicContent::getWires() { return Wires; }
QList<struct NodeInfo> SchematicContent::getNodes() { return Nodes; }

double SchematicContent::getZin() { return Comps[0].val["Z"].toDouble(); }
double SchematicContent::getZout() {
  return Comps[Comps.size() - 1].val["Z"].toDouble();
}

QString SchematicContent::getZinString() { return Comps[0].val["Z"]; }
QString SchematicContent::getZoutString() {
  return Comps[Comps.size() - 1].val["Z"];
}
