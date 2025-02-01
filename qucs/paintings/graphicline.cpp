/***************************************************************************
                              graphicline.cpp
                             -----------------
    begin                : Mon Nov 24 2003
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
#include "graphicline.h"
#include "filldialog.h"
#include "misc.h"
#include "multi_point.h"
#include "one_point.h"
#include "schematic.h"


GraphicLine::GraphicLine(int ax, int ay, int bx, int by, QPen pen_) : pen(pen_)
{
  Name = "Line ";
  isSelected = false;

  x1 = ax;
  y1 = ay;
  x2 = bx;
  y2 = by;

  updateCenter();
}

void GraphicLine::paint(QPainter *painter) {
  painter->save();
  painter->setPen(pen);
  painter->drawLine(x1, y1, x2, y2);

  if (isSelected) {
    painter->setPen(QPen(Qt::darkGray,pen.width() + 5));
    painter->drawLine(x1, y1, x2, y2);
    painter->setPen(QPen(Qt::white, pen.width(), pen.style()));
    painter->drawLine(x1, y1, x2, y2);

    misc::draw_resize_handle(painter, QPoint{x1, y1});
    misc::draw_resize_handle(painter, QPoint{x2, y2});
  }
  painter->restore();
}

void GraphicLine::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Line, x1, y1, x2, y2);
}

Painting* GraphicLine::newOne()
{
  return new GraphicLine();
}

Element* GraphicLine::info(QString& name, char* &bitmapFile, bool getNewOne)
{
  name = QObject::tr("Line");
  bitmapFile = (char *) "line";

  if (getNewOne) return new GraphicLine();
  return 0;
}

bool GraphicLine::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // x2
  auto w = n.toInt(&ok);
  if(!ok) return false;
  x2 = x1 + w;

  n  = s.section(' ',4,4);    // y2
  auto h = n.toInt(&ok);
  if(!ok) return false;
  y2 = y1 + h;

  updateCenter();

  n  = s.section(' ',5,5);    // color
  QColor co = misc::ColorFromString(n);
  pen.setColor(co);
  if(!pen.color().isValid()) return false;

  n  = s.section(' ',6,6);    // thickness
  pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',7,7);    // line style
  pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

QString GraphicLine::save()
{
  QString s = Name+QString::number(x1) + " " + QString::number(y1) + " ";
  s += QString::number(x2 - x1) + " " + QString::number(y2 - y1) + " ";
  s += pen.color().name() + " " + QString::number(pen.width()) + " ";
  s += QString::number(pen.style());
  return s;
}

QString GraphicLine::saveCpp()
{
  QString s =
    QString ("new Line (%1, %2, %3, %4, QPen (QColor (\"%5\"), %6, %7))").
    arg(x1).arg(y1).arg(x2 - x1).arg(y2 - y1).
    arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style()));
  s = "Lines.append (" + s + ");";
  return s;
}

QString GraphicLine::saveJSON()
{
  QString s =
    QString ("{\"type\" : \"line\", "
      "\"x1\" : %1, \"y1\" : %2, \"x2\" : %3, \"y2\" : %4, "
      "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\"},").
      arg(x1).arg(cy).arg(x2 - x1).arg(y2 - y1).
      arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style()));
  return s;
}

// Checks if the resize area was clicked.
bool GraphicLine::resizeTouched(const QPoint& click, int tolerance)
{
  if (qucs_s::geom::distance(click, QPoint(x1, y1)) <= tolerance) {
    lineState = State::Moving_End1;
    return true;
  }

  if (qucs_s::geom::distance(click, QPoint(x2, y2)) <= tolerance) {
    lineState = State::Moving_End2;
    return true;
  }

  lineState = State::Idle;
  return false;
}

// Mouse move action during resize.
void GraphicLine::MouseResizeMoving(int x, int y, Schematic *p)
{
  switch (lineState) {
    case State::Moving_End1:
      x1 = x;
      y1 = y;
      break;
    case State::Moving_End2:
      x2 = x;
      y2 = y;
      break;
    default:
      return;
    }

  updateCenter();
  paintScheme(p);  // paint new painting
}

void GraphicLine::MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor)
{
  if (isBeingDrawn) {
    x2 = onGrid.x();
    y2 = onGrid.y();
    updateCenter();
    paintScheme(sch);
  } else {
    x1 = onGrid.x();
    y1 = onGrid.y();
    x2 = onGrid.x();
    y2 = onGrid.y();
  }

  sch->PostPaintEvent(_Line, cursor.x() + 27, cursor.y(), cursor.x() + 15, cursor.y() + 12, 0, 0, true);  // paint new cursor scursor.y()mbol
  sch->PostPaintEvent(_Line, cursor.x() + 25, cursor.y() - 2, cursor.x() + 29, cursor.y() + 2, 0, 0, true);
  sch->PostPaintEvent(_Line, cursor.x() + 13, cursor.y() + 10, cursor.x() + 17, cursor.y() + 14, 0, 0, true);
}

bool GraphicLine::MousePressing(Schematic*)
{
  if (isBeingDrawn) updateCenter();
  isBeingDrawn = !isBeingDrawn;
  return !isBeingDrawn;
}

// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool GraphicLine::getSelected(const QPoint& click, int tolerance)
{
  return qucs_s::geom::is_near_line(click, QPoint(x1, y1), QPoint(x2, y2), tolerance);
}

// Rotates around the center.
void GraphicLine::rotate() noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, cx, cy);
  qucs_s::geom::rotate_point_ccw(x2, y2, cx, cy);
  updateCenter();
}

void GraphicLine::rotate(int xc, int yc) noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, xc, yc);
  qucs_s::geom::rotate_point_ccw(x2, y2, xc, yc);
  updateCenter();
}

// Mirrors about center line.
void GraphicLine::mirrorX() noexcept
{
  std::swap(y1, y2);
}

// Mirrors about center line.
void GraphicLine::mirrorY() noexcept
{
  std::swap(x1, x2);
}

// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool GraphicLine::Dialog(QWidget *parent)
{
  auto dialog = std::make_unique<FillDialog>(
      QObject::tr("Edit Line Properties"), false, parent);

  misc::setPickerColor(dialog->ColorButt, pen.color());

  dialog->LineWidth->setText(QString::number(pen.width()));
  dialog->StyleBox->setCurrentIndex(pen.style() - 1);

  if (dialog->exec() == QDialog::Rejected) {
    return false;
  }

  bool changed = false;

  if (pen.color() != misc::getWidgetBackgroundColor(dialog->ColorButt)) {
    pen.setColor(misc::getWidgetBackgroundColor(dialog->ColorButt));
    changed = true;
  }

  if (pen.width() != dialog->LineWidth->text().toInt()) {
    pen.setWidth(dialog->LineWidth->text().toInt());
    changed = true;
  }

  if (pen.style() != (dialog->StyleBox->currentIndex() + 1)) {
    pen.setStyle((Qt::PenStyle)(dialog->StyleBox->currentIndex() + 1));
    changed = true;
  }

  return changed;
}
