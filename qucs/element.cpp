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
#include "one_point.h"

#include <cmath>
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
  // Historically Text uses mSin and mCos values to describe
  // at what angle the text is rotated.
  //
  // It employed some clever technique to draw the text rotated
  // by applying "skew" transformations to QPainter and using mCos
  // and mSin as skew factors, but later the whole drawing routine
  // was refactored and it switched to using QPainter::rotate() and
  // degrees to describe the rotation angle.
  //
  // Many other parts of the codebase still depend on mCos and mSin,
  // for example rotating and mirroring of a component and more than
  // 200 component constructors. It is a piece of work to get rid
  // of mCos and mSin completely, so they're kept as is and their
  // "degrees" equivalent is calculated when the text needs to be drawn.

  const double radians = std::atan2(mSin, mCos);
  const double degrees = (radians * 180.0) / 3.14159265 /* Pi */;

  // QPainter::rotate() rotates *clockwise*, so if you want a text
  // to have rotation angle of 45° (towards up-right direction),
  // then the -45° has to be passed to QPainter::rotate().
  //
  // QPainter::rotate() is called in Text::draw
  return -degrees;
}

// x and y are relative to component's x and y
void Property::paint(int x, int y, QPainter* p)
{
  p->drawText(x, y, 1, 1, Qt::TextDontClip, Name + "=" + Value, &br);
}

bool Element::moveCenterTo(int x, int y) noexcept
{
  auto old_center = center();
  moveCenter(x - old_center.x(), y - old_center.y());
  return x != old_center.x() || y != old_center.y();
}

bool Element::moveCenterTo(const QPoint& p) noexcept
{
  return moveCenterTo(p.x(), p.y());
}

bool Element::moveCenter(int dx, int dy) noexcept
{
  cx += dx;
  cy += dy;
  return dx != 0 || dy != 0;
}

bool Element::rotate(int rcx, int rcy) noexcept
{
  int ncx = cx;
  int ncy = cy;
  qucs_s::geom::rotate_point_ccw(ncx, ncy, rcx, rcy);
  const bool has_moved = moveCenterTo(ncx, ncy);
  const bool has_rotated = rotate();
  return has_rotated || has_moved;
}

bool Element::rotate(const QPoint& center) noexcept
{
  return rotate(center.x(), center.y());
}

bool Element::mirrorX(int axis) noexcept
{
  const bool has_moved = moveCenterTo(cx, qucs_s::geom::mirror_coordinate(cy, axis));
  const bool has_mirrored = mirrorX();
  return has_mirrored || has_moved;
}

bool Element::mirrorY(int axis) noexcept
{
  const bool has_moved = moveCenterTo(qucs_s::geom::mirror_coordinate(cx, axis), cy);
  const bool has_mirrored = mirrorY();
  return has_mirrored || has_moved;
}

QRect Element::boundingRect() const noexcept
{
  return QRect{QPoint{x1, y1}, QPoint{x2, y2}}
    .normalized()
    .translated(center());
}

QPoint Element::center() const noexcept
{
  return {cx, cy};
}
