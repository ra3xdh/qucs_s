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

#include "node.h"

///
/// @brief Construct node with graph widget
/// @param graphWidget Parent graph widget
///
Node::Node(GraphWidget *graphWidget) : graph(graphWidget) {
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
  visible = true; // By default, the note should be visible. If the used wants
                  // the node not to be visible, they can hide it where needed.
}

///
/// @brief Construct node from NodeInfo structure
/// @param graphWidget Parent graph widget
/// @param NI Node information structure
///
Node::Node(GraphWidget *graphWidget, NodeInfo NI) : graph(graphWidget) {
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
  this->ID = NI.ID;
  setPos(NI.Coordinates.at(0), NI.Coordinates.at(1));
  visible = NI.visible;
}

///
/// @brief Get node selection shape
/// @return Path defining selectable area
///
QPainterPath Node::shape() const {
  QPainterPath path;
  path.addEllipse(-4, -4, 8, 8);
  return path;
}

///
/// @brief Paint node on scene
/// @param painter QPainter instance
/// @param option Style options
/// @param widget Target widget
///
void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/,
                 QWidget *) {

  if (!visible) {
    return;
  }
  QBrush brush = painter->brush();
  brush.setStyle(Qt::SolidPattern);
  painter->setBrush(brush);
  painter->drawEllipse(QPointF(0, 0), 1.5, 1.5);
  /* painter->setPen(QPen(Qt::red, 1));
   painter->drawPath(this->shape());*/
}

///
/// @brief Handle item change events
///
QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
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
