/***************************************************************************
                        portsymbol.cpp  -  description
                             -------------------
    begin                : Sun Sep 5 2004
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
#include "main.h"
#include "portsymbol.h"
#include "schematic.h"
#include "qucs.h"

PortSymbol::PortSymbol(int cx_, int cy_, const QString& numberStr_,
                                         const QString& nameStr_)
{
  Name = ".PortSym ";
  isSelected = false;
  cx = cx_;
  cy = cy_;

  Angel = 0;
  nameStr = nameStr_;
  numberStr = numberStr_;
  QFontMetrics  metrics(((Schematic*)QucsMain->DocumentTab->currentPage())->font());   // get size of text
  QSize r = metrics.size(0, nameStr);
  x1 = -r.width() - 8;
  y1 = -((r.height() + 8) >> 1);
  x2 = 8 - x1;
  y2 = r.height() + 8;
}

PortSymbol::~PortSymbol()
{
}

// --------------------------------------------------------------------------
void PortSymbol::paint(ViewPainter *p)
{
  p->Painter->setPen(QPen(Qt::red,1));  // like open node
  p->drawEllipse(cx-4, cy-4, 8, 8);


  QSize r = p->Painter->fontMetrics().size(0, nameStr);
  int Unit = int(8.0 * p->Scale);
  x1 = -r.width() - Unit;
  y1 = -((r.height() + Unit) >> 1);
  x2 = Unit - x1;
  y2 = r.height() + Unit;

  QMatrix wm = p->Painter->worldMatrix();
  QMatrix Mat(1.0, 0.0, 0.0, 1.0, p->DX + float(cx) * p->Scale,
				   p->DY + float(cy) * p->Scale);
  p->Painter->setWorldMatrix(Mat);

  int tmp, tx, ty;
  tx = x1 + (Unit >> 1);
  ty = y1 + (Unit >> 1);
  switch(Angel) {
    case 90:
      x1 = y1;
      y1 = -Unit;
      tmp = x2;  x2 = y2;  y2 = tmp;
      p->Painter->rotate(-90.0); // automatically enables transformation
      break;
    case 180:
      x1 = -Unit;
      tx = Unit >> 1;
      break;
    case 270:
      tx = Unit >> 1;
      tmp = x1;  x1 = y1;  y1 = tmp;
      tmp = x2;  x2 = y2;  y2 = tmp;
      p->Painter->rotate(-90.0); // automatically enables transformation
      break;
  }

  p->Painter->setPen(Qt::black);
  p->Painter->drawText(tx, ty, 0, 0, Qt::TextDontClip, nameStr);


  p->Painter->setWorldMatrix(wm);
  p->Painter->setWorldXForm(false);
  x1 = int(float(x1) / p->Scale);
  x2 = int(float(x2) / p->Scale);
  y1 = int(float(y1) / p->Scale);
  y2 = int(float(y2) / p->Scale);

  p->Painter->setPen(Qt::lightGray);
  p->drawRect(cx+x1, cy+y1, x2, y2);

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(cx+x1-4, cy+y1-4, x2+8, y2+8);
  }
}

// --------------------------------------------------------------------------
void PortSymbol::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Ellipse, cx-4, cy-4, 8, 8);
  p->PostPaintEvent(_Rect, cx+x1, cy+y1, x2, y2);
}

// --------------------------------------------------------------------------
void PortSymbol::getCenter(int& x, int &y)
{
  x = cx;
  y = cy;
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void PortSymbol::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x;  cy = y; }
}

// --------------------------------------------------------------------------
bool PortSymbol::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  numberStr  = s.section(' ',3,3);    // number
  if(numberStr.isEmpty()) return false;

  n  = s.section(' ',4,4);      // Angel
  if(n.isEmpty()) return true;  // be backward-compatible
  Angel = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString PortSymbol::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += numberStr+" "+QString::number(Angel);
  return s;
}

// --------------------------------------------------------------------------
QString PortSymbol::saveCpp()
{
  QString s = 
    QString ("new Port (%1, %2)").
    arg(cx).arg(cy);
  s = "Ports.append (" + s + "); /* " + nameStr + " */";
  return s;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool PortSymbol::getSelected(float fX, float fY, float)
{
  if(int(fX) < cx+x1)  return false;
  if(int(fY) < cy+y1)  return false;
  if(int(fX) > cx+x1+x2)  return false;
  if(int(fY) > cy+y1+y2)  return false;

  return true;
}

// --------------------------------------------------------------------------
void PortSymbol::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx+x1;     _y1 = cy+y1;
  _x2 = cx+x1+x2;  _y2 = cy+y1+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void PortSymbol::rotate()
{
  if(Angel < 270)  Angel += 90;
  else  Angel = 0;
}

// --------------------------------------------------------------------------
// Mirrors about connection node (not center line !).
void PortSymbol::mirrorX()
{
  if(Angel == 90)  Angel = 270;
  else  if(Angel == 270)  Angel = 90;
}

// --------------------------------------------------------------------------
// Mirrors about connection node (not center line !).
void PortSymbol::mirrorY()
{
  if(Angel == 0)  Angel = 180;
  else  if(Angel == 180)  Angel = 0;
}
