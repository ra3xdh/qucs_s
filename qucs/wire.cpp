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
#include "wire.h"
#include "multi_point.h"
#include "one_point.h"
#include "schematic.h"

#include <QPainter>

Wire::Wire(int _x1, int _y1, int _x2, int _y2, Node *n1, Node *n2)
{
  x1 = _x1;
  y1 = _y1;
  x2 = _x2;
  y2 = _y2;

  // Update center
  cx = (x1 + x2) / 2;
  cy = (y1 + y2) / 2;

  Port1 = n1;
  Port2 = n2;
  Label = nullptr;

  Type = isWire;
  isSelected = false;
}

Wire::~Wire()
{
  delete Label;
}

void Wire::rotate() noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, cx, cy);
  qucs_s::geom::rotate_point_ccw(x2, y2, cx, cy);

  if (Label != nullptr) {
    auto r = Label->root();
    qucs_s::geom::rotate_point_ccw(r.rx(), r.ry(), cx, cy);
    Label->moveRootTo(r.x(), r.y());
  }
}

// Lie x/y on wire ? 5 is the precision the coordinates have to fit.
bool Wire::getSelected(int x_, int y_)
{
  return qucs_s::geom::is_near_line(QPoint{x_, y_}, QPoint{x1, y1}, QPoint{x2, y2}, 5);
}

void Wire::paint(QPainter *painter) const {
  painter->save();
  if (isSelected) {
    painter->setPen(QPen(Qt::darkGray,6));
    painter->drawLine(x1, y1, x2, y2);
    painter->setPen(QPen(Qt::lightGray,2));
    painter->drawLine(x1, y1, x2, y2);
  }
  else {
    painter->setPen(QPen(Qt::darkBlue,2));
    painter->drawLine(x1, y1, x2, y2);
  }
  painter->restore();
}

void Wire::paintScheme(Schematic* sch)
{
  sch->PostPaintEvent(_Line, x1, y1, x2, y2);
}

bool Wire::isHorizontal()
{
  return (y1 == y2);
}

QPoint coordinatesFromDistance(double distanceFromPort1, const QPoint& port1, const QPoint& port2)
{
    const auto ratio = distanceFromPort1 / qucs_s::geom::distance(port1, port2);
    const auto x = static_cast<int>(port1.x() + ratio * (port2.x() - port1.x()));
    const auto y = static_cast<int>(port1.y() + ratio * (port2.y() - port1.y()));
    return {x, y};
}

// Adapter to use when loading a label from a file. Label root coordinates are stored
// as distance from port 1 of the wire. Here it's translated to absolute coordinates
// and passed further where real work is done.
void Wire::setName(int distFromPort1, int text_x, int text_y, const QString& name, const QString& value)
{
    // Zero-length wires are used to save node labels
    if (x1 == x2 && y1 == y2) {
        setName(name, value, x1, y1, text_x, text_y);
        return;
    }

    const auto root = coordinatesFromDistance(distFromPort1, QPoint{x1, y1}, QPoint{x2, y2});
    setName(name, value, root.x(), root.y(), text_x, text_y);
}

void Wire::setName(const QString& Name_, const QString& Value_, int root_x, int root_y, int x_, int y_)
{
  if(Name_.isEmpty() && Value_.isEmpty()) {
    delete Label;
    Label = nullptr;
    return;
  }

  if(!Label) {
    if (y1 == y2)
      Label = new WireLabel(Name_, root_x, y1, x_, y_, isHWireLabel);
    else if (x1 == x2)
      Label = new WireLabel(Name_, x1, root_y, x_, y_, isVWireLabel);
    else
      Label = new WireLabel(Name_, root_x, root_y, x_, y_, isLabel);
    Label->pOwner = this;
    Label->initValue = Value_;
  }
  else Label->setName(Name_);
}

// Converts all necessary data of the wire into a string. This can be used to
// save it to an ASCII file or to transport it via the clipboard.
QString Wire::save()
{
  QString s  = "<"+QString::number(x1)+" "+QString::number(y1);
          s += " "+QString::number(x2)+" "+QString::number(y2);
  if(Label) {
          s += " \""+Label->Name+"\" ";
          s += QString::number(Label->x1)+" "+QString::number(Label->y1)+" ";
          s += QString::number(static_cast<int>(qucs_s::geom::distance(QPoint{x1, y1}, Label->root())));
          s += " \""+Label->initValue+"\">";
  }
  else { s += R"( "" 0 0 0 "">)"; }
  return s;
}

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

  // Update center
  cx = (x1 + x2) / 2;
  cy = (y1 + y2) / 2;

  n = s.section('"',1,1);
  if(!n.isEmpty()) {     // is wire labeled ?
    int nx = s.section(' ',5,5).toInt(&ok);   // x coordinate
    if(!ok) return false;

    int ny = s.section(' ',6,6).toInt(&ok);   // y coordinate
    if(!ok) return false;

    int delta = s.section(' ',7,7).toInt(&ok);// delta for x/y root coordinate
    if(!ok) return false;

    setName(delta, nx, ny, n, s.section('"',3,3));  // Wire Label
  }

  return true;
}

QRect Wire::boundingRect() const noexcept
{
  return QRect{QPoint{x1, y1}, QPoint{x2, y2}}
    .normalized();
}

void Wire::moveCenter(int dx, int dy) noexcept
{
  Element::moveCenter(dx, dy);
  x1 += dx;
  y1 += dy;
  x2 += dx;
  y2 += dy;
  if (Label) Label->moveRoot(dx, dy);
}


void Wire::setP1(const QPoint& new_p1)
{
  if (x1 == new_p1.x() && y1 == new_p1.y()) {
    return;
  }

  if (Label != nullptr) {
    const QPoint old_p1{x1, y1};
    const QPoint p2{x2, y2};

    const auto ratio = qucs_s::geom::distance(old_p1, Label->root()) / qucs_s::geom::distance(old_p1, p2);
    const auto x = static_cast<int>(std::round(new_p1.x() + ratio * (p2.x() - new_p1.x())));
    const auto y = static_cast<int>(std::round(new_p1.y() + ratio * (p2.y() - new_p1.y())));

    Label->moveRootTo(x, y);
  }

  x1 = new_p1.x();
  y1 = new_p1.y();
}


void Wire::setP2(const QPoint& new_p2)
{
  if (x2 == new_p2.x() && y2 == new_p2.y()) {
    return;
  }

  if (Label != nullptr) {
    const QPoint p1{x1, y1};
    const QPoint old_p2{x2, y2};

    const auto ratio = qucs_s::geom::distance(old_p2, Label->root()) / qucs_s::geom::distance(p1, old_p2);
    const auto x = static_cast<int>(std::round(p1.x() + ratio * (new_p2.x() - p1.x())));
    const auto y = static_cast<int>(std::round(p1.y() + ratio * (new_p2.y() - p1.y())));

    Label->moveRootTo(x, y);
  }

  x2 = new_p2.x();
  y2 = new_p2.y();
}
