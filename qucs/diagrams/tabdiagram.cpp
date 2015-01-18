/***************************************************************************
                               tabdiagram.cpp
                              ----------------
    begin                : Fri Oct 24 2003
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
  \class TabDiagram
  \brief The TabDiagram class implements the Tabular diagram
*/

#include <QFontMetrics>
#include <QPolygon>
#include <QPainter>

#include "tabdiagram.h"
#include "main.h"
#include <cmath>
#include "misc.h"


TabDiagram::TabDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 0;    // no extension to select area
  y1 = 0;
  x2 = x3 = 300;  // initial size of diagram
  y2 = 200;
  Name = "Tab";
  xAxis.limit_min = 0.0;  // scroll bar position (needs to be saved in file)

  calcDiagram();
}

TabDiagram::~TabDiagram()
{
}

// ------------------------------------------------------------
void TabDiagram::paint(ViewPainter *p)
{
  // paint all lines
  foreach(Line *pl, Lines) {
    p->Painter->setPen(pl->style);
    p->drawLine(cx+pl->x1, cy-pl->y1, cx+pl->x2, cy-pl->y2);
  }

  if(x1 > 0) {  // paint scroll bar ?
    int   x, y, dx, dy;
    QPolygon Points;
    y = y2 - 20;
    // draw scroll bar
    int by = cy-y + yAxis.numGraphs;
    p->fillRect(cx-14, by+1, 12, zAxis.numGraphs-1, QColor(192, 192, 192));

    // draw frame for scroll bar
    p->Painter->setPen(QPen(Qt::black,0));
    p->drawLine(cx-17, cy-y2, cx-17, cy);
    p->drawLine(cx-17, cy-y2, cx, cy-y2);
    p->drawLine(cx-17, cy, cx, cy);
    y += 2;
    p->drawLine(cx-17, cy-y, cx, cy-y);
    y -= y2;
    p->drawLine(cx-17, cy+y, cx, cy+y);

    // draw the arrows above and below the scroll bar
    p->Painter->setBrush(QColor(192, 192, 192));
    p->Painter->setPen(QColor(152, 152, 152));
    p->drawLine(cx-2, by, cx-2, by + zAxis.numGraphs);
    p->drawLine(cx-15, by + zAxis.numGraphs, cx-2, by + zAxis.numGraphs);

    p->map(cx-14, cy-y2+3, x, y);
    p->map(cx-3,  cy-y2+14, dx, dy);
    Points.setPoints(3, x, dy, (x+dx)>>1, y, dx, dy);
    p->Painter->drawConvexPolygon(Points);
    p->Painter->setPen(QColor(224, 224, 224));
    p->Painter->drawLine(x, dy, (x+dx)>>1, y);
    p->drawLine(cx-15, by, cx-2, by);
    p->drawLine(cx-15, by, cx-15, by + zAxis.numGraphs);

    p->Painter->setPen(QColor(152, 152, 152));
    dy -= y;
    p->map(cx-14,  cy-3, x, y);
    Points.setPoints(3, x, y-dy, (x+dx)>>1, y, dx, y-dy);
    p->Painter->drawConvexPolygon(Points);
    p->Painter->setPen(QColor(208, 208, 208));
    p->Painter->drawLine(x, y-dy, (x+dx)>>1, y);
    p->Painter->setPen(QColor(224, 224, 224));
    p->Painter->drawLine(x, y-dy, dx, y-dy);

    p->Painter->setBrush(QBrush(Qt::NoBrush));
  }


  p->Painter->setPen(Qt::black);
  // write whole text
  foreach(Text *pt, Texts)
    p->drawText(pt->s, cx+pt->x, cy-pt->y);


  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->drawRect(cx-5, cy-y2-5, x2+10, y2+10);
    p->Painter->setPen(QPen(Qt::darkRed,2));
    p->drawResizeRect(cx, cy-y2);  // markers for changing the size
    p->drawResizeRect(cx, cy);
    p->drawResizeRect(cx+x2, cy-y2);
    p->drawResizeRect(cx+x2, cy);
  }
}

// ------------------------------------------------------------
// calculates the text in the tabular
int TabDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x1 = 0;  // no scroll bar
  x3 = x2;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  int tHeight = metrics.lineSpacing();
  QString Str;
  int colWidth=0, x=8, y;

  if(y2 < (41 + MIN_SCROLLBAR_SIZE))
    y2 = 41 + MIN_SCROLLBAR_SIZE;

  if(y2 < (tHeight + 8))
    y2 = tHeight + 8;
  y = y2 - tHeight - 6;

  // outer frame
  Lines.append(new Line(0, y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new Line(0, y2, 0, 0, QPen(Qt::black,0)));
  Lines.append(new Line(x2, y2, x2, 0, QPen(Qt::black,0)));
  Lines.append(new Line(0, 0, x2, 0, QPen(Qt::black,0)));
  Lines.append(new Line(0, y+2, x2, y+2, QPen(Qt::black,2)));

  if(xAxis.limit_min < 0.0)
    xAxis.limit_min = 0.0;

  Graph *firstGraph;

  QListIterator<Graph *> ig(Graphs);
  Graph *g = 0;
  if (ig.hasNext()) // point to first graph
    g = ig.next();

  if(g == 0) {  // no variables specified in diagram ?
    Str = QObject::tr("no variables");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth >= 0)
      Texts.append(new Text(x-4, y2-2, Str)); // independent variable
    return 0;
  }

  int NumAll=0;   // how many numbers per column
  int NumLeft=0;  // how many numbers could not be written

  double *py, *px;
  int counting, invisibleCount=0;
  int startWriting, lastCount = 1;

  // any graph with data ?
  while(g->cPointsX.isEmpty()) {
    if (!ig.hasNext()) break; // no more graphs
    g = ig.next(); // point to next graph
  }

  if(!g->cPointsX.isEmpty()) { // did we find a graph with data ?
    // ................................................
    counting = g->cPointsX.getFirst()->count * g->countY;  // number of values
    NumAll = counting;
    
    invisibleCount = counting - y/tHeight;
    if(invisibleCount <= 0)  xAxis.limit_min = 0.0;// height bigger than needed
    else {
      NumLeft = invisibleCount - int(xAxis.limit_min + 0.5);
      if(invisibleCount < int(xAxis.limit_min + 0.5))
	xAxis.limit_min = double(invisibleCount); // adjust limit of scroll bar
    }
    
    for(DataX *pD = g->cPointsX.last(); pD!=0; pD = g->cPointsX.prev()) {
      colWidth = 0;
      Str = pD->Var;
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
      if(colWidth < 0)  goto funcEnd;
      startWriting = int(xAxis.limit_min + 0.5);  // when to reach visible area
      
      Texts.append(new Text(x-4, y2-2, Str)); // independent variable
      if(pD->count != 0) {
	y = y2-tHeight-5;
	counting /= pD->count;   // how many rows to be skipped
	for(int z1=0; z1<lastCount; z1++) {
	  px = pD->Points;
	  for(int z=pD->count; z>0; z--) {
	    if(startWriting <= 0) { // reached visible area ?
	      y += tHeight*startWriting;
	      startWriting = 0;
	      if(y < tHeight) break;  // no room for more rows ?
	      Str = misc::StringNum(*px, 'g', g->Precision);
	      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
	      if(colWidth < 0)  goto funcEnd;
	      
	      Texts.append(new Text( x, y, Str));
	      y -= tHeight*counting;
	    }
	    else startWriting -= counting;
	    px++;
	  }
	  if(pD == g->cPointsX.getFirst())   // only paint one time
	    if(y >= tHeight) if(y < y2-tHeight-5)
	      Lines.append(new Line(0, y+1, x2, y+1, QPen(Qt::black,0)));
	}
	lastCount *= pD->count;
      }
      x += colWidth+15;
      Lines.append(new Line(x-8, y2, x-8, 0, QPen(Qt::black,0)));
    }
    Lines.last()->style = QPen(Qt::black,2);
  }  // of "if no data in graphs"

  
  firstGraph = g;
  // ................................................
  // all dependent variables
  foreach(Graph *g, Graphs) {
    y = y2-tHeight-5;
    colWidth = 0;

    Str = g->Var;
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth < 0)  goto funcEnd;
    Texts.append(new Text(x, y2-2, Str));  // dependent variable


    startWriting = int(xAxis.limit_min + 0.5); // when to reach visible area
    py = g->cPointsY - 2;
    if(g->cPointsX.getFirst()) {

      if (!g->cPointsY) {   // no data points
	Str = QObject::tr("invalid");
	colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
	if(colWidth < 0)  goto funcEnd;
	Texts.append(new Text(x, y, Str));
      }
      else if(sameDependencies(g, firstGraph)) {
        int z=g->cPointsX.getFirst()->count * g->countY;
        if(z > NumAll)  NumAll = z;

        if(g->Var.right(2) != ".X")
          for(; z>0; z--) {
            py += 2;
            if(startWriting-- > 0) continue; // reached visible area ?
            if(y < tHeight) break;           // no room for more rows ?
            switch(g->numMode) {
              case 0: Str = misc::complexRect(*py, *(py+1), g->Precision); break;
              case 1: Str = misc::complexDeg (*py, *(py+1), g->Precision); break;
              case 2: Str = misc::complexRad (*py, *(py+1), g->Precision); break;
            }

            colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
            if(colWidth < 0)  goto funcEnd;

            Texts.append(new Text(x, y, Str));
            y -= tHeight;
          }

        else {  // digital data
          char *pcy = (char*)g->cPointsY;
          for(; z>0; z--) {
            if(startWriting-- > 0) {  // reached visible area ?
              pcy += strlen(pcy) + 1;
              continue;
            }
            if(y < tHeight) break;           // no room for more rows ?
            Str = QString(pcy);

            colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
            if(colWidth < 0)  goto funcEnd;

            Texts.append(new Text(x, y, Str));
            pcy += strlen(pcy) + 1;
            y -= tHeight;
          }
        }

        if(z > NumLeft)  NumLeft = z;
      }  // of "if(sameDeps)"
      else {
        Str = QObject::tr("wrong dependency");
        colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
        if(colWidth < 0)  goto funcEnd;
        Texts.append(new Text(x, y, Str));
      }
    }
    else {   // no data in graph
      Str = QObject::tr("no data");
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
      if(colWidth < 0)  goto funcEnd;
      Texts.append(new Text(x, y, Str));
    }
    x += colWidth+15;
    if(g != Graphs.last())   // do not paint last line
      Lines.append(new Line(x-8, y2, x-8, 0, QPen(Qt::black,0)));
  }

funcEnd:
  if(invisibleCount > 0) {  // could all numbers be written ?
    x1 = 18;   // extend the select area to the left

    zAxis.limit_max = double(NumAll);  // number of data (rows) 

    // calculate data for painting scroll bar
    y = int(xAxis.limit_min + 0.5);
    NumLeft = NumAll - NumLeft - y;

    // position of scroll bar in pixel
    yAxis.numGraphs = (y2 - 39) * y / NumAll;

    // height of scroll bar
    zAxis.numGraphs = (y2 - 39) * NumLeft / NumAll;
    if(zAxis.numGraphs < MIN_SCROLLBAR_SIZE) {
      yAxis.numGraphs -= (MIN_SCROLLBAR_SIZE - zAxis.numGraphs + 1)
                         * y / NumAll;
      zAxis.numGraphs = MIN_SCROLLBAR_SIZE;
    }

    xAxis.numGraphs = NumLeft;  // number of lines in the diagram
  }

  return 1;
}

// ------------------------------------------------------------
int TabDiagram::scroll(int clickPos)
{
  if(x1 <= 0) return 0;   // no scroll bar ?
  int tmp = int(xAxis.limit_min + 0.5);

  int y = cy;
  if(clickPos > (cy-20)) {  // scroll one line down ?
    xAxis.limit_min++;
  }
  else {
    y -= y2 - 20;
    if(clickPos < y) {  // scroll bar one line up ?
      if(xAxis.limit_min <= 0.0)  return 0;
      xAxis.limit_min--;
    }
    else {
      y += yAxis.numGraphs;
      if(clickPos < y)   // scroll bar one page up ?
        xAxis.limit_min -= double(xAxis.numGraphs);
      else {
        y += zAxis.numGraphs;
        if(clickPos > y)   // a page down?
          xAxis.limit_min += double(xAxis.numGraphs);
        else
          return 2;  // click on position bar
      }
    }
  }

  calcDiagram();
  if(tmp == int(xAxis.limit_min + 0.5))
    return 0;   // did anything change ?

  return 1;
}

// ------------------------------------------------------------
bool TabDiagram::scrollTo(int initial, int, int dy)
{
  int tmp = int(xAxis.limit_min + 0.5);
  xAxis.limit_min  = double(initial);
  xAxis.limit_min += double(dy) / double(y2 - 39) * zAxis.limit_max;
  xAxis.limit_min  = floor(xAxis.limit_min + 0.5);

  calcDiagram();
  if(tmp == int(xAxis.limit_min + 0.5))
    return false;   // did anything change ?

  return true;
}

// ------------------------------------------------------------
Diagram* TabDiagram::newOne()
{
  return new TabDiagram();
}

// ------------------------------------------------------------
Element* TabDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Tabular");
  BitmapFile = (char *) "tabular";

  if(getNewOne)  return new TabDiagram();
  return 0;
}
