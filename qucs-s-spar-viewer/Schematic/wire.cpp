/// @file wire.cpp
/// @brief Wire connection between symbols (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "wire.h"

static const double Pi = 3.14159265358979323846264338327950288419717;

Wire::Wire(Symbol *sourceNode, int ps, Symbol *destNode, int pd)
    : arrowSize(10) {
  setAcceptedMouseButtons(Qt::NoButton);
  source = sourceNode;
  dest = destNode;
  port_num_source = ps;
  port_num_dest = pd;
  source->addWire(this);
  dest->addWire(this);
  WireColor = Qt::black;
  adjust();
}

void Wire::adjust() {
  if (!source || !dest) {
    return;
  }

  QPoint PortSource = source->getPortLocation(port_num_source);
  QPoint PortDest = dest->getPortLocation(port_num_dest);
  QLineF line(mapFromItem(source, PortSource.x(), PortSource.y()),
              mapFromItem(dest, PortDest.x(), PortDest.y()));

  prepareGeometryChange();

  sourcePoint = line.p1();
  destPoint = line.p2();
}

QRectF Wire::boundingRect() const {
  return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                    destPoint.y() - sourcePoint.y()))
      .normalized();
}

void Wire::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                 QWidget *) {
  if (!source || !dest) {
    return;
  }
  /*  painter->setPen(QPen(Qt::red, 1));
    painter->drawRect(this->boundingRect());*/

  QLineF lineV(sourcePoint, QPoint(sourcePoint.x(), destPoint.y()));
  QLineF lineH(QPoint(sourcePoint.x(), destPoint.y()), destPoint);

  // Draw the line itself
  painter->setPen(
      QPen(WireColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  if (sourcePoint.x() != destPoint.x()) {
    painter->drawLine(lineH);
  }
  if (sourcePoint.y() != destPoint.y()) {
    painter->drawLine(lineV);
  }
}
