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

#include "graphwidget.h"

///
/// @brief Construct graph widget
/// @param parent Parent widget
///
GraphWidget::GraphWidget(QWidget *parent) : QGraphicsView(parent), timerId(0) {
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  setScene(scene);
  setCacheMode(CacheBackground);
  setViewportUpdateMode(BoundingRectViewportUpdate);
  setRenderHint(QPainter::Antialiasing);
  setTransformationAnchor(AnchorUnderMouse);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setWindowTitle(tr("Schematic"));
}

///
/// @brief Handle timer events
///
void GraphWidget::timerEvent(QTimerEvent *event) {
  Q_UNUSED(event);

  QList<Symbol *> nodes;
  foreach (QGraphicsItem *item, scene()->items()) {
    if (Symbol *node = qgraphicsitem_cast<Symbol *>(item)) {
      nodes << node;
    }
  }
}

void GraphWidget::scaleView(qreal scaleFactor) {
  qreal factor = transform()
                     .scale(scaleFactor, scaleFactor)
                     .mapRect(QRectF(0, 0, 1, 1))
                     .width();
  if ((factor < 0.07) || (factor > 100)) {
    return;
  }

  scale(scaleFactor, scaleFactor);
}

///
/// @brief Randomize component positions
///
void GraphWidget::shuffle() {
  foreach (QGraphicsItem *item, scene()->items()) {
    if (qgraphicsitem_cast<Node *>(item)) {
      item->setPos(-150 + QRandomGenerator::global()->bounded(300),
                   -150 + QRandomGenerator::global()->bounded(300));
    }
  }
}

///
/// @brief Set components (position, type, values) in the schematic
/// @param cmps List of component information
///
void GraphWidget::setComponents(QList<ComponentInfo> cmps) {
  this->Components.clear();
  for (int i = 0; i < cmps.length(); i++) {
    struct ComponentInfo CI = cmps.at(i);
    Component *comp = new Component(this, CI);
    Components.push_back(comp);
    scene()->addItem(comp);
    connect(comp, &Component::DoubleClicked, this,
            &GraphWidget::ComponentSelectionHandler);

    //  qDebug() << "Component added to the scene: " << CI.ID << " at (" <<
    //  CI.Coordinates.at(0) << ", " << CI.Coordinates.at(1) << ")";
  }
}

///
/// @brief Modify existing component parameters
/// @param CI Component information with updated values
///
void GraphWidget::ModifyComponent(ComponentInfo CI) {
  for (unsigned int i = 0; i < Components.size(); i++) {
    if (!Components.at(i)->getID().compare(CI.ID)) {
      Components.at(i)->setParameters(CI.val);
      Components.at(i)->update();
    }
  }
}

///
/// @brief Set wires (position and connection properties) in the schematic
/// @param wrs List of wire information
///
void GraphWidget::setWires(const QList<WireInfo> &wrs) {
  // Remove any previously stored wires
  Wires.clear();

  /*
   * Helper lambda:
   * Searches a container of Node* or Component* for an element
   * whose ID matches the given string.
   *    * @return index of the element if found, -1 otherwise
   */
  auto findIndexById = [](const auto &container, const QString &id) -> int {
    for (int i = 0; i < container.size(); ++i) {
      if (container.at(i)->getID() == id)
        return i;
    }
    return -1;
  };

  // Iterate over all wire descriptions
  for (const WireInfo &info : wrs) {

    // Determine whether each endpoint refers to a Node or a Component
    // Convention: IDs starting with 'N' represent Nodes
    const bool originIsNode = info.OriginID.startsWith('N');
    const bool destIsNode = info.DestinationID.startsWith('N');

    // Locate origin index in the appropriate container
    const int originIndex = originIsNode
                                ? findIndexById(Nodes, info.OriginID)
                                : findIndexById(Components, info.OriginID);

    // Locate destination index in the appropriate container
    const int destIndex = destIsNode
                              ? findIndexById(Nodes, info.DestinationID)
                              : findIndexById(Components, info.DestinationID);

    // If either endpoint cannot be resolved, skip this wire
    // and avoid using invalid indices
    if (originIndex < 0 || destIndex < 0) {
      qWarning() << "Wire skipped: invalid endpoint"
                 << "origin =" << info.OriginID
                 << "destination =" << info.DestinationID;
      continue;
    }

    // Create and configure the wire graphics item
    Wire *w = new Wire();
    w->setColor(info.WireColor);

    // Set the wire source endpoint (node or component)
    if (originIsNode) {
      w->setSource(Nodes.at(originIndex), info.PortOrigin);
    } else {
      w->setSource(Components.at(originIndex), info.PortOrigin);
    }

    // Set the wire destination endpoint (node or component)
    if (destIsNode) {
      w->setDestination(Nodes.at(destIndex), info.PortDestination);
    } else {
      w->setDestination(Components.at(destIndex), info.PortDestination);
    }

    // Store and display the wire
    Wires.push_back(w);
    scene()->addItem(w);

    // Paint
    w->paintWire();
  }
}
///
/// @brief Set nodes (position and wiring) in the schematic
/// @param nds List of node information
///
void GraphWidget::setNodes(QList<NodeInfo> nds) {
  this->Nodes.clear();
  for (int i = 0; i < nds.length(); i++) {
    struct NodeInfo NI = nds.at(i);
    Node *nd = new Node(this, NI);
    Nodes.push_back(nd);
    scene()->addItem(nd);
    //  qDebug() << "Node added to the scene: " << NI.ID;
  }
}

///
/// @brief Set complete schematic content
/// @param SchContent Schematic content structure
///
void GraphWidget::setSchematic(SchematicContent SchContent) {
  this->setNodes(SchContent.getNodes());
  this->setComponents(SchContent.getComponents());
  this->setWires(SchContent.getWires());
  this->setTexts(SchContent.getTexts());
}

///
/// @brief Clear all items from scene
///
void GraphWidget::clear() {
  while (!Components.empty()) {
    delete Components.front(), Components.pop_front(); // Remove components
  }
  while (!Nodes.empty()) {
    delete Nodes.front(), Nodes.pop_front(); // Remove nodes
  }
  while (!Wires.empty()) {
    delete Wires.front(), Wires.pop_front(); // Remove wires
  }
  while (!Texts.empty()) {
    delete Texts.front(), Texts.pop_front(); // Remove texts
  }
}

///
/// @brief Set text items in the schematic
/// @param texts List of text items
///
void GraphWidget::setTexts(QList<QGraphicsTextItem *> texts) {
  this->Texts.clear();
  for (int i = 0; i < texts.length(); i++) {
    QGraphicsTextItem *TI = texts.at(i);
    Texts.push_back(TI);
    scene()->addItem(TI);
  }
}
