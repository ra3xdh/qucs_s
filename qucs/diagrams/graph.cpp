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
#include "misc.h"

#include <cstdlib>
#include <iostream>
#include <cmath>

#include <QPainter>
#include <QDebug>
#include <QPainterPath>

class Diagram;

Graph::Graph(Diagram const* d, const QString& _Line) :
  Element(),
  Style(GRAPHSTYLE_SOLID),
  diagram(d)
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
}

Graph::~Graph()
{
  if(cPointsY != 0)
    delete[] cPointsY;
}

// ---------------------------------------------------------------------
void Graph::createMarkerText() const
{
  for(auto pm : Markers) {
    pm->createText();
  }
}

void Graph::paint(QPainter* painter) {
  if (ScrPoints.empty()) {
    return;
  }
  painter->save();

  if (isSelected) {
    painter->setPen(QPen(Qt::darkGray,Thick + 4));
    paintLines(painter);

    painter->setPen(QPen(Qt::white, Thick, Qt::SolidLine));
    paintLines(painter);
    painter->restore();
    return;
  }

  // **** not selected ****
  painter->setPen(QPen(QColor(Color), Thick, Qt::SolidLine));
  paintLines(painter);
  painter->restore();
}

void Graph::paintLines(QPainter* painter) {
  switch(Style) {
    case GRAPHSTYLE_STAR:
      drawStarSymbols(painter);
      break;
    case GRAPHSTYLE_CIRCLE:
      drawCircleSymbols(painter);
      break;
    case GRAPHSTYLE_ARROW:
      drawArrowSymbols(painter);
      break;
    default:
      drawLines(painter);
  }
}

// ---------------------------------------------------------------------
QString Graph::save()
{
  QString s = "\t<\""+Var+"\" "+Color.name()+
	      " "+QString::number(Thick)+" "+QString::number(Precision)+
	      " "+QString::number(numMode)+" "+QString::number(Style)+
	      " "+QString::number(yAxisNo)+">";

  for (Marker *pm : Markers)
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
  Color = misc::ColorFromString(n);
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
/*!
 * Checks if the coordinates x/y point to the graph. returns the number of the
 * branch of the graph, -1 upon a miss.
 *
 * x/y are relative to diagram cx/cy. 5 is the precision the user must point
 * onto the graph.
 *
 * FIXME: should return reference to hit sample point or some context.
 */
int Graph::getSelected(int x, int y)
{
  auto pp = ScrPoints.begin();
  if(pp == ScrPoints.end()) return -1;

  int A, z=0;
  int dx, dx2, x1;
  int dy, dy2, y1;

  int countX = cPointsX.at(0)->count;
  if(pp->isStrokeEnd()) {
    if(pp->isBranchEnd()) z++;
    pp++;
    if(pp->isBranchEnd()) {
      if(pp->isGraphEnd())  return -1;   // not even one point ?
      z++;
      pp++;
      if(pp->isGraphEnd())  return -1;   // not even one point ?
    }
  }

  if(Style >= GRAPHSTYLE_STAR) {
    // for graph symbols
    while(!pp->isGraphEnd()) {
      if(!pp->isStrokeEnd()) {
        dx  = x - int((pp)->getScrX());
        dy  = y - int((pp++)->getScrY());

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
  while(!pp->isGraphEnd()) {
    while(!pp->isBranchEnd()) {
      x1 = int(pp->getScrX());
      y1 = int((pp++)->getScrY());
      dx  = x - x1;
      dy  = y - y1;

      if(pp->isPt()){
        dx2 = int(pp->getScrX());
      }else if(pp->isBranchEnd()) {
        break;
      }else if(pp->isStrokeEnd()) {
        pp++;
        dx2 = int(pp->getScrX());  // go on as graph can also be selected between strokes
        if(pp->isBranchEnd()) break;
      }
      if(dx < -5) { if(x < dx2-5) continue; } // point between x coordinates ?
      else { if(x > 5) if(x > dx2+5) continue; }

      dy2 = int(pp->getScrY());
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
  Graph *pg = new Graph(diagram, Var);

  pg->Color = Color;
  pg->Thick = Thick;
  pg->Style = Style;

  pg->Precision = Precision;
  pg->numMode   = numMode;
  pg->yAxisNo   = yAxisNo;

  for (Marker *pm : Markers)
    pg->Markers.append(pm->sameNewOne(pg));

  return pg;
}

/*!
 * find a sample point close to VarPos, snap to it, and return data at VarPos
 */
std::pair<double,double> Graph::findSample(std::vector<double>& VarPos) const
{
  DataX const* pD;
  unsigned nVarPos=0;
  unsigned n=0;
  unsigned m=1;

  for(unsigned ii=0; (pD=axis(ii)); ++ii) {
    double* pp = pD->Points;
    double v = VarPos[nVarPos];
    for(unsigned i=pD->count; i>1; i--) {  // find appropriate marker position
      if(fabs(v-(*pp)) < fabs(v-(*(pp+1)))) break;
      pp++;
      n += m;
    }

    m *= pD->count;
    VarPos[nVarPos++] = *pp;
  }

  return std::pair<double,double>(cPointsY[2*n], cPointsY[2*n+1]);
}

// -----------------------------------------------------------------------
// meaning of the values in a graph "Points" list
//#define STROKEEND   -2
//#define BRANCHEND   -10
//#define GRAPHEND    -100
// -----------------------------------------------------------------------
// screen points pseudo iterator implementation.
void Graph::ScrPt::setStrokeEnd()
{
  type = STROKEEND;
}
void Graph::ScrPt::setBranchEnd()
{
  type = BRANCHEND;
}
void Graph::ScrPt::setGraphEnd()
{
  type = GRAPHEND;
}
bool Graph::ScrPt::isPt() const{return (ScrX >= 0. && type == DataPt);}
bool Graph::ScrPt::isStrokeEnd() const{return (type >= STROKEEND);}
bool Graph::ScrPt::isBranchEnd() const{return (type >= BRANCHEND);}
bool Graph::ScrPt::isGraphEnd() const{return (type >= GRAPHEND);}

/*!
 * set screen coordinate for graph sampling point
 * these must be nonnegative, but sometimes aren't,
 * eg. between calcCoordinate and clip.
 * (negative values are reserved for control.)
 */
void Graph::ScrPt::setScrX(float x) {
    if (x < 0) {
        qDebug() << "dangerous: negative screen coordinate" << x;
    } else {
      type = DataPt;
    }
    if (ScrX < 0) {
        qDebug() << "dangerous: (maybe) overwriting control token" << x;
    }
    ScrX = x;
}

void Graph::ScrPt::setScrY(float y) {
    if (y < 0) { // need to investigate...
        qDebug() << "setting negative screen coordinate" << y << "at" << ScrY;
    } else {
      type = DataPt;
    }
    ScrY = y;
}

void Graph::ScrPt::setScr(float x, float y) {
    x = fmaxf(x, 0); // avoid negative coordinates
    y = fmaxf(y, 0);
    setScrX(x);
    setScrY(y);
}

void Graph::ScrPt::setIndep(double x)
{
  assert(ScrX>=0);
  indep = x;
}
void Graph::ScrPt::setDep(double x)
{
  assert(ScrX>=0);
  dep = x;
}
float Graph::ScrPt::getScrX() const
{
  if(ScrX<0){
    std::cerr << "dangerous: returning negative screen coordinate" << ScrX;
  }
  return ScrX;
}
float Graph::ScrPt::getScrY() const
{
  return ScrY;
}
double Graph::ScrPt::getIndep() const
{
  assert(ScrX>=0);
  return indep;
}
double Graph::ScrPt::getDep() const
{
  assert(ScrX>=0);
  return dep;
}

void Graph::drawCircleSymbols(QPainter* painter) const {
  constexpr double radius = 4.0;

  for (auto point : *this) {
    if (!point.isPt()) {
      continue;
    }
    painter->drawEllipse({point.getScrX(), point.getScrY()}, radius, radius);
  }
}

void Graph::drawArrowSymbols(QPainter* painter) const {
  // Arrow head size constants
  constexpr double head_height = 7.0;
  constexpr double head_half_width = 4.0;
  for (auto point : *this) {
    if (point.isGraphEnd()) {
      break;
    }

    if (!point.isPt()) {
      continue;
    }
    // Given a graph point we draw a vertical arrow pointed to it

    // Vertical arrow line (stem)
    painter->drawLine(QLineF{point.getScrX(), point.getScrY(), point.getScrX(), static_cast<qreal>(cy)});
    // left arrowhead part
    painter->drawLine(QLineF{point.getScrX() - head_half_width, point.getScrY() - head_height, point.getScrX(), point.getScrY()});
    // right arrowhead part
    painter->drawLine(QLineF{point.getScrX() + head_half_width, point.getScrY() - head_height, point.getScrX(), point.getScrY()});
  }
}

void Graph::drawStarSymbols(QPainter* painter) const {
  for (auto point : *this) {
    if (!point.isPt()) {
      continue;
    }
    painter->save();
    painter->translate(point.getScrX(), point.getScrY());
    painter->drawLine(-5, 0, 5, 0); // horizontal line
    painter->rotate(60);
    painter->drawLine(-5, 0, 5, 0); // upper left to lower right
    painter->rotate(-120);
    painter->drawLine(-5, 0, 5, 0); // upper right to lower left
    painter->restore();
  }
}

void Graph::drawLines(QPainter* painter) const {
  painter->save();

  QPen pen = painter->pen();
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setCapStyle(Qt::RoundCap);
  switch(Style) {
    case GRAPHSTYLE_DASH:
      pen.setDashPattern({10.0, 6.0});  // stroke len, space len
      break;
    case GRAPHSTYLE_DOT:
      pen.setDashPattern({2.0, 4.0});
      break;
    case GRAPHSTYLE_LONGDASH:
      pen.setDashPattern({24.0, 8.0});
      break;
    default:
      pen.setStyle(Qt::SolidLine);
  }
  painter->setPen(pen);

  // How graphs are drawn
  //
  // Graph object (this) contains a set of data points,
  // each described by the ScrPt struct.
  //
  // Sometimes semantically sigle graph consists of
  // multiple lines/curves — or "subgraphs": you draw
  // one subgraph, then return back to the beginning
  // and draw next, and so on.
  //
  // To describe such compound graphs the set of data
  // points contains points for each of the subgraphs
  // i.e. at first a subequence for one subgraph, then
  // a subsequence for next one and so on, all within
  // a single parent sequence:
  //    aaaaaaabbbbbbbbcccccccdddddd
  //
  // Naturally, there has to be a mean to delimit such
  // subsequences, so not all points in the data set are
  // actually "data" points — some of them are "service"
  // points describing boundaries between subsequences and
  // the end of parent sequence:
  //  - point.isStrokeEnd() returns true if point is the
  //    boundary between subsequences.
  //  - point.isGraphEnd() returns true when there is no
  //    more graph data points

  // In order to do a bit of optimization, we want a datapoint to be skipped
  // if it is too close to the previous one and graph segment between them
  // cannot be rendered distinctively.
  //
  // These are formulas from QTransform documentation (https://doc.qt.io/qt-6/qtransform.html)
  // explaining how QTransform transforms the coordinates:
  //    x' = m11*x + m21*y + dx
  //    y' = m22*y + m12*x + dy
  //
  // We don't care about the second (skew) and third parts (tranlsation). This leaves us:
  //    x' = m11*x
  //    y' = m22*y
  //
  // So, for example, a horizontal segment of length 10 will be of size 10*m11 after transformation.
  // But this works in the opposite direction too: 10/m11 gives us the length a horizontal segment
  // should have to be of length 10 *after* transformation.
  //
  // With this knowledge we can now calculate thresholds for our dataset.

  constexpr double min_pixels = 1.0;  // I can be wrong here, but I believe these are pixels…
  const double x_threshold = std::abs(min_pixels / painter->transform().m11());
  const double y_threshold = std::abs(min_pixels / painter->transform().m22());

  // Helper to quickly calculate whether two points are distant enough to be worth of drawing
  // a graph segment between them
  const auto is_too_short = [x_threshold, y_threshold](const QPointF& a, const QPointF& b) {
    return std::abs(a.x() - b.x()) < x_threshold && std::abs(a.y() - b.y()) < y_threshold;
  };

  bool drawing_started = false;
  QPointF segment_start;
  QPointF segment_end;
  QPainterPath path;

  for (const auto& point : *this) {
    // No more data points
    if (point.isGraphEnd()) {
      break;
    }

    // Subgraph has ended, let's pretend like we're
    // drawing a graph from the beginning
    if (point.isStrokeEnd()) {
      painter->drawPath(path);
      path.clear();
      drawing_started = false;
      continue;
    }

    // skip if not valid
    if (!point.isPt()) {
      continue;
    }

    // First point in a subgraph. From here the drawing starts
    if (!drawing_started) {
      segment_start.setX(point.getScrX());
      segment_start.setY(point.getScrY());
      path.moveTo(segment_start);
      drawing_started = true;
      continue;
    }

    segment_end.setX(point.getScrX());
    segment_end.setY(point.getScrY());

    if (is_too_short(segment_start, segment_end)) {
      continue;
    }

    path.lineTo(segment_end);
    segment_start = segment_end;
  }

  painter->restore();
}

// vim:ts=8:sw=2:et
