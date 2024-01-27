/***************************************************************************
                          marker.cpp  -  description
                             -------------------
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

/*!
  \class Marker
  \brief The Marker class implements the marker object used for all the
         diagram
*/

#include "marker.h"
#include "diagram.h"
#include "graph.h"
#include "main.h"

#include <QString>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

#include <limits.h>
#include <cmath>
#include <stdlib.h>

#include "misc.h"

static double default_Z0=50;

#define IND_SIZE 8

/*!
 * create a marker based on click position and
 * the branch number.
 *
 * the click position is used to compute the marker position. currently, the
 * marker position is the sampling point closest to the click.
 */

Marker::Marker(Graph *pg_, int branchNo, int cx_, int cy_) :
  Element(),
  pGraph(pg_),
  Precision(3),
  numMode(0),
  indicatorMode(indicator_Triangle),
  Z0(default_Z0) // BUG: see declaration.
{
  Type = isMarker;
  isSelected = transparent = false;

  cx =  cx_;
  cy = -cy_;
  fCX = float(cx);
  fCY = float(cy);
  if(!pGraph){
    makeInvalid();
  }else{
    initText(branchNo);   // finally create marker
    createText();
  }

  x1 =  cx + 60;
  y1 = -cy - 60;

}

Marker::~Marker()
{
}

// ---------------------------------------------------------------------
/*!
 * compute VarPos from branch number n and click position (cx, cy)
 * this is done by recreating branch samples and comparing against click
 *
 * FIXME: should use ScrPoints instead. do not call calcCoordinate from here!
 */
void Marker::initText(int n)
{
  if(pGraph->isEmpty()) {
      makeInvalid();
      return;
  }

  Axis const *pa;
  assert(diag());
  if(pGraph->yAxisNo == 0)  pa = &(diag()->yAxis);
  else  pa = &(diag()->zAxis);
  double Dummy = 0.0;   // needed for 2D graph in 3D diagram
  double *px, *py=&Dummy, *pz;
  Text = "";

  bool isCross = false;
  int nn, nnn, m, x, y, d, dmin = INT_MAX;
  DataX const *pD = pGraph->axis(0);
  px  = pD->Points;
  nnn = pD->count;
  DataX const *pDy = pGraph->axis(1);
  if(pDy) {   // only for 3D diagram
    nn = pGraph->countY * pD->count;
    py  = pDy->Points;
    if(n >= nn) {    // is on cross grid ?
      isCross = true;
      n -= nn;
      n /= nnn;
      px += (n % nnn);
      if(pGraph->axis(2))   // more than 2 indep variables ?
        n  = (n % nnn) + (n / nnn) * nnn * pDy->count;
      nnn = pDy->count;
    }
    else py += (n/pD->count) % pDy->count;
  }

  // find exact marker position
  m  = nnn - 1;
  pz = pGraph->cPointsY + 2*n;
  for(nn=0; nn<nnn; nn++) {
    diag()->calcCoordinate(px, pz, py, &fCX, &fCY, pa);
    ++px;
    pz += 2;
    if(isCross) {
      px--;
      py++;
      pz += 2*(pD->count-1);
    }
    x = int(fCX+0.5) - cx;
    y = int(fCY+0.5) - cy;
    d = x*x + y*y;
    if(d < dmin) {
      dmin = d;
      m = nn;
    }
  }
  if(isCross) m *= pD->count;
  n += m;

  // why check over and over again?! do in the right place and just assert otherwise.
  if(VarPos.size() != pGraph->numAxes()){
    qDebug() << "huh, wrong size" << VarPos.size() << pGraph->numAxes();
    VarPos.resize(pGraph->numAxes());
  }

  // gather text of all independent variables
  nn = n;
  for(unsigned i=0; (pD = pGraph->axis(i)); ++i) {
    px = pD->Points + (nn % pD->count);
    VarPos[i] = *px;
    Text += pD->Var + ": " + QString::number(*px,'g',Precision) + "\n";
    nn /= pD->count;
  }

  // createText();
}

// ---------------------------------------------------------------------
/*!
 * (should)
 * create marker label Text the screen position cx and cy from VarPos.
 * does a lot of fancy stuff to be sorted out.
 */
void Marker::createText()
{
  if(!(pGraph->cPointsY)) {
    makeInvalid();
    return;
  }

  unsigned nVarPos = VarPos.size();

  if(nVarPos > pGraph->numAxes()){
    qDebug() << "huh, VarPos too big?!";
  }
  if(nVarPos != pGraph->numAxes()){
    qDebug() << "padding" << VarPos.size() << pGraph->numAxes();
    VarPos.resize(pGraph->numAxes());
    while((unsigned int)nVarPos < pGraph->numAxes()){
      VarPos[nVarPos++] = 0.; // pad
    }
  }

  // independent variables
  Text = "";
  double *pp;
  nVarPos = pGraph->numAxes();
  DataX const *pD;

  auto p = pGraph->findSample(VarPos);
  VarDep[0] = p.first;
  VarDep[1] = p.second;

  double v=0.;   // needed for 2D graph in 3D diagram
  double *py=&v;
  pD = pGraph->axis(0);
  if(pGraph->axis(1)) {
    *py = VarPos[1];
  }else{
    qDebug() << *py << "is not" << VarPos[1]; // does it really matter?!
  }

  double pz[2];
  pz[0] = VarDep[0];
  pz[1] = VarDep[1];

  // now actually create text.
  bool engNotation = pGraph->parentDiagram()->engineeringNotation;
  for(unsigned ii=0; (pD=pGraph->axis(ii)); ++ii) {
    Text += pD->Var + ": ";
    if (engNotation) {
        Text += misc::num2str(VarPos[ii],Precision) + "\n";
    } else {
        Text += QString::number(VarPos[ii],'g',Precision) + "\n";
    }
  }

  Text += pGraph->Var + ": ";
  const Axis *ax = &(diag()->yAxis);
  if (pGraph->yAxisNo > 0) ax = &(diag()->zAxis);
  int units = ax->Units;
  if (units == Axis::NoUnits || !ax->log) {
      switch(numMode) {
      case nM_Rect: Text += misc::complexRect(*pz, *(pz+1), Precision);
          break;
      case nM_Deg: Text += misc::complexDeg(*pz, *(pz+1), Precision);
          break;
      case nM_Rad: Text += misc::complexRad(*pz, *(pz+1), Precision);
          break;
      }
  } else {
      double mag = sqrt(pz[0]*pz[0] + pz[1]*pz[1]);
      double val = qucs::num2db(mag,ax->Units);
      if (engNotation) {
          Text += misc::num2str(val,Precision) + "\n";
      } else {
          Text += QString::number(val,'g',Precision);
      }
  }

  assert(diag());
  Text += diag()->extraMarkerText(this);

  Axis const *pa;
  if(pGraph->yAxisNo == 0)  pa = &(diag()->yAxis);
  else  pa = &(diag()->zAxis);
  pp = &(VarPos[0]);

  diag()->calcCoordinate(pp, pz, py, &fCX, &fCY, pa);
  diag()->finishMarkerCoordinates(fCX, fCY);

  cx = int(fCX+0.5);
  cy = int(fCY+0.5);
  getTextSize();
}

// ---------------------------------------------------------------------
void Marker::makeInvalid()
{
  fCX = fCY = -1e3; // invalid coordinates
  assert(diag());
  diag()->finishMarkerCoordinates(fCX, fCY); // leave to diagram
  cx = int(fCX+0.5);
  cy = int(fCY+0.5);

  Text = QObject::tr("invalid");
  getTextSize();
}

// ---------------------------------------------------------------------
void Marker::getTextSize()
{
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  QSize r = metrics.size(0, Text);
  x2 = r.width()+5;
  y2 = r.height()+5;
}

// ---------------------------------------------------------------------
bool Marker::moveLeftRight(bool left)
{
  int n;
  double *px;

  DataX const *pD = pGraph->axis(0);
  px = pD->Points;
  if(!px) return false;
  for(n=0; n<pD->count; n++) {
    if(VarPos[0] <= *px) break;
    px++;
  }
  if(n == pD->count) px--;

  if(left) {
    if(px <= pD->Points) return false;
    px--;  // one position to the left
  }
  else {
    if(px >= (pD->Points + pD->count - 1)) return false;
    px++;  // one position to the right
  }
  VarPos[0] = *px;
  createText();

  return true;
}

// ---------------------------------------------------------------------
bool Marker::moveUpDown(bool up)
{
  int n, i=0;
  double *px;

  DataX const *pD = pGraph->axis(0);
  if(!pD) return false;

  if(up) {  // move upwards ? **********************
    do {
      pD = pGraph->axis(++i);
      if(!pD) return false;
      px = pD->Points;
      if(!px) return false;
      for(n=1; n<pD->count; n++) {  // go through all data points
        if(fabs(VarPos[i]-(*px)) < fabs(VarPos[i]-(*(px+1)))) break;
        px++;
      }

    } while(px >= (pD->Points + pD->count - 1));  // go to next dimension ?

    px++;  // one position up
    VarPos[i] = *px;
    while(i > 1) {
      pD = pGraph->axis(--i);
      VarPos[i] = *(pD->Points);
    }
  }
  else {  // move downwards **********************
    do {
      pD = pGraph->axis(++i);
      if(!pD) return false;
      px = pD->Points;
      if(!px) return false;
      for(n=0; n<pD->count; n++) {
        if(fabs(VarPos[i]-(*px)) < fabs(VarPos[i]-(*(px+1)))) break;
        px++;
      }

    } while(px <= pD->Points);  // go to next dimension ?

    px--;  // one position down
    VarPos[i] = *px;
    while(i > 1) {
      pD = pGraph->axis(--i);
      VarPos[i] = *(pD->Points + pD->count - 1);
    }
  }
  createText();

  return true;
}

// ---------------------------------------------------------------------
void Marker::paint(ViewPainter *p, int x0, int y0)
{
  // keep track of painter state
  p->Painter->save();

  // Workaround for bug in Qt: If WorldMatrix is turned off, \n in the
  // text creates a terrible mess.
  p->Painter->setWorldMatrixEnabled(true);
  QTransform wm = p->Painter->worldTransform();
  p->Painter->setWorldTransform(QTransform());

  int x2_, y2_;
  p->Painter->setPen(QPen(Qt::black,1));
  x2_ = p->drawText(Text, x0+x1+3, y0+y1+3, &y2_);
  x2_ += int(6.0*p->Scale);
  y2_ += int(6.0*p->Scale);
  if(!transparent) {
    p->eraseRect(x0+x1, y0+y1, x2_, y2_);
    p->drawText(Text, x0+x1+3, y0+y1+3);
  }
  p->Painter->setWorldTransform(wm);
  p->Painter->setWorldMatrixEnabled(false);

  // restore painter state
  p->Painter->restore();

  p->Painter->setPen(QPen(Qt::darkMagenta,0));
  p->drawRectD(x0+x1, y0+y1, x2_, y2_);

  x2 = int(float(x2_) / p->Scale);
  y2 = int(float(y2_) / p->Scale);

  int x1_, y1_;
  p->map(x0+x1, y0+y1, x1_, y1_);
  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(-cy >= y1+(y2>>1))
      y1_ += y2_ - 1;
  }
  else {
    x1_ += x2_ - 1;
    if(-cy >= y1+(y2>>1))
      y1_ += y2_ - 1;
  }
  float fx2, fy2;
  fx2 = (float(x0)+fCX)*p->Scale + p->DX;
  fy2 = (float(y0)-fCY)*p->Scale + p->DY;
  p->Painter->drawLine(x1_, y1_, lround(fx2), lround(fy2));

  if (indicatorMode == indicator_Square) {
    p->Painter->drawRect(fx2 - (IND_SIZE / 2 * p->Scale), fy2 - (IND_SIZE / 2 * p->Scale), 
                            IND_SIZE * p->Scale, IND_SIZE * p->Scale);
  }
  else if (indicatorMode == indicator_Triangle) {
    // Creating a path in case a filled triangle is needed.
    QPainterPath path;
    path.moveTo(fx2, fy2);
    path.lineTo(fx2 - (IND_SIZE / 2 * p->Scale), fy2 - (IND_SIZE * p->Scale));
    path.lineTo(fx2 + (IND_SIZE / 2 * p->Scale), fy2 - (IND_SIZE * p->Scale));
    path.lineTo(fx2, fy2);
    p->Painter->drawPath(path);
  }

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(x0+x1-3, y0+y1-3, x2+6, y2+6);
  }
}

// ---------------------------------------------------------------------
void Marker::paintScheme(QPainter *p)
{
  assert(diag());
  int x0 = diag()->cx;
  int y0 = diag()->cy;
  p->drawRect(x0+x1, y0+y1, x2, y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(-cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1, y0+y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1, y0+y1+y2-1);
  }
  else {
    if(-cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1+x2-1, y0+y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1+x2-1, y0+y1+y2-1);
  }
}

// ------------------------------------------------------------
void Marker::setCenter(int x, int y, bool relative)
{
  if(relative) {
    x1 += x;  y1 += y;
  }
  else {
    x1 = x;  y1 = y;
  }
}

// -------------------------------------------------------
void Marker::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(diag()) {
    _x1 = diag()->cx + x1;
    _y1 = diag()->cy + y1;
    _x2 = diag()->cx + x1+x2;
    _y2 = diag()->cy + y1+y2;
  }
  else {
    _x1 = x1;
    _y1 = y1+y2;
    _x2 = x1+x2;
    _y2 = y1;
  }
}

// ---------------------------------------------------------------------
QString Marker::save()
{
  QString s  = "<Mkr ";

  for(auto i : VarPos){
    s += QString::number(i)+"/";
  }
  s.replace(s.length()-1,1,' ');
  //s.at(s.length()-1) = (const QChar&)' ';

  s += QString::number(x1) +" "+ QString::number(y1) +" "
      +QString::number(Precision) +" "+ QString::number(numMode);
  if(transparent)  s += " 1>";
  else  s += " 0>";

  return s;
}

// ---------------------------------------------------------------------
// All graphs must have been loaded before this function !
bool Marker::load(const QString& Line)
{
  bool ok;
  QString s = Line;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  if(s.section(' ',0,0) != "Mkr") return false;

  int i=0, j;
  QString n = s.section(' ',1,1);    // VarPos

  unsigned nVarPos = 0;
  j = (n.count('/') + 3);
  VarPos.resize(j);

  do {
    j = n.indexOf('/', i);
    VarPos[nVarPos++] = n.mid(i,j-i).toDouble(&ok);
    if(!ok) return false;
    i = j+1;
  } while(j >= 0);

  n  = s.section(' ',2,2);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);      // Precision
  Precision = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);      // numMode
  numMode = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);      // transparent
  if(n.isEmpty()) return true;  // is optional
  if(n == "0")  transparent = false;
  else  transparent = true;

  return true;
}

// ------------------------------------------------------------------------
// Checks if the coordinates x/y point to the marker text. x/y are relative
// to diagram cx/cy.
bool Marker::getSelected(int x_, int y_)
{
  if(x_ >= x1) if(x_ <= x1+x2) if(y_ >= y1) if(y_ <= y1+y2)
    return true;

  return false;
}

// ------------------------------------------------------------------------
/*
 * the diagram this belongs to
 */
const Diagram* Marker::diag() const
{
  if(!pGraph) return NULL;
  return pGraph->parentDiagram();
}

// ------------------------------------------------------------------------
Marker* Marker::sameNewOne(Graph *pGraph_)
{
  Marker *pm = new Marker(pGraph_, 0, cx ,cy);

  pm->x1 = x1;  pm->y1 = y1;
  pm->x2 = x2;  pm->y2 = y2;

  pm->VarPos = VarPos;

  pm->Text        = Text;
  pm->transparent = transparent;
  pm->Precision   = Precision;
  pm->numMode     = numMode;

  return pm;
}

// vim:ts=8:sw=2:noet
