/***************************************************************************
                             timingdiagram.cpp
                            -------------------
    begin                : Sat Oct 22 2005
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
  \class TimingDiagram
  \brief The TimingDiagram class implements the Timing diagram
*/

#include "timingdiagram.h"
#include "main.h"
#include "misc.h"

#include <math.h>
#include <QPolygon>
#include <QPainter>


TimingDiagram::TimingDiagram(int _cx, int _cy) : TabDiagram(_cx, _cy)
{
  x1 = 0;    // no extension to select area
  y1 = 0;
  x2 = x3 = 300;  // initial size of diagram
  y2 = 200;
  Name = "Time";
  xAxis.limit_min = 0.0;  // scroll bar position (needs to be saved in file)

  calcDiagram();
}

TimingDiagram::~TimingDiagram()
{
}

// ------------------------------------------------------------
void TimingDiagram::paint(ViewPainter *p)
{
  // paint all lines
  foreach(Line *pl, Lines) {
    p->Painter->setPen(pl->style);
    p->drawLine(cx+pl->x1, cy-pl->y1, cx+pl->x2, cy-pl->y2);
  }

  p->Painter->setPen(Qt::black);
  // write whole text
  foreach(Text *pt, Texts)
    p->drawText(pt->s, cx+pt->x, cy-pt->y);


  if(y1 > 0) {  // paint scroll bar ?
    int   x, y, dx, dy;
    QPolygon Points;
    // draw scroll bar
    p->fillRect(cx+yAxis.numGraphs, cy+2, zAxis.numGraphs, 14, QColor(192, 192, 192));

    int bx = cx+yAxis.numGraphs+zAxis.numGraphs;
    // draw frame for scroll bar
    p->Painter->setPen(QPen(Qt::black,0));
    p->drawLine(cx+xAxis.numGraphs, cy, cx+xAxis.numGraphs, cy+17);
    p->drawLine(cx+xAxis.numGraphs+17, cy, cx+xAxis.numGraphs+17, cy+17);
    p->drawLine(cx+xAxis.numGraphs, cy+17, cx+x2, cy+17);
    p->drawLine(cx+x2, cy, cx+x2, cy+17);
    p->drawLine(cx+x2-17, cy, cx+x2-17, cy+17);

    // draw the arrows above and below the scroll bar
    p->Painter->setBrush(QColor(192, 192, 192));
    p->Painter->setPen(QColor(152, 152, 152));
    p->drawLine(cx+yAxis.numGraphs, cy+15, bx, cy+15);
    p->drawLine(bx, cy+2, bx, cy+15);

    p->map(cx+xAxis.numGraphs+3,  cy+3, x, y);
    p->map(cx+xAxis.numGraphs+14, cy+14, dx, dy);
    Points.setPoints(3, x, (y+dy)>>1, dx, y, dx, dy);
    p->Painter->drawConvexPolygon(Points);
    p->Painter->setPen(QColor(224, 224, 224));
    p->Painter->drawLine(x, (y+dy)>>1, dx, y);
    p->drawLine(cx+yAxis.numGraphs, cy+2, bx, cy+2);
    p->drawLine(cx+yAxis.numGraphs, cy+2, cx+yAxis.numGraphs, cy+15);

    p->Painter->setPen(QColor(152, 152, 152));
    dx -= x;
    p->map(cx+x2-3,  cy+3, x, y);
    Points.setPoints(3, x, (y+dy)>>1, x-dx, y, x-dx, dy);
    p->Painter->drawConvexPolygon(Points);
    p->Painter->setPen(QColor(208, 208, 208));
    p->Painter->drawLine(x-dx, y, x, (y+dy)>>1);
    p->Painter->setPen(QColor(224, 224, 224));
    p->Painter->drawLine(x-dx, y, x-dx, dy);

    p->Painter->setBrush(QBrush(Qt::NoBrush));
  }


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
int TimingDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();

  y1 = 0;  // no scroll bar
  x3 = x2;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  int tHeight = metrics.lineSpacing();
  QString Str;
  int colWidth=0, x=4, y, xStart = 0, z;

  int NumAll=0;   // how many values per row
  int NumLeft=0;  // how many values could not be written
  int invisibleCount = 0;  // how many values are invisible
  
  if(y2 < (tHeight + 8))
    y2 = tHeight + 8;
  y = y2 - tHeight - 6;

  // outer frame
  Lines.append(new Line(0, y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new Line(0, y2, 0, 0, QPen(Qt::black,0)));
  Lines.append(new Line(x2, y2, x2, 0, QPen(Qt::black,0)));
  Lines.append(new Line(0, 0, x2, 0, QPen(Qt::black,0)));
  Lines.append(new Line(0, y+2, x2, y+2, QPen(Qt::black,0)));

  if(xAxis.limit_min < 0.0)
    xAxis.limit_min = 0.0;

  Graph *firstGraph;

  QListIterator<Graph *> ig(Graphs);
  Graph *g = 0;
  if (ig.hasNext())
     g= ig.next();

  if(g == 0) {  // no variables specified in diagram ?
    Str = QObject::tr("no variables");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth >= 0)
      Texts.append(new Text(x, y2-2, Str)); // independent variable
    return 0;
  }


  double *px;
  while(g->cPointsX.isEmpty()) {  // any graph with data ?
    g = ig.next();
    if( !ig.hasNext()) break;
  }
  if(g == 0) {
    Str = QObject::tr("no data");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth < 0)  return 0;
    Texts.append(new Text(x, y2-2, Str));
    return 0;
  }
  firstGraph = g;


  // First check the maximum bit number of all vectors.
  colWidth = 0;
  foreach(Graph *g, Graphs)
    if(g->cPointsY) {
      if(g->Var.right(2) == ".X") {
        z = strlen((char*)g->cPointsY);
        if(z > colWidth)
          colWidth = z;
      }
      else {
        z = 8;
        if(z > colWidth)
          colWidth = z;
      }
    }
  int TimeStepWidth = colWidth * metrics.width("X") + 8;
  if(TimeStepWidth < 40)
    TimeStepWidth = 40;


  colWidth = 0;
if(!firstGraph->cPointsX.isEmpty()) {
  // ................................................
  if(firstGraph->cPointsX.count() > 1) {
    Str = QObject::tr("wrong dependency");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth >= 0)
      Texts.append(new Text(x, y2-2, Str)); // independent variable
    return 0;
  }


  // first, write name of independent variable
  DataX *pD = firstGraph->cPointsX.getFirst();
  NumAll = pD->count;
  Str = pD->Var;
  colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
  if(colWidth < 0)  return 1;
  Texts.append(new Text(x, y2-2, Str));
  

  y -= 5;
  // write all dependent variable names to get width of first column
  foreach(Graph *g, Graphs) {
    if(y < tHeight)  break;
    Str = g->Var;
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
    if(colWidth < 0)  return 1;
    Texts.append(new Text(x, y, Str));  // dependent variable
    y -= tHeight + 2;
  }
  x += colWidth + 13;
  xAxis.numGraphs = x -6;
  Lines.append(new Line(x-6, y2, x-6, 0, QPen(Qt::black,0)));
  xStart = x;


  invisibleCount = NumAll - (x2-xAxis.numGraphs)/TimeStepWidth;
  if(invisibleCount <= 0)  xAxis.limit_min = 0.0;  // longer than needed
  else {
    NumLeft = invisibleCount - int(xAxis.limit_min + 0.5);
    if(invisibleCount < int(xAxis.limit_min + 0.5))
      xAxis.limit_min = double(invisibleCount); // adjust limit of scroll bar
  }


  // write independent variable values (usually time)
  y = y2-tHeight-4;
  px = pD->Points;
  z = int(xAxis.limit_min + 0.5);
  px += z;
  z = pD->count - z;
  for( ; z>0; z--) {
    Str = num2str(*(px++));
    colWidth = metrics.width(Str);  // width of text
    if(x+colWidth+2 >= x2)  break;

    Texts.append(new Text( x, y2-2, Str));
    Lines.append(new Line(x+5, y, x+5, y-3, QPen(Qt::black,0)));
    x += TimeStepWidth;
  }

}  // of "if no data in graphs"


  tHeight += 2;
  // ................................................
  // work on all dependent variables
  QPen Pen;
  int  yLast, yNow;
  y = y2-tHeight-9;
  foreach(Graph *g, Graphs) {
    if(y < tHeight) {
      // mark lack of space with a small arrow
      Lines.append(new Line(4, 6, 4, -7, QPen(Qt::red,2)));
      Lines.append(new Line(1, 0, 4, -7, QPen(Qt::red,2)));
      Lines.append(new Line(7, 0, 4, -7, QPen(Qt::red,2)));
      break;
    }

    x = xStart + 5;
    colWidth = 0;

    if(g->cPointsY == 0) {
      Str = QObject::tr("no data");
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
      if(colWidth < 0)  goto funcEnd;
      Texts.append(new Text(x, y, Str));
      y -= tHeight;
      continue;
    }

    if(!sameDependencies(g, firstGraph)) {
      Str = QObject::tr("wrong dependency");
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
      if(colWidth < 0)  goto funcEnd;
      Texts.append(new Text(x, y, Str));
      y -= tHeight;
      continue;
    }

    Pen = QPen(g->Color, g->Thick);  // default is solid line
    switch(g->Style) {
      case 1: Pen.setStyle(Qt::DashLine); break;
      case 2: Pen.setStyle(Qt::DotLine);  break;
    }

    z = int(xAxis.limit_min + 0.5);
    if(g->Var.right(2) != ".X") {  // not digital variable ?
      px = g->cPointsY;
      px += 2 * z;
      z = g->cPointsX.getFirst()->count - z;
      yNow = 1 + ((tHeight - 6) >> 1);
      Lines.append(new Line(x, y-yNow, x+2, y-1, Pen));
      Lines.append(new Line(x+2, y-tHeight+5, x, y-yNow, Pen));
      for( ; z>0; z--) {
        if(x+TimeStepWidth >= x2) break;
        Lines.append(new Line(x+2, y-1, x+TimeStepWidth-2, y-1, Pen));
        Lines.append(new Line(x+2, y-tHeight+5, x+TimeStepWidth-2, y-tHeight+5, Pen));

	if (*(px+1) == 0.0)
	  // output real number
	  Texts.append(new Text(x+3, y,QString::number(*px)));
	else
	  // output magnitude of (complex) number
	  Texts.append(new Text(x+3, y,
              QString::number(sqrt((*px)*(*px) + (*(px+1))*(*(px+1))))));

        px += 2;
        x += TimeStepWidth;
        Lines.append(new Line(x-2, y-tHeight+5, x+2, y-1, Pen));
        Lines.append(new Line(x+2, y-tHeight+5, x-2, y-1, Pen));
      }
      y -= tHeight;
      continue;
    }


    // digital variable !!!
    char *pcx = (char*)g->cPointsY;
    pcx += z * (strlen(pcx)+1);

    if(strlen(pcx) < 2) {   // vector or single bit ?

      // It is single "bit".
      yLast = 0;
      if(z > 0)  yLast += 2; // vertical line before first value ?
      switch(*(pcx-yLast)) {  // high or low ?
        case '0':  // low
          yLast = tHeight - 5;
          break;
        case '1':  // high
          yLast = 1;
          break;
        default:
          yLast = 1 + ((tHeight - 6) >> 1);
      }

      z = g->cPointsX.getFirst()->count - z;
      for( ; z>0; z--) {

        switch(*pcx) {
          case '0':  // low
            yNow = tHeight - 5;
            break;
          case '1':  // high
            yNow = 1;
            break;
          default:
            yNow = 1 + ((tHeight - 6) >> 1);
        } 

        if(yLast != yNow)
          Lines.append(new Line(x, y-yLast, x, y-yNow, Pen));
        if(x+TimeStepWidth >= x2) break;
        if((*pcx & 254) == '0')
          Lines.append(new Line(x, y-yNow, x+TimeStepWidth, y-yNow, Pen));
        else {
          Texts.append(new Text(x+(TimeStepWidth>>1)-3, y, QString(pcx)));
          Lines.append(new Line(x+3, y-1, x+TimeStepWidth-3, y-1, Pen));
          Lines.append(new Line(x+3, y-tHeight+5, x+TimeStepWidth-3, y-tHeight+5, Pen));
          Lines.append(new Line(x, y-yNow, x+3, y-1, Pen));
          Lines.append(new Line(x, y-yNow, x+3, y-tHeight+5, Pen));
          Lines.append(new Line(x+TimeStepWidth-3, y-1, x+TimeStepWidth, y-yNow, Pen));
          Lines.append(new Line(x+TimeStepWidth-3, y-tHeight+5, x+TimeStepWidth, y-yNow, Pen));
        }

        yLast = yNow;
        x += TimeStepWidth;
        pcx += 2;
      }

    }
    else {  // It is a bit vector !!!

      z = g->cPointsX.getFirst()->count - z;
      yNow = 1 + ((tHeight - 6) >> 1);
      Lines.append(new Line(x, y-yNow, x+2, y-1, Pen));
      Lines.append(new Line(x+2, y-tHeight+5, x, y-yNow, Pen));
      for( ; z>0; z--) {
        if(x+TimeStepWidth >= x2) break;
        Lines.append(new Line(x+2, y-1, x+TimeStepWidth-2, y-1, Pen));
        Lines.append(new Line(x+2, y-tHeight+5, x+TimeStepWidth-2, y-tHeight+5, Pen));

        Texts.append(new Text(x+3, y, QString(pcx)));

        x += TimeStepWidth;
        pcx += strlen(pcx) + 1;
        Lines.append(new Line(x-2, y-tHeight+5, x+2, y-1, Pen));
        Lines.append(new Line(x+2, y-tHeight+5, x-2, y-1, Pen));
      }
    }

    y -= tHeight;
  }  // of "for(Graphs...)"


funcEnd:
  if(invisibleCount > 0) {  // could all values be displayed ?
    x  = x2 - xAxis.numGraphs - 37;
    if(x < MIN_SCROLLBAR_SIZE+2) {  // not enough space for scrollbar ?
      Lines.append(new Line(x2, 0, x2, -17, QPen(Qt::red,0)));
      Lines.append(new Line(xAxis.numGraphs, -17, x2, -17, QPen(Qt::red,0)));
      Lines.append(new Line(xAxis.numGraphs, 0, xAxis.numGraphs, -17, QPen(Qt::red,0)));
      return 1;
    }

    y1 = 18;   // extend the select area to the bottom
    z  = int(xAxis.limit_min + 0.5);
    if(NumLeft < 0) NumLeft = 0;
    y  = NumAll - NumLeft - z;

    // number of data (times) 
    zAxis.limit_max = double(firstGraph->cPointsX.getFirst()->count);

    // position of scroll bar in pixel
    yAxis.numGraphs = x * z / NumAll + xAxis.numGraphs + 19;

    // length of scroll bar
    zAxis.numGraphs = x * y / NumAll;
    if(zAxis.numGraphs < MIN_SCROLLBAR_SIZE) {
      yAxis.numGraphs -= (MIN_SCROLLBAR_SIZE - zAxis.numGraphs + 1)
                         * z / NumAll;
      zAxis.numGraphs = MIN_SCROLLBAR_SIZE;

      x = x2 - 19 - yAxis.numGraphs - zAxis.numGraphs;
      if(x < 0)  yAxis.numGraphs += x;
    }

    xAxis.limit_max = double(y);
  }

  return 1;
}

// ------------------------------------------------------------
int TimingDiagram::scroll(int clickPos)
{
  if(y1 <= 0) return 0;   // no scroll bar ?
  int tmp = int(xAxis.limit_min + 0.5);

  int x = cx;
  if(clickPos > (cx+x2-20)) {  // scroll one value to the right ?
    xAxis.limit_min++;
  }
  else {
    x += xAxis.numGraphs + 20;
    if(clickPos < x) {  // scroll bar one value to the left ?
      if(xAxis.limit_min <= 0.0)  return 0;
      xAxis.limit_min--;
    }
    else {
      x = cx + yAxis.numGraphs;
      if(clickPos < x)   // scroll bar one page to the left ?
        xAxis.limit_min -= xAxis.limit_max;
      else {
        x += zAxis.numGraphs;
        if(clickPos > x)   // one page to the right ?
          xAxis.limit_min += xAxis.limit_max;
        else
          return 2;  // click on position bar
      }
    }
  }

  calcDiagram();
  if(tmp == int(xAxis.limit_min+0.5))
    return 0;   // did anything change ?

  return 1;
}

// ------------------------------------------------------------
bool TimingDiagram::scrollTo(int initial, int dx, int)
{
  int tmp = int(xAxis.limit_min + 0.5);
  xAxis.limit_min  = double(initial);
  xAxis.limit_min += double(dx) / double(x2-xAxis.numGraphs-39) * zAxis.limit_max;
  xAxis.limit_min  = floor(xAxis.limit_min + 0.5);

  calcDiagram();
  if(tmp == int(xAxis.limit_min + 0.5))
    return false;   // did anything change ?

  return true;
}

// ------------------------------------------------------------
Diagram* TimingDiagram::newOne()
{
  return new TimingDiagram();
}

// ------------------------------------------------------------
Element* TimingDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Timing Diagram");
  BitmapFile = (char *) "timing";

  if(getNewOne)  return new TimingDiagram();
  return 0;
}
