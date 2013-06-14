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
#include <QtGui>
#include "wirelabel.h"
#include "viewpainter.h"
#include "wire.h"
#include "main.h"

#include "qucs.h"
#include "schematic.h"


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

// ----------------------------------------------------------------
void WireLabel::paint(ViewPainter *p)
{
  p->Painter->setPen(QPen(Qt::black,1));
  x2 = p->drawText(Name, x1, y1, &y2);

  int xpaint=0, ypaint=4, phi=0;
  switch(Type) {
    case isVWireLabel:  ypaint=0; xpaint=4; phi=16*140; break;
    case isHWireLabel:  phi=16*50; break;
    case isNodeLabel:   ypaint = 0;
    default:            ;
  }

  int c, d;
  int a = int(double(x2) / p->Scale) >> 1;
  int b = int(double(y2) / p->Scale) >> 1;
  if(cx < x1+a) {    // where should frame be painted ?
    if(cy < y1+b) {
      if(phi == 16*50)  phi += 16*180;
      p->map(x1-3, y1-2, a, b);    // low right
      c = a + (x2>>1);
      d = b + y2;
      p->map(cx+xpaint, cy+ypaint, xpaint, ypaint);
    }
    else {
      if(phi != 0)  phi += 16*180;
      p->map(x1-3, y1+1, a, b);    // up right
      b += y2;
      c  = a + (x2>>1);
      d  = b - y2;
      p->map(cx+xpaint, cy-ypaint, xpaint, ypaint);
    }
  }
  else {
    if(cy < y1+b) {
      p->map(x1+3, y1-2, a, b);   // low left
      a += x2;
      c  = a - (x2>>1);
      d  = b + y2;
      p->map(cx-xpaint, cy+ypaint, xpaint, ypaint);
    }
    else {
      if(phi > 16*90)  phi += 16*180;
      p->map(x1+3, y1+1, a, b);    // up left
      a += x2;
      b += y2;
      c  = a - (x2>>1);
      d  = b - y2;
      p->map(cx-xpaint, cy-ypaint, xpaint, ypaint);
    }
  }

  if(initValue.isEmpty())
    p->Painter->setPen(QPen(Qt::darkMagenta,0));
  else
    p->Painter->setPen(QPen(Qt::red,0));

  if(phi)  p->drawArc(cx-4, cy-4, 8, 8, phi, 16*255);
  p->Painter->drawLine(a, b, c, b);
  p->Painter->drawLine(a, b, a, d);
  p->Painter->drawLine(xpaint, ypaint, a, b);

  x2 = int(double(x2) / p->Scale);
  y2 = int(double(y2) / p->Scale);
  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(x1-2, y1-2, x2+6, y2+5);
  }
}

// ----------------------------------------------------------------
void WireLabel::setName(const QString& Name_)
{
  Name = Name_;

  QFontMetrics  metrics(((Schematic*)QucsMain->DocumentTab->currentPage())->font());   // get size of text
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
