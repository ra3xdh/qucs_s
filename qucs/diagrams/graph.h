/***************************************************************************
                                 graph.h
                                ---------
    begin                : Thu Oct 2 2003
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

#ifndef GRAPH_H
#define GRAPH_H


#include "marker.h"
#include "element.h"

#include <qcolor.h>
#include <q3ptrlist.h>
#include <qdatetime.h>


// meaning of the values in a graph "Points" list
#define STROKEEND   -2
#define BRANCHEND   -10
#define GRAPHEND    -100

#define GRAPHSTYLE_SOLID    0
#define GRAPHSTYLE_DASH     1
#define GRAPHSTYLE_DOT      2
#define GRAPHSTYLE_LONGDASH 3
#define GRAPHSTYLE_STAR     4
#define GRAPHSTYLE_CIRCLE   5
#define GRAPHSTYLE_ARROW    6

class Diagram;
class ViewPainter;


struct DataX {
  DataX(const QString& Var_, double *Points_=0, int count_=0)
       : Var(Var_), Points(Points_), count(count_) {};
 ~DataX() { if(Points) delete[] Points; };
  QString Var;
  double *Points;
  int     count;
};


class Graph : public Element {
public:
  Graph(const QString& _Line="");
 ~Graph();

  void    paint(ViewPainter*, int, int);
  void    paintLines(ViewPainter*, int, int);
  QString save();
  bool    load(const QString&);
  int     getSelected(int, int);
  Graph*  sameNewOne();

  QDateTime lastLoaded;  // when it was loaded into memory
  int     yAxisNo;       // which y axis is used
  Q3PtrList<DataX>  cPointsX;
  double *cPointsY;
  float  *ScrPoints; // data in screen coordinates
  int     countY;    // number of curves
  QString Var;
  QColor  Color;
  int     Thick;
  int     Style;
  Q3PtrList<Marker> Markers;

  // for tabular diagram
  int  Precision;   // number of digits to show
  int  numMode;     // real/imag or polar (deg/rad)
};

#endif
