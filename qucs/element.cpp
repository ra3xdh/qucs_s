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
#include "misc.h"

#include <QPainter>

namespace qucs {

void Line::draw(QPainter* painter) const {
    painter->drawLine(QPointF{x1, y1}, QPointF{x2, y2});
}

void Arc::draw(QPainter* painter) const {
    painter->drawArc(QRectF{x, y, w, h}, angle, arclen);
}

void Rect::draw(QPainter* painter) const {
    painter->drawRect(QRectF{x, y, w, h});
}

void Ellips::draw(QPainter* painter) const {
    painter->drawEllipse(QRectF{x, y, w, h});
}

void Polyline::draw(QPainter* painter) const {
    painter->drawPolyline(points.data(), points.size());
}

} // namespace qucs

void Text::draw(QPainter *painter) const {
  draw(painter, nullptr);
}

void Text::draw(QPainter *painter, QRectF* br) const {
  painter->save();

  painter->translate(x, y);
  painter->rotate(angle());

  QFont newFont = painter->font();
  newFont.setWeight(QFont::Light);
  newFont.setOverline(over);
  newFont.setUnderline(under);
  newFont.setPixelSize(Size);
  painter->setFont(newFont);

  misc::draw_richtext(painter, 0, 0, s, br);
  if (br) {
     br->moveTo(x, y);
  }

  painter->restore();
}

double Text::angle() const {
  // Historically Text uses mSin and mCos values to store
  // its rotation factor.
  //
  // The actual rotation was implemented as a clever
  // tranformation for a painter, like:
  //    QTransform(mCos, -mSin, mSin, mCos, …
  //
  // There were only four combinations of these values
  // and here we convert them to their human-readable
  // equivalents
  if (mCos == 0.0) {
    if (mSin == 1.0) {
      return 270;
    }
    return 90;
  }

  if (mCos == -1.0 && mSin == 0.0) {
    return 180;
  }

  return 0;
}


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
