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

#include "wirelabel.h"

#include <QPainter>

Node::Node(int x, int y)
{
  Label = nullptr;
  Type  = isNode;
  State = 0;
  DType = "";

  cx = x;
  cy = y;
}

Node::~Node()
{
  delete Label;
}

void Node::paint(QPainter* painter) const {
  painter->save();

  switch(connections.size()) {
    case 1:
      if (Label) {
        painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue); // open but labeled
      } else {
        painter->setPen(QPen(Qt::red,1));  // node is open
        painter->drawEllipse(cx-4, cy-4, 8, 8);
      }
      painter->restore();
      return;

    case 2:
      if (connections.front()->Type == isWire && connections.back()->Type == isWire) {
          painter->restore();
          return;
      }
      painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue);
      break;

    default:
        painter->setBrush(Qt::darkBlue);  // more than 2 connections
	      painter->setPen(QPen(Qt::darkBlue,1));
	      painter->drawEllipse(cx-3, cy-3, 6, 6);
  }
  painter->restore();
}

bool Node::getSelected(int x, int y)
{
  return cx - 5 <= x && x <= cx + 5 && cy - 5 <= y && y <= cy + 5;
}

void Node::setName(const QString& name, const QString& value, int x, int y)
{
  if (name.isEmpty() && value.isEmpty()) {
    if (Label) {
      delete Label;
      Label = nullptr;
    }
    return;
  }

  if (!Label) {
    Label = new WireLabel(name, cx, cy, x, y, isNodeLabel);
  }
  else {
    Label->setName(name);
  }
  Label->pOwner = this;
  Label->initValue = value;
}

Element* Node::other_than(Element* elem) const
{
  auto other = std::find_if_not(connections.begin(), connections.end(), [elem](auto o){return o == elem;}
  );

  return other == connections.end() ? nullptr : *other;
}

bool Node::moveCenter(int dx, int dy) noexcept
{
  Element::moveCenter(dx, dy);
  if (Label != nullptr) {
    Label->moveRoot(dx, dy);
  }
  return dx != 0 || dy != 0;
}
