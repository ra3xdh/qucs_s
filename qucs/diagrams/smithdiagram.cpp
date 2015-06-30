/***************************************************************************
                          smithdiagram.cpp  -  description
                             -------------------
    begin                : Sat Oct 18 2003
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

/*!
  \class SmithDiagram
  \brief The SmithDiagram class implements the Impedance and Admittance Smith diagram
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "smithdiagram.h"
#include "misc.h"
#include "../dialogs/matchdialog.h" // For r2z function


SmithDiagram::SmithDiagram(int _cx, int _cy, bool ImpMode) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 2;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  y3 = 0;
  x3 = 207;    // with some distance for right axes text
  if(ImpMode)  Name = "Smith";  // with impedance circles
  else  Name = "ySmith";        // with admittance circles

  Arcs.append(new struct Arc(0, y2, x2, y2, 0, 16*360, QPen(Qt::black,0)));
//  calcDiagram();    // calculate circles for smith chart with |r|=1
}

SmithDiagram::~SmithDiagram()
{
}

// ------------------------------------------------------------
// calculate the screen coordinates for the graph data
void SmithDiagram::calcCoordinate(const double*, const double* yD, const double*,
                                  float *px, float *py, Axis const*) const
{
  double yr = yD[0];
  double yi = yD[1];
  *px = float((yr/yAxis.up + 1.0)*double(x2)/2.0);
  *py = float((yi/yAxis.up + 1.0)*double(y2)/2.0);

  if(std::isfinite(*px))
    if(std::isfinite(*py))
      return;

  *px = *py = float(cx) / 2.0;
}

// ------------------------------------------------------------
void SmithDiagram::calcLimits()
{
  int a;
  calcSmithAxisScale(&yAxis, a, a);
  yAxis.limit_min = 0.0;
  yAxis.step = double(a);
  yAxis.limit_max = yAxis.up;
}

// ------------------------------------------------------------
// calculate the circles and arcs of the smith chart
int SmithDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x3 = x2 + 7;
  if(Name.at(0) == 'y')  createSmithChart(&yAxis, 6);
  else  createSmithChart(&yAxis);

  // outer most circle
  Arcs.append(new Arc(0, x2, x2, x2, 0, 16*360, QPen(Qt::black,0)));

  // horizontal line Im(r)=0
  Lines.append(new Line(0, x2>>1, x2, x2>>1, GridPen));

  return 3;
}

// ------------------------------------------------------------
Diagram* SmithDiagram::newOne()
{
  return new SmithDiagram();
}

// ------------------------------------------------------------
Element* SmithDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Smith Chart");
  BitmapFile = (char *) "smith";

  if(getNewOne)  return new SmithDiagram();
  return 0;
}

// ------------------------------------------------------------
Element* SmithDiagram::info_y(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Admittance Smith");
  BitmapFile = (char *) "ysmith";

  if(getNewOne)  return new SmithDiagram(0, 0, false);
  return 0;
}

QString SmithDiagram::extraMarkerText(Marker const* m) const
{
  assert(m);
  Graph const* pGraph = m->graph();
  assert(pGraph);
  std::vector<double> const& Pos = m->varPos();
  unsigned nVarPos = pGraph->numAxes();
  assert(nVarPos == Pos.size());
  double Zr, Zi;
  double Z0 = m->Z0;
  double Precision = m->precision(); // hmmm

  Zr = m->powReal();
  Zi = m->powImag();

  MatchDialog::r2z(Zr, Zi, Z0);
  QString Var = pGraph->Var;

  if(Var.startsWith("S")) { // uuh, ooh hack.
    return "\n"+ Var.replace('S', 'Z')+": " +misc::complexRect(Zr, Zi, Precision);
  }else{
    return "\nZ("+ Var+"): " +misc::complexRect(Zr, Zi, Precision);
  }
}

// vim:ts=8:sw=2:noet
