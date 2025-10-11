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

#include "wire.h"

static const double Pi = 3.14159265358979323846264338327950288419717;

Wire::Wire(Symbol* sourceNode, int ps, Symbol* destNode, int pd)
    : arrowSize(10) {
  setAcceptedMouseButtons(Qt::NoButton);
  source          = sourceNode;
  dest            = destNode;
  port_num_source = ps;
  port_num_dest   = pd;
  source->addWire(this);
  dest->addWire(this);
  WireColor = Qt::black;
  adjust();
}

Wire::Wire() : arrowSize(10) {
  setAcceptedMouseButtons(Qt::NoButton);
}

Wire::~Wire() {}

void Wire::setSource(Symbol* sourceNode, int port) {
  source          = sourceNode;
  port_num_source = port;
}

void Wire::setDestination(Symbol* destNode, int port) {
  dest          = destNode;
  port_num_dest = port;
}

void Wire::setColor(QColor Color) {
  WireColor = Color;
}

void Wire::paintWire() {
  source->addWire(this);
  dest->addWire(this);
  adjust();
}

Symbol* Wire::sourceNode() const {
  return source;
}

Symbol* Wire::destNode() const {
  return dest;
}

void Wire::adjust() {
  if (!source || !dest) {
    return;
  }

  QPoint PortSource = source->getPortLocation(port_num_source);
  QPoint PortDest   = dest->getPortLocation(port_num_dest);
  QLineF line(mapFromItem(source, PortSource.x(), PortSource.y()),
              mapFromItem(dest, PortDest.x(), PortDest.y()));

  prepareGeometryChange();

  sourcePoint = line.p1();
  destPoint   = line.p2();
}

QRectF Wire::boundingRect() const {
  return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                    destPoint.y() - sourcePoint.y()))
      .normalized();
}

void Wire::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
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
