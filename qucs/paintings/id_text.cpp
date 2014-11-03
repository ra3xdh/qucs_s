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
#include "schematic.h"

ID_Text::ID_Text(int cx_, int cy_)
{
  Name = ".ID ";
  isSelected = false;
  cx = cx_;
  cy = cy_;
  x2 = y2 = 20;

  Prefix = "SUB";
  Parameter.setAutoDelete(true);
}

ID_Text::~ID_Text()
{
}

// --------------------------------------------------------------------------
void ID_Text::paint(ViewPainter *p)
{
  int x, y;
  p->Painter->setPen(QPen(Qt::black,1));
  p->map(cx, cy, x, y);

  QRect r;
  p->Painter->drawText(x, y, 0, 0, Qt::TextDontClip, Prefix, -1, &r);
  x2 = r.width();
  y2 = p->LineSpacing;

  p->Painter->drawText(x, y+y2, 0, 0, Qt::TextDontClip, "File=name", -1, &r);
  if(x2 < r.width())  x2 = r.width();
  y2 += p->LineSpacing;

  SubParameter *pp;
  for(pp = Parameter.first(); pp != 0; pp = Parameter.next())
    if(pp->display) {
      p->Painter->drawText(x, y+y2, 0, 0, Qt::TextDontClip, pp->Name, -1, &r);
      if(x2 < r.width())  x2 = r.width();
      y2 += p->LineSpacing;
    }

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->Painter->drawRoundRect(x-4, y-4, x2+8, y2+8);
  }

  x2 = int(float(x2) / p->Scale);
  y2 = int(float(y2) / p->Scale);
}

// --------------------------------------------------------------------------
void ID_Text::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Rect, cx, cy, x2, y2);
}

// --------------------------------------------------------------------------
void ID_Text::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void ID_Text::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
bool ID_Text::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  Prefix = s.section(' ',3,3);    // Prefix
  if(Prefix.isEmpty()) return false;

  int i = 1;
  for(;;) {
    n = s.section('"', i,i);
    if(n.isEmpty())  break;

    Parameter.append(new SubParameter(
       (n.at(0) == '0') ? false : true,
       n.section('=', 1,2),
       n.section('=', 3,3),
       n.section('=', 4,4)));

    i += 2;
  }

  return true;
}

// --------------------------------------------------------------------------
QString ID_Text::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += Prefix;

  SubParameter *pp;
  for(pp = Parameter.first(); pp != 0; pp = Parameter.next()) {
    if(pp->display)  s += " \"1=";
    else  s += " \"0=";
    s += pp->Name + "=" + pp->Description + "=" + pp->Type + "\"";
  }

  return s;
}

// --------------------------------------------------------------------------
QString ID_Text::saveCpp()
{
  QString s =
    QString ("tx = %1; ty = %2;").
    arg(cx).arg(cy);
  return s;
}

QString ID_Text::saveJSON()
{
  QString s =  QString ("\"tx\" : %1,\n  \"ty\" : %2,").arg(cx).arg(cy);
  return s;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool ID_Text::getSelected(float fX, float fY, float)
{
  if(int(fX) < cx)  return false;
  if(int(fY) < cy)  return false;
  if(int(fX) > cx+x2)  return false;
  if(int(fY) > cy+y2)  return false;

  return true;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void ID_Text::rotate()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void ID_Text::mirrorX()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void ID_Text::mirrorY()
{
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool ID_Text::Dialog()
{
  ID_Dialog *d = new ID_Dialog(this);
  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  delete d;
  return true;
}
