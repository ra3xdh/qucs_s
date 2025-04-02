/***************************************************************************
                               ellipse.cpp
                              -------------
    begin                : Sun Nov 23 2003
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
#include "ellipse.h"
#include "filldialog.h"
#include "geometry.h"
#include "schematic.h"
#include "misc.h"

qucs::Ellipse::Ellipse(bool _filled)
{
  Name = "Ellipse ";
  isSelected = false;
  pen = QPen(QColor());
  brush = QBrush(Qt::lightGray);
  filled = _filled;
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

void qucs::Ellipse::paint(QPainter *painter) {
  painter->save();
  const auto bounds = boundingRect();

  painter->setPen(pen);
  if (filled) painter->setBrush(brush);

  painter->drawEllipse(bounds);

  if (isSelected) {
    painter->setPen(QPen(Qt::darkGray,pen.width() + 5));
    painter->drawEllipse(bounds);

    painter->setPen(QPen(Qt::white, pen.width(), pen.style()));
    painter->drawEllipse(bounds);

    misc::draw_resize_handle(painter, bounds.topLeft());
    misc::draw_resize_handle(painter, bounds.topRight());
    misc::draw_resize_handle(painter, bounds.bottomRight());
    misc::draw_resize_handle(painter, bounds.bottomLeft());
  }
  painter->restore();
}

void qucs::Ellipse::paintScheme(Schematic *p)
{
  const auto br = boundingRect();
  p->PostPaintEvent(_Ellipse, br.left(), br.top(), br.width(), br.height());
}

Painting* qucs::Ellipse::newOne()
{
  return new qucs::Ellipse();
}

Element* qucs::Ellipse::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Ellipse");
  BitmapFile = (char *) "ellipse";

  if (getNewOne) return new qucs::Ellipse();
  return nullptr;
}

Element* qucs::Ellipse::info_filled(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("filled Ellipse");
  BitmapFile = (char *) "filledellipse";

  if (getNewOne) return new qucs::Ellipse(true);
  return nullptr;
}

bool qucs::Ellipse::load(const QString& s)
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

  n  = s.section(' ',8,8);    // fill color
  co = misc::ColorFromString(n);
  brush.setColor(co);
  if(!brush.color().isValid()) return false;

  n  = s.section(' ',9,9);    // fill style
  brush.setStyle((Qt::BrushStyle)n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',10,10);    // filled
  if(n.toInt(&ok) == 0) filled = false;
  else filled = true;
  if(!ok) return false;

  return true;
}

QString qucs::Ellipse::save()
{
  QString s = Name +
	QString::number(x1) + " " + QString::number(y1) + " " +
	QString::number(x2 - x1) + " " + QString::number(y2 - y1) + " " +
	pen.color().name()  + " " + QString::number(pen.width()) + " " +
	QString::number(pen.style()) + " " +
	brush.color().name() + " " + QString::number(brush.style());
  if(filled) s += " 1";
  else s += " 0";
  return s;
}

QString qucs::Ellipse::saveCpp()
{
  QString b = filled ?
    QString (", QBrush (QColor (\"%1\"), %2)").
    arg(brush.color().name()).arg(toBrushString(brush.style())) : "";
  QString s =
    QString ("new Area (%1, %2, %3, %4, "
	     "QPen (QColor (\"%5\"), %6, %7)%8)").
    arg(x1).arg(y1).arg(x2 - x1).arg(y2 - y1).
    arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style())).
    arg(b);
  s = "Ellips.append (" + s + ");";
  return s;
}

QString qucs::Ellipse::saveJSON()
{
  QString b = filled ?
    QString ("\"colorfill\" : \"%1\", \"stylefill\" : \"%2\"").
    arg(brush.color().name()).arg(toBrushString(brush.style())) : "";
  QString s =
    QStringLiteral("{\"type\" : \"ellipse\", "
    "\"x\" : %1, \"y\" : %2, \"w\" : %3, \"h\" : %4,"
    "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\", %8},").
    arg(x1).arg(y1).arg(x2 - x1).arg(y2 - y1).
    arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style())).
    arg(b);
  return s;
}

// Checks if the resize area was clicked.
bool qucs::Ellipse::resizeTouched(const QPoint& click, int tolerance)
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
void qucs::Ellipse::MouseResizeMoving(int x, int y, Schematic* sch)
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
  paintScheme(sch);
}

void qucs::Ellipse::MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor)
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

  // paint cursor symbol
  sch->PostPaintEvent(_Ellipse, cursor.x() + 13, cursor.y(), 18, 12,0,0,true);
  if (filled) {
    sch->PostPaintEvent(_Line, cursor.x() + 14, cursor.y() + 7, cursor.x() + 20, cursor.y() + 1,0,0,true);
    sch->PostPaintEvent(_Line, cursor.x() + 25, cursor.y() + 2, cursor.x() + 18, cursor.y() + 9,0,0,true);
    sch->PostPaintEvent(_Line, cursor.x() + 29, cursor.y() + 4, cursor.x() + 23, cursor.y() + 10,0,0,true);
  }
}

bool qucs::Ellipse::MousePressing(Schematic*)
{
  if (isBeingDrawn) {
    updateCenter();
    normalize();
  }

  isBeingDrawn = !isBeingDrawn;
  return !isBeingDrawn;
}

bool qucs::Ellipse::getSelected(const QPoint& click, int tolerance)
{
  if (filled) {
    return qucs_s::geom::is_inside_ellipse(click, boundingRect(), tolerance);
  }

  return qucs_s::geom::is_near_ellipse(click, boundingRect(), tolerance);
}

bool qucs::Ellipse::rotate() noexcept
{
  // No effect for circles
  if (std::abs(x2 - x1) == std::abs(y2 - y1)) return false;

  qucs_s::geom::rotate_point_ccw(x1, y1, cx, cy);
  qucs_s::geom::rotate_point_ccw(x2, y2, cx, cy);
  updateCenter();
  return true;
}

bool qucs::Ellipse::rotate(int xc, int yc) noexcept
{
  if (xc == cx && yc == cy) return rotate();

  qucs_s::geom::rotate_point_ccw(x1, y1, xc, yc);
  qucs_s::geom::rotate_point_ccw(x2, y2, xc, yc);
  updateCenter();
  return true;
}

bool qucs::Ellipse::Dialog(QWidget* parent)
{
  bool changed = false;

  auto d = std::make_unique<FillDialog>(QObject::tr("Edit Ellipse Properties"), true, parent);
  misc::setPickerColor(d->ColorButt,pen.color());
  d->LineWidth->setText(QString::number(pen.width()));
  d->StyleBox->setCurrentIndex(pen.style()-1);
  misc::setPickerColor(d->FillColorButt,brush.color());
  d->FillStyleBox->setCurrentIndex(brush.style());
  d->CheckFilled->setChecked(filled);
  d->slotCheckFilled(filled);

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
  if (filled != d->CheckFilled->isChecked()) {
    filled = d->CheckFilled->isChecked();
    changed = true;
  }
  if (brush.color() != misc::getWidgetBackgroundColor(d->FillColorButt)) {
    brush.setColor(misc::getWidgetBackgroundColor(d->FillColorButt));
    changed = true;
  }
  if (brush.style()  != d->FillStyleBox->currentIndex()) {
    brush.setStyle((Qt::BrushStyle)d->FillStyleBox->currentIndex());
    changed = true;
  }

  return changed;
}
