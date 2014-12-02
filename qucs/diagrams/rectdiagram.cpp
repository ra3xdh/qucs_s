/***************************************************************************
                              rectdiagram.cpp
                             -----------------
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

/*!
  \class RectDiagram
  \brief The RectDiagram class implements the Cartesian diagram
*/

#include <QFontMetrics>

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <math.h>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "rectdiagram.h"
#include "main.h"
#include "misc.h"


RectDiagram::RectDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;      // position of label text
  y1 = y3 = 33;
  x2 = 240;    // initial size of diagram
  y2 = 160;
  x3 = 247;    // with some distance for right axes text

  Name = "Rect";
  calcDiagram();
}

RectDiagram::~RectDiagram()
{
}

// ------------------------------------------------------------
void RectDiagram::calcCoordinate(double* &xD, double* &yD, double* &,
                                 float *px, float *py, Axis *pa)
{
  double x  = *(xD++);
  double yr = *(yD++);
  double yi = *(yD++);
  if(xAxis.log) {
    x /= xAxis.low;
    if(x <= 0.0)  *px = -1e5;   // "negative infinity"
    else  *px = float(log10(x)/log10(xAxis.up / xAxis.low) * double(x2));
  }
  else  *px = float((x-xAxis.low)/(xAxis.up-xAxis.low)*double(x2));

  if(pa->log) {
    yr = sqrt(yr*yr + yi*yi);
    if(yr <= 0.0)  *py = -1e5;   // "negative infinity"
    else *py = float(log10(yr/fabs(pa->low)) /
                     log10(pa->up/pa->low) * double(y2));
  }
  else {
    if(fabs(yi) > 1e-250)  // preserve negative values if not complex number
      yr = sqrt(yr*yr + yi*yi);
    *py = float((yr-pa->low)/(pa->up-pa->low)*double(y2));
  }

  if(!isfinite(*px))  *px = 0.0;
  if(!isfinite(*py))  *py = 0.0;
}

// --------------------------------------------------------------
void RectDiagram::calcLimits()
{
  int i;
  double a, b, c;

  if(xAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(xAxis.log) {
      calcAxisLogScale(&xAxis, i, a, b, c, x2);
      xAxis.step = 1.0;
    }
    else  calcAxisScale(&xAxis, a, b, c, xAxis.step, double(x2));
    xAxis.limit_min = xAxis.low;
    xAxis.limit_max = xAxis.up;
  }

  if(yAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(yAxis.log) {
      calcAxisLogScale(&yAxis, i, a, b, c, y2);
      yAxis.step = 1.0;
    }
    else  calcAxisScale(&yAxis, a, b, c, yAxis.step, double(y2));
    yAxis.limit_min = yAxis.low;
    yAxis.limit_max = yAxis.up;
  }

  if(zAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(zAxis.log) {
      calcAxisLogScale(&zAxis, i, a, b, c, y2);
      zAxis.step = 1.0;
    }
    else  calcAxisScale(&zAxis, a, b, c, zAxis.step, double(y2));
    zAxis.limit_min = zAxis.low;
    zAxis.limit_max = zAxis.up;
  }
}

// --------------------------------------------------------------
int RectDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  double GridStep, corr, zD, zDstep, GridNum;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  y1 = QucsSettings.font.pointSize() + 6;

  x1 = 10;      // position of label text
  x3 = x2 + 7;
  QString tmp;
  bool back = false;
  int  z, w, valid = 0;

  // =====  give "step" the right sign (if user made it wrong)  ==============
  xAxis.step = fabs(xAxis.step);
  if(xAxis.limit_min > xAxis.limit_max)
    xAxis.step *= -1.0;

  yAxis.step = fabs(yAxis.step);
  if(yAxis.limit_min > yAxis.limit_max)
    yAxis.step *= -1.0;

  zAxis.step = fabs(zAxis.step);
  if(zAxis.limit_min > zAxis.limit_max)
    zAxis.step *= -1.0;


  // ====  x grid  =======================================================
if(xAxis.log) {
  if(xAxis.autoScale) {
    if(xAxis.max*xAxis.min < 1e-200)  goto Frame;  // invalid
  }
  else  if(xAxis.limit_min*xAxis.limit_max < 1e-200)  goto Frame;  // invalid

  back = calcAxisLogScale(&xAxis, z, zD, zDstep, corr, x2);

  if(back) z = x2;
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen));  // x grid

    if((zD < 1.5*zDstep) || (z == 0) || (z == x2)) {
      tmp = StringNiceNum(zD);
      if(xAxis.up < 0.0)  tmp = '-'+tmp;
      w = metrics.width(tmp);  // width of text
      // center text horizontally under the x tick mark
      Texts.append(new Text(z-(w>>1), -y1, tmp));
      Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));  // x tick marks
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    if(back) {
      z = int(corr*log10(zD / fabs(xAxis.up)) + 0.5); // int() implies floor()
      z = x2 - z;
    }
    else
      z = int(corr*log10(zD / fabs(xAxis.low)) + 0.5);// int() implies floor()
  }
}
else {  // not logarithmical
  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));

  double Expo;
  if(xAxis.up == 0.0)  Expo = log10(fabs(xAxis.up-xAxis.low));
  else  Expo = log10(fabs(xAxis.up));

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = StringNiceNum(GridNum);
    w = metrics.width(tmp);  // width of text
    // center text horizontally under the x tick mark
    Texts.append(new Text(z-(w>>1), -y1, tmp)); // Text(x, y, str, ...)
    GridNum += GridStep;

    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));   // x tick marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(xlog) ... else ..."


  // ====  y grid  =======================================================
  if(zAxis.numGraphs > 0) if(calcYAxis(&zAxis, x2)) valid |= 2;
  if(yAxis.numGraphs > 0) if(calcYAxis(&yAxis, 0))  valid |= 1;


Frame:
  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(Qt::black,0)));
  return valid;
}

// ------------------------------------------------------------
bool RectDiagram::insideDiagram(float x, float y)
{
  return (regionCode(x, y) == 0);
}

// ------------------------------------------------------------
void RectDiagram::clip(float* &p)
{
  rectClip(p);
}

// ------------------------------------------------------------
Diagram* RectDiagram::newOne()
{
  return new RectDiagram();
}

// ------------------------------------------------------------
Element* RectDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Cartesian");
  BitmapFile = (char *) "rect";

  if(getNewOne)  return new RectDiagram();
  return 0;
}
