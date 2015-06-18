/***************************************************************************
                          graph.cpp  -  description
                             -------------------
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
#include "graph.h"

#include <stdlib.h>

#include <QPainter>

Graph::Graph(const QString& _Line) : Element(),
  Style(GRAPHSTYLE_SOLID)
{
  Type = isGraph;

  Var    = _Line;
  countY = 0;    // no points in graph
  Thick  = numMode = 0;
  Color  = 0x0000ff;   // blue
  Precision  = 3;
  isSelected = false;
  yAxisNo = 0;   // left y axis

  cPointsY = 0;

  cPointsX.setAutoDelete(true);
}

Graph::~Graph()
{
  if(cPointsY != 0)
    delete[] cPointsY;
}

// ---------------------------------------------------------------------
void Graph::paint(ViewPainter *p, int x0, int y0)
{
  if(!ScrPoints.size())
    return;

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,Thick*p->PrintScale+4));
    paintLines(p, x0, y0);

    p->Painter->setPen(QPen(Qt::white, Thick*p->PrintScale, Qt::SolidLine));
    paintLines(p, x0, y0);
    return;
  }

  // **** not selected ****
  p->Painter->setPen(QPen(QColor(Color), Thick*p->PrintScale, Qt::SolidLine));
  paintLines(p, x0, y0);
}

// ---------------------------------------------------------------------
void Graph::paintLines(ViewPainter *p, int x0, int y0)
{
  switch(Style) {
    case GRAPHSTYLE_STAR:
      drawStarSymbols(x0, y0, p);
      break;
    case GRAPHSTYLE_CIRCLE:
      drawCircleSymbols(x0, y0, p);
      break;
    case GRAPHSTYLE_ARROW:
      drawArrowSymbols(x0, y0, p);
      break;
    default:
      drawLines(x0, y0, p);
  }
}

// ---------------------------------------------------------------------
QString Graph::save()
{
  QString s = "\t<\""+Var+"\" "+Color.name()+
	      " "+QString::number(Thick)+" "+QString::number(Precision)+
	      " "+QString::number(numMode)+" "+QString::number(Style)+
	      " "+QString::number(yAxisNo)+">";

  foreach(Marker *pm, Markers)
    s += "\n\t  "+pm->save();

  return s;
}

// ---------------------------------------------------------------------
bool Graph::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  Var = s.section('"',1,1);  // Var

  QString n;
  n  = s.section(' ',1,1);    // Color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  n  = s.section(' ',2,2);    // Thick
  Thick = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // Precision
  Precision = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // numMode
  numMode = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // Style
  int st = n.toInt(&ok);
  if(!ok) return false;
  Style = toGraphStyle(st);
  if(Style==GRAPHSTYLE_INVALID) return false;

  n  = s.section(' ',6,6);    // yAxisNo
  if(n.isEmpty()) return true;   // backward compatible
  yAxisNo = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// -----------------------------------------------------------------------
// Checks if the coordinates x/y point to the graph. x/y are relative to
// diagram cx/cy. 5 is the precision the user must point onto the graph.
int Graph::getSelected(int x, int y)
{
  auto pp = ScrPoints.begin();
  if(pp == ScrPoints.end()) return -1;

  int A, z=0;
  int dx, dx2, x1;
  int dy, dy2, y1;

  int countX = cPointsX.getFirst()->count;
  if(pp->Scr <= STROKEEND) {
    if(pp->Scr <= BRANCHEND) z++;
    pp++;
    if(pp->Scr <= BRANCHEND) {
      if(pp->Scr <= GRAPHEND)  return -1;   // not even one point ?
      z++;
      pp++;
      if(pp->Scr < BRANCHEND)  return -1;   // not even one point ?
    }
  }

  if(Style >= GRAPHSTYLE_STAR) {
    // for graph symbols
    while(pp->Scr > GRAPHEND) {
      if(pp->Scr > STROKEEND) {
        dx  = x - int((++pp)->Scr);
        dy  = y - int((++pp)->Scr);

        if(dx < -5) continue;
        if(dx >  5) continue;
        if(dy < -5) continue;
        if(dy >  5) continue;
        return z*countX;   // points on graph symbol
      }
      else {
        z++;   // next branch
        pp++;
      }
    }
    return -1;
  }

  // for graph lines
  while(pp->Scr > GRAPHEND) {
    while(pp->Scr >= STROKEEND) {
      x1 = int((++pp)->Scr);
      y1 = int((++pp)->Scr);
      dx  = x - x1;
      dy  = y - y1;

      dx2 = int(pp->Scr);
      if(dx2 <= STROKEEND) {  // end of stroke ?
        if(dx2 <= BRANCHEND) break;
        pp++;
        dx2 = int(pp->Scr);  // go on as graph can also be selected between strokes
        if(dx2 <= BRANCHEND) break;
      }
      if(dx < -5) { if(x < dx2-5) continue; } // point between x coordinates ?
      else { if(x > 5) if(x > dx2+5) continue; }

      dy2 = int((pp+1)->Scr);
      if(dy < -5) { if(y < dy2-5) continue; } // point between y coordinates ?
      else { if(y > 5) if(y > dy2+5) continue; }

      dx2 -= x1;
      dy2 -= y1;

      A  = dx2*dy - dx*dy2;    // calculate the rectangle area spanned
      A *= A;                  // avoid the need for square root
      A -= 25*(dx2*dx2 + dy2*dy2);  // substract selectable area

      if(A <= 0)  return z*countX;  // lies x/y onto the graph line ?
    }
    pp++;
    z++;
  }

  return -1;
}

// -----------------------------------------------------------------------
// Creates a new graph and copies all the properties into it.
Graph* Graph::sameNewOne()
{
  Graph *pg = new Graph(Var);

  pg->Color = Color;
  pg->Thick = Thick;
  pg->Style = Style;

  pg->Precision = Precision;
  pg->numMode   = numMode;
  pg->yAxisNo   = yAxisNo;

  foreach(Marker *pm, Markers)
    pg->Markers.append(pm->sameNewOne(pg));

  return pg;
}

// vim:ts=8:sw=2:et
