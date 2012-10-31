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
#include <QtGui>
#include "viewpainter.h"
#include "diagrams/graph.h"

#include <math.h>


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
void ViewPainter::init(QPainter *p, float Scale_, int DX_, int DY_, int dx_, int dy_, float FontScale_, float PrintScale_)
{
  Painter = p;
  Scale = Scale_;
  FontScale = FontScale_;
  PrintScale = PrintScale_;
  DX = floor(float(DX_) * Scale) - float(dx_);
  DY = floor(float(DY_) * Scale) - float(dy_);

  QFont f = p->font();
  if(FontScale == 0.0)
    FontScale = Scale;
#ifdef __MINGW32__
  FontScale = Scale;
#endif
  f.setPointSizeFloat( FontScale * float(f.pointSize()) );
  p->setFont(f);
  LineSpacing = p->fontMetrics().lineSpacing();
  p->setWorldXForm(false);   // we use our own coordinate transformation
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
void ViewPainter::drawPoint(int x1, int y1)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);

  Painter->drawPoint(x1, y1);
}

// -------------------------------------------------------------
void ViewPainter::drawLine(int x1, int y1, int x2, int y2)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);
  z = float(x2)*Scale + DX;
  x2 = TO_INT(z);
  z = float(y2)*Scale + DY;
  y2 = TO_INT(z);

  Painter->drawLine(x1, y1, x2, y2);
}

// -------------------------------------------------------------
void ViewPainter::drawLines(int x0, int y0, float *pp)
{
  float z, DX_, DY_;
  int x1, x2, y1, y2;
  if(*pp < 0)
    pp++;

  DX_ = DX + float(x0)*Scale;
  DY_ = DY + float(y0)*Scale;

  while(*pp > GRAPHEND) {
    if(*pp >= 0) {
      z = DX_ + (*pp)*Scale;
      x1 = TO_INT(z);
      z = DY_ - (*(pp+1))*Scale;
      y1 = TO_INT(z);
      Painter->drawPoint(x1, y1);
    }
    while(*pp > BRANCHEND) {   // until end of branch
      z = DX_ + (*pp)*Scale;
      x1 = TO_INT(z);
      z = DY_ - (*(pp+1))*Scale;
      y1 = TO_INT(z);
      pp += 2;
      while(*pp > STROKEEND) { // until end of stroke
        z = DX_ + (*pp)*Scale;
        x2 = TO_INT(z);
        z = DY_ - (*(pp+1))*Scale;
        y2 = TO_INT(z);
        Painter->drawLine(x1, y1, x2, y2);
        pp += 2;
        if(*pp <= STROKEEND)  break;

        z = DX_ + (*pp)*Scale;
        x1 = TO_INT(z);
        z = DY_ - (*(pp+1))*Scale;
        y1 = TO_INT(z);
        Painter->drawLine(x2, y2, x1, y1);
        pp += 2;
      }
      if(*pp <= BRANCHEND)  break;   // end of line ?
      pp++;
    }
    pp++;
  }
}

// -------------------------------------------------------------
void ViewPainter::drawStarSymbols(int x0, int y0, float *pp)
{
  int x3, x1, x2, y1, y2;
  float z, DX_, DY_;
  if(*pp < 0)
    pp++;

  DX_ = DX + float(x0)*Scale;
  DY_ = DY + float(y0)*Scale;

  while(*pp > GRAPHEND) {
    if(*pp >= 0) {
      z = DX_ + (*(pp++))*Scale;
      x0 = TO_INT(z-5.0*Scale);
      x3 = TO_INT(z+5.0*Scale);
      x1 = TO_INT(z-4.0*Scale);
      x2 = TO_INT(z+4.0*Scale);
      z = DY_ - (*(pp++))*Scale;
      y0 = TO_INT(z);
      y1 = TO_INT(z-4.0*Scale);
      y2 = TO_INT(z+4.0*Scale);
      Painter->drawLine(x0, y0, x3, y0); // horizontal line
      Painter->drawLine(x1, y2, x2, y1); // upper left to lower right
      Painter->drawLine(x2, y2, x1, y1); // upper right to lower left
    }
    else  pp++;
  }
}

// -------------------------------------------------------------
void ViewPainter::drawCircleSymbols(int x0, int y0, float *pp)
{
  int d;
  float z, DX_, DY_;
  if(*pp < 0)
    pp++;

  z = 8.0*Scale;
  d = TO_INT(z);
  DX_ = DX + float(x0)*Scale;
  DY_ = DY + float(y0)*Scale;

  while(*pp > GRAPHEND) {
    if(*pp >= 0) {
      z = DX_ + (*(pp++)-4.0)*Scale;
      x0 = TO_INT(z);
      z = DY_ - (*(pp++)+4.0)*Scale;
      y0 = TO_INT(z);
      Painter->drawEllipse(x0, y0, d, d);
    }
    else  pp++;
  }
}

// -------------------------------------------------------------
void ViewPainter::drawArrowSymbols(int x0, int y0, float *pp)
{
  int x1, x2, y1, y2;
  float z, DX_, DY_;
  if(*pp < 0)
    pp++;

  DX_ = DX + float(x0)*Scale;
  DY_ = DY + float(y0)*Scale;
  y2 = TO_INT(DY_);

  while(*pp > GRAPHEND) {
    if(*pp >= 0) {
      z = DX_ + (*(pp++))*Scale;
      x0 = TO_INT(z);
      x1 = TO_INT(z-4.0*Scale);
      x2 = TO_INT(z+4.0*Scale);
      z = DY_ - (*(pp++))*Scale;
      y0 = TO_INT(z);
      y1 = TO_INT(z+7.0*Scale);
      Painter->drawLine(x0, y0, x0, y2);
      Painter->drawLine(x1, y1, x0, y0);
      Painter->drawLine(x2, y1, x0, y0);
    }
    else  pp++;
  }
}

// -------------------------------------------------------------
void ViewPainter::drawRect(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);
  z = float(dx)*Scale;
  dx = TO_INT(z);
  z = float(dy)*Scale;
  dy = TO_INT(z);

  Painter->drawRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
void ViewPainter::drawRectD(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);

  Painter->drawRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
void ViewPainter::drawRoundRect(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);
  z = float(dx)*Scale;
  dx = TO_INT(z);
  z = float(dy)*Scale;
  dy = TO_INT(z);

  Painter->drawRoundRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
void ViewPainter::drawEllipse(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);
  z = float(dx)*Scale;
  dx = TO_INT(z);
  z = float(dy)*Scale;
  dy = TO_INT(z);

  Painter->drawEllipse(x1, y1, dx, dy);
}

// -------------------------------------------------------------
// Returns width of text (and height if pointer is not null).
int ViewPainter::drawText(const QString& Text, int x1, int y1, int *Height)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);

  QRect r;
  Painter->drawText(x1, y1, 0, 0, Qt::TextDontClip, Text, -1, &r);

  if(Height)  *Height = r.height();
  return r.width();
}

// -------------------------------------------------------------
// Returns width of text (and height if pointer is not null).
int ViewPainter::drawTextMapped(const QString& Text, int x1, int y1,
				int *Height)
{
  QRect r;
  int y = 0;
  int x = 0;
  int h = 0;
  int w = 0;
  int i = 0;

  while (!Text[i].isNull()) {
    if ((Text[i].latin1() == '_' || Text[i].latin1() == '^') &&
	!Text[i+1].isNull()) {
      bool is_sub = Text[i++].latin1() == '_';
      int len = 0;

      if (Text[i] == '{') {
        i++;
        while (!Text[i+len].isNull() && Text[i+len].latin1() != '}') len++;
      }

#ifdef __MINGW32__
      float scale = 1.0;
#else
      float scale = PrintScale;
#endif
      QFont fbak = Painter->font();
      QFont f = Painter->font();
      f.setPointSizeFloat(f.pointSizeFloat()*0.8);
      Painter->setFont(f);
      Painter->drawText(x1+x,
			y1+y + (is_sub ? +0.6 : -0.3) *
			fbak.pointSizeFloat() * scale,
			0, 0, Qt::TextDontClip,
			Text.mid(i, len ? len : 1), -1, &r);
      Painter->setFont(fbak);
      x += r.width();
      if (x > w) w = x;
      i += len ? len + 1 : 1;
    }
    else
    {
      int len = 0;
      while (!Text[i+len].isNull() && Text[i+len].latin1() != '_' &&
	     Text[i+len].latin1() != '^' && Text[i+len].latin1() != '\n')
	len++;
      Painter->drawText(x1+x, y1+y,
			0, 0, Qt::TextDontClip, Text.mid(i, len), -1, &r);
      if (h < r.height()) {
        h = r.height();
      }
      x += r.width();
      if (x > w) w = x;
      if (Text[i+len].latin1() == '\n') {
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
// Returns width of text (and height if pointer is not null).
void ViewPainter::drawArc(int x1, int y1, int w, int h, int Angle, int ArcLen)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);

  // Width and height get a different treatment due to some alaising artefacts.
  // The following procedure was found empirically.
  w = int(float(w)*Scale);
  h = int(float(h)*Scale);
  Painter->drawArc(x1, y1, w+1, h+1, Angle, ArcLen);
}

// -------------------------------------------------------------
void ViewPainter::fillRect(int x1, int y1, int dx, int dy, const QColor& Color)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);

  z = float(dx)*Scale;
  dx = TO_INT(z);
  z = float(dy)*Scale;
  dy = TO_INT(z);

  Painter->fillRect(x1, y1, dx, dy, QBrush(Color));
}

// -------------------------------------------------------------
void ViewPainter::eraseRect(int x1, int y1, int dx, int dy)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);

  Painter->eraseRect(x1, y1, dx, dy);
}

// -------------------------------------------------------------
// Draw little resize rectangles with center x1/y1 and size independent
// of zoom factor.
void ViewPainter::drawResizeRect(int x1, int y1)
{
  float z;
  z = float(x1)*Scale + DX;
  x1 = TO_INT(z);
  z = float(y1)*Scale + DY;
  y1 = TO_INT(z);

  Painter->drawRect(x1-5, y1-5, 10, 10);
}
