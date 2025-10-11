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

#include "component.h"

Component::Component(GraphWidget* graphWidget, ComponentType comp, double Rot_,
                     QMap<QString, QString> val, QString ID_)
    : graph(graphWidget) {
  ID       = ID_;
  CompType = comp;
  Rotation = Rot_;
  Value    = val;
  setFlag(ItemIsMovable, false);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}

Component::Component(GraphWidget* graphWidget, ComponentInfo CI)
    : graph(graphWidget) {
  ID       = CI.ID;
  CompType = CI.Type;
  Rotation = CI.Rotation;
  Value    = CI.val;
  setPos(CI.Coordinates.at(0),
         CI.Coordinates.at(1)); // Coordinates in the schematic window
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}
Component::~Component() {}

void Component::addWire(Wire* Wire) {
  WireList << Wire;
  Wire->adjust();
}

QList<Wire*> Component::Wires() const {
  return WireList;
}

// This function returns the bounding box of the component. That is the region
// where the component can be painted
QRectF Component::boundingRect() const {
  QRect R;
  switch (CompType) {
  case OpenStub:
  case ShortStub:
  case TransmissionLine:
  case MicrostripVia:
    R = QRect(-40, -40, 80, 80);
    break;
  case MicrostripOpen:
    R = QRect(-40, -40, 100, 80);
    break;
  case MicrostripLine:
    R = QRect(-40, -40, 100, 100);
    break;
  case MicrostripCoupledLines:
  case CoupledLines:
  case Coupler:
    R = QRect(-60, -60, 120, 120);
    break;
  case ComplexImpedance:
  case Resistor:
  case Capacitor:
  case Inductor:
    R = QRect(-40, -40, 120, 120);
    break;
  case GND:
    R = QRect(-14, -14, 30, 30);
    break;
  case Term:
    R = QRect(-50, -50, 100, 100);
    break;
  case SPAR_Block:
    R = QRect(-25, -25, 50, 50);
    break;

  case MicrostripStep:
    R = QRect(-40, -60, 80, 120);
    break;

  default:
    break;
  }
  return R;
}

// This function gives the area where the component can be selected
QPainterPath Component::shape() const {
  QPainterPath path;
  switch (CompType) {
  case Capacitor:
  case OpenStub:
  case ShortStub:
  case TransmissionLine:
  case MicrostripLine:
  case MicrostripOpen:
  case MicrostripVia:
  case MicrostripCoupledLines:
  case Resistor:
  case ComplexImpedance:
  case CoupledLines:
  case SPAR_Block:
  case Coupler:
    path.addRect(-30, -30, 60, 60);
    break;
  case GND:
    path.addRect(-2 * 7, -2 * 7, 2 * 15, 2 * 10);
    break;
  case Term:
    path.addRect(-15, -15, 30, 30);
    break;
  default:
    break;
  }

  return path;
}

void Component::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem* /*option*/, QWidget*) {
  painter->setPen(QPen(Qt::darkBlue, 1));
  painter->setFont(QFont("Arial", 6, QFont::Bold));
  switch (CompType) {
  case Capacitor:
    paintCapacitor(painter);
    break;
  case Inductor:
    paintInductor(painter);
    break;
  case TransmissionLine:
    paintTransmissionLine(painter);
    break;
  case MicrostripLine:
    paintMicrostripLine(painter);
    break;
  case MicrostripStep:
    paintMicrostripStep(painter);
    break;
  case MicrostripOpen:
    paintMicrostripOpen(painter);
    break;
  case MicrostripCoupledLines:
    paintMicrostripCoupledLines(painter);
    break;
  case MicrostripVia:
    paintMicrostripVia(painter);
    break;
  case ComplexImpedance:
    paintComplexImpedance(painter);
    break;
  case OpenStub:
    paintOpenStub(painter);
    break;
  case ShortStub:
    paintShortStub(painter);
    break;
  case Resistor:
    paintResistor(painter);
    break;
  case GND:
    paintGND(painter);
    break;
  case Term:
    paintTerm(painter);
    break;
  case CoupledLines:
    paintCoupledLines(painter);
    break;
  case Coupler:
    paintCoupler(painter);
    break;
  case SPAR_Block:
    paintSPAR(painter);
    break;
  default:
    break;
  }

  bool debug = false;
  if (debug) {
    // Debug code: Shows the bounding box of the component. This is the
    painter->setPen(QPen(Qt::red, 1));
    painter->drawPoint(QPoint(0, 0));

    // region where the selection works painter->setPen(QPen(Qt::red, 1));
    painter->drawPath(
        this->shape()); // Component box-> This is the area where the
    // component can be selected painter->setPen(QPen(Qt::green, 1));
    painter->drawRect(
        this->boundingRect()); // Component bounding box->This is the
                               // area where the component can be painted
  }
}

QVariant Component::itemChange(GraphicsItemChange change,
                               const QVariant& value) {
  switch (change) {
  case ItemPositionHasChanged:
    foreach (Wire* Wire, WireList) {
      Wire->adjust();
    }
    graph->itemMoved();
    break;
  default:
    break;
  };

  return QGraphicsItem::itemChange(change, value);
}

void Component::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  update();
  QGraphicsItem::mousePressEvent(event);
}

void Component::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  update();
  QGraphicsItem::mouseReleaseEvent(event);
}

void Component::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
  update();
  struct ComponentInfo CI;
  CI.ID       = this->ID;
  CI.Rotation = this->Rotation;
  CI.Type     = this->CompType;
  CI.val      = this->Value;
  emit DoubleClicked(CI);
  QGraphicsItem::mouseDoubleClickEvent(event);
}

// This function returns the location of the port specified by port_number. It
// is used by a wire object so as to know how to connect symbols
QPoint Component::getPortLocation(int port_number) {
  QPoint P;
  switch (CompType) {
  case OpenStub:
  case ShortStub:
  case TransmissionLine:
  case MicrostripLine:
  case Resistor:
  case Inductor:
  case Capacitor:
  case ComplexImpedance:
    switch (port_number) {
    case 1:
      P = QPoint(0, -25);
      break;
    case 0:
    default:
      P = QPoint(0, 25);
    }
    break;
  case GND:
    P = QPoint(0, -11);
    break;
  case Term:
    P = QPoint(10, 0);
    break;
  case Coupler:
    switch (port_number) {
    case 0:
    default:
      P = QPoint(-10, 25);
      break;
    case 1:
      P = QPoint(-10, -25);
      break;
    case 2:
      P = QPoint(10, -25);
      break;
    case 3:
      P = QPoint(10, 25);
      break;
    }
    break;

  case MicrostripCoupledLines:
  case CoupledLines:
    switch (port_number) {
    case 0:
    default:
      P = QPoint(-10, 25);
      break;
    case 1:
      P = QPoint(10, 25);
      break;
    case 2:
      P = QPoint(10, -25);
      break;
    case 3:
      P = QPoint(-10, -25);
      break;
    }
    break;

  case SPAR_Block:
    switch (port_number) {
    case 0:
    default:
      P = QPoint(-20, 0);
      break;
    case 1:
      P = QPoint(20, 0);
      break;
    }
    break;

  case MicrostripStep:
    switch (port_number) {
    case 0:
    default:
      P = QPoint(-20, 0);
      break;
    case 1:
      P = QPoint(20, 0);
      break;
    }
    break;

  case MicrostripVia:
    P = QPoint(0, -10);
    break;
  case MicrostripOpen:
    P = QPoint(0, -25);
    break;

  default:
    break;
  }

  RotatePoint(P);
  return P;
}

// This function rotates the port position P about the object centroid (0,0)
// according to the component rotation
void Component::RotatePoint(QPoint& P) {
  double r         = (M_PI / 180) * Rotation;
  double x_rotated = P.x() * cos(r) - P.y() * sin(r);
  double y_rotated = P.x() * sin(r) + P.y() * cos(r);
  P.setX(x_rotated);
  P.setY(y_rotated);
}

// This function rotates the port position P about the object centroid (0,0)
// according to the angle in the function arguments
void Component::RotatePoint(QPoint& P, double angle) {
  double r         = (M_PI / 180) * angle;
  double x_rotated = P.x() * cos(r) - P.y() * sin(r);
  double y_rotated = P.x() * sin(r) + P.y() * cos(r);
  P.setX(x_rotated);
  P.setY(y_rotated);
}

QString Component::getID() {
  return ID;
}

void Component::setRotation(double R) {
  Rotation = R;
}

void Component::setParameters(QMap<QString, QString> val) {
  Value = val;
}

QMap<QString, QString> Component::getParameters() {
  return Value;
}

void Component::setComponentType(ComponentType CT) {
  CompType = CT;
}

ComponentType Component::getComponentType() {
  return CompType;
}

// Given the property name, this function returns its value in coplex format
double ComponentInfo::getVal(QString Property) {
  QString val_ = this->val[Property];
  QString suffix;
  val_.remove(" "); // Remove blank spaces (if exists)
  double scale = 1;
  int index    = 1;
  // Find the suffix
  // Examine each character until finding the first letter, then determine the
  // scale factor
  for (int i = 0; i < val_.length(); i++) {
    if (val_.at(i).isLetter()) {
      index  = i;
      suffix = val_.at(i);
      break;
    }
  }

  if (suffix == "f") {
    scale = 1e-15;
  } else {
    if (suffix == "p") {
      scale = 1e-12;
    } else {
      if (suffix == "n") {
        scale = 1e-9;
      } else {
        if (suffix == "u") {
          scale = 1e-6;
        } else {
          if (suffix == "m") {
            scale = 1e-3;
          } else {
            if (suffix == "K") {
              scale = 1e3;
            } else {
              if (suffix == "M") {
                scale = 1e6;
              } else {
                if (suffix == "G") {
                  scale = 1e9;
                } else {
                  if (suffix == "T") {
                    scale = 1e12;
                  }
                }
              }
            }
          }
        }
      }
    }

    // Remove the suffix from the string and convert the property to numerical
    // format
    QString val = val_.left(index);

    // Now, find out if the number is complex or real
    /*if (index = val.indexOf("j"))
    {//Need to separate the real from the imaginary part
        double sign = 1;
        if (val[index-1] == '-') sign = -1;
        double realpart = val.left(index-1).toDouble();//Notice  we have to take
    into account the sign double imagpart = val.right(index).toDouble(); return
    std::complex<double>(realpart, sign*imagpart);
    }
    else
    {*/
    return val.toDouble() * scale;
    //}
  }
  return -1;
}
