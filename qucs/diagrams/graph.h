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

#include <QColor>
#include <Q3PtrList>
#include <QDateTime>

#include <assert.h>

// meaning of the values in a graph "Points" list
#define STROKEEND   -2
#define BRANCHEND   -10
#define GRAPHEND    -100

typedef enum{
  GRAPHSTYLE_INVALID = -1,
  GRAPHSTYLE_SOLID = 0,
  GRAPHSTYLE_DASH,
  GRAPHSTYLE_DOT,
  GRAPHSTYLE_LONGDASH,
  GRAPHSTYLE_STAR,
  GRAPHSTYLE_CIRCLE,
  GRAPHSTYLE_ARROW,
  GRAPHSTYLE_COUNT,
} graphstyle_t;

inline graphstyle_t toGraphStyle(int x){
  if (x<0){
    return GRAPHSTYLE_INVALID;
  }else if(x<GRAPHSTYLE_COUNT){
    return graphstyle_t(x);
  }else{
    return GRAPHSTYLE_INVALID;
  }
}

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

  struct ScrPt{
	  float Scr;
	  //double Data; not yet
  };
  typedef std::vector<ScrPt> container;
  typedef container::iterator iterator;
  typedef container::const_iterator const_iterator;

  void    paint(ViewPainter*, int, int);
  void    paintLines(ViewPainter*, int, int);
  QString save();
  bool    load(const QString&);
  int     getSelected(int, int);
  Graph*  sameNewOne();
  void clear(){ScrPoints.resize(0);}
  void resizeScrPoints(size_t s){assert(s>=ScrPoints.size()); ScrPoints.resize(s);}
  iterator begin(){return ScrPoints.begin();}
  iterator end(){return ScrPoints.end();}
  const_iterator begin() const{return ScrPoints.begin();}
  const_iterator end() const{return ScrPoints.end();}

  QDateTime lastLoaded;  // when it was loaded into memory
  int     yAxisNo;       // which y axis is used
  Q3PtrList<DataX>  cPointsX;
  double *cPointsY;
  int     countY;    // number of curves
  QString Var;
  QColor  Color;
  int     Thick;
  graphstyle_t Style;
  QList<Marker *> Markers;

  // for tabular diagram
  int  Precision;   // number of digits to show
  int  numMode;     // real/imag or polar (deg/rad)

private: // painting
  void drawLines(int, int, ViewPainter*) const;
  void drawStarSymbols(int, int, ViewPainter*) const;
  void drawCircleSymbols(int, int, ViewPainter*) const;
  void drawArrowSymbols(int, int, ViewPainter*) const;

private:
  std::vector<ScrPt> ScrPoints; // data in screen coordinates
};

#endif
