/***************************************************************************
                              mouseactions.cpp
                             ------------------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/* Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>        */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "mouseactions.h"
#include "components/component.h"
#include "components/componentdialog.h"
#include "components/optimizedialog.h"
#include "components/spicedialog.h"
#include "components/spicefile.h"
#include "components/vacomponent.h"
#include "diagrams/diagramdialog.h"
#include "diagrams/markerdialog.h"
#include "diagrams/tabdiagram.h"
#include "diagrams/timingdiagram.h"
#include "dialogs/labeldialog.h"
#include "dialogs/textboxdialog.h"
#include "dialogs/tuner.h"
#include "extsimkernels/customsimdialog.h"
#include "main.h"
#include "module.h"
#include "node.h"
#include "qucs.h"
#include "schematic.h"
#include "spicecomponents/sp_customsim.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextStream>
#include <qt3_compat/q3ptrlist.h>

#include <climits>
#include <cstdlib>

#define MIN_SELECT_SIZE 5.0

QAction *formerAction; // remember action before drag n'drop etc.

MouseActions::MouseActions(QucsApp *App_)
{
    App = App_;          // pointer to main app
    selElem = nullptr;         // no component/diagram is selected
    isMoveEqual = false; // mouse cursor move x and y the same way
    focusElement = 0;    //element being interacted with mouse

    // ...............................................................
    // initialize menu appearing by right mouse button click on component
    ComponentMenu = new QMenu(QucsMain);
    focusMEvent = new QMouseEvent(QEvent::MouseButtonPress,
                                  QPoint(0, 0),
                                  Qt::NoButton,
                                  Qt::NoButton,
                                  Qt::NoModifier);
}

MouseActions::~MouseActions()
{
    delete ComponentMenu;
    delete focusMEvent;
}

// -----------------------------------------------------------
void MouseActions::setPainter(Schematic *Doc)
{
    // contents to viewport transformation

    Doc->PostPaintEvent(_Translate, -Doc->contentsX(), -Doc->contentsY());
    Doc->PostPaintEvent(_Scale, Doc->Scale, Doc->Scale);
    Doc->PostPaintEvent(_Translate, -Doc->ViewX1, -Doc->ViewY1);
    Doc->PostPaintEvent(_DotLine);
    Doc->PostPaintEvent(_NotRop);
}

// -----------------------------------------------------------
bool MouseActions::pasteElements(Schematic *Doc)
{
    QClipboard *cb = QApplication::clipboard(); // get system clipboard
    QString s = cb->text(QClipboard::Clipboard);
    QTextStream stream(&s, QIODevice::ReadOnly);
    movingElements.clear();
    if (!Doc->paste(&stream, &movingElements))
        return false;

    Element *pe;
    int xmax, xmin, ymax, ymin;
    xmin = ymin = INT_MAX;
    xmax = ymax = INT_MIN;
    // First, get the max and min coordinates of all selected elements.
    for (pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
        if (pe->Type == isWire) {
            if (pe->x1 < xmin)
                xmin = pe->x1;
            if (pe->x2 > xmax)
                xmax = pe->x2;
            if (pe->y1 < ymin)
                ymin = pe->y1;
            if (pe->y2 > ymax)
                ymax = pe->y2;
        } else {
            if (pe->cx < xmin)
                xmin = pe->cx;
            if (pe->cx > xmax)
                xmax = pe->cx;
            if (pe->cy < ymin)
                ymin = pe->cy;
            if (pe->cy > ymax)
                ymax = pe->cy;
        }
    }

    xmin = -((xmax + xmin) >> 1); // calculate midpoint
    ymin = -((ymax + ymin) >> 1);
    Doc->setOnGrid(xmin, ymin);

    // moving with mouse cursor in the midpoint
    for (pe = movingElements.first(); pe != 0; pe = movingElements.next())
        if (pe->Type & isLabel) {
            pe->cx += xmin;
            pe->x1 += xmin;
            pe->cy += ymin;
            pe->y1 += ymin;
        } else
            pe->setCenter(xmin, ymin, true);

    return true;
}

// -----------------------------------------------------------
void MouseActions::editLabel(Schematic *Doc, WireLabel *pl)
{
    LabelDialog *Dia = new LabelDialog(pl, Doc);
    int Result = Dia->exec();
    if (Result == 0)
        return;

    QString Name = Dia->NodeName->text();
    QString Value = Dia->InitValue->text();
    delete Dia;

    if (Name.isEmpty() && Value.isEmpty()) { // if nothing entered, delete label
        pl->pOwner->Label = 0;               // delete name of wire
        delete pl;
    } else {
        /*    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;
    if(Name == pl->Name) return;*/
        if (Result == 1)
            return; // nothing changed

        int old_x2 = pl->x2;
        pl->setName(Name); // set new name
        pl->initValue = Value;
        if (pl->cx > (pl->x1 + (pl->x2 >> 1)))
            pl->x1 -= pl->x2 - old_x2; // don't change position due to text width
    }

    Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
// Reinserts all elements (moved by the user) back into the schematic.
void MouseActions::endElementMoving(Schematic *Doc,
                                    Q3PtrList<Element> *movElements) {
  Element *pe;
  for (pe = movElements->first(); pe != nullptr; pe = movElements->next()) {
    //    pe->isSelected = false;  // deselect first (maybe afterwards pe ==
    //    NULL)
    switch (pe->Type) { // FIXME: use casts.
    case isWire:
      if (pe->x1 == pe->x2)
        if (pe->y1 == pe->y2) {
          // Delete wires with zero length, but preserve label.
          if (((Wire *)pe)->Label) {
            Doc->insertNodeLabel((WireLabel *)((Wire *)pe)->Label);
            ((Wire *)pe)->Label = nullptr;
          }
          delete (Wire *)pe;
          break;
        }

      Doc->insertWire((Wire *)pe);
      break;
    case isDiagram:
      Doc->Diagrams->append((Diagram *)pe);
      break;
    case isPainting:
      Doc->Paintings->append((Painting *)pe);
      break;
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
      Doc->insertRawComponent((Component *)pe, false);
      break;
    case isMovingLabel:
    case isHMovingLabel:
    case isVMovingLabel:
      Doc->insertNodeLabel((WireLabel *)pe);
      break;
    case isMarker:
      assert(dynamic_cast<Marker *>(pe));
      break;
    }
  }

  movElements->clear();

  if ((MAx3 != 0) || (MAy3 != 0)) // moved or put at the same place ?
    Doc->setChanged(true, true);

  // enlarge viewarea if components lie outside the view
  Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
  Doc->enlargeView(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);

  Doc->viewport()->update();
  drawn = false;
}

// -----------------------------------------------------------
// Moves elements in "movElements" by x/y
void MouseActions::moveElements(Q3PtrList<Element> *movElements, int x, int y)
{
    Wire *pw;
    Element *pe;
    for (pe = movElements->first(); pe != 0; pe = movElements->next()) {
        if (pe->Type == isWire) {
            pw = (Wire *) pe; // connected wires are not moved completely

            if (((uintptr_t) pw->Port1) > 3) {
                pw->x1 += x;
                pw->y1 += y;
                if (pw->Label) {
                    pw->Label->cx += x;
                    pw->Label->cy += y;
                }
            } else {
                if ((uintptr_t) (pw->Port1) & 1) {
                    pw->x1 += x;
                }
                if ((uintptr_t) (pw->Port1) & 2) {
                    pw->y1 += y;
                }
            }

            if (((uintptr_t) pw->Port2) > 3) {
                pw->x2 += x;
                pw->y2 += y;
            } else {
                if ((uintptr_t) (pw->Port2) & 1)
                    pw->x2 += x;
                if ((uintptr_t) (pw->Port2) & 2)
                    pw->y2 += y;
            }

            if (pw->Label) { // root of node label must lie on wire
                if (pw->Label->cx < pw->x1)
                    pw->Label->cx = pw->x1;
                if (pw->Label->cy < pw->y1)
                    pw->Label->cy = pw->y1;
                if (pw->Label->cx > pw->x2)
                    pw->Label->cx = pw->x2;
                if (pw->Label->cy > pw->y2)
                    pw->Label->cy = pw->y2;
            }

        } else
            pe->setCenter(x, y, true);
    }
}

// ***********************************************************************
// **********                                                   **********
// **********       Functions for serving mouse moving          **********
// **********                                                   **********
// ***********************************************************************
void MouseActions::MMoveElement(Schematic *Doc, QMouseEvent *Event)
{
    if (selElem == nullptr)
        return;

    //  qDebug() << "MMoveElement got selElem";

    QPoint contentsCoordinates = Event->pos();
    QPoint modelCoordinates = Doc->contentsToModel(contentsCoordinates);

    int x = contentsCoordinates.x();
    int y = contentsCoordinates.y();
    int fx = modelCoordinates.x();
    int fy = modelCoordinates.y();
    int gx = fx;
    int gy = fy;
    Doc->setOnGrid(gx, gy);

    //QPainter painter(Doc->viewport());
    setPainter(Doc);

    if (selElem->Type == isPainting) {
        Doc->PostPaintEvent(_NotRop, 0, 0, 0, 0);
        ((Painting *) selElem)->MouseMoving(Doc, x, y, gx, gy, Doc, fx, fy, drawn);
        drawn = true;
        Doc->viewport()->update();
        return;
    } // of "if(isPainting)"

    // ********** it is a component or diagram
    if (drawn)
        selElem->paintScheme(Doc); // erase old scheme
    drawn = true;

    //  Component *comp = (Component*)selElem;
    //qDebug() << "desc" << comp->Description << "gx" << gx << "gy" << gy;

    selElem->setCenter(gx, gy);
    selElem->paintScheme(Doc); // paint scheme at new position
    Doc->viewport()->update();
}

/**
 * @brief draws wire aiming cross
 * @param Doc - pointer to Schematics object
 * @param aimX  - model x-coordinate of aim
 * @param aimY  - model y-coordinate of aim
 */
static void paintAim(Schematic *Doc, int aimX, int aimY) {
    // What we want to do here is to draw a cross centered at (aimX, aimY)
    // which lines don't touch the bounds of visible part of schematic,
    // i.e. there should be a little gap between the line ends and picture
    // bounds:
    //
    //       Bad                Good
    // +---------+---+    +--------------+
    // |         |   |    |         |    |
    // +---------+---+    | --------+--- |
    // |         |   |    |         |    |
    // |         |   |    |         |    |
    // +---------+---+    +--------------+
    //
    // The cross is drawn using Schematic 'PostPaintEvent' subsystem.
    // This is not a requirement, it's just an inherited implementation.
    // Maybe there is a better way to do it, but until it's found let's
    // stick to the way things already work.
    //
    // PostPaintEvent subsystem operates in *model* coordinates – the coordinates
    // used to locate components, wires, etc.
    //
    // To draw the cross we want we have to follow these steps:
    //   1. Find the size of viewport (visible part of schematic)
    //   2. Shrink it a bit. The resulting rectangle is the *bounding* for the cross
    //   3. Transform bounding rectangle coordinates to model coordinate system.
    //   4. Using resulting 'in-model' coordinates of bounding rectangle, post
    //      two 'paint line' events

    const QRect viewportAimBounds =
        QRect{0, 0, Doc->viewport()->width(), Doc->viewport()->height()}
        .marginsRemoved(QMargins{2, 2, 2, 2});

    const QRect aimBounds{
        Doc->viewportToModel(viewportAimBounds.topLeft()),
        Doc->viewportToModel(viewportAimBounds.bottomRight())
    };

    // Horizontal line of cross
    Doc->PostPaintEvent(_Line, aimBounds.left(), aimY, aimBounds.right(), aimY);
    // Vertical line of cross
    Doc->PostPaintEvent(_Line, aimX, aimBounds.top(), aimX, aimBounds.bottom());
}

//paint ghost line - horizontal
static void paintGhostLineH(Schematic *Doc, int fx, int fy, int fxx)
{
    Doc->PostPaintEvent(_Line, fx, fy - 1, fxx, fy - 1);
    Doc->PostPaintEvent(_Line, fx, fy + 1, fxx, fy + 1);
}

//paint ghost line - vertical
static void paintGhostLineV(Schematic *Doc, int fx, int fy, int fyy)
{
    Doc->PostPaintEvent(_Line, fx - 1, fy, fx - 1, fyy);
    Doc->PostPaintEvent(_Line, fx + 1, fy, fx + 1, fyy);
}

// -----------------------------------------------------------
/**
 * @brief MouseActions::MMoveWire2 Paint wire as it is being drawn with mouse.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveWire2(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx2 = inModel.x();
    MAy2 = inModel.y();
    Doc->setOnGrid(MAx2, MAy2);
    paintAim(Doc, MAx2, MAy2); //let we paint aim cross

    //because cross slightly masks a wire, let we make wire thicker
    //better to make it by increasing of pen, but here we cannot access
    //pen
    if (MAx1 == 0) {
        paintGhostLineV(Doc, MAx3, MAy3, MAy2);
        paintGhostLineH(Doc, MAx3, MAy2, MAx2);
    } else {
        paintGhostLineH(Doc, MAx3, MAy3, MAx2);
        paintGhostLineV(Doc, MAx2, MAy3, MAy2);
    }

    QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickWire2;
    Doc->viewport()->update();
}

/**
 * @brief MouseActions::MMoveWire1 Paint hair cross for "insert wire" mode
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveWire1(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();
    Doc->setOnGrid(MAx3, MAy3);
    paintAim(Doc, MAx3, MAy3);

    inModel = Doc->contentsToModel(
        QPoint{
            Doc->contentsX() + Doc->viewport()->width() - 1 - 2,
            Doc->contentsY() + Doc->viewport()->height() - 1 - 2
        }
    );
    MAx2 = inModel.x();
    MAx2 = inModel.y();
    Doc->viewport()->update();
}

/**
 * @brief MouseActions::MMoveSelect Paints a rectangle for select area.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveSelect(Schematic *Doc, QMouseEvent *Event)
{
    //qDebug() << "MMoveSelect " << "select area";
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx2 = inModel.x() - MAx1;
    MAy2 = inModel.y() - MAy1;
    if (isMoveEqual) { // x and y size must be equal ?
        if (abs(MAx2) > abs(MAy2)) {
            if (MAx2 < 0)
                MAx2 = -abs(MAy2);
            else
                MAx2 = abs(MAy2);
        } else {
            if (MAy2 < 0)
                MAy2 = -abs(MAx2);
            else
                MAy2 = abs(MAx2);
        }
    }

    Doc->PostPaintEvent(_SelectionRect, MAx1, MAy1, MAx2, MAy2);
}

// -----------------------------------------------------------
void MouseActions::MMoveResizePainting(Schematic *Doc, QMouseEvent *Event)
{
    setPainter(Doc);

    auto inModel = Doc->contentsToModel(Event->pos());
    MAx1 = inModel.x();
    MAy1 = inModel.y();
    Doc->setOnGrid(MAx1, MAy1);
    ((Painting *) focusElement)->MouseResizeMoving(MAx1, MAy1, Doc);
}

// -----------------------------------------------------------
// Moves components by keeping the mouse button pressed.
void MouseActions::MMoveMoving(Schematic *Doc, QMouseEvent *Event)
{
    setPainter(Doc);

    auto inModel = Doc->contentsToModel(Event->pos());
    MAx2 = inModel.x();
    MAy2 = inModel.y();

    Doc->setOnGrid(MAx2, MAy2);
    MAx3 = MAx1 = MAx2 - MAx1;
    MAy3 = MAy1 = MAy2 - MAy1;

    movingElements.clear();
    Doc->copySelectedElements(&movingElements);
    Doc->viewport()->repaint();

    Wire *pw;
    // Changes the position of all moving elements by dx/dy
    for (Element *pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
        if (pe->Type == isWire) {
            pw = (Wire *) pe; // connecting wires are not moved completely

            if (((uintptr_t) pw->Port1) > 3) {
                pw->x1 += MAx1;
                pw->y1 += MAy1;
            } else {
                if ((uintptr_t) (pw->Port1) & 1) {
                    pw->x1 += MAx1;
                }
                if ((uintptr_t) (pw->Port1) & 2) {
                    pw->y1 += MAy1;
                }
            }

            if (((uintptr_t) pw->Port2) > 3) {
                pw->x2 += MAx1;
                pw->y2 += MAy1;
            } else {
                if ((uintptr_t) (pw->Port2) & 1)
                    pw->x2 += MAx1;
                if ((uintptr_t) (pw->Port2) & 2)
                    pw->y2 += MAy1;
            }

            if (pw->Label) { // root of node label must lie on wire
                if (pw->Label->cx < pw->x1)
                    pw->Label->cx = pw->x1;
                if (pw->Label->cy < pw->y1)
                    pw->Label->cy = pw->y1;
                if (pw->Label->cx > pw->x2)
                    pw->Label->cx = pw->x2;
                if (pw->Label->cy > pw->y2)
                    pw->Label->cy = pw->y2;
            }

        } else
            pe->setCenter(MAx1, MAy1, true);

        pe->paintScheme(Doc);
    }

    drawn = true;
    MAx1 = MAx2;
    MAy1 = MAy2;
    QucsMain->MouseMoveAction = &MouseActions::MMoveMoving2;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseMoving;
}

// -----------------------------------------------------------
// Moves components by keeping the mouse button pressed.
void MouseActions::MMoveMoving2(Schematic *Doc, QMouseEvent *Event)
{
  setPainter(Doc);

  auto inModel = Doc->contentsToModel(Event->pos());
  MAx2 = inModel.x();
  MAy2 = inModel.y();

  Element *pe;
  if (drawn) // erase old scheme
    for (pe = movingElements.first(); pe != nullptr; pe = movingElements.next())
      pe->paintScheme(Doc);

  drawn = true;
  if ((Event->modifiers().testFlag(Qt::ControlModifier)) == 0)
    Doc->setOnGrid(MAx2, MAy2); // use grid only if CTRL key not pressed
  MAx1 = MAx2 - MAx1;
  MAy1 = MAy2 - MAy1;
  MAx3 += MAx1;
  MAy3 += MAy1; // keep track of the complete movement

  moveElements(&movingElements, MAx1, MAy1); // moves elements by MAx1/MAy1

  // paint afterwards to avoid conflict between wire and label painting
  for (pe = movingElements.first(); pe != 0; pe = movingElements.next())
    pe->paintScheme(Doc);
  //    if(pe->Type == isWire)  if(((Wire*)pe)->Label)
  //      if(!((Wire*)pe)->Label->isSelected)
  //        ((Wire*)pe)->Label->paintScheme(&painter);

  MAx1 = MAx2;
  MAy1 = MAy2;
}

/**
 * @brief MouseActions::MMovePaste Moves components after paste from clipboard.
 * @param Doc
 * @param Event
 */
void MouseActions::MMovePaste(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx1 = inModel.x();
    MAy1 = inModel.y();
    moveElements(Doc, MAx1, MAy1);
    paintElementsScheme(Doc);

    drawn = true;
    QucsMain->MouseMoveAction = &MouseActions::MMoveMoving2;
    QucsMain->MouseReleaseAction = &MouseActions::MReleasePaste;
}

// -----------------------------------------------------------
// Moves scroll bar of diagram (e.g. tabular) according the mouse cursor.
void MouseActions::MMoveScrollBar(Schematic *Doc, QMouseEvent *Event)
{
    TabDiagram *d = (TabDiagram *) focusElement;
    auto inModel = Doc->contentsToModel(Event->pos());
    int x = inModel.x();
    int y = inModel.y();

    if (d->scrollTo(MAx2, x - MAx1, y - MAy1)) {
        Doc->setChanged(true, true, 'm'); // 'm' = only the first time

        // FIXME #warning QPainter p(Doc->viewport());
        // FIXME #warning ViewPainter Painter;
        // FIXME #warning Painter.init(&p, Doc->Scale, -Doc->ViewX1, -Doc->ViewY1,
        // FIXME #warning                  Doc->contentsX(), Doc->contentsY());
        // FIXME #warning     Painter.fillRect(d->cx-d->x1, d->cy-d->y2, d->x2+d->x1, d->y2+d->y1,
        // FIXME #warning                      QucsSettings.BGColor);
        // FIXME #warning     d->paint(&Painter);
    }
}

/**
* @brief MouseActions::MMoveDelete
*   Paints a cross under the mouse cursor to show the delete mode.
* @param Doc Schematic document
* @param Event
*/
void MouseActions::MMoveDelete(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    // cannot draw on the viewport, it is displaced by the size of dock and toolbar
    Doc->PostPaintEvent(_Line, MAx3 - 15, MAy3 - 15, MAx3 + 15, MAy3 + 15, 0, 0, false);
    Doc->PostPaintEvent(_Line, MAx3 - 15, MAy3 + 15, MAx3 + 15, MAy3 - 15, 0, 0, false);
}

/**
 * @brief MouseActions::MMoveLabel Paints a label above the mouse cursor for "set wire label".
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveLabel(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    // paint marker
    Doc->PostPaintEvent(_Line, MAx3, MAy3, MAx3 + 10, MAy3 - 10);
    Doc->PostPaintEvent(_Line, MAx3 + 10, MAy3 - 10, MAx3 + 20, MAy3 - 10);
    Doc->PostPaintEvent(_Line, MAx3 + 10, MAy3 - 10, MAx3 + 10, MAy3 - 17);

    // paint A
    Doc->PostPaintEvent(_Line, MAx3 + 12, MAy3 - 12, MAx3 + 15, MAy3 - 23);
    Doc->PostPaintEvent(_Line, MAx3 + 14, MAy3 - 17, MAx3 + 17, MAy3 - 17);
    Doc->PostPaintEvent(_Line, MAx3 + 19, MAy3 - 12, MAx3 + 16, MAy3 - 23);
}

/**
 * @brief MouseActions::MMoveMarker Paints a triangle above the mouse for "set marker on graph"
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMarker(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Line, MAx3, MAy3 - 2, MAx3 - 8, MAy3 - 10);
    Doc->PostPaintEvent(_Line, MAx3 + 1, MAy3 - 3, MAx3 + 8, MAy3 - 10);
    Doc->PostPaintEvent(_Line, MAx3 - 7, MAy3 - 10, MAx3 + 7, MAy3 - 10);
}

/**
 * @brief MouseActions::MMoveSetLimits Sets the cursor to a magnifying glass with a wave.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveSetLimits(Schematic *Doc, QMouseEvent *Event)
{
    // TODO: Refactor to a QRectF for easy normalisation etc.
    // Update the second point of the selection rectangle.
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();
}

/**
 * @brief MouseActions::MMoveMirrorX Paints rounded "mirror about y axis" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMirrorY(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Line, MAx3 - 11, MAy3 - 4, MAx3 - 9, MAy3 - 9);
    Doc->PostPaintEvent(_Line, MAx3 - 11, MAy3 - 3, MAx3 - 6, MAy3 - 3);
    Doc->PostPaintEvent(_Line, MAx3 + 11, MAy3 - 4, MAx3 + 9, MAy3 - 9);
    Doc->PostPaintEvent(_Line, MAx3 + 11, MAy3 - 3, MAx3 + 6, MAy3 - 3);
    Doc->PostPaintEvent(_Arc, MAx3 - 10, MAy3 - 8, 21, 10, 16 * 20, 16 * 140, false);
}

/**
 * @brief MouseActions::MMoveMirrorX Paints rounded "mirror about x axis" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMirrorX(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Line, MAx3 - 4, MAy3 - 11, MAx3 - 9, MAy3 - 9);
    Doc->PostPaintEvent(_Line, MAx3 - 3, MAy3 - 11, MAx3 - 3, MAy3 - 6);
    Doc->PostPaintEvent(_Line, MAx3 - 4, MAy3 + 11, MAx3 - 9, MAy3 + 9);
    Doc->PostPaintEvent(_Line, MAx3 - 3, MAy3 + 11, MAx3 - 3, MAy3 + 6);
    Doc->PostPaintEvent(_Arc, MAx3 - 8, MAy3 - 10, 10, 21, 16 * 110, 16 * 140, false);
}

/**
 * @brief MouseActions::MMoveMirrorX Paints "rotate" mouse cursor
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveRotate(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Line, MAx3 - 6, MAy3 + 8, MAx3 - 6, MAy3 + 1);
    Doc->PostPaintEvent(_Line, MAx3 - 7, MAy3 + 8, MAx3 - 12, MAy3 + 8);
    Doc->PostPaintEvent(_Arc, MAx3 - 10, MAy3 - 10, 21, 21, -16 * 20, 16 * 240, false);
}

/**
 * @brief MouseActions::MMoveActivate Paints a crossed box mouse cursor to "(de)activate" components.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveActivate(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Rect, MAx3, MAy3 - 9, 14, 10);
    Doc->PostPaintEvent(_Line, MAx3, MAy3 - 9, MAx3 + 13, MAy3);
    Doc->PostPaintEvent(_Line, MAx3, MAy3, MAx3 + 13, MAy3 - 9);
}

/**
 * @brief MouseActions::MMoveOnGrid Paints a grid beside the mouse cursor, put "on grid" mode.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveOnGrid(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Line, MAx3 + 10, MAy3 + 3, MAx3 + 25, MAy3 + 3);
    Doc->PostPaintEvent(_Line, MAx3 + 10, MAy3 + 7, MAx3 + 25, MAy3 + 7);
    Doc->PostPaintEvent(_Line, MAx3 + 10, MAy3 + 11, MAx3 + 25, MAy3 + 11);
    Doc->PostPaintEvent(_Line, MAx3 + 13, MAy3, MAx3 + 13, MAy3 + 15);
    Doc->PostPaintEvent(_Line, MAx3 + 17, MAy3, MAx3 + 17, MAy3 + 15);
    Doc->PostPaintEvent(_Line, MAx3 + 21, MAy3, MAx3 + 21, MAy3 + 15);
}

/**
 * @brief MouseActions::MMoveMoveTextB Paints mouse symbol for "move component text" mode.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMoveTextB(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Line, MAx3 + 14, MAy3, MAx3 + 16, MAy3);
    Doc->PostPaintEvent(_Line, MAx3 + 23, MAy3, MAx3 + 25, MAy3);
    Doc->PostPaintEvent(_Line, MAx3 + 13, MAy3, MAx3 + 13, MAy3 + 3);
    Doc->PostPaintEvent(_Line, MAx3 + 13, MAy3 + 7, MAx3 + 13, MAy3 + 10);
    Doc->PostPaintEvent(_Line, MAx3 + 14, MAy3 + 10, MAx3 + 16, MAy3 + 10);
    Doc->PostPaintEvent(_Line, MAx3 + 23, MAy3 + 10, MAx3 + 25, MAy3 + 10);
    Doc->PostPaintEvent(_Line, MAx3 + 26, MAy3, MAx3 + 26, MAy3 + 3);
    Doc->PostPaintEvent(_Line, MAx3 + 26, MAy3 + 7, MAx3 + 26, MAy3 + 10);
}

/**
 * @brief MouseActions::MMoveMoveText Paint rectangle around component text being mouse moved
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveMoveText(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    int newX = inModel.x();
    int newY = inModel.y();
    MAx1 += newX - MAx3;
    MAy1 += newY - MAy3;
    MAx3 = newX;
    MAy3 = newY;

    Doc->PostPaintEvent(_Rect, MAx1, MAy1, MAx2, MAy2);
}

/**
 * @brief MouseActions::MMoveZoomIn Paints symbol beside the mouse to show the "Zoom in" modus.
 * @param Doc
 * @param Event
 */
void MouseActions::MMoveZoomIn(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx3 = inModel.x();
    MAy3 = inModel.y();

    Doc->PostPaintEvent(_Line, MAx3 + 14, MAy3, MAx3 + 22, MAy3);
    Doc->PostPaintEvent(_Line, MAx3 + 18, MAy3 - 4, MAx3 + 18, MAy3 + 4);
    Doc->PostPaintEvent(_Ellipse, MAx3 + 12, MAy3 - 6, 13, 13, 0, 0, false);
    Doc->viewport()->update();
}

// ************************************************************************
// **********                                                    **********
// **********    Functions for serving mouse button clicking     **********
// **********                                                    **********
// ************************************************************************

// Is called from several MousePress functions to show right button menu.
void MouseActions::rightPressMenu(Schematic *Doc, QMouseEvent *Event, float fX, float fY)
{
    MAx1 = int(fX);
    MAy1 = int(fY);
    focusElement = Doc->selectElement(fX, fY, false);

    if (focusElement) // remove special function (4 least significant bits)
        focusElement->Type &= isSpecialMask;

    // define menu
    ComponentMenu->clear();
    while (true) {
        if (focusElement) {
            focusElement->isSelected = true;
            QAction *editProp = new QAction(QObject::tr("Edit Properties"), QucsMain);
            QObject::connect(editProp, SIGNAL(triggered(bool)), QucsMain, SLOT(slotEditElement()));
            ComponentMenu->addAction(editProp);

            if ((focusElement->Type & isComponent) == 0)
                break;
        } else {
            /// \todo "exchange like this"
            //ComponentMenu->addAction(QucsMain->symEdit);
            //to QucsMain->symEdit->addTo(ComponentMenu);
            // see http://qt-project.org/doc/qt-4.8/qaction-qt3.html#addTo
            ComponentMenu->addAction(QucsMain->symEdit);
            ComponentMenu->addAction(QucsMain->fileSettings);
        }
        if (!QucsMain->moveText->isChecked())
            ComponentMenu->addAction(QucsMain->moveText);
        break;
    }
    while (true) {
        if (focusElement)
            if (focusElement->Type == isGraph)
                break;
        if (!QucsMain->onGrid->isChecked())
            ComponentMenu->addAction(QucsMain->onGrid);
        ComponentMenu->addAction(QucsMain->editCopy);
        if (!QucsMain->editPaste->isChecked())
            ComponentMenu->addAction(QucsMain->editPaste);
        break;
    }

    while (true) {
        if (focusElement) {
            if (focusElement->Type == isDiagram) {
                Diagram* diagram = static_cast<Diagram*>(focusElement);

                // Only show reset limits action if one or more axis is not autoscaled.
                if (diagram->Name == "Rect" && 
                    (!diagram->xAxis.autoScale || !diagram->yAxis.autoScale || !diagram->zAxis.autoScale)) {
                    ComponentMenu->addAction(QucsMain->resetDiagramLimits);
                }

                // TODO: This should probably be in qucs_init::initActions.
                QAction *actExport = new QAction(QObject::tr("Export as image"), QucsMain);
                QObject::connect(actExport,
                                 SIGNAL(triggered(bool)),
                                 QucsMain,
                                 SLOT(slotSaveDiagramToGraphicsFile()));
                ComponentMenu->addAction(actExport);
            }
            /*if (focusElement->Type & isComponent) {
          Component *pc = (Component *)focusElement;
          if (pc->Model == "EDD") {
              QAction *actEDDtoIFS = new QAction(QObject::tr("Create XSPICE IFS"), QucsMain);
              QObject::connect(actEDDtoIFS,SIGNAL(triggered(bool)),QucsMain,SLOT(slotEDDtoIFS()));
              ComponentMenu->addAction(actEDDtoIFS);
              QAction *actEDDtoMOD = new QAction(QObject::tr("Create XSPICE MOD"), QucsMain);
              QObject::connect(actEDDtoMOD,SIGNAL(triggered(bool)),QucsMain,SLOT(slotEDDtoMOD()));
              ComponentMenu->addAction(actEDDtoMOD);
          }
      }*/
        }
        break;
    }

    if (!QucsMain->editDelete->isChecked())
        ComponentMenu->addAction(QucsMain->editDelete);
    if (focusElement)
        if (focusElement->Type == isMarker) {
            ComponentMenu->addSeparator();
            QString s = QObject::tr("power matching");
            if (((Marker *) focusElement)->pGraph->Var == "Sopt")
                s = QObject::tr("noise matching");
            QAction *actPwrMatching = new QAction(s, QucsMain);
            QObject::connect(actPwrMatching,
                             SIGNAL(triggered(bool)),
                             QucsMain,
                             SLOT(slotPowerMatching()));
            ComponentMenu->addAction(actPwrMatching);
            if (((Marker *) focusElement)->pGraph->Var.left(2) == "S[") {
                QAction *act2PortMatching = new QAction(QObject::tr("2-port matching"), QucsMain);
                QObject::connect(act2PortMatching,
                                 SIGNAL(triggered(bool)),
                                 QucsMain,
                                 SLOT(slot2PortMatching()));
                ComponentMenu->addAction(act2PortMatching);
            }
        }
    do {
        if (focusElement) {
            if (focusElement->Type == isDiagram)
                break;
            if (focusElement->Type == isGraph) {
                ComponentMenu->addAction(QucsMain->graph2csv);
                break;
            }
        }
        ComponentMenu->addSeparator();
        if (focusElement)
            if (focusElement->Type & isComponent)
                if (!QucsMain->editActivate->isChecked())
                    ComponentMenu->addAction(QucsMain->editActivate);
        if (!QucsMain->editRotate->isChecked())
            ComponentMenu->addAction(QucsMain->editRotate);
        if (!QucsMain->editMirror->isChecked())
            ComponentMenu->addAction(QucsMain->editMirror);
        if (!QucsMain->editMirrorY->isChecked())
            ComponentMenu->addAction(QucsMain->editMirrorY);

        // right-click menu to go into hierarchy
        if (focusElement) {
            if (focusElement->Type & isComponent)
                if (((Component *) focusElement)->Model == "Sub")
                    if (!QucsMain->intoH->isChecked())
                        ComponentMenu->addAction(QucsMain->intoH);
        }
        // right-click menu to pop out of hierarchy
        if (!focusElement)
            if (!QucsMain->popH->isChecked())
                ComponentMenu->addAction(QucsMain->popH);
    } while (false);

    //*focusMEvent = *Event;  // remember event for "edit component" action
#if QT_VERSION >= 0x060000
    ComponentMenu->popup(Event->globalPosition().toPoint());
#else
    ComponentMenu->popup(Event->globalPos());
#endif
    Doc->viewport()->update();
    drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MPressLabel(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    int x = int(fX), y = int(fY);
    Wire *pw = 0;
    WireLabel *pl = 0;
    Node *pn = Doc->selectedNode(x, y);
    if (!pn) {
        pw = Doc->selectedWire(x, y);
        if (!pw)
            return;
    }

    QString Name, Value;
    Element *pe = 0;
    // is wire line already labeled ?
    if (pw)
        pe = Doc->getWireLabel(pw->Port1);
    else
        pe = Doc->getWireLabel(pn);
    if (pe) {
        if (pe->Type & isComponent) {
            QMessageBox::information(0,
                                     QObject::tr("Info"),
                                     QObject::tr("The ground potential cannot be labeled!"));
            return;
        }
        pl = ((Conductor *) pe)->Label;
    }

    LabelDialog *Dia = new LabelDialog(pl, Doc);
    if (Dia->exec() == 0)
        return;

    Name = Dia->NodeName->text();
    Value = Dia->InitValue->text();
    delete Dia;

    if (Name.isEmpty() && Value.isEmpty()) { // if nothing entered, delete name
        if (pe) {
            if (((Conductor *) pe)->Label)
                delete ((Conductor *) pe)->Label; // delete old name
            ((Conductor *) pe)->Label = 0;
        } else {
            if (pw)
                pw->setName("", ""); // delete name of wire
            else
                pn->setName("", "");
        }
    } else {
        /*    Name.replace(' ', '_');	// label must not contain spaces
    while(Name.at(0) == '_') Name.remove(0,1);  // must not start with '_'
    if(Name.isEmpty()) return;
*/
        if (pe) {
            delete ((Conductor *) pe)->Label; // delete old name
            ((Conductor *) pe)->Label = nullptr;
        }

        int xl = x + 30;
        int yl = y - 30;
        Doc->setOnGrid(xl, yl);
        // set new name
        if (pw)
            pw->setName(Name, Value, x - pw->x1 + y - pw->y1, xl, yl);
        else
            pn->setName(Name, Value, xl, yl);
    }

    Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MPressSelect(Schematic *Doc, QMouseEvent *Event, float fX, float fY)
{
    bool Ctrl;
    if (Event->modifiers().testFlag(Qt::ControlModifier))
        Ctrl = true;
    else
        Ctrl = false;

    int No = 0;
    MAx1 = int(fX);
    MAy1 = int(fY);
    focusElement = Doc->selectElement(fX, fY, Ctrl, &No);
    isMoveEqual = false; // moving not necessarily square

    if (focusElement)
        // print define value in hex, see element.h
        qDebug() << "MPressSelect: focusElement->Type"
                 << QString("0x%1").arg(focusElement->Type, 0, 16);
    else
        qDebug() << "MPressSelect";

    if (focusElement)
        switch (focusElement->Type) {
        case isPaintingResize: // resize painting ?
            focusElement->Type = isPainting;
            QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizePainting;
            QucsMain->MouseMoveAction = &MouseActions::MMoveResizePainting;
            QucsMain->MousePressAction = 0;
            QucsMain->MouseDoubleClickAction = 0;
            Doc->grabKeyboard(); // no keyboard inputs during move actions
            // Update matching wire label highlighting
            Doc->highlightWireLabels();
            return;

        case isDiagramResize: // resize diagram ?
            if (((Diagram *) focusElement)->Name.left(4) != "Rect")
                if (((Diagram *) focusElement)->Name.at(0) != 'T')
                    if (((Diagram *) focusElement)->Name != "Curve")
                        isMoveEqual = true; // diagram must be square

            focusElement->Type = isDiagram;
            MAx1 = focusElement->cx;
            MAx2 = focusElement->x2;
            if (((Diagram *) focusElement)->State & 1) {
                MAx1 += MAx2;
                MAx2 *= -1;
            }
            MAy1 = focusElement->cy;
            MAy2 = -focusElement->y2;
            if (((Diagram *) focusElement)->State & 2) {
                MAy1 += MAy2;
                MAy2 *= -1;
            }

            QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizeDiagram;
            QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
            QucsMain->MousePressAction = 0;
            QucsMain->MouseDoubleClickAction = 0;
            Doc->grabKeyboard(); // no keyboard inputs during move actions
            // Update matching wire label highlighting
            Doc->highlightWireLabels();
            return;

        case isDiagramHScroll: // scroll in tabular ?
        case isDiagramVScroll:
            if (focusElement->Type == isDiagramHScroll)
            {
                MAy1 = MAx1;
            }

            focusElement->Type = isDiagram;

            No = ((TabDiagram *) focusElement)->scroll(MAy1);

            switch (No) {
            case 1:
                Doc->setChanged(true, true, 'm'); // 'm' = only the first time
                break;
            case 2: // move scroll bar with mouse cursor
                QucsMain->MouseMoveAction = &MouseActions::MMoveScrollBar;
                QucsMain->MousePressAction = 0;
                QucsMain->MouseDoubleClickAction = 0;
                Doc->grabKeyboard(); // no keyboard inputs during move actions

                // Remember initial scroll bar position.
                MAx2 = int(((TabDiagram *) focusElement)->xAxis.limit_min);
                // Update matching wire label highlighting
                Doc->highlightWireLabels();
                return;
            }
            // Update matching wire label highlighting
            Doc->highlightWireLabels();
            Doc->viewport()->update();
            drawn = false;
            return;

        case isComponentText: // property text of component ?
            focusElement->Type &= (~isComponentText) | isComponent;

            MAx3 = No;
            QucsMain->slotApplyCompText();
            // Update matching wire label highlighting
            Doc->highlightWireLabels();
            return;

        case isNode:
            if (QucsSettings.NodeWiring) {
                MAx1 = 0;                // paint wire corner first up, then left/right
                MAx3 = focusElement->cx; // works even if node is not on grid
                MAy3 = focusElement->cy;
                QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
                QucsMain->MousePressAction = &MouseActions::MPressWire2;
                QucsMain->MouseReleaseAction = 0; // if function is called from elsewhere
                QucsMain->MouseDoubleClickAction = 0;

                formerAction = QucsMain->select; // to restore action afterwards
                QucsMain->activeAction = QucsMain->insWire;

                QucsMain->select->blockSignals(true);
                QucsMain->select->setChecked(false);
                QucsMain->select->blockSignals(false);

                QucsMain->insWire->blockSignals(true);
                QucsMain->insWire->setChecked(true);
                QucsMain->insWire->blockSignals(false);
                // Update matching wire label highlighting
                Doc->highlightWireLabels();
                return;
            }
        }

    QucsMain->MousePressAction = 0;
    QucsMain->MouseDoubleClickAction = 0;
    Doc->grabKeyboard(); // no keyboard inputs during move actions
    Doc->viewport()->update();
    drawn = false;

    if (focusElement == 0) {
        MAx2 = 0; // if not clicking on an element => open a rectangle
        MAy2 = 0;
        QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect2;
        QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
    } else {
        // element could be moved
        if (!Ctrl) {
            if (!focusElement->isSelected)           // Don't move selected elements if clicked
                Doc->deselectElements(focusElement); // element was not selected.
            focusElement->isSelected = true;
        }
        Doc->setOnGrid(MAx1, MAy1);
        QucsMain->MouseMoveAction = &MouseActions::MMoveMoving;
    }
    // Update matching wire label highlighting
    Doc->highlightWireLabels();
}

// -----------------------------------------------------------
void MouseActions::MPressDelete(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    Element *pe = Doc->selectElement(fX, fY, false);
    if (pe) {
        pe->isSelected = true;
        Doc->deleteElements();

        Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
        Doc->viewport()->update();
        drawn = false;
    }
}

// -----------------------------------------------------------
void MouseActions::MPressActivate(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    MAx1 = int(fX);
    MAy1 = int(fY);
    if (!Doc->activateSpecifiedComponent(MAx1, MAy1)) {
        //    if(Event->button() != Qt::LeftButton) return;
        MAx2 = 0; // if not clicking on a component => open a rectangle
        MAy2 = 0;
        QucsMain->MousePressAction = 0;
        QucsMain->MouseReleaseAction = &MouseActions::MReleaseActivate;
        QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
    }
    Doc->viewport()->update();
    drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MPressMirrorX(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    // no use in mirroring wires or diagrams
    Component *c = Doc->selectedComponent(int(fX), int(fY));
    if (c) {
        if (c->Ports.count() < 1)
            return; // only mirror components with ports
        c->mirrorX();
        Doc->setCompPorts(c);
    } else {
        Painting *p = Doc->selectedPainting(fX, fY);
        if (p == 0)
            return;
        p->mirrorX();
    }

    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MPressMirrorY(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    // no use in mirroring wires or diagrams
    Component *c = Doc->selectedComponent(int(fX), int(fY));
    if (c) {
        if (c->Ports.count() < 1)
            return; // only mirror components with ports
        c->mirrorY();
        Doc->setCompPorts(c);
    } else {
        Painting *p = Doc->selectedPainting(fX, fY);
        if (p == 0)
            return;
        p->mirrorY();
    }

    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MPressRotate(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    Element *e = Doc->selectElement(int(fX), int(fY), false);
    if (e == 0)
        return;
    e->Type &= isSpecialMask; // remove special functions

    WireLabel *pl;
    int x1, y1, x2, y2;
    //  e->isSelected = false;
    switch (e->Type) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
        if (((Component *) e)->Ports.count() < 1)
            break; // do not rotate components without ports
        ((Component *) e)->rotate();
        Doc->setCompPorts((Component *) e);
        // enlarge viewarea if component lies outside the view
        ((Component *) e)->entireBounds(x1, y1, x2, y2);
        Doc->enlargeView(x1, y1, x2, y2);
        break;

    case isWire:
        pl = ((Wire *) e)->Label;
        ((Wire *) e)->Label = 0; // prevent label to be deleted
        Doc->Wires->setAutoDelete(false);
        Doc->deleteWire((Wire *) e);
        ((Wire *) e)->Label = pl;
        ((Wire *) e)->rotate();
        Doc->setOnGrid(e->x1, e->y1);
        Doc->setOnGrid(e->x2, e->y2);
        if (pl)
            Doc->setOnGrid(pl->cx, pl->cy);
        Doc->insertWire((Wire *) e);
        Doc->Wires->setAutoDelete(true);
        if (Doc->Wires->containsRef((Wire *) e))
            Doc->enlargeView(e->x1, e->y1, e->x2, e->y2);
        break;

    case isPainting:
        ((Painting *) e)->rotate(0, 0);
        // enlarge viewarea if component lies outside the view
        ((Painting *) e)->Bounding(x1, y1, x2, y2);
        Doc->enlargeView(x1, y1, x2, y2);
        break;

    default:
        return;
    }
    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
// insert component, diagram, painting into schematic ?!
void MouseActions::MPressElement(Schematic *Doc, QMouseEvent *Event, float, float)
{
    if (selElem == 0)
        return;
    //QPainter painter(Doc->viewport());
    //setPainter(Doc, &painter);

    int x1, y1, x2, y2, rot;
    if (selElem->Type & isComponent) {
        Component *Comp = (Component *) selElem;
        //    qDebug() << "+-+ got to switch:" << Comp->Name;
        QString entryName = Comp->Name;

        switch (Event->button()) {
        case Qt::LeftButton:
            // left mouse button inserts component into the schematic
            // give the component a pointer to the schematic it's a
            // part of
            Comp->setSchematic(Doc);
            Comp->textSize(x1, y1);
            Doc->insertComponent(Comp);
            Comp->textSize(x2, y2);
            if (Comp->tx < Comp->x1)
                Comp->tx -= x2 - x1;

            // Note: insertCopmponents does increment  name1 -> name2
            //    qDebug() << "  +-+ got to insert:" << Comp->Name;

            // enlarge viewarea if component lies outside the view
            Comp->entireBounds(x1, y1, x2, y2);
            Doc->enlargeView(x1, y1, x2, y2);
            //Doc->setOnGrid(Comp->cx,Comp->cy);

            drawn = false;
            Doc->viewport()->update();
            Doc->setChanged(true, true);
            rot = Comp->rotated;

            // handle static and dynamic components
            //    QucsApp::CompChoose;
            if (Module::vaComponents.contains(entryName)) {
                QString filename = Module::vaComponents[entryName];
                //      qDebug() << "   ===+ recast";
                Comp = dynamic_cast<vacomponent *>(Comp)->newOne(filename); //va component
                qDebug() << "   => recast = Comp;" << Comp->Name << "filename: " << filename;
            } else {
                Comp = Comp->newOne(); // static component is used, so create a new one
            }
            rot -= Comp->rotated;
            rot &= 3;
            while (rot--)
                Comp->rotate(); // keep last rotation for single component
            break;

        case Qt::RightButton: // right mouse button rotates the component
            if (Comp->Ports.count() == 0)
                break;              // do not rotate components without ports
            Comp->paintScheme(Doc); // erase old component scheme
            Doc->viewport()->repaint();
            Comp->rotate();
            Doc->setOnGrid(Comp->cx,Comp->cy);
            Comp->paintScheme(Doc); // paint new component scheme
            break;

        default:; // avoids compiler warnings
        }
        //    qDebug() << "   => selElem = Comp;" << Comp->Name;
        // comp it getting empty
        selElem = Comp;
        return;

    } // of "if(isComponent)"
    else if (selElem->Type == isDiagram) {
        if (Event->button() != Qt::LeftButton)
            return;

        Diagram *Diag = (Diagram *) selElem;
        QFileInfo Info(Doc->DocName);
        // dialog is Qt::WDestructiveClose !!!
        DiagramDialog *dia = new DiagramDialog(Diag, Doc);
        if (dia->exec() == QDialog::Rejected) { // don't insert if dialog canceled
            Doc->viewport()->update();
            drawn = false;
            return;
        }

        Doc->Diagrams->append(Diag);
        Doc->enlargeView(Diag->cx, Diag->cy - Diag->y2, Diag->cx + Diag->x2, Diag->cy);
        Doc->setChanged(true, true); // document has been changed

        Doc->viewport()->repaint();
        Diag = Diag->newOne(); // the component is used, so create a new one
        Diag->paintScheme(Doc);
        selElem = Diag;
        return;
    } // of "if(isDiagram)"

    // ***********  it is a painting !!!
    if (((Painting *) selElem)->MousePressing(Doc)) {
        Doc->Paintings->append((Painting *) selElem);
        ((Painting *) selElem)->Bounding(x1, y1, x2, y2);
        //Doc->enlargeView(x1, y1, x2, y2);
        selElem = ((Painting *) selElem)->newOne();

        Doc->viewport()->update();
        Doc->setChanged(true, true);

        MMoveElement(Doc, Event); // needed before next mouse pressing
        drawn = false;
    }
}

/**
 * @brief MouseActions::MPressWire1 Is called if starting point of wire is pressed
 * @param Doc
 * @param fX
 * @param fY
 */
void MouseActions::MPressWire1(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    //Doc->PostPaintEvent (_DotLine);
    //Doc->PostPaintEvent (PPENotRop);
    //if(drawn) {
#if 0 //ALYS - it draws some garbage, not deleted because of possible questions
	Doc->PostPaintEvent (_Line, 0, MAy3, MAx2, MAy3); // erase old mouse cross
    Doc->PostPaintEvent (_Line, MAx3, 0, MAx3, MAy2);
#endif
    //}
    //drawn = false;

    MAx1 = 0; // paint wire corner first up, then left/right
    MAx3 = int(fX);
    MAy3 = int(fY);
    Doc->setOnGrid(MAx3, MAy3);

    //ALYS - draw aiming cross
    paintAim(Doc, MAx3, MAy3);
    //#######################

    formerAction = 0; // keep wire action active after first wire finished
    QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
    QucsMain->MousePressAction = &MouseActions::MPressWire2;
    // Double-click action is set in "MMoveWire2" to not initiate it
    // during "Wire1" actions.
    Doc->viewport()->update();
}

/**
 * @brief MouseActions::MPressWire2 Is called if ending point of wire is pressed
 * @param Doc
 * @param Event
 * @param fX
 * @param fY
 */
void MouseActions::MPressWire2(Schematic *Doc, QMouseEvent *Event, float fX, float fY)
{
    int set1 = 0, set2 = 0;
    switch (Event->button()) {
    case Qt::LeftButton:
        if (MAx1 == 0) { // which wire direction first ?
            if (MAy2 != MAy3)
                set1 = Doc->insertWire(new Wire(MAx3, MAy3, MAx3, MAy2));
            if (MAx2 != MAx3) {
                set2 = set1;
                set1 = Doc->insertWire(new Wire(MAx3, MAy2, MAx2, MAy2));
            }
        } else {
            if (MAx2 != MAx3)
                set1 = Doc->insertWire(new Wire(MAx3, MAy3, MAx2, MAy3));
            if (MAy2 != MAy3) {
                set2 = set1;
                set1 = Doc->insertWire(new Wire(MAx2, MAy3, MAx2, MAy2));
            }
        }

        if (set1 & 2) {
            // if last port is connected, then...
            if (formerAction) {
                // ...restore old action
                QucsMain->select->setChecked(true);
            } else {
                // ...start a new wire
                QucsMain->MouseMoveAction = &MouseActions::MMoveWire1;
                QucsMain->MousePressAction = &MouseActions::MPressWire1;
                QucsMain->MouseDoubleClickAction = 0;
            }
        }

        //ALYS: excessive update. end of function does it.
        //Doc->viewport()->update();

        drawn = false;
        if (set1 | set2)
            Doc->setChanged(true, true);
        MAx3 = MAx2;
        MAy3 = MAy2;
        break;

        /// \todo document right mouse button changes the wire corner
    case Qt::RightButton:

#if 0
	//ALYS - old code preserved because isn't clear - what it was???
	//looks like deletion via painting.
	//i'll delete it after possible clarification from team
	if(MAx1 == 0) {
      Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx3, MAy2); // erase old
      Doc->PostPaintEvent (_Line, MAx3, MAy2, MAx2, MAy2); // erase old
    }
    else {
      Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx2, MAy3); // erase old
      Doc->PostPaintEvent (_Line, MAx2, MAy3, MAx2, MAy2); // erase old
    }
#endif

        MAx2 = int(fX);
        MAy2 = int(fY);
        Doc->setOnGrid(MAx2, MAy2);

        MAx1 ^= 1; // change the painting direction of wire corner
        if (MAx1 == 0) {
            paintGhostLineV(Doc, MAx3, MAy3, MAy2);
            paintGhostLineH(Doc, MAx3, MAy2, MAx2);
        } else {
            paintGhostLineH(Doc, MAx3, MAy3, MAx2);
            paintGhostLineV(Doc, MAx2, MAy3, MAy2);
        }
        break;

    default:; // avoids compiler warnings
    }

    paintAim(Doc, MAx2, MAy2); //ALYS - added missed aiming cross
    Doc->viewport()->update();
}

// -----------------------------------------------------------
// Is called for setting a marker on a diagram's graph
void MouseActions::MPressMarker(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    MAx1 = int(fX);
    MAy1 = int(fY);
    Marker *pm = Doc->setMarker(MAx1, MAy1);

    if (pm) {
        assert(pm->diag());
        int x0 = pm->diag()->cx;
        int y0 = pm->diag()->cy;
        Doc->enlargeView(x0 + pm->x1, y0 - pm->y1 - pm->y2, x0 + pm->x1 + pm->x2, y0 - pm->y1);
    }
    Doc->viewport()->update();
    drawn = false;
}

/**
 * @brief MouseActions::MPressSetLimits Sets the start point of the diagram limits.
 * @param Doc
 * @param Event
 */
void MouseActions::MPressSetLimits(Schematic *Doc, QMouseEvent*, float fX, float fY)
{
    // fX, fY are the scaled / adjusted coordinates, equivalent to DOC_X_POS(Event->x()).
    // MAx1, MAy1 are needed to set the start of the selection box.
    MAx1 = int(fX);
    MAy1 = int(fY);

    // TODO: Diagrams is currently a Q3PtrList, but it would be better to refactor
    // this (and many other collections) to be std::vector.
    // Check to see if the mouse is within a diagram using the oddly named "getSelected".
    for (Diagram* diagram = Doc->Diagrams->last(); diagram != 0; diagram = Doc->Diagrams->prev()) {
        // BUG: Obtaining the diagram type by name is marked as a bug elsewhere (to be solved separately).
        // TODO: Currently only rectangular diagrams are supported.
        if (diagram->getSelected(fX, fY) && diagram->Name == "Rect") {
            qDebug() << "In a rectangular diagram, setting up for area selection.";

            // cx and cy are the adjusted points of the diagram's bottom left hand corner.
            mouseDownPoint = QPointF(fX - diagram->cx, diagram->cy - fY);
            pActiveDiagram = diagram;

            QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
            QucsMain->MouseReleaseAction = &MouseActions::MReleaseSetLimits;
            Doc->grabKeyboard(); // no keyboard inputs during move actions
            
            // No need to continue searching;
            break;
        }
    }

    Doc->viewport()->update();
    drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MPressOnGrid(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    Element *pe = Doc->selectElement(fX, fY, false);
    if (pe) {
        pe->Type &= isSpecialMask; // remove special functions (4 lowest bits)

        // onGrid is toggle action -> no other element can be selected
        pe->isSelected = true;
        Doc->elementsOnGrid();

        Doc->sizeOfAll(Doc->UsedX1, Doc->UsedY1, Doc->UsedX2, Doc->UsedY2);
        // Update matching wire label highlighting
        Doc->highlightWireLabels();
        Doc->viewport()->update();
        drawn = false;
    }
}

// -----------------------------------------------------------
void MouseActions::MPressMoveText(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    MAx1 = int(fX);
    MAy1 = int(fY);
    focusElement = Doc->selectCompText(MAx1, MAy1, MAx2, MAy2);

    if (focusElement) {
        MAx3 = MAx1;
        MAy3 = MAy1;
        MAx1 = ((Component *) focusElement)->cx + ((Component *) focusElement)->tx;
        MAy1 = ((Component *) focusElement)->cy + ((Component *) focusElement)->ty;
        Doc->viewport()->update();
        drawn = false;
        QucsMain->MouseMoveAction = &MouseActions::MMoveMoveText;
        QucsMain->MouseReleaseAction = &MouseActions::MReleaseMoveText;
        Doc->grabKeyboard(); // no keyboard inputs during move actions
    }
}

// -----------------------------------------------------------
void MouseActions::MPressZoomIn(Schematic *Doc, QMouseEvent *, float fX, float fY)
{
    qDebug() << "zoom into box";
    MAx1 = int(fX);
    MAy1 = int(fY);
    MAx2 = 0; // rectangle size
    MAy2 = 0;

    QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseZoomIn;
    Doc->grabKeyboard(); // no keyboard inputs during move actions
    Doc->viewport()->update();
    drawn = false;
}

// ***********************************************************************
// **********                                                   **********
// **********    Functions for serving mouse button releasing   **********
// **********                                                   **********
// ***********************************************************************
void MouseActions::MReleaseSelect(Schematic *Doc, QMouseEvent *Event)
{
    bool ctrl;
    if (Event->modifiers().testFlag(Qt::ControlModifier))
        ctrl = true;
    else
        ctrl = false;

    if (!ctrl)
        Doc->deselectElements(focusElement);

    if (focusElement)
        if (Event->button() == Qt::LeftButton)
            if (focusElement->Type == isWire) {
                Doc->selectWireLine(focusElement, ((Wire *) focusElement)->Port1, ctrl);
                Doc->selectWireLine(focusElement, ((Wire *) focusElement)->Port2, ctrl);
            }

    Doc->releaseKeyboard(); // allow keyboard inputs again
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    QucsMain->MouseMoveAction = 0; // no element moving
    Doc->highlightWireLabels();
    Doc->viewport()->update();
    drawn = false;
}

// -----------------------------------------------------------
// Is called after the rectangle for selection is released.
void MouseActions::MReleaseSelect2(Schematic *Doc, QMouseEvent *Event)
{
    if (Event->button() != Qt::LeftButton)
        return;

    bool IsCtrl = Event->modifiers().testFlag(Qt::ControlModifier);
    bool IsShift = Event->modifiers().testFlag(Qt::ShiftModifier);

    // selects all elements within the rectangle
    Doc->selectElements(MAx1, MAy1, MAx1 + MAx2, MAy1 + MAy2, IsCtrl, !IsShift);

    Doc->releaseKeyboard(); // allow keyboard inputs again
    QucsMain->MouseMoveAction = 0;
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    Doc->highlightWireLabels();
    Doc->PostedPaintEvents.clear();
    Doc->viewport()->update();
    drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MReleaseActivate(Schematic *Doc, QMouseEvent *Event)
{
    if (Event->button() != Qt::LeftButton)
        return;

    // activates all components within the rectangle
    Doc->activateCompsWithinRect(MAx1, MAy1, MAx1 + MAx2, MAy1 + MAy2);

    QucsMain->MouseMoveAction = &MouseActions::MMoveActivate;
    QucsMain->MousePressAction = &MouseActions::MPressActivate;
    QucsMain->MouseReleaseAction = 0;
    QucsMain->MouseDoubleClickAction = 0;
    Doc->highlightWireLabels();
    Doc->viewport()->update();
    drawn = false;
}

// -----------------------------------------------------------
// Is called after moving selected elements.
void MouseActions::MReleaseMoving(Schematic *Doc, QMouseEvent *)
{
    // Allow all mouse buttons, because for others than the left one,
    // a menu has already created.
    endElementMoving(Doc, &movingElements);
    Doc->releaseKeyboard(); // allow keyboard inputs again

    QucsMain->MouseMoveAction = nullptr;
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
}

// -----------------------------------------------------------
void MouseActions::MReleaseResizeDiagram(Schematic *Doc, QMouseEvent *Event)
{
    if (Event->button() != Qt::LeftButton)
        return;

    MAx3 = focusElement->cx;
    MAy3 = focusElement->cy;
    if (MAx2 < 0) { // resize diagram
        if (MAx2 > -10)
            MAx2 = -10; // not smaller than 10 pixels
        focusElement->x2 = -MAx2;
        focusElement->cx = MAx1 + MAx2;
    } else {
        if (MAx2 < 10)
            MAx2 = 10;
        focusElement->x2 = MAx2;
        focusElement->cx = MAx1;
    }
    if (MAy2 < 0) {
        if (MAy2 > -10)
            MAy2 = -10;
        focusElement->y2 = -MAy2;
        focusElement->cy = MAy1;
    } else {
        if (MAy2 < 10)
            MAy2 = 10;
        focusElement->y2 = MAy2;
        focusElement->cy = MAy1 + MAy2;
    }
    MAx3 -= focusElement->cx;
    MAy3 -= focusElement->cy;

    Diagram *pd = (Diagram *) focusElement;
    pd->updateGraphData();
    for (Graph *pg : pd->Graphs)
        for (Marker *pm : pg->Markers) {
            pm->x1 += MAx3; // correct changes due to move of diagram corner
            pm->y1 += MAy3;
        }

    int x1, x2, y1, y2;
    pd->Bounding(x1, x2, y1, y2);
    Doc->enlargeView(x1, x2, y1, y2);

    QucsMain->MouseMoveAction = nullptr;
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    Doc->releaseKeyboard(); // allow keyboard inputs again

    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MReleaseResizePainting(Schematic *Doc, QMouseEvent *Event)
{
    if (Event->button() != Qt::LeftButton)
        return;

    QucsMain->MouseMoveAction = nullptr;
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    Doc->releaseKeyboard(); // allow keyboard inputs again

    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MReleaseSetLimits(Schematic *Doc, QMouseEvent *Event)
{
    Doc->releaseKeyboard();

    // TODO: Make a point version of DOC_n_POS.
    auto inModel = Doc->contentsToModel(Event->pos());
    MAx2 = inModel.x();
    MAy2 = inModel.y();

    qDebug() << "Mouse released after setting limits.";
    // Check to see if the mouse is within a diagram using the oddly named "getSelected".
    for (Diagram* diagram = Doc->Diagrams->last(); diagram != 0; diagram = Doc->Diagrams->prev()) {
        
        // Only process the selection if it ends in a diagram, and is the same diagram as start.
        if (diagram->getSelected(MAx2, MAy2) && diagram == pActiveDiagram) {
            qDebug() << "In the same diagram, setting limits";
            
            mouseUpPoint = QPointF(MAx2 - diagram->cx, diagram->cy - MAy2);

            // Normalise the selection in case user starts at bottom and/or right.
            QRectF select = QRectF(mouseDownPoint, mouseUpPoint).normalized();      

            // Only process if there is a valid selection box.
            if (select.width() < MIN_SELECT_SIZE || select.height() < MIN_SELECT_SIZE)
                break;

            diagram->setLimitsBySelectionRect(select);

            // TODO: Consider refactoring loadGraphData to reload the current dataset if an empty string is passed.
            QFileInfo Info(Doc->DocName);
            QString defaultDataSet = Info.absolutePath() + QDir::separator() + Doc->DataSet;
            diagram->loadGraphData(defaultDataSet);

            Doc->setChanged(true, true);

            // No need to keep searching.
            break;
        }
    }

    // Stay in set limits and allow user to choose a new start point.
    QucsMain->MouseMoveAction = &MouseActions::MMoveSetLimits;
    QucsMain->MouseReleaseAction = nullptr;
    Doc->viewport()->update();
}

// -----------------------------------------------------------
void MouseActions::paintElementsScheme(Schematic *p)
{
    Element *pe;
    for (pe = movingElements.first(); pe != nullptr; pe = movingElements.next())
        pe->paintScheme(p);
}

// -----------------------------------------------------------
void MouseActions::moveElements(Schematic *Doc, int &x1, int &y1)
{
    Element *pe;
    Doc->setOnGrid(x1, y1);

    for (pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
        if (pe->Type & isLabel) {
            pe->cx += x1;
            pe->x1 += x1;
            pe->cy += y1;
            pe->y1 += y1;
        } else
            pe->setCenter(x1, y1, true);
    }
}

// -----------------------------------------------------------
void MouseActions::rotateElements(Schematic *Doc, int &x1, int &y1)
{
    int x2;
    Element *pe;
    Doc->setOnGrid(x1, y1);

    for (pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
        switch (pe->Type) {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            ((Component *) pe)->rotate(); // rotate !before! rotating the center
            x2 = x1 - pe->cx;
            pe->setCenter(pe->cy - y1 + x1, x2 + y1);
            break;
        case isWire:
            x2 = pe->x1;
            pe->x1 = pe->y1 - y1 + x1;
            pe->y1 = x1 - x2 + y1;
            x2 = pe->x2;
            pe->x2 = pe->y2 - y1 + x1;
            pe->y2 = x1 - x2 + y1;
            break;
        case isPainting:
            ((Painting *) pe)->rotate(x1, y1); // rotate around the center x1 y1
            break;
        default:
            x2 = x1 - pe->cx; // if diagram -> only rotate cx/cy
            pe->setCenter(pe->cy - y1 + x1, x2 + y1);
            break;
        }
    }
}

// -----------------------------------------------------------
void MouseActions::MReleasePaste(Schematic *Doc, QMouseEvent *Event)
{
    int x1, y1, x2, y2, rot;
    QFileInfo Info(Doc->DocName);
    //QPainter painter(Doc->viewport());

    Element *pe;
    switch (Event->button()) {
    case Qt::LeftButton:
        // insert all moved elements into document
        for (pe = movingElements.first(); pe != 0; pe = movingElements.next()) {
            pe->isSelected = false;
            switch (pe->Type) {
            case isWire:
                if (pe->x1 == pe->x2)
                    if (pe->y1 == pe->y2)
                        break;
                Doc->insertWire((Wire *) pe);
                if (Doc->Wires->containsRef((Wire *) pe))
                    Doc->enlargeView(pe->x1, pe->y1, pe->x2, pe->y2);
                else
                    pe = NULL;
                break;
            case isDiagram:
                Doc->Diagrams->append((Diagram *) pe);
                ((Diagram *) pe)
                    ->loadGraphData(Info.absolutePath() + QDir::separator() + Doc->DataSet);
                Doc->enlargeView(pe->cx, pe->cy - pe->y2, pe->cx + pe->x2, pe->cy);
                break;
            case isPainting:
                Doc->Paintings->append((Painting *) pe);
                ((Painting *) pe)->Bounding(x1, y1, x2, y2);
                Doc->enlargeView(x1, y1, x2, y2);
                break;
            case isMovingLabel:
                pe->Type = isNodeLabel;
                Doc->placeNodeLabel((WireLabel *) pe);
                break;
            case isComponent:
            case isAnalogComponent:
            case isDigitalComponent:
                Doc->insertComponent((Component *) pe);
                ((Component *) pe)->entireBounds(x1, y1, x2, y2);
                Doc->enlargeView(x1, y1, x2, y2);
                break;
            }
        }

        pasteElements(Doc);
        // keep rotation sticky for pasted elements
        rot = movingRotated;
        x1 = y1 = 0;
        while (rot--)
            rotateElements(Doc, x1, y1);

        QucsMain->MouseMoveAction = &MouseActions::MMovePaste;
        QucsMain->MousePressAction = 0;
        QucsMain->MouseReleaseAction = 0;
        QucsMain->MouseDoubleClickAction = 0;

        drawn = false;
        Doc->viewport()->update();
        Doc->setChanged(true, true);
        break;

    // ............................................................
    case Qt::RightButton: {// right button rotates the elements
        //setPainter(Doc, &painter);

        if (drawn) // erase old scheme
            paintElementsScheme(Doc);
        drawn = true;

        auto inModel = Doc->contentsToModel(Event->pos());
        x1 = inModel.x();
        y1 = inModel.y();
        rotateElements(Doc, x1, y1);
        paintElementsScheme(Doc);
        // save rotation
        movingRotated++;
        movingRotated &= 3;
        break;
    }

    default:; // avoids compiler warnings
    }
}

// -----------------------------------------------------------
void MouseActions::MReleaseMoveText(Schematic *Doc, QMouseEvent *Event)
{
    if (Event->button() != Qt::LeftButton)
        return;

    QucsMain->MouseMoveAction = &MouseActions::MMoveMoveTextB;
    QucsMain->MouseReleaseAction = 0;
    Doc->releaseKeyboard(); // allow keyboard inputs again

    ((Component *) focusElement)->tx = MAx1 - ((Component *) focusElement)->cx;
    ((Component *) focusElement)->ty = MAy1 - ((Component *) focusElement)->cy;
    Doc->viewport()->update();
    drawn = false;
    Doc->setChanged(true, true);
}

// -----------------------------------------------------------
void MouseActions::MReleaseZoomIn(Schematic *Doc, QMouseEvent *Event)
{
    if (Event->button() != Qt::LeftButton)
        return;

    MAx1 = Event->pos().x();
    MAy1 = Event->pos().y();


    const QPoint click{Event->pos().x() , Event->pos().y()};
    // "false" = "coordinates are not relative to viewport"
    Doc->zoomAroundPoint(1.5, click, false);

    QucsMain->MouseMoveAction = &MouseActions::MMoveZoomIn;
    QucsMain->MouseReleaseAction = 0;
    Doc->releaseKeyboard(); // allow keyboard inputs again
}

// ***********************************************************************
// **********                                                   **********
// **********    Functions for mouse button double clicking     **********
// **********                                                   **********
// ***********************************************************************
void MouseActions::editElement(Schematic *Doc, QMouseEvent *Event)
{
    //    qDebug() << "+double click, editElement";

    if (focusElement == 0)
        return;

    //  qDebug() << "+focusElement->Type" << focusElement->Type;

    Graph *pg;
    Component *c;
    Diagram *dia;
    DiagramDialog *ddia;
    MarkerDialog *mdia;
    int x1, y1, x2, y2;

    QFileInfo Info(Doc->DocName);
    auto inModel = Doc->contentsToModel(Event->pos());
    float fX = static_cast<float>(inModel.x());
    float fY = static_cast<float>(inModel.y());

    switch (focusElement->Type) {
    case isComponent:
    case isAnalogComponent:
    case isDigitalComponent:
        c = (Component *) focusElement;
        //         qDebug() << "cast focusElement into" << c->Name;
        if (c->Model == "GND")
            return;

        if ((c->Model == ".CUSTOMSIM") || (c->Model == ".XYCESCR") || (c->Model == "INCLSCR")) {
            CustomSimDialog *sd = new CustomSimDialog((SpiceCustomSim *) c, Doc);
            if (sd->exec() != 1)
                break; // dialog is WDestructiveClose
        } else if (c->Model == "SPICE") {
            SpiceDialog *sd = new SpiceDialog(App, (SpiceFile *) c, Doc);
            if (sd->exec() != 1)
                break; // dialog is WDestructiveClose
        } else if (c->Model == ".Opt") {
            OptimizeDialog *od = new OptimizeDialog((Optimize_Sim *) c, Doc);
            if (od->exec() != 1)
                break; // dialog is WDestructiveClose
        } else if (c->Model == "SPICEINIT") {
            TextBoxDialog *od = new TextBoxDialog("Edit .spiceinit configuration", c, Doc);
            if (od->exec() != 1)
                break; // dialog is WDestructiveClose
        } else {
            ComponentDialog *cd = new ComponentDialog(c, Doc);
            if (cd->exec() != 1)
                break; // dialog is WDestructiveClose

            Doc->Components->findRef(c);
            Doc->Components->take();
            Doc->setComponentNumber(c); // for ports/power sources
            Doc->Components->append(c);
        }

        Doc->setChanged(true, true);
        c->entireBounds(x1, y1, x2, y2);
        Doc->enlargeView(x1, y1, x2, y2);
        break;

    case isDiagram:
        dia = (Diagram *) focusElement;
        if (dia->Name.at(0) == 'T') { // don't open dialog on scrollbar
            if (dia->Name == "Time") {
                if (dia->cy < int(fY)) {
                    if (((TimingDiagram *) focusElement)->scroll(MAx1))
                        Doc->setChanged(true, true, 'm'); // 'm' = only the first time
                    break;
                }
            } else {
                if (dia->cx > int(fX)) {
                    if (((TabDiagram *) focusElement)->scroll(MAy1))
                        Doc->setChanged(true, true, 'm'); // 'm' = only the first time
                    break;
                }
            }
        }

        ddia = new DiagramDialog(dia, Doc);
        if (ddia->exec() != QDialog::Rejected) // is WDestructiveClose
            Doc->setChanged(true, true);

        dia->Bounding(x1, x2, y1, y2);
        Doc->enlargeView(x1, x2, y1, y2);
        break;

    case isGraph:
        pg = (Graph *) focusElement;
        // searching diagram for this graph
        for (dia = Doc->Diagrams->last(); dia != 0; dia = Doc->Diagrams->prev())
            if (dia->Graphs.indexOf(pg) >= 0)
                break;
        if (!dia)
            break;

        ddia = new DiagramDialog(dia, Doc, pg);
        if (ddia->exec() != QDialog::Rejected) // is WDestructiveClose
            Doc->setChanged(true, true);
        break;

    case isWire:
        MPressLabel(Doc, Event, fX, fY);
        break;

    case isNodeLabel:
    case isHWireLabel:
    case isVWireLabel:
        editLabel(Doc, (WireLabel *) focusElement);
        // update highlighting, labels may have changed
        Doc->highlightWireLabels();
        break;

    case isPainting:
        if (((Painting *) focusElement)->Dialog(Doc))
            Doc->setChanged(true, true);
        break;

    case isMarker:
        mdia = new MarkerDialog((Marker *) focusElement, Doc);
        if (mdia->exec() > 1)
            Doc->setChanged(true, true);
        break;
    }

    // Very strange: Now an open VHDL editor gets all the keyboard input !?!
    // I don't know why it only happens here, nor am I sure whether it only
    // happens here. Anyway, I hope the best and give the focus back to the
    // current document.
    Doc->setFocus();

    Doc->viewport()->update();
    drawn = false;
}

// -----------------------------------------------------------
void MouseActions::MDoubleClickSelect(Schematic *Doc, QMouseEvent *Event)
{
    Doc->releaseKeyboard(); // allow keyboard inputs again
    QucsMain->editText->setHidden(true);
    editElement(Doc, Event);
}

/**
 * @brief MouseActions::MDoubleClickWire2  Double click terminates wire insertion.
 * @param Doc
 * @param Event
 */
void MouseActions::MDoubleClickWire2(Schematic *Doc, QMouseEvent *Event)
{
    auto inModel = Doc->contentsToModel(Event->pos());
    MPressWire2(Doc, Event, static_cast<float>(inModel.x()), static_cast<float>(inModel.y()));

    if (formerAction)
        QucsMain->select->setChecked(true); // restore old action
    else {
        QucsMain->MouseMoveAction = &MouseActions::MMoveWire1;
        QucsMain->MousePressAction = &MouseActions::MPressWire1;
        QucsMain->MouseDoubleClickAction = 0;
    }
}

void MouseActions::MPressTune(Schematic *Doc, QMouseEvent *Event, float fX, float fY)
{
    Q_UNUSED(Event);
    int No = 0;
    MAx1 = int(fX);
    MAy1 = int(fY);
    focusElement = Doc->selectElement(fX, fY, false, &No);
    isMoveEqual = false; // moving not neccessarily square

    if (focusElement)
        // print define value in hex, see element.h
        qDebug() << "MPressTune: focusElement->Type"
                 << QString("0x%1").arg(focusElement->Type, 0, 16);
    else
        qDebug() << "MPressTune";

    if (focusElement && App->TuningMode) {
        Component *pc = nullptr;
        Property *pp = nullptr;
        switch (focusElement->Type) {
        case isComponent: // pick first property if device is clicked (i.e. R,C)
        case isAnalogComponent:
            pc = (Component *) focusElement;
            if (!pc)
                return;
            if (pc->Props.isEmpty())
                return;
            pp = pc->Props.at(0);
            break;
        case isComponentText: // property text of component ?
            focusElement->Type &= (~isComponentText) | isComponent;
            pc = (Component *) focusElement;
            pp = 0;
            if (!pc)
                return; // should never happen

            // current property
            if (No > 0) {
                No--; // No counts also the on/screen component Name, so subtract 1 to get the actual property number
                pp = pc->Props.at(No);
            }
            if (!pp)
                return; // should not happen
            break;
        default:
            return;
        }
        if (pc == nullptr || pp == nullptr)
            return;
        if (!App->tunerDia->containsProperty(pp)) {
            if (isPropertyTunable(pc, pp)) {
                tunerElement *tune = new tunerElement(App->tunerDia, pc, pp, No);
                tune->schematicName = Doc->DocName;
                if (tune != NULL)
                    App->tunerDia->addTunerElement(tune); //Tunable property
            } else {
                QMessageBox::warning(nullptr,
                                     "Property not correct",
                                     "You selected a non-tunable property",
                                     QMessageBox::Ok);
                return;
            }
        }
    }
}

// vim:ts=8:sw=2:noet
