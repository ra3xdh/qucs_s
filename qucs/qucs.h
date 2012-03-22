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

#include <qmainwindow.h>
#include <qstring.h>
#include <qdir.h>

class QucsDoc;
class Schematic;
class SimMessage;
class MouseActions;
class SearchDialog;
class QLabel;
class QAction;
class QPrinter;
class QToolBar;
class QLineEdit;
class QComboBox;
class QTabWidget;
class QPopupMenu;
class QListBox;
class QListBoxItem;
class QListView;
class QListViewItem;
class QIconView;
class QIconViewItem;
class VTabbedDockWidget;
class VTabWidget;
<<<<<<< HEAD
=======
class OctaveWindow;
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d

typedef bool (Schematic::*pToggleFunc) ();
typedef void (MouseActions::*pMouseFunc) (Schematic*, QMouseEvent*);
typedef void (MouseActions::*pMouseFunc2) (Schematic*, QMouseEvent*, float, float);

extern QDir QucsWorkDir;
extern QDir QucsHomeDir;


class QucsApp : public QMainWindow {
  Q_OBJECT
public:
  QucsApp();
 ~QucsApp();

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
  void slotSelectComponent(QIconViewItem*);

  void slotEditElement();
  void slotPowerMatching();
  void slot2PortMatching();

  // for menu that appears by right click in content ListView
  void slotShowContentMenu(QListViewItem*, const QPoint&, int);
  void slotCMenuOpen();
  void slotCMenuRename();
  void slotCMenuDelete();
  void slotCMenuDelGroup();

private slots:
  void slotMenuOpenProject();
  void slotOpenProject(QListBoxItem*);
  void slotMenuCloseProject();
  void slotSelectSubcircuit(QListViewItem*);
  void slotExpandContentList(QListViewItem*);
  void slotOpenContent(QListViewItem*);
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
  QIconView  *CompComps;

  // menu appearing by right mouse button click on content listview
  QPopupMenu *ContentMenu;

  QAction *fileNew, *textNew, *fileNewDpl, *fileOpen, *fileSave, *fileSaveAs,
          *fileSaveAll, *fileClose, *fileSettings, *filePrint, *fileQuit,
          *projNew, *projOpen, *projDel, *projClose, *applSettings,
          *editCut, *editCopy, *magAll, *magOne, *magMinus, *filePrintFit,
          *symEdit, *intoH, *popH, *simulate, *dpl_sch, *undo, *redo, *dcbias;

  QAction *activeAction;    // pointer to the action selected by the user

private:
  QPrinter  *Printer; // printer is global (to remember the user settings)

// ********* Widgets on the main area **********************************
  VTabbedDockWidget   *dock;
  VTabWidget    *TabView;
<<<<<<< HEAD
=======
  QDockWindow   *octDock;
  OctaveWindow  *octave;
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d

  QListBox      *Projects;
  QListView     *Content;
  QListViewItem *ConSchematics, *ConSources, *ConDisplays, *ConDatasets,
<<<<<<< HEAD
                *ConOthers, *ConVerilog, *ConVerilogA;
=======
                *ConOthers, *ConVerilog, *ConVerilogA, *ConOctave;
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d

  QComboBox     *CompChoose;

// ********** Properties ************************************************
  QPtrList<QString> HierarchyHistory; // keeps track of "go into subcircuit"
  QString  QucsFileFilter;

// ********** Methods ***************************************************
  void initView();
  void initCursorMenu();
  void initContentListView();

  void printCurrentDocument(bool);
  bool saveFile(QucsDoc *Doc=0);
  bool saveAs();
  void readProjects();
  void readProjectFiles();
  void openProject(const QString&, const QString&);
  bool deleteProject(const QString&, const QString&);
  void updatePortNumber(QucsDoc*, int);
  void fillComboBox(bool);
  void switchSchematicDoc(bool);
  void switchEditMode(bool);
  void changeSchematicSymbolMode(Schematic*);
  bool deleteDirectoryContent(QDir& Dir);
  bool isTextDocument(QWidget *);


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
<<<<<<< HEAD
=======
  void slotViewOctaveDock(bool); // toggle the dock window
  void slotToggleOctave(bool);
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d
  void slotToggleDock(bool);
  void slotHelpAbout();     // shows an about dialog
  void slotHelpAboutQt();   // shows the standard about dialog for Qt

private:
  void initActions();    // initializes all QActions of the application
  void initMenuBar();    // creates the menu_bar and inserts the menuitems
  void initToolBar();    // creates the toolbars
  void initStatusBar();  // setup the statusbar

  QAction *helpAboutApp, *helpAboutQt, *viewToolBar, *viewStatusBar,
<<<<<<< HEAD
          *viewBrowseDock;
=======
          *viewBrowseDock, *viewOctaveDock;
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d

  // menus contain the items of their menubar
  QPopupMenu *fileMenu, *editMenu, *insMenu, *projMenu, *simMenu, *viewMenu,
             *helpMenu, *alignMenu, *toolMenu;

  QToolBar *fileToolbar, *editToolbar, *viewToolbar, *workToolbar;

  QAccel *mainAccel;     // to set more than one key to one action
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
