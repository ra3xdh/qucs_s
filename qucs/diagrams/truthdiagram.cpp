/***************************************************************************
                              truthdiagram.cpp
                             ------------------
    begin                : Sat Nov 12 2005
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
#include <QtGui>
#include "truthdiagram.h"
#include "main.h"

#include <math.h>
#include "qucs.h"
#include "schematic.h"


TruthDiagram::TruthDiagram(int _cx, int _cy) : TabDiagram(_cx, _cy)
{
  x1 = 0;    // no extension to select area
  y1 = 0;
  x2 = x3 = 150;  // initial size of diagram
  y2 = 200;
  Name = "Truth";
  xAxis.limit_min = 0.0;  // scroll bar position (needs to be saved in file)

  calcDiagram();
}

TruthDiagram::~TruthDiagram()
{
}

// ------------------------------------------------------------
// calculates the text in the tabular
int TruthDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x1 = 0;  // no scroll bar
  x3 = x2;
  QFontMetrics  metrics(((Schematic*)QucsMain->DocumentTab->currentPage())->font());   // get size of text
  int tHeight = metrics.lineSpacing();
  QString Str;
  int colWidth=0, x=6, y;

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
  Graph *g = Graphs.first();
  if(g == 0) {  // no variables specified in diagram ?
    Str = QObject::tr("no variables");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth >= 0)
      Texts.append(new Text(x-4, y2-2, Str)); // independent variable
    return 0;
  }


  int NumAll=0;   // how many numbers per column
  int NumLeft=0;  // how many numbers could not be written

  char *py;
  int counting, invisibleCount=0;
  int startWriting, z;

  while(g->cPointsX.isEmpty()) {  // any graph with data ?
    g = Graphs.next();
    if(g == 0) break;
  }
if(g) if(!g->cPointsX.isEmpty()) {
  // ................................................
  NumAll = g->cPointsX.getFirst()->count * g->countY;  // number of values

  invisibleCount = NumAll - y/tHeight;
  if(invisibleCount <= 0)  xAxis.limit_min = 0.0;// height bigger than needed
  else {
    if(invisibleCount < int(xAxis.limit_min + 0.5))
      xAxis.limit_min = double(invisibleCount); // adjust limit of scroll bar
    NumLeft = invisibleCount - int(xAxis.limit_min + 0.5);
  }


  colWidth = 0;
  Texts.append(new Text(x-4, y2-2, Str)); // independent variable
  if(NumAll != 0) {
    z = metrics.width("1");
    colWidth = metrics.width("0");
    if(z > colWidth)  colWidth = z;
    colWidth += 2;
    counting = int(log(double(NumAll)) / log(2.0) + 0.9999); // number of bits

    if((x+colWidth*counting) >= x2) {    // enough space for text ?
      checkColumnWidth("0", metrics, 0, x2, y);
      goto funcEnd;
    }

    y = y2-tHeight-5;
    startWriting = x;
    for(z=int(xAxis.limit_min + 0.5); z<NumAll; z++) {
      if(y < tHeight) break;  // no room for more rows ?
      startWriting = x;
      for(int zi=counting-1; zi>=0; zi--) {
        if(z & (1 << zi))  Str = "1";
        else  Str = "0";
        Texts.append(new Text( startWriting, y, Str));
        startWriting += colWidth;
      }
      y -= tHeight;
    }
    x = startWriting + 15;
  }
  Lines.append(new Line(x-8, y2, x-8, 0, QPen(Qt::black,2)));

}  // of "if no data in graphs"


  int zi, digitWidth;
  firstGraph = g;
  // ................................................
  // all dependent variables
  for(g = Graphs.first(); g!=0; g = Graphs.next()) {
    y = y2-tHeight-5;

    Str = g->Var;
    colWidth = checkColumnWidth(Str, metrics, 0, x, y2);
    if(colWidth < 0)  goto funcEnd;
    Texts.append(new Text(x, y2-2, Str));  // dependent variable


    startWriting = int(xAxis.limit_min + 0.5);  // when to reach visible area
    if(g->cPointsX.getFirst()) {

      if(sameDependencies(g, firstGraph)) {

        if(g->Var.right(2) != ".X") {  // not a digital variable ?
          double *pdy = g->cPointsY - 2;
          for(z = NumAll; z>0; z--) {
            pdy += 2;
            if(startWriting-- > 0) continue; // reached visible area ?
            if(y < tHeight) break;           // no room for more rows ?
            Str = QString::number(sqrt((*pdy)*(*pdy) + (*(pdy+1))*(*(pdy+1))));

            colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
            if(colWidth < 0)  goto funcEnd;

            Texts.append(new Text(x, y, Str));
            y -= tHeight;
          }
        }

        else {  // digital variable !!!
          py = (char*)g->cPointsY;
          counting = strlen((char*)py);    // count number of "bits"

          digitWidth = metrics.width("X") + 2;
          if((x+digitWidth*counting) >= x2) {    // enough space for "bit vector" ?
            checkColumnWidth("0", metrics, 0, x2, y);
            goto funcEnd;
          }

          for(z = NumAll; z>0; z--) {
            if(startWriting-- > 0) {   // reached visible area ?
              py += counting + 1;
              continue;
            }
            if(y < tHeight) break;    // no room for more rows ?

            zi = 0;
            while(*py) {
              Str = *(py++);
              Texts.append(new Text(x + zi, y, Str));
              zi += digitWidth;
            }
            py++;
            y -= tHeight;
          }

          digitWidth *= counting;
          if(colWidth < digitWidth)
            colWidth = digitWidth;
        }

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
    if(g != Graphs.getLast())   // do not paint last line
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
Diagram* TruthDiagram::newOne()
{
  return new TruthDiagram();
}

// ------------------------------------------------------------
Element* TruthDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Truth Table");
  BitmapFile = (char *) "truth";

  if(getNewOne)  return new TruthDiagram();
  return 0;
}
