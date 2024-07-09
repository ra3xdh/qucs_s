/***************************************************************************
                               schematic.h
                              -------------
    begin                : Sat Mar 11 2006
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

#ifndef SCHEMATIC_H
#define SCHEMATIC_H

// maybe in another place...
#ifdef NDEBUG
// cast without overhead
#  define prechecked_cast static_cast
#else
// cast safely, for debugging purposes
#  define prechecked_cast dynamic_cast
#endif

#include "wire.h"
#include "node.h"
#include "qucsdoc.h"
#include "diagrams/diagram.h"
#include "paintings/painting.h"
#include "components/component.h"

#include "qt3_compat/qt_compat.h"
#include "qt3_compat/q3scrollview.h"
#include <QVector>
#include <QStringList>
#include <QFileInfo>

class QTextStream;
class QTextEdit;
class QPlainTextEdit;
class QDragMoveEvent;
class QDropEvent;
class QDragLeaveEvent;
class QWheelEvent;
class QMouseEvent;
class QDragEnterEvent;
class QPainter;

// digital signal data
struct DigSignal {
  DigSignal() { Name=""; Type=""; }
  DigSignal(const QString& _Name, const QString& _Type = "")
    : Name(_Name), Type(_Type) {}
  QString Name; // name
  QString Type; // type of signal
};
typedef QMap<QString, DigSignal> DigMap;
typedef enum {_NotRop, _Rect, _SelectionRect, _Line, _Ellipse, _Arc, _DotLine, _DotRect, _Translate, _Scale} PE;
typedef struct {PE pe; int x1; int y1;int x2;int y2;int a; int b; bool PaintOnViewport;}PostedPaintEvent;

// subcircuit, vhdl, etc. file structure
struct SubFile {
  SubFile() { Type=""; File=""; PortTypes.clear(); }
  SubFile(const QString& _Type, const QString& _File)
    : Type(_Type), File(_File) { PortTypes.clear(); }
  QString Type;          // type of file
  QString File;          // file name identifier
  QStringList PortTypes; // data types of in/out signals
};
typedef QMap<QString, SubFile> SubMap;

class Schematic : public Q3ScrollView, public QucsDoc {
  Q_OBJECT
public:
  Schematic(QucsApp*, const QString&);
 ~Schematic();

  void setName(const QString&);
  void setChanged(bool, bool fillStack=false, char Op='*');
  void print(QPrinter*, QPainter*, bool printAll, bool fitToPage, QMargins margins={});

  void paintSchToViewpainter(QPainter* painter, bool printAll);

  void PostPaintEvent(PE pe, int x1=0, int y1=0, int x2=0, int y2=0, int a=0, int b=0,bool PaintOnViewport=false);

  float textCorr();
  bool sizeOfFrame(int&, int&);

  /**
    Iterates over all elements of schematic to find the size of the smallest
    rectangle able to fit all elements. The bounds are then stored in members
    @c UsedX1, @c UsedY1, @c UsedX2, @c UsedY2 , i.e. the internal state
    of schematic is updated.
  */
  void updateAllBoundingRect();

  /**
    Returns the smallest rectangle enclosing all elements of schematic
  */
  QRect allBoundingRect();
  QRect  sizeOfSelection() const;
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  QPoint setOnGrid(const QPoint& p);
  void  setOnGrid(int&, int&);
  bool  elementsOnGrid();

  /**
    Zoom around a "zooming center". Zooming center is a point on the canvas,
    which doesn't move relative to a viewport while canvas is being zoomed in or
    out.

    This produces the effect of "concentrating" on a zooming center: with each
    zoom-in step one would get closer and closer to the point, while point's
    surroundings would go "out of sight", beyound the viewport's borders.

    Zooming out works in backwards order: everything is like being "sucked into"
    the zooming center.

    If param \c viewportRelative is \c true then the given coordinates are
    treated as relative to the viewport top-left corner. Otherwise they're
    considered to be absolute i.e. relative to canvas top-left corner.

    @param scaleChange       a multiplier for a current scale value
    @param coords            coordinates of the "zooming center"
    @param viewportRelative  tells if coordinates are absolute or relative to viewport
  */
  void zoomAroundPoint(double scaleChange, QPoint coords, bool viewportRelative);
  float zoomBy(float);
  void  showAll();
  void zoomToSelection();
  void  showNoZoom();
  void  enlargeView(int, int, int, int);
  void  switchPaintMode();
  int   adjustPortNumbers();
  int   orderSymbolPorts();
  void  reloadGraphs();
  bool  createSubcircuitSymbol();

  /**
    @brief Given cordinates of a model point returns coordinates of this point
           relative to viewport. It's a reverse of @ref Schematic::viewportToModel
  */
  QPoint modelToViewport(QPoint modelCoordinates);

  /**
    Given a coordinates of viewport point returns coordinates of the model plane point
    displayed at given location of the viewport.
  */
  QPoint viewportToModel(QPoint viewportCoordinates);

  /**
    Given coordinates of a point on the view plane (schematic's canvas), this method
    returns coordinates of a corresponding point on the model plane.

    @param viewCoordinates a point on the view plane
    @return a corresponding point on the model plane
  */
  QPoint contentsToModel(const QPoint& viewCoordinates);

  /**
    Given coordinates of a point on the model plane, this method returns coordinates
    of a corresponding point on the view plane (schematic's canvas).

    @param modelCoordinates a point on the model plane
    @return a corresponding point on the view plane
  */
  QPoint modelToContents(const QPoint& modelCoordinates);

  void    cut();
  void    copy();
  bool    paste(QTextStream*, Q3PtrList<Element>*);
  bool    load();
  int     save();
  int     saveSymbolCpp (void);
  int     saveSymbolJSON (void);
  int     savePropsJSON (void);
  void    becomeCurrent(bool);
  bool    undo();
  bool    redo();

  void scrollUp(int);
  void scrollDown(int);
  void scrollLeft(int);
  void scrollRight(int);

  bool checkDplAndDatNames();

  // The pointers points to the current lists, either to the schematic
  // elements "Doc..." or to the symbol elements "SymbolPaints".
  std::list<std::shared_ptr<Wire>>      *Wires, DocWires;
  std::list<std::shared_ptr<Node>>      *Nodes, DocNodes;
  std::list<std::shared_ptr<Diagram>>   *Diagrams, DocDiags;
  std::list<std::shared_ptr<Painting>>  *Paintings, DocPaints;
  std::list<std::shared_ptr<Component>> *Components, DocComps;

  std::list<std::shared_ptr<Painting>>  SymbolPaints;  // symbol definition for subcircuit

  std::list<std::shared_ptr<PostedPaintEvent>>  PostedPaintEvents;
  bool symbolMode;  // true if in symbol painting mode
  bool isSymbolOnly;

  // Horizontal and vertical grid step
  int GridX, GridY;

  // Variables View* are the coordinates of top-level and bottom-right corners
  // of a rectangle representing the schematic "model". This
  // rectangle may grow and shrink when user scrolls the view, and its
  // coordinates change accordingly. Everything (elements, wires, etc.) lies
  // inside this rectangle. The size of this rectangle is the "logical" size
  // of the schematic. The comment in "renderModel" method describes how
  // these variables ("model") is used to draw the scematic.
  int ViewX1, ViewY1, ViewX2, ViewY2;

  int showFrame;
  QString Frame_Text0, Frame_Text1, Frame_Text2, Frame_Text3;

  // Two of those data sets are needed for Schematic and for symbol.
  // Which one is in "tmp..." depends on "symbolMode".
  float tmpScale;
  int tmpViewX1, tmpViewY1, tmpViewX2, tmpViewY2;
  int tmpUsedX1, tmpUsedY1, tmpUsedX2, tmpUsedY2;

  int undoActionIdx;
  QVector<QString *> undoAction;
  int undoSymbolIdx;
  QVector<QString *> undoSymbol;    // undo stack for circuit symbol

  /*! \brief Get (schematic) file reference */
  QFileInfo getFileInfo (void) { return FileInfo; }
  /*! \brief Set reference to file (schematic) */
  void setFileInfo(QString FileName) { FileInfo = QFileInfo(FileName); }

signals:
  void signalCursorPosChanged(int, int, QString);
  void signalUndoState(bool);
  void signalRedoState(bool);
  void signalFileChanged(bool);
  void signalComponentDeleted(Component *);

protected:
  // overloaded function to get actions of user
  void drawContents(QPainter*, int, int, int, int)override;
  void contentsMouseMoveEvent(QMouseEvent*)override;
  void contentsMousePressEvent(QMouseEvent*)override;
  void contentsMouseDoubleClickEvent(QMouseEvent*)override;
  void contentsMouseReleaseEvent(QMouseEvent*)override;
  void contentsWheelEvent(QWheelEvent*)override;
  void contentsDropEvent(QDropEvent*)override;
  void contentsDragEnterEvent(QDragEnterEvent*)override;
  void contentsDragLeaveEvent(QDragLeaveEvent*)override;
  void contentsDragMoveEvent(QDragMoveEvent*)override;
  void contentsNativeGestureZoomEvent( QNativeGestureEvent* ) override;

protected slots:
  void slotScrollUp();
  void slotScrollDown();
  void slotScrollLeft();
  void slotScrollRight();

private:
  // Variables Used* hold the coordinates of top-left and bottom-right corners
  // of a smallest rectangle which can fit all elements of the schematic.
  // This rectangle exists in the same coordinate system as View*-rectangle
  int UsedX1, UsedY1, UsedX2, UsedY2;

  void  sizeOfAll(int&, int&, int&, int&);

  // Viewport-realative coordinates of the cursor between mouse movements.
  // Used in "pan with mouse" feature.
  QPoint previousCursorPosition;

  bool dragIsOkay;
  /*! \brief hold system-independent information about a schematic file */
  QFileInfo FileInfo;

  /**
    Minimum scale at which schematic could be drawn.
  */
  static constexpr double minScale = 0.1;

  /**
    Maximum scale at which schematic could be drawn.
  */
  static constexpr double maxScale = 10.0;

  /**
    Returns a rectangle which describes the model plane of the schematic.
    The rectangle is a copy, changes made to it do not affect schematic state.
  */
  QRect modelRect();

  /**
    Returns a rectangle which describes the viewport. Top-left corner is (0,0),
    width and height are equal to viewport's width and height.
  */
  QRect viewportRect();

  /**
    If given value violates lower or upper scale limit, then returns
    the limit value, original value otherwise.
  */
  static double clipScale(double);

  /**
    Tells whether the model should be rerendered. Model should be rendered
    if the given scale differs from the current one or if the point displayed
    at \a viewportCoords in the viewport differs from the point \a modelCoords.
    Otherwise there is no changes and no need to rerender.

    @param scale desired scale
    @param newModelBounds a rectangle describing the desired model bounds
    @param modelCoords coordinates of a point on the model plane
    @param viewportCoords coordinates of a point in the viewport.
  */
  bool shouldRender(const double& scale, const QRect& newModelBounds, const QPoint& modelCoords, const QPoint& viewportCoords);

  /**
    Renders schematic model on Q3ScrollView's contents at a given scale,
    and positions the contents so that the point \a modelPlaneCoords of the
    model is displayed at location \a viewportCoords of the viewport.

    There is no need to call "update" on Q3ScrollView after using this method.
    It is done as a part of rendering process.

    Usage examples:
    1. Imagine you want to handle user's right scroll and you want scrolling
       to be infinite. Each scroll has to "stretch" schematic model to the right.
       First step is to take current model and create a new desired
       model size from it by shifting its right bound.
       After scrolling you want rightmost point of the model to be diplayed
       at right bound of the viewport. Then second step is to take coordinates
       of top-right corner of @b new @b desired model and coordinates of top-rigth
       corner of viewport and pass to @c renderModel along with new model:
       @code
       renderModel(sameScale, newModel, newModel.topRight(), viewportRect().topRight());
       @endcode
    2. Suppose you want to zoom at some element, so that its center would be
       displayed at the center of the viewport after zooming.
       First, find coordinates of the element center
       Second, find coordinates of the viewport center
       Third, call @c renderModel:
       @code
       renderModel(zoomScale, sameModel, elementCenter, viewportRect().center());
       @endcode
    3. Imagine you want to scroll and zoom so that the point currently
       displayed at the center of the viewport would be at the viewport
       top-left corner after.
       @code
       renderModel(zoomScale, sameModel, viewportToModel(viewportRect().center()), viewportRect.topLeft());
       @endcode
    @param  scale            desired new scale. It is clipped when exceeds
                             a lower or upperlimit
    @param  newModelBounds   a rectangle describing the desired model bounds
    @param  modelPlaneCoords coordinates of a point somewhere within
                             \a newModelBounds
    @param  viewportCoords   coordinates of the point on the viewport where
                             \a modelPlaneCoords should be placed after rendering
    @return new scale value
  */
  double renderModel(double scale, QRect newModelBounds, QPoint modelPlaneCoords, QPoint viewportCoords);
  void drawElements(QPainter* painter);
  void drawDcBiasPoints(QPainter* painter);
  void drawPostPaintEvents(QPainter* painter);
  void paintFrame(QPainter* painter);
  void drawGrid(QPainter* painter);

/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_element.cpp". They only access the QPtrList  *****
   *****  pointers "Wires", "Nodes", "Diagrams", "Paintings" and  *****
   *****  "Components".                                           *****
   ******************************************************************** */

public:
  Node* insertNode(int, int, Element*);
  Node* selectedNode(int, int);

  int   insertWireNode1(Wire*);
  bool  connectHWires1(Wire*);
  bool  connectVWires1(Wire*);
  int   insertWireNode2(Wire*);
  bool  connectHWires2(Wire*);
  bool  connectVWires2(Wire*);
  int   insertWire(Wire*);
  void  selectWireLine(Element*, Node*, bool);
  Wire* selectedWire(int, int);
  Wire* splitWire(Wire*, Node*);
  bool  oneTwoWires(Node*);
  void  deleteWire(Wire*);

  Marker* setMarker(int, int);
  void    markerLeftRight(bool, Q3PtrList<Element>*);
  void    markerUpDown(bool, Q3PtrList<Element>*);

  Element* selectElement(float, float, bool, int *index=0);
  void     deselectElements(Element*) const;
  int      selectElements(int, int, int, int, bool, bool) const;
  void     selectMarkers() const;
  void     newMovingWires(Q3PtrList<Element>*, Node*, int) const;
  int      copySelectedElements(Q3PtrList<Element>*);
  bool     deleteElements();
  bool     aligning(int);
  bool     distributeHorizontal();
  bool     distributeVertical();

  void       setComponentNumber(Component*);
  void       insertRawComponent(Component*, bool noOptimize=true);
  void       recreateComponent(const std::shared_ptr<Component> &);
  void       insertComponent(Component*);
  void       activateCompsWithinRect(int, int, int, int);
  bool       activateSpecifiedComponent(int, int);
  bool       activateSelectedComponents();
  void       setCompPorts(Component*);
  Component* selectCompText(int, int, int&, int&) const;
  Component* searchSelSubcircuit();
  Component* selectedComponent(int, int);
  void       deleteComp(Component*);
  Component* getComponentByName(const QString& compname) const;

  void     oneLabel(Node*);
  int      placeNodeLabel(WireLabel*);
  Element* getWireLabel(Node*);
  void     insertNodeLabel(WireLabel*);
  void     copyLabels(int&, int&, int&, int&, QList<Element *> *);

  Painting* selectedPainting(float, float);
  void      copyPaintings(int&, int&, int&, int&, QList<Element *> *);


private:
  void insertComponentNodes(Component*, bool);
  int  copyWires(int&, int&, int&, int&, QList<Element *> *);
  int  copyComponents(int&, int&, int&, int&, QList<Element *> *);
  void copyComponents2(int&, int&, int&, int&, QList<Element *> *);
  bool copyComps2WiresPaints(int&, int&, int&, int&, QList<Element *> *);
  int  copyElements(int&, int&, int&, int&, QList<Element *> *);


/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_file.cpp". They only access the QPtrLists    *****
   *****  and their pointers. ("DocComps", "Components" etc.)     *****
   ******************************************************************** */

public:
  static int testFile(const QString &);
  bool createLibNetlist(QTextStream*, QPlainTextEdit*, int);
  bool createSubNetlist(QTextStream *, int&, QStringList&, QPlainTextEdit*, int);
  void createSubNetlistPlain(QTextStream*, QPlainTextEdit*, int);
  int  prepareNetlist(QTextStream&, QStringList&, QPlainTextEdit*);
  QString createNetlist(QTextStream&, int);
  bool isDigitalCircuit();
  bool loadDocument();
  void highlightWireLabels (void);
  void clearSignalsAndFileList();
  void clearSignals();

private:
  int  saveDocument();

  bool loadProperties(QTextStream*);
  void simpleInsertComponent(Component*);
  bool loadComponents(QTextStream*, Q3PtrList<Component> *List=0);
  void simpleInsertWire(Wire*);
  bool loadWires(QTextStream*, Q3PtrList<Element> *List=0);
  bool loadDiagrams(QTextStream*, Q3PtrList<Diagram>*);
  bool loadPaintings(QTextStream*, Q3PtrList<Painting>*);
  bool loadIntoNothing(QTextStream*);

  QString createClipboardFile();
  bool    pasteFromClipboard(QTextStream *, Q3PtrList<Element>*);

  QString createUndoString(char);
  bool    rebuild(QString *);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(QString *);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);
  void throughAllNodes(bool, QStringList&, int&);
  void propagateNode(QStringList&, int&, Node*);
  void collectDigitalSignals(void);
  bool giveNodeNames(QTextStream *, int&, QStringList&, QPlainTextEdit*, int);
  void beginNetlistDigital(QTextStream &);
  void endNetlistDigital(QTextStream &);
  bool throughAllComps(QTextStream *, int&, QStringList&, QPlainTextEdit *, int);

  DigMap Signals; // collecting node names for VHDL signal declarations
  QStringList PortTypes;

public:
  bool isAnalog;
  bool isVerilog;
  bool creatingLib;

};

#endif
