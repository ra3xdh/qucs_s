/***************************************************************************
                               component.cpp
                              ---------------
    begin                : Sat Aug 23 2003
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
#include <cmath>

#include "component.h"
#include "libcomp.h"
#include "equation.h"
#include "sparamfile.h"
#include "spicefile.h"
#include "subcircuit.h"
#include "main.h"
#include "schematic.h"
#include "module.h"
#include "misc.h"

#include <QPen>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>

/*!
 * \file component.cpp
 * \brief Implementation of the Component class.
 */


/*!
 * \class Component
 * \brief The Component class implements a generic analog component
 */
Component::Component() {
    Type = isAnalogComponent;

    WrongSimulatorPen = QPen(Qt::gray);

    SpiceModel = "";
    isSimulation = false;
    isEquation = false;
    mirroredX = false;
    rotated = 0;
    isSelected = false;
    isActive = COMP_IS_ACTIVE;
    showName = true;

    cx = 0;
    cy = 0;
    tx = 0;
    ty = 0;
    PartCounter = 0; // Identifies the number of component inside the schematic
    XML_Defined = false; // By default, the component is set to be a hardcoded device.
    containingSchematic = NULL;
}

Component::Component(const ComponentInfo& CI) : Component() {
  // Call the default constructor first to initialize all members
  // Then load data from ComponentInfo
  Ports.clear();
  loadfromComponentInfo(CI);
}

// -------------------------------------------------------
Component *Component::newOne() {
  Component *C = new Component();
  // Load component data from LibraryComponents
  QMap<QString, ComponentInfo> Components = LibraryComponents[Category];
  ComponentInfo CI = Components[ComponentName];

  C->loadfromComponentInfo(CI);
  return C;
}

// -------------------------------------------------------
void Component::Bounding(int &_x1, int &_y1, int &_x2, int &_y2) {
    _x1 = x1 + cx;
    _y1 = y1 + cy;
    _x2 = x2 + cx;
    _y2 = y2 + cy;
}

// -------------------------------------------------------
// Size of component text.
int Component::textSize(int &textPropertyMaxWidth, int &totalTextPropertiesHeight) {
    // get size of text using the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, 0);
    int textPropertiesCount = 0;
    textPropertyMaxWidth = totalTextPropertiesHeight = 0;

    if (showName) {
        textPropertyMaxWidth = metrics.boundingRect(Name).width();
        totalTextPropertiesHeight = metrics.height();
        textPropertiesCount++;
    }

    constexpr int flags = 0b00000000;
    for (Property* p : Props) {
        if (!(p->display)) continue;

        // Update overall width if text of the current property is wider
        QString PropertyValue = getValue(p);

        auto w = metrics.size(flags, p->Name + "=" + PropertyValue).width();
        if (w > textPropertyMaxWidth) {
            textPropertyMaxWidth = w;
        }
        // keeps total height of all text properties of component
        // taking line breaks into account
        totalTextPropertiesHeight += metrics.height();
        textPropertiesCount++;
    }
    return textPropertiesCount;
}

// -------------------------------------------------------
// Boundings including the component text.
void Component::entireBounds(int& boundingRectLeft, int& boundingRectTop,
                             int& boundingRectRight, int& boundingRectBottom) {
    boundingRectLeft = std::min(x1, tx) + cx;
    boundingRectTop  = std::min(y1, ty) + cy;

    int textPropertyMaxWidth, totalTextPropertiesHeight;
    textSize(textPropertyMaxWidth, totalTextPropertiesHeight);

    boundingRectRight  = std::max(tx + textPropertyMaxWidth, x2) + cx;
    boundingRectBottom = std::max(ty + totalTextPropertiesHeight, y2) + cy;
}

// -------------------------------------------------------
void Component::setCenter(int x, int y, bool relative) {
    if (relative) {
        cx += x;
        cy += y;
    }
    else {
        cx = x;
        cy = y;
    }
}

// -------------------------------------------------------
void Component::getCenter(int &x, int &y) {
    x = cx;
    y = cy;
}

// Given coordinates of a point (usually coming from a mouse click), finds
// out whether this point is within boundaries of one of component's text
// properties (i.e whether a text property is clicked) and returns the
// index of that property.
//
// Returns:
//     -1 when point is not within bounds of any of component's texts, i.e.
//        click has missed
//      0 when click is within bounds of component name
//  n + 1 when click is within bounds of one of component's properties, `n`
//        is the index of that property
//
// To find out whether given coordinates are within one of text properties,
// we iterate over all properties, computing their bounding rectangles and
// testing if the coordinates lie whithin the rectangle.
//
// Simplified example of component texts and their bounding rectangles:
//
//   (tx,ty)
//      o------+
//      | Name |
//      +------+-----+
//      | prop = val |
//      +------------+---+
//      | prop = multi   |
//      |  line property |
//      +-----------+----+
//      | prop = 10 |
//      +-----------+
//
// (tx,ty) is the top left corner of the region containing all component's
// properties
int Component::getTextSelected(int point_x, int point_y) {
    if (Model == ".CUSTOMSIM") { // block multiline text editing
      return -1;                 // for Nutmeg script
    }
    // cx and cy are subtracted from coordinates to make them
    // component-local, i.e relative to component
    point_x -= cx;
    point_y -= cy;
    if (point_x < tx || point_y < ty) {
        return -1;
    }

    const QPoint click{point_x, point_y};

    // Tracks bottom coordinate of previous bounding rectangle to know
    // where the next text's bounding rectangle's top should be placed
    int bounding_rect_top = ty;

    // Tracks the number of processed texts. We have to return the index of
    // the text being "clicked"
    int text_index = 0;

    const QFontMetrics font_metrics(QucsSettings.font, 0);
    const int flags = 0b00000000;

    if (showName) {
        QRect text_br{{tx, bounding_rect_top}, font_metrics.size(flags, Name)};
        if (text_br.contains(click)) {
            return text_index;
        }

        bounding_rect_top = text_br.bottom();
    }
    text_index += 1;

    for (auto* prop : Props) {
        if (!prop->display) {
            text_index += 1;
            continue;
        }

        // The value of the parameter may be an expression. In that case, it must be parsed before displaying
        QString PropertyValue = getValue(prop);

        QRect text_br{{tx, bounding_rect_top}, font_metrics.size(flags, prop->Name + "=" + PropertyValue)};
        if (text_br.contains(click)) {
            return text_index;
        }

        bounding_rect_top = text_br.bottom();
        text_index += 1;
    }

    return -1;
}


// -------------------------------------------------------
bool Component::getSelected(int x_, int y_) {
    x_ -= cx;
    y_ -= cy;
    if (x_ >= x1)
        if (x_ <= x2)
            if (y_ >= y1)
                if (y_ <= y2)
                    return true;

    return false;
}

void Component::paint(QPainter *p) {
    p->save();
    p->translate(cx, cy);

    drawSymbol(p);

    p->setPen(QPen(Qt::black, 1));
    QRect text_br{tx, ty, 0, 0};

    if (showName) {
        p->drawText(tx, ty, 1, 1, Qt::TextDontClip, Name, &text_br);
    }

    for (auto *prop : Props) {
      if (!prop->display) continue;

      QString displayValue = prop->Value;
      if (prop->IsExpression) {
        // Evaluate the expression
        double result = evaluateExpression(prop->Value);
        // Display only two decimals. If the value is lower than 0.1, use scientific notation
        if (std::abs(result) < 0.1 && result != 0) {
          displayValue = QString::number(result, 'e', 2);
        } else {
          displayValue = QString::number(result, 'f', 2);
        }      }

      bool isNumber;
      displayValue.toDouble(&isNumber);
      if (isNumber) {
        // The string is a number. Let's check if the property has units to be attached
        if (!prop->Unit.isEmpty()) {
          displayValue.append(QString(" "));
          displayValue.append(prop->Unit);
        }
      }


      // Modify the Property::paint function call to include the evaluated value
      prop->paint(text_br.left(), text_br.bottom(), p, displayValue);
      text_br = prop->boundingRect();
    }

    if (isActive == COMP_IS_OPEN)
        p->setPen(QPen(Qt::red, 0));
    else if (isActive & COMP_IS_SHORTEN)
        p->setPen(QPen(Qt::darkGreen, 0));

    if (isActive != COMP_IS_ACTIVE) {
        p->drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
        p->drawLine(x1, y1, x2, y2);
        p->drawLine(x1, y2, x2, y1);
    }

    // draw component bounding box
    if (isSelected) {
        p->setPen(QPen(Qt::darkGray, 3));
        p->drawRoundedRect(x1, y1, x2 - x1, y2 - y1, 4, 4);
    }

    p->restore();
}

void Component::drawSymbol(QPainter* p) {
    const bool correctSimulator = (Simulator & QucsSettings.DefaultSimulator) == QucsSettings.DefaultSimulator;

    auto draw_primitive = [&](qucs::DrawingPrimitive* prim, QPainter* p) {
        p->save();
        p->setPen(correctSimulator ? prim->penHint() : WrongSimulatorPen);
        p->setBrush(prim->brushHint());
        prim->draw(p);
        p->restore();
    };

    for (qucs::DrawingPrimitive *line: Lines) {
        draw_primitive(line, p);
    }

    for (qucs::DrawingPrimitive *pl: Polylines) {
        draw_primitive(pl, p);
    }

    for (qucs::DrawingPrimitive *arc: Arcs) {
        draw_primitive(arc, p);
    }

    for (qucs::DrawingPrimitive *rect: Rects) {
        draw_primitive(rect, p);
    }

    for (qucs::DrawingPrimitive *ellips: Ellipses) {
        draw_primitive(ellips, p);
    }

    for (qucs::DrawingPrimitive *text: Texts) {
        draw_primitive(text, p);
    }
}

// paint device icon for left panel list
void Component::paintIcon(QPixmap* pixmap) {
    pixmap->fill(Qt::transparent);

    QPainter painter{pixmap};
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    if ((x1 == 0) && (x2 == 0))
    {
      // Default limits
      // TO DO: Calculate this from the lines and port coordinates.
      x1 = -30;
      x2 = 30;
      y1 = -40;
      y2 = 40;
    }

    const QRectF component_br{
        QRect{x1, y1, x2 - x1, y2 - y1}.marginsAdded(QMargins{5, 5, 5, 5})};
    const QRectF picture{pixmap->rect()};

    const double scale = std::min(picture.height() / component_br.height(),
                                  picture.width() / component_br.width());

    painter.translate(picture.center().x() - component_br.center().x(),
                      picture.center().y() - component_br.center().y());
    painter.scale(scale, scale);
    // These have to be applied after scaling. TBH I am not quite sure
    // how it works, but it makes icons better aligned.
    painter.translate(icon_dx, icon_dy);

    painter.save();

    painter.setPen(QPen{Qt::red});
    for (auto* port : Ports) {
        painter.drawEllipse(port->x - 2, port->y - 2, 4, 4);
    }

    for (qucs::Line* line : Lines) {
        painter.setPen(line->penHint());
        line->draw(&painter);
    }

    for (qucs::Polyline* pl : Polylines) {
        painter.setPen(pl->penHint());
        pl->draw(&painter);
    }

    for (qucs::Arc* arc : Arcs) {
        painter.setPen(arc->penHint());
        arc->draw(&painter);
    }

    for (qucs::Rect* rect : Rects) {
        painter.setPen(rect->penHint());
        painter.setBrush(rect->brushHint());
        rect->draw(&painter);
    }

    for (qucs::Ellips* ellipse : Ellipses) {
        painter.setPen(ellipse->penHint());
        painter.setBrush(ellipse->brushHint());
        ellipse->draw(&painter);
    }
    painter.restore();

    for (Text* pt : Texts) {
        pt->draw(&painter);
    }
}

// -------------------------------------------------------
// Paints the component when moved with the mouse.
void Component::paintScheme(Schematic *p) {
    // qDebug() << "paintScheme" << Model;
    if (Model.at(0) == '.' || isEquation) {   // is simulation component (dc, ac, ...) + Equations
//        int a, b, xb, yb;
//        QFont newFont = p->font();
//
//        float Scale =
//                ((Schematic *) QucsMain->DocumentTab->currentWidget())->Scale;
//        newFont.setPointSizeF(float(Scale) * QucsSettings.largeFontSize);
//        newFont.setWeight(QFont::DemiBold);
//        // here the font metric is already the screen metric, since the font
//        // is the current font the painter is using
//        QFontMetrics metrics(newFont);
//
//        a = b = 0;
//        QSize r;
//        for (Text *pt: Texts) {
//            r = metrics.size(0, pt->s);
//            b += r.height();
//            if (a < r.width()) a = r.width();
//        }
//        xb = a + int(12.0 * Scale);
//        yb = b + int(10.0 * Scale);
//        x2 = x1 + 25 + int(float(a) / Scale);
//        y2 = y1 + 23 + int(float(b) / Scale);
//        if (ty < y2 + 1) if (ty > y1 - r.height()) ty = y2 + 1;
//
//        p->PostPaintEvent(_Rect, cx - 6, cy - 5, xb, yb);
//        p->PostPaintEvent(_Line, cx - 1, cy + yb, cx - 6, cy + yb - 5);
//        p->PostPaintEvent(_Line, cx + xb - 2, cy + yb, cx - 1, cy + yb);
//        p->PostPaintEvent(_Line, cx + xb - 2, cy + yb, cx + xb - 6, cy + yb - 5);
//        p->PostPaintEvent(_Line, cx + xb - 2, cy + yb, cx + xb - 2, cy);
//        p->PostPaintEvent(_Line, cx + xb - 2, cy, cx + xb - 6, cy - 5);

        int _x1, _x2, _y1, _y2;
        // textCorr to entireBounds
        entireBounds(_x1, _y1, _x2, _y2);
        p->PostPaintEvent(_Rect, _x1, _y1, _x2 - _x1, _y2 - _y1);

        return;
    }

    // paint all lines
    for (qucs::Line *p1: Lines)
        p->PostPaintEvent(_Line, cx + p1->x1, cy + p1->y1, cx + p1->x2, cy + p1->y2);

    for (auto* pl : Polylines) {
        for (size_t i = 1; i < pl->points.size(); i++) {
            auto& prev = pl->points.at(i - 1);
            auto& curr = pl->points.at(i);
            p->PostPaintEvent(_Line, cx + prev.x(), cy + prev.y(), cx + curr.x(), cy + curr.y());
        }
    }

    // paint all ports
    for (Port *p2 : Ports)
      if (p2->avail) {
        Node *node = p2->Connection;
        if (!node) {
          p->PostPaintEvent(_Ellipse, cx + p2->x - 4, cy + p2->y - 4, 8, 8);
        }
      }

    for (qucs::Arc *p3: Arcs)   // paint all arcs
        p->PostPaintEvent(_Arc, cx + p3->x, cy + p3->y, p3->w, p3->h, p3->angle, p3->arclen);


    for (qucs::Rect *pa: Rects) // paint all rectangles
        p->PostPaintEvent(_Rect, cx + pa->x, cy + pa->y, pa->w, pa->h);

    for (qucs::Ellips *pa: Ellipses) // paint all ellipses
        p->PostPaintEvent(_Ellipse, cx + pa->x, cy + pa->y, pa->w, pa->h);
}

// -------------------------------------------------------
// Rotates the component 90 counter-clockwise around its center
void Component::rotate() {
    // Port count only available after recreate, createSymbol
    if ((Model != "Sub") && (Model != "VHDL") && (Model != "Verilog")
        && (Model != "SpLib")) // skip port count
        if (Ports.count() < 1) return;  // do not rotate components without ports
    int tmp, dx, dy;

    // rotate all lines
    for (qucs::Line *p1: Lines) {
        tmp = -p1->x1;
        p1->x1 = p1->y1;
        p1->y1 = tmp;
        tmp = -p1->x2;
        p1->x2 = p1->y2;
        p1->y2 = tmp;
    }

    // rotate all ports
    for (Port *p2: Ports) {
        tmp = -p2->x;
        p2->x = p2->y;
        p2->y = tmp;
    }

    // rotate all arcs
    for (qucs::Arc *p3: Arcs) {
        tmp = -p3->x;
        p3->x = p3->y;
        p3->y = tmp - p3->w;
        tmp = p3->w;
        p3->w = p3->h;
        p3->h = tmp;
        p3->angle += 16 * 90;
        if (p3->angle >= 16 * 360) p3->angle -= 16 * 360;
    }

    // rotate all rectangles
    for (qucs::Rect *pa: Rects) {
        tmp = -pa->x;
        pa->x = pa->y;
        pa->y = tmp - pa->w;
        tmp = pa->w;
        pa->w = pa->h;
        pa->h = tmp;
    }

    // rotate all ellipses
    for (qucs::Ellips *pa: Ellipses) {
        tmp = -pa->x;
        pa->x = pa->y;
        pa->y = tmp - pa->w;
        tmp = pa->w;
        pa->w = pa->h;
        pa->h = tmp;
    }

    // rotate all text
    float ftmp;
    for (Text *pt: Texts) {
        tmp = -pt->x;
        pt->x = pt->y;
        pt->y = tmp;

        ftmp = -pt->mSin;
        pt->mSin = pt->mCos;
        pt->mCos = ftmp;
    }

    for (qucs::Polyline* pl : Polylines) {
        for (auto& pt : pl->points) {
            std::swap(pt.rx(), pt.ry());
            pt.ry() *= -1;
        };
    }

    tmp = -x1;   // rotate boundings
    x1 = y1;
    y1 = -x2;
    x2 = y2;
    y2 = tmp;

    tmp = -tx;    // rotate text position
    tx = ty;
    ty = tmp;
    // use the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, 0);   // get size of text
    dx = dy = 0;
    if (showName) {
        dx = metrics.boundingRect(Name).width();
        dy = metrics.lineSpacing();
    }
    for (Property *pp : Props)
        if (pp->display) {
            // get width of text
            QString PropertyValue = getValue(pp);
            tmp = metrics.boundingRect(pp->Name + "=" + PropertyValue).width();
            if (tmp > dx) dx = tmp;
            dy += metrics.lineSpacing();
        }
    if (tx > x2) ty = y1 - ty + y2;    // rotate text position
    else if (ty < y1) ty -= dy;
    else if (tx < x1) {
        tx += dy - dx;
        ty = y1 - ty + y2;
    }
    else ty -= dx;

    if (containingSchematic != nullptr) {
        containingSchematic->setOnGrid(cx,cy);
    }

    rotated++;  // keep track of what's done
    rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the x-axis.
void Component::mirrorX() {
    // Port count only available after recreate, createSymbol
    if ((Model != "Sub") && (Model != "VHDL") && (Model != "Verilog")
        && (Model != "SpLib")) // skip port count
        if (Ports.count() < 1) return;  // do not rotate components without ports

    // mirror all lines
    for (qucs::Line *p1: Lines) {
        p1->y1 = -p1->y1;
        p1->y2 = -p1->y2;
    }

    // mirror all ports
    for (Port *p2: Ports)
        p2->y = -p2->y;

    // mirror all arcs
    for (qucs::Arc *p3: Arcs) {
        p3->y = -p3->y - p3->h;
        if (p3->angle > 16 * 180) p3->angle -= 16 * 360;
        p3->angle = -p3->angle;    // mirror
        p3->angle -= p3->arclen;    // go back to end of arc
        if (p3->angle < 0) p3->angle += 16 * 360;  // angle has to be > 0
    }

    // mirror all rectangles
    for (qucs::Rect *pa: Rects)
        pa->y = -pa->y - pa->h;

    // mirror all ellipses
    for (qucs::Ellips *pa: Ellipses)
        pa->y = -pa->y - pa->h;

    QFont f = QucsSettings.font;
    // mirror all text
    for (Text *pt: Texts) {
        f.setPixelSize(pt->Size);
        // use the screen-compatible metric
        QFontMetrics smallMetrics(f, 0);
        QSize s = smallMetrics.size(0, pt->s);   // use size for more lines
        pt->y = -pt->y - int(pt->mCos) * s.height() + int(pt->mSin) * s.width();
    }

    for (qucs::Polyline* pl : Polylines) {
        for (auto& pt : pl->points) {
            pt.ry() *= -1;
        }
    }

    int tmp = y1;
    y1 = -y2;
    y2 = -tmp;   // mirror boundings
    // use the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, nullptr);   // get size of text
    int dy = 0;
    if (showName)
        dy = metrics.lineSpacing();   // for "Name"
    for (Property *pp : Props)
        if (pp->display) dy += metrics.lineSpacing();
    if ((tx > x1) && (tx < x2)) ty = -ty - dy;     // mirror text position
    else ty = y1 + ty + y2;

    mirroredX = !mirroredX;    // keep track of what's done
    rotated += rotated << 1;
    rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the y-axis.
void Component::mirrorY() {
    // Port count only available after recreate, createSymbol
    if ((Model != "Sub") && (Model != "VHDL") && (Model != "Verilog")
        && (Model != "SpLib")) // skip port count
        if (Ports.count() < 1) return;  // do not rotate components without ports

    // mirror all lines
    for (qucs::Line *p1: Lines) {
        p1->x1 = -p1->x1;
        p1->x2 = -p1->x2;
    }

    // mirror all ports
    for (Port *p2: Ports)
        p2->x = -p2->x;

    // mirror all arcs
    for (qucs::Arc *p3: Arcs) {
        p3->x = -p3->x - p3->w;
        p3->angle = 16 * 180 - p3->angle - p3->arclen;  // mirror
        if (p3->angle < 0) p3->angle += 16 * 360;   // angle has to be > 0
    }

    // mirror all rectangles
    for (qucs::Rect *pa: Rects)
        pa->x = -pa->x - pa->w;

    // mirror all ellipses
    for (qucs::Ellips *pa: Ellipses)
        pa->x = -pa->x - pa->w;

    int tmp;
    QFont f = QucsSettings.font;
    // mirror all text
    for (Text *pt: Texts) {
        f.setPixelSize(pt->Size);
        // use the screen-compatible metric
        QFontMetrics smallMetrics(f, 0);
        QSize s = smallMetrics.size(0, pt->s);   // use size for more lines
        pt->x = -pt->x - int(pt->mSin) * s.height() - int(pt->mCos) * s.width();
    }

    for (qucs::Polyline* pl : Polylines) {
        for (auto& pt : pl->points) {
            pt.rx() *= -1;
        }
    }

    tmp = x1;
    x1 = -x2;
    x2 = -tmp;   // mirror boundings
    // use the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, 0);   // get size of text
    int dx = 0;
    if (showName)
        dx = metrics.boundingRect(Name).width();
    for (Property *pp : Props)
        if (pp->display) {
            // get width of text
            QString PropertyValue = getValue(pp);
            tmp = metrics.boundingRect(pp->Name + "=" + PropertyValue).width();
            if (tmp > dx) dx = tmp;
        }
    if ((ty > y1) && (ty < y2)) tx = -tx - dx;     // mirror text position
    else tx = x1 + tx + x2;

    mirroredX = !mirroredX;   // keep track of what's done
    rotated += rotated << 1;
    rotated += 2;
    rotated &= 3;
}

// -------------------------------------------------------
QString Component::netlist() {

  if (!Name.compare("Ground")) { // This was included to prevent GND component to be included as a component in the netlist.
    return QString();
  }

  if (Netlists.isEmpty()) {
    // Components not present in the XML libraries, such as simulation blocks.
    QString s = Model + ":" + Name;
    // output all node names
    for (Port *p1: Ports) {
      s += " " + p1->Connection->Name;   // node names
    }

           // output all properties
    for (Property *p2 : Props) {
      if (p2->Name != "Symbol") {
        QString PropertyValue = getValue(p2);
        s += " " + p2->Name + "=\"" + PropertyValue + "\"";
      }
    }
    return s + '\n';
  } else {
    // Components defined in the XML library
    QString nets;
    QString netlist_line;

    // Get the nodes at which the component is connected
    for (Port *p1: Ports) {
      nets += QString("%1 ").arg(p1->Connection->Name);   // node names
    }
    nets.chop(1); // Remove the last space

    // Check the current simulator and get its template line
    if (QucsSettings.DefaultSimulator == spicecompat::simNgspice) {
      netlist_line = Netlists["Ngspice"];
    } else {
      if (QucsSettings.DefaultSimulator == spicecompat::simQucsator) {
        netlist_line = Netlists["Qucsator"];
      } else {

      }
    }

    // Set the component number in the schematic
    netlist_line.replace(QString("{PartCounter}"), QString::number(PartCounter));

    // Add connections
    netlist_line.replace("{nets}", nets);

    // For each property in the template get the actual values. It is looking for
    // variables inside brackets, defined in the XML library. Once found, they need
    // to be replaced by its actual value. For doing this, the program must to find them
    // on QList<Property*> Props;

    QRegularExpression re("\\{([^}]+)\\}");
    QRegularExpressionMatchIterator i = re.globalMatch(netlist_line);

    while (i.hasNext()) {
      QRegularExpressionMatch match = i.next();
      QString placeholder = match.captured(1);

      // Search for the property in the QList
      auto it = std::find_if(Props.begin(), Props.end(),
                             [&placeholder](const Property* prop) {
                               return prop->Name == placeholder;
                             });

      if (it != Props.end()) {
        // Replace the placeholder with the property's value
        // If the simulation is to be run in Ngspice, remove the unit and leave only the greek suffix
        QString value = getValue(*it);
        if (QucsSettings.DefaultSimulator == spicecompat::simNgspice){
          value = extractValue(value);
        }
        netlist_line.replace(QString("{%1}").arg(placeholder), value);
      }
    }

    netlist_line.append('\n');
    return netlist_line;
  }
}

// This function is used to convert the user input to Ngspice values. For example, values like "1000 Ohm" -> "1000", "1k" -> "1k"
QString Component::extractValue(const QString &input) {
  // Regular expression to match numbers in standard or scientific notation,
  // optionally followed by a unit prefix
  QRegularExpression regex("(\\d+(?:\\.\\d+)?(?:[eE][+-]?\\d+)?)\\s*([fpnumkMGTPEZY]?)");
  QRegularExpressionMatch match = regex.match(input);

  if (match.hasMatch()) {
    QString value = match.captured(1);
    QString suffix = match.captured(2);

           // If the value is in scientific notation, convert it to a double and back to a string
           // to normalize the representation
    if (value.contains('e', Qt::CaseInsensitive)) {
      bool ok;
      double numValue = value.toDouble(&ok);
      if (ok) {
        value = QString::number(numValue, 'g', 12);  // 12 significant digits
      }
    }

    return value + suffix;
  }

  return QString();
}


// Forms spice parameter list
// ignore_list --- QStringList with spice_incompatible parameters
// convert_list ---  QString list with parameters that needs names conversion
//                   list format is: ( qucs_parameter_name<i> , spice_parameter_name<i>, ... )

QString Component::form_spice_param_list(QStringList &ignore_list, QStringList &convert_list) {
    QString par_str = "";

    for (int i = 0; i < Props.count(); i++) {
        if (!ignore_list.contains(Props.at(i)->Name)) {
            QString unit, nam;
            if (convert_list.contains(Props.at(i)->Name)) {
                nam = convert_list.at(convert_list.indexOf(Props.at(i)->Name) + 1);
            } else {
                nam = Props.at(i)->Name;
            }
            QString PropertyValue = getValue(Props.at(i));
            QString val = spicecompat::normalize_value(PropertyValue);
            par_str += QStringLiteral("%1=%2 ").arg(nam).arg(val);
        }

    }

    return par_str;
}

QString Component::spice_netlist(spicecompat::SpiceDialect) {
    return QStringLiteral("\n"); // ignore if not implemented
}

QString Component::cdl_netlist() {
    return QStringLiteral("\n"); // ignore if not implemented
}

QString Component::va_code() {
    return QString(); // ignore if not implemented
}

// -------------------------------------------------------
QString Component::getNetlist() {
    switch (isActive) {
        case COMP_IS_ACTIVE:
            return netlist();
        case COMP_IS_OPEN:
            return QString();
    }

    // Component is shortened.
    int z = 0;
    QListIterator<Port *> iport(Ports);
    Port *pp = iport.next();
    QString Node1 = pp->Connection->Name;
    QString s = "";
    while (iport.hasNext())
        s += "R:" + Name + "." + QString::number(z++) + " " +
             Node1 + " " + iport.next()->Connection->Name + " R=\"0\"\n";
    return s;
}

QString Component::getSpiceNetlist(spicecompat::SpiceDialect dialect /* = SPICEDefault */) {
    QString s;
    switch (isActive) {
        case COMP_IS_ACTIVE:
            s = dialect == spicecompat::CDL ? cdl_netlist() : spice_netlist(dialect);
            s.replace(" gnd ", " 0 ");
            return s;
        case COMP_IS_OPEN:
            return QString();
    }

    // Component is shortened.
    int z = 0;
    QListIterator<Port *> iport(Ports);
    Port *pp = iport.next();
    QString Node1 = pp->Connection->Name;
    s = "";
    while (iport.hasNext()) // Add minR resistors
        s += "R" + Name + QString::number(z++) + " " +
             Node1 + " " + iport.next()->Connection->Name + " 1e-12\n";

    s.replace(" gnd ", " 0 ");
    return s;
}

QString Component::getVerilogACode() {
    QString s;
    switch (isActive) {
        case COMP_IS_ACTIVE:
            s = va_code();
            return s;
        case COMP_IS_OPEN:
        default:
            return QString();
    }
}

QString Component::getExpression(spicecompat::SpiceDialect) {
    return QString();
}

QString Component::getEquations(QString, QStringList &) {
    return QString();
}

QStringList Component::getExtraVariables() {
    return QStringList();
}

QString Component::getProbeVariable(spicecompat::SpiceDialect) {
    return QString();
}

QString Component::getSpiceModel() {
    return QString();
}

QString Component::getNgspiceBeforeSim(QString sim, int lvl) {
    Q_UNUSED(sim) // To suppress warning
    Q_UNUSED(lvl)
    return QString();
}

QString Component::getNgspiceAfterSim(QString sim, int lvl) {
    Q_UNUSED(sim) // To suppress warning
    Q_UNUSED(lvl)
    return QString();
}

// -------------------------------------------------------
QString Component::verilogCode(int) {
    return QString();   // no digital model
}

// -------------------------------------------------------
QString Component::get_Verilog_Code(int NumPorts) {
    switch (isActive) {
        case COMP_IS_OPEN:
            return QString();
        case COMP_IS_ACTIVE:
            return verilogCode(NumPorts);
    }

    // Component is shortened.
    QListIterator<Port *> iport(Ports);
    Port *pp = iport.next();
    QString Node1 = pp->Connection->Name;
    QString s = "";
    while (iport.hasNext())
        s += "  assign " + iport.next()->Connection->Name + " = " + Node1 + ";\n";
    return s;
}

// -------------------------------------------------------
QString Component::vhdlCode(int) {
    return QString();   // no digital model
}

// -------------------------------------------------------
QString Component::get_VHDL_Code(int NumPorts) {
    switch (isActive) {
        case COMP_IS_OPEN:
            return QString();
        case COMP_IS_ACTIVE:
            return vhdlCode(NumPorts);
    }

    // Component is shortened.
    // This puts the signal of the second port onto the first port.
    // This is logically correct for the inverter only, but makes
    // some sense for the gates (OR, AND etc.).
    // Has anyone a better idea?
    QString Node1 = Ports.at(0)->Connection->Name;
    return "  " + Node1 + " <= " + Ports.at(1)->Connection->Name + ";\n";
}

// -------------------------------------------------------
QString Component::save() {
#if XML
    QDomDocument doc;
    QDomElement el = doc.createElement (Model);
    doc.appendChild (el);
    el.setTagName (Model);
    el.setAttribute ("inst", Name.isEmpty() ? "*" : Name);
    el.setAttribute ("display", isActive | (showName ? 4 : 0));
    el.setAttribute ("cx", cx);
    el.setAttribute ("cy", cy);
    el.setAttribute ("tx", tx);
    el.setAttribute ("ty", ty);
    el.setAttribute ("mirror", mirroredX);
    el.setAttribute ("rotate", rotated);

    for (Property *pr = Props.first(); pr != 0; pr = Props.next()) {
      QString val = pr->Value;
      val.replace("\n","\\n");
      val.replace("\"","''");
      el.setAttribute (pr->Name, (pr->display ? "1@" : "0@") + val);
    }
    qDebug (doc.toString());
#endif
    QString s;

    if (XML_Defined) {
      // To avoid ambiguity, each line contains the library and the component model. Spaces are replaced by "{ }"
      s = "<" + QString(Category).replace(" ", "{_}") + ":" + QString(ComponentName).replace(" ", "{_}") ;
    } else {
      s = "<" + Model;
    }

    if (Name.isEmpty()) s += " * ";
    else s += " " + Name + " ";

    int i = 0;
    if (!showName)
        i = 4;
    i |= isActive;
    s += QString::number(i);
    s += " " + QString::number(cx) + " " + QString::number(cy);
    s += " " + QString::number(tx) + " " + QString::number(ty);
    if (mirroredX) s += " 1";
    else s += " 0";
    s += " " + QString::number(rotated);

    // write all properties
    for (Property *p1 : Props) {
        QString val = getValue(p1); // enable newline in properties
        val.replace("\n", "\\n");
        val.replace("\"", "''");
        if (p1->Description.isEmpty() || (p1->Description == "Expression"))
            s += " \"" + p1->Name + "=" + val + "\"";   // e.g. for equations
        else s += " \"" + val + "\"";
        if (p1->display) s += " 1";
        else s += " 0";
    }

    return s + ">";
}

// -------------------------------------------------------
bool Component::load(const QString &_s) {
    bool ok;
    int ttx, tty, tmp;
    QString s = _s;

    if (s.at(0) != '<') return false;
    if (s.at(s.length() - 1) != '>') return false;
    s = s.mid(1, s.length() - 2);   // cut off start and end character

    QString n;
    Name = s.section(' ', 1, 1);    // Name
    if (Name == "*") Name = "";

    n = s.section(' ', 2, 2);      // isActive
    tmp = n.toInt(&ok);
    if (!ok) return false;
    isActive = tmp & 3;

    if (tmp & 4)
        showName = false;
    else
        showName = true;

    n = s.section(' ', 3, 3);    // cx
    cx = n.toInt(&ok);
    if (!ok) return false;

    n = s.section(' ', 4, 4);    // cy
    cy = n.toInt(&ok);
    if (!ok) return false;

    n = s.section(' ', 5, 5);    // tx
    ttx = n.toInt(&ok);
    if (!ok) return false;

    n = s.section(' ', 6, 6);    // ty
    tty = n.toInt(&ok);
    if (!ok) return false;

    if (Model.at(0) != '.') {  // is simulation component (dc, ac, ...) ?

        n = s.section(' ', 7, 7);    // mirroredX
        if (n.toInt(&ok) == 1) mirrorX();
        if (!ok) return false;

        n = s.section(' ', 8, 8);    // rotated
        tmp = n.toInt(&ok);
        if (!ok) return false;
        if (rotated > tmp)  // necessary because of historical flaw in ...
            tmp += 4;        // ... components like "volt_dc"
        for (int z = rotated; z < tmp; z++) rotate();

    }

    tx = ttx;
    ty = tty; // restore text position (was changed by rotate/mirror)

    unsigned int counts = s.count('"');
    if (Model == "Sub")
        tmp = 2;   // first property (File) already exists
    else if (Model == "Lib")
        tmp = 3;
    else if (Model == "EDD")
        tmp = 5;
    else if (Model == "RFEDD")
        tmp = 8;
    else if (Model == "VHDL")
        tmp = 2;
    else if (Model == "MUTX")
        tmp = 5; // number of properties for the default MUTX (2 inductors)
    else tmp = counts + 1;    // "+1" because "counts" could be zero

    for (int k=1; tmp <= (int) counts / 2; tmp++, k++){
      Props.append(new Property("p" + QString::number(k), "", true, " "));
    }


    // load all properties
    unsigned int z = 0;
    for (auto p1 = Props.begin(); p1 != Props.end(); ++p1) {
        z++;
        n = s.section('"', z, z);    // property value
        n.replace("\\n", "\n");
        n.replace("''", "\"");
        z++;
        //qDebug() << "LOAD: " << p1->Description;

        // not all properties have to be mentioned (backward compatible)
        if (z > counts) {
            if ((*p1)->Description.isEmpty()){
              Props.erase(p1++);   // remove if allocated in vain
            }

            if (Model == "Diode") {
                if (counts < 56) {  // backward compatible
                    counts >>= 1;
                    p1 = Props.begin();
                    std::advance(p1,std::min<int>(counts-1, std::distance(p1, Props.end())));
                    for (; p1 != Props.begin();) {
                        if (counts-- < 19){break;}

                        auto p1prev = p1;
                        --p1prev;
                        n = (*p1prev)->Value;
                        (*p1)->Value = n;
                        --p1;
                    }

                    Props.at(17)->Value = Props.at(11)->Value;
                    (*p1)->Value = "0";
                }
            } else if (Model == "AND" || Model == "NAND" || Model == "NOR" ||
                       Model == "OR" || Model == "XNOR" || Model == "XOR") {
                if (counts < 10) {   // backward compatible
                    counts >>= 1;
                    p1 = Props.begin();
                    std::advance(p1,std::min<int>(counts, std::distance(p1, Props.end())));
                    for (; p1 != Props.begin();) {
                      if (counts-- < 4){break;}
                        auto p1prev = p1;
                        --p1prev;
                        n = (*p1prev)->Value;
                        (*p1)->Value = n;
                        --p1;
                    }
                    (*p1)->Value = "10";
                }
            } else if (Model == "Buf" || Model == "Inv") {
                if (counts < 8) {   // backward compatible
                    counts >>= 1;
                    p1 = Props.begin();
                    std::advance(p1,std::min<int>(counts, std::distance(p1, Props.end())));
                    for(; p1 != Props.begin(); ) {
                        if (counts-- < 3) {break;}

                        auto p1prev = p1;
                        --p1prev;
                        n = (*p1prev)->Value;
                        (*p1)->Value = n;
                        --p1;
                    }
                    (*p1)->Value = "10";
                  }
            }
            return true;
        }

        // for equations
        if (Model != "EDD" && Model != "RFEDD" && Model != "RFEDD2P"){
          if ((*p1)->Description.isEmpty() || (*p1)->Description == "Expression") {  // unknown number of properties ?
            (*p1)->Name = n.section('=', 0, 0);
            n = n.section('=', 1);
            // allocate memory for a new property (e.g. for equations)
            if (static_cast<unsigned int>(Props.size()) < (counts >> 1)) {
              int index = std::distance(Props.begin(), p1);
              Props.insert(index + 1, new Property("y", "1", true));
              p1 = Props.begin() + index;
            }
          }
        }


        if (z == 6 && counts == 6 && Model == "R"){ // backward compatible
            Props.back()->Value = n;
            return true;
        }
        (*p1)->Value = n;

        n = s.section('"', z, z);    // display
        (*p1)->display = (n.at(1) == '1');
    }

    return true;
}

// *******************************************************************
// ***  The following functions are used to load the schematic symbol
// ***  from file. (e.g. subcircuit, library component)

int Component::analyseLine(const QString &Row, int numProps) {
    QPen Pen;
    QBrush Brush;
    QColor Color;
    QString s;
    int i1, i2, i3, i4, i5, i6;

    s = Row.section(' ', 0, 0);    // component type
    if ((s == "PortSym") || (s == ".PortSym")) {  // backward compatible
        if (!getIntegers(Row, &i1, &i2, &i3))
            return -1;
        for (i6 = Ports.count(); i6 < i3; i6++)  // if ports not in numerical order
            Ports.append(new Port(0, 0, false));

        Port *po = Ports.at(i3 - 1);
        po->x = i1;
        po->y = i2;
        po->avail = true;

        if (i1 < x1) x1 = i1;  // keep track of component boundings
        if (i1 > x2) x2 = i1;
        if (i2 < y1) y1 = i2;
        if (i2 > y2) y2 = i2;
        return 0;   // do not count Ports
    } else if (s == "Line") {
        if (!getIntegers(Row, &i1, &i2, &i3, &i4)) return -1;
        if (!getPen(Row, Pen, 5)) return -1;
        i3 += i1;
        i4 += i2;
        Lines.append(new qucs::Line(i1, i2, i3, i4, Pen));

        if (i1 < x1) x1 = i1;  // keep track of component boundings
        if (i1 > x2) x2 = i1;
        if (i2 < y1) y1 = i2;
        if (i2 > y2) y2 = i2;
        if (i3 < x1) x1 = i3;
        if (i3 > x2) x2 = i3;
        if (i4 < y1) y1 = i4;
        if (i4 > y2) y2 = i4;
        return 1;
    } else if (s == "EArc") {
        if (!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))
            return -1;
        if (!getPen(Row, Pen, 7)) return -1;
        Arcs.append(new struct qucs::Arc(i1, i2, i3, i4, i5, i6, Pen));

        if (i1 < x1) x1 = i1;  // keep track of component boundings
        if (i1 + i3 > x2) x2 = i1 + i3;
        if (i2 < y1) y1 = i2;
        if (i2 + i4 > y2) y2 = i2 + i4;
        return 1;
    } else if (s == ".ID") {
        if (!getIntegers(Row, &i1, &i2)) return -1;
        tx = i1;
        ty = i2;
        Name = Row.section(' ', 3, 3);
        if (Name.isEmpty()) Name = "SUB";

        i1 = 1;
        auto pp = Props.begin();
        std::advance(pp,std::min<int>( (numProps-1), std::distance(pp, Props.end())));
        for (;;) {
            s = Row.section('"', i1, i1);
            if (s.isEmpty()) break;

            pp++;
            if (pp == Props.end()) {
                Props.append(new Property());
                pp = --Props.end();
                (*pp)->display = (s.at(0) == '1');
                (*pp)->Value = s.section('=', 2, 2);
            }

            (*pp)->Name = s.section('=', 1, 1);
            (*pp)->Description = s.section('=', 3, 3);
            if ((*pp)->Description.isEmpty())
                (*pp)->Description = " ";

            i1 += 2;
        }

        if(pp != Props.end()-1){
            Props.erase(pp, Props.end());
        }
        return 0;   // do not count IDs
    } else if (s == "Arrow") {
        if (!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6)) return -1;
        if (!getPen(Row, Pen, 7)) return -1;

        double beta = atan2(double(i6), double(i5));
        double phi = atan2(double(i4), double(i3));
        double Length = sqrt(double(i6 * i6 + i5 * i5));

        i3 += i1;
        i4 += i2;
        if (i1 < x1) x1 = i1;  // keep track of component boundings
        if (i1 > x2) x2 = i1;
        if (i3 < x1) x1 = i3;
        if (i3 > x2) x2 = i3;
        if (i2 < y1) y1 = i2;
        if (i2 > y2) y2 = i2;
        if (i4 < y1) y1 = i4;
        if (i4 > y2) y2 = i4;

        Lines.append(new qucs::Line(i1, i2, i3, i4, Pen));   // base line

        double w = beta + phi;
        i5 = i3 - int(Length * cos(w));
        i6 = i4 - int(Length * sin(w));
        Lines.append(new qucs::Line(i3, i4, i5, i6, Pen)); // arrow head
        if (i5 < x1) x1 = i5;  // keep track of component boundings
        if (i5 > x2) x2 = i5;
        if (i6 < y1) y1 = i6;
        if (i6 > y2) y2 = i6;

        w = phi - beta;
        i5 = i3 - int(Length * cos(w));
        i6 = i4 - int(Length * sin(w));
        Lines.append(new qucs::Line(i3, i4, i5, i6, Pen));
        if (i5 < x1) x1 = i5;  // keep track of component boundings
        if (i5 > x2) x2 = i5;
        if (i6 < y1) y1 = i6;
        if (i6 > y2) y2 = i6;

        return 1;
    } else if (s == "Ellipse") {
        if (!getIntegers(Row, &i1, &i2, &i3, &i4)) return -1;
        if (!getPen(Row, Pen, 5)) return -1;
        if (!getBrush(Row, Brush, 8)) return -1;
        Ellipses.append(new qucs::Ellips(i1, i2, i3, i4, Pen, Brush));

        if (i1 < x1) x1 = i1;  // keep track of component boundings
        if (i1 > x2) x2 = i1;
        if (i2 < y1) y1 = i2;
        if (i2 > y2) y2 = i2;
        if (i1 + i3 < x1) x1 = i1 + i3;
        if (i1 + i3 > x2) x2 = i1 + i3;
        if (i2 + i4 < y1) y1 = i2 + i4;
        if (i2 + i4 > y2) y2 = i2 + i4;
        return 1;
    } else if (s == "Rectangle") {
        if (!getIntegers(Row, &i1, &i2, &i3, &i4)) return -1;
        if (!getPen(Row, Pen, 5)) return -1;
        if (!getBrush(Row, Brush, 8)) return -1;
        Rects.append(new qucs::Rect(i1, i2, i3, i4, Pen, Brush));

        if (i1 < x1) x1 = i1;  // keep track of component boundings
        if (i1 > x2) x2 = i1;
        if (i2 < y1) y1 = i2;
        if (i2 > y2) y2 = i2;
        if (i1 + i3 < x1) x1 = i1 + i3;
        if (i1 + i3 > x2) x2 = i1 + i3;
        if (i2 + i4 < y1) y1 = i2 + i4;
        if (i2 + i4 > y2) y2 = i2 + i4;
        return 1;
    } else if (s == "Text") {  // must be last in order to reuse "s" *********
        if (!getIntegers(Row, &i1, &i2, &i3, 0, &i4)) return -1;
        Color=misc::ColorFromString(Row.section(' ', 4, 4));
        if (!Color.isValid()) return -1;

        s = Row.mid(Row.indexOf('"') + 1);    // Text (can contain " !!!)
        s = s.left(s.length() - 1);
        if (s.isEmpty()) return -1;
        misc::convert2Unicode(s);

        QFont Font(QucsSettings.font);
        Font.setPointSizeF(float(i3));

        Texts.append(new Text(i1, i2, s, Color, static_cast<double>(QFontInfo{Font}.pixelSize()),
                              float(cos(float(i4) * pi / 180.0)),
                              float(sin(float(i4) * pi / 180.0))));

        QFontMetrics metrics(Font, 0); // use the screen-compatible metric
        QSize r = metrics.size(0, s);    // get size of text
        i3 = i1 + int(float(r.width()) * Texts.last()->mCos)
             + int(float(r.height()) * Texts.last()->mSin);
        i4 = i2 + int(float(r.width()) * -Texts.last()->mSin)
             + int(float(r.height()) * Texts.last()->mCos);

        if (i1 < x1) x1 = i1;  // keep track of component boundings
        if (i2 < y1) y1 = i2;
        if (i1 > x2) x2 = i1;
        if (i2 > y2) y2 = i2;

        if (i3 < x1) x1 = i3;
        if (i4 < y1) y1 = i4;
        if (i3 > x2) x2 = i3;
        if (i4 > y2) y2 = i4;
        return 1;
    }

    return 0;
}

// ---------------------------------------------------------------------
bool Component::getIntegers(const QString &s, int *i1, int *i2, int *i3,
                            int *i4, int *i5, int *i6) {
    bool ok;
    QString n;

    if (!i1) return true;
    n = s.section(' ', 1, 1);
    *i1 = n.toInt(&ok);
    if (!ok) return false;

    if (!i2) return true;
    n = s.section(' ', 2, 2);
    *i2 = n.toInt(&ok);
    if (!ok) return false;

    if (!i3) return true;
    n = s.section(' ', 3, 3);
    *i3 = n.toInt(&ok);
    if (!ok) return false;

    if (i4) {
        n = s.section(' ', 4, 4);
        *i4 = n.toInt(&ok);
        if (!ok) return false;
    }

    if (!i5) return true;
    n = s.section(' ', 5, 5);
    *i5 = n.toInt(&ok);
    if (!ok) return false;

    if (!i6) return true;
    n = s.section(' ', 6, 6);
    *i6 = n.toInt(&ok);
    if (!ok) return false;

    return true;
}

// ---------------------------------------------------------------------
bool Component::getPen(const QString &s, QPen &Pen, int i) {
    bool ok;
    QString n;

    n = s.section(' ', i, i);    // color
    QColor co = misc::ColorFromString(n);
    Pen.setColor(co);
    if (!Pen.color().isValid()) return false;

    i++;
    n = s.section(' ', i, i);    // thickness
    Pen.setWidth(n.toInt(&ok));
    if (!ok) return false;

    i++;
    n = s.section(' ', i, i);    // line style
    Pen.setStyle((Qt::PenStyle) n.toInt(&ok));
    if (!ok) return false;

    return true;
}

// ---------------------------------------------------------------------
bool Component::getBrush(const QString &s, QBrush &Brush, int i) {
    bool ok;
    QString n;

    n = s.section(' ', i, i);    // fill color
    QColor co = misc::ColorFromString(n);
    Brush.setColor(co);
    if (!Brush.color().isValid()) return false;

    i++;
    n = s.section(' ', i, i);    // fill style
    Brush.setStyle((Qt::BrushStyle) n.toInt(&ok));
    if (!ok) return false;

    i++;
    n = s.section(' ', i, i);    // filled
    if (n.toInt(&ok) == 0) Brush.setStyle(Qt::NoBrush);
    if (!ok) return false;

    return true;
}

// ---------------------------------------------------------------------
Property *Component::getProperty(const QString &name) {      
    for(auto pp = Props.begin(); pp != Props.end(); ++pp) {
      if((*pp)->Name == name) {
        return *pp;
      }
    }

    return nullptr;
}

// ---------------------------------------------------------------------
void Component::copyComponent(Component *pc) {
    Type = pc->Type;
    x1 = pc->x1;
    y1 = pc->y1;
    x2 = pc->x2;
    y2 = pc->y2;

    Model = pc->Model;
    Name = pc->Name;
    showName = pc->showName;
    Description = pc->Description;

    isActive = pc->isActive;
    rotated = pc->rotated;
    mirroredX = pc->mirroredX;
    tx = pc->tx;
    ty = pc->ty;

    Props = pc->Props;
    Ports = pc->Ports;
    Lines = pc->Lines;
    Arcs = pc->Arcs;
    Rects = pc->Rects;
    Ellipses = pc->Ellipses;
    Texts = pc->Texts;
}


// ***********************************************************************
// ********                                                       ********
// ********          Functions of class MultiViewComponent        ********
// ********                                                       ********
// ***********************************************************************
void MultiViewComponent::recreate(Schematic *Doc) {
    if (Doc) {
        Doc->a_Components->setAutoDelete(false);
        Doc->deleteComp(this);
    }

    Ellipses.clear();
    Texts.clear();
    Ports.clear();
    Lines.clear();
    Rects.clear();
    Arcs.clear();
    createSymbol();

    bool mmir = mirroredX;
    int rrot = rotated;
    if (mmir && rrot == 2) // mirrorX and rotate 180 = mirrorY
        mirrorY();
    else {
        if (mmir)
            mirrorX();   // mirror
        if (rrot)
            for (int z = 0; z < rrot; z++) rotate(); // rotate
    }

    rotated = rrot;   // restore properties (were changed by rotate/mirror)
    mirroredX = mmir;

    if (Doc) {
        Doc->insertRawComponent(this);
        Doc->a_Components->setAutoDelete(true);
    }
}


// ***********************************************************************
// ********                                                       ********
// ********            Functions of class GateComponent           ********
// ********                                                       ********
// ***********************************************************************
GateComponent::GateComponent() {
    Type = isComponent;   // both analog and digital
    Name = "Y";

    // the list order must be preserved !!!
    Props.append(new Property("in", "2", false,
                              QObject::tr("number of input ports")));
    Props.append(new Property("V", "1 V", false,
                              QObject::tr("voltage of high level")));
    Props.append(new Property("t", "1 ns", false,
                              QObject::tr("delay time")));
    Props.append(new Property("TR", "10", false,
                              QObject::tr("transfer function scaling factor")));

    // this must be the last property in the list !!!
    Props.append(new Property("Symbol", "old", false,
                              QObject::tr("schematic symbol") + " [old, DIN40900]"));
}

// -------------------------------------------------------
QString GateComponent::netlist() {
    QString s = Model + ":" + Name;

    // output all node names
    for (Port *pp: Ports)
        s += " " + pp->Connection->Name;   // node names

    // output all properties
    s += " " + Props.at(1)->Name + "=\"" + Props.at(1)->Value + "\"";
    s += " " + Props.at(2)->Name + "=\"" + Props.at(2)->Value + "\"";
    s += " " + Props.at(3)->Name + "=\"" + Props.at(3)->Value + "\"\n";
    return s;
}

QString GateComponent::spice_netlist(spicecompat::SpiceDialect dialect) {
    if (dialect == spicecompat::SPICEXyce)
    {
        return QString();
    }

    QString s = SpiceModel + Name;
    QString tmp_model = "model_" + Name;
    QString type = "d_" + Model;
    type = type.toLower();
    QString td = spicecompat::normalize_value(getProperty("t")->Value);
    s += " [";
    for (int i = Ports.count(); i >= 2; i--) {
        s += " " + Ports.at(i - 1)->Connection->Name;
    }
    s += "] " + Ports.at(0)->Connection->Name;
    s += " " + tmp_model + "\n";
    s += QStringLiteral(".model %1 %2(rise_delay=%3 fall_delay=%3 input_load=5e-13)\n")
            .arg(tmp_model).arg(type).arg(td);
    return s;
}

// -------------------------------------------------------
QString GateComponent::vhdlCode(int NumPorts) {
    QListIterator<Port *> iport(Ports);
    Port *pp = iport.next();
    QString s = "  " + pp->Connection->Name + " <= ";  // output port

    // xnor NOT defined for std_logic, so here use not and xor
    if (Model == "XNOR") {
        QString Op = " xor ";

        // first input port
        pp = iport.next();
        QString rhs = pp->Connection->Name;

        // output all input ports with node names
        while (iport.hasNext()) {
            pp = iport.next();
            rhs = "not ((" + rhs + ")" + Op + pp->Connection->Name + ")";
        }
        s += rhs;
    } else {
        QString Op = ' ' + Model.toLower() + ' ';
        if (Model.at(0) == 'N') {
            s += "not (";    // nor, nand is NOT assoziative !!! but xnor is !!!
            Op = Op.remove(1, 1);
        }

        pp = iport.next();
        s += pp->Connection->Name;   // first input port

        // output all input ports with node names
        while (iport.hasNext()) {
            pp = iport.next();
            s += Op + pp->Connection->Name;
        }
        if (Model.at(0) == 'N')
            s += ')';
    }

    if (NumPorts <= 0) { // no truth table simulation ?
        QString td = Props.at(2)->Value;        // delay time
        if (!misc::VHDL_Delay(td, Name)) return td;
        s += td;
    }

    s += ";\n";
    return s;
}

// -------------------------------------------------------
QString GateComponent::verilogCode(int NumPorts) {
    bool synthesize = true;
    QListIterator<Port *> iport(Ports);
    Port *pp = iport.next();
    QString s("");

    if (synthesize) {
        QString op = Model.toLower();
        if (op == "and" || op == "nand")
            op = "&";
        else if (op == "or" || op == "nor")
            op = "|";
        else if (op == "xor")
            op = "^";
        else if (op == "xnor")
            op = "^~";

        s = "  assign";

        if (NumPorts <= 0) { // no truth table simulation ?
            QString td = Props.at(2)->Value;        // delay time
            if (!misc::Verilog_Delay(td, Name)) return td;
            s += td;
        }
        s += " " + pp->Connection->Name + " = ";  // output port
        if (Model.at(0) == 'N') s += "~(";

        pp = iport.next();
        s += pp->Connection->Name;   // first input port

        // output all input ports with node names
        while (iport.hasNext()) {
            pp = iport.next();
            s += " " + op + " " + pp->Connection->Name;
        }

        if (Model.at(0) == 'N') s += ")";
        s += ";\n";
    } else {
        s = "  " + Model.toLower();

        if (NumPorts <= 0) { // no truth table simulation ?
            QString td = Props.at(2)->Value;        // delay time
            if (!misc::Verilog_Delay(td, Name)) return td;
            s += td;
        }
        s += " " + Name + " (" + pp->Connection->Name;  // output port

        pp = iport.next();
        s += ", " + pp->Connection->Name;   // first input port

        // output all input ports with node names
        while (iport.hasNext()) {
            pp = iport.next();
            s += ", " + pp->Connection->Name;
        }

        s += ");\n";
    }
    return s;
}

// -------------------------------------------------------
void GateComponent::createSymbol() {
    int Num = Props.front()->Value.toInt();
    if (Num < 2) Num = 2;
    else if (Num > 8) Num = 8;
    Props.front()->Value = QString::number(Num);

    int xl, xr, y = 10 * Num, z;
    x1 = -30;
    y1 = -y - 3;
    x2 = 30;
    y2 = y + 3;

    tx = x1 + 4;
    ty = y2 + 4;

    z = 0;
    if (Model.at(0) == 'N') z = 1;

    if (Props.back()->Value.at(0) == 'D') {  // DIN symbol
        xl = -15;
        xr = 15;
        Lines.append(new qucs::Line(15, -y, 15, y, QPen(Qt::darkBlue, 2)));
        Lines.append(new qucs::Line(-15, -y, 15, -y, QPen(Qt::darkBlue, 2)));
        Lines.append(new qucs::Line(-15, y, 15, y, QPen(Qt::darkBlue, 2)));
        Lines.append(new qucs::Line(-15, -y, -15, y, QPen(Qt::darkBlue, 2)));
        Lines.append(new qucs::Line(15, 0, 30, 0, QPen(Qt::darkBlue, 2)));

        if (Model.at(z) == 'O') {
            Lines.append(new qucs::Line(-11, 6 - y, -6, 9 - y, QPen(Qt::darkBlue, 0)));
            Lines.append(new qucs::Line(-11, 12 - y, -6, 9 - y, QPen(Qt::darkBlue, 0)));
            Lines.append(new qucs::Line(-11, 14 - y, -6, 14 - y, QPen(Qt::darkBlue, 0)));
            Lines.append(new qucs::Line(-11, 16 - y, -6, 16 - y, QPen(Qt::darkBlue, 0)));
            Texts.append(new Text(-4, 3 - y, "1", Qt::darkBlue, 15.0));
        } else if (Model.at(z) == 'A')
            Texts.append(new Text(-10, 3 - y, "&", Qt::darkBlue, 15.0));
        else if (Model.at(0) == 'X') {
            if (Model.at(1) == 'N') {
                Ellipses.append(new qucs::Ellips(xr, -4, 8, 8,
                                             QPen(Qt::darkBlue, 0), QBrush(Qt::darkBlue)));
                Texts.append(new Text(-11, 3 - y, "=1", Qt::darkBlue, 15.0));
            } else
                Texts.append(new Text(-11, 3 - y, "=1", Qt::darkBlue, 15.0));
        }
    } else {   // old symbol

        if (Model.at(z) == 'O') xl = 10;
        else xl = -10;
        xr = 10;
        Lines.append(new qucs::Line(-10, -y, -10, y, QPen(Qt::darkBlue, 2)));
        Lines.append(new qucs::Line(10, 0, 30, 0, QPen(Qt::darkBlue, 2)));
        Arcs.append(new qucs::Arc(-30, -y, 40, 30, 0, 16 * 90, QPen(Qt::darkBlue, 2)));
        Arcs.append(new qucs::Arc(-30, y - 30, 40, 30, 0, -16 * 90, QPen(Qt::darkBlue, 2)));
        Lines.append(new qucs::Line(10, 15 - y, 10, y - 15, QPen(Qt::darkBlue, 2)));

        if (Model.at(0) == 'X') {
            Lines.append(new qucs::Line(-5, 0, 5, 0, QPen(Qt::darkBlue, 1)));
            if (Model.at(1) == 'N') {
                Lines.append(new qucs::Line(-5, -3, 5, -3, QPen(Qt::darkBlue, 1)));
                Lines.append(new qucs::Line(-5, 3, 5, 3, QPen(Qt::darkBlue, 1)));
            } else {
                Arcs.append(new qucs::Arc(-5, -5, 10, 10, 0, 16 * 360, QPen(Qt::darkBlue, 1)));
                Lines.append(new qucs::Line(0, -5, 0, 5, QPen(Qt::darkBlue, 1)));
            }
        }
    }

    if (Model.at(0) == 'N')
        Ellipses.append(new qucs::Ellips(xr, -4, 8, 8,
                                     QPen(Qt::darkBlue, 0), QBrush(Qt::darkBlue)));

    Ports.append(new Port(30, 0));
    y += 10;
    for (z = 0; z < Num; z++) {
        y -= 20;
        Ports.append(new Port(-30, y));
        if (xl == 10)
            if ((z == 0) || (z == Num - 1)) {
                Lines.append(new qucs::Line(-30, y, 8, y, QPen(Qt::darkBlue, 2)));
                continue;
            }
        Lines.append(new qucs::Line(-30, y, xl, y, QPen(Qt::darkBlue, 2)));
    }
}


// ***********************************************************************
// ********                                                       ********
// ******** The following function does not belong to any class.  ********
// ******** It creates a component by getting the identification  ********
// ******** string used in the schematic file and for copy/paste. ********
// ********                                                       ********
// ***********************************************************************

Component *getComponentFromName(QString &Line, Schematic *p) {
    Component *c = 0;

    Line = Line.trimmed();
    if (Line.at(0) != '<') {
        QMessageBox::critical(0, QObject::tr("Error"),
                              QObject::tr("Format Error:\nWrong line start!"));
        return 0;
    }

    QString cstr = Line.section(' ', 0, 0); // component type
    cstr.remove(0, 1);    // remove leading "<"
    if (cstr == "Lib") c = new LibComp();
    else if (cstr == "Eqn") c = new Equation();
    else if (cstr == "SPICE") c = new SpiceFile();
    else if (cstr.left(6) == "SPfile" && cstr != "SPfile") {
        // backward compatible
        c = new SParamFile();
        c->Props.back()->Value = cstr.mid(6);
    } else{
        // First look for the component in the QMap library. If the component is not there
        // then must be a hardcoded component (i.e. simulation block, etc.)
      ComponentInfo* component_data = findComponentByModel(cstr);

      if (component_data) {
        // Component found in QMap library (static component)
        c = new Component(*component_data);
      } else {
        // Dynamic component
        c = Module::getComponent(cstr);
      }
    }

    if (!c) {
        /// \todo enable user to load partial schematic, skip unknown components
        if (QucsMain != nullptr) {
            QMessageBox *msg = new QMessageBox(QMessageBox::Warning, QObject::tr("Warning"),
                                               QObject::tr("Format Error:\nUnknown component!\n"
                                                           "%1\n\n"
                                                           "Do you want to load schematic anyway?\n"
                                                           "Unknown components will be replaced \n"
                                                           "by dummy subcircuit placeholders.").arg(cstr),
                                               QMessageBox::Yes | QMessageBox::No);
            int r = msg->exec();
            delete msg;
            if (r == QMessageBox::Yes) {
                c = new Subcircuit();
                // Hack: insert dummy File property before the first property
                int pos1 = Line.indexOf('"');
                QString filestr = QStringLiteral("\"%1.sch\" 1 ").arg(cstr);
                Line.insert(pos1, filestr);
            } else return 0;
        } else {
            QString err_msg = QStringLiteral("Schematic loading error! Unknown device %1").arg(cstr);
            qCritical() << err_msg;
            return 0;
        }

    }

    if (!c->load(Line)) {
        QMessageBox::critical(0, QObject::tr("Error"),
                              QObject::tr("Format Error:\nWrong 'component' line format!"));
        delete c;
        return 0;
    }

    cstr = c->Name;   // is perhaps changed in "recreate" (e.g. subcircuit)
    int x = c->tx, y = c->ty;
    c->setSchematic(p);
    c->recreate(0);
    c->Name = cstr;
    c->tx = x;
    c->ty = y;
    return c;
}


// Used to "shape" this generic component according to the component the user wants to place.
// The parameters and the symbol come in the "ComponentInfo" structure. This info is loaded
// from a XML file at the beginning of the program.
void Component::loadfromComponentInfo(ComponentInfo C)
{
  Name = C.name;
  Model = C.Model;
  ComponentName = Name;
  Description = C.description;
  Category = C.Category;
  showName = C.ShowNameinSchematic;
  XML_Defined = true;

  // Clear symbol information and properties
  Lines.clear();
  Polylines.clear();
  Arcs.clear();
  Rects.clear();
  Ellipses.clear();
  Ports.clear();
  Texts.clear();
  Props.clear();


  // Iterate over all the parameters defined in the "ComponentInfo" object and add them to the "Component" list of properties
  for (auto it = C.parameters.constBegin(); it != C.parameters.constEnd(); ++it) {
    QString parameter_name = it.key();
    ParameterInfo parameter = it.value();

    QString value;
    if (parameter.IsExpression) {
      value = parameter.DefaultValue.toString(); // It will be parsed later before displaying
    } else {
      value = QString("%1").arg(parameter.DefaultValue.toDouble());
    }

    // Append to the component's QList of properties
    Props.append(new Property(parameter_name, value, parameter.Unit, parameter.Show, parameter.Description, parameter.IsExpression));
  }

  // Now pick the symbol information from the "ComponentInfo" object and load it into the "Component" class properties
  // By default, take the first symbol
  QMap<QString, SymbolDescription>::const_iterator symbol_it = C.symbol.constBegin();
  SymbolDescription SymbolInfo = symbol_it.value();
  loadSymbol(SymbolInfo, Ports, Lines, Arcs, Polylines);

  // Set component bounding box. This is required to have the greyed zone when the user clicks on the component
  QVector<int> SymbolBoundingBox = C.SymbolBoundingBox["Standard"];
  x1 = SymbolBoundingBox[0]; // Minimum x
  x2 = SymbolBoundingBox[1]; // Maximum x
  y1 = SymbolBoundingBox[2]; // Minimum y
  y2 = SymbolBoundingBox[3]; // Maximum y

  // Load models
  Netlists = C.Netlists;
}

QString Component::getValue(const Property* p) {
  if (p->IsExpression) {
    // The value is an expression. It must be evaluated by muparser
    return QString::number(evaluateExpression(p->Value));
  } else {
    // The value is a number stored as a string
    bool isNumber;
    p->Value.toDouble(&isNumber);
    if (isNumber) {
      // The string is a number. Check if the property has units and attach them, if they're not empty
      if (!p->Unit.isEmpty()){
        return p->Value + p->Unit; // Attach the units, as they're not empty
      } else{
        return p->Value; // The property is unitless
      }
    } else {
      // The string is not a number. It contains the units already. Don't attach them twice...
      return p->Value;
    }

  }
}



double Component::evaluateExpression(QString expression) {
  mu::Parser parser;
  QMap<QString, double> variables;

  // Populate variables from Props
  for (const Property* prop : Props) {
    if (!prop->IsExpression) {
      bool ok;
      double value = prop->Value.toDouble(&ok);
      if (ok) {
        variables[prop->Name] = value;
        parser.DefineVar(prop->Name.toStdString(), &variables[prop->Name]);
      }
    }
  }

  // Evaluate expressions in Props and add them to variables
  bool changed;
  do {
    changed = false;
    for (const Property* prop : Props) {
      if (prop->IsExpression && !variables.contains(prop->Name)) {
        try {
          parser.SetExpr(prop->Value.toStdString());
          double result = parser.Eval();
          variables[prop->Name] = result;
          parser.DefineVar(prop->Name.toStdString(), &variables[prop->Name]);
          changed = true;
        } catch (mu::Parser::exception_type &e) {
          // Skip this expression if it can't be evaluated yet
        }
      }
    }
  } while (changed);

         // Set the final expression to evaluate
  parser.SetExpr(expression.toStdString());

  try {
    return parser.Eval();
  } catch (mu::Parser::exception_type &e) {
    qDebug() << "Error evaluating expression:" << e.GetMsg().c_str();
    return 0.0;
  }
}


// This function is needed to turn the symbol description structure into actual Qucs-S geometrical objects.
void Component::loadSymbol(SymbolDescription SymbolInfo, QList<Port *>& Ports, QList<qucs::Line *>&Lines, QList<struct qucs::Arc *>&Arcs, QList<qucs::Polyline *>&Polylines)
{
  // Populate Ports
  for (const PortInfo& portInfo : SymbolInfo.Ports) {
    Port* newPort = new Port(portInfo.x, portInfo.y);
    Ports.append(newPort);
  }

  // Populate Lines
  for (const LineInfo& lineInfo : SymbolInfo.Lines) {
    qucs::Line* newLine = new qucs::Line(lineInfo.x1, lineInfo.y1, lineInfo.x2, lineInfo.y2, lineInfo.Pen);
    Lines.append(newLine);
  }

  // Populate Arcs
  for (const ArcInfo& arcInfo : SymbolInfo.Arcs) {
    struct qucs::Arc * newArc = new struct qucs::Arc(arcInfo.x, arcInfo.y, arcInfo.width, arcInfo.height, arcInfo.angle, arcInfo.arclen, arcInfo.Pen);
    Arcs.append(newArc);
  }

  // Populate PolyLines
  for (const PolylineInfo& polyInfo : SymbolInfo.Polylines) {
    // Convert QList<QPointF> to std::vector<QPointF>
    std::vector<QPointF> points(polyInfo.Points.begin(), polyInfo.Points.end());

    qucs::Polyline* newPolyLine = new qucs::Polyline(
        points,
        polyInfo.Pen,
        polyInfo.Brush
        );
    Polylines.append(newPolyLine);
  }
}


// This function is used for getting a component from the QMap library
ComponentInfo* findComponentByModel(const QString& schematic_id)
{
  // First check if the component includes a semicolon. In that case, the format is the following: Category:Model.
  if (schematic_id.contains(":")) {
    QStringList parts = schematic_id.split(":");
    QString Category = parts.first();
    Category.replace("{_}", " "); // The library name in the schematic file cannot contain a space, so an underscore is placed instead
    QString Name = parts.at(1);
    Name.replace("{_}", " ");
    ComponentInfo c = LibraryComponents[Category][Name];
    return &LibraryComponents[Category][Name];
  } else {
    // Iterate over the categories to find the Model
    // This should guarantee compatibility with previous Qucs-S file formats
    for (auto& category : LibraryComponents) {
      for (auto& component : category) {
        if (component.Model == schematic_id) {
          return &component;
        }
      }
    }
  }
  return nullptr;
}
