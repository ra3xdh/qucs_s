/// @file graphwidget.cpp
/// @brief Schematic editor graphics widget (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "graphwidget.h"

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

void GraphWidget::shuffle() {
  foreach (QGraphicsItem *item, scene()->items()) {
    if (qgraphicsitem_cast<Node *>(item)) {
      item->setPos(-150 + QRandomGenerator::global()->bounded(300),
                   -150 + QRandomGenerator::global()->bounded(300));
    }
  }
}

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

void GraphWidget::ModifyComponent(ComponentInfo CI) {
  for (unsigned int i = 0; i < Components.size(); i++) {
    if (!Components.at(i)->getID().compare(CI.ID)) {
      Components.at(i)->setParameters(CI.val);
      Components.at(i)->update();
    }
  }
}

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
    for (std::size_t i = 0; i < container.size(); ++i) {
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

void GraphWidget::setSchematic(SchematicContent SchContent) {
  this->setNodes(SchContent.getNodes());
  this->setComponents(SchContent.getComponents());
  this->setWires(SchContent.getWires());
  this->setTexts(SchContent.getTexts());
}

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

void GraphWidget::setTexts(QList<QGraphicsTextItem *> texts) {
  this->Texts.clear();
  for (int i = 0; i < texts.length(); i++) {
    QGraphicsTextItem *TI = texts.at(i);
    Texts.push_back(TI);
    scene()->addItem(TI);
  }
}
