/***************************************************************************
                               diagram.h
                              -----------
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

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "graph.h"
#include "marker.h"
#include "element.h"
#include "viewpainter.h"

#include <QFile>
#include <QTextStream>
#include <QList>

#define MIN_SCROLLBAR_SIZE 8

#define INVALID_STR QObject::tr(" <invalid>")

// ------------------------------------------------------------
// Enlarge memory block if neccessary.
#define  FIT_MEMORY_SIZE  \
  if(p >= p_end) {     \
    Size += 256;        \
    tmp = p - g->ScrPoints; \
    p = p_end = g->ScrPoints = (float*)realloc(g->ScrPoints, Size*sizeof(float)); \
    p += tmp; \
    p_end += Size - 9; \
  } \


struct Axis {
  double  min, max; // least and greatest values of all graph data
  double  low, up;  // the limits of the diagram
  bool    log;      // in "rectdiagram": logarithmic or linear
  QString Label;
  int     numGraphs;  // counts number of graphs using this axis
  bool    GridOn;

  bool   autoScale;    // manual limits or auto-scale ?
  double limit_min, limit_max, step;   // if not auto-scale
};


class Diagram : public Element {
public:
  Diagram(int _cx=0, int _cy=0);
  virtual ~Diagram();

  virtual Diagram* newOne();
  virtual int  calcDiagram() { return 0; };
  virtual void calcCoordinate
               (double* &, double* &, double* &, float*, float*, Axis*) {};
  virtual void calcLimits() {};
  
  virtual void paint(ViewPainter*);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  void    paintScheme(Schematic*);
  void    Bounding(int&, int&, int&, int&);
  bool    getSelected(int, int);
  bool    resizeTouched(float, float, float);
  QString save();
  bool    load(const QString&, QTextStream*);

  void getAxisLimits(Graph*);
  void updateGraphData();
  void loadGraphData(const QString&);
  void recalcGraphData();
  int  loadVarData(const QString&, Graph*);
  int  loadIndepVarData(const QString&, char*, Axis*, Graph*);
  bool sameDependencies(Graph*, Graph*);
  int  checkColumnWidth(const QString&, const QFontMetrics&, int, int, int);

  virtual bool insideDiagram(float, float);
  Marker* setMarker(int x, int y);

  QString Name; // identity of diagram type (e.g. Polar), used for saving etc.
  QPen    GridPen;

  QList<Graph *>  Graphs;
  QList<Arc *>    Arcs;
  QList<Line *>   Lines;
  QList<Text *>   Texts;

  int x3, y3;
  Axis  xAxis, yAxis, zAxis;   // axes (x, y left, y right)
  int State;  // to remember which resize area was touched

  bool hideLines;       // for "Rect3D": hide invisible lines ?
  int rotX, rotY, rotZ; // for "Rect3D": rotation around x, y and z axis

protected:
  void calcSmithAxisScale(Axis*, int&, int&);
  void createSmithChart(Axis*, int Mode=7);
  void calcPolarAxisScale(Axis*, double&, double&, double&);
  void createPolarDiagram(Axis*, int Mode=3);

  bool calcAxisScale(Axis*, double&, double&, double&, double&, double);
  bool calcAxisLogScale(Axis*, int&, double&, double&, double&, int);
  bool calcYAxis(Axis*, int);
  virtual void createAxisLabels();

  int  regionCode(float, float);
  virtual void clip(float* &);
  void rectClip(float* &);

  virtual void calcData(Graph*);

private:
  int Bounding_x1, Bounding_x2, Bounding_y1, Bounding_y2;
};

#endif
