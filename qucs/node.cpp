/***************************************************************************
                          node.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "node.h"

#include "component.h"
#include "wire.h"

#include <QPainter>

#include <QDebug>

Node::Node(int x, int y) :
    a_wires(),
    a_components(),
    a_dtype(""),
    a_state(0),
    a_name()
{
    Type  = isNode;
    cx = x;
    cy = y;
}

void Node::paint(QPainter* painter) const
{
    painter->save();

    if (isSelected) {
        painter->setPen(QPen(Qt::darkGray, 5));
        painter->drawEllipse(cx-5, cy-5, 10, 10);
    }
    else if (conn_count() == 1) {
        if (hasLabel()) {
            painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue); // open but labeled
        } else {
            painter->setPen(QPen(Qt::red,1));  // node is open
            painter->drawEllipse(cx-4, cy-4, 8, 8);
        }
    }
    else if (conn_count() > 2) {
        painter->setBrush(Qt::darkBlue);  // more than 2 connections
        painter->setPen(QPen(Qt::darkBlue,1));
        painter->drawEllipse(cx-3, cy-3, 6, 6);
    }
    else if (a_wires.size() != 2) {
        painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue);
    }

  painter->restore();
}

bool Node::getSelected(int x, int y)
{
    return cx - 3 <= x && x <= cx + 3 && cy - 3 <= y && y <= cy + 3;
}

void Node::setName(const QString& name, const QString& value, int x, int y)
{
    // Passing two empty strings acted like a signal to remove the label
    // and later was superseded by dropLabel() method. This assertion is
    // just merely a guard against legacy usage, it may be freely removed
    // after some time.
    // Added on 2025-06-12.
    assert(!(name.isEmpty() && value.isEmpty()));

    if (!hasLabel()) {
        acquireLabel(std::make_unique<WireLabel>(name, cx, cy, x, y));
    }
    else {
        label()->setName(name);
    }
    label()->initValue = value;
}

bool Node::moveCenter(int dx, int dy) noexcept
{
    Element::moveCenter(dx, dy);
    if (hasLabel()) {
        label()->moveRoot(dx, dy);
    }
    return dx != 0 || dy != 0;
}

Node* Node::merge(Node* donor)
{
    std::ranges::for_each(donor->wires(), [this,donor](auto* w) { w->Port1 == donor ? w->Port1 = this : w->Port2 = this; });
    std::ranges::copy(donor->wires(), std::back_inserter(a_wires));
    donor->a_wires.clear();

    for (auto* c : donor->components()) {
        for (auto* p : c->Ports) {
            if (p->Connection == donor) {
                p->Connection = this;
            }
        }
    }

    std::ranges::copy(donor->components(), std::back_inserter(a_components));
    donor->a_components.clear();

    if (!this->hasLabel() && donor->hasLabel()) {
        this->acquireLabel(donor->releaseLabel());
    }

    this->isSelected = this->isSelected || donor->isSelected;

    return donor;
}

bool Node::isOverlapping(int otherX, int otherY) const {
  return (otherX == x() && otherY == y());
}

bool Node::isOverlapping(const Node* other) const {
  // Comparison of self is false
  if (this == other) {
    return false;
  }

  return isOverlapping(other->x(), other->y());
}
