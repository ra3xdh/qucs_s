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

#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsItem>
#include <QPainter>
#include <math.h>

#include "symbol.h"

class Symbol;

//! [0]
class Wire : public QGraphicsItem {
public:
  Wire(Symbol* sourceNode, int port_num_source, Symbol* destNode,
       int port_num_dest);
  Wire();
  ~Wire();

  void setSource(Symbol*, int);
  void setDestination(Symbol*, int);
  void setColor(QColor);
  void paintWire();

  Symbol* sourceNode() const;
  Symbol* destNode() const;

  void adjust();

  enum { Type = UserType + 2 };
  int type() const { return Type; }
  QPoint getPortLocation(int);

protected:
  QRectF boundingRect() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget);

private:
  Symbol *source, *dest;
  QColor WireColor;
  QPointF sourcePoint;
  QPointF destPoint;
  qreal arrowSize;
  int port_num_source, port_num_dest;
};
//! [0]

#endif
