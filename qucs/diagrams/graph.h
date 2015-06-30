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

#include <cmath>
#include <QColor>
#include <Q3PtrList>
#include <QDateTime>

#include <assert.h>

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
       : Var(Var_), Points(Points_), count(count_), Min(INFINITY), Max(-INFINITY) {};
 ~DataX() { if(Points) delete[] Points; };
  QString Var;
  double *Points;
  int     count;

public:
  const double& min()const {return Min;}
  const double& max()const {return Max;}
public: // only called from Graph. cleanup later.
  const double& min(const double& x){if (Min<x) Min=x; return Min;}
  const double& max(const double& x){if (Max>x) Max=x; return Max;}
private:
  double Min;
  double Max;
};

struct Axis;

/*!
 * prepare data for plotting purposes in Diagram.
 * a Graph is a list of graphs (bug?!)
 * iterating yields points (in screen coordinates) and control tokens.
 *
 * also stores markers.
 */
class Graph : public Element {
public:
  Graph(const Diagram*, const QString& _Line="");
 ~Graph();

  class ScrPt{
    float ScrX;
    float ScrY;

    double indep; // top level indep value (sweep)
    double dep; // top level dep value // FIXME: type?!
  public:
    ScrPt() : ScrX(0){}
    ~ScrPt(){}

    void setStrokeEnd();
    void setBranchEnd();
    void setGraphEnd();
    void setScrX(float); // screen horizontal coordinate
    void setScrY(float); // screen vertical coordinate
    void setScr(float,float); // both @ once.
    void setIndep(double);
    void setDep(double);
//    void attachCoords(double*);

    bool isPt() const; // indicate if this is a point on the screen
    bool isStrokeEnd() const;
    bool isBranchEnd() const;
    bool isGraphEnd() const;

    float getScrX() const;
    float getScrY() const;
    double getIndep() const;
    double getDep() const;
  };
  typedef std::vector<ScrPt> container;
  typedef container::iterator iterator;
  typedef container::const_iterator const_iterator;

  int loadDatFile(const QString& filename);
  int loadIndepVarData(const QString&, char* datfilecontent, DataX* where);

  void    paint(ViewPainter*, int, int);
  void    paintLines(ViewPainter*, int, int);
  QString save();
  bool    load(const QString&);
  int     getSelected(int, int);
  Graph*  sameNewOne();
  
private: // tmp hack
  DataX* mutable_axis(uint i) { if(i<(uint)cPointsX.size()) return cPointsX.at(i); return NULL;}
public:
  unsigned numAxes() const { return cPointsX.size(); }
  DataX const* axis(uint i) const { if(i<(uint)cPointsX.size()) return cPointsX.at(i); return NULL;}
  size_t count(uint i) const { if(axis(i)) return axis(i)->count; return 0; }
  QString axisName(unsigned i) const {if(axis(i))return axis(i)->Var; return "";}
  bool isEmpty() const { return !cPointsX.size(); }
  QVector<DataX*>& mutable_axes(){return cPointsX;} // HACK

  void clear(){ScrPoints.resize(0);}
  void resizeScrPoints(size_t s){assert(s>=ScrPoints.size()); ScrPoints.resize(s);}
  iterator begin(){return ScrPoints.begin();}
  iterator end(){return ScrPoints.end();}
  const_iterator begin() const{return ScrPoints.begin();}
  const_iterator end() const{return ScrPoints.end();}

  QDateTime lastLoaded;  // when it was loaded into memory
  int     yAxisNo;       // which y axis is used
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
public: // marker related
  void createMarkerText() const;
  std::pair<double,double> findSample(std::vector<double>&) const;
  Diagram const* parentDiagram() const{return diagram;}
private:
  QVector<DataX*>  cPointsX;
  std::vector<ScrPt> ScrPoints; // data in screen coordinates
  Diagram const* diagram;
};

#endif

// vim:ts=8:sw=2:noet
