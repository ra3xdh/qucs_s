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
#include "schematic.h"

#include <QPainter>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

#include "misc.h"

GraphicLine::GraphicLine(int cx_, int cy_, int x2_, int y2_, QPen Pen_)
{
  Name = "Line ";
  isSelected = false;
  Pen = Pen_;
  cx = cx_;
  cy = cy_;
  x1 = y1 = 0;
  x2 = x2_;
  y2 = y2_;
}

GraphicLine::~GraphicLine()
{
}

// --------------------------------------------------------------------------
void GraphicLine::paint(ViewPainter *p)
{
  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,Pen.width()+5));
    p->drawLine(cx, cy, cx+x2, cy+y2);
    p->Painter->setPen(QPen(Qt::white, Pen.width(), Pen.style()));
    p->drawLine(cx, cy, cx+x2, cy+y2);

    p->Painter->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy);  // markers for changing the size
    p->drawResizeRect(cx+x2, cy+y2);
    return;
  }
  p->Painter->setPen(Pen);
  p->drawLine(cx, cy, cx+x2, cy+y2);
}

// --------------------------------------------------------------------------
void GraphicLine::paintScheme(Schematic *p)
{
  p->PostPaintEvent(_Line, cx, cy, cx+x2, cy+y2);
}

// --------------------------------------------------------------------------
void GraphicLine::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void GraphicLine::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
Painting* GraphicLine::newOne()
{
  return new GraphicLine();
}

// --------------------------------------------------------------------------
Element* GraphicLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Line");
  BitmapFile = (char *) "line";

  if(getNewOne)  return new GraphicLine();
  return 0;
}

// --------------------------------------------------------------------------
bool GraphicLine::load(const QString& s)
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
  co.fromString(n);
#else
  co.setNamedColor(n);
#endif
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  n  = s.section(' ',6,6);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',7,7);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString GraphicLine::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  s += Pen.color().name()+" "+QString::number(Pen.width())+" ";
  s += QString::number(Pen.style());
  return s;
}

// --------------------------------------------------------------------------
QString GraphicLine::saveCpp()
{
  QString s =
    QString ("new Line (%1, %2, %3, %4, QPen (QColor (\"%5\"), %6, %7))").
    arg(cx+x1).arg(cy+y1).arg(cx+x2).arg(cy+y2).
    arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
  s = "Lines.append (" + s + ");";
  return s;
}

QString GraphicLine::saveJSON()
{
  QString s =
    QString ("{\"type\" : \"line\", "
      "\"x1\" : %1, \"y1\" : %2, \"x2\" : %3, \"y2\" : %4, "
      "\"color\" : \"%5\", \"thick\" : %6, \"style\" : \"%7\"},").
      arg(cx+x1).arg(cy+y1).arg(cx+x2).arg(cy+y2).
      arg(Pen.color().name()).arg(Pen.width()).arg(toPenString(Pen.style()));
  return s;
}

// --------------------------------------------------------------------------
// Checks if the resize area was clicked.
bool GraphicLine::resizeTouched(float fX, float fY, float len)
{
  float fCX = float(cx), fCY = float(cy);

  if(fX <= fCX+len) if(fX >= fCX-len) if(fY <= fCY+len) if(fY >= fCY-len) {
    State = 1;
    return true;
  }

  fCX += float(x2);
  fCY += float(y2);
  if(fX <= fCX+len) if(fX >= fCX-len) if(fY <= fCY+len) if(fY >= fCY-len) {
    State = 2;
    return true;
  }

  State = 0;
  return false;
}

// --------------------------------------------------------------------------
// Mouse move action during resize.
void GraphicLine::MouseResizeMoving(int x, int y, Schematic *p)
{
  paintScheme(p);  // erase old painting
  if(State == 1) { x2 += cx-x; y2 += cy-y; cx = x; cy = y; } // move beginning
  else { x2 = x-cx;  y2 = y-cy; }  // move ending

  paintScheme(p);  // paint new painting
}

// --------------------------------------------------------------------------
// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
void GraphicLine::MouseMoving(
	Schematic *paintScale, int, int, int gx, int gy,
	Schematic *p, int x, int y, bool drawn)
{
  if(State > 0) {
    if(State > 1)
      paintScale->PostPaintEvent(_Line, cx, cy, cx+x2, cy+y2);  // erase old painting
    State++;
    x2 = gx-cx;
    y2 = gy-cy;
    paintScale->PostPaintEvent(_Line, cx, cy, cx+x2, cy+y2);  // paint new painting
  }
  else { cx = gx; cy = gy; }


  // FIXME #warning p->setPen(Qt::SolidLine);
  if(drawn) {
    p->PostPaintEvent(_Line, x1+27, y1, x1+15, y1+12,0,0,true);  // erase old cursor symbol
    p->PostPaintEvent(_Line, x1+25, y1-2, x1+29, y1+2,0,0,true);
    p->PostPaintEvent(_Line, x1+13, y1+10, x1+17, y1+14,0,0,true);
  }
  x1 = x;
  y1 = y;
  p->PostPaintEvent(_Line, x1+27, y1, x1+15, y1+12,0,0,true);  // paint new cursor symbol
  p->PostPaintEvent(_Line, x1+25, y1-2, x1+29, y1+2,0,0,true);
  p->PostPaintEvent(_Line, x1+13, y1+10, x1+17, y1+14,0,0,true);
}

// --------------------------------------------------------------------------
bool GraphicLine::MousePressing()
{
  State++;
  if(State > 2) {
    x1 = y1 = 0;
    State = 0;
    return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool GraphicLine::getSelected(float fX, float fY, float w)
{
  fX -= float(cx);
  fY -= float(cy);

  if(fX < -w) {
    if(fX < float(x2)-w)  // is point between x coordinates ?
      return false;
  }
  else {
    if(fX > w)
      if(fX > float(x2)+w)
        return false;
  }

  if(fY < -w) {
    if(fY < float(y2)-w)   // is point between y coordinates ?
      return false;
  }
  else {
    if(fY > w)
      if(fY > float(y2)+w)
        return false;
  }

  float A = float(x2)*fY - fX*float(y2); // calculate the rectangle area spanned
  A *= A;               // avoid the need for square root

  if(A <= w*w*float(x2*x2 + y2*y2))
    return true;     // x/y lies on the graph line

  return false;
}

// --------------------------------------------------------------------------
void GraphicLine::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(x2 < 0) { _x1 = cx+x2; _x2 = cx; }
  else { _x1 = cx; _x2 = cx+x2; }

  if(y2 < 0) { _y1 = cy+y2; _y2 = cy; }
  else { _y1 = cy; _y2 = cy+y2; }
}

// --------------------------------------------------------------------------
// Rotates around the center.
void GraphicLine::rotate()
{
  cx += (x2>>1) - (y2>>1);
  cy += (x2>>1) + (y2>>1);

  int tmp = x2;
  x2  =  y2;
  y2  = -tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicLine::mirrorX()
{
  cy +=  y2;
  y2  = -y2;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicLine::mirrorY()
{
  cx +=  x2;
  x2  = -x2;
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool GraphicLine::Dialog()
{
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Line Properties"), false);
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
  if(Pen.style()  != (d->StyleBox->currentIndex()+1)) {
    Pen.setStyle((Qt::PenStyle)(d->StyleBox->currentIndex()+1));
    changed = true;
  }

  delete d;
  return changed;
}
