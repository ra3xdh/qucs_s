/***************************************************************************
                                arrow.cpp
                               -----------
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

/*!
  \class Arrow
  \brief The Arrow class implements the arrow painting
*/

#include "arrow.h"
#include "arrowdialog.h"
#include "misc.h"
#include "multi_point.h"
#include "one_point.h"
#include "schematic.h"


Arrow::Arrow() : headStyle(ArrowHeadStyle::empty), headHeight(20.0), headWidth(8.0), headWingLength(sqrt(headWidth*headWidth + headHeight*headHeight)), headAngle(atan2(headWidth, headHeight))
{
  Name = "Arrow ";
  isSelected = false;
  pen = QPen(QColor());
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

void Arrow::paint(QPainter* painter) {
  painter->save();

  painter->setPen(isSelected ? QPen(Qt::darkGray,pen.width() + 5) : pen);

  // Schaft
  painter->drawLine(x1, y1, x2, y2);

  const QPoint arrowTip{x2, y2};

  switch (headStyle) {
    case ArrowHeadStyle::empty:
      painter->drawLine(arrowTip, headLeftWing);
      painter->drawLine(arrowTip, headRightWing);
      break;
    case ArrowHeadStyle::filled:
      painter->setBrush(isSelected ? Qt::white : pen.brush());
      painter->drawConvexPolygon(QPolygon(QList{headLeftWing, arrowTip, headRightWing}));
    break;
    default:
      assert(false);
  }

  if (isSelected) {
    misc::draw_resize_handle(painter, QPoint{x1, y1});
    misc::draw_resize_handle(painter, QPoint{x2, y2});
  }

  painter->restore();
}

void Arrow::paintScheme(Schematic *p)
{
  // Schaft
  p->PostPaintEvent(_Line, x1, y1, x2, y2, 0, 0, false);

  // Wings
  p->PostPaintEvent(_Line, x2, y2, headLeftWing.x(), headLeftWing.y(), 0, 0, false);
  p->PostPaintEvent(_Line, x2, y2, headRightWing.x(), headRightWing.y(), 0, 0, false);
}

Painting* Arrow::newOne()
{
  return new Arrow();
}

Element* Arrow::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Arrow");
  BitmapFile = (char *) "arrow";

  if (getNewOne) return new Arrow();
  return 0;
}

bool Arrow::load(const QString& s)
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

  n  = s.section(' ',5,5);    // height
  headHeight = n.toDouble(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // width
  headWidth = n.toDouble(&ok);
  if(!ok) return false;

  headAngle      = atan2(headWidth, headHeight);
  headWingLength = sqrt(headWidth*headWidth + headHeight*headHeight);
  updateHead();

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

  n  = s.section(' ',10,10);    // arrow style
  if(!n.isEmpty()) {            // backward compatible
    headStyle = static_cast<ArrowHeadStyle>(n.toInt(&ok));
    if(!ok) return false;
  }

  return true;
}

QString Arrow::save()
{
  QString s = Name+QString::number(x1)+" "+QString::number(y1)+" ";
  s += QString::number(x2 - x1)+" "+QString::number(y2 - y1)+" ";
  s += QString::number(static_cast<int>(headHeight))+" "+QString::number(static_cast<int>(headWidth))+" ";
  s += pen.color().name()+" "+QString::number(pen.width())+" ";
  s += QString::number(pen.style()) + " " + QString::number(headStyle);
  return s;
}

QString Arrow::saveCpp()
{
  // arrow not allowed in symbols, thus we use line here
  QString s =
    QString ("new Line (%1, %2, %3, %4, QPen (QColor (\"%5\"), %6, %7))").
    arg(x1).arg(y1).arg(x2 - x1).arg(y2 - y1).
    arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style()));
  s = "Lines.append (" + s + ");";
  return s;
}

QString Arrow::saveJSON()
{
  // arrow not allowed in symbols, thus we use line here
  QString s =
    QStringLiteral("{\"type\" : \"arrow\", "
       "\"x1\" : %1, \"y1\" : %2, \"x2\" : %3, \"y2\" : %4, "
       "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\"},").
       arg(x1).arg(y1).arg(x2 - x1).arg(y2 - y1).
       arg(pen.color().name()).arg(pen.width()).arg(toPenString(pen.style()));
  return s;
}

// Checks if the resize area was clicked.
bool Arrow::resizeTouched(const QPoint& click, int tolerance)
{
  if (qucs_s::geom::distance(click, QPoint(x1, y1)) <= tolerance) {
    arrowState = State::moving_tail;
    return true;
  }

  if (qucs_s::geom::distance(click, QPoint(x2, y2)) <= tolerance) {
    arrowState = State::moving_head;
    return true;
  }

  arrowState = State::idle;
  return false;
}

// Mouse move action during resize.
void Arrow::MouseResizeMoving(int x, int y, Schematic *p)
{
  switch (arrowState) {
    case State::moving_tail:
      x1 = x;
      y1 = y;
      break;
    case State::moving_head:
      x2 = x;
      y2 = y;
      break;
    default:
      return;
    }

  updateCenter();
  updateHead();
  paintScheme(p);  // paint new painting
}

void Arrow::updateHead() noexcept
{
  double arrow_angle  = atan2(double(y2 - y1), double(x2 - x1));

  double w = headAngle + arrow_angle;

  headLeftWing = QPoint{
    x2 - static_cast<int>(headWingLength * cos(w)),
    y2 - static_cast<int>(headWingLength * sin(w))
  };

  w = arrow_angle-headAngle;

  headRightWing = QPoint{
    x2 - static_cast<int>(headWingLength * cos(w)),
    y2 - static_cast<int>(headWingLength * sin(w))
  };
}

// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
void Arrow::MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor)
{
  if (isBeingDrawn) {
    x2 = onGrid.x();
    y2 = onGrid.y();
    updateCenter();
    updateHead();
    paintScheme(sch);
  } else {
    x1 = onGrid.x();
    y1 = onGrid.y();
    x2 = onGrid.x();
    y2 = onGrid.y();
  }

  // paint cursor scursor.y()mbol
  sch->PostPaintEvent(_Line, cursor.x() + 25, cursor.y(), cursor.x() + 13, cursor.y() + 12,0,0,true);
  sch->PostPaintEvent(_Line, cursor.x() + 18, cursor.y() + 2, cursor.x() + 25, cursor.y(),0,0,true);
  sch->PostPaintEvent(_Line, cursor.x() + 23, cursor.y() + 7, cursor.x() + 25, cursor.y(),0,0,true);
}

bool Arrow::MousePressing(Schematic*)
{
  if (isBeingDrawn) updateCenter();
  isBeingDrawn = !isBeingDrawn;
  return !isBeingDrawn;
}

bool Arrow::getSelected(const QPoint& click, int tolerance)
{
  const QPoint arrowTip(x2, y2);

  return
    qucs_s::geom::is_near_line(click, QPoint(x1, y1), arrowTip, tolerance) ||
    qucs_s::geom::is_near_line(click, headLeftWing, arrowTip, tolerance) ||
    qucs_s::geom::is_near_line(click, headRightWing, arrowTip, tolerance);
}

// Rotates around the center.
void Arrow::rotate() noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, cx, cy);
  qucs_s::geom::rotate_point_ccw(x2, y2, cx, cy);
  updateHead();
}

void Arrow::rotate(int xc, int yc) noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, xc, yc);
  qucs_s::geom::rotate_point_ccw(x2, y2, xc, yc);
  qucs_s::geom::rotate_point_ccw(cx, cy, xc, yc);
  updateHead();
}

// Mirrors about center line.
void Arrow::mirrorX() noexcept
{
  qucs_s::geom::mirror_coordinate(y1, cy);
  qucs_s::geom::mirror_coordinate(y2, cy);
  updateHead();
}

// Mirrors about center line.
void Arrow::mirrorY() noexcept
{
  qucs_s::geom::mirror_coordinate(x1, cx);
  qucs_s::geom::mirror_coordinate(x2, cx);
  updateHead();
}


// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Arrow::Dialog(QWidget *parent)
{
  bool changed = false;

  const auto d = std::make_unique<ArrowDialog>(parent);
  d->HeadWidth->setText(QString::number(headWidth));
  d->HeadLength->setText(QString::number(headHeight));

  QPalette palette;
  palette.setColor(d->ColorButt->backgroundRole(), pen.color());
  d->ColorButt->setPalette(palette);

  d->LineWidth->setText(QString::number(pen.width()));
  d->SetComboBox(pen.style());
  d->ArrowStyleBox->setCurrentIndex(headStyle);

  if (d->exec() == QDialog::Rejected) {
    return false;
  }

  if (headWidth != d->HeadWidth->text().toDouble()) {
    headWidth = d->HeadWidth->text().toDouble();
    changed = true;
  }
  if (headHeight != d->HeadLength->text().toDouble()) {
    headHeight = d->HeadLength->text().toDouble();
    changed = true;
  }
  if (pen.color() !=
      d->ColorButt->palette().color(d->ColorButt->backgroundRole())) {
    pen.setColor(d->ColorButt->palette().color(d->ColorButt->backgroundRole()));
    changed = true;
  }
  if (pen.width() != d->LineWidth->text().toInt()) {
    pen.setWidth(d->LineWidth->text().toInt());
    changed = true;
  }
  if (pen.style() != d->LineStyle) {
    pen.setStyle(d->LineStyle);
    changed = true;
  }
  if (headStyle != d->ArrowStyleBox->currentIndex()) {
    headStyle = static_cast<ArrowHeadStyle>(d->ArrowStyleBox->currentIndex());
    changed = true;
  }

  headAngle = atan2(double(headWidth), double(headHeight));
  headWingLength = sqrt(headWidth * headWidth + headHeight * headHeight);
  updateHead();

  return changed;
}
