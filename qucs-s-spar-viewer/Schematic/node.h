/// @file node.h
/// @brief Wiring node for circuit connections (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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

/// @class Node
/// @brief Wiring node for circuit connections
class Node : public Symbol {
public:
  /// @brief Construct node with graph widget
  /// @param graphWidget Parent graph widget
  Node(GraphWidget* graphWidget);

  /// @brief Construct node from NodeInfo structure
  /// @param graphWidget Parent graph widget
  /// @param NI Node information structure
  Node(GraphWidget* graphWidget, struct NodeInfo);

  /// @brief Class destructor
  ~Node() {}

  /// @brief Add wire connection to node
  /// @param Wire Wire to connect
  void addWire(Wire* Wire) {
    WireList << Wire;
    Wire->adjust();
  }

  /// @brief Get list of connected wires
  /// @return List of wire pointers
  QList<Wire*> Wires() const { return WireList; }

  enum { Type = UserType + 1 };
  int type() const { return Type; }

  /// @brief Get node bounding rectangle
  /// @return Bounding rectangle for painting

  QRectF boundingRect() const { return QRectF(-4, -4, 8, 8); }

  /// @brief Get node selection shape
  /// @return Path defining selectable area
  QPainterPath shape() const;

  /// @brief Paint node on scene
  /// @param painter QPainter instance
  /// @param option Style options
  /// @param widget Target widget
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget);

  /// @brief Get port location (always returns origin)
  /// @param port_number Port index (unused)
  /// @return Port position at origin
  QPoint getPortLocation(int /*port_number*/) { return QPoint(0, 0); }

  /// @brief Get node identifier
  /// @return Node ID string
  QString getID() { return ID; }

  /// @brief Show node
  void show() { visible = true; }

  /// @brief Hide node
  void hide() { visible = false; }

protected:
  /// @brief Handle item change events
  QVariant itemChange(GraphicsItemChange change, const QVariant& value);

  /// @brief Handle mouse press events
  void mousePressEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mousePressEvent(event);
  }

  /// @brief Handle mouse release events
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
