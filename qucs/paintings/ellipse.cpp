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
#include <QtGui>
#include "ellipse.h"
#include "filldialog.h"
#include "schematic.h"
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>


Ellipse::Ellipse(bool _filled)
{
  Name = "Ellipse ";
  isSelected = false;
  Pen = QPen(QColor());
  Brush = QBrush(Qt::lightGray);
  filled = _filled;
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

Ellipse::~Ellipse()
{
}

// --------------------------------------------------------------------------
void Ellipse::paint(ViewPainter *p)
{
  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,Pen.width()+5));
    if(filled)  p->Painter->setBrush(Brush);
    p->drawEllipse(cx, cy, x2, y2);
    p->Painter->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
    p->Painter->setBrush(Qt::NoBrush);
    p->drawEllipse(cx, cy, x2, y2);

    p->Painter->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy+y2);  // markers for changing the size
    p->drawResizeRect(cx, cy);
    p->drawResizeRect(cx+x2, cy+y2);
    p->drawResizeRect(cx+x2, cy);
    return;
  }
  p->Painter->setPen(Pen);
  if(filled)  p->Painter->setBrush(Brush);
  p->drawEllipse(cx, cy, x2, y2);
  p->Painter->setBrush(Qt::NoBrush); // no filling for the next paintings
}

// --------------------------------------------------------------------------
void Ellipse::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Ellipse, cx, cy, x2, y2);
}

// --------------------------------------------------------------------------
void Ellipse::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void Ellipse::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
Painting* Ellipse::newOne()
{
  return new Ellipse();
}

// --------------------------------------------------------------------------
Element* Ellipse::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Ellipse");
  BitmapFile = (char *) "ellipse";

  if(getNewOne)  return new Ellipse();
  return 0;
}

// --------------------------------------------------------------------------
Element* Ellipse::info_filled(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("filled Ellipse");
  BitmapFile = (char *) "filledellipse";

  if(getNewOne)  return new Ellipse(true);
  return 0;
}

// --------------------------------------------------------------------------
bool Ellipse::load(const QString& s)
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

  n  = s.section(' ',5,5);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  n  = s.section(' ',6,6);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',7,7);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',8,8);    // fill color
  co.setNamedColor(n);
  Brush.setColor(co);
  if(!Brush.color().isValid()) return false;

  n  = s.section(' ',9,9);    // fill style
  Brush.setStyle((Qt::BrushStyle)n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',10,10);    // filled
  if(n.toInt(&ok) == 0) filled = false;
  else filled = true;
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString Ellipse::save()
{
  QString s = Name +
	QString::number(cx) + " " + QString::number(cy) + " " +
	QString::number(x2) + " " + QString::number(y2) + " " +
	Pen.color().name()  + " " + QString::number(Pen.width()) + " " +
	QString::number(Pen.style()) + " " +
	Brush.color().name() + " " + QString::number(Brush.style());
  if(filled) s += " 1";
  else s += " 0";
  return s;
}

// --------------------------------------------------------------------------
QString Ellipse::saveCpp()
{
  QString b = filled ?
    QString (", QBrush (QColor (\"%1\"), %2)").
    arg(Brush.color().name()).arg(toBrushString(Brush.style())) : "";
  QString s = 
    QString ("new Area (%1, %2, %3, %4, "
	     "QPen (QColor (\"%5\"), %6, %7)%8)").
    arg(cx).arg(cy).arg(x2).arg(y2).
    arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style())).
    arg(b);
  s = "Ellips.append (" + s + ");";
  return s;
}

// --------------------------------------------------------------------------
// Checks if the resize area was clicked.
bool Ellipse::resizeTouched(float fX, float fY, float len)
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
void Ellipse::MouseResizeMoving(int x, int y, Schematic *p)
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
void Ellipse::MouseMoving(
	Schematic *paintScale, int, int, int gx, int gy,
	Schematic *p, int x, int y, bool drawn)
{
  if(State > 0) {
    if(State > 1)
      paintScale->PostPaintEvent(_Ellipse, x1, y1, x2-x1, y2-y1); // erase old painting
    State++;
    x2 = gx;
    y2 = gy;
    paintScale->PostPaintEvent(_Ellipse, x1, y1, x2-x1, y2-y1);  // paint new painting
  }
  else { x2 = gx; y2 = gy; }


  #warning p->setPen(Qt::SolidLine);
  if(drawn) {
    p->PostPaintEvent(_Ellipse, cx+13, cy, 18, 12);  // erase old cursor symbol
    if(filled) {
      p->PostPaintEvent(_Line, cx+14, cy+7, cx+20, cy+1);
      p->PostPaintEvent(_Line, cx+25, cy+2, cx+18, cy+9);
      p->PostPaintEvent(_Line, cx+29, cy+4, cx+23, cy+10);
    }
  }
  cx = x;
  cy = y;
  p->PostPaintEvent(_Ellipse, cx+13, cy, 18, 12);  // paint new cursor symbol
  if(filled) {
    p->PostPaintEvent(_Line, cx+14, cy+7, cx+20, cy+1);
    p->PostPaintEvent(_Line, cx+25, cy+2, cx+18, cy+9);
    p->PostPaintEvent(_Line, cx+29, cy+4, cx+23, cy+10);
  }
}

// --------------------------------------------------------------------------
bool Ellipse::MousePressing()
{
  State++;
  if(State == 1) {
    x1 = x2;
    y1 = y2;    // first corner is determined
  }
  else {
    if(x1 < x2) { cx = x1; x2 = x2-x1; } // cx/cy to upper left corner
    else { cx = x2; x2 = x1-x2; }
    if(y1 < y2) { cy = y1; y2 = y2-y1; }
    else { cy = y2; y2 = y1-y2; }
    x1 = y1 = 0;
    State = 0;
    return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool Ellipse::getSelected(float fX, float fY, float w)
{
  float fX2 = float(x2);
  float fY2 = float(y2);
  fX -= float(cx) + fX2/2.0;
  fY -= float(cy) + fY2/2.0;

  if(filled) {
    float a = 2.0 * fX / fX2;  a *= a;
    float b = 2.0 * fY / fY2;  b *= b;

    if(a+b > 1.0)
      return false;
  }
  else {
    float a1 = fX / (fX2/2.0 - w);  a1 *= a1;
    float a2 = fX / (fX2/2.0 + w);  a2 *= a2;
    float b1 = fY / (fY2/2.0 - w);  b1 *= b1;
    float b2 = fY / (fY2/2.0 + w);  b2 *= b2;

    if(a1+b1 < 1.0)  return false;
    if(a2+b2 > 1.0)  return false;
  }

  return true;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void Ellipse::rotate()
{
  cy += (y2-x2) >> 1;
  cx += (x2-y2) >> 1;
  int tmp = x2;
  x2 = y2;
  y2 = tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Ellipse::mirrorX()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Ellipse::mirrorY()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Ellipse::Dialog()
{
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Ellipse Properties"));
  d->ColorButt->setPaletteBackgroundColor(Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->StyleBox->setCurrentItem(Pen.style()-1);
  d->FillColorButt->setPaletteBackgroundColor(Brush.color());
  d->FillStyleBox->setCurrentItem(Brush.style());
  d->CheckFilled->setChecked(filled);
  d->slotCheckFilled(filled);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  if(Pen.color() != d->ColorButt->paletteBackgroundColor()) {
    Pen.setColor(d->ColorButt->paletteBackgroundColor());
    changed = true;
  }
  if(Pen.width()  != d->LineWidth->text().toUInt()) {
    Pen.setWidth(d->LineWidth->text().toUInt());
    changed = true;
  }
  if(Pen.style()  != (Qt::PenStyle)(d->StyleBox->currentItem()+1)) {
    Pen.setStyle((Qt::PenStyle)(d->StyleBox->currentItem()+1));
    changed = true;
  }
  if(filled != d->CheckFilled->isChecked()) {
    filled = d->CheckFilled->isChecked();
    changed = true;
  }
  if(Brush.color() != d->FillColorButt->paletteBackgroundColor()) {
    Brush.setColor(d->FillColorButt->paletteBackgroundColor());
    changed = true;
  }
  if(Brush.style()  != d->FillStyleBox->currentItem()) {
    Brush.setStyle((Qt::BrushStyle)d->FillStyleBox->currentItem());
    changed = true;
  }

  delete d;
  return changed;
}
