/***************************************************************************
                        psdiagram.cpp  -  description
                             -------------------
    begin                : Sat Nov 06 2004
    copyright            : (C) 2004 by Michael Margraf
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

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <QtGui>
#include <math.h>
#include <float.h>
#ifdef __MINGW32__
# define finite(x) _finite(x)
#endif
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "psdiagram.h"
#include "main.h"

// mixed polar and smith diagram
PSDiagram::PSDiagram(int _cx, int _cy, bool _polarUp)
			: Diagram(_cx, _cy)
{
  x1 = 10;       // position of label text
  y1 = y3 = 2;
  x2 = 200;      // initial size of diagram
  x3 = 207;
  y2 = 200;
  if(_polarUp)  Name = "PS";  // polar diagram upper half ?
  else  Name = "SP";

  Arcs.append(new struct Arc(0, y2, x2, y2, 0, 16*360, QPen(Qt::black,0)));
//  calcDiagram();
}

PSDiagram::~PSDiagram()
{
}

// ------------------------------------------------------------
void PSDiagram::calcCoordinate(double* &, double* &yD, double* &,
                               float *px, float *py, Axis *pa)
{
  double yr = *(yD++);
  double yi = *(yD++);
  *px = float((yr/pa->up + 1.0)*double(x2)/2.0);
  *py = float((yi/pa->up + 1.0)*double(y2)/2.0);

  if(finite(*px))
    if(finite(*py))
      return;

  *px = *py = float(cx) / 2.0;
}

// --------------------------------------------------------------
void PSDiagram::calcLimits()
{
  int i;
  double a, b;
  Axis *polarAxis, *smithAxis;

  if(Name == "PS") {
    smithAxis = &yAxis;
    polarAxis = &zAxis;
  }
  else {
    polarAxis = &yAxis;
    smithAxis = &zAxis;
  }

  calcSmithAxisScale(smithAxis, i, i);
  smithAxis->limit_min = 0.0;
  smithAxis->step = double(i);
  smithAxis->limit_max = smithAxis->up;

  calcPolarAxisScale(polarAxis, a, polarAxis->step, b);
  polarAxis->limit_min = 0.0;
  polarAxis->limit_max = polarAxis->up;

}

// --------------------------------------------------------------
int PSDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x3 = x2 + 7;
  if(Name == "PS") {   // if polar not in upper half -> mirror
    createSmithChart(&yAxis, 5);
    createPolarDiagram(&zAxis, 1);
  }
  else {
    createSmithChart(&zAxis, 3);
    createPolarDiagram(&yAxis, 2);
  }

  // x line
  Lines.append(new Line(0, y2>>1, x2, y2>>1, GridPen));
  return 3;
}

// ------------------------------------------------------------
Diagram* PSDiagram::newOne()
{
  return new PSDiagram();
}

// ------------------------------------------------------------
Element* PSDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Polar-Smith Combi");
  BitmapFile = (char *) "polarsmith";

  if(getNewOne)  return new PSDiagram();
  return 0;
}

// ------------------------------------------------------------
Element* PSDiagram::info_sp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Smith-Polar Combi");
  BitmapFile = (char *) "smithpolar";

  if(getNewOne)  return new PSDiagram(0, 0, false);
  return 0;
}
