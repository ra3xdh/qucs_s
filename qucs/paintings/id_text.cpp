/***************************************************************************
                               id_text.cpp
                              -------------
    begin                : Thu Oct 14 2004
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
#include "id_text.h"
#include "id_dialog.h"
#include "one_point.h"
#include "schematic.h"

#include <QPainter>

ID_Text::ID_Text(int x1_, int y1_)
{
  Name = ".ID ";
  isSelected = false;
  x1 = x1_;
  y1 = y1_;
  x2 = y2 = 20;

  prefix = "SUB";
}

void ID_Text::paint(QPainter* painter) {
  painter->save();
  painter->translate(x1, y1);

  painter->setPen(QPen(Qt::black,1));

  QRect r;
  painter->drawText(QRect(0, 0, 1, 1), Qt::TextDontClip, prefix, &r);
  x2 = r.width();
  y2 = r.height();

  painter->drawText(QRect(0, y2, 1, 1), Qt::TextDontClip, "File=name", &r);
  x2 = std::max(x2, r.width());
  y2 += r.height();

  for (const auto& sub_param : subParameters) {
    if (sub_param->display) {
      painter->drawText(QRect(0, y2, 1, 1), Qt::TextDontClip, sub_param->name, &r);
      x2 = std::max(x2, r.width());
      y2 += r.height();
    }
  }

  if(isSelected) {
    painter->setPen(QPen(Qt::darkGray,3));
    painter->drawRoundedRect(-4, -4, x2+8, y2+8, 4.0, 4.0);
  }
  painter->restore();
  updateCenter();
}

void ID_Text::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Rect, x1, y1, x2, y2);
}

bool ID_Text::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // x1
  x1 = n.toInt(&ok);
  if (!ok) return false;

  n  = s.section(' ',2,2);    // y1
  y1 = n.toInt(&ok);
  if (!ok) return false;

  prefix = s.section(' ',3,3);    // Prefix
  if( prefix.isEmpty()) return false;

  int i = 1;
  for(;;) {
    n = s.section('"', i,i);
    if(n.isEmpty())  break;

    subParameters.push_back(std::make_unique<SubParameter>(
       n.at(0) != '0',
       n.section('=', 1,2),
       n.section('=', 3,3),
       n.section('=', 4,4)));

    i += 2;
  }

  return true;
}

QString ID_Text::save()
{
  QString s = Name+QString::number(x1)+" "+QString::number(y1)+" ";
  s += prefix;

  for (const auto& sub_param : subParameters) {
    s += (sub_param->display ? " \"1=" : " \"0=");
    s += sub_param->name + "=" + sub_param->description + "=" + sub_param->type + "\"";
  }

  return s;
}

QString ID_Text::saveCpp()
{
  QString s =
    QString ("tx = %1; ty = %2;").
    arg(x1).arg(y1);
  return s;
}

QString ID_Text::saveJSON()
{
  QString s =  QString ("\"tx\" : %1,\n  \"ty\" : %2,").arg(x1).arg(y1);
  return s;
}

// Checks if the coordinates x/y point to the painting.
bool ID_Text::getSelected(const QPoint& click, int tolerance)
{
  return boundingRect()
      .marginsAdded(QMargins(tolerance, tolerance, tolerance, tolerance))
      .contains(click);
}

bool ID_Text::rotate(int rcx, int rcy) noexcept
{
  qucs_s::geom::rotate_point_ccw(x1, y1, rcx, rcy);
  qucs_s::geom::rotate_point_ccw(x2, y2, rcx, rcy);
  updateCenter();
  return true;
}

// If there were changes, it returns 'true'.
bool ID_Text::Dialog(QWidget *parent)
{
  auto d = std::make_unique<ID_Dialog>(this, parent);
  return d->exec() != QDialog::Rejected;
}
