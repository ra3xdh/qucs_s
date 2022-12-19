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

#include <QMainWindow>
#include <QString>
#include <QHash>
#include <QStack>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

class QucsDoc;
class Schematic;
class SimMessage;
class MouseActions;
class SearchDialog;
class OctaveWindow;
class MessageDock;
class ProjectView;

class QLabel;
class QAction;
class QLineEdit;
class QComboBox;
class QTabWidget;
class QDir;
class QMouseEvent;
class QCloseEvent;
class QMenu;
class QToolBar;
class QSettings;
class QListWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;
class QListWidget;
class QShortcut;
class QListView;
class QModelIndex;
class QPushButton;
class QTextEdit;

class SymbolWidget;

typedef bool (Schematic::*pToggleFunc) ();
typedef void (MouseActions::*pMouseFunc) (Schematic*, QMouseEvent*);
typedef void (MouseActions::*pMouseFunc2) (Schematic*, QMouseEvent*, float, float);

class QucsFileSystemModel : public QFileSystemModel {
Q_OBJECT
public:
    explicit QucsFileSystemModel(QObject* parent = nullptr) : QFileSystemModel(parent) {};
    QVariant data(const QModelIndex& index, int role) const override;
};

class QucsSortFilterProxyModel : public QSortFilterProxyModel {
Q_OBJECT

public:
    explicit QucsSortFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {};
protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class QucsApp : public QMainWindow {
  Q_OBJECT
public:
  QucsApp();
 ~QucsApp();
  bool closeAllFiles();
  bool gotoPage(const QString&);   // to load a document
  QucsDoc *getDoc(int No=-1);
  QucsDoc* findDoc (QString, int * Pos = 0);
  QString fileType (const QString&);
  static bool isTextDocument(QWidget *);

  QString ProjName;   // name of the project, that is open
  QHash<QString,QString> schNameHash; // QHash for the schematic files lookup
  QHash<QString,QString> spiceNameHash; // QHash for the spice files lookup

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
  void slotApplSettings();// open dialog to change application settings
  void slotRefreshSchPath(); // refresh the schematic path hash

  void slotIntoHierarchy();
  void slotPopHierarchy();

  void slotShowAll();
  void slotShowOne();
  void slotZoomOut(); // Zoom out by 2

  void slotToPage();
  void slotSelectComponent(QListWidgetItem*);
  void slotSearchComponent(const QString &);
  void slotSearchClear();

  void slotEditElement();
  void slotPowerMatching();
  void slot2PortMatching();

  // for menu that appears by right click in content ListView
  void slotShowContentMenu(const QPoint &);

  void slotCMenuOpen();
  void slotCMenuCopy();
  void slotCMenuRename();
  void slotCMenuDelete();
  void slotCMenuInsert();

  void slotUpdateTreeview();

  void slotMenuProjClose();

private slots:
  void slotMenuProjOpen();
  void slotMenuProjDel();
  void slotListProjOpen(const QModelIndex &);
  void slotSelectSubcircuit(const QModelIndex &);
  void slotSelectLibComponent(QTreeWidgetItem*);
  void slotOpenContent(const QModelIndex &);
  void slotSetCompView(int);
  void slotButtonProjNew();
  void slotButtonProjOpen();
  void slotButtonProjDel();
  void slotChangeView();
  void slotSimulate();
  void slotAfterSimulation(int, SimMessage*);
  void slotDCbias();
  void slotChangePage(QString&, QString&);
  void slotHideEdit();
  void slotFileChanged(bool);
  void slotSimSettings();
  void slotSimulateWithSpice();
  void slotAfterSpiceSimulation();
  void slotBuildVAModule();
  void slotBuildXSPICEIfs(int mode = 0);
  void slotEDDtoIFS();
  void slotEDDtoMOD();
  void slotShowModel();
  void slotSearchLibComponent(const QString &);
  void slotSearchLibClear();

signals:
  void signalKillEmAll();

public:
  MouseActions *view;
  QTabWidget *DocumentTab;
  QListWidget *CompComps;
  QTreeWidget *libTreeWidget;
  QTextEdit *CompDescr;
  QLineEdit *LibCompSearch;
  SymbolWidget *Symbol;
  QPushButton *btnShowModel;

  // menu appearing by right mouse button click on content listview
  QMenu *ContentMenu;

  // corresponding actions
  QAction *ActionCMenuOpen, *ActionCMenuCopy, *ActionCMenuRename, *ActionCMenuDelete, *ActionCMenuInsert;

  QAction *fileNew, *textNew, *fileNewDpl, *fileOpen, *fileSave, *fileSaveAs,
          *fileSaveAll, *fileClose, *fileExamples, *fileSettings, *filePrint, *fileQuit,
          *projNew, *projOpen, *projDel, *projClose, *applSettings, *refreshSchPath,
          *editCut, *editCopy, *magAll, *magOne, *magMinus, *filePrintFit,
          *symEdit, *intoH, *popH, *simulate, *dpl_sch, *undo, *redo, *dcbias;

  QAction *exportAsImage;

  QAction *activeAction;    // pointer to the action selected by the user

private:
// ********* Widgets on the main area **********************************
  QDockWidget     *dock;
  QTabWidget      *TabView;
  QDockWidget     *octDock;
  OctaveWindow    *octave;
  MessageDock     *messageDock;

  QListView       *Projects;
  ProjectView     *Content;

  QLineEdit       *CompSearch;
  QPushButton     *CompSearchClear;
  QComboBox       *CompChoose;

// ********** Properties ************************************************
  QStack<QString> HierarchyHistory; // keeps track of "go into subcircuit"
  QString  QucsFileFilter;
  QFileSystemModel *m_homeDirModel;
  QucsSortFilterProxyModel *m_proxyModel;
  QFileSystemModel *m_projModel;
  int ccCurIdx; // CompChooser current index (used during search)

// ********** Methods ***************************************************
  void initView();
  void initCursorMenu();

  void printCurrentDocument(bool);
  bool saveFile(QucsDoc *Doc=0);
  bool saveAs();
  void openProject(const QString &);
  bool deleteProject(const QString &);
  void updatePortNumber(QucsDoc*, int);
  void fillComboBox(bool);
  void switchSchematicDoc(bool);
  void switchEditMode(bool);
  void changeSchematicSymbolMode(Schematic*);
  bool recurRemove(const QString &);
  void closeFile(int);

  void updateRecentFilesList(QString s);
  void successExportMessages(bool ok);
  void fillLibrariesTreeView (void);

public:

  void readProjects();
  void updatePathList(void); // update the list of paths, pruning non-existing paths
  void updatePathList(QStringList);
  void updateSchNameHash(void); // maps all schematic files in the path list
  void updateSpiceNameHash(void); // maps all spice files in the path list

/* **************************************************
   *****  The following methods are located in  *****
   *****  "qucs_init.cpp".                      *****
   ************************************************** */

public slots:
  void slotShowWarnings();
  void slotResetWarnings();
  void printCursorPosition(int, int);
  void slotUpdateUndo(bool);  // update undo available state
  void slotUpdateRedo(bool);  // update redo available state

private slots:
  void slotViewToolBar(bool toggle);    // toggle the toolbar
  void slotViewStatusBar(bool toggle);  // toggle the statusbar
  void slotViewBrowseDock(bool toggle); // toggle the dock window
  void slotViewOctaveDock(bool); // toggle the dock window
  void slotToggleOctave(bool);
  void slotToggleDock(bool);
  void slotHelpAbout();     // shows an about dialog

private:
  void initActions();    // initializes all QActions of the application
  void initMenuBar();    // creates the menu_bar and inserts the menuitems
  void initToolBar();    // creates the toolbars
  void initStatusBar();  // setup the statusbar

  QAction *helpAboutApp, *helpAboutQt, *viewToolBar, *viewStatusBar,
          *viewBrowseDock, *viewOctaveDock;

  // menus contain the items of their menubar
  enum { MaxRecentFiles = 8 };
  QMenu *fileMenu, *editMenu, *insMenu, *projMenu, *simMenu, *viewMenu,
             *helpMenu, *alignMenu, *toolMenu, *recentFilesMenu, *cmMenu;
  QAction *fileRecentAction[MaxRecentFiles];
  QAction *fileClearRecent;

  // submenus for the PDF documents
  QMenu *helpTechnical, *helpReport, *helpTutorial;

  QToolBar *fileToolbar, *editToolbar, *viewToolbar, *workToolbar;

  // Shortcuts for scrolling schematic / TextEdit
  // This is rather cumbersome -> Make this with a QScrollView instead??
  QShortcut *cursorUp, *cursorLeft, *cursorRight, *cursorDown;

  QLabel *WarningLabel, *PositionLabel;  // labels in status bar
  QLabel *SimulatorLabel;



/* **************************************************
   *****  The following methods are located in  *****
   *****  "qucs_actions.cpp".                   *****
   ************************************************** */

public:
  void editFile(const QString&);

  QAction *insWire, *insLabel, *insGround, *insPort, *insEquation, *magPlus,
          *editRotate, *editMirror, *editMirrorY, *editPaste, *select,
          *editActivate, *wire, *editDelete, *setMarker, *onGrid, *moveText,
          *helpIndex, *helpGetStart, *callEditor, *callFilter, *callLine, *callActiveFilter,
          *showMsg, *showNet, *alignTop, *alignBottom, *alignLeft, *alignRight,
          *distrHor, *distrVert, *selectAll, *callMatch, *changeProps,
          *addToProj, *editFind, *insEntity, *selectMarker,
          *createLib, *importData, *graph2csv, *createPkg, *extractPkg,
          *callAtt, *centerHor, *centerVert, *loadModule, *buildModule, *callPwrComb;

  QAction *helpQucsIndex;
  QAction *simSettings;
  QAction *buildVAModule, *buildIFS;

public slots:
  void slotEditRotate(bool);  // rotate the selected items
  void slotEditMirrorX(bool); // mirror the selected items about X axis
  void slotEditMirrorY(bool); // mirror the selected items about Y axis
  void slotEditCut();         // put marked object into clipboard and delete it
  void slotEditCopy();        // put the marked object into the clipboard
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
  void slotCallActiveFilter();
  void slotCallLine();
  void slotCallMatch();
  void slotCallAtt();
  void slotCallPwrComb();
  void slotHelpIndex();       // shows a HTML docu: Help Index
  void slotHelpQucsIndex();
  void slotGettingStarted();  // shows a HTML docu: Getting started
  void slotChangeProps();
  void slotAddToProject();
  void slotApplyCompText();
  void slotOpenRecent();
  void slotSaveDiagramToGraphicsFile();
  void slotSaveSchematicToGraphicsFile(bool diagram = false);

private slots:
  void slotCursorLeft(bool left=true);
  void slotCursorRight() {return slotCursorLeft(false);}
  void slotCursorUp(bool up=true);
  void slotCursorDown() {return slotCursorUp(false);}
  void slotResizePropEdit(const QString&);
  void slotCreateLib();
  void slotImportData();
  void slotExportGraphAsCsv();
  void slotCreatePackage();
  void slotExtractPackage();
  void slotUpdateRecentFiles();
  void slotClearRecentFiles();
  void slotLoadModule();
  void slotBuildModule();

private:
  void buildWithOpenVAF();
  bool performToggleAction(bool, QAction*, pToggleFunc, pMouseFunc, pMouseFunc2);
  void launchTool(const QString&, const QString&,
                  const QStringList& = QStringList(),bool qucs_tool = false); // tool, description and args
  friend class SaveDialog;
  QString lastExportFilename;
};

#endif /* QUCS_H */
