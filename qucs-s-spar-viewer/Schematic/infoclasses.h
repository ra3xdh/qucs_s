#ifndef INFOCLASSES_H
#define INFOCLASSES_H
#include "../Schematic/structures.h"
#include <QColor>
#include <QMap>

class ComponentInfo {
public:
  ComponentInfo() : Coordinates(2){}

  ComponentInfo(QString ID_, ComponentType Type_, double rot_, double x,
                double y)
      : ID(ID_), Type(Type_), Rotation(rot_), Coordinates(2) {
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  QString ID;
  ComponentType Type;
  double Rotation;
  QString Net1, Net2; // ID of the nodes where the component is connected
  std::vector<double> Coordinates;
  int getNumberOfPorts() {
    switch (Type) {
    case CoupledLines:
    case Coupler:
      return 4;
    default:
      return 2;
    }
  }

  void setParams(QString ID_, ComponentType Type_, double Rotation_, double x,
                 double y // Coordinates
                 ) {
    ID = ID_;
    Type = Type_;
    Rotation = Rotation_;
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  QString getQucsCode() {
    QString code;
    switch (Type) {
    case GND:
      return QString(""); // Grounds are interpreted as a component in the
                          // sense they have a graphical representation,
      // but they have no meaning in terms of the Qucs netlist
    case Capacitor:
      code = "C";
      break;
    case Inductor:
      code = "L";
      break;
    case CoupledLines:
      code = "CTLIN";
      break;
    case OpenStub:
    case ShortStub:
    case TransmissionLine:
      code = "TLIN";
      break;
    case Term:
      code = "Pac";
      val["Num"] = QString(ID).remove("T");
      val["f"] = "1 GHz";
      break;
    case Resistor:
      code = "R";
      break;
    case Coupler:
      code = "Coupler";
      break;
    default:
      break;
    }
    code += QString(":%1").arg(ID);
    return code;
  }
  QString getQucsProperties() {
    QMap<QString, QString>::iterator it = val.begin();
    QString code;
    QString prop; // Temporal variable to translate the internal property
                  // names to Qucs property names
    while (it != val.end()) {
      prop = it.key();
      switch (Type) {
      case OpenStub:
      case ShortStub:
      case TransmissionLine:
      case CoupledLines:
        if (prop == "Length")
          prop = "L";
        if (prop == "Z0")
          prop = "Z";
        code += QString(" %1=\"%2\"").arg(prop).arg(it.value());
        break;

      case Term:
        if (prop == "Z0")
          prop = "Z";
        code += QString(" %1=\"%2\"").arg(prop).arg(it.value());
        break;

      case Coupler:
      default:
        code += QString(" %1=\"%2\"").arg(it.key()).arg(it.value());
      }
      it++;
    }
    code += "\n";
    return code;
  };
  QMap<QString, QString> val; // freq, L1.L, C1.C,...
  double getVal(QString);
};

class WireInfo {
public:
  WireInfo(){};
  WireInfo(QString O, int OP, QString D, int DP)
      : OriginID(O), PortOrigin(OP), DestinationID(D), PortDestination(DP){};
  void setParams(QString O, int OP, QString D, int DP) {
    OriginID = O, DestinationID = D;
    PortOrigin = OP, PortDestination = DP;
    WireColor = Qt::black;
  };
  void setParams(QString O, int OP, QString D, int DP, QColor Color) {
    OriginID = O, DestinationID = D;
    PortOrigin = OP, PortDestination = DP;
    WireColor = Color;
  };
  void setID(QString id) { ID = id; };
  QString getID() { return ID; };

  void setNet(QString net) { Net = net; };
  QString getNet() { return Net; };

  QString OriginID;
  int PortOrigin;
  QString DestinationID;
  int PortDestination;
  QColor WireColor;

private:
  QString ID;
  QString Net;
};

class NodeInfo {
public:
  NodeInfo() : Coordinates(2){};
  NodeInfo(QString ID_, double x, double y) : ID(ID_), Coordinates(2) {
    Coordinates[0] = x;
    Coordinates[1] = y;
  };
  void setParams(QString ID_, double x, double y) {
    ID = ID_;
    Coordinates[0] = x;
    Coordinates[1] = y;
  };
  QString ID;
  QString Net;
  std::vector<double> Coordinates;
};
#endif // INFOCLASSES_H
