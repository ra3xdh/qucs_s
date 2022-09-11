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
#include "main.h"
#include "schematic.h"

#include <QPainter>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

#include "misc.h"

#include <cmath>

EllipseArc::EllipseArc()
{
  Name = "EArc ";
  isSelected = false;
  Pen = QPen(QColor());
  cx = cy = x1 = x2 = y1 = y2 = Angle = ArcLen = 0;
}

EllipseArc::~EllipseArc()
{
}

// --------------------------------------------------------------------------
void EllipseArc::paint(ViewPainter *p)
{
  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,Pen.width()+5));
    p->drawArc(cx, cy, x2, y2, Angle, ArcLen);
    p->Painter->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
    p->drawArc(cx, cy, x2, y2, Angle, ArcLen);

    p->Painter->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy+y2);  // markers for changing the size
    p->drawResizeRect(cx, cy);
    p->drawResizeRect(cx+x2, cy+y2);
    p->drawResizeRect(cx+x2, cy);
    return;
  }
  p->Painter->setPen(Pen);
  p->drawArc(cx, cy, x2, y2, Angle, ArcLen);
}

// --------------------------------------------------------------------------
void EllipseArc::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, ArcLen);
}

// --------------------------------------------------------------------------
void EllipseArc::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void EllipseArc::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
Painting* EllipseArc::newOne()
{
  return new EllipseArc();
}

// --------------------------------------------------------------------------
Element* EllipseArc::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Elliptic Arc");
  BitmapFile = (char *) "ellipsearc";

  if(getNewOne)  return new EllipseArc();
  return 0;
}

// --------------------------------------------------------------------------
bool EllipseArc::load(const QString& s)
{
  bool ok;
  QString n;

  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // x2
  x2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // y2
  y2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // start angle
  Angle = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // arc length
  ArcLen = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  n  = s.section(' ',8,8);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',9,9);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString EllipseArc::save()
{
  QString s = Name +
	QString::number(cx) + " " + QString::number(cy) + " " +
	QString::number(x2) + " " + QString::number(y2) + " " +
	QString::number(Angle) + " " + QString::number(ArcLen) + " " +
	Pen.color().name()  + " " + QString::number(Pen.width()) + " " +
	QString::number(Pen.style());
  return s;
}

// --------------------------------------------------------------------------
QString EllipseArc::saveCpp()
{
  QString s =
    QString ("new Arc (%1, %2, %3, %4, %5, %6, "
	     "QPen (QColor (\"%7\"), %8, %9))").
    arg(cx).arg(cy).arg(x2).arg(y2).arg(Angle).arg(ArcLen).
    arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
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
      arg(cx).arg(cy).arg(x2).arg(y2).arg(Angle).arg(ArcLen).
      arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
  return s;
}

// --------------------------------------------------------------------------
// Checks if the resize area was clicked.
bool EllipseArc::resizeTouched(float fX, float fY, float len)
{
  float fCX = float(cx), fCY = float(cy);
  float fX2 = float(cx+x2), fY2 = float(cy+y2);

  State = -1;
  if(fX < fCX-len) return false;
  if(fY < fCY-len) return false;
  if(fX > fX2+len) return false;
  if(fY > fY2+len) return false;

  State = 0;
  if(fX < fCX+len) State = 1;
  else if(fX <= fX2-len) { State = -1; return false; }
  if(fY < fCY+len)  State |= 2;
  else if(fY <= fY2-len) { State = -1; return false; }

  return true;
}

// --------------------------------------------------------------------------
// Mouse move action during resize.
void EllipseArc::MouseResizeMoving(int x, int y, Schematic *p)
{
  paintScheme(p);  // erase old painting
  switch(State) {
    case 0: x2 = x-cx; y2 = y-cy; // lower right corner
	    break;
    case 1: x2 -= x-cx; cx = x; y2 = y-cy; // lower left corner
	    break;
    case 2: x2 = x-cx; y2 -= y-cy; cy = y; // upper right corner
	    break;
    case 3: x2 -= x-cx; cx = x; y2 -= y-cy; cy = y; // upper left corner
	    break;
  }
  if(x2 < 0) { State ^= 1; x2 *= -1; cx -= x2; }
  if(y2 < 0) { State ^= 2; y2 *= -1; cy -= y2; }

  paintScheme(p);  // paint new painting
}

// --------------------------------------------------------------------------
// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
void EllipseArc::MouseMoving(
	Schematic *paintScale, int fx, int fy, int gx, int gy,
	Schematic *p, int x, int y, bool drawn)
{
  switch(State) {
    case 0 :
       x2 = gx;
       y2 = gy;
       break;
    case 1 :
      State++;
      x2 = gx - cx;
      y2 = gy - cy;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, 0, 16*360);  // paint new painting
      break;
    case 2 :
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, 0, 16*360);  // erase old painting
      x2 = gx - cx;
      y2 = gy - cy;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, 0, 16*360);  // paint new painting
      break;
    case 3 :
      State++;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, 0, 16*360);  // erase old painting
      if(x2 < 0) { cx += x2;  x2 *= -1; }
      if(y2 < 0) { cy += y2;  y2 *= -1; }

      Angle = int(16.0*180.0/pi
		* atan2(double(x2*(cy+(y2>>1) - fy)),
			double(y2*(fx - cx-(x2>>1)))));
      if(Angle < 0) Angle += 16*360;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, 16*180); // new painting
      break;
    case 4 :
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, 16*180);// erase old painting
      Angle = int(16.0*180.0/pi
		* atan2(double(x2*(cy+(y2>>1) - fy)),
			double(y2*(fx - cx-(x2>>1)))));
      if(Angle < 0) Angle += 16*360;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, 16*180);// paint new painting
      break;
    case 5 :
      State++;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, 16*180);// erase old painting
      ArcLen = int(16.0*180.0/pi
		* atan2(double(x2*(cy+(y2>>1) - fy)),
			double(y2*(fx - cx-(x2>>1)))));
      ArcLen -= Angle;
      while(ArcLen < 0) ArcLen += 16*360;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, ArcLen);// paint new painting
      break;
    case 6 :
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, ArcLen);// erase old painting
      ArcLen = int(16.0*180.0/pi
		* atan2(double(x2*(cy+(y2>>1) - fy)),
			double(y2*(fx - cx-(x2>>1)))));
      ArcLen -= Angle;
      while(ArcLen <= 32) ArcLen += 16*360;
      paintScale->PostPaintEvent(_Arc, cx, cy, x2, y2, Angle, ArcLen);// paint new painting
      break;
  }


  // FIXME #warning p->setPen(Qt::SolidLine);
  if(drawn)
    p->PostPaintEvent(_Arc, x1+13, y1, 18, 12, 16*45, 16*200,true); // erase old cursor symbol

  x1 = x;
  y1 = y;
  p->PostPaintEvent(_Arc, x1+13, y1, 18, 12, 16*45, 16*200,true);  // paint new cursor symbol
}

// --------------------------------------------------------------------------
bool EllipseArc::MousePressing()
{
  State++;
  switch(State) {
    case 1 :
	cx = x2;
	cy = y2;    // first corner is determined
	x2 = y2 = Angle = ArcLen = 0;
	break;
    case 7 :
	State = 0;
	return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool EllipseArc::getSelected(float fX, float fY, float w)
{
  float fX2 = float(x2)/2.0;
  float fY2 = float(y2)/2.0;
  fX -= float(cx) + fX2;
  fY -= float(cy) + fY2;

  int Phase =
      int(16.0*180.0/pi *
          atan2(-double(x2)*double(fY), double(y2)*double(fX)));
  Phase -= Angle;
  while(Phase < 0) Phase += 16*360;

  if(Phase > ArcLen)
    return false;

  float a1 = fX / (fX2 - w);  a1 *= a1;
  float a2 = fX / (fX2 + w);  a2 *= a2;
  float b1 = fY / (fY2 - w);  b1 *= b1;
  float b2 = fY / (fY2 + w);  b2 *= b2;

  if(a1+b1 < 1.0)  return false;
  if(a2+b2 > 1.0)  return false;

  return true;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void EllipseArc::rotate()
{
  cy += (y2-x2) >> 1;
  cx += (x2-y2) >> 1;
  int tmp = x2;
  x2 = y2;
  y2 = tmp;

  Angle += 16*90;
  if(Angle >= 16*360)  Angle -= 16*360;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void EllipseArc::mirrorX()
{
  Angle += ArcLen;
  if(Angle >= 16*360) Angle -= 16*360;

  if(Angle != 0)  Angle = 16*360 - Angle;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void EllipseArc::mirrorY()
{
  Angle += ArcLen;
  if(Angle >= 16*360) Angle -= 16*360;

  if(Angle <= 16*180)  Angle = 16*180 - Angle;
  else  Angle = 16*540 - Angle;
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool EllipseArc::Dialog()
{
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Arc Properties"), false);
  misc::setPickerColor(d->ColorButt,Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->StyleBox->setCurrentIndex(Pen.style()-1);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  if(Pen.color() != misc::getWidgetBackgroundColor(d->ColorButt)) {
    Pen.setColor(misc::getWidgetBackgroundColor(d->ColorButt));
    changed = true;
  }
  if(Pen.width()  != d->LineWidth->text().toInt()) {
    Pen.setWidth(d->LineWidth->text().toInt());
    changed = true;
  }
  if(Pen.style()  != (Qt::PenStyle)(d->StyleBox->currentIndex()+1)) {
    Pen.setStyle((Qt::PenStyle)(d->StyleBox->currentIndex()+1));
    changed = true;
  }

  delete d;
  return changed;
}
