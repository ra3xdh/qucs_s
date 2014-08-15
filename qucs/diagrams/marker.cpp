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
#include <QtGui>
#include "marker.h"
#include "diagram.h"
#include "graph.h"
#include "main.h"
#include "../dialogs/matchdialog.h" //For r2z function

#include <QString>
#include <QWidget>
#include <QPainter>

#include <limits.h>
#include <math.h>
#include <stdlib.h>


#include "qucs.h"
#include "schematic.h"



Marker::Marker(Diagram *Diag_, Graph *pg_, int _nn, int cx_, int cy_)
{
  Type = isMarker;
  isSelected = transparent = false;

  Diag   = Diag_;
  pGraph = pg_;
  Precision = 3;   // before createText()
  VarPos = 0;
  numMode = nVarPos = 0;
  cx =  cx_;
  cy = -cy_;
  fCX = float(cx);
  fCY = float(cy);
  Z0 = 50;		//Used for Smith chart marker, to calculate impedance
  if(!pGraph)  makeInvalid();
  else initText(_nn);   // finally create marker

  x1 =  cx + 60;
  y1 = -cy - 60;

}

Marker::~Marker()
{
  if(VarPos)  free(VarPos);
}

// ---------------------------------------------------------------------
void Marker::initText(int n)
{
  if(pGraph->cPointsX.isEmpty()) {
      makeInvalid();
      return;
  }

  Axis *pa;
  if(pGraph->yAxisNo == 0)  pa = &(Diag->yAxis);
  else  pa = &(Diag->zAxis);
  double Dummy = 0.0;   // needed for 2D graph in 3D diagram
  double *px, *py=&Dummy, *pz;
  Text = "";

  bool isCross = false;
  int nn, nnn, m, x, y, d, dmin = INT_MAX;
  DataX *pD = pGraph->cPointsX.first();
  px  = pD->Points;
  nnn = pD->count;
  DataX *pDy = pGraph->cPointsX.next();
  if(pDy) {   // only for 3D diagram
    nn = pGraph->countY * pD->count;
    py  = pDy->Points;
    if(n >= nn) {    // is on cross grid ?
      isCross = true;
      n -= nn;
      n /= nnn;
      px += (n % nnn);
      if(pGraph->cPointsX.next())   // more than 2 indep variables ?
        n  = (n % nnn) + (n / nnn) * nnn * pDy->count;
      nnn = pDy->count;
    }
    else py += (n/pD->count) % pDy->count;
  }

  // find exact marker position
  m  = nnn - 1;
  pz = pGraph->cPointsY + 2*n;
  for(nn=0; nn<nnn; nn++) {
    Diag->calcCoordinate(px, pz, py, &fCX, &fCY, pa);
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

  nVarPos = 0;
  nn = (pGraph->cPointsX.count() + 2) * sizeof(double);
  if(VarPos)
    VarPos = (double*)realloc(VarPos, nn);
  else
    VarPos = (double*)malloc(nn);

  // gather text of all independent variables
  nn = n;
  for(pD = pGraph->cPointsX.first(); pD!=0; pD = pGraph->cPointsX.next()) {
    px = pD->Points + (nn % pD->count);
    VarPos[nVarPos++] = *px;
    Text += pD->Var + ": " + QString::number(*px,'g',Precision) + "\n";
    nn /= pD->count;
  }

  // gather text of dependent variable
  pz = pGraph->cPointsY + 2*n;
  Text += pGraph->Var + ": ";
  switch(numMode) {
    case 0: Text += complexRect(*pz, *(pz+1), Precision);
            break;
    case 1: Text += complexDeg(*pz, *(pz+1), Precision);
            break;
    case 2: Text += complexRad(*pz, *(pz+1), Precision);
            break;
  }
  VarPos[nVarPos] = *pz;
  VarPos[nVarPos+1] = *(pz+1);
  if(Diag->Name=="Smith") //impedance is useful as well here
  {
    double Zr, Zi;
    Zr = *pz;
    Zi = *(pz+1);

    MatchDialog::r2z(Zr, Zi, Z0);
    QString Var = pGraph->Var;
    if(Var.startsWith("S"))
        Text += "\n"+ Var.replace('S', 'Z')+": " +complexRect(Zr, Zi, Precision);
    else
        Text += "\nZ("+ Var+"): " +complexRect(Zr, Zi, Precision);
  }
  px = VarPos;
  if(py != &Dummy)   // 2D in 3D diagram ?
    py = VarPos + 1;
  Diag->calcCoordinate(px, pz, py, &fCX, &fCY, pa);

  if(!Diag->insideDiagram(fCX, fCY)) {
    // if marker out of valid bounds, point to origin
    if((Diag->Name.left(4) != "Rect") && (Diag->Name != "Curve")) {
      fCX = float(Diag->x2 >> 1);
      fCY = float(Diag->y2 >> 1);
    }
    else
      fCX = fCY = 0.0;
  }

  cx = int(fCX+0.5);
  cy = int(fCY+0.5);
  getTextSize(QucsSettings.font);
}

// ---------------------------------------------------------------------
void Marker::createText()
{
  if(!(pGraph->cPointsY)) {
    makeInvalid();
    return;
  }

  VarPos = (double*)realloc(VarPos,
              (pGraph->cPointsX.count() + 2) * sizeof(double));

  while((unsigned int)nVarPos < pGraph->cPointsX.count())
    VarPos[nVarPos++] = 0.0;   // fill up VarPos


  // independent variables
  Text = "";
  double *pp, v;
  int n = 0, m = 1, i;
  DataX *pD;
  nVarPos = 0;
  for(pD = pGraph->cPointsX.first(); pD!=0; pD = pGraph->cPointsX.next()) {
    pp = pD->Points;
    v  = VarPos[nVarPos];
    for(i=pD->count; i>1; i--) {  // find appropiate marker position
      if(fabs(v-(*pp)) < fabs(v-(*(pp+1)))) break;
      pp++;
      n += m;
    }

    m *= pD->count;
    VarPos[nVarPos++] = *pp;
    Text += pD->Var + ": " + QString::number(*pp,'g',Precision) + "\n";
  }


  v = 0.0;   // needed for 2D graph in 3D diagram
  double *py=&v, *pz = pGraph->cPointsY + 2*n;
  pD = pGraph->cPointsX.first();
  if(pGraph->cPointsX.next()) {
    py = pGraph->cPointsX.current()->Points;   // only for 3D diagram
    py += (n / pD->count) % pGraph->cPointsX.current()->count;
  }

  Text += pGraph->Var + ": ";
  switch(numMode) {
    case 0: Text += complexRect(*pz, *(pz+1), Precision);
      break;
    case 1: Text += complexDeg(*pz, *(pz+1), Precision);
      break;
    case 2: Text += complexRad(*pz, *(pz+1), Precision);
      break;
  }
  if(Diag->Name=="Smith") //impedance is useful as well here
  {
	double Zr, Zi; 
	Zr = *pz;
	Zi = *(pz+1);
	  
	MatchDialog::r2z(Zr, Zi, Z0);
	QString Var = pGraph->Var;
	if(Var.startsWith("S"))
  		Text += "\n"+ Var.replace('S', 'Z')+": " +complexRect(Zr, Zi, Precision);
	else
		Text += "\nZ("+ Var+"): " +complexRect(Zr, Zi, Precision);
  }
  VarPos[nVarPos] = *pz;
  VarPos[nVarPos+1] = *(pz+1);

  Axis *pa;
  if(pGraph->yAxisNo == 0)  pa = &(Diag->yAxis);
  else  pa = &(Diag->zAxis);
  pp = &(VarPos[0]);

  Diag->calcCoordinate(pp, pz, py, &fCX, &fCY, pa);

  if(!Diag->insideDiagram(fCX, fCY)) {
    // if marker out of valid bounds, point to origin
    if((Diag->Name.left(4) != "Rect") && (Diag->Name != "Curve")) {
      fCX = float(Diag->x2 >> 1);
      fCY = float(Diag->y2 >> 1);
    }
    else
      fCX = fCY = 0.0;
  }

  cx = int(fCX+0.5);
  cy = int(fCY+0.5);
  getTextSize(QucsSettings.font);
}

// ---------------------------------------------------------------------
void Marker::makeInvalid()
{
  cx = 0;
  cy = 0;
  if(Diag) if(Diag->Name.left(4) != "Rect") if(Diag->Name != "Curve") {
    cx = Diag->x2 >> 1;
    cy = Diag->y2 >> 1;
  }
  Text = QObject::tr("invalid");

  fCX = float(cx);
  fCY = float(cy);
  getTextSize(QucsSettings.font);
}

// ---------------------------------------------------------------------
void Marker::getTextSize(const QFont& Font)
{
  QFontMetrics  metrics(((Schematic*)QucsMain->DocumentTab->currentPage())->font());   // get size of text
  QSize r = metrics.size(0, Text);
  x2 = r.width()+5;
  y2 = r.height()+5;
}

// ---------------------------------------------------------------------
bool Marker::moveLeftRight(bool left)
{
  int n;
  double *px;

  DataX *pD = pGraph->cPointsX.getFirst();
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

  DataX *pD = pGraph->cPointsX.first();
  if(!pD) return false;

  if(up) {  // move upwards ? **********************
    do {
      i++;
      pD = pGraph->cPointsX.next();
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
      pD = pGraph->cPointsX.prev();
      i--;
      VarPos[i] = *(pD->Points);
    }
  }
  else {  // move downwards **********************
    do {
      i++;
      pD = pGraph->cPointsX.next();
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
      pD = pGraph->cPointsX.prev();
      i--;
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
  p->Painter->setWorldXForm(true);
  QMatrix wm = p->Painter->worldMatrix();
  p->Painter->setWorldMatrix(QMatrix());

  int x2_, y2_;
  p->Painter->setPen(QPen(Qt::black,1));
  x2_ = p->drawText(Text, x0+x1+3, y0+y1+3, &y2_);
  x2_ += int(6.0*p->Scale);
  y2_ += int(6.0*p->Scale);
  if(!transparent) {
    p->eraseRect(x0+x1, y0+y1, x2_, y2_);
    p->drawText(Text, x0+x1+3, y0+y1+3);
  }
  p->Painter->setWorldMatrix(wm);
  p->Painter->setWorldXForm(false);

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
  p->Painter->drawLine(x1_, y1_, TO_INT(fx2), TO_INT(fy2));

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(x0+x1-3, y0+y1-3, x2+6, y2+6);
  }
}

// ---------------------------------------------------------------------
void Marker::paintScheme(QPainter *p)
{
  int x0 = Diag->cx;
  int y0 = Diag->cy;
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
  if(Diag) {
    _x1 = Diag->cx + x1;
    _y1 = Diag->cy + y1;
    _x2 = Diag->cx + x1+x2;
    _y2 = Diag->cy + y1+y2;
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

  for(int i=0; i<nVarPos; i++)
    s += QString::number(VarPos[i])+"/";
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
bool Marker::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  if(s.section(' ',0,0) != "Mkr") return false;

  int i=0, j;
  QString n = s.section(' ',1,1);    // VarPos

  nVarPos = 0;
  j = (n.count('/') + 3) * sizeof(double);
  if(VarPos)
    VarPos = (double*)realloc(VarPos, j);
  else
    VarPos = (double*)malloc(j);

  do {
    j = n.find('/', i);
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
Marker* Marker::sameNewOne(Graph *pGraph_)
{
  Marker *pm = new Marker(Diag, pGraph_, 0, cx ,cy);

  pm->x1 = x1;  pm->y1 = y1;
  pm->x2 = x2;  pm->y2 = y2;

  int z = (nVarPos+2) * sizeof(double);
  if(pm->VarPos)
    pm->VarPos = (double*)realloc(pm->VarPos, z);
  else
    pm->VarPos = (double*)malloc(z);

  pm->nVarPos = nVarPos;
  for(z=0; z<nVarPos; z++)
    pm->VarPos[z] = VarPos[z];

  pm->Text        = Text;
  pm->transparent = transparent;
  pm->Precision   = Precision;
  pm->numMode     = numMode;

  return pm;
}
