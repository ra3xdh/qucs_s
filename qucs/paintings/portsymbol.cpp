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
#include "one_point.h"
#include "portsymbol.h"
#include "schematic.h"

#include <QInputDialog>
#include <QMargins>
#include <QPainter>

namespace helper {

inline QSize textSize(const QString& text) {
  return QFontMetrics(QucsSettings.font, nullptr).size(0, text);
}


class TextHelper {
  static constexpr int c = 8; // Some inherited magic number
  int m_angle;
  QSize m_textSize;

public:
  TextHelper(int a, const QString& s) : m_angle{a} {
    m_textSize = QFontMetrics(QucsSettings.font, nullptr).size(0, s);
  }

  QPoint offset() const {
    const int half_height = m_textSize.height() / 2;

    switch (m_angle) {
    case 180:
    case 270:
      return {c, -half_height};
    default:
      return {-c - m_textSize.width(), -half_height};
    }
  }

  QRect bounds() const {
    auto to = offset();
    switch (m_angle) {
    case 90:
      return QRect{to.y(), to.x() + m_textSize.width() + 2 * c, m_textSize.height(),
                   m_textSize.width()};
    case 270:
      return QRect{to.x() - m_textSize.height(), to.y() - m_textSize.width(), m_textSize.height(),
                   m_textSize.width()};
    default:
      return QRect{to, m_textSize};
    }
  }
};


// Calculates the bounding of port symbol and offset of port name text
std::pair<QRect, QPoint> boundingAndTextOffset(int angle, const QString& portName, int circleRadius) {
  const QRect circle_br{
    -QPoint{circleRadius, circleRadius},
     QPoint{circleRadius, circleRadius}};

  const TextHelper th{angle, portName};

  const auto total_br = circle_br
    .united(th.bounds())
    .normalized()
    .marginsAdded(QMargins{2, 2, 2, 2});

  return {total_br, th.offset()};
}
} // namespace helper


constexpr int portCircleRadius = 4;
constexpr int portCircleDiameter = 2 * portCircleRadius;


PortSymbol::PortSymbol(int cx_, int cy_, const QString& numberStr_,
                                         const QString& nameStr_)
    : numberStr(numberStr_)
    , nameStr(nameStr_)
    , angle(0)
{
  Name = ".PortSym ";
  isSelected = false;
  cx = cx_;
  cy = cy_;

  updateBounds();
}

void PortSymbol::paint(QPainter *painter)
{
  // Little circle and port name
  {
    painter->save();

    painter->translate(center());

    // Little circle
    const QRect circle_br{ -portCircleRadius, -portCircleRadius, portCircleDiameter, portCircleDiameter };
    painter->setPen(QPen(Qt::red,1));  // like open node
    painter->drawEllipse(circle_br);

    // Port name
    painter->setPen(Qt::black);
    if (angle == 90 || angle == 270) painter->rotate(-90.0);
    painter->drawText(m_textOrigin.x(), m_textOrigin.y(), 1, 1, Qt::TextDontClip, nameStr.isEmpty() ? numberStr : nameStr);
    painter->restore();
  }


  // Rectangle and selection box
  {
    painter->save();

    // Rectangle around the text and the circle.
    painter->setPen(Qt::lightGray);
    painter->drawRect(boundingRect());

    // Selection box
    if (isSelected) {
      painter->setPen(QPen(Qt::darkGray,3));
      painter->drawRoundedRect(boundingRect().marginsAdded(QMargins{3, 3, 3, 3}), 4, 4);
    }

    painter->restore();
  }
}

void PortSymbol::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Ellipse, cx - portCircleRadius, cy - portCircleRadius, portCircleDiameter, portCircleDiameter);
  p->PostPaintEvent(_Rect, x1, y1, x2 - x1, y2 - y1);
}

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
  angle = n.toInt(&ok);
  if(!ok) return false;

  // name string
  n = s.section(' ', 5);
  if (n.isEmpty()) return true;
  nameStr = n;

  updateBounds();
  return true;
}

QString PortSymbol::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += numberStr+" "+QString::number(angle) + " " + nameStr;
  return s;
}

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

// Checks if the coordinates x/y point to the painting.
bool PortSymbol::getSelected(const QPoint& click, int /*tolerance*/)
{
  return QRect{QPoint{x1, y1}, QPoint{x2, y2}}.contains(click);
}

// Rotates around the center.
inline void PortSymbol::rotate() noexcept
{
  if (angle < 270) {
    angle += 90;
  } else {
    angle = 0;
  }
  updateBounds();
}

// Rotates around the center.
inline void PortSymbol::rotate(int x, int y) noexcept
{
  qucs_s::geom::rotate_point_ccw(cx, cy, x, y);
  rotate();
}

// Mirrors about connection node (not center line !).
void PortSymbol::mirrorX() noexcept
{
  switch (angle) {
    case 90:
      angle = 270;
      break;
    case 270:
      angle = 90;
      break;
    default:
      break;
  };
  updateBounds();
}

// Mirrors about connection node (not center line !).
void PortSymbol::mirrorY() noexcept
{
  switch (angle) {
    case 0:
      angle = 180;
      break;
    case 180:
      angle = 0;
      break;
    default:
      break;
  };
  updateBounds();
}

bool PortSymbol::MousePressing(Schematic *sch) {
  if (!sch->getIsSymbolOnly()) {
    return false;
  }
  QString text = QInputDialog::getText(nullptr, QObject::tr("Port name"),
                                        QObject::tr("Input port name:"));
  if (!text.isNull() && !text.isEmpty()) {
    nameStr = text;
    numberStr = "0"; // 0 indicates no number assigned
    updateBounds();
    return true;
  }

  return false;
}

void PortSymbol::MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& /*cursor*/) {
  moveCenterTo(onGrid.x(), onGrid.y());
  paintScheme(sch);
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
  if (!sch->getIsSymbolOnly()) {
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

  updateBounds();
  return true;
}

void PortSymbol::updateBounds()
{
  const QString& text = nameStr.isEmpty() ? numberStr : nameStr;
  auto [br, to] = helper::boundingAndTextOffset(angle, text, portCircleRadius);

  m_textOrigin = to;
  x1 = cx + br.left();
  y1 = cy + br.top();
  x2 = cx + br.right();
  y2 = cy + br.bottom();
}