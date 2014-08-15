/***************************************************************************
                              graphictext.cpp
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
#include <QtGui>
#include "main.h"
#include "mnemo.h"
#include "viewpainter.h"
#include "graphictext.h"
#include "graphictextdialog.h"
#include "schematic.h"
#include "qucs.h"
#include <QWidget>
#include <QPainter>
#include <QLineEdit>
#include <Q3TextEdit>
#include <QPushButton>

#include <math.h>


GraphicText::GraphicText()
{
  Name = "Text ";
  isSelected = false;
  Color = QColor(0,0,0);
  Font = QucsSettings.font;
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
  Angle = 0;
}

GraphicText::~GraphicText()
{
}

// -----------------------------------------------------------------------
void GraphicText::paint(ViewPainter *p)
{
  // keep track of painter state
  p->Painter->save();

  QMatrix wm = p->Painter->worldMatrix();
  QMatrix Mat(1.0, 0.0, 0.0, 1.0, p->DX + float(cx) * p->Scale,
				   p->DY + float(cy) * p->Scale);
  p->Painter->setWorldMatrix(Mat);
  p->Painter->rotate(-Angle);   // automatically enables transformation

  int Size = Font.pointSize();
  Font.setPointSizeFloat( p->FontScale * float(Size) );

  QFont f = p->Painter->font();
  p->Painter->setPen(Color);
  p->Painter->setFont(Font);

  // Because of a bug in Qt 3.1, drawing this text is dangerous, if it
  // contains linefeeds. Qt has problems with linefeeds. It remembers the
  // last font metrics (within the font ???) and does not calculate it again.
  // The error often appears at a very different drawText function !!!
  int w, h;
  w = p->drawTextMapped(Text, 0, 0, &h);

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->Painter->drawRect(-3, -2, w+6, h+5);
  }

  Font.setPointSize(Size);   // restore real font size
  p->Painter->setWorldMatrix(wm);
  p->Painter->setWorldXForm(false);

  // restore painter state
  p->Painter->restore();

  x2 = int(float(w) / p->Scale);
  y2 = int(float(h) / p->Scale);
  p->Painter->setFont(f);
}

// -----------------------------------------------------------------------
void GraphicText::paintScheme(Schematic *p)
{
  // FIXME #warning QMatrix wm = p->worldMatrix();
  // FIXME #warning QMatrix Mat (wm.m11(), 0.0, 0.0, wm.m22(),
// FIXME #warning 		wm.dx() + double(cx) * wm.m11(),
// FIXME #warning 		wm.dy() + double(cy) * wm.m22());
  // FIXME #warning p->setWorldMatrix(Mat);
  // FIXME #warning p->rotate(-Angle);
  p->PostPaintEvent(_Rect, 0, 0, x2, y2);

  // FIXME #warning p->setWorldMatrix(wm);
}

// ------------------------------------------------------------------------
void GraphicText::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// -----------------------------------------------------------------------
// Sets the center of the painting to x/y.
void GraphicText::setCenter(int x, int y, bool relative)
{
  if(relative) {  cx += x;  cy += y;  }
  else {  cx = x-(x2>>1);  cy = y-(y2>>1);  }
}

// -----------------------------------------------------------------------
Painting* GraphicText::newOne()
{
  return new GraphicText();
}

// --------------------------------------------------------------------------
Element* GraphicText::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Text");
  BitmapFile = (char *) "text";

  if(getNewOne)  return new GraphicText();
  return 0;
}

// -----------------------------------------------------------------------
bool GraphicText::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // Size
  Font.setPointSize(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',4,4);    // Color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  n  = s.section(' ',5,5);    // Angle
  Angle = n.toInt(&ok);
  if(!ok) return false;

  Text = s.mid(s.find('"')+1);    // Text (can contain " !!!)
  Text.truncate(Text.length()-1);
  if(Text.isEmpty()) return false;

  convert2Unicode(Text);
  QFontMetrics  metrics(((Schematic*)QucsMain->DocumentTab->currentPage())->font());   // get size of text
  QSize r = metrics.size(0, Text);    // get size of text
  x2 = r.width();
  y2 = r.height();

  return true;
}

// -----------------------------------------------------------------------
QString GraphicText::save()
{
  QString t = Text;
  convert2ASCII(t);

  // The 'Text' property has to be the last within the line !
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" "
		+ QString::number(Font.pointSize())+" "+Color.name()+" "
		+ QString::number(Angle) + " \""+t+"\"";
  return s;
}

// --------------------------------------------------------------------------
QString GraphicText::saveCpp()
{
  QString t = Text;
  convert2ASCII(t);

  QString s =
    QString ("new Text (%1, %2, \"%3\", QColor (\"%4\"), %5, %6, %7)").
    arg(cx).arg(cy).arg(t).
    arg(Color.name()).arg(Font.pointSize()).
    arg(cos(M_PI * Angle / 180.0)).arg(sin(M_PI * Angle / 180.0));
  s = "Texts.append (" + s + ");";
  return s;
}

QString GraphicText::saveJSON()
{
  QString t = Text;
  convert2ASCII(t);

  QString s =
    QString ("{\"type\" : \"graphictext\", "
      "\"x\" : %1, \"y\" : %2, \"s\" : \"%3\", "
      "\"color\" : \"%4\", \"size\" : %5, \"cos\" : %6, \"sin\" : %7},").
      arg(cx).arg(cy).arg(t).
      arg(Color.name()).arg(Font.pointSize()).
      arg(cos(M_PI * Angle / 180.0)).arg(sin(M_PI * Angle / 180.0));
  return s;
}

// -----------------------------------------------------------------------
// fx/fy are the precise coordinates, gx/gy are the coordinates set on grid.
// x/y are coordinates without scaling.
void GraphicText::MouseMoving(
	Schematic*, int, int, int gx, int gy,
	Schematic *p, int x, int y, bool drawn)
{
  // FIXME #warning p->setPen(Qt::SolidLine);
  if(drawn) {
    p->PostPaintEvent(_Line, x1+15, y1+15, x1+20, y1,0,0,true);  // erase old cursor symbol
    p->PostPaintEvent(_Line, x1+26, y1+15, x1+21, y1,0,0,true);
    p->PostPaintEvent(_Line, x1+17, y1+8,  x1+23, y1+8,0,0,true);
  }
  x1 = x;
  y1 = y;
  p->PostPaintEvent(_Line, x1+15, y1+15, x1+20, y1,0,0,true);  // paint new cursor symbol
  p->PostPaintEvent(_Line, x1+26, y1+15, x1+21, y1,0,0,true);
  p->PostPaintEvent(_Line, x1+17, y1+8,  x1+23, y1+8,0,0,true);

  cx = gx;
  cy = gy;
}

// ------------------------------------------------------------------------
bool GraphicText::MousePressing()
{
  return Dialog();
}

// ------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool GraphicText::getSelected(float fX, float fY, float)
{
  double phi  = M_PI/180.0*double(Angle);
  float  sine = sin(phi), cosine = cos(phi);

  fX -= float(cx);
  fY -= float(cy);
  int _x = int( fX*cosine - fY*sine );
  int _y = int( fY*cosine + fX*sine );

  if(_x >= 0) if(_y >= 0) if(_x <= x2) if(_y <= y2)
    return true;

  return false;
}

// ------------------------------------------------------------------------
void GraphicText::Bounding(int& xmin, int& ymin, int& xmax, int& ymax)
{
  double phi = M_PI/180.0*double(Angle);
  double sine = sin(phi), cosine = cos(phi);
  int dx = int( double(y2) * sine );
  int dy = int( double(y2) * cosine );
  xmin = dx;  xmax = cx;
  ymin = dy;  ymax = cy;
  if(xmin < 0)  xmin += cx;
  else { xmax += xmin;  xmin = cx; }
  if(ymin < 0)  ymin += cy;
  else { ymax += ymin;  ymin = cy; }

  int x = cx + int( double(x2) * cosine );
  if(xmax < x)  xmax = x;
  else if(xmin > x)  xmin = x;
  x += dx;
  if(xmax < x)  xmax = x;
  else if(xmin > x)  xmin = x;

  int y = cy - int( double(x2) * sine );
  if(ymax < y)  ymax = y;
  else if(ymin > y)  ymin = y;
  y += dy;
  if(ymax < y)  ymax = y;
  else if(ymin > y)  ymin = y;
}

// -----------------------------------------------------------------------
// Rotates around the center.
void GraphicText::rotate()
{
  Angle += 90;
  Angle %= 360;
  cx -= x2 >> 1;
  cy -= y2 >> 1;
}

// -----------------------------------------------------------------------
// Mirrors about center line.
void GraphicText::mirrorX()
{   // do not mirror, because unreadable
}

// -----------------------------------------------------------------------
// Mirrors about center line.
void GraphicText::mirrorY()
{   // do not mirror, because unreadable
}

// -----------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool GraphicText::Dialog()
{
  QFont f(QucsSettings.font);   // to avoid wrong text width
  bool changed = false;

  GraphicTextDialog *d = new GraphicTextDialog();
  d->ColorButt->setPaletteBackgroundColor(Color);
  d->TextSize->setText(QString::number(Font.pointSize()));
  d->Angle->setText(QString::number(Angle));
  QString _Text = Text;
  decode_String(_Text);  // replace special characters with LaTeX commands
  d->text->setText(_Text);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  if(Color != d->ColorButt->paletteBackgroundColor()) {
    Color = d->ColorButt->paletteBackgroundColor();
    changed = true;
  }
  f.setPointSize(d->TextSize->text().toInt());   // to avoid wrong text width
  if(Font.pointSize()  != d->TextSize->text().toInt()) {
    Font.setPointSize(d->TextSize->text().toInt());
    changed = true;
  }
  int tmp = d->Angle->text().toInt();
  if(Angle  != tmp) {
    Angle = tmp % 360;
    changed = true;
  }

  encode_String(d->text->text(), _Text);  // create special characters
  if(!_Text.isEmpty())
    if(_Text != Text) {
      Text = _Text;
      changed = true;
    }
  QFontMetrics  m(((Schematic*)QucsMain->DocumentTab->currentPage())->font());   // get size of text
// FIXME #warning is this the right way? it was this: QFontMetrics  m(f);
  QSize s = m.size(0, Text);    // get size of text
  x2 = s.width();
  y2 = s.height();

  delete d;
  return changed;
}
