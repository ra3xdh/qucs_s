/***************************************************************************
                                 element.h
                                -----------
    begin                : Sat Sep 20 2003
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

/** \file element.h
  * \brief Defines drawing elements for schematics
  *
  * element.h contains definitions of various drawing elements used
  * used to render schematics and the schematic symbols. The following
  * structs are defined to hold information on various drawing types:
  *
  *    Line
  *    Arc
  *    Area
  *    Port
  *    Text
  *    Property
  *
  * The Element class is also defined here which is a superclass
  * of every component symbol.
  *
  */

#ifndef ELEMENT_H
#define ELEMENT_H

#include <QPen>
#include <vector>

class Node;
class Schematic;

namespace qucs { // otherwise conflict with <windows.h>
                 // coming from Qt5 headers

class DrawingPrimitive {
public:
  virtual ~DrawingPrimitive() {}
  virtual void draw([[maybe_unused]] QPainter* painter) const {};
  virtual QBrush brushHint() const { return Qt::NoBrush; }
  virtual QPen penHint() const { return Qt::NoPen; }
};

struct Line : DrawingPrimitive {
  Line(double _x1, double _y1, double _x2, double _y2, QPen _style)
       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
  virtual ~Line() {}
  double   x1, y1, x2, y2;
  QPen  style;
  void draw(QPainter* painter) const override;
  QPen penHint() const override { return style; }
};

struct Arc : DrawingPrimitive {
  Arc(double _x, double _y, double _w, double _h, int _angle, int _arclen, QPen _style)
      : x(_x), y(_y), w(_w), h(_h), angle(_angle),
	arclen(_arclen), style(_style) {};
  virtual ~Arc() {}
  double   x, y, w, h;
  int angle, arclen;
  QPen  style;
  void draw(QPainter* painter) const override;
  QPen penHint() const override { return style; }
};

struct Rect : DrawingPrimitive {
  Rect(double _x, double _y, double _w, double _h, QPen _Pen,
	QBrush _Brush = QBrush(Qt::NoBrush))
	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
  double    x, y, w, h;
  QPen   Pen;
  QBrush Brush;    // filling style/color
  void draw(QPainter* painter) const override;
  QPen penHint() const override { return Pen; }
  QBrush brushHint() const override { return Brush; }
};

// 'ellipse' conflicts 'ellipse' defined in paintings.h in the same namespace
struct Ellips : DrawingPrimitive {
  Ellips(double _x, double _y, double _w, double _h, QPen _Pen,
	QBrush _Brush = QBrush(Qt::NoBrush))
	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
  double    x, y, w, h;
  QPen   Pen;
  QBrush Brush;    // filling style/color
  void draw(QPainter* painter) const override;
  QPen penHint() const override { return Pen; }
  QBrush brushHint() const override { return Brush; }
};

struct Polyline : DrawingPrimitive {
  std::vector<QPointF> points;
  QPen pen;
  QBrush brush;

  Polyline(const std::vector<QPointF> &pts, QPen p = QPen{Qt::NoPen}, QBrush b = QBrush{Qt::NoBrush})
    : points(pts), pen{p}, brush{b} {};

  void draw(QPainter* painter) const override;
  QPen penHint() const override { return pen; }
  QBrush brushHint() const override { return brush; }
};

}

struct Port {
  Port() {};
  Port(int _x, int _y, bool _avail=true) : x(_x), y(_y), avail(_avail) {
    Type=""; Connection=0;};
  int   x, y;
  bool  avail;
  QString Type;
  Node *Connection;
};

struct Text : qucs::DrawingPrimitive {
  Text(double _x, double _y, const QString& _s, QColor _Color = QColor(0,0,0),
	double _Size = 10.0, double _mCos=1.0, double _mSin=0.0)
	: x(_x), y(_y), s(_s), Color(_Color), Size(_Size),
	  mSin(_mSin), mCos(_mCos) { over = under = false; };
  virtual ~Text() {}
  double	  x, y;
  QString s;
  QColor  Color;
  double	  Size, mSin, mCos; // font size and rotation coefficients
  bool	  over, under;      // text attributes
  void draw(QPainter *painter) const override;
  void draw(QPainter* painter, QRectF* br) const;
  QPen penHint() const override { return Color; }
  double angle() const;
};

struct Property {
  Property(const QString& _Name="", const QString& _Value="",
	   bool _display=false, const QString& Desc="")
	 : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
  QString Name, Value;
  bool    display;   // show on schematic or not ?
  QString Description;
  QRect boundingRect() const { return br; };
  void paint(int x, int y, QPainter* p);
private:
  QRect br;
};


// valid values for Element.Type
// The 4 least significant bits of each value are reserved for special
// additionals !!!
#define isDummyElement    0
#define isSpecialMask    -16

#define isComponent        0x30000
#define isComponentText    0x30002
#define isAnalogComponent  0x10000
#define isDigitalComponent 0x20000

#define isGraph            0x0020
#define isNode             0x0040
#define isMarker           0x0080
#define isWire             0x0100

#define isPainting         0x2000
#define isPaintingResize   0x2001

#define isLabel            0x4000
#define isHWireLabel       0x4020
#define isVWireLabel       0x4040
#define isNodeLabel        0x4080
#define isMovingLabel      0x4001
#define isHMovingLabel     0x4002
#define isVMovingLabel     0x4004

#define isDiagram          0x8000
#define isDiagramResize    0x8001
#define isDiagramHScroll   0x8002
#define isDiagramVScroll   0x8003


/** \class Element
  * \brief Superclass of all schematic drawing elements
  *
  *
  */
class Element {
public:
  virtual ~Element() = default;
  virtual void paintScheme(Schematic *) { /* default no-op */ };

  /** Rotates element in-place*/
  virtual bool rotate() noexcept { /* default no-op */ return false; }

  /** Rotates element around point.
      Defined as
        1. Rotate coordinates of center and move element there
        2. Rotate element in-place
  */
  virtual bool rotate(int rcx, int rcy) noexcept;

  /** Overload of rotate around point */
  virtual bool rotate(const QPoint& center) noexcept;

  /** Mirrors element vertically in-place */
  virtual bool mirrorX() noexcept { /* default no-op */ return false; }

  /** Mirrors element horizontally in-place */
  virtual bool mirrorY() noexcept { /* default no-op */ return false; }

  /** Mirrors element vertically around axis.
      Defined as
        1. Mirror coordinate Y of center and move element there
        2. Mirror in-place
  */
  virtual bool mirrorX(int axis) noexcept;

  /** Mirrors element horizontally around axis.
      Defined as
        1. Mirror coordinate Y of center and move element there
        2. Mirror in-place
  */
  virtual bool mirrorY(int axis) noexcept;

  /** Moves elements so that its center is at (x,y) after move */
  virtual bool moveCenterTo(int x, int y) noexcept;

  /** Overload of moveCenterTo */
  virtual bool moveCenterTo(const QPoint& p) noexcept;

  /** Moves element center relatively to current location */
  virtual bool moveCenter(int dx, int dy) noexcept;

  /** Returns the tightest rectangle which can fit the entire element */
  virtual QRect boundingRect() const noexcept;

  /** Coordinates of center */
  virtual QPoint center() const noexcept;

  bool isSelected = false;
  int  Type = isDummyElement;    // whether it is Component, Wire, ...

  // center and relative boundings
  int cx = 0;
  int cy = 0;
  int x1 = 0;
  int y1 = 0;
  int x2 = 0;
  int y2 = 0;
};

#endif
