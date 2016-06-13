/***************************************************************************
                              schematic.cpp
                             ---------------
    begin                : Sat Mar 3 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include <stdlib.h>
#include <limits.h>

#include <QFileInfo>
#include <QPrinter>
#include <QPaintDevice>
#include <QDir>
#include <QTextStream>
#include <QDragLeaveEvent>
#include <Q3PtrList>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QAction>
#include <QLineEdit>
#include <QUrl>
#include <QListWidget>
#include <QDebug>
#include <QApplication>
#include <QClipboard>

#include "qucs.h"
#include "schematic.h"
#include "main.h"
#include "node.h"
#include "textdoc.h"
#include "viewpainter.h"
#include "mouseactions.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/vafile.h"

// just dummies for empty lists
Q3PtrList<Wire>      SymbolWires;
Q3PtrList<Node>      SymbolNodes;
Q3PtrList<Diagram>   SymbolDiags;
Q3PtrList<Component> SymbolComps;


Schematic::Schematic(QucsApp *App_, const QString& Name_)
    : QucsDoc(App_, Name_)
{
  symbolMode = false;

  // ...........................................................
  GridX  = GridY  = 10;
  ViewX1=ViewY1=0;
  ViewX2=ViewY2=800;
  UsedX1 = UsedY1 = INT_MAX;
  UsedX2 = UsedY2 = INT_MIN;

  tmpPosX = tmpPosY = -100;
  tmpUsedX1 = tmpUsedY1 = tmpViewX1 = tmpViewY1 = -200;
  tmpUsedX2 = tmpUsedY2 = tmpViewX2 = tmpViewY2 =  200;
  tmpScale = 1.0;

  DocComps.setAutoDelete(true);
  DocWires.setAutoDelete(true);
  DocNodes.setAutoDelete(true);
  DocDiags.setAutoDelete(true);
  DocPaints.setAutoDelete(true);
  SymbolPaints.setAutoDelete(true);

  // The 'i' means state for being unchanged.
  undoActionIdx = 0;
  undoAction.append(new QString(" i\n</>\n</>\n</>\n</>\n"));
  undoSymbolIdx = 0;
  undoSymbol.append(new QString(" i\n</>\n</>\n</>\n</>\n"));

  isVerilog = false;
  creatingLib = false;

  showFrame = 0;  // don't show
  Frame_Text0 = tr("Title");
  Frame_Text1 = tr("Drawn By:");
  Frame_Text2 = tr("Date:");
  Frame_Text3 = tr("Revision:");

  setVScrollBarMode(Q3ScrollView::AlwaysOn);
  setHScrollBarMode(Q3ScrollView::AlwaysOn);
  viewport()->setPaletteBackgroundColor(QucsSettings.BGColor);
  viewport()->setMouseTracking(true);
  viewport()->setAcceptDrops(true);  // enable drag'n drop

  // to repair some strange  scrolling artefacts
  connect(this, SIGNAL(horizontalSliderReleased()),
      viewport(), SLOT(update()));
  connect(this, SIGNAL(verticalSliderReleased()),
      viewport(), SLOT(update()));
  if (App_) {
    connect(this, SIGNAL(signalCursorPosChanged(int, int)), 
        App_, SLOT(printCursorPosition(int, int)));
    connect(this, SIGNAL(horizontalSliderPressed()), 
        App_, SLOT(slotHideEdit()));
    connect(this, SIGNAL(verticalSliderPressed()),
        App_, SLOT(slotHideEdit()));
    connect(this, SIGNAL(signalUndoState(bool)),
        App_, SLOT(slotUpdateUndo(bool)));
    connect(this, SIGNAL(signalRedoState(bool)),
        App_, SLOT(slotUpdateRedo(bool)));
    connect(this, SIGNAL(signalFileChanged(bool)),
        App_, SLOT(slotFileChanged(bool)));
  }
}

Schematic::~Schematic()
{
}

// ---------------------------------------------------
bool Schematic::createSubcircuitSymbol()
{
  // If the number of ports is not equal, remove or add some.
  unsigned int countPort = adjustPortNumbers();

  // If a symbol does not yet exist, create one.
  if(SymbolPaints.count() != countPort)
    return false;

  int h = 30*((countPort-1)/2) + 10;
  SymbolPaints.prepend(new ID_Text(-20, h+4));

  SymbolPaints.append(
     new GraphicLine(-20, -h, 40,  0, QPen(Qt::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine( 20, -h,  0,2*h, QPen(Qt::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine(-20,  h, 40,  0, QPen(Qt::darkBlue,2)));
  SymbolPaints.append(
     new GraphicLine(-20, -h,  0,2*h, QPen(Qt::darkBlue,2)));

  unsigned int i=0, y = 10-h;
  while(i<countPort) {
    i++;
    SymbolPaints.append(
       new GraphicLine(-30, y, 10, 0, QPen(Qt::darkBlue,2)));
    SymbolPaints.at(i)->setCenter(-30,  y);

    if(i == countPort)  break;
    i++;
    SymbolPaints.append(
       new GraphicLine( 20, y, 10, 0, QPen(Qt::darkBlue,2)));
    SymbolPaints.at(i)->setCenter(30,  y);
    y += 60;
  }
  return true;
}

// ---------------------------------------------------
void Schematic::becomeCurrent(bool update)
{
  emit signalCursorPosChanged(0, 0);

  // update appropriate menu entry
  if (symbolMode) {
    if (DocName.right(4) == ".sym") {
      App->symEdit->setText(tr("Edit Text"));
      App->symEdit->setStatusTip(tr("Edits the Text"));
      App->symEdit->setWhatsThis(tr("Edit Text\n\nEdits the text file"));
    }
    else {
      App->symEdit->setText(tr("Edit Schematic"));
      App->symEdit->setStatusTip(tr("Edits the schematic"));
      App->symEdit->setWhatsThis(tr("Edit Schematic\n\nEdits the schematic"));
    }
  }
  else {
    App->symEdit->setText(tr("Edit Circuit Symbol"));
    App->symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
    App->symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  }

  if(symbolMode) {
    Nodes = &SymbolNodes;
    Wires = &SymbolWires;
    Diagrams = &SymbolDiags;
    Paintings = &SymbolPaints;
    Components = &SymbolComps;

    // if no symbol yet exists -> create one
    if(createSubcircuitSymbol()) {
      sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
      setChanged(true, true);
    }

    emit signalUndoState(undoSymbolIdx != 0);
    emit signalRedoState(undoSymbolIdx != undoSymbol.size()-1);
  }
  else {
    Nodes = &DocNodes;
    Wires = &DocWires;
    Diagrams = &DocDiags;
    Paintings = &DocPaints;
    Components = &DocComps;

    emit signalUndoState(undoActionIdx != 0);
    emit signalRedoState(undoActionIdx != undoAction.size()-1);
    if(update)
      reloadGraphs();   // load recent simulation data
  }
}

// ---------------------------------------------------
void Schematic::setName (const QString& Name_)
{
  DocName = Name_;
  QFileInfo Info (DocName);
  QString base = Info.completeBaseName ();
  QString ext = Info.suffix();
  DataSet = base + ".dat";
  Script = base + ".m";
  if (ext != "dpl")
    DataDisplay = base + ".dpl";
  else
    DataDisplay = base + ".sch";
}

// ---------------------------------------------------
// Sets the document to be changed or not to be changed.
void Schematic::setChanged(bool c, bool fillStack, char Op)
{
  if((!DocChanged) && c)
    emit signalFileChanged(true);
  else if(DocChanged && (!c))
    emit signalFileChanged(false);
  DocChanged = c;

  showBias = -1;   // schematic changed => bias points may be invalid

  if(!fillStack)
    return;


  // ................................................
  if(symbolMode) {  // for symbol edit mode
    while(undoSymbol.size() > undoSymbolIdx + 1) {
      delete undoSymbol.last();
      undoSymbol.pop_back();
    }

    undoSymbol.append(new QString(createSymbolUndoString(Op)));
    undoSymbolIdx++;

    emit signalUndoState(true);
    emit signalRedoState(false);

    while(static_cast<unsigned int>(undoSymbol.size()) > QucsSettings.maxUndo) { // "while..." because
      delete undoSymbol.first();
      undoSymbol.pop_front();
      undoSymbolIdx--;
    }
    return;
  }

  // ................................................
  // for schematic edit mode
  while(undoAction.size() > undoActionIdx + 1) {
    delete undoAction.last();
    undoAction.pop_back();
  }

  if(Op == 'm') {   // only one for move marker
    if (undoAction.at(undoActionIdx)->at(0) == Op) {
      delete undoAction.last();
      undoAction.pop_back();
      undoActionIdx--;
    }
  }

  undoAction.append(new QString(createUndoString(Op)));
  undoActionIdx++;

  emit signalUndoState(true);
  emit signalRedoState(false);

  while(static_cast<unsigned int>(undoAction.size()) > QucsSettings.maxUndo) { // "while..." because
    delete undoAction.first(); // "maxUndo" could be decreased meanwhile
    undoAction.pop_front();
    undoActionIdx--;
  }
  return;
}

// -----------------------------------------------------------
bool Schematic::sizeOfFrame(int& xall, int& yall)
{
  // Values exclude border of 1.5cm at each side.
  switch(showFrame) {
    case 1:  xall = 1020; yall =  765; break;  // DIN A5 landscape
    case 2:  xall =  765; yall = 1020; break;  // DIN A5 portrait
    case 3:  xall = 1530; yall = 1020; break;  // DIN A4 landscape
    case 4:  xall = 1020; yall = 1530; break;  // DIN A4 portrait
    case 5:  xall = 2295; yall = 1530; break;  // DIN A3 landscape
    case 6:  xall = 1530; yall = 2295; break;  // DIN A3 portrait
    case 7:  xall = 1414; yall = 1054; break;  // letter landscape
    case 8:  xall = 1054; yall = 1414; break;  // letter portrait
    default:  return false;
  }

  return true;
}

// -----------------------------------------------------------
void Schematic::paintFrame(ViewPainter *p)
{
  // dimensions:  X cm / 2.54 * 144
  int xall, yall;
  if(!sizeOfFrame(xall, yall))
    return;
  p->Painter->setPen(QPen(Qt::darkGray,1));
  //p->Painter->setPen(QPen(Qt::black,0));
  int d = p->LineSpacing + int(4.0 * p->Scale);
  int x1_, y1_, x2_, y2_;
  p->map(xall, yall, x1_, y1_);
  x2_ = int(xall * p->Scale) + 1;
  y2_ = int(yall * p->Scale) + 1;
  p->Painter->drawRect(x1_, y1_, -x2_, -y2_);
  p->Painter->drawRect(x1_-d, y1_-d, 2*d-x2_, 2*d-y2_);

  int z;
  int step = xall / ((xall+127) / 255);
  for(z=step; z<=xall-step; z+=step) {
    p->map(z, 0, x2_, y2_);
    p->Painter->drawLine(x2_, y2_, x2_, y2_+d);
    p->Painter->drawLine(x2_, y1_-d, x2_, y1_);
  }
  char Letter[2] = "1";
  for(z=step/2+5; z<xall; z+=step) {
    p->drawText(Letter, z, 3, 0);
    p->map(z, yall+3, x2_, y2_);
    p->Painter->drawText(x2_, y2_-d, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  step = yall / ((yall+127) / 255);
  for(z=step; z<=yall-step; z+=step) {
    p->map(0, z, x2_, y2_);
    p->Painter->drawLine(x2_, y2_, x2_+d, y2_);
    p->Painter->drawLine(x1_-d, y2_, x1_, y2_);
  }
  Letter[0] = 'A';
  for(z=step/2+5; z<yall; z+=step) {
    p->drawText(Letter, 5, z, 0);
    p->map(xall+5, z, x2_, y2_);
    p->Painter->drawText(x2_-d, y2_, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  // draw text box with text
  p->map(xall-340, yall-3, x1_, y1_);
  p->map(xall-3,   yall-3, x2_, y2_);
  x1_ -= d;  x2_ -= d;
  y1_ -= d;  y2_ -= d;
  d = int(6.0 * p->Scale);
  z = int(200.0 * p->Scale);
  y1_ -= p->LineSpacing + d;
  p->Painter->drawLine(x1_, y1_, x2_, y1_);
  p->Painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, Frame_Text2);
  p->Painter->drawLine(x1_+z, y1_, x1_+z, y1_ + p->LineSpacing+d);
  p->Painter->drawText(x1_+d+z, y1_+(d>>1), 0, 0, Qt::TextDontClip, Frame_Text3);
  y1_ -= p->LineSpacing + d;
  p->Painter->drawLine(x1_, y1_, x2_, y1_);
  p->Painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, Frame_Text1);
  y1_ -= (Frame_Text0.count('\n')+1) * p->LineSpacing + d;
  p->Painter->drawRect(x2_, y2_, x1_-x2_-1, y1_-y2_-1);
  p->Painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, Frame_Text0);
}

// -----------------------------------------------------------
// Is called when the content (schematic or data display) has to be drawn.
void Schematic::drawContents(QPainter *p, int, int, int, int)
{
  ViewPainter Painter;

  Painter.init(p, Scale, -ViewX1, -ViewY1, contentsX(), contentsY());

  paintGrid(&Painter, contentsX(), contentsY(),
            visibleWidth(), visibleHeight());

  if(!symbolMode)
    paintFrame(&Painter);

  for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    pc->paint(&Painter);

  for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next()) {
    pw->paint(&Painter);
    if(pw->Label)
      pw->Label->paint(&Painter);  // separate because of paintSelected
  }

  Node *pn;
  for(pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
    pn->paint(&Painter);
    if(pn->Label)
      pn->Label->paint(&Painter);  // separate because of paintSelected
  }

  // FIXME disable here, issue with select box goes away
  // also, instead of red, line turns blue
  for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    pd->paint(&Painter);

  for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
    pp->paint(&Painter);

  if(showBias > 0) {  // show DC bias points in schematic ?
    int x, y, z;
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
      if(pn->Name.isEmpty()) continue;
      x = pn->cx;
      y = pn->cy + 4;
      z = pn->x1;
      if(z & 1) x -= Painter.Painter->fontMetrics().width(pn->Name);
      if(!(z & 2)) {
        y -= (Painter.LineSpacing>>1) + 4;
        if(z & 1) x -= 4;
        else x += 4;
      }
      if(z & 0x10)
        Painter.Painter->setPen(Qt::darkGreen);  // green for currents
      else
        Painter.Painter->setPen(Qt::blue);   // blue for voltages
      Painter.drawText(pn->Name, x, y);
    }
  }

  /*
   * The following events used to be drawn from mouseactions.cpp, but since Qt4
   * Paint actions can only be called from within the paint event, so they
   * are put into a QList (PostedPaintEvents) and processed here
   */
  for(int i=0;i<PostedPaintEvents.size();i++)
  {
    PostedPaintEvent p = PostedPaintEvents[i];
    QPainter painter2(viewport());

    switch(p.pe)
    {
      case _NotRop:
        if(p.PaintOnViewport)
          painter2.setCompositionMode(QPainter::RasterOp_SourceAndNotDestination);
        else
          Painter.Painter->setCompositionMode(QPainter::RasterOp_SourceAndNotDestination);
        break;
      case _Rect:
        if(p.PaintOnViewport)
          painter2.drawRect(p.x1, p.y1, p.x2, p.y2);
        else
          Painter.drawRect(p.x1, p.y1, p.x2, p.y2);
        break;
      case _Line:
        if(p.PaintOnViewport)
          painter2.drawLine(p.x1, p.y1, p.x2, p.y2);
        else
          Painter.drawLine(p.x1, p.y1, p.x2, p.y2);
        break;
      case _Ellipse:
        if(p.PaintOnViewport)
          painter2.drawEllipse(p.x1, p.y1, p.x2, p.y2);
        else
          Painter.drawEllipse(p.x1, p.y1, p.x2, p.y2);
        break;
      case _Arc:
        if(p.PaintOnViewport)
          painter2.drawArc(p.x1, p.y1, p.x2, p.y2, p.a, p.b);
        else
          Painter.drawArc(p.x1, p.y1, p.x2, p.y2, p.a, p.b);
        break;
      case _DotLine:
        Painter.Painter->setPen(Qt::DotLine);
        break;
      case _Translate:

        painter2.translate(p.x1, p.y1);
        break;
      case _Scale:
        painter2.scale(p.x1,p.y1);
        break;
    }

  }
  PostedPaintEvents.clear();

}

void Schematic::PostPaintEvent (PE pe, int x1, int y1, int x2, int y2, int a, int b, bool PaintOnViewport)
{
  PostedPaintEvent p = {pe, x1,y1,x2,y2,a,b,PaintOnViewport};
  PostedPaintEvents.push_back(p);
  viewport()->update();
  update();
}


// ---------------------------------------------------
void Schematic::contentsMouseMoveEvent(QMouseEvent *Event)
{
  emit signalCursorPosChanged(Event->pos().x(), Event->pos().y());
  if(App->MouseMoveAction)
    (App->view->*(App->MouseMoveAction))(this, Event);
}

// -----------------------------------------------------------
void Schematic::contentsMousePressEvent(QMouseEvent *Event)
{
  App->editText->setHidden(true); // disable text edit of component property
  if(App->MouseReleaseAction == &MouseActions::MReleasePaste)
    return;

  float x = float(Event->pos().x())/Scale + float(ViewX1);
  float y = float(Event->pos().y())/Scale + float(ViewY1);

  if(Event->button() != Qt::LeftButton)
    if(App->MousePressAction != &MouseActions::MPressElement)
      if(App->MousePressAction != &MouseActions::MPressWire2) {
        // show menu on right mouse button
        App->view->rightPressMenu(this, Event, x, y);
        if(App->MouseReleaseAction)
           // Is not called automatically because menu has focus.
          (App->view->*(App->MouseReleaseAction))(this, Event);
        return;
      }

  if(App->MousePressAction)
    (App->view->*(App->MousePressAction))(this, Event, x, y);
}

// -----------------------------------------------------------
void Schematic::contentsMouseReleaseEvent(QMouseEvent *Event)
{
  if(App->MouseReleaseAction)
    (App->view->*(App->MouseReleaseAction))(this, Event);
}

// -----------------------------------------------------------
void Schematic::contentsMouseDoubleClickEvent(QMouseEvent *Event)
{
  if(App->MouseDoubleClickAction)
    (App->view->*(App->MouseDoubleClickAction))(this, Event);
}

// -----------------------------------------------------------
void Schematic::print(QPrinter*, QPainter *Painter, bool printAll, bool fitToPage)
{
  QPaintDevice *pdevice = Painter->device();
  float printerDpiX = (float)pdevice->logicalDpiX();
  float printerDpiY = (float)pdevice->logicalDpiY();
  float printerW = (float)pdevice->width();
  float printerH = (float)pdevice->height();
  QPainter pa(viewport());
  float screenDpiX = (float)pa.device()->logicalDpiX();
  float screenDpiY = (float)pa.device()->logicalDpiY();
  float PrintScale = 0.5;
  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  int marginX = (int)(40 * printerDpiX / screenDpiX);
  int marginY = (int)(40 * printerDpiY / screenDpiY);

  if(fitToPage) {

    float ScaleX = float((printerW - 2*marginX) /
                   float((UsedX2-UsedX1) * printerDpiX)) * screenDpiX;
    float ScaleY = float((printerH - 2*marginY) /
                   float((UsedY2-UsedY1) * printerDpiY)) * screenDpiY;

    if(showFrame){
        int xall, yall;
        sizeOfFrame(xall, yall);
        ScaleX = ((float)(printerW - 2*marginX) /
                       (float)(xall * printerDpiX)) * screenDpiX;
        ScaleY = ((float)(printerH - 2*marginY) /
                       (float)(yall * printerDpiY)) * screenDpiY;
    }

    if(ScaleX > ScaleY)
      PrintScale = ScaleY;
    else
      PrintScale = ScaleX;
  }


  //bool selected;
  ViewPainter p;
  int StartX = UsedX1;
  int StartY = UsedY1;
  if(showFrame) {
    if(UsedX1 > 0)  StartX = 0;
    if(UsedY1 > 0)  StartY = 0;
  }

  float PrintRatio = printerDpiX / screenDpiX;
  QFont oldFont = Painter->font();
  QFont printFont = Painter->font();
#ifdef __MINGW32__
  printFont.setPointSizeF(printFont.pointSizeF()/PrintRatio);
  Painter->setFont(printFont);
#endif
  p.init(Painter, PrintScale * PrintRatio,
         -StartX, -StartY, -marginX, -marginY, PrintScale, PrintRatio);

  if(!symbolMode)
    paintFrame(&p);

  paintSchToViewpainter(&p,printAll,false,screenDpiX,printerDpiX);

  Painter->setFont(oldFont);
}


void Schematic::paintSchToViewpainter(ViewPainter *p, bool printAll, bool toImage, int screenDpiX, int printerDpiX)
{
    bool selected;

    if (printAll) {
        int x2,y2;
        if (sizeOfFrame(x2,y2)) paintFrame(p);
    }

    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
      if(pc->isSelected || printAll) {
        selected = pc->isSelected;
        pc->isSelected = false;
        if (toImage) {
            pc->paint(p);
        } else {
            pc->print(p, (float)screenDpiX / (float)printerDpiX);
        }
        pc->isSelected = selected;
      }

    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next()) {
      if(pw->isSelected || printAll) {
        selected = pw->isSelected;
        pw->isSelected = false;
        pw->paint(p);   // paint all selected wires
        pw->isSelected = selected;
      }
      if(pw->Label)
        if(pw->Label->isSelected || printAll) {
          selected = pw->Label->isSelected;
          pw->Label->isSelected = false;
          pw->Label->paint(p);
          pw->Label->isSelected = selected;
        }
    }

    Element *pe;
    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
      for(pe = pn->Connections.first(); pe != 0; pe = pn->Connections.next())
        if(pe->isSelected || printAll) {
          pn->paint(p); // paint all nodes with selected elements
          break;
        }
      if(pn->Label)
        if(pn->Label->isSelected || printAll) {
          selected = pn->Label->isSelected;
          pn->Label->isSelected = false;
          pn->Label->paint(p);
          pn->Label->isSelected = selected;
        }
    }

    for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
      if(pp->isSelected || printAll) {
        selected = pp->isSelected;
        pp->isSelected = false;
        pp->paint(p);   // paint all selected paintings
        pp->isSelected = selected;
      }

    for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
      if(pd->isSelected || printAll) {
        // if graph or marker is selected, deselect during printing
        foreach(Graph *pg, pd->Graphs) {
      if(pg->isSelected)  pg->Type |= 1;  // remember selection
      pg->isSelected = false;
      foreach(Marker *pm, pg->Markers) {
        if(pm->isSelected)  pm->Type |= 1;  // remember selection
        pm->isSelected = false;
      }
        }

        selected = pd->isSelected;
        pd->isSelected = false;
        pd->paint(p);  // paint all selected diagrams with graphs and markers
        pd->isSelected = selected;

        // revert selection of graphs and markers
        foreach(Graph *pg, pd->Graphs) {
      if(pg->Type & 1)  pg->isSelected = true;
      pg->Type &= -2;
      foreach(Marker *pm, pg->Markers) {
        if(pm->Type & 1)  pm->isSelected = true;
        pm->Type &= -2;
      }
        }
      }

    if(showBias > 0) {  // show DC bias points in schematic ?
      int x, y, z;
      for(Node* pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
        if(pn->Name.isEmpty()) continue;
        x = pn->cx;
        y = pn->cy + 4;
        z = pn->x1;
        if(z & 1) x -= p->Painter->fontMetrics().width(pn->Name);
        if(!(z & 2)) {
          y -= (p->LineSpacing>>1) + 4;
          if(z & 1) x -= 4;
          else x += 4;
        }
        if(z & 0x10)
          p->Painter->setPen(Qt::darkGreen);  // green for currents
        else
          p->Painter->setPen(Qt::blue);   // blue for voltages
        p->drawText(pn->Name, x, y);
      }
    }
}

// -----------------------------------------------------------
float Schematic::zoom(float s)
{
  Scale *= s;
  if(Scale > 10.0) Scale = 10.0f;
  if(Scale < 0.1) Scale = 0.1f;

  // "resizeContents()" performs an immediate repaint. So, set widget
  // to hidden. This causes some flicker, but it is still nicer.
  viewport()->setHidden(true);
//  setHidden(true);
  resizeContents(int(Scale*float(ViewX2 - ViewX1)),
                 int(Scale*float(ViewY2 - ViewY1)));
//  setHidden(false);
  viewport()->setHidden(false);

  viewport()->update();
  App->view->drawn = false;
  return Scale;
}

// -----------------------------------------------------------
float Schematic::zoomBy(float s)
{
  zoom(s);
  s -= 1.0;
  scrollBy( int(s * float(contentsX()+visibleWidth()/2)),
            int(s * float(contentsY()+visibleHeight()/2)) );
  return Scale;
}

// ---------------------------------------------------
void Schematic::showAll()
{
  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(UsedX1 == 0)
    if(UsedX2 == 0)
      if(UsedY1 == 0)
        if(UsedY2 == 0) {
    UsedX1 = UsedY1 = INT_MAX;
    UsedX2 = UsedY2 = INT_MIN;
    return;
  }

  float xScale = float(visibleWidth()) / float(UsedX2-UsedX1+80);
  float yScale = float(visibleHeight()) / float(UsedY2-UsedY1+80);
  if(xScale > yScale) xScale = yScale;
  xScale /= Scale;

  ViewX1 = UsedX1 - 40;
  ViewY1 = UsedY1 - 40;
  ViewX2 = UsedX2 + 40;
  ViewY2 = UsedY2 + 40;
  zoom(xScale);
}

// ---------------------------------------------------
void Schematic::showNoZoom()
{
  Scale = 1.0;

  int x1 = UsedX1;
  int y1 = UsedY1;
  int x2 = UsedX2;
  int y2 = UsedY2;

  if(x1 > x2) {  // happens e.g. if untitled without changes
    x1 = 0;
    x2 = 800;
  }
  if(y1 > y2) {
    y1 = 0;
    y2 = 800;
  }
  if(x2==0) if(y2==0) if(x1==0) if(y1==0) x2 = y2 = 800;

  ViewX1 = x1-40;
  ViewY1 = y1-40;
  ViewX2 = x2+40;
  ViewY2 = y2+40;
  resizeContents(x2-x1+80, y2-y1+80);
  viewport()->update();
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Enlarge the viewport area if the coordinates x1-x2/y1-y2 exceed the
// visible area.
void Schematic::enlargeView(int x1, int y1, int x2, int y2)
{
  int dx=0, dy=0;
  if(x1 < UsedX1) UsedX1 = x1;
  if(y1 < UsedY1) UsedY1 = y1;
  if(x2 > UsedX2) UsedX2 = x2;
  if(y2 > UsedY2) UsedY2 = y2;

  if(x1 < ViewX1) {
    dx = int(Scale * float(ViewX1-x1+40));
    ViewX1 = x1-40;
  }
  if(y1 < ViewY1) {
    dy = int(Scale * float(ViewY1-y1+40));
    ViewY1 = y1-40;
  }
  if(x2 > ViewX2) ViewX2 = x2+40;
  if(y2 > ViewY2) ViewY2 = y2+40;

  resizeContents(int(Scale*float(ViewX2 - ViewX1)),
		int(Scale*float(ViewY2 - ViewY1)));
  scrollBy(dx,dy);
}

// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
void Schematic::setOnGrid(int& x, int& y)
{
  if(x<0) x -= (GridX >> 1) - 1;
  else x += GridX >> 1;
  x -= x % GridX;

  if(y<0) y -= (GridY >> 1) - 1;
  else y += GridY >> 1;
  y -= y % GridY;
}

// ---------------------------------------------------
void Schematic::paintGrid(ViewPainter *p, int cX, int cY, int Width, int Height)
{
  if(!GridOn) return;

  p->Painter->setPen(QPen(Qt::black,0));
  int dx = -int(Scale*float(ViewX1)) - cX;
  int dy = -int(Scale*float(ViewY1)) - cY;
  p->Painter->drawLine(-3+dx, dy, 4+dx, dy); // small cross at origin
  p->Painter->drawLine( dx,-3+dy, dx, 4+dy);


  int x1  = int(float(cX)/Scale) + ViewX1;
  int y1  = int(float(cY)/Scale) + ViewY1;

  /// \todo setting the center of rotation on the grid causes the center to move when doing multiple rotations when it is not already on the grid. Should not force the center but force the component alignment after rotation.
  setOnGrid(x1, y1);
  if(x1<0) x1 -= GridX - 1;
  else x1 += GridX;
  x1 -= x1 % (GridX << 1);

  if(y1<0) y1 -= GridY - 1;
  else y1 += GridY;
  y1 -= y1 % (GridY << 1);

  float X, Y, Y0, DX, DY;
  X = float(x1)*Scale + p->DX;
  Y = Y0 = float(y1)*Scale + p->DY;
  x1 = X > 0.0 ? int(X + 0.5) : int(X - 0.5);
  y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);


  int xEnd = x1 + Width;
  int yEnd = y1 + Height;
  DX = float(GridX << 1) * Scale;   // every second grid a point
  DY = float(GridY << 1) * Scale;
  while(DX <= 8.0)  DX *= 1.5;  // if too narrow, every third grid a point
  while(DY <= 8.0)  DY *= 1.5;  // if too narrow, every third grid a point

  while(x1 < xEnd) {
    Y = Y0;
    y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);
    while(y1 < yEnd) {
      p->Painter->drawPoint(x1, y1);    // paint grid
      Y += DY;
      y1 = Y > 0.0 ? int(Y + 0.5) : int(Y - 0.5);
    }
    X += DX;
    x1 = X > 0.0 ? int(X + 0.5) : int(X - 0.5);
  }
}

// ---------------------------------------------------
// Correction factor for unproportional font scaling.
float Schematic::textCorr()
{
  QFont Font = QucsSettings.font;
  Font.setPointSizeF( Scale * float(Font.pointSize()) );
  // use the screen-compatible metric
  QFontMetrics  metrics(Font, 0);
  return (Scale / float(metrics.lineSpacing()));
}

// ---------------------------------------------------
void Schematic::sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax)
{
  xmin=INT_MAX;
  ymin=INT_MAX;
  xmax=INT_MIN;
  ymax=INT_MIN;
  Component *pc;
  Diagram *pd;
  Wire *pw;
  WireLabel *pl;
  Painting *pp;

  if(Components->isEmpty())
    if(Wires->isEmpty())
      if(Diagrams->isEmpty())
        if(Paintings->isEmpty()) {
          xmin = xmax = 0;
          ymin = ymax = 0;
          return;
        }


  float Corr = textCorr();
  int x1, y1, x2, y2;
  // find boundings of all components
  for(pc = Components->first(); pc != 0; pc = Components->next()) {
    pc->entireBounds(x1, y1, x2, y2, Corr);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }

  // find boundings of all wires
  for(pw = Wires->first(); pw != 0; pw = Wires->next()) {
    if(pw->x1 < xmin) xmin = pw->x1;
    if(pw->x2 > xmax) xmax = pw->x2;
    if(pw->y1 < ymin) ymin = pw->y1;
    if(pw->y2 > ymax) ymax = pw->y2;

    pl = pw->Label;
    if(pl) {     // check position of wire label
      if(pl->x1 < xmin)  xmin = pl->x1;
      if((pl->x1+pl->x2) > xmax)  xmax = pl->x1 + pl->x2;
      if(pl->y1 > ymax)  ymax = pl->y1;
      if((pl->y1-pl->y2) < ymin)  ymin = pl->y1 - pl->y2;
    }
  }

  // find boundings of all node labels
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
    pl = pn->Label;
    if(pl) {     // check position of node label
      if(pl->x1 < xmin)  xmin = pl->x1;
      if((pl->x1+pl->x2) > xmax)  xmax = pl->x1 + pl->x2;
      if(pl->y1 > ymax)  ymax = pl->y1;
      if((pl->y1-pl->y2) < ymin)  ymin = pl->y1 - pl->y2;
    }
  }

  // find boundings of all diagrams
  for(pd = Diagrams->first(); pd != 0; pd = Diagrams->next()) {
    pd->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;

    foreach(Graph *pg, pd->Graphs)
      // test all markers of diagram
      foreach(Marker *pm, pg->Markers) {
        pm->Bounding(x1, y1, x2, y2);
        if(x1 < xmin) xmin = x1;
        if(x2 > xmax) xmax = x2;
        if(y1 < ymin) ymin = y1;
        if(y2 > ymax) ymax = y2;
      }
  }

  // find boundings of all Paintings
  for(pp = Paintings->first(); pp != 0; pp = Paintings->next()) {
    pp->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }
}

// ---------------------------------------------------
// Rotates all selected components around their midpoint.
bool Schematic::rotateElements()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1=INT_MAX, y1=INT_MAX;
  int x2=INT_MIN, y2=INT_MIN;
  QList<Element *> ElementCache;
  copyLabels(x1, y1, x2, y2, &ElementCache);   // must be first of all !
  copyComponents(x1, y1, x2, y2, &ElementCache);
  copyWires(x1, y1, x2, y2, &ElementCache);
  copyPaintings(x1, y1, x2, y2, &ElementCache);
  if(y1 == INT_MAX) return false;   // no element selected

  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // center for rotation
  y1 = (y1+y2) >> 1;
  //setOnGrid(x1, y1);


  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  foreach(Element *pe, ElementCache)
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->rotate();   //rotate component !before! rotating its center
        pc->setCenter(pc->cy - y1 + x1, x1 - pc->cx + y1);
        insertRawComponent(pc);
        break;

      case isWire:
        pw = (Wire*)pe;
        x2 = pw->x1;
        pw->x1 = pw->y1 - y1 + x1;
        pw->y1 = x1 - x2 + y1;
        x2 = pw->x2;
        pw->x2 = pw->y2 - y1 + x1;
        pw->y2 = x1 - x2 + y1;
        pl = pw->Label;
        if(pl) {
          x2 = pl->cx;
          pl->cx = pl->cy - y1 + x1;
          pl->cy = x1 - x2 + y1;
          if(pl->Type == isHWireLabel)
            pl->Type = isVWireLabel;
          else pl->Type = isHWireLabel;
        }
        insertWire(pw);
        break;

      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	x2 = pl->x1;
	pl->x1 = pl->y1 - y1 + x1;
	pl->y1 = x1 - x2 + y1;
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0) {
	  x2 = pl->x1;
	  pl->x1 = pl->y1 - y1 + x1;
	  pl->y1 = x1 - x2 + y1;
	}
	x2 = pl->cx;
	pl->cx = pl->cy - y1 + x1;
	pl->cy = x1 - x2 + y1;
	insertNodeLabel(pl);
	break;

      case isPainting:
        pp = (Painting*)pe;
        pp->rotate();   // rotate painting !before! rotating its center
        pp->getCenter(x2, y2);
        //qDebug("pp->getCenter(x2, y2): (%i,%i)\n", x2, y2);
        //qDebug("(x1,y1) (x2,y2): (%i,%i) (%i,%i)\n", x1,y1,x2,y2);
        pp->setCenter(y2-y1 + x1, x1-x2 + y1);
        Paintings->append(pp);
        break;
      default: ;
    }

  ElementCache.clear();

  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components.
// First copy them to 'ElementCache', then mirror and insert again.
bool Schematic::mirrorXComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QList<Element *> ElementCache;
  if(!copyComps2WiresPaints(x1, y1, x2, y2, &ElementCache))
    return false;
  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  y1 = (y1+y2) >> 1;   // axis for mirroring
  setOnGrid(y2, y1);
  y1 <<= 1;


  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  foreach(Element *pe, ElementCache)
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
	pc = (Component*)pe;
	pc->mirrorX();   // mirror component !before! mirroring its center
	pc->setCenter(pc->cx, y1 - pc->cy);
	insertRawComponent(pc);
	break;
      case isWire:
	pw = (Wire*)pe;
	pw->y1 = y1 - pw->y1;
	pw->y2 = y1 - pw->y2;
	pl = pw->Label;
	if(pl)  pl->cy = y1 - pl->cy;
	insertWire(pw);
	break;
      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	pl->y1 = y1 - pl->y1;
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0)
	  pl->y1 = y1 - pl->y1;
	pl->cy = y1 - pl->cy;
	insertNodeLabel(pl);
	break;
      case isPainting:
	pp = (Painting*)pe;
	pp->getCenter(x2, y2);
	pp->mirrorX();   // mirror painting !before! mirroring its center
	pp->setCenter(x2, y1 - y2);
	Paintings->append(pp);
	break;
      default: ;
    }

  ElementCache.clear();
  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components. First copy them to 'ElementCache', then mirror and insert again.
bool Schematic::mirrorYComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QList<Element *> ElementCache;
  if(!copyComps2WiresPaints(x1, y1, x2, y2, &ElementCache))
    return false;
  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // axis for mirroring
  setOnGrid(x1, x2);
  x1 <<= 1;

  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  foreach(Element *pe, ElementCache)
    switch(pe->Type) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->mirrorY();   // mirror component !before! mirroring its center
        pc->setCenter(x1 - pc->cx, pc->cy);
        insertRawComponent(pc);
        break;
      case isWire:
        pw = (Wire*)pe;
        pw->x1 = x1 - pw->x1;
        pw->x2 = x1 - pw->x2;
        pl = pw->Label;
        if(pl)  pl->cx = x1 - pl->cx;
        insertWire(pw);
        break;
      case isHWireLabel:
      case isVWireLabel:
        pl = (WireLabel*)pe;
        pl->x1 = x1 - pl->x1;
        break;
      case isNodeLabel:
        pl = (WireLabel*)pe;
        if(pl->pOwner == 0)
          pl->x1 = x1 - pl->x1;
        pl->cx = x1 - pl->cx;
        insertNodeLabel(pl);
        break;
      case isPainting:
        pp = (Painting*)pe;
        pp->getCenter(x2, y2);
        pp->mirrorY();   // mirror painting !before! mirroring its center
        pp->setCenter(x1 - x2, y2);
        Paintings->append(pp);
        break;
      default: ;
    }

  ElementCache.clear();
  setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Updates the graph data of all diagrams (load from data files).
void Schematic::reloadGraphs()
{
  QFileInfo Info(DocName);
  for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    pd->loadGraphData(Info.path()+QDir::separator()+DataSet);
}

// Copy function, 
void Schematic::copy()
{
  QString s = createClipboardFile();
  QClipboard *cb = QApplication::clipboard();  // get system clipboard
  if (!s.isEmpty()) {
    cb->setText(s, QClipboard::Clipboard);
  }
}

// ---------------------------------------------------
// Cut function, copy followed by deletion
void Schematic::cut()
{
  copy();
  deleteElements(); //delete selected elements
  viewport()->update();
}

// ---------------------------------------------------
// Performs paste function from clipboard
bool Schematic::paste(QTextStream *stream, Q3PtrList<Element> *pe)
{
  return pasteFromClipboard(stream, pe);
}

// ---------------------------------------------------
// Loads this Qucs document.
bool Schematic::load()
{
  DocComps.clear();
  DocWires.clear();
  DocNodes.clear();
  DocDiags.clear();
  DocPaints.clear();
  SymbolPaints.clear();

  if(!loadDocument()) return false;
  lastSaved = QDateTime::currentDateTime();

  while(!undoAction.isEmpty()) {
    delete undoAction.last();
    undoAction.pop_back();
  }
  undoActionIdx = 0;
  while(!undoSymbol.isEmpty()) {
    delete undoSymbol.last();
    undoSymbol.pop_back();
  }
  symbolMode = true;
  setChanged(false, true); // "not changed" state, but put on undo stack
  undoSymbolIdx = 0;
  undoSymbol.at(undoSymbolIdx)->replace(1, 1, 'i');
  symbolMode = false;
  setChanged(false, true); // "not changed" state, but put on undo stack
  undoActionIdx = 0;
  undoAction.at(undoActionIdx)->replace(1, 1, 'i');

  // The undo stack of the circuit symbol is initialized when first
  // entering its edit mode.
  
  // have to call this to avoid crash at sizeOfAll
  becomeCurrent(false);

  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(ViewX1 > UsedX1)  ViewX1 = UsedX1;
  if(ViewY1 > UsedY1)  ViewY1 = UsedY1;
  if(ViewX2 < UsedX2)  ViewX2 = UsedX2;
  if(ViewY2 < UsedY2)  ViewY2 = UsedY2;
  zoom(1.0f);
  setContentsPos(tmpViewX1, tmpViewY1);
  tmpViewX1 = tmpViewY1 = -200;   // was used as temporary cache
  return true;
}

// ---------------------------------------------------
// Saves this Qucs document. Returns the number of subcircuit ports.
int Schematic::save()
{
  int result = adjustPortNumbers();// same port number for schematic and symbol
  if(saveDocument() < 0)
     return -1;

  QFileInfo Info(DocName);
  lastSaved = Info.lastModified();

  if(result >= 0) {
    setChanged(false);

    QVector<QString *>::iterator it;
    for (it = undoAction.begin(); it != undoAction.end(); it++) {
      (*it)->replace(1, 1, ' '); //at(1) = ' '; state of being changed
    }
    //(1) = 'i';   // state of being unchanged
    undoAction.at(undoActionIdx)->replace(1, 1, 'i');

    for (it = undoSymbol.begin(); it != undoSymbol.end(); it++) {
      (*it)->replace(1, 1, ' '); //at(1) = ' '; state of being changed
    }
    //at(1) = 'i';   // state of being unchanged
    undoSymbol.at(undoSymbolIdx)->replace(1, 1, 'i');
  }
  // update the subcircuit file lookup hashes
  QucsMain->updateSchNameHash();
  QucsMain->updateSpiceNameHash();

  return result;
}

// ---------------------------------------------------
// If the port number of the schematic and of the symbol are not
// equal add or remove some in the symbol.
int Schematic::adjustPortNumbers()
{
  int x1, x2, y1, y2;
  // get size of whole symbol to know where to place new ports
  if(symbolMode)  sizeOfAll(x1, y1, x2, y2);
  else {
    Components = &SymbolComps;
    Wires      = &SymbolWires;
    Nodes      = &SymbolNodes;
    Diagrams   = &SymbolDiags;
    Paintings  = &SymbolPaints;
    sizeOfAll(x1, y1, x2, y2);
    Components = &DocComps;
    Wires      = &DocWires;
    Nodes      = &DocNodes;
    Diagrams   = &DocDiags;
    Paintings  = &DocPaints;
  }
  x1 += 40;
  y2 += 20;
  setOnGrid(x1, y2);


  Painting *pp;
  // delete all port names in symbol
  for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
    if(pp->Name == ".PortSym ")
      ((PortSymbol*)pp)->nameStr = "";

  QString Str;
  int countPort = 0;

  QFileInfo Info (DataDisplay);
  QString Suffix = Info.suffix();

  // handle VHDL file symbol
  if (Suffix == "vhd" || Suffix == "vhdl") {
    QStringList::iterator it;
    QStringList Names, GNames, GTypes, GDefs;
    int Number;

    // get ports from VHDL file
    QFileInfo Info(DocName);
    QString Name = Info.path() + QDir::separator() + DataDisplay;

    // obtain VHDL information either from open text document or the
    // file directly
    VHDL_File_Info VInfo;
    TextDoc * d = (TextDoc*)App->findDoc (Name);
    if (d)
      VInfo = VHDL_File_Info (d->document()->toPlainText());
    else
      VInfo = VHDL_File_Info (Name, true);

    if (!VInfo.PortNames.isEmpty())
      Names = VInfo.PortNames.split(",", QString::SkipEmptyParts);

    for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
      if(pp->Name == ".ID ") {
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.EntityName.toUpper();
	id->Parameter.clear();
	if (!VInfo.GenNames.isEmpty())
	  GNames = VInfo.GenNames.split(",", QString::SkipEmptyParts);
	if (!VInfo.GenTypes.isEmpty())
	  GTypes = VInfo.GenTypes.split(",", QString::SkipEmptyParts);
	if (!VInfo.GenDefs.isEmpty())
	  GDefs = VInfo.GenDefs.split(",", QString::SkipEmptyParts);;
	for(Number = 1, it = GNames.begin(); it != GNames.end(); ++it) {
	  id->Parameter.append(new SubParameter(
 	    true,
	    *it+"="+GDefs[Number-1],
	    tr("generic")+" "+QString::number(Number),
	    GTypes[Number-1]));
	  Number++;
	}
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) {
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else {
	SymbolPaints.append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
  }
  // handle Verilog-HDL file symbol
  else if (Suffix == "v") {

    QStringList::iterator it;
    QStringList Names;
    int Number;

    // get ports from Verilog-HDL file
    QFileInfo Info (DocName);
    QString Name = Info.path() + QDir::separator() + DataDisplay;

    // obtain Verilog-HDL information either from open text document or the
    // file directly
    Verilog_File_Info VInfo;
    TextDoc * d = (TextDoc*)App->findDoc (Name);
    if (d)
      VInfo = Verilog_File_Info (d->document()->toPlainText());
    else
      VInfo = Verilog_File_Info (Name, true);
    if (!VInfo.PortNames.isEmpty())
      Names = VInfo.PortNames.split(",", QString::SkipEmptyParts);

    for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
      if(pp->Name == ".ID ") {
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.ModuleName.toUpper();
	id->Parameter.clear();
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) {
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else {
	SymbolPaints.append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
  }
  // handle Verilog-A file symbol
  else if (Suffix == "va") {

    QStringList::iterator it;
    QStringList Names;
    int Number;

    // get ports from Verilog-A file
    QFileInfo Info (DocName);
    QString Name = Info.path() + QDir::separator() + DataDisplay;

    // obtain Verilog-A information either from open text document or the
    // file directly
    VerilogA_File_Info VInfo;
    TextDoc * d = (TextDoc*)App->findDoc (Name);
    if (d)
      VInfo = VerilogA_File_Info (d->toPlainText());
    else
      VInfo = VerilogA_File_Info (Name, true);

    if (!VInfo.PortNames.isEmpty())
      Names = VInfo.PortNames.split(",", QString::SkipEmptyParts);

    for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
      if(pp->Name == ".ID ") {
	ID_Text * id = (ID_Text *) pp;
	id->Prefix = VInfo.ModuleName.toUpper();
	id->Parameter.clear();
      }

    for(Number = 1, it = Names.begin(); it != Names.end(); ++it, Number++) {
      countPort++;

      Str = QString::number(Number);
      // search for matching port symbol
      for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
	if(pp->Name == ".PortSym ")
	  if(((PortSymbol*)pp)->numberStr == Str) break;

      if(pp)
	((PortSymbol*)pp)->nameStr = *it;
      else {
	SymbolPaints.append(new PortSymbol(x1, y2, Str, *it));
	y2 += 40;
      }
    }
  }
  // handle schematic symbol
  else
  {
      // go through all components in a schematic
      for(Component *pc = DocComps.first(); pc!=0; pc = DocComps.next())
      {
         if(pc->Model == "Port")
         {
             countPort++;

             Str = pc->Props.getFirst()->Value;
             // search for matching port symbol
             for(pp = SymbolPaints.first(); pp!=0; pp = SymbolPaints.next())
             {
                 if(pp->Name == ".PortSym ")
                 {
                   if(((PortSymbol*)pp)->numberStr == Str) break;
                 }
             }

             if(pp)
             {
                 ((PortSymbol*)pp)->nameStr = pc->Name;
             }
             else
             {
                 SymbolPaints.append(new PortSymbol(x1, y2, Str, pc->Name));
                 y2 += 40;
             }
          }
      }
  }

  // delete not accounted port symbols
  for(pp = SymbolPaints.first(); pp!=0; ) {
    if(pp->Name == ".PortSym ")
      if(((PortSymbol*)pp)->nameStr.isEmpty()) {
        SymbolPaints.remove();
        pp = SymbolPaints.current();
        continue;
      }
    pp = SymbolPaints.next();
  }

  return countPort;
}

// ---------------------------------------------------
bool Schematic::undo()
{
  if(symbolMode) {
    if (undoSymbolIdx == 0) { return false; }

    rebuildSymbol(undoSymbol.at(--undoSymbolIdx));
    adjustPortNumbers();  // set port names

    emit signalUndoState(undoSymbolIdx != 0);
    emit signalRedoState(undoSymbolIdx != undoSymbol.size()-1);

    if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i' && 
        undoAction.at(undoActionIdx)->at(1) == 'i') {
      setChanged(false, false);
      return true;
    }

    setChanged(true, false);
    return true;
  }


  // ...... for schematic edit mode .......
  if (undoActionIdx == 0) { return false; }

  rebuild(undoAction.at(--undoActionIdx));
  reloadGraphs();  // load recent simulation data

  emit signalUndoState(undoActionIdx != 0);
  emit signalRedoState(undoActionIdx != undoAction.size()-1);

  if(undoAction.at(undoActionIdx)->at(1) == 'i') {
    if(undoSymbol.isEmpty()) {
      setChanged(false, false);
      return true;
    }
    else if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i') {
      setChanged(false, false);
      return true;
    }
  }

  setChanged(true, false);
  return true;
}

// ---------------------------------------------------
bool Schematic::redo()
{
  if(symbolMode) {
    if (undoSymbolIdx == undoSymbol.size() - 1) { return false; }

    rebuildSymbol(undoSymbol.at(++undoSymbolIdx));
    adjustPortNumbers();  // set port names

    emit signalUndoState(undoSymbolIdx != 0);
    emit signalRedoState(undoSymbolIdx != undoSymbol.size()-1);

    if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i'
        && undoAction.at(undoActionIdx)->at(1) == 'i') {
      setChanged(false, false);
      return true;
    }

    setChanged(true, false);
    return true;
  }


  //
  // ...... for schematic edit mode .......
  if (undoActionIdx == undoAction.size()-1) { return false; }

  rebuild(undoAction.at(++undoActionIdx));
  reloadGraphs();  // load recent simulation data

  emit signalUndoState(undoActionIdx != 0);
  emit signalRedoState(undoActionIdx != undoAction.size()-1);

  if (undoAction.at(undoActionIdx)->at(1) == 'i') {
    if(undoSymbol.isEmpty()) {
      setChanged(false, false);
      return true;
    }
    else if(undoSymbol.at(undoSymbolIdx)->at(1) == 'i') {
      setChanged(false, false);
      return true;
    }
    
  }

  setChanged(true, false);
  return true;
}

// ---------------------------------------------------
// Sets selected elements on grid.
bool Schematic::elementsOnGrid()
{
  int x, y, No;
  bool count = false;
  WireLabel *pl, *pLabel;
  Q3PtrList<WireLabel> LabelCache;

  // test all components
  Components->setAutoDelete(false);
  for(Component *pc = Components->last(); pc != 0; pc = Components->prev())
    if(pc->isSelected) {

      // rescue non-selected node labels
      foreach(Port *pp, pc->Ports)
        if(pp->Connection->Label)
          if(pp->Connection->Connections.count() < 2) {
            LabelCache.append(pp->Connection->Label);
            pp->Connection->Label->pOwner = 0;
            pp->Connection->Label = 0;
          }

      x = pc->cx;
      y = pc->cy;
      No = Components->at();
      deleteComp(pc);
      setOnGrid(pc->cx, pc->cy);
      insertRawComponent(pc);
      Components->at(No);   // restore current list position
      pc->isSelected = false;
      count = true;

      x -= pc->cx;
      y -= pc->cy;    // re-insert node labels and correct position
      for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next()) {
        pl->cx -= x;
        pl->cy -= y;
        insertNodeLabel(pl);
      }
      LabelCache.clear();
    }
  Components->setAutoDelete(true);

  Wires->setAutoDelete(false);
  // test all wires and wire labels
  for(Wire *pw = Wires->last(); pw != 0; pw = Wires->prev()) {
    pl = pw->Label;
    pw->Label = 0;

    if(pw->isSelected) {
      // rescue non-selected node label
      pLabel = 0;
      if(pw->Port1->Label) {
        if(pw->Port1->Connections.count() < 2) {
            pLabel = pw->Port1->Label;
            pw->Port1->Label = 0;
        }
      }
      else if(pw->Port2->Label) {
        if(pw->Port2->Connections.count() < 2) {
            pLabel = pw->Port2->Label;
            pw->Port2->Label = 0;
        }
      }

      No = Wires->at();
      deleteWire(pw);
      setOnGrid(pw->x1, pw->y1);
      setOnGrid(pw->x2, pw->y2);
      insertWire(pw);
      Wires->at(No);   // restore current list position
      pw->isSelected = false;
      count = true;
      if(pl)
        setOnGrid(pl->cx, pl->cy);

      if(pLabel) {
        setOnGrid(pLabel->cx, pLabel->cy);
        insertNodeLabel(pLabel);
      }
    }

    if(pl) {
      pw->Label = pl;
      if(pl->isSelected) {
        setOnGrid(pl->x1, pl->y1);
        pl->isSelected = false;
        count = true;
      }
    }
  }
  Wires->setAutoDelete(true);

  // test all node labels
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label)
      if(pn->Label->isSelected) {
        setOnGrid(pn->Label->x1, pn->Label->y1);
        pn->Label->isSelected = false;
        count = true;
      }

  // test all diagrams
  for(Diagram *pd = Diagrams->last(); pd != 0; pd = Diagrams->prev()) {
    if(pd->isSelected) {
      setOnGrid(pd->cx, pd->cy);
      pd->isSelected = false;
      count = true;
    }

    foreach(Graph *pg,pd->Graphs)
      // test markers of diagram
      foreach(Marker *pm, pg->Markers)
        if(pm->isSelected) {
	  x = pm->x1 + pd->cx;
	  y = pm->y1 + pd->cy;
	  setOnGrid(x, y);
	  pm->x1 = x - pd->cx;
	  pm->y1 = y - pd->cy;
	  pm->isSelected = false;
	  count = true;
        }
  }

  // test all paintings
  for(Painting *pa = Paintings->last(); pa != 0; pa = Paintings->prev())
    if(pa->isSelected) {
      setOnGrid(pa->cx, pa->cy);
      pa->isSelected = false;
      count = true;
    }

  if(count) setChanged(true, true);
  return count;
}

// ---------------------------------------------------
void Schematic::switchPaintMode()
{
  symbolMode = !symbolMode;  // change mode

  int tmp, t2;
  float temp;
  temp = Scale; Scale  = tmpScale;  tmpScale  = temp;
  tmp = contentsX();
  t2  = contentsY();
  setContentsPos(tmpPosX, tmpPosY);
  tmpPosX = tmp;
  tmpPosY = t2;
  tmp = ViewX1; ViewX1 = tmpViewX1; tmpViewX1 = tmp;
  tmp = ViewY1; ViewY1 = tmpViewY1; tmpViewY1 = tmp;
  tmp = ViewX2; ViewX2 = tmpViewX2; tmpViewX2 = tmp;
  tmp = ViewY2; ViewY2 = tmpViewY2; tmpViewY2 = tmp;
  tmp = UsedX1; UsedX1 = tmpUsedX1; tmpUsedX1 = tmp;
  tmp = UsedY1; UsedY1 = tmpUsedY1; tmpUsedY1 = tmp;
  tmp = UsedX2; UsedX2 = tmpUsedX2; tmpUsedX2 = tmp;
  tmp = UsedY2; UsedY2 = tmpUsedY2; tmpUsedY2 = tmp;
}


// *********************************************************************
// **********                                                 **********
// **********      Function for serving mouse wheel moving    **********
// **********                                                 **********
// *********************************************************************
void Schematic::contentsWheelEvent(QWheelEvent *Event)
{
  App->editText->setHidden(true);  // disable edit of component property
  int delta = Event->delta() >> 1;     // use smaller steps

  // ...................................................................
  if((Event->modifiers() & Qt::ShiftModifier) ||
     (Event->orientation() == Qt::Horizontal)) { // scroll horizontally ?
      if(delta > 0) { if(scrollLeft(delta)) scrollBy(-delta, 0); }
      else { if(scrollRight(delta)) scrollBy(-delta, 0); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }
  // ...................................................................
  else if(Event->modifiers() & Qt::ControlModifier) {  // use mouse wheel to zoom ?
      float Scaling;
      if(delta < 0) Scaling = float(delta)/-60.0/1.1;
      else Scaling = 1.1*60.0/float(delta);
      zoom(Scaling);
      Scaling -= 1.0;
      scrollBy( int(Scaling * float(Event->pos().x())),
                int(Scaling * float(Event->pos().y())) );
  }
  // ...................................................................
  else {     // scroll vertically !
      if(delta > 0) { if(scrollUp(delta)) scrollBy(0, -delta); }
      else { if(scrollDown(delta)) scrollBy(0, -delta); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }

  Event->accept();   // QScrollView must not handle this event
}

// -----------------------------------------------------------
// Scrolls the visible area upwards and enlarges or reduces the view
// area accordingly.
bool Schematic::scrollUp(int step)
{
  int diff;

  diff = contentsY() - step;
  if(diff < 0) {     // scroll outside the active area ?  (upwards)
    resizeContents(contentsWidth(), contentsHeight()-diff);
    ViewY1 += diff;
    scrollBy(0, diff);
    return false;
  }

  diff = ViewY2 - UsedY2 - 20;    // keep border of 20
  if(diff > 0) {      // make active area smaller ?
    if(step < diff) diff = step;
    resizeContents(contentsWidth(), contentsHeight()-diff);
    ViewY2 -= diff;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area downwards and enlarges or reduces the view
// area accordingly. ("step" must be negative!)
bool Schematic::scrollDown(int step)
{
  int diff;

  diff = contentsHeight() - contentsY()-visibleHeight() + step;
  if(diff < 0) {     // scroll outside the active area ?  (downwards)
    resizeContents(contentsWidth(), contentsHeight()-diff);
    ViewY2 -= diff;
    scrollBy(0, -step);
    return false;
  }

  diff = ViewY1 - UsedY1 + 20;    // keep border of 20
  if(diff < 0) {      // make active area smaller ?
    if(step > diff) diff = step;
    resizeContents(contentsWidth(), contentsHeight()+diff);
    ViewY1 -= diff;
    return false;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the left and enlarges or reduces the view
// area accordingly.
bool Schematic::scrollLeft(int step)
{
  int diff;

  diff = contentsX() - step;
  if(diff < 0) {     // scroll outside the active area ?  (to the left)
    resizeContents(contentsWidth()-diff, contentsHeight());
    ViewX1 += diff;
    scrollBy(diff, 0);
    return false;
  }

  diff = ViewX2 - UsedX2 - 20;    // keep border of 20
  if(diff > 0) {      // make active area smaller ?
    if(step < diff) diff = step;
    resizeContents(contentsWidth()-diff, contentsHeight());
    ViewX2 -= diff;
  }

  return true;
}

// -----------------------------------------------------------
// Scrolls the visible area to the right and enlarges or reduces the
// view area accordingly. ("step" must be negative!)
bool Schematic::scrollRight(int step)
{
  int diff;

  diff = contentsWidth() - contentsX()-visibleWidth() + step;
  if(diff < 0) {     // scroll outside the active area ?  (to the right)
    resizeContents(contentsWidth()-diff, contentsHeight());
    ViewX2 -= diff;
    scrollBy(-step, 0);
    return false;
  }

  diff = ViewX1 - UsedX1 + 20;    // keep border of 20
  if(diff < 0) {      // make active area smaller ?
    if(step > diff) diff = step;
    resizeContents(contentsWidth()+diff, contentsHeight());
    ViewX1 -= diff;
    return false;
  }

  return true;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollUp()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollUp(verticalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollDown()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollDown(-verticalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollLeft()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollLeft(horizontalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}

// -----------------------------------------------------------
// Is called if the scroll arrow of the ScrollBar is pressed.
void Schematic::slotScrollRight()
{
  App->editText->setHidden(true);  // disable edit of component property
  scrollRight(-horizontalScrollBar()->singleStep());
  viewport()->update(); // because QScrollView thinks nothing has changed
  App->view->drawn = false;
}


// *********************************************************************
// **********                                                 **********
// **********        Function for serving drag'n drop         **********
// **********                                                 **********
// *********************************************************************

// Is called if an object is dropped (after drag'n drop).
void Schematic::contentsDropEvent(QDropEvent *Event)
{
  if(dragIsOkay) {
    QList<QUrl> urls = Event->mimeData()->urls();
    if (urls.isEmpty()) {
      return;
    }

    // do not close untitled document to avoid segfault
    QucsDoc *d = QucsMain->getDoc(0);
    bool changed = d->DocChanged;
    d->DocChanged = true;

    // URI:  file:/home/linuxuser/Desktop/example.sch
    foreach(QUrl url, urls) {
      App->gotoPage(QDir::convertSeparators(url.toLocalFile()));
    }

    d->DocChanged = changed;
    return;
  }


  QMouseEvent e(QEvent::MouseButtonPress, Event->pos(),
                Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  int x = int(Event->pos().x()/Scale) + ViewX1;
  int y = int(Event->pos().y()/Scale) + ViewY1;

  App->view->MPressElement(this, &e, x, y);

  if(App->view->selElem) delete App->view->selElem;
  App->view->selElem = 0;  // no component selected

  if(formerAction)
    formerAction->setChecked(true);  // restore old action
}

// ---------------------------------------------------
void Schematic::contentsDragEnterEvent(QDragEnterEvent *Event)
{
  //FIXME: the function of drag library component seems not working?
  formerAction = 0;
  dragIsOkay = false;

  // file dragged in ?
  if(Event->mimeData()->hasUrls()) {
    dragIsOkay = true;
    Event->accept();
    return;
  }

  // drag library component
  if(Event->mimeData()->hasText()) {
    QString s = Event->mimeData()->text();
    if(s.left(15) == "QucsComponent:<") {
      s = s.mid(14);
      App->view->selElem = getComponentFromName(s);
      if(App->view->selElem) {
        Event->accept();
        return;
      }
    }
    Event->ignore();
    return;
  }


  if(Event->format(1) == 0) {  // only one MIME type ?

    // drag component from listview
    if(Event->provides("application/x-qabstractitemmodeldatalist")) {
      QListWidgetItem *Item = App->CompComps->currentItem();
      if(Item) {
        formerAction = App->activeAction;
        App->slotSelectComponent(Item);  // also sets drawn=false
        App->MouseMoveAction = 0;
        App->MousePressAction = 0;

        Event->accept();
        return;
      }
    }
  }

  Event->ignore();
}

// ---------------------------------------------------
void Schematic::contentsDragLeaveEvent(QDragLeaveEvent*)
{
  if(App->view->selElem)
    if(App->view->selElem->Type & isComponent)
      if(App->view->drawn) {

        QPainter painter(viewport());
        App->view->setPainter(this);
        ((Component*)App->view->selElem)->paintScheme(this);
        App->view->drawn = false;
      }

  if(formerAction)
    formerAction->setChecked(true);  // restore old action
}

// ---------------------------------------------------
void Schematic::contentsDragMoveEvent(QDragMoveEvent *Event)
{
  if(!dragIsOkay) {
    if(App->view->selElem == 0) {
      Event->ignore();
      return;
    }

    QMouseEvent e(QEvent::MouseMove, Event->pos(), Qt::NoButton, 
		  Qt::NoButton, Qt::NoModifier);
    App->view->MMoveElement(this, &e);
  }

  Event->accept();
}

