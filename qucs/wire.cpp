/***************************************************************************
                          wire.cpp  -  description
                             -------------------
    begin                : Wed Sep 3 2003
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
#include <QtGui>
#include "wire.h"


Wire::Wire(int _x1, int _y1, int _x2, int _y2, Node *n1, Node *n2)
{
  cx = 0;
  cy = 0;
  x1 = _x1;
  y1 = _y1;
  x2 = _x2;
  y2 = _y2;
  Port1 = n1;
  Port2 = n2;
  Label  = 0;

  Type = isWire;
  isSelected = false;
}

Wire::~Wire()
{
}

// ----------------------------------------------------------------
void Wire::rotate()
{
  int xm, ym, tmp;

  xm = (x1+x2) >> 1;
  ym = (y1+y2) >> 1;

  tmp = x1;
  x1  = xm + y1  - ym;
  y1  = ym - tmp + xm;

  tmp = x2;
  x2  = xm + y2  - ym;
  y2  = ym - tmp + xm;

  if(Label) {
    tmp = Label->cx;
    Label->cx = xm + Label->cy - ym;
    Label->cy = ym - tmp + xm;
    if(Label->Type == isHWireLabel) Label->Type = isVWireLabel;
    else Label->Type = isHWireLabel;
  }
}

// ----------------------------------------------------------------
void Wire::setCenter(int x, int y, bool relative)
{
  if(relative) {
    x1 += x;  x2 += x;
    y1 += y;  y2 += y;
//    if(Label) Label->setCenter(x, y, true);
  }
  else {
    x1 = x;  x2 = x;
    y1 = y;  y2 = y;
  }
}

// ----------------------------------------------------------------
void Wire::getCenter(int& x, int& y)
{
  x = (x1+x2) >> 1;
  y = (y1+y2) >> 1;
}

// ----------------------------------------------------------------
// Lie x/y on wire ? 5 is the precision the coordinates have to fit.
bool Wire::getSelected(int x_, int y_)
{
  if(x1-5 <= x_) if(x2+5 >= x_) if(y1-5 <= y_) if(y2+5 >= y_)
    return true;

  return false;
}

// ----------------------------------------------------------------
void Wire::paintScheme(QPainter *p)
{
  p->drawLine(x1, y1, x2, y2);
//  if(Label)
//    if((Label->Type == isHWireLabel) || (Label->Type == isHWireLabel))
//    if(Label->Type == isHWireLabel)
//      Label->paintScheme(p);
}

// ----------------------------------------------------------------
void Wire::paint(ViewPainter *p)
{
  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,6));
    p->drawLine(x1, y1, x2, y2);
    p->Painter->setPen(QPen(Qt::lightGray,2));
    p->drawLine(x1, y1, x2, y2);
  }
  else {
    p->Painter->setPen(QPen(Qt::darkBlue,2));
    p->drawLine(x1, y1, x2, y2);
  }
}

// ----------------------------------------------------------------
bool Wire::isHorizontal()
{
  return (y1 == y2);
}

// ----------------------------------------------------------------
void Wire::setName(const QString& Name_, const QString& Value_, int delta_, int x_, int y_)
{
  if(Name_.isEmpty() && Value_.isEmpty()) {
    if(Label) delete Label;
    Label = 0;
    return;
  }

  if(!Label) {
    if(isHorizontal())
      Label = new WireLabel(Name_, x1+delta_, y1, x_, y_, isHWireLabel);
    else
      Label = new WireLabel(Name_, x1, y1+delta_, x_, y_, isVWireLabel);
    Label->pOwner = this;
    Label->initValue = Value_;
  }
  else Label->setName(Name_);
}

// ----------------------------------------------------------------
// Converts all necessary data of the wire into a string. This can be used to
// save it to an ASCII file or to transport it via the clipboard.
QString Wire::save()
{
  QString s  = "<"+QString::number(x1)+" "+QString::number(y1);
          s += " "+QString::number(x2)+" "+QString::number(y2);
  if(Label) {
          s += " \""+Label->Name+"\" ";
          s += QString::number(Label->x1)+" "+QString::number(Label->y1)+" ";
          s += QString::number(Label->cx-x1 + Label->cy-y1);
          s += " \""+Label->initValue+"\">";
  }
  else { s += " \"\" 0 0 0 \"\">"; }
  return s;
}

// ----------------------------------------------------------------
// This is the counterpart to Wire::save.
bool Wire::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString n;
  n  = s.section(' ',0,0);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',1,1);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // x2
  x2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // y2
  y2 = n.toInt(&ok);
  if(!ok) return false;

  n = s.section('"',1,1);
  if(!n.isEmpty()) {     // is wire labeled ?
    int nx = s.section(' ',5,5).toInt(&ok);   // x coordinate
    if(!ok) return false;

    int ny = s.section(' ',6,6).toInt(&ok);   // y coordinate
    if(!ok) return false;

    int delta = s.section(' ',7,7).toInt(&ok);// delta for x/y root coordinate
    if(!ok) return false;

    setName(n, s.section('"',3,3), delta, nx, ny);  // Wire Label
  }

  return true;
}
