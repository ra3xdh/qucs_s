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

class ComponentInfo {
public:
  ComponentInfo() : Coordinates(2) {}

  ComponentInfo(QString ID_, ComponentType Type_, double rot_, double x,
                double y)
      : ID(ID_), Type(Type_), Rotation(rot_), Coordinates(2) {
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  ComponentInfo(QString ID_, ComponentType Type_, double rot_, QPoint P)
      : ID(ID_), Type(Type_), Rotation(rot_), Coordinates(2) {
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  ComponentInfo(QString ID_, ComponentType Type_, QPoint P)
      : ID(ID_), Type(Type_), Coordinates(2) {
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
      Rotation = 0;
  }

  ~ComponentInfo() {}

  QString ID;
  ComponentType Type;
  double Rotation;
  QString Net1, Net2; // ID of the nodes where the component is connected
  std::vector<double> Coordinates;
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

  void setParams(QString ID_, ComponentType Type_, double Rotation_, double x,
                 double y // Coordinates
  ) {
    ID             = ID_;
    Type           = Type_;
    Rotation       = Rotation_;
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  void setParams(QString ID_, ComponentType Type_, double Rotation_, QPoint P) // Coordinates
      {
      ID             = ID_;
      Type           = Type_;
      Rotation       = Rotation_;
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  void setParams(QString ID_, ComponentType Type_, QPoint P) // Coordinates
  {
      ID             = ID_;
      Type           = Type_;
      Rotation       = 0;
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  QMap<QString, QString> val; // freq, L1.L, C1.C,...
  double getVal(QString);
};

class WireInfo {
public:
  WireInfo() {}
  ~WireInfo() {}
  WireInfo(QString O, int OP, QString D, int DP)
      : OriginID(O), PortOrigin(OP), DestinationID(D), PortDestination(DP) {}
  void setParams(QString O, int OP, QString D, int DP) {
    OriginID = O, DestinationID = D;
    PortOrigin = OP, PortDestination = DP;
    WireColor = Qt::black;
  }
  void setParams(QString O, int OP, QString D, int DP, QColor Color) {
    OriginID = O, DestinationID = D;
    PortOrigin = OP, PortDestination = DP;
    WireColor = Color;
  }
  void setID(QString id) { ID = id; }
  QString getID() { return ID; }

  void setNet(QString net) { Net = net; }
  QString getNet() { return Net; }

  QString OriginID;
  int PortOrigin;
  QString DestinationID;
  int PortDestination;
  QColor WireColor;

  QString ID;

private:
  QString Net;
};

class NodeInfo {
public:
  NodeInfo() : Coordinates(2) {}
  ~NodeInfo() {}
  NodeInfo(QString ID_, double x, double y) : ID(ID_), Coordinates(2) {
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  NodeInfo(QString ID_, QPoint P) : ID(ID_), Coordinates(2) {
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }


  void setParams(QString ID_, double x, double y) {
    ID             = ID_;
    Coordinates[0] = x;
    Coordinates[1] = y;
  }

  void setParams(QString ID_, QPoint P) {
      ID             = ID_;
      Coordinates[0] = P.x();
      Coordinates[1] = P.y();
  }

  QString ID;
  QString Net;
  std::vector<double> Coordinates;
  bool visible = true;
};

class TextInfo {
public:
  TextInfo() : position(0, 0) {}

  TextInfo(QString ID_, QString text_, QFont font_ = QFont(),
           QColor color_ = Qt::black, QPointF position_ = QPointF())
      : ID(ID_), text(text_), font(font_), color(color_), position(position_) {}

  void setParams(QString ID_, QString text_, QFont font_ = QFont(),
                 QColor color_ = Qt::black, QPointF position_ = QPointF()) {
    ID       = ID_;
    text     = text_;
    font     = font_;
    color    = color_;
    position = position_;
  }

  QString ID;
  QString text;
  QFont font;
  QColor color;
  QPointF position;
};

#endif // INFOCLASSES_H
