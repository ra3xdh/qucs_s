/***************************************************************************
                          wirelabel.cpp  -  description
                             -------------------
    begin                : Sun February 29 2004
    copyright            : (C) 2004 by Michael Margraf
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
#include "wirelabel.h"
#include "multi_point.h"
#include "one_point.h"
#include "wire.h"
#include "main.h"

#include <QMargins>
#include <QString>
#include <QPainter>

WireLabel::WireLabel(const QString& _Name, int _cx, int _cy,
                     int _x1, int _y1, int _Type)
{
  cx = _cx;
  cy = _cy;
  x1 = _x1;
  y1 = _y1;
  setName(_Name);

  QFontMetrics metrics(QucsSettings.font, 0);
  textSize = metrics.size(0, Name);
  x2 = x1 + textSize.width();
  y2 = y1 + textSize.height();

  Type = _Type;
}

bool WireLabel::getSelected(int x, int y)
{
  const QPoint click{x, y};
  const QPoint text{x1, y1};
  return
    QRect{text, textSize}
      .marginsAdded({5, 5, 5, 5})
      .contains(click)
    ||
    qucs_s::geom::is_near_line(click, root(), text, 5);
}

void WireLabel::paint(QPainter *p) const {
  p->save();

  QFont newFont{ p->font() };
  newFont.setWeight(isHighlighted ? QFont::Bold : QFont::Normal);
  p->setFont(newFont);

  p->setPen(QPen{
    isHighlighted ? Qt::darkBlue : Qt::black,
    isHighlighted ? 3.0 : 1.0
  });

  QRect text_br;
  p->drawText(x1, y1, 1, 1, Qt::TextDontClip, Name, &text_br);

  bool right = text_br.right() < cx;
  bool bottom = text_br.bottom() < cy;

  p->setPen(QPen{initValue.isEmpty() ? Qt::darkMagenta : Qt::red,0});

  text_br = text_br.marginsAdded(QMargins{3, 3, 3, 3});
  p->drawLine(cx, cy, right ? text_br.right() : text_br.left(), bottom ? text_br.bottom() : text_br.top());
  p->drawLine(
    right ? text_br.right() : text_br.left(),
    bottom ? text_br.bottom() : text_br.top(),
    right ? text_br.right() : text_br.left(),
    bottom ? text_br.top() : text_br.bottom()
  );

  p->drawLine(
    right ? text_br.right() : text_br.left(),
    bottom ? text_br.bottom() : text_br.top(),
    right ? text_br.left() : text_br.right(),
    bottom ? text_br.bottom() : text_br.top()
  );

  if (Type != isNodeLabel) {
    int start_angle = 0;
    // TODO: angle need to be calculated from the wire
    // angle or not used at all.
    switch (Type) {
    case isHWireLabel:
      start_angle = 16 * (right ? 45 : 225);
      break;
    case isVWireLabel:
    case isLabel: //  same as V for simplicity
      start_angle = 16 * (bottom ? -45 : 135);
      break;
    default:
      assert(false);  // shouln't get there
    }

    constexpr int span_angle = 16 * 270;
    p->drawArc(cx-4, cy-4, 8, 8, start_angle, span_angle);
  }

  if(isSelected)
  {
    p->setPen(QPen(Qt::darkGray,3));
    p->drawRoundedRect(QRect{{x1,y1}, textSize}, 4, 4);
  }
  p->restore();
}

void WireLabel::setName(const QString& Name_)
{
  Name = Name_;

  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  textSize = metrics.size(0, Name);
  x2 = x1 + textSize.width();
  y2 = y1 + textSize.height();
}

// Converts all necessary data of the wire into a string. This can be used to
// save it to an ASCII file or to transport it via the clipboard.
// Wire labels use the same format like wires, but with length zero.
QString WireLabel::save()
{
  QString s("<");
	s += QString::number(cx)+" "+QString::number(cy)+" "
	  +  QString::number(cx)+" "+QString::number(cy)
	  +  " \""+Name +"\" "
	  +  QString::number(x1)+" "+QString::number(y1)+" 0 \""
	  +  initValue+"\">";
  return s;
}


void WireLabel::getLabelBounding(int& _xmin, int& _ymin, int& _xmax, int& _ymax)
{
    _xmin = std::min(x1,x1+(x2+6));
    _xmax = std::max(x1,x1+(x2+6));
    _ymin = std::min(y1,y1+(y2+6));
    _ymax = std::max(y1,y1+(y2+5));
    _ymax = std::max(cy,_ymax);
}

QPoint WireLabel::root() const noexcept
{
  return QPoint{cx, cy};
}

void WireLabel::moveRoot(int dx, int dy) noexcept
{
  cx += dx;
  cy += dy;
}

void WireLabel::moveRootTo(int x, int y) noexcept
{
  cx = x;
  cy = y;
}

QPoint WireLabel::center() const noexcept
{
  return QRect{{x1, y1}, textSize}.center();
}

void WireLabel::moveCenter(int dx, int dy) noexcept
{
  x1 += dx;
  y1 += dy;
}

void WireLabel::rotate() noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, cx, cy);
}

void WireLabel::mirrorX() noexcept
{
  moveCenter(0, (center().y() - root().y()) * 2);
}

void WireLabel::mirrorY() noexcept
{
  moveCenter((center().x() - root().x()) * 2, 0);
}


QRect WireLabel::boundingRect() const noexcept
{
  return QRect{QPoint{cx, cy}, QPoint{x1, y1}}
    .normalized()
    .united(QRect{{x1, y1}, textSize}.normalized());
}