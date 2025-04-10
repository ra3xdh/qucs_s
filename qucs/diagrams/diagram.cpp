/***************************************************************************
                                diagram.cpp
                               -------------
    begin                : Thu Oct 2 2003
    copyright            : (C) 2003, 2004, 2005 by Michael Margraf
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
  \class Diagram
  \brief The Diagram class is a superclass for diagrams.

  \todo Should probably use QFontMetrics::boundingRect(QString).width instead
        of QFontMetrics::width(QString), since only the first gives the actual
	width (see Qt docs)
*/

#if HAVE_CONFIG_H

# include <config.h>

#endif

#include <cstdlib>
#include <cmath>
#include <cfloat>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include <clocale>

#include "diagram.h"
#include "main.h"
#include "mnemo.h"
#include "schematic.h"

#include "rect3ddiagram.h"
#include "misc.h"

#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDateTime>
#include <QPainter>
#include <QDebug>
#include <QtAlgorithms>

Diagram::Diagram(int _cx, int _cy) {
    cx = _cx;
    cy = _cy;

    // x1, x2, y1, y2 are the selectable boundings of the diagram, but these
    // are the real boundings. They are set in "createAxisLabels()".
    Bounding_x1 = Bounding_x2 = Bounding_y1 = Bounding_y2 = 0;

    xAxis.numGraphs = yAxis.numGraphs = zAxis.numGraphs = 0;
    xAxis.min = xAxis.low =
    yAxis.min = yAxis.low = zAxis.min = zAxis.low = 0.0;
    xAxis.max = xAxis.up =
    yAxis.max = yAxis.up = zAxis.max = zAxis.up = 1.0;
    xAxis.GridOn = yAxis.GridOn = true;
    zAxis.GridOn = false;
    xAxis.log = yAxis.log = zAxis.log = false;
    xAxis.Units = Axis::NoUnits;
    yAxis.Units = Axis::NoUnits;
    zAxis.Units = Axis::NoUnits;

    xAxis.limit_min = yAxis.limit_min = zAxis.limit_min = 0.0;
    xAxis.limit_max = yAxis.limit_max = zAxis.limit_max = 1.0;
    xAxis.step = yAxis.step = zAxis.step = 1.0;
    xAxis.autoScale = yAxis.autoScale = zAxis.autoScale = true;

    rotX = 315;  // for 3D diagram
    rotY = 0;
    rotZ = 225;
    hideLines = true;  // hide invisible lines

    engineeringNotation = true;

    Type = isDiagram;
    isSelected = false;
    GridPen = QPen(Qt::lightGray, 0);
}

Diagram::~Diagram() {
    qDeleteAll(Graphs);
    qDeleteAll(Arcs);
    qDeleteAll(Lines);
    qDeleteAll(Texts);
}

/*!
   Paint function for most diagrams (cartesian, smith, polar, ...)
*/
void Diagram::paint(QPainter *p) {
    paintDiagram(p);
    paintMarkers(p);
}

void Diagram::paintDiagram(QPainter *painter) {
    painter->save();

    painter->translate(cx, cy);
    painter->save();

    for (qucs::Line* line : Lines) {
        painter->setPen(line->penHint());
        painter->drawLine(QLineF{line->x1, - line->y1, line->x2, - line->y2});
    }

    for (qucs::Arc* arc : Arcs) {
        painter->setPen(arc->penHint());
        painter->drawArc(QRectF{arc->x, - arc->y, arc->w, arc->h}, arc->angle, arc->arclen);
    }

    painter->scale(1.0, -1.0); // make Y-axis grow upwards
    for (Graph *pg: Graphs) {
        pg->paint(painter);
    }
    painter->restore();  // to translated(cx, cy) with no negative y-scale

    for (Text *pt: Texts) {
        painter->save();

        painter->setPen(pt->Color);
        painter->translate(pt->x, -pt->y);
        painter->rotate(pt->angle());
        painter->drawText(0, 0, pt->s);

        painter->restore();
    }

    if (isSelected) {
        QRectF bounds(0, -y2, x2, y2);
        painter->setPen(QPen(Qt::darkGray, 3));
        painter->drawRect(bounds.marginsAdded(QMargins{5, 5, 5, 5}));

        misc::draw_resize_handle(painter, bounds.topLeft());
        misc::draw_resize_handle(painter, bounds.bottomLeft());
        misc::draw_resize_handle(painter, bounds.bottomRight());
        misc::draw_resize_handle(painter, bounds.topRight());
    }
    painter->restore();
}

void Diagram::paintMarkers(QPainter *p, bool paintAll) {
    // draw markers last, so they are at the top of painting layers
    for (Graph *pg: Graphs)
        for (Marker *pm: pg->Markers)
            if (paintAll || (pm->Type & 1)) {
                pm->paint(p);
            }
}

// ------------------------------------------------------------
void Diagram::paintScheme(Schematic *p) {
    p->PostPaintEvent(_Rect, cx, cy - y2, x2, y2);
}

/*!
   Put axis labels into the text list.
*/
void Diagram::createAxisLabels() {
    int x, y, w, wmax = 0;
    QString Str;
    // get size of text using the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, nullptr);
    int LineSpacing = metrics.lineSpacing();


    x = (x2 >> 1);
    y = -y1;
    if (xAxis.Label.isEmpty()) {
        // write all x labels ----------------------------------------
        for (Graph *pg: Graphs) {
            DataX const *pD = pg->axis(0);
            if (!pD) continue;
            y -= LineSpacing;
            if (Name[0] != 'C') {   // locus curve ?
                w = metrics.boundingRect(pD->Var).width() >> 1;
                if (w > wmax) wmax = w;
                Texts.append(new Text(x - w, y, pD->Var, pg->Color, 12.0));
            } else {
                w = metrics.boundingRect("real(" + pg->Var + ")").width() >> 1;
                if (w > wmax) wmax = w;
                Texts.append(new Text(x - w, y, "real(" + pg->Var + ")",
                                      pg->Color, 12.0));
            }
        }
    } else {
        y -= LineSpacing;
        encode_String(xAxis.Label, Str);
        w = metrics.boundingRect(Str).width() >> 1;
        if (w > wmax) wmax = w;
        Texts.append(new Text(x - w, y, Str, Qt::black, 12.0));
    }
    Bounding_y2 = 0;
    Bounding_y1 = y - LineSpacing;
    Bounding_x2 = wmax - (x2 >> 1);
    if (Bounding_x2 < 0) Bounding_x2 = 0;
    Bounding_x1 = Bounding_x2;


    wmax = 0;
    x = -x1;
    y = y2 >> 1;

    QStringList used_kernels, used_simulations;
    for (const auto pg: Graphs) {
      if (!pg->Var.contains("/")) continue; // Qucsator data
        QString kernel_name = pg->Var.section('/', 0, 0);
        QString var_name = pg->Var;
        auto p = var_name.indexOf('/');
        var_name = var_name.mid(p + 1);
        QString sim_name = var_name.section('.', 0, 0);
        if (!used_simulations.contains(sim_name))
            used_simulations.append(sim_name);
        if (!used_kernels.contains(kernel_name))
            used_kernels.append(kernel_name);
    }

    if (yAxis.Label.isEmpty()) {
        // draw left y-label for all graphs ------------------------------

        // strip simulator name and simulation name like ngspice/ac.
        // if all graphs are from the same simulator and simulation

        for (Graph *pg: Graphs) {
            if (pg->yAxisNo != 0) continue;
            if (pg->cPointsY) {
                QString var_name = pg->Var;
                if (!QucsSettings.fullTraceName) {
                    if (used_kernels.count() == 1) {
                        auto p = var_name.indexOf('/');
                        var_name = var_name.mid(p + 1);
                    }
                    if (used_simulations.count() == 1) {
                        auto p = var_name.indexOf('.');
                        var_name = var_name.mid(p + 1);
                    }
                }
                if (Name[0] != 'C') {   // location curve ?
                    w = metrics.boundingRect(var_name).width() >> 1;
                    if (w > wmax) wmax = w;
                    Texts.append(new Text(x, y - w, var_name, pg->Color, 12.0, 0.0, 1.0));
                } else {
                    w = metrics.boundingRect("imag(" + var_name + ")").width() >> 1;
                    if (w > wmax) wmax = w;
                    Texts.append(new Text(x, y - w, "imag(" + var_name + ")",
                                          pg->Color, 12.0, 0.0, 1.0));
                }
            } else {     // if no data => <invalid>
                w = metrics.boundingRect(pg->Var + INVALID_STR).width() >> 1;
                if (w > wmax) wmax = w;
                Texts.append(new Text(x, y - w, pg->Var + INVALID_STR,
                                      pg->Color, 12.0, 0.0, 1.0));
            }
            x -= LineSpacing;
        }
    } else {
        encode_String(yAxis.Label, Str);
        w = metrics.boundingRect(Str).width() >> 1;
        if (w > wmax) wmax = w;
        Texts.append(new Text(x, y - w, Str, Qt::black, 12.0, 0.0, 1.0));
        x -= LineSpacing;
    }
    if (Bounding_x1 < -x) Bounding_x1 = -x;


    x = x3;
    y = y2 >> 1;
    if (zAxis.Label.isEmpty()) {
        // draw right y-label for all graphs ------------------------------
        for (Graph *pg: Graphs) {
            if (pg->yAxisNo != 1) continue;
            if (pg->cPointsY) {
                QString var_name = pg->Var;
                if (!QucsSettings.fullTraceName) {
                    if (used_kernels.count() == 1) {
                        auto p = var_name.indexOf('/');
                        var_name = var_name.mid(p + 1);
                    }
                    if (used_simulations.count() == 1) {
                        auto p = var_name.indexOf('.');
                        var_name = var_name.mid(p + 1);
                    }
                }
                if (Name[0] != 'C') {   // location curve ?
                    w = metrics.boundingRect(var_name).width() >> 1;
                    if (w > wmax) wmax = w;
                    Texts.append(new Text(x, y + w, var_name,
                                          pg->Color, 12.0, 0.0, -1.0));
                } else {
                    w = metrics.boundingRect("imag(" + var_name + ")").width() >> 1;
                    if (w > wmax) wmax = w;
                    Texts.append(new Text(x, y + w, "imag(" + var_name + ")",
                                          pg->Color, 12.0, 0.0, -1.0));
                }
            } else {     // if no data => <invalid>
                w = metrics.boundingRect(pg->Var + INVALID_STR).width() >> 1;
                if (w > wmax) wmax = w;
                Texts.append(new Text(x, y + w, pg->Var + INVALID_STR,
                                      pg->Color, 12.0, 0.0, -1.0));
            }
            x += LineSpacing;
        }
    } else {
        encode_String(zAxis.Label, Str);
        w = metrics.boundingRect(Str).width() >> 1;
        if (w > wmax) wmax = w;
        Texts.append(new Text(x, y + w, Str, Qt::black, 12.0, 0.0, -1.0));
    }
    x -= x2;
    if (Bounding_x2 < x) Bounding_x2 = x;

    wmax -= y2 >> 1;
    if (wmax > 0) {
        Bounding_y2 = wmax;
        wmax *= -1;
        if (wmax < Bounding_y1) Bounding_y1 = wmax;
    }
}

// ------------------------------------------------------------
int Diagram::regionCode(float x, float y) const {
    int code = 0;   // code for clipping
    if (x < 0.0)
        code |= 1;
    else if (x > float(x2))  // compare as float to avoid integer overflow
        code |= 2;

    if (y < 0.0)
        code |= 4;
    else if (y > float(y2))  // compare as float to avoid integer overflow
        code |= 8;

    return code;
}

// ------------------------------------------------------------
// Is virtual. This one is for round diagrams only.
bool Diagram::insideDiagram(float x, float y) const {
    float R = float(x2) / 2.0 + 1.0; // +1 seems better (graph sometimes little outside)
    x -= R;
    y -= R;
    return ((x * x + y * y) <= R * R);
}

/*!
   (try to) set a Marker to a diagram
*/
Marker *Diagram::setMarker(int x, int y) {
    if (getSelected(x, y)) {
        // test all graphs of the diagram
        for (Graph *pg: Graphs) {
            int n = pg->getSelected(x - cx, cy - y); // sic!
            if (n >= 0) {
                assert(pg->parentDiagram() == this);
                Marker *pm = new Marker(pg, n, x - cx, y - cy);
                pg->Markers.append(pm);
                return pm;
            }
        }
    }
    return nullptr;
}

/*!
   Cohen-Sutherland clipping algorithm
*/
void Diagram::rectClip(Graph::iterator &p) const {
    int code, z = 0;
    float x = 0, y = 0, dx, dy;
    float x_1 = (p - 2)->getScrX(), y_1 = (p - 2)->getScrY();
    float x_2 = (p - 1)->getScrX(), y_2 = (p - 1)->getScrY();

    int code1 = regionCode(x_1, y_1);
    int code2 = regionCode(x_2, y_2);
    if ((code1 | code2) == 0) return;  // line completely inside ?

    if (code1 != 0)
        if ((p - 3)->isPt()) {
            p++;
            (p - 3)->setStrokeEnd();
        }
    if (code1 & code2)   // line not visible at all ?
        goto endWithHidden;

    if (code2 != 0) {
        p->setStrokeEnd();
        (p + 1)->setScr(x_2, y_2);
        z += 2;
    }


    for (;;) {
        if ((code1 | code2) == 0) break;  // line completely inside ?

        if (code1) code = code1;
        else code = code2;

        dx = x_2 - x_1;  // dx and dy never equals zero !
        dy = y_2 - y_1;
        if (code & 1) {
            y = y_1 - dy * x_1 / dx;
            x = 0.0;
        } else if (code & 2) {
            y = y_1 + dy * (x2 - x_1) / dx;
            x = float(x2);
        } else if (code & 4) {
            x = x_1 - dx * y_1 / dy;
            y = 0.0;
        } else if (code & 8) {
            x = x_1 + dx * (y2 - y_1) / dy;
            y = float(y2);
        }

        if (code == code1) {
            x_1 = x;
            y_1 = y;
            code1 = regionCode(x, y);
        } else {
            x_2 = x;
            y_2 = y;
            code2 = regionCode(x, y);
        }
        if (code1 & code2)
            goto endWithHidden; // line not visible at all ?
    }

    (p - 2)->setScr(x_1, y_1);
    (p - 1)->setScr(x_2, y_2);
    p += z;
    return;

    endWithHidden:
    (p - 2)->setScr(x_2, y_2);
    p -= 1;
}

/*!
   Clipping for round diagrams (smith, polar, ...)
*/
void Diagram::clip(Graph::iterator &p) const {
    float R = float(x2) / 2.0;
    float x_1 = (p - 2)->getScrX() - R, y_1 = (p - 2)->getScrY() - R;
    float x_2 = (p - 1)->getScrX() - R, y_2 = (p - 1)->getScrY() - R;

    float dt1 = R * R;   // square of radius
    float dt2 = dt1 - x_2 * x_2 - y_2 * y_2;
    dt1 -= x_1 * x_1 + y_1 * y_1;

    if (dt1 >= 0.0) if (dt2 >= 0.0) return;  // line completely inside ?

    if (dt1 < 0.0)
        if ((p - 3)->isPt()) { // is there already a line end flag ?
            p++;
            (p - 3)->setStrokeEnd();
        }

    float x = x_1 - x_2;
    float y = y_1 - y_2;
    float C = x_1 * x + y_1 * y;
    float D = x * x + y * y;
    float F = C * C + dt1 * D;

    x_1 += R;
    y_1 += R;
    x_2 += R;
    y_2 += R;
    if (F <= 0.0) {   // line not visible at all ?
        (p - 2)->setScr(x_2, y_2);
        p -= 1;
        return;
    }

    int code = 0;
    R = std::sqrt(F);
    dt1 = C - R;
    if ((dt1 > 0.0) && (dt1 < D)) { // intersection outside start/end point ?
        (p - 2)->setScr(x_1 - x * dt1 / D, y_1 - y * dt1 / D);
        code |= 1;
    } else {
        (p - 2)->setScr(x_1, y_1);
    }

    dt2 = C + R;
    if ((dt2 > 0.0) && (dt2 < D)) { // intersection outside start/end point ?
        (p - 1)->setScr(x_1 - x * dt2 / D, y_1 - y * dt2 / D);
        p->setStrokeEnd();
        p += 2;
        code |= 2;
    }
    (p - 1)->setScr(x_2, y_2);

    if (code == 0) {   // intersections both lie outside ?
        (p - 2)->setScr(x_2, y_2);
        --p;
    }

}


// ------------------------------------------------------------
// g->Points must already be empty!!!
// is this a Graph Member?
void Diagram::calcData(Graph *g) {
    double *px;
    double *pz = g->cPointsY;
    if (!pz) return;
    if (g->numAxes() < 1) return;

    int i, z, Counter = 2;
    int Size = ((2 * (g->count(0)) + 1) * g->countY) + 10;

    if (xAxis.autoScale)
        if (yAxis.autoScale)
            if (zAxis.autoScale)
                Counter = -50000;

    double Dummy = 0.0;  // not used
    double *py = &Dummy;

    g->resizeScrPoints(Size);
    auto p = g->begin();
    auto p_end = g->begin();
    p_end += Size - 9;   // limit of buffer
    p->setStrokeEnd();
    ++p;
    assert(p != g->end());

    Axis *pa;
    if (g->yAxisNo == 0) pa = &yAxis;
    else pa = &zAxis;

    switch (g->Style) {
        case GRAPHSTYLE_SOLID: // ***** solid line ****************************
        case GRAPHSTYLE_DASH:
        case GRAPHSTYLE_DOT:
        case GRAPHSTYLE_LONGDASH:

            for (i = g->countY; i > 0; i--) {  // every branch of curves
                px = g->axis(0)->Points;
                calcCoordinateP(px, pz, py, p, pa);
                ++px;
                pz += 2;
                ++p;
                for (z = g->axis(0)->count - 1; z > 0; z--) {  // every point
                    FIT_MEMORY_SIZE;  // need to enlarge memory block ?
                    calcCoordinateP(px, pz, py, p, pa);
                    ++px;
                    pz += 2;
                    ++p;
                    if (Counter >= 2)   // clipping only if an axis is manual
                        clip(p);
                }
                if ((p - 3)->isStrokeEnd() && !(p - 3)->isBranchEnd())
                    p -= 3;  // no single point after "no stroke"
                else if ((p - 2)->isBranchEnd() && !(p - 1)->isGraphEnd()) {
                    if ((!(p - 1)->isPt()))
                        --p; // erase last hidden point
                }
                (p++)->setBranchEnd();
            }

            p->setGraphEnd();
/*z = p-g->Points+1;
p = g->Points;
qDebug("\n****** p=%p", p);
for(int zz=0; zz<z; zz+=2)
  qDebug("c: %d/%d", *(p+zz), *(p+zz+1));*/
            return;

        default:  // symbol (e.g. star) at each point **********************
            for (i = g->countY; i > 0; i--) {  // every branch of curves
                px = g->axis(0)->Points;
                for (z = g->axis(0)->count; z > 0; z--) {  // every point
                    calcCoordinateP(px, pz, py, p, pa);
                    ++px;
                    pz += 2;
                    if (insideDiagramP(p))    // within diagram ?
                        ++p;
                }
                (p++)->setBranchEnd();
                assert(p != g->end());
            }
            (p++)->setGraphEnd();
/*qDebug("\n******");
for(int zz=0; zz<60; zz+=2)
  qDebug("c: %d/%d", *(g->Points+zz), *(g->Points+zz+1));*/
            return;
    }

    // unreachable
}

// -------------------------------------------------------
void Diagram::Bounding(int &_x1, int &_y1, int &_x2, int &_y2) {
    _x1 = cx - Bounding_x1;
    _y1 = cy - y2 - Bounding_y2;
    _x2 = cx + x2 + Bounding_x2;
    _y2 = cy - Bounding_y1;
}

// -------------------------------------------------------
bool Diagram::getSelected(int x_, int y_) {
    if (x_ >= cx - x1)
        if (x_ <= cx + x3)
            if (y_ >= cy - y2)
                if (y_ <= cy + y1)
                    return true;

    return false;
}

/*!
   Checks if the resize area was clicked. If so return "true" and sets
   x1/y1 and x2/y2 to the border coordinates to draw a rectangle.
*/
bool Diagram::resizeTouched(float fX, float fY, float len) {
    float fCX = float(cx), fCY = float(cy);
    float fX2 = float(cx + x2), fY2 = float(cy - y2);
    if (fX < fCX - len) return false;
    if (fX > fX2 + len) return false;
    if (fY < fY2 - len) return false;
    if (fY > fCY + len) return false;

    State = 0;
    if (fX < fCX + len) State = 1;
    else if (fX <= fX2 - len) return false;
    if (fY > fCY - len) State |= 2;
    else if (fY >= fY2 + len) return false;

    return true;
}

// --------------------------------------------------------------------------
void Diagram::getAxisLimits(Graph *pg) {
    // FIXME: Graph should know the limits. but it doesn't yet.
    //        we should only copy here. better: just wrap, dont use {x,y,z}Axis
    int z;
    double x, y, *p;
    DataX const *pD = pg->axis(0);
    if (pD == 0) return;

    if (Name[0] != 'C') {   // not for location curves
        p = pD->Points;
        for (z = pD->count; z > 0; z--) { // check x coordinates (1. dimension)
            x = *(p++);
            if (std::isfinite(x)) {
                if (x > xAxis.max) xAxis.max = x;
                if (x < xAxis.min) xAxis.min = x;
            }
        }
    }

    if (Name == "Rect3D") {
        DataX const *pDy = pg->axis(1);
        if (pDy) {
            p = pDy->Points;
            for (z = pDy->count; z > 0; z--) { // check y coordinates (2. dimension)
                y = *(p++);
                if (std::isfinite(y)) {
                    if (y > yAxis.max) yAxis.max = y;
                    if (y < yAxis.min) yAxis.min = y;
                }
            }
        }
    }

    Axis *pa;
    if (pg->yAxisNo == 0) pa = &yAxis;
    else pa = &zAxis;
    (pa->numGraphs)++;    // count graphs
    p = pg->cPointsY;
    if (p == 0) return;    // if no data => invalid
    for (z = pg->countY * pD->count; z > 0; z--) {  // check every y coordinate
        x = *(p++);
        y = *(p++);

        if (Name[0] != 'C') {
            if (fabs(y) >= 1e-250) x = sqrt(x * x + y * y);
            if (std::isfinite(x)) {
                if (x > pa->max) pa->max = x;
                if (x < pa->min) pa->min = x;
            }
        } else {   // location curve needs different treatment
            if (std::isfinite(x)) {
                if (x > xAxis.max) xAxis.max = x;
                if (x < xAxis.min) xAxis.min = x;
            }
            if (std::isfinite(y)) {
                if (y > pa->max) pa->max = y;
                if (y < pa->min) pa->min = y;
            }
        }
    }
}

// --------------------------------------------------------------------------
void Diagram::loadGraphData(const QString &defaultDataSet) {
    int yNum = yAxis.numGraphs;
    int zNum = zAxis.numGraphs;
    yAxis.numGraphs = zAxis.numGraphs = 0;

    double xmin = xAxis.min, ymin = yAxis.min, zmin = zAxis.min;
    double xmax = xAxis.max, ymax = yAxis.max, zmax = zAxis.max;
    yAxis.min = zAxis.min = xAxis.min = DBL_MAX;
    yAxis.max = zAxis.max = xAxis.max = -DBL_MAX;

    int No = 0;
    for (Graph *pg: Graphs) {
        qDebug() << "load GraphData load" << defaultDataSet << pg->Var;
        if (pg->loadDatFile(defaultDataSet) != 1)   // load data, determine max/min values
            No++;
        getAxisLimits(pg);
    }

    if (No <= 0) {   // All dataset files unchanged ?
        yAxis.numGraphs = yNum;  // rebuild scrollbar position
        zAxis.numGraphs = zNum;

        xAxis.min = xmin;
        yAxis.min = ymin;
        zAxis.min = zmin;
        xAxis.max = xmax;
        yAxis.max = ymax;
        zAxis.max = zmax;
        return;    // -> no update necessary
    }

    if (xAxis.min > xAxis.max)
        xAxis.min = xAxis.max = 0.0;
    if (yAxis.min > yAxis.max)
        yAxis.min = yAxis.max = 0.0;
    if (zAxis.min > zAxis.max)
        zAxis.min = zAxis.max = 0.0;

/*  if((Name == "Polar") || (Name == "Smith")) {  // one axis only
    if(yAxis.min > zAxis.min)  yAxis.min = zAxis.min;
    if(yAxis.max < zAxis.max)  yAxis.max = zAxis.max;
  }*/
    updateGraphData();
}

/*!
   Calculate diagram again without reading dataset from file.
*/
void Diagram::recalcGraphData() {
    yAxis.min = zAxis.min = xAxis.min = DBL_MAX;
    yAxis.max = zAxis.max = xAxis.max = -DBL_MAX;
    yAxis.numGraphs = zAxis.numGraphs = 0;

    // get maximum and minimum values
    for (Graph *pg: Graphs)
        getAxisLimits(pg);

    if (xAxis.min > xAxis.max) {
        xAxis.min = 0.0;
        xAxis.max = 1.0;
    }
    if (yAxis.min > yAxis.max) {
        yAxis.min = 0.0;
        yAxis.max = 1.0;
    }
    if (zAxis.min > zAxis.max) {
        zAxis.min = 0.0;
        zAxis.max = 1.0;
    }
    if ((Name == "Polar") || (Name == "Smith")) {  // one axis only
        if (yAxis.min > zAxis.min) yAxis.min = zAxis.min;
        if (yAxis.max < zAxis.max) yAxis.max = zAxis.max;
    }

    updateGraphData();
}

// ------------------------------------------------------------------------
void Diagram::updateGraphData() {
    int valid = calcDiagram();   // do not calculate graph data if invalid

    for (Graph *pg: Graphs) {
        pg->clear();
        if ((valid & (pg->yAxisNo + 1)) != 0)
            calcData(pg);   // calculate screen coordinates
        else if (pg->cPointsY) {
            delete[] pg->cPointsY;
            pg->cPointsY = 0;
        }
    }

    createAxisLabels();  // virtual function

    // Setting markers must be done last, because in 3D diagram "Mem"
    // is released in "createAxisLabels()".
    for (Graph *pg: Graphs) {
        pg->createMarkerText();
    }
}

// --------------------------------------------------------------------------
/*!
 * does not (yet) load a dat file. only part of it.
 * this way, it would belong to graph.cpp. but it's too obsolete, lets see..
 *
 * FIXME: must invalidate markers.
 */
int Graph::loadDatFile(const QString &fileName) {
    Graph *g = this;
    QFile file;
    QString Variable;
    QFileInfo Info(fileName);

    int pos1 = g->Var.indexOf('/');
//  if(g->Var.right(3) == "].X")  // e.g. stdl[8:0].X
//    if(pos > g->Var.indexOf('['))
//      pos = -1;

    /* WORK-AROUND: A bug in SCIM (libscim) which Qt is linked to causes
       to change the locale to the default. */
    setlocale(LC_NUMERIC, "C");

    QString tail = "";
    QString svar = g->Var;
    if (pos1 > 0) {  // remove simulator signature
        tail = '.' + g->Var.section('/', 0, 0);
        svar = g->Var.mid(pos1 + 1);
    }

    int pos = svar.indexOf(':');
    if (pos <= 0) {
        file.setFileName(fileName + tail);
        Variable = svar;
    } else {
        QString ss3 = Info.path() + QDir::separator() + svar.left(pos) + ".dat" + tail;
        qDebug() << ss3;
        file.setFileName(ss3);
        Variable = svar.mid(pos + 1);
    }

    Info.setFile(file);
    if (g->lastLoaded.isValid())
        if (g->lastLoaded.toMSecsSinceEpoch() >=
            Info.lastModified().toMSecsSinceEpoch()) //Millisecond resulution is needed for tuning
            return 1;    // dataset unchanged -> no update necessary
    qDebug() << "Loading data from " << Info.canonicalFilePath();

    qDeleteAll(g->mutable_axes());
    g->mutable_axes().clear();
    g->countY = 0;
    if (g->cPointsY != nullptr) {
        delete[] g->cPointsY;
        g->cPointsY = nullptr;
    }
    if (Variable.isEmpty()) return 0;

#if 0 // FIXME encapsulation. implement digital waves later.
    if(Variable.right(2) == ".X")
      if(Name.at(0) != 'T')
        return 0;  // digital variables only for tabulars and ziming diagram
#endif

    // PlotVs() emulation
    bool hasExplIndep = false; // Ex[licit indep var
    QString ExplIndep = "";
    if (Variable.contains("@")) {
        hasExplIndep = true;
        ExplIndep = Variable.section("@", 1, 1);
        Variable = Variable.section("@", 0, 0);
    }


    if (!file.open(QIODevice::ReadOnly)) return 0;

    // *****************************************************************
    // To strongly speed up the file read operation the whole file is
    // read into the memory in one piece.
    QByteArray FileContent;
    FileContent = file.readAll();
    file.close();
    char *FileString = FileContent.data();
    if (!FileString) return 0;
    char *pPos = FileString + FileContent.size() - 1;
    if (*pPos > ' ') if (*pPos != '>') return 0;
    *pPos = 0;


    // *****************************************************************
    // look for variable name in data file  ****************************
    bool isIndep = false;
    Variable = "dep " + Variable + " ";
    // "pFile" is used through-out the whole function and must NOT used
    // for other purposes!
    char *pFile = strstr(FileString, Variable.toLatin1());
    while (pFile) {
        if (*(pFile - 1) == '<')     // is dependent variable ?
            break;
        else if (strncmp(pFile - 3, "<in", 3) == 0) {  // is independent variable ?
            isIndep = true;
            break;
        }
        pFile = strstr(pFile + 4, Variable.toLatin1());
    }

    if (!pFile) return 0;   // data not found

    QString Line, tmp;
    pFile += Variable.length();
    pPos = strchr(pFile, '>');
    if (!pPos) return 0;   // file corrupt
    *pPos = 0;
    Line = QString(pFile);
    *pPos = '>';
    pFile = pPos + 1;
    if (!isIndep) {
        pos = 0;
        tmp = Line.section(' ', pos, pos);
        while (!tmp.isEmpty()) {
            if (hasExplIndep)g->mutable_axes().push_back(new DataX(ExplIndep));
            else g->mutable_axes().push_back(new DataX(tmp));  // name of independent variable
            pos++;
            tmp = Line.section(' ', pos, pos);
        }
    }

    // *****************************************************************
    // get independent variable ****************************************
    bool ok = true;
    double *p;
    int counting = 0;
    if (isIndep) {    // create independent variable by myself ?
        counting = Line.toInt(&ok);  // get number of values
        g->mutable_axes().push_back(new DataX("number", 0, counting));
        if (!ok) return 0;

        p = new double[counting];  // memory of new independent variable
        g->countY = 1;
        g->mutable_axes().back()->Points = p;
        for (int z = 1; z <= counting; z++) *(p++) = double(z);
        auto Axis = g->mutable_axes().back();
        Axis->min(1.);
        Axis->max(double(counting));
    } else {  // ...................................
        // get independent variables from data file
        g->countY = 1;
#if 0 // FIXME: we do not have a Name.
        DataX *bLast = 0;
        if(Name == "Rect3D")  bLast = g->axis(1);  // y axis for Rect3D
#endif

#if 0 // FIXME: this is about diagram. do after load.
        double min_tmp = xAxis.min, max_tmp = xAxis.max;
#endif
        DataX const *pD;
        for (int ii = g->numAxes(); (pD = g->axis(--ii));) {
#if 0 // FIXME: this is about diagram. do after load.
            pa = &xAxis;
            if(pD == g->axis(0)) {
              xAxis.min = min_tmp;    // only count first independent variable
              xAxis.max = max_tmp;
            }
            else if(pD == bLast)  pa = &yAxis;   // y axis for Rect3D
#endif
            counting = loadIndepVarData(pD->Var, FileString, mutable_axis(ii));
            if (counting <= 0) return 0;

            g->countY *= counting;
        }
        g->countY /= counting;
    }


    // *****************************************************************
    // get dependent variables *****************************************
    counting *= g->countY;
    p = new double[2 * counting]; // memory for dependent variables
    g->cPointsY = p;
#if 0 // FIXME: what does this do?!
    if(g->yAxisNo == 0)  pa = &yAxis;   // for which axis
    else  pa = &zAxis;
    (pa->numGraphs)++;    // count graphs
#endif

    char *pEnd;
    double x, y;
    pPos = pFile;

    if (Variable.right(3) != ".X ") { // not "digital"

        for (int z = counting; z > 0; z--) {
            pEnd = 0;
            while ((*pPos) && (*pPos <= ' ')) pPos++; // find start of next number
            x = strtod(pPos, &pEnd);  // real part
            pPos = pEnd + 1;
            if (*pEnd < ' ')   // is there an imaginary part ?
                y = 0.0;
            else {
                if (((*pEnd != '+') && (*pEnd != '-')) || (*pPos != 'j')) {
                    delete[] g->cPointsY;
                    g->cPointsY = 0;
                    return 0;
                }
                *pPos = *pEnd;  // overwrite 'j' with sign
                pEnd = 0;
                y = strtod(pPos, &pEnd); // imaginary part
                *pPos = 'j';   // write back old character
                pPos = pEnd;
            }
            *(p++) = x;
            *(p++) = y;
#if 0 // FIXME there is no Name here.
            if(Name[0] != 'C')
#endif
            {
                if (fabs(y) >= 1e-250) x = sqrt(x * x + y * y);
                if (std::isfinite(x)) {
                    auto Axis = g->mutable_axes().back();
                    Axis->min(x);
                    Axis->max(x);
                }
            }

#if 0 // this is not location curve code.
            else {   // location curve needs different treatment
              if(std::isfinite(x)) {
                if(x > xAxis.max) xAxis.max = x;
                if(x < xAxis.min) xAxis.min = x;
              }
              if(std::isfinite(y)) {
                if(y > pa->max) pa->max = y;
                if(y < pa->min) pa->min = y;
              }
            }
#endif
        }

    } else {  // of "if not digital"

        char *pc = (char *) p;
        pEnd = pc + 2 * (counting - 1) * sizeof(double);
        // for digital variables (e.g. 100ZX0):
        for (int z = counting; z > 0; z--) {

            while ((*pPos) && (*pPos <= ' ')) pPos++; // find start of next bit vector
            if (*pPos == 0) {
                delete[] g->cPointsY;
                g->cPointsY = 0;
                return 0;
            }

            while (*pPos > ' ') {    // copy bit vector
                *(pc++) = *(pPos++);
                if (pEnd <= pc) {
                    counting = pc - (char *) g->cPointsY;
                    pc = (char *) realloc(g->cPointsY, counting + 1024);
                    pEnd = pc;
                    g->cPointsY = (double *) pEnd;
                    pc += counting;
                    pEnd += counting + 1020;
                }
            }
            *(pc++) = 0;   // terminate each vector with NULL
        }

    }  // of "if not digital"

    lastLoaded = QDateTime::currentDateTime();
    return 2;
}

/*!
   Reads the data of an independent variable. Returns the number of points.
*/
int Graph::loadIndepVarData(const QString &Variable,
                            char *FileString, DataX *pD) {
    bool isIndep = false;
    QString Line, tmp;

    /* WORK-AROUND: A bug in SCIM (libscim) which Qt is linked to causes
       to change the locale to the default. */
    setlocale(LC_NUMERIC, "C");

    Line = "dep " + Variable + " ";
    // "pFile" is used through-out the whole function and must NOT used
    // for other purposes!
    char *pFile = strstr(FileString, Line.toLatin1());
    while (pFile) {
        if (*(pFile - 1) == '<')     // is dependent variable ?
            break;
        else if (strncmp(pFile - 3, "<in", 3) == 0) {  // is independent variable ?
            isIndep = true;
            break;
        }
        pFile = strstr(pFile + 4, Line.toLatin1());
    }

    if (!pFile) return -1;   // data not found

    pFile += Line.length();
    char *pPos = strchr(pFile, '>');
    if (!pPos) return -1;   // file corrupt
    *pPos = 0;
    Line = QString(pFile);
    *pPos = '>';
    pFile = pPos + 1;
    char *pEnd;
    if (!isIndep) {           // dependent variable can also be used...
        if (Line.indexOf(' ') >= 0) return -1; // ...if only one dependency
        Line = "<indep " + Line + " ";
        pPos = strstr(FileString, Line.toLatin1());
        if (!pPos) return -1;
        pPos += Line.length();
        pEnd = strchr(pPos, '>');
        if (!pEnd) return -1;   // file corrupt
        *pEnd = 0;
        Line = QString(pPos);
        *pEnd = '>';
    }


    bool ok;
    int n = Line.toInt(&ok);  // number of values
    if (!ok) return -1;

    double *p = new double[n];     // memory for new independent variable
//  DataX *pD = pg->mutable_axes().back();
    pD->Points = p;
    pD->count = n;


    double x;
    pPos = pFile;
    // find first position containing no whitespace
    while ((*pPos) && (*pPos <= ' ')) pPos++;

    for (int z = 0; z < n; z++) {
        pEnd = 0;
        x = strtod(pPos, &pEnd);  // real part

        if (*pEnd > ' ')  // drop imaginary part because
            while (*pEnd > ' ') pEnd++; // Complex number on X-axis has no sense

        if (pPos == pEnd) {
            delete[] pD->Points;
            pD->Points = 0;
            return -1;
        }

        *(p++) = x;
#if 0 // this is not location curve code
        if(Name[0] != 'C')   // not for location curves
          if(std::isfinite(x)) {
            if(x > pa->max) pa->max = x;
            if(x < pa->min) pa->min = x;
          }
#endif

        pPos = pEnd;
        while ((*pPos) && (*pPos <= ' ')) pPos++;  // find start of next number
    }

    return n;   // return number of independent data
}

/*!
   Checks if the two graphs have the same independent variables.
*/
bool Diagram::sameDependencies(Graph const *g1, Graph const *g2) const {
    // FIXME
    // return g1->same(*g2);
    if (g1 == g2) return true;
    if (g1->numAxes() != g2->numAxes()) return false;

    for (unsigned i = 0; i < g1->numAxes(); ++i) {
        if (g1->axisName(i) != g2->axisName(i)) return false;
    }

    return true;
}

// ------------------------------------------------------------
int Diagram::checkColumnWidth(const QString &Str,
                              const QFontMetrics &metrics, int colWidth, int x, int y) {
    //qDebug("%i", metrics.charWidth(Str,0));
    int w = metrics.boundingRect(Str).width();  // width of text
    if (w > colWidth) {
        colWidth = w;
        if ((x + colWidth) >= x2) {    // enough space for text ?
            // mark lack of space with a small arrow
            Lines.append(new qucs::Line(x2 - 6, y - 4, x2 + 7, y - 4, QPen(Qt::red, 2)));
            Lines.append(new qucs::Line(x2, y - 7, x2 + 6, y - 4, QPen(Qt::red, 2)));
            Lines.append(new qucs::Line(x2, y - 1, x2 + 6, y - 4, QPen(Qt::red, 2)));
            return -1;
        }
    }
    return colWidth;
}

// ------------------------------------------------------------
Diagram *Diagram::newOne() {
    return new Diagram();
}

// ------------------------------------------------------------
void Diagram::finishMarkerCoordinates(float &fCX, float &fCY) const {
    if (!insideDiagram(fCX, fCY)) {
        fCX = float(x2 >> 1);
        fCY = float(y2 >> 1);
    }
}

// ------------------------------------------------------------
QString Diagram::save() {
    QString s = "<" + Name + " " + QString::number(cx) + " " + QString::number(cy) + " ";
    s += QString::number(x2) + " " + QString::number(y2) + " ";
    char c = '0';
    if (xAxis.GridOn) c |= 1;
    if (hideLines) c |= 2;
    s += c;
    s += " " + GridPen.color().name() + " " + QString::number(GridPen.style());

    if (xAxis.log) s += " 1"; else s += " 0";
    c = '0';
    if (yAxis.log) c |= 1;
    if (zAxis.log) c |= 2;
    s += c;

    if (xAxis.autoScale) s += " 1 ";
    else s += " 0 ";
    s += QString::number(xAxis.limit_min) + " ";
    s += QString::number(xAxis.step) + " ";
    s += QString::number(xAxis.limit_max);
    if (yAxis.autoScale) s += " 1 ";
    else s += " 0 ";
    s += QString::number(yAxis.limit_min) + " ";
    s += QString::number(yAxis.step) + " ";
    s += QString::number(yAxis.limit_max);
    if (zAxis.autoScale) s += " 1 ";
    else s += " 0 ";
    s += QString::number(zAxis.limit_min) + " ";
    s += QString::number(zAxis.step) + " ";
    s += QString::number(zAxis.limit_max) + " ";

    s += QString::number(rotX) + " " + QString::number(rotY) + " " +
         QString::number(rotZ);

    if (engineeringNotation) s += " 1 ";
    else s += " 0 ";

    s += QString::number(yAxis.Units) + " "
         + QString::number(zAxis.Units);

    // labels can contain spaces -> must be last items in the line
    s += " \"" + xAxis.Label + "\" \"" + yAxis.Label + "\" \"" + zAxis.Label + "\">\n";

    for (Graph *pg: Graphs)
        s += pg->save() + "\n";

    s += "  </" + Name + ">";
    return s;
}

// ------------------------------------------------------------
bool Diagram::load(const QString &Line, QTextStream *stream) {
    bool ok;
    QString s = Line;

    if (s.at(0) != '<') return false;
    if (s.at(s.length() - 1) != '>') return false;
    s = s.mid(1, s.length() - 2);   // cut off start and end character

    QString n;
    n = s.section(' ', 1, 1);    // cx
    cx = n.toInt(&ok);
    if (!ok) return false;

    n = s.section(' ', 2, 2);    // cy
    cy = n.toInt(&ok);
    if (!ok) return false;

    n = s.section(' ', 3, 3);    // x2
    x2 = n.toInt(&ok);
    if (!ok) return false;

    n = s.section(' ', 4, 4);    // y2
    y2 = n.toInt(&ok);
    if (!ok) return false;

    char c;
    n = s.section(' ', 5, 5);    // GridOn
    c = n.at(0).toLatin1() - '0';
    xAxis.GridOn = yAxis.GridOn = (c & 1) != 0;
    hideLines = (c & 2) != 0;

    n = s.section(' ', 6, 6);    // color for GridPen
    QColor co = misc::ColorFromString(n);
    GridPen.setColor(co);
    if (!GridPen.color().isValid()) return false;

    n = s.section(' ', 7, 7);    // line style
    GridPen.setStyle((Qt::PenStyle) n.toInt(&ok));
    if (!ok) return false;

    n = s.section(' ', 8, 8);    // xlog, ylog
    xAxis.log = n.at(0) != '0';
    c = n.at(1).toLatin1();
    yAxis.log = ((c - '0') & 1) == 1;
    zAxis.log = ((c - '0') & 2) == 2;

    n = s.section(' ', 9, 9);   // xAxis.autoScale
    if (n.at(0) != '"') {      // backward compatible
        if (n == "1") xAxis.autoScale = true;
        else xAxis.autoScale = false;

        n = s.section(' ', 10, 10);    // xAxis.limit_min
        xAxis.limit_min = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 11, 11);  // xAxis.step
        xAxis.step = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 12, 12);  // xAxis.limit_max
        xAxis.limit_max = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 13, 13);    // yAxis.autoScale
        if (n == "1") yAxis.autoScale = true;
        else yAxis.autoScale = false;

        n = s.section(' ', 14, 14);    // yAxis.limit_min
        yAxis.limit_min = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 15, 15);    // yAxis.step
        yAxis.step = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 16, 16);    // yAxis.limit_max
        yAxis.limit_max = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 17, 17);    // zAxis.autoScale
        if (n == "1") zAxis.autoScale = true;
        else zAxis.autoScale = false;

        n = s.section(' ', 18, 18);    // zAxis.limit_min
        zAxis.limit_min = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 19, 19);    // zAxis.step
        zAxis.step = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 20, 20);    // zAxis.limit_max
        zAxis.limit_max = n.toDouble(&ok);
        if (!ok) return false;

        n = s.section(' ', 21, 21); // rotX
        if (n.at(0) != '"') {      // backward compatible
            rotX = n.toInt(&ok);
            if (!ok) return false;

            n = s.section(' ', 22, 22); // rotY
            rotY = n.toInt(&ok);
            if (!ok) return false;

            n = s.section(' ', 23, 23); // rotZ
            rotZ = n.toInt(&ok);
            if (!ok) return false;

            n = s.section(' ', 24, 24);
            if (n.at(0) != '"') {
                if (n == "1") engineeringNotation = true;
                else engineeringNotation = false;
                n = s.section(' ', 25, 25);
                if (n.at(0) != '"') {
                    yAxis.Units = n.toInt(&ok);
                    if (!ok) return false;

                    n = s.section(' ', 26, 26);
                    zAxis.Units = n.toInt(&ok);
                    if (!ok) return false;
                }
            }
        }
    }

    xAxis.Label = s.section('"', 1, 1);   // xLabel
    yAxis.Label = s.section('"', 3, 3);   // yLabel left
    zAxis.Label = s.section('"', 5, 5);   // yLabel right

    Graph *pg;
    // .......................................................
    // load graphs of the diagram
    while (!stream->atEnd()) {
        s = stream->readLine();
        s = s.trimmed();
        if (s.isEmpty()) continue;

        if (s == ("</" + Name + ">")) return true;  // found end tag ?
        if (s.section(' ', 0, 0) == "<Mkr") {

            // .......................................................
            // load markers of the diagram
            pg = Graphs.last();
            if (!pg) return false;
            assert(pg->parentDiagram() == this);
            Marker *pm = new Marker(pg);
            if (!pm->load(s)) {
                delete pm;
                return false;
            }
            pg->Markers.append(pm);
            continue;
        }

        pg = new Graph(this);
        if (!pg->load(s)) {
            delete pg;
            return false;
        }
        Graphs.append(pg);
    }

    return false;   // end tag missing
}

// --------------------------------------------------------------
void Diagram::calcSmithAxisScale(Axis *Axis, int &GridX, int &GridY) {
    xAxis.low = xAxis.min;
    xAxis.up = xAxis.max;

    Axis->low = 0.0;
    if (fabs(Axis->min) > Axis->max)
        Axis->max = fabs(Axis->min);  // also fit negative values
    if (Axis->autoScale) {
        if (Axis->max > 1.01) Axis->up = 1.05 * Axis->max;
        else Axis->up = 1.0;
        GridX = GridY = 4;
    } else {
        Axis->up = Axis->limit_max = fabs(Axis->limit_max);
        GridX = GridY = int(Axis->step);
    }
}

// ------------------------------------------------------------
void Diagram::createSmithChart(Axis *Axis, int Mode) {
    int GridX;    // number of arcs with re(z)=const
    int GridY;    // number of arcs with im(z)=const
    calcSmithAxisScale(Axis, GridX, GridY);


    if (!xAxis.GridOn) return;

    bool Zplane = ((Mode & 1) == 1);   // impedance or admittance chart ?
    bool Above = ((Mode & 2) == 2);   // paint upper half ?
    bool Below = ((Mode & 4) == 4);   // paint lower half ?

    int dx2 = x2 >> 1;

    double im, n_cos, n_sin, real, real1, real2, root;
    double rMAXq = Axis->up * Axis->up;
    int theta, beta, phi, len, m, x, y;

    int R1 = lround(x2 / Axis->up);
    // ....................................................
    // draw arcs with im(z)=const
    for (m = 1; m < GridY; m++) {
        n_sin = pi * double(m) / double(GridY);
        n_cos = cos(n_sin);
        n_sin = sin(n_sin);
        im = (1.0 - n_cos) / n_sin * pow(Axis->up, 0.7); // up^0.7 is beauty correction
        y = lround(im / Axis->up * x2);  // diameter

        if (Axis->up <= 1.0) {       // Smith chart with |r|=1
            beta = int(16.0 * 180.0 * atan2(n_sin - im, n_cos - 1.0) / pi - 0.5);
            if (beta < 0) beta += 16 * 360;
            theta = 16 * 270 - beta;
        } else {         // Smith chart with |r|>1
            im = 1.0 / im;
            real = (rMAXq + 1.0) / (rMAXq - 1.0);
            root = real * real - im * im - 1.0;
            if (root < 0.0) {  // circle lies completely within the Smith chart ?
                beta = 0;       // yes, ...
                theta = 16 * 360; // ... draw whole circle
            } else {
                // calculate both intersections with most outer circle
                real1 = sqrt(root) - real;
                real2 = -sqrt(root) - real;

                root = (real1 + 1.0) * (real1 + 1.0) + im * im;
                n_cos = (real1 * real1 + im * im - 1.0) / root;
                n_sin = 2.0 * im / root;
                beta = int(16.0 * 180.0 * atan2(n_sin - 1.0 / im, n_cos - 1.0) / pi);
                if (beta < 0) beta += 16 * 360;

                root = (real2 + 1.0) * (real2 + 1.0) + im * im;
                n_cos = (real2 * real2 + im * im - 1.0) / root;
                n_sin = 2.0 * im / root;
                theta = int(16.0 * 180.0 * atan2(n_sin - 1 / im, n_cos - 1) / pi);
                if (theta < 0) theta += 16 * 360;
                theta = theta - beta;   // arc length
                if (theta < 0) theta = 16 * 360 + theta;
            }
        }

        if (Zplane)
            x = (x2 + R1 - y) >> 1;
        else {
            x = (x2 - R1 - y) >> 1;
            beta = 16 * 180 - beta - theta;  // mirror
            if (beta < 0) beta += 16 * 360;   // angle has to be > 0
        }

        if (Above)
            Arcs.append(new struct qucs::Arc(x, dx2 + y, y, y, beta, theta, GridPen));
        if (Below)
            Arcs.append(new struct qucs::Arc(x, dx2, y, y, 16 * 360 - beta - theta, theta, GridPen));
    }

    // ....................................................
    // draw  arcs with Re(z)=const
    theta = 0;       // arc length
    beta = 16 * 180;  // start angle
    if (Above) {
        beta = 0;
        theta = 16 * 180;
    }
    if (Below) theta += 16 * 180;

    for (m = 1; m < GridX; m++) {
        im = m * (Axis->up + 1.0) / GridX - Axis->up;
        y = lround((1.0 - im) / Axis->up * double(dx2));  // diameter

        if (Zplane)
            x = ((x2 + R1) >> 1) - y;
        else
            x = (x2 - R1) >> 1;
        if (fabs(fabs(im) - 1.0) > 0.2)   // if too near to |r|=1, it looks ugly
            Arcs.append(new struct qucs::Arc(x, (x2 + y) >> 1, y, y, beta, theta, GridPen));

        if (Axis->up > 1.0) {  // draw arcs on the rigth-handed side ?
            im = 1.0 - im;
            im = (rMAXq - 1.0) / (im * (im / 2.0 + 1.0)) - 1.0;
            if (Zplane) x += y;
            else x -= y;
            if (im >= 1.0)
                Arcs.append(new struct qucs::Arc(x, (x2 + y) >> 1, y, y, beta, theta, GridPen));
            else {
                phi = int(16.0 * 180.0 / pi * acos(im));
                len = 16 * 180 - phi;
                if (Above && Below) len += len;
                else if (Below) phi = 16 * 180;
                if (!Zplane) phi += 16 * 180;
                Arcs.append(new struct qucs::Arc(x, (x2 + y) >> 1, y, y, phi, len, GridPen));
            }
        }
    }


    // ....................................................
    if (Axis->up > 1.0) {  // draw circle with |r|=1 ?
        x = (x2 - R1) >> 1;
        y = (x2 + R1) >> 1;
        Arcs.append(new struct qucs::Arc(x, y, R1, R1, beta, theta, QPen(Qt::black, 0)));

        // vertical line Re(r)=1 (visible only if |r|>1)
        if (Zplane) x = y;
        y = lround(sqrt(rMAXq - 1) / Axis->up * dx2);
        if (Above) m = y;
        else m = 0;
        if (!Below) y = 0;
        Lines.append(new qucs::Line(x, dx2 + m, x, dx2 - y, GridPen));

        if (Below) y = 4;
        else y = y2 - 4 - QucsSettings.font.pointSize();
        Texts.append(new Text(0, y, misc::StringNum(Axis->up)));
    }

}


// --------------------------------------------------------------
void Diagram::calcPolarAxisScale(Axis *Axis, double &numGrids,
                                 double &GridStep, double &zD) {
    if (Axis->autoScale) {  // auto-scale or user defined limits ?
        double Expo, Base;
        numGrids = floor(double(x2) / 80.0); // minimal grid is 40 pixel
        Expo = floor(log10(Axis->max / numGrids));
        Base = Axis->max / numGrids / pow(10.0, Expo);// get first significant digit
        if (Base < 3.5) {       // use only 1, 2 and 5, which ever is best fitted
            if (Base < 1.5) Base = 1.0;
            else Base = 2.0;
        } else {
            if (Base < 7.5) Base = 5.0;
            else {
                Base = 1.0;
                Expo++;
            }
        }
        GridStep = Base * pow(10.0, Expo); // grid distance in real values
        numGrids -= floor(numGrids - Axis->max / GridStep); // correct num errors
        Axis->up = GridStep * numGrids;

        zD = double(x2) / numGrids;   // grid distance in pixel
    } else {   // no auto-scale
        Axis->up = Axis->limit_max = fabs(Axis->limit_max);
        GridStep = Axis->step;
        zD = double(x2) / Axis->limit_max * Axis->step; // grid distance in pixel

        if (fabs(zD) < 2.0) {  // if grid too small, then no grid
            zD = double(x2);
            GridStep = Axis->step = Axis->up;
            numGrids = 1.0;
        } else numGrids = Axis->limit_max / Axis->step;
    }
}

// ------------------------------------------------------------
void Diagram::createPolarDiagram(Axis *Axis, int Mode) {
    xAxis.low = xAxis.min;
    xAxis.up = xAxis.max;
    Axis->low = 0.0;
    if (fabs(Axis->min) > Axis->max)
        Axis->max = fabs(Axis->min);  // also fit negative values


    bool Above = ((Mode & 1) == 1);  // paint upper half ?
    bool Below = ((Mode & 2) == 2);  // paint lower half ?

    int i, z, tmp;
    if (Above) i = y2; else i = y2 >> 1;
    if (Below) z = 0; else z = y2 >> 1;
    // y line
    Lines.append(new qucs::Line(x2 >> 1, i, x2 >> 1, z, GridPen));

    int len = 0;       // arc length
    int beta = 16 * 180;  // start angle
    if (Above) {
        beta = 0;
        len = 16 * 180;
    }
    if (Below) len += 16 * 180;

    int phi, tPos;
    int tHeight = QucsSettings.font.pointSize() + 5;
    if (!Below) tPos = (y2 >> 1) + 3;
    else tPos = (y2 >> 1) - tHeight + 3;

    double Expo, Base, numGrids, GridStep, zD;
    if (xAxis.GridOn) {
        calcPolarAxisScale(Axis, numGrids, GridStep, zD);

        double zDstep = zD;
        double GridNum = 0.0;
        for (i = int(numGrids); i > 1; i--) {    // create all grid circles
            z = int(zD);
            GridNum += GridStep;
            QString lbl;
            if (engineeringNotation) lbl = misc::num2str(GridNum);
            else lbl = misc::StringNiceNum(GridNum);
            Texts.append(new Text(((x2 + z) >> 1) - 10, tPos, lbl));

            phi = int(16.0 * 180.0 / pi * atan(double(2 * tHeight) / zD));
            if (!Below) tmp = beta + phi;
            else tmp = beta;
            Arcs.append(new struct qucs::Arc((x2 - z) >> 1, (y2 + z) >> 1, z, z, tmp, len - phi,
                                             GridPen));
            zD += zDstep;
        }
    } else {  // of  "if(GridOn)"
        Expo = floor(log10(Axis->max));
        Base = ceil(Axis->max / pow(10.0, Expo) - 0.01);
        Axis->up = Base * pow(10.0, Expo);  // separate Base * 10^Expo
    }

    // create outer circle
    Texts.append(new Text(x2 - 8, tPos, misc::StringNiceNum(Axis->up)));
    phi = int(16.0 * 180.0 / pi * atan(double(2 * tHeight) / double(x2)));
    if (!Below) tmp = phi;
    else tmp = 0;
    Arcs.append(new struct qucs::Arc(0, y2, x2, y2, tmp, 16 * 360 - phi, QPen(Qt::black, 0)));

    // get size of text using the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, 0);
    QSize r = metrics.size(0, Texts.last()->s);  // width of text
    len = x2 + r.width() - 4;   // more space at the right
    if (len > x3) x3 = len;
}

/*!
 Calculations for Cartesian diagrams (RectDiagram and Rect3DDiagram).
 \param   Axis - pointer to the axis to scale
 \param   Dist - length of axis in pixel on the screen
 \return value: "true" if axis runs from largest to smallest value

  \param[out]             GridNum  - number where the first numbered grid is placed
  \param[out]             GridStep - distance from one grid to the next
  \param[out]             zD     - screen coordinate where the first grid is placed
  \param[out]             zDstep - distance on screen from one grid to the next
*/
bool Diagram::calcAxisScale(Axis *Axis, double &GridNum, double &zD,
                            double &zDstep, double &GridStep, double Dist) {
    bool back = false;
    double numGrids, Base, Expo, corr;
    if (Axis->autoScale) {

        if (fabs(Axis->max - Axis->min) < 1e-200) {
            if ((Axis->max == 0.0) && (Axis->min == 0.0)) {
                Axis->up = 1.0;
                Axis->low = -1.0;
            } else {   // if max = min, double difference
                Axis->up = Axis->max + fabs(Axis->max);
                Axis->low = Axis->min - fabs(Axis->min);
            }
        } else if (Axis != &xAxis) {
            // keep a small bounding between graph and  diagram limit
            Axis->up = Axis->max + 0.1 * (Axis->max - Axis->min);
            Axis->low = Axis->min - 0.1 * (Axis->max - Axis->min);
        } else {
            Axis->up = Axis->max;   // normal case for x axis
            Axis->low = Axis->min;
        }


        numGrids = floor(Dist / 60.0);   // minimal grid is 60 pixel
        if (numGrids < 1.0) Base = Axis->up - Axis->low;
        else Base = (Axis->up - Axis->low) / numGrids;
        Expo = floor(log10(Base));
        Base = Base / pow(10.0, Expo);        // separate first significant digit
        if (Base < 3.5) {     // use only 1, 2 and 5, which ever is best fitted
            if (Base < 1.5) Base = 1.0;
            else Base = 2.0;
        } else {
            if (Base < 7.5) Base = 5.0;
            else {
                Base = 1.0;
                Expo++;
            }
        }
        GridStep = Base * pow(10.0, Expo);   // grid distance in real coordinates
        corr = floor((Axis->up - Axis->low) / GridStep - numGrids);
        if (corr < 0.0) corr++;
        numGrids += corr;     // correct rounding faults


        // upper y boundary ...........................
        zD = fabs(fmod(Axis->up, GridStep));// expand grid to upper diagram edge ?
        GridNum = zD / GridStep;
        if ((1.0 - GridNum) < 1e-10) GridNum = 0.0;  // fix rounding errors
        if (Axis->up <= 0.0) {
            if (GridNum < 0.3) {
                Axis->up += zD;
                zD = 0.0;
            }
        } else if (GridNum > 0.7) Axis->up += GridStep - zD;
        else if (GridNum < 0.1)
            if (GridNum * Dist >= 1.0)// more than 1 pixel above ?
                Axis->up += 0.3 * GridStep;  // beauty correction


        // lower y boundary ...........................
        zD = fabs(fmod(Axis->low, GridStep));// expand grid to lower diagram edge ?
        GridNum = zD / GridStep;
        if ((1.0 - GridNum) < 1e-10) zD = GridNum = 0.0;  // fix rounding errors
        if (Axis->low <= 0.0) {
            if (GridNum > 0.7) {
                Axis->low -= GridStep - zD;
                zD = 0.0;
            }
            else if (GridNum < 0.1)
                if (GridNum * Dist >= 1.0) { // more than 1 pixel above ?
                    Axis->low -= 0.3 * GridStep;   // beauty correction
                    zD += 0.3 * GridStep;
                }
        } else {
            if (GridNum > 0.3) {
                zD = GridStep - zD;
                if (GridNum > 0.9) {
                    if ((1.0 - GridNum) * Dist >= 1.0) { // more than 1 pixel above ?
                        Axis->low -= 0.3 * GridStep;    // beauty correction
                        zD += 0.3 * GridStep;
                    }
                }
            } else {
                Axis->low -= zD;
                zD = 0.0;
            }
        }

        GridNum = Axis->low + zD;
        zD /= (Axis->up - Axis->low) / Dist;
    } else {   // user defined limits
        zD = 0.0;
        Axis->low = GridNum = Axis->limit_min;
        Axis->up = Axis->limit_max;
        if (Axis->limit_max < Axis->limit_min)
            back = true;
        GridStep = Axis->step;
    }

    zDstep = GridStep / (Axis->up - Axis->low) * Dist; // grid in pixel

    if (fabs(zDstep) < 2.0) {  // if grid too small, then no grid
        zDstep = Dist;
        GridStep = Axis->step = Axis->up - Axis->low;
    }

    return back;
}

/*!
  Calculations for logarithmical Cartesian diagrams
  (RectDiagram and  Rect3DDiagram).

 \param       Axis   - pointer to the axis to scale
 \param       len    - length of axis in pixel on the screen
 \return value: "true" if axis runs from largest to smallest value

 \param[out]  z      - screen coordinate where the first grid is placed
 \param[out]  zD     - number where the first grid is placed
 \param[out]  zDstep - number increment from one grid to the next
 \param[out]  corr   - scale factor for calculate screen coordinate

 \todo use this as example to document other methods
*/
bool Diagram::calcAxisLogScale(Axis *Axis, int &z, double &zD,
                               double &zDstep, double &corr, int len) {
    bool mirror = false;

    if (Axis->autoScale) {
	double minExp = floor(log10(Axis->min));
	double maxExp = ceil(log10(Axis->max));
	if ( minExp == maxExp ) {
	    minExp -= 1;
	    maxExp += 1;
	}
	Axis->low = pow(10.0, minExp);
	Axis->up = pow(10.0, maxExp);
    } else {
	Axis->low = Axis->limit_min;
	Axis->up = Axis->limit_max;
    }

    if (Axis->up < Axis->low) {
	double tmp = Axis->low;
	Axis->low = Axis->up;
	Axis->up = tmp;
	mirror = true;
    }

    double Expo = floor(log10(Axis->low));
    double Base = ceil(Axis->low / pow(10.0, Expo));
    zD = Base * pow(10.0, Expo);
    zDstep = pow(10.0, Expo);
    if (zD > 9.5 * zDstep) zDstep *= 10.0;

    corr = double(len) / log10(Axis->up / Axis->low);
    z = lround(corr * log10(zD / Axis->low)); // int(..) implies floor(..)

    if (mirror) {   // set back values ?
	double tmp = Axis->low;
	Axis->low = Axis->up;
	Axis->up = tmp;
    }

    return mirror;
}

// --------------------------------------------------------------
bool Diagram::calcYAxis(Axis *Axis, int x0) {
    int z, w;
    double GridStep, corr, zD, zDstep, GridNum;

    QString tmp;
    // get size of text using the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, 0);
    int maxWidth = 0;

    bool back = false;
    if (Axis->log) {
        if (Axis->autoScale) {
            if (Axis->max <= 0.0 or Axis->min <= 0.0) return false;  // invalid
        } else if (Axis->limit_min <= 0.0 or Axis->limit_max <= 0.0) return false;  // invalid

        back = calcAxisLogScale(Axis, z, zD, zDstep, corr, y2);

        if (back) z = y2;
        while ((z <= y2) && (z >= 0)) {    // create all grid lines
            if (Axis->GridOn)
                if (z < y2)
                    if (z > 0) {
                        if (Axis->Units == Axis::NoUnits) {
                            Lines.prepend(new qucs::Line(0, z, x2, z, GridPen));  // y grid
                        }
                    }

            if ((zD < 1.5 * zDstep) || (z == 0)) {
                double yVal = qucs::num2db(zD, Axis->Units);
                if (engineeringNotation) tmp = misc::num2str(yVal, 2);
                else tmp = misc::StringNiceNum(yVal);

                if (Axis->up < 0.0) tmp = '-' + tmp;

                w = metrics.boundingRect(tmp).width();  // width of text
                if (maxWidth < w) maxWidth = w;
                if (x0 > 0)
                    Texts.append(new Text(x0 + 7, z - 6, tmp)); // text aligned left
                else
                    Texts.append(new Text(-w - 7, z - 6, tmp)); // text aligned right

                // y marks
                if (Axis->Units != Axis::NoUnits) {
                    Lines.prepend(new qucs::Line(0, z, x2, z, GridPen));  // y grid
                }
                Lines.append(new qucs::Line(x0 - 5, z, x0 + 5, z, QPen(Qt::black, 0)));
            }

            zD += zDstep;
            if (zD > 9.5 * zDstep) zDstep *= 10.0;
            if (back) {
                z = lround(corr * log10(zD / fabs(Axis->up))); // int() implies floor()
                z = y2 - z;
            } else
                z = lround(corr * log10(zD / fabs(Axis->low)));// int() implies floor()
        }
    } else {  // not logarithmical
        back = calcAxisScale(Axis, GridNum, zD, zDstep, GridStep, double(y2));

        double Expo;
        if (Axis->up == 0.0) Expo = log10(fabs(Axis->up - Axis->low));
        else Expo = log10(fabs(Axis->up));

        zD += 0.5;     // perform rounding
        z = int(zD);   //  "int(...)" implies "floor(...)"
        while ((z <= y2) && (z >= 0)) {  // create all grid lines
            if (fabs(GridNum) < 0.01 * pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
            if (engineeringNotation) tmp = misc::num2str(GridNum);
            else tmp = misc::StringNiceNum(GridNum);

            w = metrics.boundingRect(tmp).width();  // width of text
            if (maxWidth < w) maxWidth = w;
            if (x0 > 0)
                Texts.append(new Text(x0 + 8, z - 6, tmp));  // text aligned left
            else
                Texts.append(new Text(-w - 7, z - 6, tmp));  // text aligned right
            GridNum += GridStep;

            if (Axis->GridOn)
                if (z < y2)
                    if (z > 0)
                        Lines.prepend(new qucs::Line(0, z, x2, z, GridPen));  // y grid
            Lines.append(new qucs::Line(x0 - 5, z, x0 + 5, z, QPen(Qt::black, 0))); // y marks
            zD += zDstep;
            z = int(zD);
        }
    } // of "if(ylog) ... else ..."
    if (x0 == 0) x1 = maxWidth + 14;
    else x3 = x2 + maxWidth + 14;
    return true;
}

// convenience wrappers
bool Diagram::insideDiagramP(Graph::iterator const &p) const {
    float f1 = p->getScrX();
    float f2 = p->getScrY();
    return insideDiagram(f1, f2);
}

void Diagram::calcCoordinateP(const double *x, const double *y, const double *z, Graph::iterator &p, Axis const *A) const {
    float f1, f2;
    calcCoordinate(x, y, z, &f1, &f2, A);
    p->setScr(f1, f2);
};


QRect Diagram::boundingRect() const noexcept
{
    // Despite of having "Bounding_" in the name these are apparently
    // not boungings at all. Computations are taken "as is" from legacy
    // implementation, they work though it's hard to tell how.
    int x1_ = cx - Bounding_x1;
    int y1_ = cy - y2 - Bounding_y2;
    int x2_ = cx + x2 + Bounding_x2;
    int y2_ = cy - Bounding_y1;
    return QRect{QPoint{x1_, y1_}, QPoint{x2_, y2_}}.normalized();
}

//void Diagram::SetLimitsBySelectionRect(QRectF) {}


// vim:ts=8:sw=2:noet
