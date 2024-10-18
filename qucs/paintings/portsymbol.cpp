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
#include "main.h"
#include "portsymbol.h"
#include "schematic.h"

#include <QInputDialog>
#include <QMargins>
#include <QPainter>

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
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  QSize r = metrics.size(0, nameStr);
  x1 = -r.width() - 8;
  y1 = -((r.height() + 8) >> 1);
  x2 = 8 - x1;
  y2 = r.height() + 8;
}

PortSymbol::~PortSymbol()
{
}

void PortSymbol::paint(QPainter *painter) {
  painter->save();

  QRect circle_br{cx - 4, cy - 4, 8, 8};
  painter->setPen(QPen(Qt::red,1));  // like open node
  painter->drawEllipse(circle_br);

  QSize name_size = painter->fontMetrics().size(0b0, nameStr.isEmpty() ? numberStr : nameStr);
  const int half_nameheight = static_cast<int>(std::round(name_size.height() / 2.0));

  constexpr int offset = 8;

  int tx, ty;
  switch(Angel) {
  case 90:
    tx = cx - half_nameheight;
    ty = cy + offset + name_size.width();
    break;
  case 180:
    tx = cx + offset;
    ty = cy - half_nameheight;
    break;
  case 270:
    tx = cx - half_nameheight;
    ty = cy - offset;
    break;
  default:
    tx = cx - offset - name_size.width();
    ty = cy - half_nameheight;
  }

  const bool is_vertical = Angel == 90 || Angel == 270;

  painter->save();
  {
    painter->translate(tx, ty);
    if (is_vertical) {
        painter->rotate(-90.0);
        name_size.transpose();
    }

    painter->setPen(Qt::black);
    painter->drawText(0, 0, 1, 1, Qt::TextDontClip, nameStr.isEmpty() ? numberStr : nameStr);
  }
  painter->restore();

  QRect name_br{
    tx, ty, name_size.width(), name_size.height() * (is_vertical ? -1 : 1)};

  QRect total_br = circle_br
    .united(name_br.normalized())
    .marginsAdded(QMargins{2, 2, 2, 2});

  x1 = total_br.left() - cx;
  y1 = total_br.top() - cy;
  x2 = total_br.width();
  y2 = total_br.height();

  painter->setPen(Qt::lightGray);
  painter->drawRect(total_br);

  if (isSelected) {
    painter->setPen(QPen(Qt::darkGray,3));
    painter->drawRoundedRect(total_br.marginsAdded(QMargins{3, 3, 3, 3}), 4, 4);
  }
  painter->restore();
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

  // name string
  n = s.section(' ', 5);
  if (n.isEmpty()) return true;
  nameStr = n;

  return true;
}

// --------------------------------------------------------------------------
QString PortSymbol::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += numberStr+" "+QString::number(Angel) + " " + nameStr;
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

QString PortSymbol::saveJSON()
{
  QString s = QString ("{\"type\" : \"portsymbol\", "
                       "\"x\" : %1, \"y\" : %2},").arg(cx).arg(cy);
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
void PortSymbol::rotate(int, int)
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

bool PortSymbol::MousePressing(Schematic *sch) {
  if (!sch->isSymbolOnly) {
    return false;
  }
  QString text = QInputDialog::getText(nullptr, QObject::tr("Port name"),
                                        QObject::tr("Input port name:"));
  if (!text.isNull() && !text.isEmpty()) {
    nameStr = text;
    numberStr = "0"; // 0 indicates no number assigned
    return true;
  }

  return false;
}

void PortSymbol::MouseMoving(Schematic* doc, int, int, int gx, int gy, Schematic *, int, int) {
  cx = gx;
  cy = gy;
  paintScheme(doc);
}

Painting* PortSymbol::newOne() {
  return new PortSymbol();
}

// This function is called from double click handler, see mouseactions.cpp
// Returned bool signal whether the object has changed as a result of
// the invocation.
bool PortSymbol::Dialog(QWidget* /*parent*/Doc) {
  // Forbid manual editing, change port name on schematic to change it in the symbol
  // Allow to edit ports only for SymbolOnly documents (*.sym).
  Schematic *sch = (Schematic *) Doc;
  if (!sch->isSymbolOnly) {
    return false;
  }

  // When nameStr is empty, we're dealing with just a symbol without
  // a corresponding schematic. In that case allow to user to input
  // port name
  QString text = QInputDialog::getText(nullptr, QObject::tr("Port name"),
                                        QObject::tr("Input port name:"),
                                        QLineEdit::Normal,
                                        nameStr);
  if (text.isNull() || text.isEmpty()) {
    return false;
  }

  // nameStr is auto derived from corresponding port in schematic.
  // When there is no such port, fallback value is used.
  nameStr = text;
  return true;
}
