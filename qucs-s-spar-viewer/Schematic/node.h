/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef NODE_H
#define NODE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include "Network.h"
#include "graphwidget.h"
#include "symbol.h"
#include "wire.h"

///
/// @class Node
/// @brief Wiring node for circuit connections
///
class Node : public Symbol {
public:
  ///
  /// @brief Construct node with graph widget
  /// @param graphWidget Parent graph widget
  ///
  Node(GraphWidget* graphWidget);

  ///
  /// @brief Construct node from NodeInfo structure
  /// @param graphWidget Parent graph widget
  /// @param NI Node information structure
  ///
  Node(GraphWidget* graphWidget, struct NodeInfo);

  ///
  /// @brief Class destructor
  ///
  ~Node() {}

  ///
  /// @brief Add wire connection to node
  /// @param Wire Wire to connect
  ///
  void addWire(Wire* Wire) {
    WireList << Wire;
    Wire->adjust();
  }

  ///
  /// @brief Get list of connected wires
  /// @return List of wire pointers
  ///
  QList<Wire*> Wires() const { return WireList; }

  enum { Type = UserType + 1 };
  int type() const { return Type; }

  ///
  /// @brief Get node bounding rectangle
  /// @return Bounding rectangle for painting
  ///
  QRectF boundingRect() const { return QRectF(-4, -4, 8, 8); }

  ///
  /// @brief Get node selection shape
  /// @return Path defining selectable area
  ///
  QPainterPath shape() const;

  ///
  /// @brief Paint node on scene
  /// @param painter QPainter instance
  /// @param option Style options
  /// @param widget Target widget
  ///
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget);

  ///
  /// @brief Get port location (always returns origin)
  /// @param port_number Port index (unused)
  /// @return Port position at origin
  ///
  QPoint getPortLocation(int /*port_number*/) { return QPoint(0, 0); }

  ///
  /// @brief Get node identifier
  /// @return Node ID string
  ///
  QString getID() { return ID; }

  ///
  /// @brief Show node
  ///
  void show() { visible = true; }

  ///
  /// @brief Hide node
  ///
  void hide() { visible = false; }

protected:
  ///
  /// @brief Handle item change events
  ///
  QVariant itemChange(GraphicsItemChange change, const QVariant& value);

  ///
  /// @brief Handle mouse press events
  ///
  void mousePressEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mousePressEvent(event);
  }

  ///
  /// @brief Handle mouse release events
  ///
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event){
    update();
    QGraphicsItem::mouseReleaseEvent(event);
  }


private:
  QList<Wire*> WireList;     ///< Connected wires
  QPointF newPos;            ///< New position during moves
  GraphWidget* graph;        ///< Parent graph widget
  QString ID;                ///< Node identifier
  bool visible;              ///< Visibility flag
};

#endif
