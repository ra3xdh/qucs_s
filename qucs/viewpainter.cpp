/***************************************************************************
                        viewpainter.cpp  -  description
                             -------------------
    begin                : Tue Oct 05 2004
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

#include "main.h"
#include "viewpainter.h"
#include "diagrams/graph.h"

#include <QPainter>
#include <QFont>
#include <QDebug>

ViewPainter::ViewPainter(QPainter *p)
{
  Painter = p;
  DX = DY = Scale = 0.0;
  FontScale = PrintScale = 1.0;
}

ViewPainter::~ViewPainter()
{
}

// -------------------------------------------------------------
void ViewPainter::init(QPainter *p, float Scale_, int DX_, int DY_, 
		       int dx_, int dy_, 
		       float FontScale_, float PrintScale_)
{
  Painter = p;
  Scale = Scale_;
  FontScale = FontScale_;
  PrintScale = PrintScale_;
  DX = float(DX_) * Scale - float(dx_);
  DY = float(DY_) * Scale - float(dy_);

  QFont f = p->font();
  if(FontScale == 0.0)
    FontScale = Scale;
#ifdef __MINGW32__
  FontScale = Scale;
#endif
  f.setPointSizeF( FontScale * float(f.pointSize()) );
  p->setFont(f);
  LineSpacing = p->fontMetrics().lineSpacing();
  p-> setMatrixEnabled(false);   // we use our own coordinate transformation

  QPainter::RenderHints hints = 0;
  // Ask to to antialias drawings if requested
  if (QucsSettings.GraphAntiAliasing) hints |= QPainter::Antialiasing;
  // Ask to antialias text if requested
  if (QucsSettings.TextAntiAliasing) hints |= QPainter::TextAntialiasing;
  p->setRenderHints(hints);
}

// -------------------------------------------------------------
void ViewPainter::map(int x1, int y1, int& x, int& y)
{
  float z;
  z = float(x1)*Scale + DX;
  x = TO_INT(z);
  z = float(y1)*Scale + DY;
  y = TO_INT(z);
}

// -------------------------------------------------------------
void ViewPainter::drawPoint(int x1i, int y1i)
{
  float x1, y1;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;

  Painter->drawPoint(QPointF(x1, y1));
}

// -------------------------------------------------------------
void ViewPainter::drawLine(int x1i, int y1i, int x2i, int y2i)
{
  float x1, y1, x2, y2;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;
  x2 = float(x2i)*Scale + DX;
  y2 = float(y2i)*Scale + DY;

  Painter->drawLine(QLineF(x1, y1, x2, y2));
}

// -------------------------------------------------------------
/*!
 * draw a (line) graph from screen coord pairs
 */
void Graph::drawLines(int x0, int y0, ViewPainter *p) const
{
  float DX_, DY_;
  float x1, y1;
  auto Scale = p->Scale;
  auto Painter = p->Painter;
  QVector<qreal> dashes;

  double Stroke=10., Space=0.;
  switch(Style) {
    case GRAPHSTYLE_DASH:
      Stroke = 10.; Space =  6.;
      break;
    case GRAPHSTYLE_DOT:
      Stroke =  2.; Space =  4.;
      break;
    case GRAPHSTYLE_LONGDASH:
      Stroke = 24.; Space =  8.;
      break;
    default:
      break;
  }

  QPen pen = Painter->pen();
  switch(Style) {
    case GRAPHSTYLE_DASH:
    case GRAPHSTYLE_DOT:
    case GRAPHSTYLE_LONGDASH:
      dashes << Stroke << Space;
      pen.setDashPattern(dashes);
      Painter->setPen(pen);
      break;
    default:
      pen.setStyle(Qt::SolidLine);
      break;
  }
  Painter->setPen(pen);

  auto pp = begin();
  if(!pp->isPt())
    pp++;

  DX_ = p->DX + float(x0)*Scale;
  DY_ = p->DY + float(y0)*Scale;

  while(!pp->isGraphEnd()) {
    if(pp->isStrokeEnd()) ++pp; // ??
    QPainterPath path;
    if(pp->isPt()) {
      x1 = DX_ + pp->getScrX()*Scale;
      y1 = DY_ - pp->getScrY()*Scale;
      path.moveTo(x1,y1);
      ++pp;
    }else{
      break;
    }

    while(!pp->isStrokeEnd()) {
      x1 = DX_ + pp->getScrX()*Scale;
      y1 = DY_ - pp->getScrY()*Scale;
      path.lineTo(x1,y1);
      ++pp;
    }

    Painter->drawPath(path);
  }
}

// -------------------------------------------------------------
void Graph::drawStarSymbols(int x0i, int y0i, ViewPainter *p) const
{
  float x3, x0, y0, x1, x2, y1, y2;
  float z, DX_, DY_;
  auto Scale = p->Scale;
  auto Painter = p->Painter;
  auto pp = begin();
  if(!pp->isPt())
    pp++;

  DX_ = p->DX + float(x0i)*Scale;
  DY_ = p->DY + float(y0i)*Scale;

  while(!pp->isGraphEnd()) {
    if(pp->isPt()) {
      z = DX_ + pp->getScrX()*Scale;
      x0 = z-5.0*Scale;
      x3 = z+5.0*Scale;
      x1 = z-4.0*Scale;
      x2 = z+4.0*Scale;
      z = DY_ - (pp++)->getScrY()*Scale;
      y0 = z;
      y1 = z-4.0*Scale;
      y2 = z+4.0*Scale;
      Painter->drawLine(QLineF(x0, y0, x3, y0)); // horizontal line
      Painter->drawLine(QLineF(x1, y2, x2, y1)); // upper left to lower right
      Painter->drawLine(QLineF(x2, y2, x1, y1)); // upper right to lower left
    }
    else  pp++;
  }
}

// -------------------------------------------------------------
void Graph::drawCircleSymbols(int x0i, int y0i, ViewPainter *p) const
{
  float x0, y0;
  float z, DX_, DY_;
  auto Scale = p->Scale;
  auto Painter = p->Painter;
  auto pp = begin();
  if(!pp->isPt())
    pp++;

  z = 8.0*Scale;
  DX_ = p->DX + float(x0i)*Scale;
  DY_ = p->DY + float(y0i)*Scale;

  while(!pp->isGraphEnd()) {
    if(pp->isPt()) {
      x0 = DX_ + (pp->getScrX()-4.0)*Scale;
      y0 = DY_ - ((pp++)->getScrY()+4.0)*Scale;
      Painter->drawEllipse(QRectF(x0, y0, z, z));
    }
    else  pp++;
  }
}

// -------------------------------------------------------------
void Graph::drawArrowSymbols(int x0i, int y0i, ViewPainter *p) const
{
  int x0, y0, x1, x2, y1, y2;
  float DX_, DY_;
  auto Scale = p->Scale;
  auto Painter = p->Painter;
  auto pp = begin();
  if(!pp->isPt())
    pp++;

  DX_ = p->DX + float(x0i)*Scale;
  DY_ = p->DY + float(y0i)*Scale;
  y2 = DY_;

  while(!pp->isGraphEnd()) {
    if(pp->isPt()) {
      x0 = DX_ + pp->getScrX()*Scale;
      x1 = x0-4.0*Scale;
      x2 = x0+4.0*Scale;
      y0 = DY_ - (pp++)->getScrY()*Scale;
      y1 = y0+7.0*Scale;
      Painter->drawLine(QLineF(x0, y0, x0, y2));
      Painter->drawLine(QLineF(x1, y1, x0, y0));
      Painter->drawLine(QLineF(x2, y1, x0, y0));
    }
    else  pp++;
  }
}

// -------------------------------------------------------------
void ViewPainter::drawRect(int x1i, int y1i, int dxi, int dyi)
{
  float x1, y1, dx, dy;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;
  dx = float(dxi)*Scale;
  dy = float(dyi)*Scale;

  Painter->drawRect(QRectF(x1, y1, dx, dy));
}

// -------------------------------------------------------------
void ViewPainter::drawRectD(int x1i, int y1i, int dx, int dy)
{
  float x1, y1;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;

  Painter->drawRect(QRectF(x1, y1, dx, dy));
}

// -------------------------------------------------------------
void ViewPainter::drawRoundRect(int x1i, int y1i, int dxi, int dyi)
{
  float x1, y1, dx, dy;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;
  dx = float(dxi)*Scale;
  dy = float(dyi)*Scale;

  Painter->drawRoundRect(QRectF(x1, y1, dx, dy));
}

// -------------------------------------------------------------
void ViewPainter::drawEllipse(int x1i, int y1i, int dxi, int dyi)
{
  float x1, y1, dx, dy;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;
  dx = float(dxi)*Scale;
  dy = float(dyi)*Scale;

  Painter->drawEllipse(QRectF(x1, y1, dx, dy));
}

// -------------------------------------------------------------
// Returns width of text (and height if pointer is not null).
int ViewPainter::drawText(const QString& Text, int x1i, int y1i, int *Height)
{
  float x1, y1;
  x1= float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;

  QRectF rf;
  rf = Painter->boundingRect(QRectF(x1, y1, 0, 0), Qt::TextDontClip, Text);
  Painter->drawText(QRectF(x1, y1, 0, 0), Qt::TextDontClip, Text);

  if(Height)  *Height = TO_INT(rf.height());
  return TO_INT(rf.width());
}

// -------------------------------------------------------------
// Returns width of text (and height if pointer is not null).
int ViewPainter::drawTextMapped(const QString& Text, int x1, int y1,
				int *Height)
{
  QRectF rf;
  float y = 0.0;
  float x = 0.0;
  float h = 0.0;
  float w = 0.0;
  int i = 0;

  while (Text.length()>i) {
    if ((Text[i].toLatin1() == '_' || Text[i].toLatin1() == '^') &&
	!Text[i+1].isNull()) {
      bool is_sub = Text[i++].toLatin1() == '_';
      int len = 0;

      if (Text[i] == '{') {
        i++;
        while (!Text[i+len].isNull() && Text[i+len].toLatin1() != '}') len++;
      }

#ifdef __MINGW32__
      float scale = 1.0;
#else
      float scale = PrintScale;
#endif
      QFont fbak = Painter->font();
      QFont f = Painter->font();
      f.setPointSizeF(f.pointSizeF()*0.8);
      Painter->setFont(f);

      rf = Painter->boundingRect(QRectF(x1+x,
			y1+y + (is_sub ? +0.6 : -0.3) *
			fbak.pointSizeF() * scale,
			0, 0), Qt::TextDontClip,
			Text.mid(i, len ? len : 1));

      Painter->drawText(QRectF(x1+x,
			y1+y + (is_sub ? +0.6 : -0.3) *
			fbak.pointSizeF() * scale,
			0, 0), Qt::TextDontClip,
			Text.mid(i, len ? len : 1));

      Painter->setFont(fbak);
      x += rf.width();
      if (x > w) w = x;
      i += len ? len + 1 : 1;
    }
    else
    {
      int len = 0;
      while (Text.length()>(i+len)
             /*!Text[i+len].isNull()*/ && Text[i+len].toLatin1() != '_' &&
	     Text[i+len].toLatin1() != '^' && Text[i+len].toLatin1() != '\n')
			len++;

      rf = Painter->boundingRect(QRectF(x1+x, y1+y, 0, 0),
				 Qt::TextDontClip, Text.mid(i, len));
      Painter->drawText(QRectF(x1+x, y1+y, 0, 0),
			Qt::TextDontClip, Text.mid(i, len));

      if (h < rf.height()) {
        h = rf.height();
      }
      x += rf.width();
      if (x > w) w = x;
      if (Text.length()>(i+len)&&Text[i+len].toLatin1() == '\n') {
		    y += h;
		    x = 0;
		    i++;
      }
      i += len;
    }
  }

  if(Height) *Height = y+h;
  return w;
}

// -------------------------------------------------------------
void ViewPainter::drawArc(int x1i, int y1i, int wi, int hi, int Angle, int ArcLen)
{
  float x1, y1, w, h;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;
  w = float(wi)*Scale;
  h = float(hi)*Scale;

  Painter->drawArc(QRectF(x1, y1, w, h), Angle, ArcLen);
}

// -------------------------------------------------------------
void ViewPainter::fillRect(int x1i, int y1i, int dxi, int dyi, const QColor& Color)
{
  float x1, y1, dx, dy;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;

  dx = float(dxi)*Scale;
  dy = float(dyi)*Scale;

  Painter->fillRect(QRectF(x1, y1, dx, dy), QBrush(Color));
}

// -------------------------------------------------------------
void ViewPainter::eraseRect(int x1i, int y1i, int dx, int dy)
{
  float x1, y1;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;

  Painter->eraseRect(QRectF(x1, y1, dx, dy));
}

// -------------------------------------------------------------
// Draw little resize rectangles with center x1/y1 and size independent
// of zoom factor.
void ViewPainter::drawResizeRect(int x1i, int y1i)
{
  float x1, y1;
  x1 = float(x1i)*Scale + DX;
  y1 = float(y1i)*Scale + DY;

  Painter->drawRect(QRectF(x1-5, y1-5, 10, 10));
}

// vim:ts=8:sw=2:noet
