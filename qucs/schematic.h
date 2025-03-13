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
#include "wire_planner.h"

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

  struct Selection {
    QRect bounds;
    std::vector<Component*> components;
    std::vector<Wire*> wires;
    std::vector<Painting*> paintings;
    std::vector<Diagram*> diagrams;
    std::vector<WireLabel*> labels;
    std::vector<Marker*> markers;
    std::vector<Node*> nodes;
  };

  Selection  currentSelection() const;
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
  void  enlargeView(const Element* e);
  void  switchPaintMode();
  int   adjustPortNumbers();
  int   orderSymbolPorts();
  void  reloadGraphs();
  bool  createSubcircuitSymbol();

  /**
    @brief Given cordinates of a model point returns coordinates of this point
           relative to viewport. It's a reverse of @ref Schematic::viewportToModel
  */
  QPoint modelToViewport(const QPoint& modelCoordinates);

  /**
    Given a coordinates of viewport point returns coordinates of the model plane point
    displayed at given location of the viewport.
  */
  QPoint viewportToModel(const QPoint& viewportCoordinates);

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
  bool    paste(QTextStream*, QList<Element*>*);
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

  /*! \brief Get (schematic) file reference */
  QFileInfo getFileInfo (void) { return a_FileInfo; }
  /*! \brief Set reference to file (schematic) */
  void setFileInfo(QString FileName) { a_FileInfo = QFileInfo(FileName); }

  QString getFrame_Text0() const { return a_Frame_Text0; }
  void setFrame_Text0(const QString value) { a_Frame_Text0 = value; }
  QString getFrame_Text1() const { return a_Frame_Text1; }
  void setFrame_Text1(const QString value) { a_Frame_Text1 = value; }
  QString getFrame_Text2() const { return a_Frame_Text2; }
  void setFrame_Text2(const QString value) { a_Frame_Text2 = value; }
  QString getFrame_Text3() const { return a_Frame_Text3; }
  void setFrame_Text3(const QString value) { a_Frame_Text3 = value; }
  int getShowFrame() const { return a_showFrame; }
  void setShowFrame(int value) { a_showFrame = value; }
  int getViewX1() const { return a_ViewX1; }
  int getViewY1() const { return a_ViewY1; }
  int getGridX() const { return a_GridX; }
  void setGridX(int value) { a_GridX = value; }
  int getGridY() const { return a_GridY; }
  void setGridY(int value) { a_GridY = value; }
  void setGridColor(const QColor& color) { a_GridColor = color; }
  QColor getGridColor() const { return a_GridColor; }
  bool getSymbolMode() const { return a_symbolMode; }
  void setSymbolMode(bool value) { a_symbolMode = value; }
  bool getIsSymbolOnly() const { return a_isSymbolOnly; }
  void setIsSymbolOnly(bool value) { a_isSymbolOnly = value; }
  void clearPostedPaintEvents() { a_PostedPaintEvents.clear(); }

  // The pointers points to the current lists, either to the schematic
  // elements "Doc..." or to the symbol elements "SymbolPaints".
  Q3PtrList<Wire> *a_Wires;
  Q3PtrList<Wire> a_DocWires;
  Q3PtrList<Node>* a_Nodes;
  Q3PtrList<Node> a_DocNodes;
  Q3PtrList<Diagram>* a_Diagrams;
  Q3PtrList<Diagram> a_DocDiags;
  Q3PtrList<Painting>* a_Paintings;
  Q3PtrList<Painting> a_DocPaints;
  Q3PtrList<Component>* a_Components;
  Q3PtrList<Component> a_DocComps;

  Q3PtrList<Painting> a_SymbolPaints;  // symbol definition for subcircuit

private:
  QList<PostedPaintEvent> a_PostedPaintEvents;

  bool a_symbolMode;  // true if in symbol painting mode
  bool a_isSymbolOnly;

  // Horizontal and vertical grid step, grid color.
  int a_GridX;
  int a_GridY;
  QColor a_GridColor;

  // Variables View* are the coordinates of top-level and bottom-right corners
  // of a rectangle representing the schematic "model". This
  // rectangle may grow and shrink when user scrolls the view, and its
  // coordinates change accordingly. Everything (elements, wires, etc.) lies
  // inside this rectangle. The size of this rectangle is the "logical" size
  // of the schematic. The comment in "renderModel" method describes how
  // these variables ("model") is used to draw the scematic.
  int a_ViewX1;
  int a_ViewY1;
  int a_ViewX2;
  int a_ViewY2;

  int a_showFrame;
  QString a_Frame_Text0;
  QString a_Frame_Text1;
  QString a_Frame_Text2;
  QString a_Frame_Text3;

  // Two of those data sets are needed for Schematic and for symbol.
  // Which one is in "tmp..." depends on "symbolMode".
  float a_tmpScale;
  int a_tmpViewX1;
  int a_tmpViewY1;
  int a_tmpViewX2;
  int a_tmpViewY2;
  int a_tmpUsedX1;
  int a_tmpUsedY1;
  int a_tmpUsedX2;
  int a_tmpUsedY2;

  int a_undoActionIdx;
  QVector<QString *> a_undoAction;
  int a_undoSymbolIdx;
  QVector<QString *> a_undoSymbol;    // undo stack for circuit symbol

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
  int a_UsedX1;
  int a_UsedY1;
  int a_UsedX2;
  int a_UsedY2;

  void sizeOfAll(int&, int&, int&, int&);

  // Viewport-realative coordinates of the cursor between mouse movements.
  // Used in "pan with mouse" feature.
  QPoint a_previousCursorPosition;

  bool a_dragIsOkay;
  /*! \brief hold system-independent information about a schematic file */
  QFileInfo a_FileInfo;

  /**
    Minimum scale at which schematic could be drawn.
  */
  static constexpr double a_minScale = 0.1;

  /**
    Maximum scale at which schematic could be drawn.
  */
  static constexpr double a_maxScale = 10.0;

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
  Node* provideNode(int, int);
  Node* selectedNode(int, int);

  qucs_s::wire::Planner a_wirePlanner;
  std::pair<bool,Node*> connectWithWire(const QPoint& a, const QPoint& b) noexcept;
  std::pair<bool,Node*> connectWithWire(const QPoint& a, const QPoint& b, bool optimize, qucs_s::wire::Planner::PlanType planType) noexcept;
  void showEphemeralWire(const QPoint& a, const QPoint& b) noexcept;
  void  optimizeWires();
  std::pair<bool,Node*> installWire(Wire* wire);
  void displayMutations();
  void heal(qucs_s::wire::Planner::PlanType planType = qucs_s::wire::Planner::PlanType::Straight);
  void dumbConnectWithWire(const QPoint& a, const QPoint& b) noexcept;

  void  selectWireLine(Element*, Node*, bool);
  Wire* selectedWire(int, int);
  Wire* splitWire(Wire*, Node*);
  void  deleteWire(Wire*, bool remove_orphans=true);

  Marker* setMarker(int, int);
  void    markerLeftRight(bool, const std::vector<Marker*>& markers);
  void    markerUpDown(bool, const std::vector<Marker*>& markers);

  Element* selectElement(float, float, bool, int *index=0);
  void     deselectElements(Element*) const;
  int      selectElements(const QRect&, bool, bool) const;
  void     selectMarkers() const;
  bool     deleteElements();
  bool     aligning(int);
  bool     distributeHorizontal();
  bool     distributeVertical();

  void       setComponentNumber(Component*);
  void       insertRawComponent(Component*, bool noOptimize=true);
  void       recreateComponent(Component*);
  void       insertComponent(Component*);
  void       activateCompsWithinRect(int, int, int, int);
  bool       activateSpecifiedComponent(int, int);
  bool       activateSelectedComponents();
  Component* selectCompText(int, int, int&, int&) const;
  Component* searchSelSubcircuit();
  Component* selectedComponent(int, int);
  void       deleteComp(Component*);
  void       detachComp(Component*);
  Component* getComponentByName(const QString& compname) const;

  void     oneLabel(Node*);
  int      placeNodeLabel(WireLabel*);
  Element* getWireLabel(Node*);

  Painting* selectedPainting(float, float);


private:
  void insertComponentNodes(Component*, bool);

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

  void setIsAnalog(bool value) { a_isAnalog = value; }
  bool getIsAnalog() const { return a_isAnalog; }
  void setIsVerilog(bool value) { a_isVerilog = value; }
  bool getIsVerilog() const { return a_isVerilog; }
  bool giveNodeNames(QTextStream *, int&, QStringList&, QPlainTextEdit*, int);

private:
  int  saveDocument();

  bool loadProperties(QTextStream*);
  void simpleInsertComponent(Component*);
  bool loadComponents(QTextStream*, QList<Component*> *List=0);
  void simpleInsertWire(Wire*);
  bool loadWires(QTextStream*, QList<Element*> *List=0);
  bool loadDiagrams(QTextStream*, QList<Diagram*>*);
  bool loadPaintings(QTextStream*, QList<Painting*>*);
  bool loadIntoNothing(QTextStream*);

  QString createClipboardFile();
  bool    pasteFromClipboard(QTextStream *, QList<Element*>*);

  QString createUndoString(char);
  bool    rebuild(QString *);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(QString *);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);
  void throughAllNodes(bool, QStringList&, int&);
  void propagateNode(QStringList&, int&, Node*);
  void collectDigitalSignals(void);
  void beginNetlistDigital(QTextStream &);
  void endNetlistDigital(QTextStream &);
  bool throughAllComps(QTextStream *, int&, QStringList&, QPlainTextEdit *, int);

  DigMap a_Signals; // collecting node names for VHDL signal declarations
  QStringList a_PortTypes;

  bool a_isAnalog;
  bool a_isVerilog;
  bool a_creatingLib;
};

#endif
