/***************************************************************************
                                symbolwidget.h
                               ----------------
    begin                : Sat May 29 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifndef SYMBOLWIDGET_H
#define SYMBOLWIDGET_H

#include <QWidget>
#include <QSize>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QString>
#include <QList>
#include <QMouseEvent>
#include <QPaintEvent>

#include "element.h"

class QPaintEvent;
class QSizePolicy;


//struct Line {
//  Line(int _x1, int _y1, int _x2, int _y2, QPen _style)
//       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
//  int   x1, y1, x2, y2;
//  QPen  style;
//};
//
//struct Arc {
//  Arc(int _x, int _y, int _w, int _h, int _angle, int _arclen, QPen _style)
//      : x(_x), y(_y), w(_w), h(_h), angle(_angle),
//	arclen(_arclen), style(_style) {};
//  int   x, y, w, h, angle, arclen;
//  QPen  style;
//};
//
//struct Area {
//  Area(int _x, int _y, int _w, int _h, QPen _Pen,
//	QBrush _Brush = QBrush(Qt::NoBrush))
//	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
//  int    x, y, w, h;
//  QPen   Pen;
//  QBrush Brush;    // filling style/color
//};
//
//struct Text {
//  Text(int _x, int _y, const QString& _s, QColor _Color = QColor(0,0,0),
//	float _Size = 10.0, float _mCos=1.0, float _mSin=0.0)
//	: x(_x), y(_y), s(_s), Color(_Color), Size(_Size) {};
//  int     x, y;
//  QString s;
//  QColor  Color;
//  float   Size;
//};



class SymbolWidget : public QWidget  {
   Q_OBJECT
public:
  SymbolWidget(QWidget *parent = 0);
 ~SymbolWidget();

  QString theModel();
  int setSymbol( QString&, const QString&, const QString&);
  int createSymbol(const QString&, const QString&);

  // component properties
  int Text_x, Text_y;
  QString Prefix, LibraryName, ComponentName;
  QString ModelString, VerilogModelString, VHDLModelString;

protected:
  void mouseMoveEvent(QMouseEvent*);

private:
  void  paintEvent(QPaintEvent*);

  int  analyseLine(const QString&);


  bool getPen  (const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  //Q3DragObject *myDragObject;


  QString PaintText, DragNDropText;
  int TextWidth, DragNDropWidth, TextHeight;
  int cx, cy, x1, x2, y1, y2;
  QList<Line *> Lines;
  QList<Arc *> Arcs;
  QList<Area *> Rects, Ellips;
  QList<Text *>  Texts;
  //Q3PtrList<Line>       Lines;
  //Q3PtrList<struct Arc> Arcs;
  //Q3PtrList<Area>       Rects, Ellips;
  //Q3PtrList<Text>       Texts;
};

#endif // SYMBOLWIDGET_H
