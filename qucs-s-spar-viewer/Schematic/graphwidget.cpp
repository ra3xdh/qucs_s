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

//! [0]
GraphWidget::GraphWidget(QWidget* parent) : QGraphicsView(parent), timerId(0) {
  QGraphicsScene* scene = new QGraphicsScene(this);
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  // scene->setSceneRect(-200, -200, 600, 300);
  setScene(scene);
  setCacheMode(CacheBackground);
  setViewportUpdateMode(BoundingRectViewportUpdate);
  setRenderHint(QPainter::Antialiasing);
  setTransformationAnchor(AnchorUnderMouse);
  setDragMode(QGraphicsView::ScrollHandDrag);
  // scale(qreal(0.8), qreal(0.8));
  setWindowTitle(tr("Schematic"));
}

void GraphWidget::itemMoved() {
  if (!timerId) {
    timerId = startTimer(1000 / 25);
  }
}

void GraphWidget::ComponentSelectionHandler(struct ComponentInfo CI) {
  emit this->SendComponentSelectionToMainFunction(CI);
}

void GraphWidget::keyPressEvent(QKeyEvent* /*event*/) {}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent* event) {
  Q_UNUSED(event);

  QList<Symbol*> nodes;
  foreach (QGraphicsItem* item, scene()->items()) {
    if (Symbol* node = qgraphicsitem_cast<Symbol*>(item)) {
      nodes << node;
    }
  }
}

void GraphWidget::wheelEvent(QWheelEvent* event) {
  scaleView(pow((double)2, -event->angleDelta().y() / 240.0));
}

void GraphWidget::scaleView(qreal scaleFactor) {
  qreal factor = transform()
                     .scale(scaleFactor, scaleFactor)
                     .mapRect(QRectF(0, 0, 1, 1))
                     .width();
  if (factor < 0.07 || factor > 100) {
    return;
  }

  scale(scaleFactor, scaleFactor);
}
//! [7]

void GraphWidget::shuffle() {
  foreach (QGraphicsItem* item, scene()->items()) {
    if (qgraphicsitem_cast<Node*>(item)) {
      item->setPos(-150 + QRandomGenerator::global()->bounded(300),
                   -150 + QRandomGenerator::global()->bounded(300));
    }
  }
}

void GraphWidget::zoomIn() {
  scaleView(qreal(1.2));
}

void GraphWidget::zoomOut() {
  scaleView(1 / qreal(1.2));
}

// This function sets the properties of the components such as position, type
// and value
void GraphWidget::setComponents(QList<ComponentInfo> cmps) {
  this->Components.clear();
  for (int i = 0; i < cmps.length(); i++) {
    struct ComponentInfo CI = cmps.at(i);
    Component* comp         = new Component(this, CI);
    Components.push_back(comp);
    scene()->addItem(comp);
    connect(comp, SIGNAL(DoubleClicked(struct ComponentInfo)), this,
            SLOT(ComponentSelectionHandler(struct ComponentInfo)));
    //  qDebug() << "Component added to the scene: " << CI.ID << " at (" <<
    //  CI.Coordinates.at(0) << ", " << CI.Coordinates.at(1) << ")";
  }
}

void GraphWidget::ModifyComponent(ComponentInfo CI) {
  for (unsigned int i = 0; i < Components.size(); i++) {
    if (!Components.at(i)->getID().compare(CI.ID)) {
      Components.at(i)->setParameters(CI.val);
      Components.at(i)->update();
    }
  }
}

// This function sets the position and the connection properties of the wires
void GraphWidget::setWires(QList<WireInfo> wrs) {
  this->Wires.clear();
  int origin, destination;
  bool OriginIsNode, DestIsNode;

  for (int i = 0; i < wrs.length(); i++) {
    //************ Find origin ****************
    // is Node or Component?
    OriginIsNode = wrs.at(i).OriginID.at(0) == 'N';
    for (int index = 0;
         index < (OriginIsNode) ? Nodes.size() : Components.size(); index++) {
      if (OriginIsNode) {
        if (!Nodes.at(index)->getID().compare(wrs.at(i).OriginID)) {
          origin = index;
          break;
        }
      } else {
        if (!Components.at(index)->getID().compare(wrs.at(i).OriginID)) {
          origin = index;
          break;
        }
      }
    }
    // Find destination index
    // is Node or Component?
    DestIsNode = wrs.at(i).DestinationID.at(0) == 'N';
    for (int index = 0; index < (DestIsNode) ? Nodes.size() : Components.size();
         index++) {

      if (DestIsNode) {
        if (!Nodes.at(index)->getID().compare(wrs.at(i).DestinationID)) {
          destination = index;
          break;
        }
      } else {
        if (!Components.at(index)->getID().compare(wrs.at(i).DestinationID)) {
          destination = index;
          break;
        }
      }
    }

    Wire* w = new Wire();
    w->setColor(wrs.at(i).WireColor);
    OriginIsNode ? w->setSource(Nodes.at(origin), wrs.at(i).PortOrigin)
                 : w->setSource(Components.at(origin), wrs.at(i).PortOrigin);
    DestIsNode
        ? w->setDestination(Nodes.at(destination), wrs.at(i).PortDestination)
        : w->setDestination(Components.at(destination),
                            wrs.at(i).PortDestination);
    Wires.push_back(w);
    scene()->addItem(w);
    w->paintWire();
  }
}

// This functions sets the position of the wiring nodes
void GraphWidget::setNodes(QList<NodeInfo> nds) {
  this->Nodes.clear();
  for (int i = 0; i < nds.length(); i++) {
    struct NodeInfo NI = nds.at(i);
    Node* nd           = new Node(this, NI);
    Nodes.push_back(nd);
    scene()->addItem(nd);
    //  qDebug() << "Node added to the scene: " << NI.ID;
  }
}

void GraphWidget::setSchematic(SchematicContent SchContent) {
  this->setNodes(SchContent.getNodes());
  this->setComponents(SchContent.getComponents());
  this->setWires(SchContent.getWires());
  this->setTexts(SchContent.getTexts());
}

// Clear the scene
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

void GraphWidget::setTexts(QList<QGraphicsTextItem*> texts) {
  this->Texts.clear();
  for (int i = 0; i < texts.length(); i++) {
    QGraphicsTextItem* TI = texts.at(i);
    Texts.push_back(TI);
    scene()->addItem(TI);
  }
}
