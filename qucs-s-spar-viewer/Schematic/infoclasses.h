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

#ifndef INFOCLASSES_H
#define INFOCLASSES_H
#include "../Schematic/structures.h"
#include <QColor>
#include <QFont>
#include <QMap>
#include <QPointF>

///
/// @brief Component information container
///
class ComponentInfo {
public:
  ///
  /// @brief Default constructor
  ///
  ComponentInfo() : Coordinates(2) {}

  ///
  /// @brief Construct with ID, type, rotation and coordinates
  /// @param ID_ Component identifier
  /// @param Type_ Component type
  /// @param rot_ Rotation angle in degrees
  /// @param x X coordinate
  /// @param y Y coordinate
  ///
  ComponentInfo(QString ID_, ComponentType Type_, double rot_, double x,
                double y)
      : ID(ID_), Type(Type_), Rotation(rot_), Coordinates(2) {
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  ///
  /// @brief Construct with ID, type, rotation and point
  /// @param ID_ Component identifier
  /// @param Type_ Component type
  /// @param rot_ Rotation angle in degrees
  /// @param P Position point
  ///
  ComponentInfo(QString ID_, ComponentType Type_, double rot_, QPoint P)
      : ID(ID_), Type(Type_), Rotation(rot_), Coordinates(2) {
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  ///
  /// @brief Construct with ID, type and point (zero rotation)
  /// @param ID_ Component identifier
  /// @param Type_ Component type
  /// @param P Position point
  ///
  ComponentInfo(QString ID_, ComponentType Type_, QPoint P)
      : ID(ID_), Type(Type_), Coordinates(2) {
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
      Rotation = 0;
  }

  ///
  /// @brief Class destructor
  ///
  ~ComponentInfo() {}

  QString ID;                          ///< Component identifier
  ComponentType Type;                  ///< Component type
  double Rotation;                     ///< Rotation angle in degrees
  QString Net1, Net2;                  ///< Connected node IDs
  std::vector<double> Coordinates;     ///< Position coordinates [x, y]
  QMap<QString, QString> val;          ///< Component parameters (e.g., freq, L1.L, C1.C)

  ///
  /// @brief Get number of ports for component type
  /// @return Number of ports (2 or 4)
  ///
  int getNumberOfPorts() const {
    switch (Type) {
    case MicrostripCoupledLines:
    case CoupledLines:
    case Coupler:
      return 4;
    default:
      return 2;
    }
  }

  ///
  /// @brief Set component parameters with coordinates
  /// @param ID_ Component identifier
  /// @param Type_ Component type
  /// @param Rotation_ Rotation angle
  /// @param x X coordinate
  /// @param y Y coordinate
  ///
  void setParams(QString ID_, ComponentType Type_, double Rotation_, double x,
                 double y // Coordinates
  ) {
    ID             = ID_;
    Type           = Type_;
    Rotation       = Rotation_;
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  ///
  /// @brief Set component parameters with point and rotation
  /// @param ID_ Component identifier
  /// @param Type_ Component type
  /// @param Rotation_ Rotation angle
  /// @param P Position point
  ///
  void setParams(QString ID_, ComponentType Type_, double Rotation_, QPoint P) // Coordinates
      {
      ID             = ID_;
      Type           = Type_;
      Rotation       = Rotation_;
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  ///
  /// @brief Set component parameters with point (zero rotation)
  /// @param ID_ Component identifier
  /// @param Type_ Component type
  /// @param P Position point
  ///
  void setParams(QString ID_, ComponentType Type_, QPoint P) // Coordinates
  {
      ID             = ID_;
      Type           = Type_;
      Rotation       = 0; // Default rotation = 0
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  ///
  /// @brief Parse parameter value with SI suffix
  /// @param Property Parameter name
  /// @return Numeric value with scale applied
  ///
  double getVal(const QString &);
};

///
/// @brief Wire connection information
///
class WireInfo {
public:

  ///
  /// @brief Class constructor
  ///
  WireInfo() {}

  ///
  /// @brief Class destructor
  ///
  ~WireInfo() {}

  ///
  /// @brief Construct wire with endpoints
  /// @param O Origin component/node ID
  /// @param OP Origin port number
  /// @param D Destination component/node ID
  /// @param DP Destination port number
  ///
  WireInfo(QString O, int OP, QString D, int DP)
      : OriginID(O), PortOrigin(OP), DestinationID(D), PortDestination(DP) {}

  ///
  /// @brief Set wire parameters
  /// @param O Origin ID
  /// @param OP Origin port
  /// @param D Destination ID
  /// @param DP Destination port
  ///
  void setParams(QString O, int OP, QString D, int DP) {
    OriginID = O, DestinationID = D;
    PortOrigin = OP, PortDestination = DP;
    WireColor = Qt::black;
  }

  ///
  /// @brief Set wire parameters with color
  /// @param O Origin ID
  /// @param OP Origin port
  /// @param D Destination ID
  /// @param DP Destination port
  /// @param Color Wire color
  ///
  void setParams(QString O, int OP, QString D, int DP, QColor Color) {
    OriginID = O, DestinationID = D;
    PortOrigin = OP, PortDestination = DP;
    WireColor = Color;
  }

  ///
  /// @brief Set wire identifier
  /// @param id Wire ID
  ///
  void setID(QString id) { ID = id; }

  ///
  /// @brief Get wire identifier
  /// @return Wire ID
  ///
  QString getID() { return ID; }

  ///
  /// @brief Set net name
  /// @param net Net identifier
  ///
  void setNet(QString net) { Net = net; }

  ///
  /// @brief Get net name
  /// @return Net identifier
  ///
  QString getNet() { return Net; }

  QString OriginID;          ///< Origin component/node ID
  int PortOrigin;            ///< Origin port number
  QString DestinationID;     ///< Destination component/node ID
  int PortDestination;       ///< Destination port number
  QColor WireColor;          ///< Wire display color
  QString ID;                ///< Wire identifier

private:
  QString Net;               ///< Net identifier
};

///
/// @brief Wiring node information class
///
class NodeInfo {
public:
  ///
  /// @brief Default constructor
  ///
  NodeInfo() : Coordinates(2) {}

  ///
  /// @brief Class destructor
  ///
  ~NodeInfo() {}

  ///
  /// @brief Construct with ID and coordinates
  /// @param ID_ Node identifier
  /// @param x X coordinate
  /// @param y Y coordinate
  ///
  NodeInfo(QString ID_, double x, double y) : ID(ID_), Coordinates(2) {
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  ///
  /// @brief Construct with ID and point
  /// @param ID_ Node identifier
  /// @param P Position point
  ///
  NodeInfo(QString ID_, QPoint P) : ID(ID_), Coordinates(2) {
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  ///
  /// @brief Set node parameters with coordinates
  /// @param ID_ Node identifier
  /// @param x X coordinate
  /// @param y Y coordinate
  ///
  void setParams(QString ID_, double x, double y) {
    ID             = ID_;
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  ///
  /// @brief Set node parameters with point
  /// @param ID_ Node identifier
  /// @param P Position point
  ///
  void setParams(QString ID_, QPoint P) {
      ID             = ID_;
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  QString ID;                        ///< Node identifier
  QString Net;                       ///< Net identifier
  std::vector<double> Coordinates;   ///< Position coordinates [x, y]
  bool visible = true;               ///< Node visibility flag
};

/// @brief Text annotation information
class TextInfo {
public:
  ///
  /// @brief Default constructor
  ///
  TextInfo() : position(0, 0) {}

  ///
  /// @brief Construct with full parameters
  /// @param ID_ Text identifier
  /// @param text_ Display text
  /// @param font_ Text font
  /// @param color_ Text color
  /// @param position_ Text position
  ///
  TextInfo(QString ID_, QString text_, QFont font_ = QFont(),
           QColor color_ = Qt::black, QPointF position_ = QPointF())
      : ID(ID_), text(text_), font(font_), color(color_), position(position_) {}

  ///
  /// @brief Set text parameters
  /// @param ID_ Text identifier
  /// @param text_ Display text
  /// @param font_ Text font
  /// @param color_ Text color
  /// @param position_ Text position
  ///
  void setParams(QString ID_, QString text_, QFont font_ = QFont(),
                 QColor color_ = Qt::black, QPointF position_ = QPointF()) {
    ID       = ID_;
    text     = text_;
    font     = font_;
    color    = color_;
    position = position_;
  }

  QString ID;          ///< Text identifier
  QString text;        ///< Display text content
  QFont font;          ///< Text font
  QColor color;        ///< Text color
  QPointF position;    ///< Text position
};

#endif // INFOCLASSES_H
