/// @file graphwidget.cpp
/// @brief Schematic editor graphics widget (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QMap>
#include <QtGui>
#include <deque>
#include <math.h>

#include "SchematicContent.h"
#include "component.h"
#include "node.h"
#include "structures.h"
#include "wire.h"

class Node;
class Wire;

/// @class Component
/// @brief Schematic editor graphics widget
class GraphWidget : public QGraphicsView {
  Q_OBJECT

public:
  /// @brief Construct graph widget
  /// @param parent Parent widget
  GraphWidget(QWidget* parent = 0);

  /// @brief Notify that an item has moved
  void itemMoved() {
    if (!timerId) {
      timerId = startTimer(1000 / 25);
    }
  }

  /// @brief Set components (position, type, values) in the schematic
  /// @param cmps List of component information
  void setComponents(QList<ComponentInfo>);


  /// @brief Modify existing component parameters
  /// @param CI Component information with updated values
  void ModifyComponent(ComponentInfo);

  /// @brief Set wires (position and connection properties) in the schematic
  /// @param wrs List of wire information
  void setWires(const QList<WireInfo>&);

  /// @brief Set nodes in the schematic
  /// @param nds List of node information
  void setNodes(QList<NodeInfo>);

  /// @brief Set complete schematic content
  /// @param SchContent Schematic content structure
  void setSchematic(SchematicContent);

  /// @brief Set text items in the schematic
  /// @param texts List of text items
  void setTexts(QList<QGraphicsTextItem*>);

  /// @brief Clear all items from scene
  void clear();

public slots:
  /// @brief Randomize component positions
  void shuffle();

  /// @brief Zoom in view
  void zoomIn() { scaleView(qreal(1.2)); }

  /// @brief Zoom out view
  void zoomOut() { scaleView(1 / qreal(1.2)); }

  /// @brief Handle component selection events
  /// @param CI Selected component information
  void ComponentSelectionHandler(struct ComponentInfo CI) {
    emit this->SendComponentSelectionToMainFunction(CI);
  }

protected:
  /// @brief Handle key press events
  void keyPressEvent(QKeyEvent* /*event*/){}

  /// @brief Handle timer events
  void timerEvent(QTimerEvent* event);

  /// @brief Handle mouse wheel events
  void wheelEvent(QWheelEvent* event) {
    scaleView(pow((double)2, -event->angleDelta().y() / 240.0));
  }

  /// @brief Scale view by factor
  /// @param scaleFactor Scaling factor
  void scaleView(qreal scaleFactor);

private:
  int timerId;                           ///< Timer ID for animations
  std::deque<Component*> Components;     ///< Component list
  std::deque<Wire*> Wires;               ///< Wire list
  std::deque<Node*> Nodes;               ///< Node list
  std::deque<QGraphicsTextItem*> Texts;  ///< Text item list

signals:
  /// @brief Send component selection to main function
  /// @param CI Selected component information
  void SendComponentSelectionToMainFunction(ComponentInfo);
};

#endif
