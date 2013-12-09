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
#include <QtGui>
#include "wire.h"
#include "diagrams/diagram.h"
#include "paintings/painting.h"
#include "components/component.h"
#include "qucsdoc.h"
//Added by qt3to4:
#include <Q3TextStream>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include "viewpainter.h"
#include "node.h"

#include <Q3ScrollView>
#include <QPainter>
#include <Q3PtrList>
#include <QFile>
#include <QStringList>

class Q3TextEdit;
class QTextEdit;

// digital signal data
struct DigSignal {
  DigSignal() { Name=""; Type=""; }
  DigSignal(const QString& _Name, const QString& _Type = "")
    : Name(_Name), Type(_Type) {}
  QString Name; // name
  QString Type; // type of signal
};
typedef QMap<QString, DigSignal> DigMap;
typedef enum {_NotRop, _Rect, _Line, _Ellipse, _Arc, _DotLine, _Translate, _Scale}PE;
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
  void paintGrid(ViewPainter*, int, int, int, int);
  void print(QPrinter*, QPainter*, bool, bool);

  void paintSchToViewpainter(ViewPainter* p, bool printAll, bool toImage, int screenDpiX=96, int printerDpiX=300);

  void PostPaintEvent(PE pe, int x1=0, int y1=0, int x2=0, int y2=0, int a=0, int b=0,bool PaintOnViewport=false);

  float textCorr();
  void  sizeOfAll(int&, int&, int&, int&);
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  void  setOnGrid(int&, int&);
  bool  elementsOnGrid();

  float zoom(float);
  float zoomBy(float);
  void  showAll();
  void  showNoZoom();
  void  enlargeView(int, int, int, int);
  void  switchPaintMode();
  int   adjustPortNumbers();
  void  reloadGraphs();
  bool  createSubcircuitSymbol();

  QString copySelected(bool);
  bool    paste(Q3TextStream*, Q3PtrList<Element>*);
  bool    load();
  int     save();
  int     saveSymbolCpp (void);
  void    becomeCurrent(bool);
  bool    undo();
  bool    redo();

  bool scrollUp(int);
  bool scrollDown(int);
  bool scrollLeft(int);
  bool scrollRight(int);

  // The pointers points to the current lists, either to the schematic
  // elements "Doc..." or to the symbol elements "SymbolPaints".
  Q3PtrList<Wire>      *Wires, DocWires;
  Q3PtrList<Node>      *Nodes, DocNodes;
  Q3PtrList<Diagram>   *Diagrams, DocDiags;
  Q3PtrList<Painting>  *Paintings, DocPaints;
  Q3PtrList<Component> *Components, DocComps;

  Q3PtrList<Painting>  SymbolPaints;  // symbol definition for subcircuit

  QList<PostedPaintEvent>   PostedPaintEvents;
  bool symbolMode;  // true if in symbol painting mode


  int GridX, GridY;
  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area
  int UsedX1, UsedY1, UsedX2, UsedY2;  // document area used by elements

  int showFrame;
  QString Frame_Text0, Frame_Text1, Frame_Text2, Frame_Text3;

  // Two of those data sets are needed for Schematic and for symbol.
  // Which one is in "tmp..." depends on "symbolMode".
  float tmpScale;
  int tmpViewX1, tmpViewY1, tmpViewX2, tmpViewY2;
  int tmpUsedX1, tmpUsedY1, tmpUsedX2, tmpUsedY2;

  Q3PtrList<QString> UndoStack;
  Q3PtrList<QString> UndoSymbol;    // undo stack for circuit symbol

protected:
  bool sizeOfFrame(int&, int&);
  void paintFrame(ViewPainter*);

  // overloaded function to get actions of user
  void drawContents(QPainter*, int, int, int, int);
  void contentsMouseMoveEvent(QMouseEvent*);
  void contentsMousePressEvent(QMouseEvent*);
  void contentsMouseDoubleClickEvent(QMouseEvent*);
  void contentsMouseReleaseEvent(QMouseEvent*);
  void contentsWheelEvent(QWheelEvent*);
  void contentsDropEvent(QDropEvent*);
  void contentsDragEnterEvent(QDragEnterEvent*);
  void contentsDragLeaveEvent(QDragLeaveEvent*);
  void contentsDragMoveEvent(QDragMoveEvent*);

protected slots:
  void slotScrollUp();
  void slotScrollDown();
  void slotScrollLeft();
  void slotScrollRight();

private:
  bool dragIsOkay;

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
  void     deselectElements(Element*);
  int      selectElements(int, int, int, int, bool);
  void     selectMarkers();
  void     newMovingWires(Q3PtrList<Element>*, Node*, int);
  int      copySelectedElements(Q3PtrList<Element>*);
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
  void       setCompPorts(Component*);
  Component* selectCompText(int, int, int&, int&);
  Component* searchSelSubcircuit();
  Component* selectedComponent(int, int);
  void       deleteComp(Component*);

  void     oneLabel(Node*);
  int      placeNodeLabel(WireLabel*);
  Element* getWireLabel(Node*);
  void     insertNodeLabel(WireLabel*);
  void     copyLabels(int&, int&, int&, int&, Q3PtrList<Element>*);

  Painting* selectedPainting(float, float);
  void      copyPaintings(int&, int&, int&, int&, Q3PtrList<Element>*);

private:
  void insertComponentNodes(Component*, bool);
  int  copyWires(int&, int&, int&, int&, Q3PtrList<Element>*);
  int  copyComponents(int&, int&, int&, int&, Q3PtrList<Element>*);
  void copyComponents2(int&, int&, int&, int&, Q3PtrList<Element>*);
  bool copyComps2WiresPaints(int&, int&, int&, int&, Q3PtrList<Element>*);
  int  copyElements(int&, int&, int&, int&, Q3PtrList<Element>*);


/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_file.cpp". They only access the QPtrLists    *****
   *****  and their pointers. ("DocComps", "Components" etc.)     *****
   ******************************************************************** */

public:
  bool createLibNetlist(QTextStream*, QTextEdit*, int);
  bool createSubNetlist(QTextStream *, int&, QStringList&, QTextEdit*, int);
  void createSubNetlistPlain(QTextStream*, QTextEdit*, int);
  int  prepareNetlist(QTextStream&, QStringList&, QTextEdit*);
  QString createNetlist(QTextStream&, int);
  bool loadDocument();

private:
  int  saveDocument();

  bool loadProperties(Q3TextStream*);
  void simpleInsertComponent(Component*);
  bool loadComponents(Q3TextStream*, Q3PtrList<Component> *List=0);
  void simpleInsertWire(Wire*);
  bool loadWires(Q3TextStream*, Q3PtrList<Element> *List=0);
  bool loadDiagrams(Q3TextStream*, Q3PtrList<Diagram>*);
  bool loadPaintings(Q3TextStream*, Q3PtrList<Painting>*);
  bool loadIntoNothing(Q3TextStream*);

  QString createClipboardFile();
  bool    pasteFromClipboard(Q3TextStream*, Q3PtrList<Element>*);

  QString createUndoString(char);
  bool    rebuild(QString *);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(QString *);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);
  void throughAllNodes(bool, QStringList&, int&);
  void propagateNode(QStringList&, int&, Node*);
  void collectDigitalSignals(void);
  bool giveNodeNames(QTextStream *, int&, QStringList&, QTextEdit*, int);
  void beginNetlistDigital(QTextStream &);
  void endNetlistDigital(QTextStream &);
  bool throughAllComps(QTextStream *, int&, QStringList&, QTextEdit *, int);

  DigMap Signals; // collecting node names for VHDL signal declarations
  QStringList PortTypes;

public:
  bool isAnalog;
  bool isVerilog;
  bool creatingLib;
};

#endif
