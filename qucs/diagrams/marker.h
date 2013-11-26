/***************************************************************************
                                marker.h
                               ----------
    begin                : Sat Apr 10 2004
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

#ifndef MARKER_H
#define MARKER_H

#include <QtGui>
#include "element.h"
#include "viewpainter.h"

#include <QPainter>
#include <q3valuelist.h>

class Diagram;
class Graph;


class Marker : public Element {
public:
  Marker(Diagram *Diag_, Graph *pg_=0, int _nn=0, int cx_=0, int cy_=0);
 ~Marker();

  void    initText(int);
  void    createText();
  void    makeInvalid();
  bool    moveLeftRight(bool);
  bool    moveUpDown(bool);
  void    paint(ViewPainter*, int, int);
  void    paintScheme(QPainter*);
  void    setCenter(int, int, bool);
  void    Bounding(int& _x1, int& _y1, int& _x2, int& _y2);
  QString save();
  bool    load(const QString& Line);
  bool    getSelected(int, int);
  Marker* sameNewOne(Graph*);
  void    getTextSize(const QFont&);

  Diagram *Diag;     // the corresponding diagram
  Graph   *pGraph;   // the corresponding graph

  int    nVarPos;   // number of values in "VarPos"
  double *VarPos;   // values the marker is pointing to
  float  fCX, fCY;  // coordinates for the line from graph to marker body

  QString Text;     // the string to be displayed in the marker text
  bool transparent; // background shines through marker body

  int Precision; // number of digits to show
  int numMode;   // real/imag or polar (deg/rad)
	double Z0;		//Only used in smith chart marker, to convert S to Z
};

#endif
