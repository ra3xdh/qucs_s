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

///
/// @class Component
/// @brief Schematic editor graphics widget
///
class GraphWidget : public QGraphicsView {
  Q_OBJECT

public:
  ///
  /// @brief Construct graph widget
  /// @param parent Parent widget
  ///
  GraphWidget(QWidget* parent = 0);

  ///
  /// @brief Notify that an item has moved
  ///
  void itemMoved() {
    if (!timerId) {
      timerId = startTimer(1000 / 25);
    }
  }

  ///
  /// @brief Set components (position, type, values) in the schematic
  /// @param cmps List of component information
  ///
  void setComponents(QList<ComponentInfo>);

  ///
  /// @brief Modify existing component parameters
  /// @param CI Component information with updated values
  ///
  void ModifyComponent(ComponentInfo);

  ///
  /// @brief Set wires (position and connection properties) in the schematic
  /// @param wrs List of wire information
  ///
  void setWires(const QList<WireInfo>&);

  ///
  /// @brief Set nodes in the schematic
  /// @param nds List of node information
  ///
  void setNodes(QList<NodeInfo>);

  ///
  /// @brief Set complete schematic content
  /// @param SchContent Schematic content structure
  ///
  void setSchematic(SchematicContent);

  ///
  /// @brief Set text items in the schematic
  /// @param texts List of text items
  ///
  void setTexts(QList<QGraphicsTextItem*>);

  ///
  /// @brief Clear all items from scene
  ///
  void clear();

public slots:
  ///
  /// @brief Randomize component positions
  ///
  void shuffle();

  ///
  /// @brief Zoom in view
  ///
  void zoomIn() { scaleView(qreal(1.2)); }

  ///
  /// @brief Zoom out view
  ///
  void zoomOut() { scaleView(1 / qreal(1.2)); }

  ///
  /// @brief Handle component selection events
  /// @param CI Selected component information
  ///
  void ComponentSelectionHandler(struct ComponentInfo CI) {
    emit this->SendComponentSelectionToMainFunction(CI);
  }

protected:
  ///
  /// @brief Handle key press events
  ///
  void keyPressEvent(QKeyEvent* /*event*/){}

  ///
  /// @brief Handle timer events
  ///
  void timerEvent(QTimerEvent* event);

  ///
  /// @brief Handle mouse wheel events
  ///
  void wheelEvent(QWheelEvent* event) {
    scaleView(pow((double)2, -event->angleDelta().y() / 240.0));
  }

  ///
  /// @brief Scale view by factor
  /// @param scaleFactor Scaling factor
  ///
  void scaleView(qreal scaleFactor);

private:
  int timerId;                           ///< Timer ID for animations
  std::deque<Component*> Components;     ///< Component list
  std::deque<Wire*> Wires;               ///< Wire list
  std::deque<Node*> Nodes;               ///< Node list
  std::deque<QGraphicsTextItem*> Texts;  ///< Text item list

signals:
  ///
  /// @brief Send component selection to main function
  /// @param CI Selected component information
  ///
  void SendComponentSelectionToMainFunction(ComponentInfo);
};

#endif
