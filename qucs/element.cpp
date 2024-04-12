/***************************************************************************
                          element.cpp  -  description
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

#include "element.h"

namespace qucs {

void Line::draw(ViewPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawLine(cx + x1, cy - y1, cx + x2, cy - y2);
  } else {
    painter->drawLine(cx + x1, cy + y1, cx + x2, cy + y2);
  }
}

void Line::draw(QPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawLine(cx + x1, cy - y1, cx + x2, cy - y2);
  } else {
    painter->drawLine(cx + x1, cy + y1, cx + x2, cy + y2);
  }
}

void Arc::draw(ViewPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawArc(cx + x, cy - y, w, h, angle, arclen);
  } else {
    painter->drawArc(cx + x, cy + y, w, h, angle, arclen);
  }
}

void Arc::draw(QPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawArc(cx + x, cy - y, w, h, angle, arclen);
  } else {
    painter->drawArc(cx + x, cy + y, w, h, angle, arclen);
  }
}

void Rect::draw(ViewPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawRect(cx + x, cy - y, w, h);
  } else {
    painter->drawRect(cx + x, cy + y, w, h);
  }
}

void Rect::draw(QPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawRect(cx + x, cy - y, w, h);
  } else {
    painter->drawRect(cx + x, cy + y, w, h);
  }
}

void Ellips::draw(ViewPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawEllipse(cx + x, cy - y, w, h);
  } else {
    painter->drawEllipse(cx + x, cy + y, w, h);
  }
}

void Ellips::draw(QPainter* painter, int cx, int cy, bool y_grows_up) const {
  // For explanation please refer to parent class doc
  if (y_grows_up) {
    painter->drawEllipse(cx + x, cy - y, w, h);
  } else {
    painter->drawEllipse(cx + x, cy + y, w, h);
  }
}

} // namespace qucs

Element::Element()
{
  Type = isDummyElement;
  isSelected = false;
  cx = cy = x1 = y1 = x2 = y2 = 0;
}

Element::~Element()
{
}

void Element::paintScheme(Schematic *)
{
}

void Element::paintScheme(QPainter *)
{
}

void Element::setCenter(int, int, bool)
{
}

void Element::getCenter(int&, int&)
{
}
