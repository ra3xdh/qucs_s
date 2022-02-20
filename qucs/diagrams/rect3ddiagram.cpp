/***************************************************************************
                              rect3ddiagram.cpp
                             -------------------
    begin                : Sat Mar 5 2005
    copyright            : (C) 2005 by Michael Margraf
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
  \class Rect3DDiagram
  \brief The Rect3DDiagram class implements the 3D-Cartesian diagram
*/

#include <QFontMetrics>

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <cmath>
#include <float.h>
#include <limits.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "rect3ddiagram.h"
#include "main.h"
#include "misc.h"

Rect3DDiagram::Rect3DDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = y3 = 7;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  x3 = 207;    // with some distance for right axes text

  Mem = pMem = 0;  // auxiliary buffer for hidden lines

  Name = "Rect3D"; // BUG
  // symbolic diagram painting
  Lines.append(new qucs::Line(0, 0, cx,  0, QPen(Qt::black,0)));
  Lines.append(new qucs::Line(0, 0,  0, cy, QPen(Qt::black,0)));
  Lines.append(new qucs::Line(0, 0, cx/2, cy/2, QPen(Qt::black,0)));
}

Rect3DDiagram::~Rect3DDiagram()
{
}

// ------------------------------------------------------------
// Calculates the coefficients for 3D -> 2D transformation
void Rect3DDiagram::calcCoefficients()
{
  double rX = double(rotX) * pi/180.0;
  double rY = double(rotY) * pi/180.0;
  double rZ = double(rotZ) * pi/180.0;
  
  cxy = sin(rZ);  cxx = cos(rZ);
  cxz = sin(rY);  rY  = cos(rY);
  cyz = sin(rX);  czz = cos(rX);
  rX  = cyz*cxz;  rZ = czz*cxz;
  cyx = czz * cxy + rX * cxx;
  cyy = czz * cxx - rX * cxy;
  czx = cyz * cxy - rZ * cxx;
  czy = cyz * cxx + rZ * cxy;
  cxx *=  rY;  cxy *= -rY;  cyz *= -rY;  czz *=  rY;
}

// ------------------------------------------------------------
double Rect3DDiagram::calcX_2D(double x, double y, double z) const
{
  return (cxx * x + cxy * y + cxz * z) * scaleX;
}

// ------------------------------------------------------------
double Rect3DDiagram::calcY_2D(double x, double y, double z) const
{
  return (cyx * x + cyy * y + cyz * z) * scaleY;
}

// ------------------------------------------------------------
double Rect3DDiagram::calcZ_2D(double x, double y, double z) const
{
  return czx * x + czy * y + czz * z;
}

// ------------------------------------------------------------
// Determines the position of the coordinate cross, i.e. calculates
// "scaleX", "scaleY", "xorig" and "yorig". Returns the corner with
// the largest distance from the viewer.
int Rect3DDiagram::calcCross(int *Xses, int *Yses)
{
  double x3D, y3D, z3D, x2D[8], y2D[8], z2D;
  double XMIN_2D, XMAX_2D, YMIN_2D, YMAX_2D, ZMIN_2D;

  int z, Center = 0;  // used to save minimum z (is center for axis cross)
  scaleX = scaleY = 1.0;  // used in "calcX_2D" and "calcY_2D"
  XMIN_2D = YMIN_2D = XMAX_2D = YMAX_2D = ZMIN_2D = 0.0; // origin is zero
  for(z=0; z<8; z++) {  // check 2D coordinates of all 8 quadrat corners
    if(z & 1) x3D = 1.0;  else x3D = 0.0;
    if(z & 2) y3D = 1.0;  else y3D = 0.0;
    if(z & 4) z3D = 1.0;  else z3D = 0.0;
    x2D[z] = calcX_2D(x3D, y3D, z3D);
    y2D[z] = calcY_2D(x3D, y3D, z3D);
    z2D = calcZ_2D(x3D, y3D, z3D);

    if(x2D[z] < XMIN_2D)  XMIN_2D = x2D[z];
    if(x2D[z] > XMAX_2D)  XMAX_2D = x2D[z];
    if(y2D[z] < YMIN_2D)  YMIN_2D = y2D[z];
    if(y2D[z] > YMAX_2D)  YMAX_2D = y2D[z];
    if(z2D < ZMIN_2D) { ZMIN_2D = z2D; Center = z; }
  }

  scaleX = double(x2) / (XMAX_2D - XMIN_2D); // scaling 3D -> 2D transformation
  scaleY = double(y2) / (YMAX_2D - YMIN_2D);
  xorig  = -XMIN_2D * scaleX;   // position of origin
  yorig  = -YMIN_2D * scaleY;

  for(z=0; z<8; z++) {  // calculate 2D coordinates of all corners
    *(Xses+z) = int(x2D[z] * scaleX + 0.5 + xorig);
    *(Yses+z) = int(y2D[z] * scaleY + 0.5 + yorig);
  }
  return Center;
}

// ------------------------------------------------------------
// Is needed for markers.
void Rect3DDiagram::calcCoordinate(const double* xD, const double* zD, const double* yD,
                                   float *px, float *py, Axis const*) const
{
  double x3D = zD[0];
  double y3D = zD[1];
  double z3D;
  if(zAxis.log) {
    z3D = sqrt(x3D*x3D + y3D*y3D);
/*    if(z3D <= 0.0)  clipping not yet correct implemented
      z3D = -1e5;   // "negative infinity"
    else*/
      z3D = log10(z3D / fabs(zAxis.low)) / log10(zAxis.up / zAxis.low);
  }
  else {
    if(fabs(y3D) > 1e-250) // preserve negative values if no complex number
      x3D = sqrt(x3D*x3D + y3D*y3D);
    z3D = (x3D - zAxis.low) / (zAxis.up - zAxis.low);
  }

  x3D = *(xD++);
  if(xAxis.log) {
    x3D /= xAxis.low;
/*    if(x3D <= 0.0)  clipping not yet correct implemented
      x3D = -1e5;   // "negative infinity"
    else*/
      x3D = log10(x3D) / log10(xAxis.up / xAxis.low);
  }
  else
    x3D = (x3D - xAxis.low) / (xAxis.up - xAxis.low);

  if(yAxis.log) {
    y3D = (*yD) / yAxis.low;
/*    if(y3D <= 0.0)  clipping not yet correct implemented
      y3D = -1e5;   // "negative infinity"
    else*/
      y3D = log10(y3D) / log10(yAxis.up / yAxis.low);
  }
  else
    y3D = (*yD - yAxis.low) / (yAxis.up - yAxis.low);

  *px = float(calcX_2D(x3D, y3D, z3D)) + xorig;
  *py = float(calcY_2D(x3D, y3D, z3D)) + yorig;

  if(std::isfinite(*px))
    if(std::isfinite(*py))
      return;

  *px = float(xorig);
  *py = float(yorig);
}

// --------------------------------------------------------------
void Rect3DDiagram::finishMarkerCoordinates(float& fCX, float& fCY) const
{
  if(!insideDiagram(fCX, fCY)) {
	  fCX = fCY = 0.0;
  }
}

// ------------------------------------------------------------
void Rect3DDiagram::calcCoordinate3D(double x, double y, double zr, double zi,
					tPoint3D *p, tPointZ *pz)
{
  if(zAxis.log) {
    zr = sqrt(zr*zr + zi*zi);
/*    if(zr <= 0.0)  clipping not yet correct implemented
      zr = -1e5;   // "negative infinity"
    else*/
      zr = log10(zr / fabs(zAxis.low)) / log10(zAxis.up / zAxis.low);
  }
  else {
    if(fabs(zi) > 1e-250) // preserve negative values if no complex number
      zr = sqrt(zr*zr + zi*zi);
    zr = (zr - zAxis.low) / (zAxis.up - zAxis.low);
  }

  if(xAxis.log) {
    x /= xAxis.low;
/*    if(x <= 0.0)  clipping not yet correct implemented
      x = -1e5;   // "negative infinity"
    else*/
      x = log10(x) / log10(xAxis.up / xAxis.low);
  }
  else
    x = (x - xAxis.low) / (xAxis.up - xAxis.low);

  if(yAxis.log) {
    y = y / yAxis.low;
/*    if(y <= 0.0)  clipping not yet correct implemented
      y = -1e5;   // "negative infinity"
    else*/
      y = log10(y) / log10(yAxis.up / yAxis.low);
  }
  else
    y = (y - yAxis.low) / (yAxis.up - yAxis.low);

  p->x  = int(calcX_2D(x, y, zr) + 0.5 + xorig);
  p->y  = int(calcY_2D(x, y, zr) + 0.5 + yorig);
  p->No = pz->No = p-Mem;
  p->done = 0;
  pz->z = float(calcZ_2D(x, y, zr));
}

// --------------------------------------------------------------
bool Rect3DDiagram::isHidden(int x, int y, tBound *Bounds, char *zBuffer)
{
  // remember the boundings of the polygon
  if( (Bounds+x)->max < y )  (Bounds+x)->max = y;
  if( (Bounds+x)->min > y )  (Bounds+x)->min = y;

  // diagram area already used ?
  return ( *(zBuffer + (y>>3) + x * ((y2+7)>>3)) & (1 << (y & 7)) ) != 0;
}

// --------------------------------------------------------------
// Enlarge memory block if neccessary.
void Rect3DDiagram::enlargeMemoryBlock(tPoint3D* &MemEnd)
{
  if(pMem >= MemEnd) {
    int Size = MemEnd - Mem + 256;
    MemEnd = Mem;
    Mem  = (tPoint3D*)realloc(Mem, Size*sizeof(tPoint3D));
    pMem += Mem - MemEnd;
    MemEnd = Mem + Size - 5;
  }
}

// --------------------------------------------------------------
// Calculate all 2D points of the line between point "p" and "p+1".
// Parameters:   p        - pointer on 3D coordinate of line start point
//                          (p+1 points onto line end point)
//               MemEnd   - pointer where memory block ends
//               Bounds   - memory block for occupied polygon area
//               zBuffer  - memory block for occupied diagram area
void Rect3DDiagram::calcLine(tPoint3D* &p, tPoint3D* &MemEnd,
				tBound *Bounds, char *zBuffer)
{
  int Pos_;
  int x1_ = p->x, y1_ = p->y;
  int x2_ = (p+1)->x, y2_ = (p+1)->y;

  bool wasHidden = isHidden(x1_, y1_, Bounds, zBuffer);
  if(wasHidden)
    if((p->done & 1) == 0)
      p->done |= 4;   // mark as hidden

  int ax_ = 0, ay_ = 0;
  int ix_, iy_, dx_, dy_, of_;

  if(x2_ >= x1_) {
    dx_ = x2_ - x1_;
    ix_ = 1;
  }
  else {
    dx_ = x1_ - x2_;
    ix_ = -1;
  }

  if(y2_ >= y1_) {
    dy_ = y2_ - y1_;
    iy_ = 1;
  }
  else {
    dy_ = y1_ - y2_;
    iy_ = -1;
  }

  if(dx_ < dy_) {
    of_ = dx_;   // exchange dx and dy
    dx_ = dy_;
    dy_ = of_;

    ax_ = iy_;
    ay_ = ix_;
    ix_ = iy_ = 0;
  }

  of_ = dx_ >> 1;
  for(int i=dx_; i>1; i--) {   // calculate each point of the line
    x1_ += ix_;
    y1_ += ax_;
    of_ += dy_;
    if(of_ > dx_) {
      of_ -= dx_;
      x1_ += ay_;
      y1_ += iy_;
    }

    // This section has significant impact to the hiding algorithm
    //   be aware isHidden() modifies (Bounds+x)->min/max
    // if isHidden() is replaced by  false  nearly all segments are drawn,
    //   nothing set hidden by this section ... though few segments are hidden
    // if isHidden() is replaced by true  most segments are not drawn
    //   and many line-ends are outside the diagram-are (mem/malloc-issue?)
    if( isHidden(x1_, y1_, Bounds, zBuffer) != wasHidden )
      if((p->done & 1) == 0) {
        wasHidden = !wasHidden;
        pMem->x  = x1_;
        pMem->y  = y1_;
        pMem->No = p->No;
        pMem->done = 0;
        if(wasHidden) pMem->done = 4;   // mark as hidden
        pMem++;

        Pos_ = p - Mem;
        // Enlarge memory block if neccessary.
        enlargeMemoryBlock(MemEnd);   // this may make "p" invalid (realloc)
        p = Mem + Pos_;  // rebuild "p"
    }
  }

  // extra treatment for last point (create no further point)
  // OLD: implementation hides most of the segmets
  // NEW: unhide missing segments
  if(!isHidden((p+1)->x, (p+1)->y, Bounds, zBuffer))
  {
    if(!isHidden((p)->x, (p)->y, Bounds, zBuffer))
    {
      (p+1)->done &= ~4;
    }
    else
    {
      (p)->done &= ~4;
    }
  }

  // If this assignment is commented-out all inner segments are not drawn.
  // Just the surrounding boundary (all segments) of the mesh is drawn
  p->done |= 1;   // mark as already worked on
}

// --------------------------------------------------------------
// Compare functions for GNU qsort routine.
int Rect3DDiagram::comparePoint3D(const void *Point1, const void *Point2)
{
  return ((tPoint3D*)Point1)->No - ((tPoint3D*)Point2)->No;
}
int Rect3DDiagram::comparePointZ(const void *Point1, const void *Point2)
{
  if((((tPointZ*)Point2)->z - ((tPointZ*)Point1)->z) < 0.0f)
    return -1;
  return 1;
}

// --------------------------------------------------------------
// Removes the invisible parts of the graph.
void Rect3DDiagram::removeHiddenLines(char *zBuffer, tBound *Bounds)
{
  double Dummy = 0.0;  // number for 1-dimensional data in 3D cartesian
  double *px, *py, *pz;

  tPoint3D *p;
  int i, j, z, dx, dy, Size=0;
  // pre-calculate buffer size to avoid reallocations in the first step
  foreach(Graph *g, Graphs)
    if(g->cPointsY)
      Size += g->axis(0)->count * g->countY;

  // "Mem" should be the last malloc to simplify realloc
  // multiplicator 'malloc_8xsize' added
  // 'malloc_8xsize' increases the requested size in the 2 malloc lines below
  // this comment section.
  // And it is added to the calculation of 'MemEnd'.
  // Main reason are the segments drawn to points outside the diagram.
  // It is assumed to be a memory issue.
  // Note: As of today the memory calculation is still not 100% clean.
  int malloc_8xsize=8;
  tPointZ *zMem = (tPointZ*)malloc( malloc_8xsize*(Size+2)*sizeof(tPointZ) );
  Mem  = (tPoint3D*)malloc( malloc_8xsize*2*(Size+2)*sizeof(tPoint3D) );

  pMem = Mem;
  tPointZ *zp = zMem, *zp_tmp;

  // ...............................................................
  foreach(Graph *g, Graphs) {

    pz = g->cPointsY;
    if(!pz) continue;
    if(g->numAxes() < 1) continue;

    py = &Dummy;
    if(g->countY > 1)  py = g->axis(1)->Points;

    p = pMem;  // save status for cross grid
    zp_tmp = zp;
    // ..........................................
    // calculate coordinates of all lines
    dx = g->axis(0)->count;
    if(g->countY > 1)  dy = g->axis(1)->count;
    else  dy = 0;
    for(i=g->countY-1; i>=0; i--) {   // y coordinates
      px = g->axis(0)->Points;
    
      for(j=dx; j>0; j--) { // x coordinates
        calcCoordinate3D(*(px++), *py, *pz, *(pz+1), pMem++, zp++);
        pz += 2;
      }

      (pMem-1)->done |= 8;  // mark as "last in line"
      py++;
      if(dy > 0) if((i % dy) == 0)
        py = g->axis(1)->Points;
    }
    (pMem-1)->done |= 512;  // mark as "last point before grid"

    // ..........................................
    // copy points for cross lines ("dx", "dy" still unchanged ! )
    if(g->countY > 1) {
      zp = zp_tmp;
      for(j=g->countY/dy; j>0; j--) { // every plane
        for(i=dx; i>0; i--) {  // every branch
          for(z=dy; z>0; z--) {  // every point
            pMem->x  = p->x;
            pMem->y  = p->y;
            pMem->No = pMem-Mem;
            pMem->done = 0;
            zp->NoCross = pMem-Mem; // position of its cross grid
            pMem++;
            p += dx;  // next coordinate
            zp += dx;
          }
          (pMem-1)->done |= 8;  // mark as "last in line"
          p  -= dx*dy - 1;  // next z coordinate
          zp -= dx*dy - 1;
        }
        p  += dx*(dy-1);
        zp += dx*(dy-1);
      }
    }
    (pMem-1)->done |= 256;  // mark as "very last point"


    if(hideLines) {
    // ..........................................
    // Calculate the z-coordinate of all polygons by building the
    // sum of the z-coordinates of all of its 4 corners.
    // After this, each point represents one polygon. The unneccessary
    // points are filled with "-FLTMAX".
    zp = zp_tmp;
    // "dx" and "dy" are still unchanged !
    for(i=g->countY-1; i>=0; i--) {   // all branches
      if(dy > 0) if(i % dy) {
        for(j=dx-1; j>0; j--) {   // x coordinates
          zp->z += (zp+1)->z + (zp+dx)->z + (zp+dx+1)->z;
          zp++;
        }
        zp->z = -FLT_MAX;  // last one not needed
        zp++;
        continue;
      }

      // last line not needed
      for(j=dx; j>0; j--) {   // x coordinates
        zp->z = -FLT_MAX;  // last one not needed
        zp++;
      }
    }
    }  // of "if(hideLines)"

  }  // of "for(Graphs)"


  if(!hideLines) {  // do not hide invisible lines
    free(zMem);
    return;
  }

#if 0
  qDebug("##########################################");
  qDebug("Size 1b: Size=%d, %d, %d", Size, pMem-Mem, zp-zMem);
  for(tPoint3D *p=Mem; p<pMem; p++)
    qDebug("xyPoints:  %d/%d - %d - %d", p->x, p->y, p->No, p->done);
  qDebug("------------------------------------------");
  for(tPointZ *p=zMem; p-zMem<Size; p++)
    qDebug("zPoints:   %g - %d", p->z, p->No);
#endif


  // ..........................................
  // Sort z-coordinates (greatest first).
  // After this the polygons that have the smallest distance to the
  // viewer are on top of the list and thus, will be processed first.
  qsort(zMem, Size, sizeof(tPointZ), comparePointZ);

#if 0
  qDebug("--------------------------- z sorting");
  for(tPointZ *p=zMem; p-zMem<Size; p++)
    qDebug("zPoints:   %g - %d", p->z, p->No);
#endif


  // ..........................................
  char *pc;
  // malloc_8xsize added (see comment before malloc_8xsize declaration)
  tPoint3D *MemEnd = Mem + malloc_8xsize*2*Size - 5;   // limit of buffer

  zp = zMem;
  foreach(Graph *g, Graphs) {
    if(!g->cPointsY) continue;
    dx = g->axis(0)->count;
    if(g->countY > 1)  dy = g->axis(1)->count;
    else  dy = 1;

    // look for hidden lines ...
    for(int No = g->countY/dy * (dx-1)*(dy-1); No>0; No--) {

      // reset the polygon bounding buffer
      for(i=x2; i>=0; i--) {
        (Bounds+i)->max = INT_MIN;
        (Bounds+i)->min = INT_MAX;
      }

      // work on all 4 lines of polygon
      p = Mem + zp->No;  // polygon corner coordinates
      calcLine(p, MemEnd, Bounds, zBuffer);

      p += dx;
      calcLine(p, MemEnd, Bounds, zBuffer);

      p = Mem + zp->NoCross;  // cross grid
      calcLine(p, MemEnd, Bounds, zBuffer);

      p += dy;
      calcLine(p, MemEnd, Bounds, zBuffer);

      // mark the area of the polygon (stored in "*Bounds") as used
      for(i=x2-1; i>=0; i--)  // all x coordinates
        if( (Bounds+i)->max > INT_MIN) {
          pc = zBuffer + i * ((y2+7)>>3);
          for(j=(Bounds+i)->min; j<=(Bounds+i)->max; j++) // all y coordinates
            *(pc + (j>>3)) |= (1 << (j & 7));
        }

      zp++;   // next polygon
    }

  }  // of "for(Graphs)"

#if 0
  qDebug("--------------------------- hidden lines %d", pMem-Mem);
  for(tPoint3D *p=Mem; p<pMem; p++)
    qDebug("xyPoints:  %d/%d - %d - %d", p->x, p->y, p->No, p->done);
#endif

  free(zMem);

  // sort "No" (least one first)
  qsort(Mem, pMem - Mem, sizeof(tPoint3D), comparePoint3D);

#if 0
  qDebug("--------------------------- last sorting %d", pMem-Mem);
  for(tPoint3D *p=Mem; p<pMem; p++)
    qDebug("xyPoints:  %d/%d - %d - %d", p->x, p->y, p->No, p->done);
  qDebug("\n");
#endif
}

// --------------------------------------------------------------
// Removes the invisible parts of the coordinate cross.
void Rect3DDiagram::removeHiddenCross(int x1_, int y1_, int x2_, int y2_,
                                      char *zBuffer, tBound *Bounds)
{
  pMem = Mem;

  pMem->x  = x1_;
  pMem->y  = y1_;
  pMem->No = 0;
  pMem->done = 0;
  pMem++;

  pMem->x  = x2_;
  pMem->y  = y2_;
  pMem->No = 1;
  pMem->done = 0;
  pMem++;

  tPoint3D *p = Mem+6;
  calcLine(Mem, p, Bounds, zBuffer);
  *pMem = *(Mem+1);
  *(Mem+1) = *Mem;
  p = Mem+2;
  do {
    if(((p-1)->done & 4) == 0)
      Lines.append(new qucs::Line((p-1)->x, (p-1)->y, p->x, p->y, QPen(Qt::black,0)));
    p++;
  } while(p <= pMem);
}

// --------------------------------------------------------------
void Rect3DDiagram::calcLimits()
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
int Rect3DDiagram::calcAxis(Axis *Axis, int x, int y,
                            double xD, double phi)
{

  double GridStep, corr, yD, stepD, GridNum, Expo;
  double xstepD, ystepD;

  QString tmp;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  int maxWidth = 0;
  int count, gx, gy, w;

  if(phi > 0.0)  Expo = phi - pi/2.0;
  else  Expo = phi + pi/2.0;
  gx = int(5.4 * cos(Expo) + 0.5);  // short grid marker lines
  gy = int(5.4 * sin(Expo) + 0.5);

  
  if(Axis->log) {

    bool back = calcAxisLogScale(Axis, w, yD, stepD, corr, int(xD));

    //double upD  = Axis->up;
    if(yD > 1.5*stepD)  yD = 10.0*stepD;  // always start at power of 10
    if(back) {
      //upD  = Axis->low;
      phi += pi;
      xD   = 0.0;
    }

    int xLen, yLen;
    ystepD = corr * log10(yD / fabs(Axis->low));
    while(ystepD <= xD) {  // create all grid lines
      
      tmp = misc::StringNiceNum(yD);
      if(Axis->up < 0.0)  tmp = '-'+tmp;
      w = metrics.width(tmp);  // width of text
      if(maxWidth < w) maxWidth = w;
      
      xLen = int(ystepD * cos(phi) + 0.5) + x;
      yLen = int(ystepD * sin(phi) + 0.5) + y;
//      if(Qt::DockRight)
	Texts.append(new Text(xLen+3+gx, yLen-6+gy, tmp));
//      else
//	Texts.append(new Text(xLen-w-2-gx, yLen-6-gy, tmp));
      
      // short grid marks
      Lines.append(new qucs::Line(xLen-gx, yLen-gy, xLen+gx, yLen+gy,
			    QPen(Qt::black,0)));
      yD *= 10.0;
      ystepD += corr;
    }
    
  }
  else {  // not logarithmical
    calcAxisScale(Axis, GridNum, yD, stepD, GridStep, xD);
    count = int((xD - yD) / stepD) + 1;  // number of grids
    
    xstepD = stepD * cos(phi);
    ystepD = stepD * sin(phi);
    xD = yD * cos(phi) + 0.5 + double(x);
    yD = yD * sin(phi) + 0.5 + double(y);
    
    if(Axis->up == 0.0)  Expo = log10(fabs(Axis->up-Axis->low));
    else  Expo = log10(fabs(Axis->up));
    
    for(; count>0; count--) {
      x = int(xD);
      y = int(yD);
      if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0; // make 0 really 0
      tmp = misc::StringNiceNum(GridNum);
      
      w = metrics.width(tmp);  // width of text
      if(maxWidth < w) maxWidth = w;
//      if(Qt::DockRight)
	Texts.append(new Text(x+3+gx, y-6+gy, tmp)); // place text right
//      else
//	Texts.append(new Text(x-w-2-gx, y-6-gy, tmp)); // place left
      GridNum += GridStep;
      
      // short grid marks
      Lines.append(new qucs::Line(x-gx, y-gy, x+gx, y+gy, QPen(Qt::black,0)));
      xD += xstepD;
      yD += ystepD;
    }
  } // of "if(Axis->log) ... else ..."
  
  return maxWidth+5;
}

// --------------------------------------------------------------
void Rect3DDiagram::createAxis(Axis *Axis, bool Right,
                               int x1_, int y1_, int x2_, int y2_)
{
  Q_UNUSED(Right);

  DataX const *pD;
  double phi, cos_phi, sin_phi;
  int x, y, z, w, valid, Index = 0;
  if(Axis == &yAxis)  Index = 1;

  QString s;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);

  x = x2_ - x1_;
  y = y2_ - y1_;
  cos_phi = sqrt(double(x*x) + double(y*y));
  phi = atan2(double(y), double(x));
  
  valid = calcAxis(Axis, x1_, y1_, cos_phi, phi); // axis numbering
  z = (int)cos_phi;
  cos_phi = cos(phi);
  sin_phi = sin(phi);

  if(fabs(phi-1e-5) > pi/2.0) {
    x1_ = x2_;  cos_phi *= -1;
    y1_ = y2_;  sin_phi *= -1;
  }
  x = x1_ + int(double(valid)*sin_phi);
  y = y1_ - int(double(valid)*cos_phi);
  if(Axis->Label.isEmpty()) {
    // write all labels ----------------------------------------
    foreach(Graph *pg, Graphs) {
      if(Axis != &zAxis) {
        if(!pg->cPointsY)  continue;
        if(valid < 0) {
          delete[] pg->cPointsY;
          pg->cPointsY = 0;
          continue;
        }
        pD = pg->axis(Index);
        if(!pD) continue;
        s = pD->Var;
      }
      else {
        s = pg->Var;
        if(!pg->cPointsY)  s += INVALID_STR;
      }
      x += int(double(metrics.lineSpacing())*sin_phi);
      y -= int(double(metrics.lineSpacing())*cos_phi);
      w = metrics.width(s);
      Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                            y+int(double((z-w)>>1)*sin_phi),
                            s, pg->Color, 12.0, cos_phi, sin_phi));
    }
  }
  else {
    x += int(double(metrics.lineSpacing())*sin_phi);
    y -= int(double(metrics.lineSpacing())*cos_phi);
    w = metrics.width(Axis->Label);
    Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                          y+int(double((z-w)>>1)*sin_phi),
                          Axis->Label, Qt::black, 12.0, cos_phi, sin_phi));
  }
}

// --------------------------------------------------------------
int Rect3DDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  double GridStep, corr, zD, zDstep, GridNum;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);

  x3 = x2 + 7;
  int z, z2, o, w;

  char *zBuffer=0;   // hidden line algorithm
  tBound *Bounds=0;


  // =====  give "step" the right sign  ==================================
  xAxis.step = fabs(xAxis.step);
  if(xAxis.limit_min > xAxis.limit_max)
    xAxis.step *= -1.0;

  yAxis.step = fabs(yAxis.step);
  if(yAxis.limit_min > yAxis.limit_max)
    yAxis.step *= -1.0;

  zAxis.step = fabs(zAxis.step);
  if(zAxis.limit_min > zAxis.limit_max)
    zAxis.step *= -1.0;


  // =====  calculate Axis.up and Axis.low  ==============================
  if(xAxis.log) {
    if(xAxis.autoScale) {
      if(xAxis.max*xAxis.min <= 0.0)  goto Frame;  // invalid
    }
    else  if(xAxis.limit_min*xAxis.limit_max <= 0.0)  goto Frame;  // invalid
    calcAxisLogScale(&xAxis, z, zD, zDstep, corr, x2);
  }
  else  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));
    
  if(yAxis.log) {
    if(yAxis.autoScale) {
      if(yAxis.max*yAxis.min <= 0.0)  goto Frame;  // invalid
    }
    else  if(yAxis.limit_min*yAxis.limit_max <= 0.0)  goto Frame;  // invalid
    calcAxisLogScale(&yAxis, z, zD, zDstep, corr, x2);
  }
  else  calcAxisScale(&yAxis, GridNum, zD, zDstep, GridStep, double(x2));

  if(zAxis.log) {
    if(zAxis.autoScale) {
      if(zAxis.max*zAxis.min <= 0.0)  goto Frame;  // invalid
    }
    else  if(zAxis.limit_min*zAxis.limit_max <= 0.0)  goto Frame;  // invalid
    calcAxisLogScale(&zAxis, z, zD, zDstep, corr, x2);
  }
  else  calcAxisScale(&zAxis, GridNum, zD, zDstep, GridStep, double(x2));


  // ===  calculate transformation coefficients from rotation angles ===
  calcCoefficients();

  // =====  check calculate position of axes in 2D rectangle ===========
  int X[8], Y[8];
  o = calcCross(X, Y);
  // "o" is now the index of the origin coordinates.


  // =====  paint coordinate cross  ====================================
  // xy area
  Lines.append(new qucs::Line(X[o^1], Y[o^1], X[o^3], Y[o^3], QPen(Qt::black,0)));
  Lines.append(new qucs::Line(X[o^2], Y[o^2], X[o^3], Y[o^3], QPen(Qt::black,0)));

  // yz area
  Lines.append(new qucs::Line(X[o^2], Y[o^2], X[o^6], Y[o^6], QPen(Qt::black,0)));
  Lines.append(new qucs::Line(X[o^4], Y[o^4], X[o^6], Y[o^6], QPen(Qt::black,0)));

  // xz area
  Lines.append(new qucs::Line(X[o^1], Y[o^1], X[o^5], Y[o^5], QPen(Qt::black,0)));
  Lines.append(new qucs::Line(X[o^4], Y[o^4], X[o^5], Y[o^5], QPen(Qt::black,0)));


  // =====  create axis  =============================================
  if(X[o^1] < X[o^2])  w = 2;   // use which z axis ?
  else  w = 1;

  z = o^2;
  if(z & 1)  z ^= 1;  // from where to where ?
  z2 = z^1;
  createAxis(&xAxis, w == 2, X[z], Y[z], X[z2], Y[z2]);

  z = o^1;
  if(z & 2)  z ^= 2;  // from where to where ?
  z2 = z^2;
  createAxis(&yAxis, w == 1, X[z], Y[z], X[z2], Y[z2]);

  z = o^w;
  if(z & 4)  z ^= 4;  // from where to where ?
  z2 = z^4;
  createAxis(&zAxis, true, X[z], Y[z], X[z2], Y[z2]);


  if(hideLines) {
    w = (x2+1) * (y2/8 + 1);
    // To store the pixel coordinates that are already used (hidden).
    // Use one bit per pixel.
    zBuffer = (char*)malloc(w);
    memset(zBuffer, 0, w);

    // To store the boundings of the current polygon.
    Bounds = (tBound*)malloc((x2+1) * sizeof(tBound));
  }

  // hide invisible parts of graphs
  removeHiddenLines(zBuffer, Bounds);

  if(hideLines) {
    // now hide invisible part of coordinate cross
    tPoint3D *MemTmp = Mem;
    Mem = (tPoint3D*)malloc( 10*sizeof(tPoint3D) );
    
    removeHiddenCross(X[o^1], Y[o^1], X[o], Y[o], zBuffer, Bounds); // x axis
    removeHiddenCross(X[o^2], Y[o^2], X[o], Y[o], zBuffer, Bounds); // y axis
    removeHiddenCross(X[o^4], Y[o^4], X[o], Y[o], zBuffer, Bounds); // z axis
  
    free(Mem);
    Mem = MemTmp;  // write back values

    free(Bounds);
    free(zBuffer);
  }
  else {
    Lines.append(new qucs::Line(X[o], Y[o], X[o^1], Y[o^1], QPen(Qt::black,0)));
    Lines.append(new qucs::Line(X[o], Y[o], X[o^2], Y[o^2], QPen(Qt::black,0)));
    Lines.append(new qucs::Line(X[o], Y[o], X[o^4], Y[o^4], QPen(Qt::black,0)));
  }

  pMem = Mem;
  return 3;


Frame:   // jump here if error occurred (e.g. impossible log boundings)
  Lines.append(new qucs::Line(0,  y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new qucs::Line(x2, y2, x2,  0, QPen(Qt::black,0)));
  Lines.append(new qucs::Line(0,   0, x2,  0, QPen(Qt::black,0)));
  Lines.append(new qucs::Line(0,  y2,  0,  0, QPen(Qt::black,0)));
  return 0;
}

// ------------------------------------------------------------
// g->Points must already be empty!!!
void Rect3DDiagram::calcData(Graph *g)
{
  if(!pMem)  return;
  if(!g->cPointsY) return;

  int Size = ((2*(g->axis(0)->count) + 1) * g->countY) + 10;
  Size *= 2;  // memory for cross grid lines

  g->resizeScrPoints(Size);
  auto p = g->begin();
  auto p_end = g->begin();
  p_end += Size - 9;   // limit of buffer


  p->setStrokeEnd();
  ++p;
  switch(g->Style) {
    case GRAPHSTYLE_SOLID:
    case GRAPHSTYLE_DASH:
    case GRAPHSTYLE_DOT:
    case GRAPHSTYLE_LONGDASH:
      do {

        while(1) {
          if(pMem->done & 11)    // is grid point ?
            if(pMem->done & 4) { // is hidden
              if(pMem > Mem) {
                if((pMem-1)->done & 12)
                  break;
              }
              else  break;
	    }
          FIT_MEMORY_SIZE;  // need to enlarge memory block ?
	  (p++)->setScr(pMem->x, pMem->y);
          break;
        }

        FIT_MEMORY_SIZE;  // need to enlarge memory block ?
        if(pMem->done & 8)  (p++)->setBranchEnd();

        if(pMem->done & 4)   // point invisible ?
          if((p-1)->isPt())
            (p++)->setStrokeEnd();

      } while(((pMem++)->done & 256) == 0);
      p->setGraphEnd();
      break;

    default:  // symbol (e.g. star) at each point **********************
      do {
        while(1) {
          if(pMem->done & 11)    // is grid point ?
            if(pMem->done & 4) { // is hidden
              if(pMem > Mem) {
                if((pMem-1)->done & 12)
                  break;
              }
              else  break;
	    }

          (p++)->setScr(pMem->x, pMem->y);
          break;
        }

        if(pMem->done & 8)
          (p++)->setBranchEnd();
      } while(((pMem++)->done & 512) == 0);
      p->setGraphEnd();
  }

  return;
}

// ------------------------------------------------------------
// The labels are created during "calcDiagram", but the memory
// for the coordinates is released here.
void Rect3DDiagram::createAxisLabels()
{
  if(Mem)  free (Mem);
  Mem  = 0;
  pMem = 0;
}

// ------------------------------------------------------------
bool Rect3DDiagram::insideDiagram(float x, float y) const
{
  return (regionCode(x, y) == 0);
}

// ------------------------------------------------------------
Diagram* Rect3DDiagram::newOne()
{
  return new Rect3DDiagram();
}

// ------------------------------------------------------------
Element* Rect3DDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("3D-Cartesian");
  BitmapFile = (char *) "rect3d";

  if(getNewOne)  return new Rect3DDiagram();
  return 0;
}

// vim:ts=8:sw=2:noet
