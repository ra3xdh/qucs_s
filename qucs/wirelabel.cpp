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
  initValue = "";

  Type = _Type;
  isSelected = false;
  isHighlighted = false;
}

WireLabel::~WireLabel()
{
}

// ----------------------------------------------------------------
void WireLabel::paintScheme(QPainter *p)
{
  p->drawRect(x1, y1, x2, y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(cy < y1+(y2>>1))
      p->drawLine(cx, cy, x1, y1);
    else
      p->drawLine(cx, cy, x1, y1+y2);
  }
  else {
    if(cy < y1+(y2>>1))
      p->drawLine(cx, cy, x1+x2, y1);
    else
      p->drawLine(cx, cy, x1+x2, y1+y2);
  }
}

// ----------------------------------------------------------------
void WireLabel::setCenter(int x_, int y_, bool relative)
{
  switch(Type) {
    case isMovingLabel:
      if(relative) {
        x1 += x_;  cx += x_;
        y1 += y_;  cy += y_;
      }
      else {
        x1 = x_;  cx = x_;
        y1 = y_;  cy = y_;
      }
      break;
    case isHMovingLabel:
      if(relative) { x1 += x_;  cx += x_; }
      else { x1 = x_;  cx = x_; }
      break;
    case isVMovingLabel:
      if(relative) { y1 += y_;  cy += y_; }
      else { y1 = y_;  cy = y_; }
      break;
    default:
      if(relative) {
        x1 += x_;  y1 += y_; // moving cx/cy is done by owner (wire, node)
      }
      else { x1 = x_; y1 = y_; }
  }
}

// ----------------------------------------------------------------
bool WireLabel::getSelected(int x, int y)
{
  if(x1 <= x)
    if(y1 <= y)
      if((x1+x2) >= x)
        if((y1+y2) >= y)
          return true;

  return false;
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
  p->drawText(x1, y1, 0, 0, Qt::TextDontClip, Name, &text_br);

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
    switch (Type) {
    case isHWireLabel:
      start_angle = 16 * (right ? 45 : 225);
      break;
    case isVWireLabel:
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
    p->drawRoundedRect(x1-2, y1-2, x2+6, y2+5, 4, 4);
  }
  p->restore();
}

// ----------------------------------------------------------------
void WireLabel::setName(const QString& Name_)
{
  Name = Name_;
  
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  QSize r = metrics.size(0, Name);
  x2 = r.width();
  y2 = r.height()-2;    // remember size of text
}

// ----------------------------------------------------------------
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
