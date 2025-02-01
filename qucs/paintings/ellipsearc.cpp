/***************************************************************************
                               ellipsearc.cpp
                              ----------------
    begin                : Thu Sep 9 2004
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
#include "ellipsearc.h"
#include "filldialog.h"
#include "geometry.h"
#include "schematic.h"
#include "misc.h"

#include <numbers>


EllipseArc::EllipseArc()
{
  Name = "EArc ";
  isSelected = false;
  pen = QPen(QColor());
  cx = cy = x1 = x2 = y1 = y2 = arcStartAngle = arcLengthAngle = 0;
}

void EllipseArc::paint(QPainter *painter) {
  painter->save();
  const auto bounds = boundingRect();
  painter->setPen(pen);

  painter->drawArc(bounds, arcStartAngle, arcLengthAngle);

  if (isSelected) {
    painter->setPen(QPen(Qt::darkGray,pen.width() + 5));
    painter->drawArc(bounds, arcStartAngle, arcLengthAngle);
    painter->setPen(QPen(Qt::white, pen.width(), pen.style()));
    painter->drawArc(bounds, arcStartAngle, arcLengthAngle);

    misc::draw_resize_handle(painter, bounds.topLeft());
    misc::draw_resize_handle(painter, bounds.topRight());
    misc::draw_resize_handle(painter, bounds.bottomRight());
    misc::draw_resize_handle(painter, bounds.bottomLeft());
  }
  painter->restore();
}

void EllipseArc::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Arc, x1, y1, x2 - x1, y2 - y1, arcStartAngle, arcLengthAngle);
}

Painting* EllipseArc::newOne()
{
  return new EllipseArc();
}

Element* EllipseArc::info(QString& name, char* &bitmapFile, bool getNewOne)
{
  name = QObject::tr("Elliptic Arc");
  bitmapFile = (char *) "ellipsearc";

  if (getNewOne) return new EllipseArc();
  return nullptr;
}

bool EllipseArc::load(const QString& s)
{
  bool ok;
  QString n;

  n  = s.section(' ',1,1);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // width
  auto w = n.toInt(&ok);
  if(!ok) return false;
  x2 = x1 + w;

  n  = s.section(' ',4,4);    // height
  auto h = n.toInt(&ok);
  if(!ok) return false;
  y2 = y1 + h;

  updateCenter();

  n  = s.section(' ',5,5);    // start angle
  arcStartAngle = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // arc length
  arcLengthAngle = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // color
  QColor co = misc::ColorFromString(n);
  pen.setColor(co);
  if(!pen.color().isValid()) return false;

  n  = s.section(' ',8,8);    // thickness
  pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',9,9);    // line style
  pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

QString EllipseArc::save()
{
  QString s = Name +
	QString::number(x1) + " " + QString::number(y1) + " " +
	QString::number(x2 - x1) + " " + QString::number(y2 - y1) + " " +
	QString::number(arcStartAngle) + " " + QString::number(arcLengthAngle) + " " +
	pen.color().name()  + " " + QString::number(pen.width()) + " " +
	QString::number(pen.style());
  return s;
}

QString EllipseArc::saveCpp()
{
  QString s =
    QString ("new Arc (%1, %2, %3, %4, %5, %6, "
	     "QPen (QColor (\"%7\"), %8, %9))").
    arg(x1).arg(y1).arg(x2 - x1).arg(y2 - y1).arg(arcStartAngle).arg(arcLengthAngle).
    arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style()));
  s = "Arcs.append (" + s + ");";
  return s;
}

QString EllipseArc::saveJSON()
{
  QString s =
    QString ("{\"type\" : \"ellipsearc\", "
      "\"x\" : %1, \"y\" : %2, \"w\" : %3, \"h\" : %4, "
      "\"angle\" : %5, \"arclen\" : %6, "
      "\"color\" : \"%7\", \"thick\" : %8, \"style\" : \"%9\"},").
      arg(x1).arg(y1).arg(x2 - x1).arg(y2 - y1).arg(arcStartAngle).arg(arcLengthAngle).
      arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style()));
  return s;
}

// Checks if the resize area was clicked.
bool EllipseArc::resizeTouched(const QPoint& click, int tolerance)
{
  using qucs_s::geom::distance;
  normalize();
  const auto bounds = boundingRect();

  if (distance(bounds.topLeft(), click) < tolerance) {
    resizeState = State::moving_top_left;
    return true;
  }

  if (distance(bounds.topRight(), click) < tolerance) {
    resizeState = State::moving_top_right;
    return true;
  }

  if (distance(bounds.bottomRight(), click) < tolerance) {
    resizeState = State::moving_bottom_right;
    return true;
  }

  if (distance(bounds.bottomLeft(), click) < tolerance) {
    resizeState = State::moving_bottom_left;
    return true;
  }

  resizeState = State::idle;
  return false;
}

// Mouse move action during resize.
void EllipseArc::MouseResizeMoving(int x, int y, Schematic *p)
{
  switch (resizeState) {
    case State::moving_top_left:
      x1 = x;
      y1 = y;
      break;
    case State::moving_top_right:
      x2 = x;
      y1 = y;
      break;
    case State::moving_bottom_right:
      x2 = x;
      y2 = y;
      break;
    case State::moving_bottom_left:
      x1 = x;
      y2 = y;
      break;
    default:
      return;
  }

  updateCenter();
  paintScheme(p);
}

namespace internal {
constexpr int qtDegreeCoef = 16;
constexpr int qtFullCircle = qtDegreeCoef * 360;
constexpr int qtHalfCircle = qtDegreeCoef * 180;
constexpr int qtQuarterCircle = qtDegreeCoef * 90;

inline double toDegrees(double radians) {
  return radians * 180 / std::numbers::pi;
}

int calculateQtAngle(const QPoint& origin, const QPoint& cursorPos)
{
  auto relCursorPos = cursorPos - origin;
  auto angle = toDegrees(std::atan2(relCursorPos.y(), relCursorPos.x()));
  return static_cast<int>(-1 * qtDegreeCoef * angle);
}

QPoint calculatePointOnEllipse(const QRect& ellipseBounds, const QPoint& cursorPos) {
  auto relCursorPos = cursorPos - ellipseBounds.center();
  auto angle = std::atan2(relCursorPos.y(), relCursorPos.x());
  return ellipseBounds.center() + QPoint(
    (ellipseBounds.width() / 2.0) * std::cos(angle),
    (ellipseBounds.height() / 2.0) * std::sin(angle)
  );
}
}

void EllipseArc::MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor)
{
  switch (drawingState) {
    case DrawingState::idle:
      x1 = onGrid.x();
      y1 = onGrid.y();
      break;
    case DrawingState::defining_arc_bounds:
      x2 = onGrid.x();
      y2 = onGrid.y();
      sch->PostPaintEvent(_Ellipse, x1, y1, x2 - x1, y2 - y1);
      break;
    case DrawingState::defining_arc_start: {
      arcStartAngle = internal::calculateQtAngle({cx, cy}, cursor);
      auto p = internal::calculatePointOnEllipse(boundingRect(), cursor);
      // draw full ellipse
      sch->PostPaintEvent(_Ellipse, x1, y1, x2 - x1, y2 - y1);
      // draw tiny circle where arc starts
      sch->PostPaintEvent(_Ellipse, p.x() - 3, p.y() - 3, 6, 6);
      break;
    }
    case DrawingState::defining_arc_length:
      arcLengthAngle = internal::calculateQtAngle({cx, cy}, cursor) - arcStartAngle;
      sch->PostPaintEvent(_Arc, x1, y1, x2 - x1, y2 - y1, arcStartAngle, arcLengthAngle);
      break;
    default:
      assert(false);
  }

  // paint cursor symbol
  sch->PostPaintEvent(_Arc, cursor.x() + 13, cursor.y(), 18, 12, 16*45, 16*200,true);
}

bool EllipseArc::MousePressing(Schematic*)
{
  switch (drawingState) {
    case DrawingState::idle:
      drawingState = DrawingState::defining_arc_bounds;
      break;
    case DrawingState::defining_arc_bounds:
      normalize();
      drawingState = DrawingState::defining_arc_start;
      break;
    case DrawingState::defining_arc_start:
      drawingState = DrawingState::defining_arc_length;
      break;
    case DrawingState::defining_arc_length:
      drawingState = DrawingState::idle;
      break;
    default:
      assert(false);
  }
  updateCenter();
  return drawingState == DrawingState::idle;
}

// Checks if the coordinates x/y point to the painting.
bool EllipseArc::getSelected(const QPoint& click, int tolerance)
{
  const auto bounds = boundingRect();

  auto a = internal::calculateQtAngle(bounds.center(), click);

  if (a < arcStartAngle && a > arcStartAngle + arcLengthAngle) {
    return false;
  }

  return qucs_s::geom::is_near_ellipse(click, bounds, tolerance);
}

// Rotates around the center.
void EllipseArc::rotate() noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, cx, cy);
  qucs_s::geom::rotate_point_ccw(x2, y2, cx, cy);
  updateCenter();

  arcStartAngle += internal::qtQuarterCircle;
  if (arcStartAngle >= internal::qtFullCircle) {
    arcStartAngle -= internal::qtFullCircle;
  }
}

// Mirrors about center line.
void EllipseArc::mirrorX() noexcept
{
  arcStartAngle += arcLengthAngle;

  if (arcStartAngle >= internal::qtFullCircle) {
    arcStartAngle -= internal::qtFullCircle;
  }

  if (arcStartAngle != 0) {
    arcStartAngle = internal::qtFullCircle - arcStartAngle;
  }
}

// Mirrors about center line.
void EllipseArc::mirrorY() noexcept
{
  arcStartAngle += arcLengthAngle;

  if (arcStartAngle >= internal::qtFullCircle) {
    arcStartAngle -= internal::qtFullCircle;
  }

  if (arcStartAngle <= internal::qtHalfCircle) {
    arcStartAngle = internal::qtHalfCircle - arcStartAngle;
  } else  {
    arcStartAngle = internal::qtFullCircle + internal::qtHalfCircle - arcStartAngle;
  }
}

// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool EllipseArc::Dialog(QWidget *parent)
{
  bool changed = false;

  auto d = std::make_unique<FillDialog>(QObject::tr("Edit Arc Properties"), false, parent);
  misc::setPickerColor(d->ColorButt,pen.color());
  d->LineWidth->setText(QString::number(pen.width()));
  d->StyleBox->setCurrentIndex(pen.style()-1);

  if (d->exec() == QDialog::Rejected) {
    return false;
  }

  if (pen.color() != misc::getWidgetBackgroundColor(d->ColorButt)) {
    pen.setColor(misc::getWidgetBackgroundColor(d->ColorButt));
    changed = true;
  }
  if (pen.width()  != d->LineWidth->text().toInt()) {
    pen.setWidth(d->LineWidth->text().toInt());
    changed = true;
  }
  if (pen.style()  != (Qt::PenStyle)(d->StyleBox->currentIndex()+1)) {
    pen.setStyle((Qt::PenStyle)(d->StyleBox->currentIndex()+1));
    changed = true;
  }

  return changed;
}
