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

#include "viewpainter.h"
#include <QPainter>
#include <QPen>
#include <QBrush>

class Node;
class WireLabel;
class Schematic;

namespace qucs { // otherwise conflict with <windows.h>
                 // coming from Qt5 headers

class DrawingPrimitive {
public:
  /**
     Draws a primitive using given @c ViewPainter

     Params @c cx and @c cy define offset on corresponding axis. Actually, the
     proper way to offset primitive coordinates is to use "translate" mechanism
     of QPainter, but historically Qucs-S codebase doesn't use it, relying
     instead on manual control of offsets. I think this something what should be
     refactored; lines, ellipses, etc. should not be aware of any "offsets".

         Boolean switch @c y_grows_up tells whether Y-axis coordinates of
     primitive should be treated as if they are defined in space where the
     Y-axis directed upwards or downwards. Painting devices like QPainter or
     ViewPainter always operate in space where Y-axis directed downwards, but
     drawing primitives maybe defined with upwards Y-axis in mind, so to draw
     them correctly this switch must be used. Inheriting implementations MUST
     take this switch into account and alter their drawing logic accordingly.
         This is actually a workaround for a couple of cases. Diagram and some
     of its subclasses define a handful of primitives in upwards Y-axis space
     (I believe it's easier to reason about lines, rectangles, etc. that way when
     you drawing a diagram). It should be refactored in future: that's a painting
     device liability change orientation, calculate offsets, etc. Drawing
     primitives should concentrate on drawing only.

     @param cx offset on X-axis
     @param cy offset on Y-axis
     @param y_grows_up Y-axis orientation switch
  */
  virtual void draw([[maybe_unused]] ViewPainter* painter, [[maybe_unused]] int cx, [[maybe_unused]] int cy, [[maybe_unused]] bool y_grows_up=false) const {};
  /**
     The same as the version with @c ViewPainter, but with @c QPainter
  */
  virtual void draw([[maybe_unused]] QPainter* painter, [[maybe_unused]] int cx, [[maybe_unused]] int cy, [[maybe_unused]] bool y_grows_up=false) const {};
  virtual QBrush brushHint() const { return Qt::NoBrush; }
  virtual QPen penHint() const { return Qt::NoPen; }
};

struct Line : DrawingPrimitive {
  Line(int _x1, int _y1, int _x2, int _y2, QPen _style)
       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
  int   x1, y1, x2, y2;
  QPen  style;
  void draw(ViewPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  void draw(QPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  QPen penHint() const override { return style; }
};

struct Arc : DrawingPrimitive {
  Arc(int _x, int _y, int _w, int _h, int _angle, int _arclen, QPen _style)
      : x(_x), y(_y), w(_w), h(_h), angle(_angle),
	arclen(_arclen), style(_style) {};
  int   x, y, w, h, angle, arclen;
  QPen  style;
  void draw(ViewPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  void draw(QPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  QPen penHint() const override { return style; }
};

struct Rect : DrawingPrimitive {
  Rect(int _x, int _y, int _w, int _h, QPen _Pen,
	QBrush _Brush = QBrush(Qt::NoBrush))
	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
  int    x, y, w, h;
  QPen   Pen;
  QBrush Brush;    // filling style/color
  void draw(ViewPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  void draw(QPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  QPen penHint() const override { return Pen; }
  QBrush brushHint() const override { return Brush; }
};

// 'ellipse' conflicts 'ellipse' defined in paintings.h in the same namespace
struct Ellips : DrawingPrimitive {
  Ellips(int _x, int _y, int _w, int _h, QPen _Pen,
	QBrush _Brush = QBrush(Qt::NoBrush))
	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
  int    x, y, w, h;
  QPen   Pen;
  QBrush Brush;    // filling style/color
  void draw(ViewPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  void draw(QPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  QPen penHint() const override { return Pen; }
  QBrush brushHint() const override { return Brush; }
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
  Text(int _x, int _y, const QString& _s, QColor _Color = QColor(0,0,0),
	float _Size = 10.0, float _mCos=1.0, float _mSin=0.0)
	: x(_x), y(_y), s(_s), Color(_Color), Size(_Size),
	  mSin(_mSin), mCos(_mCos) { over = under = false; };
  int	  x, y;
  QString s;
  QColor  Color;
  float	  Size, mSin, mCos; // font size and rotation coefficients
  bool	  over, under;      // text attributes
  void draw(ViewPainter* painter, int cx, int cy, bool y_grows_up=false) const override;
  QPen penHint() const override { return Color; }
};

struct Property {
  Property(const QString& _Name="", const QString& _Value="",
	   bool _display=false, const QString& Desc="")
	 : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
  QString Name, Value;
  bool    display;   // show on schematic or not ?
  QString Description;
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
  Element();
  virtual ~Element();

  virtual void paintScheme(Schematic *);
  virtual void paintScheme(QPainter *);
  virtual void setCenter(int, int, bool relative=false);
  virtual void getCenter(int&, int&);

  bool isSelected;
  int  Type;    // whether it is Component, Wire, ...
  int  cx, cy, x1, y1, x2, y2;  // center and relative boundings
};


/** \class Conductor
  * \brief label for Node and Wire classes
  *
  */
class Conductor : public Element {
public:
  WireLabel *Label;
};

#endif
