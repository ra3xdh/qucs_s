/***************************************************************************
                                   qucs.h
                                  --------
    begin                : Thu Aug 28 2003
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

#ifndef QUCS_H
#define QUCS_H

#include <QtGui>
//#include <q3mainwindow.h>
#include <QMainWindow>
#include <qstring.h>
#include <qdir.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3PtrList>
//#include <Q3PopupMenu>
#include <QMouseEvent>
#include <QCloseEvent>
#include <Q3ScrollView>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>
#include <QDockWidget>
//#include <QListWidget>
#include <QSettings>

class QucsDoc;
class Schematic;
class SimMessage;
class MouseActions;
class SearchDialog;
class QLabel;
class QAction;
class QPrinter;
//class Q3ToolBar;
class QLineEdit;
class QComboBox;
class QTabWidget;
//class Q3PopupMenu;
//class Q3ListBox;
//class Q3ListBoxItem;
class Q3ListView;
class Q3ListViewItem;
//class Q3IconView;
//class Q3IconViewItem;
//class VTabbedDockWidget;
//class VTabWidget;
class OctaveWindow;

typedef bool (Schematic::*pToggleFunc) ();
typedef void (MouseActions::*pMouseFunc) (Schematic*, QMouseEvent*);
typedef void (MouseActions::*pMouseFunc2) (Schematic*, QMouseEvent*, float, float);

class QucsApp : public QMainWindow {
  Q_OBJECT
public:
  QucsApp();
 ~QucsApp();
  QSettings *qucsSettings;
  bool closeAllFiles();
  static int testFile(const QString&);
  bool gotoPage(const QString&);   // to load a document
  QucsDoc *getDoc(int No=-1);
  QucsDoc* findDoc (QString, int * Pos = 0);
  QString fileType (const QString&);

  QString ProjName;   // name of the project, that is open

  QLineEdit *editText;  // for edit component properties on schematic
  SearchDialog *SearchDia;  // global in order to keep values

  // current mouse methods
  void (MouseActions::*MouseMoveAction) (Schematic*, QMouseEvent*);
  void (MouseActions::*MousePressAction) (Schematic*, QMouseEvent*, float, float);
  void (MouseActions::*MouseDoubleClickAction) (Schematic*, QMouseEvent*);
  void (MouseActions::*MouseReleaseAction) (Schematic*, QMouseEvent*);

protected:
  void closeEvent(QCloseEvent*);

public slots:
  void slotFileNew();     // generate a new schematic in the view TabBar
  void slotTextNew();     // generate a new text editor in the view TabBar
  void slotFileOpen();    // open a document
  void slotFileSave();    // save a document
  void slotFileSaveAs();  // save a document under a different filename
  void slotFileSaveAll(); // save all open documents
  void slotFileClose();   // close the actual file
  void slotFileExamples();   // show the examples in a file browser
  void slotHelpTutorial();   // Open a pdf tutorial
  void slotHelpReport();   // Open a pdf report
  void slotHelpTechnical();   // Open a pdf technical document
  void slotFileClose (int); // close the file with given index
  void slotSymbolEdit();  // edit the symbol for the schematic
  void slotFileSettings();// open dialog to change file settings
  void slotFilePrint();   // print the current file
  void slotFilePrintFit();// Print and fit to page
  void slotFileQuit();    // exits the application
  void slotEditCut();     // put marked object into clipboard and delete it
  void slotEditCopy();    // put the marked object into the clipboard
  void slotApplSettings();// open dialog to change application settings

  void slotIntoHierarchy();
  void slotPopHierarchy();

  void slotShowAll();
  void slotShowOne();
  void slotZoomOut(); // Zoom out by 2

  void slotToPage();
  void slotSelectComponent(QListWidgetItem*);

  void slotEditElement();
  void slotPowerMatching();
  void slot2PortMatching();

  // for menu that appears by right click in content ListView
  void slotShowContentMenu(const QPoint &);
  //void slotShowContentMenu(Q3ListViewItem*, const QPoint&, int);
  void slotCMenuOpen();
  void slotCMenuRename();
  void slotCMenuDelete();
  void slotCMenuDelGroup();
  void slotCMenuInsert();

private slots:
  void slotMenuOpenProject();
  void slotOpenProject(QListWidgetItem*);
  void slotMenuCloseProject();
  void slotSelectSubcircuit(QTreeWidgetItem*);
  void slotOpenContent(QTreeWidgetItem*);
  void slotSetCompView(int);
  void slotProjNewButt();
  void slotProjOpenButt();
  void slotProjDelButt();
  void slotMenuDelProject();
  void slotChangeView(QWidget*);
  void slotSimulate();
  void slotAfterSimulation(int, SimMessage*);
  void slotDCbias();
  void slotChangePage(QString&, QString&);
  void slotNextTab();
  void slotHideEdit();
signals:
  void signalKillEmAll();

public:
  MouseActions *view;
  QTabWidget *DocumentTab;
  //Q3IconView  *CompComps;
  QListWidget *CompComps;

  // menu appearing by right mouse button click on content listview
  QMenu *ContentMenu;

  // corresponding actions
  QAction *ActionCMenuOpen, *ActionCMenuRename, *ActionCMenuDelete, *ActionCMenuDelGroup, *ActionCMenuInsert;

  QAction *fileNew, *textNew, *fileNewDpl, *fileOpen, *fileSave, *fileSaveAs,
          *fileSaveAll, *fileClose, *fileExamples, *fileSettings, *filePrint, *fileQuit,
          *projNew, *projOpen, *projDel, *projClose, *applSettings,
          *editCut, *editCopy, *magAll, *magOne, *magMinus, *filePrintFit,
          *symEdit, *intoH, *popH, *simulate, *dpl_sch, *undo, *redo, *dcbias;

  QAction *activeAction;    // pointer to the action selected by the user

private:
  QPrinter  *Printer; // printer is global (to remember the user settings)

// ********* Widgets on the main area **********************************
  //VTabbedDockWidget   *dock;
  QDockWidget *dock;
  QTabWidget *TabView;
  QDockWidget *octDock;
  OctaveWindow  *octave;

  QListWidget      *Projects;
  //Q3ListView     *Content;
  QTreeWidget *Content;
  //Q3ListViewItem *ConSchematics, *ConSources, *ConDisplays, *ConDatasets,
  //              *ConOthers, *ConVerilog, *ConVerilogA, *ConOctave;
  QTreeWidgetItem *ConSchematics, *ConSources, *ConDisplays, *ConDatasets,
                *ConOthers, *ConVerilog, *ConVerilogA, *ConOctave;


  QComboBox     *CompChoose;

// ********** Properties ************************************************
  Q3PtrList<QString> HierarchyHistory; // keeps track of "go into subcircuit"
  QString  QucsFileFilter;

// ********** Methods ***************************************************
  void initView();
  void initCursorMenu();
  void initContentListView();

  void printCurrentDocument(bool);
  bool saveFile(QucsDoc *Doc=0);
  bool saveAs();
  void openProject(const QString&, const QString&);
  bool deleteProject(const QString&, const QString&);
  void updatePortNumber(QucsDoc*, int);
  void fillComboBox(bool);
  void switchSchematicDoc(bool);
  void switchEditMode(bool);
  void changeSchematicSymbolMode(Schematic*);
  bool deleteDirectoryContent(QDir& Dir);
  bool isTextDocument(QWidget *);
  void closeFile(int);
public:

  void readProjects();
  void readProjectFiles();

/* **************************************************
   *****  The following methods are located in  *****
   *****  "qucs_init.cpp".                      *****
   ************************************************** */

public slots:
  void slotShowWarnings();
  void slotResetWarnings();
  void printCursorPosition(int, int);

private slots:
  void slotViewToolBar(bool toggle);    // toggle the toolbar
  void slotViewStatusBar(bool toggle);  // toggle the statusbar
  void slotViewBrowseDock(bool toggle); // toggle the dock window
  void slotViewOctaveDock(bool); // toggle the dock window
  void slotToggleOctave(bool);
  void slotToggleDock(bool);
  void slotHelpAbout();     // shows an about dialog
  void slotHelpAboutQt();   // shows the standard about dialog for Qt

private:
  void initActions();    // initializes all QActions of the application
  void initMenuBar();    // creates the menu_bar and inserts the menuitems
  void initToolBar();    // creates the toolbars
  void initStatusBar();  // setup the statusbar

  QAction *helpAboutApp, *helpAboutQt, *viewToolBar, *viewStatusBar,
          *viewBrowseDock, *viewOctaveDock;

  // menus contain the items of their menubar
  QMenu *fileMenu, *editMenu, *insMenu, *projMenu, *simMenu, *viewMenu,
             *helpMenu, *alignMenu, *toolMenu;

  // submenus for the PDF documents 
  QMenu *helpTechnical, *helpReport, *helpTutorial;

  QToolBar *fileToolbar, *editToolbar, *viewToolbar, *workToolbar;

  // Shortcuts for scolling schematic / TextEdit
  // This is rather cumbersome -> Make this with a QScrollView instead??
  QShortcut *cursorUp, *cursorLeft, *cursorRight, *cursorDown;

  QLabel *WarningLabel, *PositionLabel;  // labels in status bar



/* **************************************************
   *****  The following methods are located in  *****
   *****  "qucs_actions.cpp".                   *****
   ************************************************** */

public:
  void editFile(const QString&);

  QAction *insWire, *insLabel, *insGround, *insPort, *insEquation, *magPlus,
          *editRotate, *editMirror, *editMirrorY, *editPaste, *select,
          *editActivate, *wire, *editDelete, *setMarker, *onGrid, *moveText,
          *helpIndex, *helpGetStart, *callEditor, *callFilter, *callLine,
          *showMsg, *showNet, *alignTop, *alignBottom, *alignLeft, *alignRight,
          *distrHor, *distrVert, *selectAll, *callLib, *callMatch, *changeProps,
          *addToProj, *editFind, *editFindAgain, *insEntity, *selectMarker,
          *createLib, *importData, *graph2csv, *createPkg, *extractPkg,
          *callAtt, *centerHor, *centerVert;

public slots:
  void slotEditRotate(bool);  // rotate the selected items
  void slotEditMirrorX(bool); // mirror the selected items about X axis
  void slotEditMirrorY(bool); // mirror the selected items about Y axis
  void slotEditPaste(bool);   // paste the clipboard into the document
  void slotEditDelete(bool);  // delete the selected items
  void slotInsertEquation(bool);
  void slotInsertGround(bool);
  void slotInsertPort(bool);
  void slotInsertEntity();
  void slotSetWire(bool);
  void slotEscape();
  void slotSelect(bool);
  void slotEditActivate(bool);
  void slotInsertLabel(bool);
  void slotSetMarker(bool);
  void slotOnGrid(bool);      // set selected elements on grid
  void slotMoveText(bool);    // move property text of components
  void slotZoomIn(bool);
  void slotEditUndo();    // makes the last operation undone
  void slotEditRedo();    // makes the last undo undone
  void slotEditFind();    // searches for a piece of text
  void slotEditFindAgain();  // searches for the same piece of text again
  void slotAlignTop();    // align selected elements with respect to top
  void slotAlignBottom(); // align selected elements with respect to bottom
  void slotAlignLeft();   // align selected elements with respect to left
  void slotAlignRight();  // align selected elements with respect to right
  void slotDistribHoriz();// distribute horizontally selected elements
  void slotDistribVert(); // distribute vertically selected elements
  void slotCenterHorizontal();
  void slotCenterVertical();
  void slotSelectAll();
  void slotSelectMarker();
  void slotShowLastMsg();
  void slotShowLastNetlist();
  void slotCallEditor();
  void slotCallFilter();
  void slotCallLine();
  void slotCallLibrary();
  void slotCallMatch();
  void slotCallAtt();
  void slotHelpIndex();       // shows a HTML docu: Help Index
  void slotGettingStarted();  // shows a HTML docu: Getting started
  void slotChangeProps();
  void slotAddToProject();
  void slotApplyCompText();

private slots:
  void slotCursorLeft();
  void slotCursorRight();
  void slotCursorUp();
  void slotCursorDown();
  void slotResizePropEdit(const QString&);
  void slotCreateLib();
  void slotImportData();
  void slotExportGraphAsCsv();
  void slotCreatePackage();
  void slotExtractPackage();

private:
  void showHTML(const QString&);
  bool performToggleAction(bool, QAction*, pToggleFunc, pMouseFunc, pMouseFunc2);
  friend class SaveDialog;
};
#endif
