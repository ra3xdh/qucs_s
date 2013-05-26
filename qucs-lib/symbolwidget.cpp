/***************************************************************************
                               symbolwidget.cpp
                              ------------------
    begin                : Sat May 28 2005
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

#include <math.h>
#include <limits.h>

#include <qpainter.h>
//#include <q3dragobject.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QTextIStream>
#include <QMimeData>
#include <QDrag>
#include <QTextStream>

#include "symbolwidget.h"
#include "qucslib.h"


const char *empty_xpm[] = {  // for drag n'drop
"1 1 1 1", "  c None", " "};


SymbolWidget::SymbolWidget(QWidget *parent) : QWidget(parent)
{

  Text_x = Text_y = 0;
  PaintText = tr("Symbol:");
  QFontMetrics  metrics(QucsSettings.font);
  TextWidth = metrics.width(PaintText) + 4;    // get size of text

  DragNDropText = tr("! Drag n'Drop me !");
  DragNDropWidth = metrics.width(DragNDropText);    // get size of text
  TextHeight = metrics.lineSpacing();
  
  ///setPaletteBackgroundColor(Qt::white);
  QPalette palette;
  palette.setColor(backgroundRole(), Qt::white);
  setPalette(palette);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

SymbolWidget::~SymbolWidget()
{
}

// ************************************************************
QString SymbolWidget::theModel()
{
  // single component line
  if(!ModelString.isEmpty())
    if(ModelString.count('\n') < 2)
      return ModelString.remove('\n');

  // real library component
  return "<Lib " + Prefix + " 1 0 0 " +
         QString::number(Text_x) + " " +
         QString::number(Text_y) + " 0 0 \"" +
         LibraryName + "\" 0 \"" + ComponentName + "\" 0>";
}

// ************************************************************
void SymbolWidget::mouseMoveEvent(QMouseEvent*)
{

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  mimeData->setText("QucsComponent:"+theModel());
  drag->setMimeData(mimeData);
  drag->setPixmap( QPixmap(empty_xpm));
  drag->setHotSpot(QPoint(drag->pixmap().width()/2,drag->pixmap().height()));
  drag->exec();

}

// ************************************************************
void SymbolWidget::paintEvent(QPaintEvent*)
{
  QPainter Painter(this);
  Painter.drawText(2, 2, 0, 0, Qt::AlignLeft | Qt::TextDontClip, PaintText);

  int dx = (x2-x1)/2 + TextWidth - DragNDropWidth/2;
  if(dx < 2)  dx = 2;
  Painter.drawText(dx, y2-y1+2, 0, 0, Qt::AlignLeft | Qt::TextDontClip, DragNDropText);

  // paint all lines
  for(int i=0; i<Lines.size(); i++) {
    Line *pl = Lines.at(i);
    Painter.setPen(pl->style);
    Painter.drawLine(cx+pl->x1, cy+pl->y1, cx+pl->x2, cy+pl->y2);
  }

  // paint all arcs
  for(int i=0; i<Arcs.size(); i++) {
    Arc *pc = Arcs.at(i);
    Painter.setPen(pc->style);
    Painter.drawArc(cx+pc->x, cy+pc->y, pc->w, pc->h, pc->angle, pc->arclen);
  }

  // paint all rectangles
  for(int i=0; i<Rects.size(); i++) {
    Area *pa = Rects.at(i);
    Painter.setPen(pa->Pen);
    Painter.setBrush(pa->Brush);
    Painter.drawRect(cx+pa->x, cy+pa->y, pa->w, pa->h);
  }

  // paint all ellipses
  for(int i=0; i<Ellips.size(); i++) {
    Area *pa = Ellips.at(i);
    Painter.setPen(pa->Pen);
    Painter.setBrush(pa->Brush);
    Painter.drawEllipse(cx+pa->x, cy+pa->y, pa->w, pa->h);
  }

  Painter.setPen(QPen(Qt::black,1));
  QFont Font = Painter.font();   // save current font
  Font.setWeight(QFont::Light);
  // write all text
  for(int i=0; i<Texts.size(); i++) {
    Text *pt = Texts.at(i);
    Font.setPointSizeF(pt->Size);
    Painter.setFont(Font);
    Painter.setPen(pt->Color);
    Painter.drawText(cx+pt->x, cy+pt->y, 0, 0, Qt::TextDontClip, pt->s);
  }
}

// ************************************************************
// Creates a symbol from the model name of a component.
int SymbolWidget::createSymbol(const QString& Lib_, const QString& Comp_)
{
  Arcs.clear();
  Lines.clear();
  Rects.clear();
  Ellips.clear();
  Texts.clear();
  LibraryName = Lib_;
  ComponentName = Comp_;

  int PortNo = 0;
  QString Comp = ModelString.section(' ', 0,0);
  Comp.remove(0, 1);  // remove '<'

  QString FirstProp = ModelString.section('"', 1,1);

  if(Comp == "_BJT") {
    Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

    if(FirstProp == "npn") {
      Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
      Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
    }
    else {
      Lines.append(new Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
      Lines.append(new Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
    }

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, -34, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, 26, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 3;
    x1 = -34; y1 = -34;
    x2 =   4; y2 =  34;
  }
  else if(Comp == "JFET") {
    Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10,-10,  0,-10,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,-10,  0,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10, 10,  0, 10,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0, 10,  0, 30,QPen(Qt::darkBlue,2)));

    Lines.append(new Line( -4, 24,  4, 20,QPen(Qt::darkBlue,2)));

    if(FirstProp == "nfet") {
      Lines.append(new Line(-16, -5,-11,  0,QPen(Qt::darkBlue,2)));
      Lines.append(new Line(-16,  5,-11,  0,QPen(Qt::darkBlue,2)));
    }
    else {
      Lines.append(new Line(-18, 0,-13, -5,QPen(Qt::darkBlue,2)));
      Lines.append(new Line(-18, 0,-13,  5,QPen(Qt::darkBlue,2)));
    }

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, -34, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, 26, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 3;
    x1 = -30; y1 = -30;
    x2 =   4; y2 =  30;
  }
  else if(Comp == "_MOSFET") {
    Lines.append(new Line(-14,-13,-14, 13,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));

    Lines.append(new Line(-10,-11,  0,-11,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,-11,  0,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10, 11,  0, 11,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,  0,  0, 30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10,  0,  0,  0,QPen(Qt::darkBlue,2)));

    Lines.append(new Line(-10,-16,-10, -7,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-10,  7,-10, 16,QPen(Qt::darkBlue,3)));

    if(FirstProp == "nfet") {
      Lines.append(new Line( -9,  0, -4, -5,QPen(Qt::darkBlue,2)));
      Lines.append(new Line( -9,  0, -4,  5,QPen(Qt::darkBlue,2)));
    }
    else {
      Lines.append(new Line( -1,  0, -6, -5,QPen(Qt::darkBlue,2)));
      Lines.append(new Line( -1,  0, -6,  5,QPen(Qt::darkBlue,2)));
    }

    // depletion or enhancement MOSFET ?
    if((ModelString.section('"', 3,3).trimmed().at(0) == '-') ==
       (FirstProp == "nfet"))
      Lines.append(new Line(-10, -8,-10,  8,QPen(Qt::darkBlue,3)));
    else
      Lines.append(new Line(-10, -4,-10,  4,QPen(Qt::darkBlue,3)));

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, -34, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(-4, 26, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 3;
    x1 = -34; y1 = -34;
    x2 =   4; y2 =  34;
  }
  else if(Comp == "Diode") {
    Lines.append(new Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6, -9, -6,  9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  6, -9,  6,  9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6,  0,  6, -9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6,  0,  6,  9,QPen(Qt::darkBlue,2)));

    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));
    Arcs.append(new struct Arc(26, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    PortNo = 2;
    x1 = -34; y1 = -9;
    x2 =  34; y2 =  9;
  }
  else if(Comp == "hicumL2V2p1" || Comp == "hic2_full" ||
	  Comp == "hic0_full" || Comp == "hicumL0V1p2" ||
	  Comp == "hicumL2V2p23" || Comp == "hicumL2V2p24" ||
	  Comp == "hicumL0V1p2g" || Comp == "hicumL0V1p3") {
    // normal bipolar
    Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
    Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

    // substrate node
    Lines.append(new Line(  9,  0, 30,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  9, -7,  9,  7,QPen(Qt::darkBlue,3)));

    // thermal node
    Lines.append(new Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-20, 17,-20, 23,QPen(Qt::darkBlue,2)));  

    // arrow
    if(FirstProp == "npn" || Comp == "hic2_full" || Comp == "hicumL2V2p23" ||
       Comp == "hicumL2V2p1") {
      Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
      Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
    } else {
      Lines.append(new Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
      Lines.append(new Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
    }

    // H
    Lines.append(new Line(-30,-30,-30,-24,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-30,-27,-26,-27,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-26,-30,-26,-24,QPen(Qt::darkBlue,1)));
    // I
    Lines.append(new Line(-24,-30,-24,-24,QPen(Qt::darkBlue,1)));
    // C
    Lines.append(new Line(-22,-30,-22,-24,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-22,-30,-19,-30,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-22,-24,-19,-24,QPen(Qt::darkBlue,1)));
    // U
    Lines.append(new Line(-17,-30,-17,-24,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-14,-30,-14,-24,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-17,-24,-14,-24,QPen(Qt::darkBlue,1)));
    // M
    Lines.append(new Line(-12,-30,-12,-24,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( -8,-30, -8,-24,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-12,-30,-10,-28,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( -8,-30,-10,-28,QPen(Qt::darkBlue,1)));

    // terminal definitions
    Arcs.append(new struct Arc( -4, -34, 8, 8, 0, 16*360,
                               QPen(Qt::red,1))); // collector
    Arcs.append(new struct Arc(-34, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1))); // base
    Arcs.append(new struct Arc( -4, 26, 8, 8, 0, 16*360,
                               QPen(Qt::red,1))); // emitter
    Arcs.append(new struct Arc( 26, -4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1))); // substrate
    Arcs.append(new struct Arc(-34, 16, 8, 8, 0, 16*360,
                               QPen(Qt::red,1))); // thermal node

    // relative boundings
    x1 = -34; y1 = -34;
    x2 =  34; y2 =  34;
  }
  else if(Comp == "SUBST") {
    Lines.append(new Line(-30,-16, 30,-16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30,-12, 30,-12,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30, 16, 30, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30, 12, 30, 12,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30,-16,-30, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30,-16, 30, 16,QPen(Qt::darkBlue,2)));

    Lines.append(new Line(-30,-16, 16,-40,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30,-16, 80,-40,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30,-12, 80,-36,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30, 12, 80,-16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 30, 16, 80,-12,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 16,-40, 80,-40,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 80,-40, 80,-12,QPen(Qt::darkBlue,2)));
  
    Lines.append(new Line(-30,  0,-18,-12,QPen(Qt::darkBlue,1)));
    Lines.append(new Line(-22, 12,  2,-12,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( -2, 12, 22,-12,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 18, 12, 30,  0,QPen(Qt::darkBlue,1)));

    Lines.append(new Line( 30,  1, 37,  8,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 37,-15, 52,  0,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 52,-22, 66, -8,QPen(Qt::darkBlue,1)));
    Lines.append(new Line( 66,-30, 80,-16,QPen(Qt::darkBlue,1)));

    PortNo = 0;
    x1 = -34; y1 =-44;
    x2 =  84; y2 = 20;
  }
  
  x1 -= 4;   // enlarge component boundings a little
  x2 += 4;
  y1 -= 4;
  y2 += 4;
  cx  = -x1 + TextWidth;
  cy  = -y1;

  int dx = x2-x1 + TextWidth;
  if((x2-x1) < DragNDropWidth)
    dx = (x2-x1 + DragNDropWidth)/2 + TextWidth;
  if(dx < DragNDropWidth)
    dx = DragNDropWidth;
  setMinimumSize(dx, y2-y1 + TextHeight+4);
  if(width() > dx)  dx = width();
  resize(dx, y2-y1 + TextHeight+4);
  update();
  return PortNo;
}

// ************************************************************
// Loads the symbol for the component from the symbol field and
// returns the number of painting elements.
int SymbolWidget::setSymbol( QString& SymbolString,
                            const QString& Lib_, const QString& Comp_)
{
  Arcs.clear();
  Lines.clear();
  Rects.clear();
  Ellips.clear();
  Texts.clear();
  LibraryName = Lib_;
  ComponentName = Comp_;

  QString Line;
  ///QString foo = SymbolString;
  QTextStream stream(&SymbolString, QIODevice::ReadOnly);

  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  int z=0, Result;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line.at(0) != '<') return -1;
    if(Line.at(Line.length()-1) != '>') return -1;
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line);
    if(Result < 0) return -6;   // line format error
    z += Result;
  }

  x1 -= 4;   // enlarge component boundings a little
  x2 += 4;
  y1 -= 4;
  y2 += 4;
  cx  = -x1 + TextWidth;
  cy  = -y1;

  int dx = x2-x1 + TextWidth;
  if((x2-x1) < DragNDropWidth)
    dx = (x2-x1 + DragNDropWidth)/2 + TextWidth;
  if(dx < DragNDropWidth)
    dx = DragNDropWidth;
  setMinimumSize(dx, y2-y1 + TextHeight+4);
  if(width() > dx)  dx = width();
  resize(dx, y2-y1 + TextHeight+4);
  update();
  return z;      // return number of ports
}

// ---------------------------------------------------------------------
int SymbolWidget::analyseLine(const QString& Row)
{
  QPen Pen;
  QBrush Brush;
  QColor Color;
  QString s;
  int i1, i2, i3, i4, i5, i6;

  s = Row.section(' ',0,0);    // component type
  if(s == ".PortSym") {  // here: ports are open nodes
    if(!getIntegers(Row, &i1, &i2, &i3))  return -1;
    Arcs.append(new struct Arc(i1-4, i2-4, 8, 8, 0, 16*360,
                               QPen(Qt::red,1)));

    if((i1-4) < x1)  x1 = i1-4;  // keep track of component boundings
    if((i1+4) > x2)  x2 = i1+4;
    if((i2-4) < y1)  y1 = i2-4;
    if((i2+4) > y2)  y2 = i2+4;
    return 0;   // do not count Ports
  }
  else if(s == "Line") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    i3 += i1;
    i4 += i2;
    Lines.append(new Line(i1, i2, i3, i4, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i3 < x1)  x1 = i3;
    if(i3 > x2)  x2 = i3;
    if(i4 < y1)  y1 = i4;
    if(i4 > y2)  y2 = i4;
    return 1;
  }
  else if(s == "EArc") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))  return -1;
    if(!getPen(Row, Pen, 7))  return -1;
    Arcs.append(new struct Arc(i1, i2, i3, i4, i5, i6, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2 < y1)  y1 = i2;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == ".ID") {
    if(!getIntegers(Row, &i1, &i2))  return -1;
    Text_x = i1;
    Text_y = i2;
    Prefix = Row.section(' ',3,3);
    if(Prefix.isEmpty())  Prefix = "X";
    return 0;   // do not count IDs
  }
  else if(s == "Arrow") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))  return -1;
    if(!getPen(Row, Pen, 7))  return -1;

    double beta   = atan2(double(i6), double(i5));
    double phi    = atan2(double(i4), double(i3));
    double Length = sqrt(double(i6*i6 + i5*i5));

    i3 += i1;
    i4 += i2;
    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i3 < x1)  x1 = i3;
    if(i3 > x2)  x2 = i3;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i4 < y1)  y1 = i4;
    if(i4 > y2)  y2 = i4;

    Lines.append(new Line(i1, i2, i3, i4, Pen));   // base line

    double w = beta+phi;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.append(new Line(i3, i4, i5, i6, Pen)); // arrow head
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    w = phi-beta;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.append(new Line(i3, i4, i5, i6, Pen));
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    return 1;
  }
  else if(s == "Ellipse") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Ellips.append(new Area(i1, i2, i3, i4, Pen, Brush));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i1+i3 < x1)  x1 = i1+i3;
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2+i4 < y1)  y1 = i2+i4;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == "Rectangle") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Rects.append(new Area(i1, i2, i3, i4, Pen, Brush));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i1+i3 < x1)  x1 = i1+i3;
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2+i4 < y1)  y1 = i2+i4;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == "Text") {  // must be last in order to reuse "s" *********
    if(!getIntegers(Row, &i1, &i2, &i3))  return -1;
    Color.setNamedColor(Row.section(' ',4,4));
    if(!Color.isValid()) return -1;

    s = Row.mid(Row.indexOf('"')+1);    // Text (can contain " !!!)
    s = s.left(s.length()-1);
    if(s.isEmpty()) return -1;
    s.replace("\\n", "\n");
    s.replace("\\\\", "\\");

    Texts.append(new Text(i1, i2, s, Color, float(i3)));

    QFont Font(QucsSettings.font);
    Font.setPointSizeF(float(i3));
    QFontMetrics  metrics(Font);
    QSize r = metrics.size(0, s);    // get size of text
    i3 = i1 + r.width();
    i4 = i2 + r.height();

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i3 > x2)  x2 = i3;
    if(i2 < y1)  y1 = i2;
    if(i4 > y2)  y2 = i4;
  }

  return 0;
}

// ---------------------------------------------------------------------
bool SymbolWidget::getIntegers(const QString& s, int *i1, int *i2, int *i3,
			     int *i4, int *i5, int *i6)
{
  bool ok;
  QString n;

  if(!i1) return true;
  n  = s.section(' ',1,1);
  *i1 = n.toInt(&ok);
  if(!ok) return false;

  if(!i2) return true;
  n  = s.section(' ',2,2);
  *i2 = n.toInt(&ok);
  if(!ok) return false;

  if(!i3) return true;
  n  = s.section(' ',3,3);
  *i3 = n.toInt(&ok);
  if(!ok) return false;

  if(!i4) return true;
  n  = s.section(' ',4,4);
  *i4 = n.toInt(&ok);
  if(!ok) return false;

  if(!i5) return true;
  n  = s.section(' ',5,5);
  *i5 = n.toInt(&ok);
  if(!ok) return false;

  if(!i6) return true;
  n  = s.section(' ',6,6);
  *i6 = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// ---------------------------------------------------------------------
bool SymbolWidget::getPen(const QString& s, QPen& Pen, int i)
{
  bool ok;
  QString n;

  n = s.section(' ',i,i);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  i++;
  n = s.section(' ',i,i);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  i++;
  n = s.section(' ',i,i);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

// ---------------------------------------------------------------------
bool SymbolWidget::getBrush(const QString& s, QBrush& Brush, int i)
{
  bool ok;
  QString n;

  n = s.section(' ',i,i);    // fill color
  QColor co;
  co.setNamedColor(n);
  Brush.setColor(co);
  if(!Brush.color().isValid()) return false;

  i++;
  n = s.section(' ',i,i);    // fill style
  Brush.setStyle((Qt::BrushStyle)n.toInt(&ok));
  if(!ok) return false;

  i++;
  n = s.section(' ',i,i);    // filled
  if(n.toInt(&ok) == 0) Brush.setStyle(Qt::NoBrush);
  if(!ok) return false;

  return true;
}
