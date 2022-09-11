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

#include "viewpainter.h"
#include "wirelabel.h"

#include <QPainter>

Node::Node(int _x, int _y)
{
  Label = 0;
  Type  = isNode;
  State = 0;
  DType = "";

  cx = _x;
  cy = _y;
}

Node::~Node()
{
}

// -------------------------------------------------------------
void Node::paint(ViewPainter *p)
{
  switch(Connections.count()) {
    case 1:  if(Label)
               p->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue); // open but labeled
             else {
               p->Painter->setPen(QPen(Qt::red,1));  // node is open
               p->drawEllipse(cx-4, cy-4, 8, 8);
             }
             return;
    case 2:  if(Connections.getFirst()->Type == isWire)
               if(Connections.getLast()->Type == isWire) return;
             p->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue);
             break;
    default: p->Painter->setBrush(Qt::darkBlue);  // more than 2 connections
	     p->Painter->setPen(QPen(Qt::darkBlue,1));
	     p->drawEllipse(cx-3, cy-3, 6, 6);
	     p->Painter->setBrush(Qt::NoBrush);
             break;
  }
}

// ----------------------------------------------------------------
bool Node::getSelected(int x_, int y_)
{
  if(cx-5 <= x_) if(cx+5 >= x_) if(cy-5 <= y_) if(cy+5 >= y_)
    return true;

  return false;
}

// ----------------------------------------------------------------
void Node::setName(const QString& Name_, const QString& Value_, int x_, int y_)
{
  if(Name_.isEmpty() && Value_.isEmpty()) {
    if(Label) delete Label;
    Label = 0;
    return;
  }

  if(!Label) Label = new WireLabel(Name_, cx, cy, x_, y_, isNodeLabel);
  else Label->setName(Name_);
  Label->pOwner = this;
  Label->initValue = Value_;
}
