/// @file component.cpp
/// @brief Graphical component representation in schematic (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "component.h"

Component::Component(GraphWidget *graphWidget, ComponentType comp, double Rot_,
                     QMap<QString, QString> val, QString ID_)
    : graph(graphWidget) {
  ID = ID_;
  CompType = comp;
  Rotation = Rot_;
  Value = val;
  setFlag(ItemIsMovable, false);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}

Component::Component(GraphWidget *graphWidget, ComponentInfo CI)
    : graph(graphWidget) {
  ID = CI.ID;
  CompType = CI.Type;
  Rotation = CI.Rotation;
  Value = CI.val;
  setPos(CI.Coordinates.at(0),
         CI.Coordinates.at(1)); // Coordinates in the schematic window
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}

void Component::addWire(Wire *Wire) {
  WireList << Wire;
  Wire->adjust();
}

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

void Component::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem * /*option*/, QWidget *) {
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
                               const QVariant &value) {
  switch (change) {
  case ItemPositionHasChanged:
    foreach (Wire *Wire, WireList) {
      Wire->adjust();
    }
    graph->itemMoved();
    break;
  default:
    break;
  };

  return QGraphicsItem::itemChange(change, value);
}

void Component::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  update();
  struct ComponentInfo CI;
  CI.ID = this->ID;
  CI.Rotation = this->Rotation;
  CI.Type = this->CompType;
  CI.val = this->Value;
  emit DoubleClicked(CI);
  QGraphicsItem::mouseDoubleClickEvent(event);
}

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

void Component::RotatePoint(QPoint &P) {
  double r = (M_PI / 180) * Rotation;
  double x_rotated = P.x() * cos(r) - P.y() * sin(r);
  double y_rotated = P.x() * sin(r) + P.y() * cos(r);
  P.setX(x_rotated);
  P.setY(y_rotated);
}

void Component::RotatePoint(QPoint &P, double angle) {
  double r = (M_PI / 180) * angle;
  double x_rotated = P.x() * cos(r) - P.y() * sin(r);
  double y_rotated = P.x() * sin(r) + P.y() * cos(r);
  P.setX(x_rotated);
  P.setY(y_rotated);
}

double ComponentInfo::getVal(const QString &property) {
  QString val_ = this->val[property];
  val_.remove(' '); // strip blanks

  double scale = 1.0;
  int index = -1; // position of the first letter
  QString suffix;

  // Find the first alphabetic character (the suffix)
  for (int i = 0; i < val_.length(); ++i) {
    if (val_.at(i).isLetter()) {
      index = i;
      suffix = val_.mid(i, 1);
      break;
    }
  }

  // Determine the multiplier
  if (suffix == "f") {
    scale = 1e-15;
  } else if (suffix == "p") {
    scale = 1e-12;
  } else if (suffix == "n") {
    scale = 1e-9;
  } else if (suffix == "u") {
    scale = 1e-6;
  } else if (suffix == "m") {
    scale = 1e-3;
  } else if (suffix == "K") {
    scale = 1e3;
  } else if (suffix == "M") {
    scale = 1e6;
  } else if (suffix == "G") {
    scale = 1e9;
  } else if (suffix == "T") {
    scale = 1e12;
  }
  // If none of the above matched, `scale` stays at 1.0

  // Strip the suffix and convert the numeric part
  QString numStr = (index > 0) ? val_.left(index) : val_;
  bool ok = false;
  double value = numStr.toDouble(&ok);
  if (!ok)
    return -1; // conversion failed

  return value * scale;
}
