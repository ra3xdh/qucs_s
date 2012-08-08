/***************************************************************************
                                 qucs.cpp
                                ----------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003, 2004, 2005, 2006 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <limits.h>

#include <qaccel.h>
#include <qimage.h>
#include <qsplitter.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qpainter.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qfont.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qaction.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qiconview.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qprinter.h>
#include <qfiledialog.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qprocess.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <qdragobject.h>
#include <qsyntaxhighlighter.h>

#include "main.h"
#include "qucs.h"
#include "qucsdoc.h"
#include "textdoc.h"
#include "syntax.h"
#include "schematic.h"
#include "mouseactions.h"
#include "wire.h"
#include "module.h"
#include "components/components.h"
#include "paintings/paintings.h"
#include "diagrams/diagrams.h"
#include "dialogs/savedialog.h"
#include "dialogs/newprojdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/digisettingsdialog.h"
#include "dialogs/vasettingsdialog.h"
#include "dialogs/qucssettingsdialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/sweepdialog.h"
#include "dialogs/labeldialog.h"
#include "dialogs/matchdialog.h"
#include "dialogs/simmessage.h"
#include "dialogs/vtabwidget.h"
#include "dialogs/vtabbeddockwidget.h"
#include "octave_window.h"

extern const char *empty_xpm[];

QDir QucsWorkDir;  // current project path
QDir QucsHomeDir;  // Qucs user directory where all projects are located


// IconView without dragging icon bitmap
class myIconView : public QIconView
{
public:
  myIconView(QWidget* parent_) : QIconView(parent_, 0, 0) {};
 ~myIconView() {};

protected:
  QDragObject *dragObject() {
    QIconViewItem *Item = currentItem();
    if(!Item) return 0;

    // no picture during dragging, but bounding rectangles in QListView
    QIconDrag *DragPic = new QIconDrag( viewport() );
    DragPic->setPixmap( QPixmap(empty_xpm), QPoint(0, 0) );
    DragPic->append( QIconDragItem(),
        QRect( Item->pixmapRect().width() / -2,
               Item->pixmapRect().height() / -2,
               Item->pixmapRect().width(), Item->pixmapRect().height() ),
        QRect( Item->textRect().width() / -2,
               Item->pixmapRect().height() / 2 + 5,
               Item->textRect().width(), Item->textRect().height() ) );
    return DragPic;
  };
};



QucsApp::QucsApp()
{
  setCaption("Qucs " PACKAGE_VERSION);

  QucsFileFilter =
    tr("Schematic")+" (*.sch);;"+
    tr("Data Display")+" (*.dpl);;"+
    tr("Qucs Documents")+" (*.sch *.dpl);;"+
    tr("VHDL Sources")+" (*.vhdl *.vhd);;"+
    tr("Verilog Sources")+" (*.v);;"+
    tr("Verilog-A Sources")+" (*.va);;"+
    tr("Octave Scripts")+" (*.m *.oct);;"+
    tr("Any File")+" (*)";
  QucsWorkDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));
  QucsHomeDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));

  move  (QucsSettings.x,  QucsSettings.y);
  resize(QucsSettings.dx, QucsSettings.dy);

  MouseMoveAction = 0;
  MousePressAction = 0;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  initView();
  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();
  viewToolBar->setOn(true);
  viewStatusBar->setOn(true);
  viewBrowseDock->setOn(true);
  slotViewOctaveDock(false);
  initCursorMenu();
  HierarchyHistory.setAutoDelete(true);
  Module::registerModules ();

  // default settings of the printer
  Printer = new QPrinter(QPrinter::HighResolution);
#if defined (QT_VERSION) && QT_VERSION > 0x030200
  Printer->setOptionEnabled(QPrinter::PrintSelection, true);
  Printer->setOptionEnabled(QPrinter::PrintPageRange, false);
  Printer->setOptionEnabled(QPrinter::PrintToFile, true);
#endif
  Printer->setColorMode(QPrinter::Color);
  Printer->setFullPage(true);

  SearchDia = new SearchDialog(this);

  // creates a document called "untitled"
  new Schematic(this, "");

  select->setOn(true);  // switch on the 'select' action
  switchSchematicDoc(true);  // "untitled" document is schematic

  // load documents given as command line arguments
  for(int z=1; z<qApp->argc(); z++) {
    QString arg = qApp->argv()[z];
    if(*(arg) != '-') {
      // allow uri's: file:/home/linuxuser/Desktop/example.sch
      if(arg.contains(":/")) {
        QString f = QDir::convertSeparators(QUriDrag::uriToLocalFile(arg));
        if(f.isEmpty()) f = arg;
        gotoPage(f);
      } else {
        gotoPage(arg);
      }
    }
  }
}

QucsApp::~QucsApp()
{
  Module::unregisterModules ();
  delete Printer;
}


// #######################################################################
// ##########                                                   ##########
// ##########     Creates the working area (QTabWidget etc.)    ##########
// ##########                                                   ##########
// #######################################################################
void QucsApp::initContentListView()
{
  Content->clear();   // remove all documents
  ConOthers     = new QListViewItem(Content, tr("Others"));
  ConDatasets   = new QListViewItem(Content, tr("Datasets"));
  ConDisplays   = new QListViewItem(Content, tr("Data Displays"));
  ConOctave     = new QListViewItem(Content, tr("Octave"));
  ConVerilog    = new QListViewItem(Content, tr("Verilog"));
  ConVerilogA   = new QListViewItem(Content, tr("Verilog-A"));
  ConSources    = new QListViewItem(Content, tr("VHDL"));
  ConSchematics = new QListViewItem(Content, tr("Schematics"));
}

void QucsApp::initView()
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));

  DocumentTab = new QTabWidget(this);
  setCentralWidget(DocumentTab);
  connect(DocumentTab,
          SIGNAL(currentChanged(QWidget*)), SLOT(slotChangeView(QWidget*)));

  dock = new VTabbedDockWidget(QDockWindow::InDock, this);
  TabView = new VTabWidget(VTabInterface::TabLeft,dock);  // tabs on the left side
  
  
  connect(dock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleDock(bool)));

  view = new MouseActions();

  editText = new QLineEdit(this);  // for editing component properties
  editText->setFrame(false);
  editText->setHidden(true);
  editText->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(editText, SIGNAL(returnPressed()), SLOT(slotApplyCompText()));
  connect(editText, SIGNAL(textChanged(const QString&)),
          SLOT(slotResizePropEdit(const QString&)));
  connect(editText, SIGNAL(lostFocus()), SLOT(slotHideEdit()));

  // ----------------------------------------------------------
  // "Project Tab" of the left QTabWidget
  QVBox *ProjGroup = new QVBox(this);
  QHBox *ProjButts = new QHBox(ProjGroup);
  QPushButton *ProjNew   = new QPushButton(tr("New"),ProjButts);
  connect(ProjNew, SIGNAL(clicked()), SLOT(slotProjNewButt()));
  QPushButton *ProjOpen  = new QPushButton(tr("Open"),ProjButts);
  connect(ProjOpen, SIGNAL(clicked()), SLOT(slotProjOpenButt()));
  QPushButton *ProjDel   = new QPushButton(tr("Delete"),ProjButts);
  connect(ProjDel, SIGNAL(clicked()), SLOT(slotProjDelButt()));

  Projects = new QListBox(ProjGroup);
  TabView->addPage(ProjGroup, tr("Projects"));
  TabView->setTabToolTip(TabView->id(ProjGroup),
			 tr("content of project directory"));

  connect(Projects, SIGNAL(doubleClicked(QListBoxItem*)),
		    SLOT(slotOpenProject(QListBoxItem*)));

  // ----------------------------------------------------------
  // "Content Tab" of the left QTabWidget
  Content = new QListView(this);
  Content->setRootIsDecorated(true); // open/close decoration for root items
  Content->setSorting(-1);    // no sorting
  Content->addColumn(tr("Content of"));
  Content->addColumn(tr("Note"));
  Content->setColumnWidthMode(0,QListView::Manual);
  Content->setColumnWidth(0, 150);

  initContentListView();
  TabView->addPage(Content,tr("Content"));
  TabView->setTabToolTip(TabView->id(Content), tr("content of current project"));

  connect(Content, SIGNAL(doubleClicked(QListViewItem*)),
		   SLOT(slotOpenContent(QListViewItem*)));
  connect(Content, SIGNAL(clicked(QListViewItem*)),
		   SLOT(slotSelectSubcircuit(QListViewItem*)));
  connect(Content, SIGNAL(expanded(QListViewItem*)),
		   SLOT(slotExpandContentList(QListViewItem*)));

  // ----------------------------------------------------------
  // "Component Tab" of the left QTabWidget
  QVBox *CompGroup  = new QVBox(this);
  CompChoose = new QComboBox(CompGroup);
  CompComps  = new myIconView(CompGroup);
  TabView->addPage(CompGroup,tr("Components"));
  TabView->setTabToolTip(TabView->id(CompGroup), tr("components and diagrams"));
  fillComboBox(true);

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(clicked(QIconViewItem*)),
		     SLOT(slotSelectComponent(QIconViewItem*)));
  dock->setWidget(TabView);
  setDockEnabled(dock,DockTop,false);
  setDockEnabled(dock,DockBottom,false);
  moveDockWindow(dock,DockLeft);
  TabView->setCurrentPage(0);

  // ----------------------------------------------------------
  // Octave docking window
  octDock = new QDockWindow(QDockWindow::InDock, this);
  octDock->setCloseMode(QDockWindow::Always);
  connect(octDock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleOctave(bool)));
  octave = new OctaveWindow(octDock);
  moveDockWindow(octDock, Qt::DockBottom);

  // ............................................
  readProjects(); // reads all projects and inserts them into the ListBox
}

// ---------------------------------------------------------------
// Returns a pointer to the QucsDoc object whose number is "No".
// If No < 0 then a pointer to the current document is returned.
QucsDoc* QucsApp::getDoc(int No)
{
  QWidget *w;
  if(No < 0)
    w = DocumentTab->currentPage();
  else
    w = DocumentTab->page(No);

  if(w) {
    if(isTextDocument (w))
      return (QucsDoc*) ((TextDoc*)w);
    else
      return (QucsDoc*) ((Schematic*)w);
  }

  return 0;
}

// ---------------------------------------------------------------
// Returns a pointer to the QucsDoc object whose file name is "Name".
QucsDoc * QucsApp::findDoc (QString File, int * Pos)
{
  QucsDoc * d;
  int No = 0;
  File = QDir::convertSeparators (File);
  while ((d = getDoc (No++)) != 0)
    if (QDir::convertSeparators (d->DocName) == File) {
      if (Pos) *Pos = No - 1;
      return d;
    }
  return 0;
}

// ---------------------------------------------------------------
// Put the component groups into the ComboBox. It is possible to
// only put the paintings in it, because of "symbol painting mode".
void QucsApp::fillComboBox (bool setAll)
{
  CompChoose->setSizeLimit (11); // Increase this if you add items below.
  CompChoose->clear ();

  QStringList cats = Category::getCategories ();
  for (QStringList::Iterator it = cats.begin (); it != cats.end (); ++it) {
    if (*it != QObject::tr("paintings")) {
      if (setAll) CompChoose->insertItem (*it);
    }
    else CompChoose->insertItem (*it);
  }
}

// ----------------------------------------------------------
// Whenever the Component Library ComboBox is changed, this slot fills the
// Component IconView with the appropriat components.
void QucsApp::slotSetCompView (int index)
{
  editText->setHidden (true); // disable text edit of component property

  QPtrList<Module> Comps;
  CompComps->clear ();   // clear the IconView
  if (CompChoose->count () <= 0) return;
  QString item = CompChoose->text (index);
  if ((index + 1) >= CompChoose->count ()) // because of symbol edit mode
    Comps = Category::getModules (QObject::tr("paintings"));
  else
    Comps = Category::getModules (item);

  char * File;
  QString Name;
  Module * Mod;
  for (Mod = Comps.first(); Mod; Mod = Comps.next ()) {
    if (Mod->info) {
      *(Mod->info) (Name, File, false);
      new QIconViewItem (CompComps, Name,
		QImage (QucsSettings.BitmapDir + QString (File) + ".png"));
    }
  }
}

// ------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QIconViewItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem  = 0;   // no component/diagram/painting selected

  if(item == 0) {   // mouse button pressed not over an item ?
    CompComps->clearSelection();  // deselect component in ViewList
    return;
  }

  if(view->drawn)
    ((QScrollView*)DocumentTab->currentPage())->viewport()->update();
  view->drawn = false;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  pInfoFunc Infos = 0;
  int i = CompComps->index (item);
  QPtrList<Module> Comps;
  if((CompChoose->currentItem()+1) >= CompChoose->count())
    // the only one in "symbol-painting" mode
    Comps = Category::getModules (QObject::tr("paintings"));
  else
    Comps = Category::getModules (CompChoose->currentText ());
  Infos = Comps.at(i)->info;

  char * Dummy2;
  QString Dummy1;
  if (Infos)
    view->selElem = (*Infos) (Dummy1, Dummy2, true);
}

// ####################################################################
// #####  Functions for the menu that appears when left-clicking  #####
// #####  on a file in the "Content" ListView.                    #####
// ####################################################################

void QucsApp::initCursorMenu()
{
  ContentMenu = new QPopupMenu(Content);
  ContentMenu->insertItem(tr("Open"), this, SLOT(slotCMenuOpen()));
  ContentMenu->insertItem(tr("Rename"), this, SLOT(slotCMenuRename()));
  ContentMenu->insertItem(tr("Delete"), this, SLOT(slotCMenuDelete()));
  ContentMenu->insertItem(tr("Delete Group"), this, SLOT(slotCMenuDelGroup()));

  connect(Content,
	  SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
	  SLOT(slotShowContentMenu(QListViewItem*, const QPoint&, int)));
}

// ----------------------------------------------------------
// Shows the menu.
void QucsApp::slotShowContentMenu(QListViewItem *item, const QPoint& point, int)
{
  if(item)
    if(item->parent() != 0) {   // no component, but item "schematic", ...
      if(item->parent()->nextSibling()) // "Others" section in listview ?
        ContentMenu->setItemEnabled(ContentMenu->idAt(3), true);
      else
        ContentMenu->setItemEnabled(ContentMenu->idAt(3), false);
      ContentMenu->popup(point);
    }
}

// ----------------------------------------------------------
void QucsApp::slotCMenuOpen()
{
  QListViewItem *Item = Content->selectedItem();
  if(Item == 0) return;

  slotOpenContent(Item);
}

// ----------------------------------------------------------
void QucsApp::slotCMenuRename()
{
  QListViewItem *Item = Content->selectedItem();
  if(!Item) return;

  QString Name = Item->text(0);
  if (findDoc (QucsWorkDir.filePath(Name))) {
    QMessageBox::critical(this, tr("Error"),
			        tr("Cannot rename an open file!"));
    return;
  }

  QString Suffix = Name.section('.',-1);   // remember suffix
  QString Base   = Name.section('.',0,-2);
  if(Base.isEmpty()) Base = Name;

  bool ok;
  QString s = QInputDialog::getText(tr("Rename file"), tr("Enter new name:"),
		QLineEdit::Normal, Base, &ok, this);
  if(!ok) return;
  if(s.isEmpty()) return;

  QString NewName;
  if(s.contains('.'))
    NewName = s;
  else
    NewName = s+"."+Suffix;
  QDir file(QucsWorkDir.path());
  if(!file.rename(Name, NewName)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot rename file: ")+Name);
    return;
  }
  Item->setText(0, NewName);
}

// ----------------------------------------------------------
void QucsApp::slotCMenuDelete()
{
  QListViewItem *item = Content->selectedItem();
  if(item == 0) return;
  QString FileName = QucsWorkDir.filePath(item->text(0));

  if (findDoc (FileName)) {
    QMessageBox::critical(this, tr("Error"),
			        tr("Cannot delete an open file!"));
    return;
  }

  int No;
  No = QMessageBox::warning(this, tr("Warning"),
             tr("This will delete the file permanently! Continue ?"),
             tr("No"), tr("Yes"));
  if(No != 1) return;

  if(!QFile::remove(FileName)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete schematic: ")+item->text(0));
    return;
  }

  item->parent()->takeItem(item);
  delete item;
}

// ----------------------------------------------------------
QString QucsApp::fileType (const QString& Ext)
{
  QString Type = tr("unknown");
  if (Ext == "v")
    Type = tr("Verilog source");
  else if (Ext == "va")
    Type = tr("Verilog-A source");
  else if (Ext == "vhd" || Ext == "vhdl")
    Type = tr("VHDL source");
  else if (Ext == "dat")
    Type = tr("data file");
  else if (Ext == "dpl")
    Type = tr("data display");
  else if (Ext == "sch")
    Type = tr("schematic");
  else if (Ext == "sym")
    Type = tr("symbol");
  else if (Ext == "vhdl.cfg" || Ext == "vhdl.cfg")
    Type = tr("VHDL configuration");
  else if (Ext == "cfg")
    Type = tr("configuration");
  return Type;
}

// ----------------------------------------------------------
// Deletes all files with that name (and suffix sch, dpl, dat, vhdl, etc.).
void QucsApp::slotCMenuDelGroup ()
{
  QListViewItem * item = Content->selectedItem ();
  if (item == 0)
    return;
  QString s = item->text (0);
  s = QucsDoc::fileBase (s); // cut off suffix from file name

  const char * extensions[] =
    { "sch", "dpl", "dat", "vhdl", "vhd", "v", "sym",
      "vhdl.cfg", "vhd.cfg", "va", 0 };

  int i;
  for (i = 0; extensions[i] != 0; i++) {
    QString Short = s + "." + extensions[i];
    QString Name = QucsWorkDir.filePath (Short);
    // search, if files are open
    if (findDoc (Name)) {
      QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete the open file \"%1\"!").arg(Short));
      return;
    }
  }

  // check existence of files
  QString Str = "\n";
  for (i = 0; extensions[i] != 0; i++) {
    QString Short = s + "." + extensions[i];
    QString Long = QucsWorkDir.filePath (Short);
    bool exists = QFile::exists (Long);
    if (exists)
      Str += Short + "\n";
  }
  int No;
  No = QMessageBox::warning (this, tr("Warning"),
	tr("This will delete the files%1permanently! Continue ?").arg(Str),
	tr("No"), tr("Yes"));
  if (No != 1)
    return;

  // file removal
  for (i = 0; extensions[i] != 0; i++) {
    QString Short = s + "." + extensions[i];
    QString Name = QucsWorkDir.filePath (Short);
    bool exists = QFile::exists (Name);
    if (exists) {
      // remove files
      if (!QFile::remove (Name)) {
	QMessageBox::critical(this, tr("Error"),
		tr("Cannot delete %1: \"%2\"!").arg(fileType (extensions[i])).
	        arg(Short));
	continue;
      }
      // remove items from listview
      item = Content->findItem (Short, 0);
      if (item) {
	item->parent()->takeItem (item);
	delete item;
      }
    }
  }
}


// ################################################################
// #####    Functions that handle the project operations.     #####
// ################################################################

// Checks for qucs directory and reads all existing Qucs projects.
void QucsApp::readProjects()
{
  QDir ProjDir(QDir::homeDirPath());
  if(!ProjDir.cd(".qucs")) { // work directory exists ?
    if(!ProjDir.mkdir(".qucs")) { // no, then create it
      QMessageBox::warning(this, tr("Warning"),
                   tr("Cannot create work directory !"));
      return;
    }
    ProjDir.cd(".qucs");
  }

  // get all directories
  QStringList PrDirs = ProjDir.entryList("*", QDir::Dirs, QDir::Name);
  PrDirs.pop_front(); // delete "." from list
  PrDirs.pop_front(); // delete ".." from list

  Projects->clear();
  QStringList::iterator it;
  // inserts all project directories
  for(it = PrDirs.begin(); it != PrDirs.end(); it++)
     if ((*it).right(4) == "_prj") {  // project directories end with "_prj"
       (*it) = (*it).left((*it).length()-4); // remove "_prj" from name
       Projects->insertItem(*it);
     }
}

// ----------------------------------------------------------
// Is called, when "Create New Project" button is pressed.
void QucsApp::slotProjNewButt()
{
  editText->setHidden(true); // disable text edit of component property

  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir projDir(QucsHomeDir.path());
  if(projDir.mkdir(d->ProjName->text()+"_prj")) {
    Projects->insertItem(d->ProjName->text(),0);  // at first position
    if(d->OpenProj->isChecked())
      slotOpenProject(Projects->firstItem());
  }
  else QMessageBox::information(this, tr("Info"),
                    tr("Cannot create project directory !"));
}

// ----------------------------------------------------------
// Checks whether this file is a qucs file and whether it is an subcircuit.
// It returns the number of subcircuit ports.
int QucsApp::testFile(const QString& DocName)
{
  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    return -1;
  }

  QString Line;
  // .........................................
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.read();
  file.close();
  QTextStream stream(&FileString, IO_ReadOnly);


  // read header ........................
  do {
    if(stream.atEnd()) {
      file.close();
      return -2;
    }
    Line = stream.readLine();
    Line = Line.stripWhiteSpace();
  } while(Line.isEmpty());
  
  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    return -3;
  }

  Line = Line.mid(16, Line.length()-17);
  if(!checkVersion(Line)) { // wrong version number ?
    file.close();
    return -4;
  }

  // read content ....................
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Components>") break;
  }

  int z=0;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "</Components>") {
      file.close();
      return z;       // return number of ports
    }

    Line = Line.stripWhiteSpace();
    QString s = Line.section(' ',0,0);    // component type
    if(s == "<Port") z++;
  }
  return -5;  // component field not closed
}

// ----------------------------------------------------------
// Reads all files in the project directory and sort them into the
// content ListView
void QucsApp::readProjectFiles()
{
  // Delete the content files, but don't delete the parent items !!!
  while(ConSchematics->firstChild())
    delete ConSchematics->firstChild();
  while(ConDisplays->firstChild())
    delete ConDisplays->firstChild();
  while(ConDatasets->firstChild())
    delete ConDatasets->firstChild();
  while(ConSources->firstChild())
    delete ConSources->firstChild();
  while(ConVerilog->firstChild())
    delete ConVerilog->firstChild();
  while(ConVerilogA->firstChild())
    delete ConVerilogA->firstChild();
  while(ConOthers->firstChild())
    delete ConOthers->firstChild();
  while(ConOctave->firstChild())
    delete ConOctave->firstChild();

  int n;
  // put all files into "Content"-ListView
  QStringList Elements = QucsWorkDir.entryList("*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  QString Str;
  for(it = Elements.begin(); it != Elements.end(); ++it) {
    Str = QucsDoc::fileSuffix (*it);
    if(Str == "sch") {
      n = testFile(QucsWorkDir.filePath((*it).ascii()));
      if(n >= 0) {
        if(n > 0)
          new QListViewItem(ConSchematics, (*it).ascii(),
                            QString::number(n)+tr("-port"));
        else new QListViewItem(ConSchematics, (*it).ascii());
      }
    }
    else if(Str == "dpl")
      new QListViewItem(ConDisplays, (*it).ascii());
    else if(Str == "dat")
      new QListViewItem(ConDatasets, (*it).ascii());
    else if((Str == "vhdl") || (Str == "vhd"))
      new QListViewItem(ConSources, (*it).ascii());
    else if(Str == "v")
      new QListViewItem(ConVerilog, (*it).ascii());
    else if(Str == "va")
      new QListViewItem(ConVerilogA, (*it).ascii());
    else if((Str == "m") || (Str == "oct"))
      new QListViewItem(ConOctave, (*it).ascii());
    else
      new QListViewItem(ConOthers, (*it).ascii());
  }
}

// ----------------------------------------------------------
// Opens an existing project.
void QucsApp::openProject(const QString& Path, const QString& Name)
{
  editText->setHidden(true); // disable text edit of component property

  if(!closeAllFiles()) return;   // close files and ask for saving them
  new Schematic(this, "");
  view->drawn = false;

  slotResetWarnings();

  QDir ProjDir(QDir::cleanDirPath(Path));
  if(!ProjDir.exists() || !ProjDir.isReadable()) { // check project directory
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: ")+Path);
    return;
  }
  QucsWorkDir.setPath(ProjDir.path());
  octave->adjustDirectory();

  Content->setColumnText(0,tr("Content of '")+Name+tr("'")); // column text
  ConSchematics->setOpen(false); // get sure to have it closed
  ConSchematics->setOpen(true);  // also calls readProjectFiles()

  TabView->setCurrentPage(1);   // switch to "Content"-Tab
  ProjName = Name;   // remember the name of project

  // show name in title of main window
  setCaption("Qucs " PACKAGE_VERSION + tr(" - Project: ")+Name);
}

// ----------------------------------------------------------
// Is called when the open project menu is called.
void QucsApp::slotMenuOpenProject()
{
  QFileDialog *d = new QFileDialog(QucsHomeDir.path());
  d->setCaption(tr("Choose Project Directory for Opening"));
  d->setShowHiddenFiles(true);
  d->setMode(QFileDialog::DirectoryOnly);
  if(d->exec() != QDialog::Accepted) return;

  QString s = d->selectedFile();
  if(s.isEmpty()) return;

  s = s.left(s.length()-1);   // cut off trailing '/'
  int i = s.findRev('/');
  if(i > 0) s = s.mid(i+1);   // cut out the last subdirectory
  s.remove("_prj");
  openProject(d->selectedFile(), s);
}

// ----------------------------------------------------------
// Is called, when "Open Project" button is pressed.
void QucsApp::slotProjOpenButt()
{
  editText->setHidden(true); // disable text edit of component property

  QListBoxItem *item = Projects->selectedItem();
  if(item) slotOpenProject(item);
  else QMessageBox::information(this, tr("Info"),
				tr("No project is selected !"));
}

// ----------------------------------------------------------
// Is called when project is double-clicked to open it.
void QucsApp::slotOpenProject(QListBoxItem *item)
{
  openProject(QucsHomeDir.filePath(item->text()+"_prj"), item->text());
}

// ----------------------------------------------------------
// Is called when the close project menu is called.
void QucsApp::slotMenuCloseProject()
{
  editText->setHidden(true); // disable text edit of component property

  if(!closeAllFiles()) return;   // close files and ask for saving them
  new Schematic(this, "");
  view->drawn = false;

  slotResetWarnings();
  setCaption("Qucs " PACKAGE_VERSION + tr(" - Project: "));
  QucsWorkDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));
  octave->adjustDirectory();

  Content->setColumnText(0,tr("Content of")); // column text
  initContentListView();

  TabView->setCurrentPage(0);   // switch to "Projects"-Tab
  ProjName = "";
}

// ----------------------------------------------------------
bool QucsApp::deleteDirectoryContent(QDir& Dir)
{
  // removes every file, remove("*") does not work
  QStringList Files = Dir.entryList("*", QDir::Files);  // all files
  QStringList::iterator it;
  for(it = Files.begin(); it != Files.end(); it++) {
     if(!Dir.remove(*it)) {
       QMessageBox::information(this, tr("Info"),
				tr("Cannot delete file: ")+(*it));
       return false;
     }
  }

  QDir myDir(Dir);
  // Remove all directories recursively.
  Files = Dir.entryList("*", QDir::Dirs);
  Files.pop_front();  // delete "." from list
  Files.pop_front();  // delete ".." from list
  for(it = Files.begin(); it != Files.end(); it++) {
     myDir.cd(*it);
     if(!deleteDirectoryContent(myDir))
       return false;
     myDir.cdUp();
     if(!myDir.rmdir(*it)) {
       QMessageBox::information(this, tr("Info"),
				tr("Cannot remove directory: ")+(*it));
       return false;
    }
  }
  return true;
}

// ----------------------------------------------------------
bool QucsApp::deleteProject(const QString& Path, const QString& Name)
{
  editText->setHidden(true); // disable text edit of component property

  if(Name == ProjName) {
    QMessageBox::information(this, tr("Info"),
			tr("Cannot delete an open project !"));
    return false;
  }

  // first ask, if really delete project ?
  if(QMessageBox::warning(this, tr("Warning"),
     tr("This will destroy all the project files permanently ! Continue ?"),
     tr("&Yes"), tr("&No"), 0,1,1))  return false;

  QDir projDir = QDir(Path);
  if(!deleteDirectoryContent(projDir))
    return false;

  projDir.cdUp();  // leave project directory for deleting
  if(!projDir.rmdir(Name+"_prj")) {
    QMessageBox::information(this, tr("Info"),
			     tr("Cannot remove project directory !"));
    return false;
  }

  return true;
}

// ----------------------------------------------------------
// Is called, when "Delete Project" menu is activated.
void QucsApp::slotMenuDelProject()
{
  QFileDialog *d = new QFileDialog(QucsHomeDir.path());
  d->setCaption(tr("Choose Project Directory for Deleting"));
  d->setShowHiddenFiles(true);
  d->setMode(QFileDialog::DirectoryOnly);
  if(d->exec() != QDialog::Accepted) return;

  QString s = d->selectedFile();
  if(s.isEmpty()) return;

  s = s.left(s.length()-1);  // cut off trailing '/'
  int i = s.findRev('/');
  if(i > 0) s = s.mid(i+1);  // cut out the last subdirectory
  s = s.left(s.length()-4);  // remove "_prj" from name
  deleteProject(d->selectedFile(), s);
  readProjects();   // re-reads all projects and inserts them into the ListBox
}

// ----------------------------------------------------------
// Is called, when "Delete Project" button is pressed.
void QucsApp::slotProjDelButt()
{
  QListBoxItem *item = Projects->selectedItem();
  if(!item) {
    QMessageBox::information(this, tr("Info"),
			     tr("No project is selected !"));
    return;
  }

  if(!deleteProject(QucsHomeDir.filePath(item->text()+"_prj"),
	item->text()))  return;
  Projects->removeItem(Projects->currentItem());  // remove from project list
}


// ################################################################
// #####  Functions that handle the file operations for the   #####
// #####  documents.                                          #####
// ################################################################

void QucsApp::slotFileNew()
{
  statusBar()->message(tr("Creating new schematic..."));
  editText->setHidden(true); // disable text edit of component property

  new Schematic(this, "");

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotTextNew()
{
  statusBar()->message(tr("Creating new text editor..."));
  editText->setHidden(true); // disable text edit of component property

  new TextDoc(this, "");

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
// Changes to the document "Name". If already open then it goes to it
// directly, otherwise it loads it.
bool QucsApp::gotoPage(const QString& Name)
{
  int No = DocumentTab->currentPageIndex();

  int i = 0;
  QucsDoc * d = findDoc (Name, &i);  // search, if page is already loaded

  if(d) {   // open page found ?
    d->becomeCurrent(true);
    DocumentTab->setCurrentPage(i);  // make new document the current
    return true;
  }

  QFileInfo Info(Name);
  if(Info.extension(false) == "sch" || Info.extension(false) == "dpl" || 
     Info.extension(false) == "sym")
    d = new Schematic(this, Name);
  else
    d = new TextDoc(this, Name);

  if(!d->load()) {    // load document if possible
    delete d;
    DocumentTab->setCurrentPage(No);
    view->drawn = false;
    return false;
  }
  slotChangeView(DocumentTab->currentPage());

  // if only an untitled document was open -> close it
  if(getDoc(0)->DocName.isEmpty())
    if(!getDoc(0)->DocChanged)
      delete DocumentTab->page(0);

  view->drawn = false;
  return true;
}

QString lastDirOpenSave; // to remember last directory and file

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  editText->setHidden(true); // disable text edit of component property

  statusBar()->message(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(
	lastDirOpenSave.isEmpty() ? QString(".") : lastDirOpenSave,
	QucsFileFilter, this, 0, tr("Enter a Schematic Name"));

  if(s.isEmpty())
    statusBar()->message(tr("Opening aborted"), 2000);
  else {
    gotoPage(s);
    lastDirOpenSave = s;   // remember last directory and file
    statusBar()->message(tr("Ready."));
  }
}

// --------------------------------------------------------------
bool QucsApp::saveFile(QucsDoc *Doc)
{
  if(!Doc)
    Doc = getDoc();

  if(Doc->DocName.isEmpty())
    return saveAs();

  int Result = Doc->save();
  if(Result < 0)  return false;

  updatePortNumber(Doc, Result);
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSave()
{
  statusBar()->message(tr("Saving file..."));
  DocumentTab->blockSignals(true);   // no user interaction during that time
  editText->setHidden(true); // disable text edit of component property

  if(!saveFile()) {
    DocumentTab->blockSignals(false);
    statusBar()->message(tr("Saving aborted"), 2000);
    statusBar()->message(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
bool QucsApp::saveAs()
{
  QWidget *w = DocumentTab->currentPage();
  QucsDoc *Doc = getDoc();

  int n = -1;
  bool intoView = true;
  QString s, Filter;
  QFileInfo Info;
  while(true) {
    intoView = true;
    s = Doc->DocName;
    Info.setFile(s);
    if(s.isEmpty()) {   // which is default directory ?
      if(ProjName.isEmpty()) {
        if(lastDirOpenSave.isEmpty())  s = QDir::currentDirPath();
        else  s = lastDirOpenSave;
      }
      else s = QucsWorkDir.path();
    }

    // list of known file extensions
    QString ext = "vhdl;vhd;v;va;sch;dpl;m;oct";
    QStringList extlist = QStringList::split (';', ext);

    if(isTextDocument (w))
      Filter = tr("VHDL Sources")+" (*.vhdl *.vhd);;" +
	       tr("Verilog Sources")+" (*.v);;"+
	       tr("Verilog-A Sources")+" (*.va);;"+
	       tr("Octave Scripts")+" (*.m *.oct);;"+
	       tr("Any File")+" (*)";
    else
      Filter = QucsFileFilter;
    s = QFileDialog::getSaveFileName(s, Filter,
                     this, "", tr("Enter a Document Name"));
    if(s.isEmpty())  return false;
    Info.setFile(s);               // try to guess the best extension ...
    ext = Info.extension(false);
    if(ext.isEmpty() ||
       !extlist.contains(ext)) {   // ... if no one was specified or is unknown
      if(isTextDocument (w))
        s += ".vhdl";
      else
        s += ".sch";
    }

    Info.setFile(s);
    if(QFile::exists(s)) {
      n = QMessageBox::warning(this, tr("Warning"),
		tr("The file '")+Info.fileName()+tr("' already exists!\n")+
		tr("Saving will overwrite the old one! Continue?"),
		tr("No"), tr("Yes"), tr("Cancel"));
      if(n == 2) return false;    // cancel
      if(n == 0) continue;
      intoView = false;    // file already exists
    }

    // search, if document is open
    QucsDoc * d = findDoc (s);
    if(d) {
      QMessageBox::information(this, tr("Info"),
		tr("Cannot overwrite an open document"));
      return false;
    }

    break;
  }
  Doc->setName(s);
  lastDirOpenSave = Info.dirPath(true);  // remember last directory and file

  if(intoView) {    // insert new name in Content ListView ?
    if(Info.dirPath(true) == QucsWorkDir.absPath())
      if(!ProjName.isEmpty()) {
        s = Info.fileName();  // remove path from file name
	QString ext = Info.extension (false);
        if(ext == "sch")
          Content->setSelected(new QListViewItem(ConSchematics, s), true);
        else if(ext == "dpl")
          Content->setSelected(new QListViewItem(ConDisplays, s), true);
        else if(ext == "dat")
          Content->setSelected(new QListViewItem(ConDatasets, s), true);
        else if((ext == "vhdl") || (ext == "vhd"))
          Content->setSelected(new QListViewItem(ConSources, s), true);
        else if(ext == "v")
          Content->setSelected(new QListViewItem(ConVerilog, s), true);
        else if(ext == "va")
          Content->setSelected(new QListViewItem(ConVerilogA, s), true);
        else if(ext == "m" || ext == "oct")
          Content->setSelected(new QListViewItem(ConOctave, s), true);
        else
          Content->setSelected(new QListViewItem(ConOthers, s), true);
      }
  }

  n = Doc->save();   // SAVE
  if(n < 0)  return false;

  updatePortNumber(Doc, n);
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSaveAs()
{
  statusBar()->message(tr("Saving file under new filename..."));
  DocumentTab->blockSignals(true);   // no user interaction during the time
  editText->setHidden(true); // disable text edit of component property

  if(!saveAs()) {
    DocumentTab->blockSignals(false);
    statusBar()->message(tr("Saving aborted"), 3000);
    statusBar()->message(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->message(tr("Ready."));
}


// --------------------------------------------------------------
void QucsApp::slotFileSaveAll()
{
  statusBar()->message(tr("Saving all files..."));
  editText->setHidden(true); // disable text edit of component property
  DocumentTab->blockSignals(true);   // no user interaction during the time

  int No=0;
  QucsDoc *Doc;  // search, if page is already loaded
  while((Doc=getDoc(No++)) != 0) {
    if(Doc->DocName.isEmpty())  // make document the current ?
      DocumentTab->setCurrentPage(No-1);
    saveFile(Doc);
  }

  DocumentTab->blockSignals(false);
  // Call update() to update subcircuit symbols in current document.
  ((QScrollView*)DocumentTab->currentPage())->viewport()->update();
  view->drawn = false;
  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotFileClose()
{
  statusBar()->message(tr("Closing file..."));
  editText->setHidden(true); // disable text edit of component property

  QucsDoc *Doc = getDoc();
  if(Doc->DocChanged) {
    switch(QMessageBox::warning(this,tr("Closing Qucs document"),
      tr("The document contains unsaved changes!\n")+
      tr("Do you want to save the changes before closing?"),
      tr("&Save"), tr("&Discard"), tr("Cancel"), 0, 2)) {
      case 0 : slotFileSave();
               break;
      case 2 : return;
    }
  }

  delete Doc;

  if(DocumentTab->count() < 1)  // if no document left, create an untitled
    new Schematic(this, "");

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------
bool QucsApp::closeAllFiles()
{
  SaveDialog *sd = new SaveDialog(this);
  sd->setApp(this);
  for(int i=0; i < DocumentTab->count(); ++i) {
    QucsDoc *doc = getDoc(i);
    if(doc->DocChanged)
      sd->addUnsavedDoc(doc);
  }
  int Result = SaveDialog::DontSave;
  if(!sd->isEmpty())
     Result = sd->exec();
  delete sd;

  if(Result == SaveDialog::AbortClosing)
    return false;

  QucsDoc *doc = 0;
  while((doc = getDoc()) != 0)
    delete doc;

  switchEditMode(true);   // set schematic edit mode
  return true;
}   

// --------------------------------------------------------------
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(QWidget *w)
{
  editText->setHidden (true); // disable text edit of component property

  QucsDoc * Doc;

  // for text documents
  if (isTextDocument (w)) {
    TextDoc *d = (TextDoc*)w;
    Doc = (QucsDoc*)d;
    // update menu entries, etc. if neccesary
    if(mainAccel->isEnabled())
      switchSchematicDoc (false);
  }
  // for schematic documents
  else {
    Schematic *d = (Schematic*)w;
    Doc = (QucsDoc*)d;
    // already in schematic?
    if(mainAccel->isEnabled()) {
      // which mode: schematic or symbol editor ?
      if((CompChoose->count() > 1) == d->symbolMode)
        changeSchematicSymbolMode (d);
    }
    else {
      switchSchematicDoc(true);
      changeSchematicSymbolMode(d);
    }
  }

  Doc->becomeCurrent(true);
  view->drawn = false;

  if(!HierarchyHistory.isEmpty())
    if(*(HierarchyHistory.getLast()) != "*") {
      HierarchyHistory.clear();   // no subcircuit history anymore
      popH->setEnabled(false);
    }
}

// --------------------------------------------------------------
// Changes to the next document in the TabBar.
void QucsApp::slotNextTab()
{
  int No = DocumentTab->currentPageIndex() + 1;
  if(No >= DocumentTab->count())
    No = 0;

  // make new document the current (calls "slotChangeView(int)" indirectly)
  DocumentTab->setCurrentPage(No);
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotFileSettings ()
{
  editText->setHidden (true); // disable text edit of component property

  QWidget * w = DocumentTab->currentPage ();
  if (isTextDocument (w)) {
    QucsDoc * Doc = (QucsDoc *) ((TextDoc *) w);
    QString ext = Doc->fileSuffix ();
    // Octave properties
    if (ext == "m" || ext == "oct") {
    }
    // Verilog-A properties
    else if (ext == "va") {
      VASettingsDialog * d = new VASettingsDialog ((TextDoc *) w);
      d->exec ();
    }
    // VHDL and Verilog-HDL properties
    else {
      DigiSettingsDialog * d = new DigiSettingsDialog ((TextDoc *) w);
      d->exec ();
    }
  }
  // schematic properties
  else {
    SettingsDialog * d = new SettingsDialog ((Schematic *) w);
    d->exec ();
  }
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotApplSettings()
{
  editText->setHidden(true); // disable text edit of component property

  QucsSettingsDialog *d = new QucsSettingsDialog(this);
  d->exec();
  view->drawn = false;
}


// --------------------------------------------------------------
void QucsApp::updatePortNumber(QucsDoc *currDoc, int No)
{
  if(No<0) return;

  QString pathName = currDoc->DocName;
  QString ext = currDoc->fileSuffix ();
  QFileInfo Info (pathName);
  QString Model, File, Name = Info.fileName();

  if (ext == "sch") {
    Model = "Sub";

    // enter new port number into ListView
    QListViewItem *p;
    for(p = ConSchematics->firstChild(); p!=0; p = p->nextSibling()) {
      if(p->text(0) == Name) {
        if(No == 0) p->setText(1,"");
        else p->setText(1,QString::number(No)+tr("-port"));
        break;
      }
    }
  }
  else if (ext == "vhdl" || ext == "vhd") {
    Model = "VHDL";
  }
  else if (ext == "v") {
    Model = "Verilog";
  }

  // update all occurencies of subcircuit in all open documents
  No = 0;
  QWidget *w;
  Component *pc_tmp;
  while((w=DocumentTab->page(No++)) != 0) {
    if(isTextDocument (w))  continue;

    // start from the last to omit re-appended components
    Schematic *Doc = (Schematic*)w;
    for(Component *pc=Doc->Components->last(); pc!=0; ) {
      if(pc->Model == Model) {
        File = pc->Props.getFirst()->Value;
        if((File == pathName) || (File == Name)) {
          pc_tmp = Doc->Components->prev();
          Doc->recreateComponent(pc);  // delete and re-append component
          if(!pc_tmp)  break;
          Doc->Components->findRef(pc_tmp);
          pc = Doc->Components->current();
          continue;
        }
      }
      pc = Doc->Components->prev();
    }
  }
}


// --------------------------------------------------------------
void QucsApp::printCurrentDocument(bool fitToPage)
{
  statusBar()->message(tr("Printing..."));
  editText->setHidden(true); // disable text edit of component property

  if(isTextDocument (DocumentTab->currentPage()))
    Printer->setOrientation(QPrinter::Portrait);
  else
    Printer->setOrientation(QPrinter::Landscape);

  //Printer->setPrintRange(QPrinter::AllPages);

  if(Printer->setup(this)) {   // printer dialog

    QPainter Painter(Printer);
    if(!Painter.device())      // valid device available ?
      goto Error;

    for(int z=Printer->numCopies(); z>0 ; z--) {
      if(Printer->aborted())
        break;

      getDoc()->print(Printer, &Painter,
              Printer->printRange() == QPrinter::AllPages, fitToPage);
      if(z > 1)
        if(!Printer->newPage())
          goto Error;
    }
  }

  statusBar()->message(tr("Ready."));
  return;

Error:
  statusBar()->message(tr("Printer Error."));
}

// --------------------------------------------------------------
void QucsApp::slotFilePrint()
{
  printCurrentDocument(false);
}

// --------------------------------------------------------------
// Fit printed content to page size.
void QucsApp::slotFilePrintFit()
{
  printCurrentDocument(true);
}

// --------------------------------------------------------------------
// Exits the application.
void QucsApp::slotFileQuit()
{
  statusBar()->message(tr("Exiting application..."));
  editText->setHidden(true); // disable text edit of component property

  int exit = QMessageBox::information(this,
      tr("Quit..."), tr("Do you really want to quit?"),
      tr("Yes"), tr("No"));

  if(exit == 0)
    if(closeAllFiles()) {
      emit signalKillEmAll();   // kill all subprocesses
      qApp->quit();
    }

  statusBar()->message(tr("Ready."));
}

//-----------------------------------------------------------------
// To get all close events.
void QucsApp::closeEvent(QCloseEvent* Event)
{
   if(closeAllFiles()) {
      emit signalKillEmAll();   // kill all subprocesses
      Event->accept();
      qApp->quit();
   }
   else
      Event->ignore();
}

// --------------------------------------------------------------------
void QucsApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(isTextDocument (Doc)) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->cut();
    return;
  }

  editText->setHidden(true); // disable text edit of component property
  QClipboard *cb = QApplication::clipboard();  // get system clipboard

  QString s = Doc->copySelected(true);
  if(!s.isEmpty()) {
    cb->setText(s, QClipboard::Clipboard);
    Doc->viewport()->update();
  }

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------------
void QucsApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(isTextDocument (Doc)) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->copy();
    return;
  }
  
  editText->setHidden(true); // disable text edit of component property
  QClipboard *cb = QApplication::clipboard();  // get system clipboard

  QString s = Doc->copySelected(false);
  if(!s.isEmpty())
    cb->setText(s, QClipboard::Clipboard);

  statusBar()->message(tr("Ready."));
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to go into a subcircuit.
void QucsApp::slotIntoHierarchy()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  Component *pc = Doc->searchSelSubcircuit();
  if(pc == 0) return;

  QString *ps = new QString("*");
  HierarchyHistory.append(ps);    // sign not to clear HierarchyHistory

  QString s = QucsWorkDir.filePath(pc->Props.getFirst()->Value);
  if(!gotoPage(s)) {
    HierarchyHistory.remove();
    return;
  }

  *(HierarchyHistory.getLast()) = Doc->DocName; // remember for the way back
  popH->setEnabled(true);
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to leave a subcircuit.
void QucsApp::slotPopHierarchy()
{
  editText->setHidden(true); // disable text edit of component property

  if(HierarchyHistory.count() == 0) return;

  QString Doc = *(HierarchyHistory.getLast());
  *(HierarchyHistory.last()) = "*";  // sign not to clear HierarchyHistory

  if(!gotoPage(Doc)) {
    *(HierarchyHistory.getLast()) = Doc;
    return;
  }

  HierarchyHistory.remove();
  if(HierarchyHistory.count() == 0)
    popH->setEnabled(false);
}

// --------------------------------------------------------------
void QucsApp::slotShowAll()
{
  editText->setHidden(true); // disable text edit of component property
  getDoc()->showAll();
}

// -----------------------------------------------------------
// Sets the scale factor to 1.
void QucsApp::slotShowOne()
{
  editText->setHidden(true); // disable text edit of component property
  getDoc()->showNoZoom();
}

// -----------------------------------------------------------
void QucsApp::slotZoomOut()
{
  editText->setHidden(true); // disable text edit of component property
  getDoc()->zoomBy(0.7f);
}

// -----------------------------------------------------------------------
// Is called when the simulate toolbar button is pressed.
void QucsApp::slotSimulate()
{
  editText->setHidden(true); // disable text edit of component property

  QucsDoc *Doc;
  QWidget *w = DocumentTab->currentPage();
  if(isTextDocument (w)) {
    Doc = (QucsDoc*)((TextDoc*)w);
    if(Doc->SimTime.isEmpty() && ((TextDoc*)Doc)->simulation) {
      DigiSettingsDialog *d = new DigiSettingsDialog((TextDoc*)Doc);
      if(d->exec() == QDialog::Rejected)
	return;
    }
  }
  else
    Doc = (QucsDoc*)((Schematic*)w);

  if(Doc->DocName.isEmpty()) // if document 'untitled' ...
    if(!saveAs()) return;    // ... save schematic before

  // Perhaps the document was modified from another program ?
  QFileInfo Info(Doc->DocName);
  if(Doc->lastSaved.isValid()) {
    if(Doc->lastSaved < Info.lastModified()) {
      int No = QMessageBox::warning(this, tr("Warning"),
               tr("The document was modified by another program !") + '\n' +
               tr("Do you want to reload or keep this version ?"),
               tr("Reload"), tr("Keep it"));
      if(No == 0)
        Doc->load();
    }
  }

  slotResetWarnings();

  if(Info.extension(false) == "m" || Info.extension(false) == "oct") {
    // It is an Octave script.
    if(Doc->DocChanged)
      Doc->save();
    slotViewOctaveDock(true);
    octave->runOctaveScript(Doc->DocName);
    return;
  }

  SimMessage *sim = new SimMessage(w, this);
  // disconnect is automatically performed, if one of the involved objects
  // is destroyed !
  connect(sim, SIGNAL(SimulationEnded(int, SimMessage*)), this,
		SLOT(slotAfterSimulation(int, SimMessage*)));
  connect(sim, SIGNAL(displayDataPage(QString&, QString&)),
		this, SLOT(slotChangePage(QString&, QString&)));

  sim->show();
  if(!sim->startProcess()) return;

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), sim, SLOT(slotClose()));
}

// ------------------------------------------------------------------------
// Is called after the simulation process terminates.
void QucsApp::slotAfterSimulation(int Status, SimMessage *sim)
{
  if(Status != 0) return;  // errors ocurred ?

  if(sim->ErrText->lines() > 1)   // were there warnings ?
    slotShowWarnings();

  int i=0;
  QWidget *w;  // search, if page is still open
  while((w=DocumentTab->page(i++)) != 0)
    if(w == sim->DocWidget)
      break;

  if(sim->showBias == 0) {  // paint dc bias into schematic ?
    sim->slotClose();   // close and delete simulation window
    if(w) {  // schematic still open ?
      SweepDialog *Dia = new SweepDialog((Schematic*)sim->DocWidget);
      Dia->show();
    }
  }
  else {
    if(sim->SimRunScript) {
      // run script
      octave->startOctave();
      octave->runOctaveScript(sim->Script);
    }
    if(sim->SimOpenDpl) {
      // switch to data display
      if(sim->DataDisplay.right(2) == ".m" ||
	 sim->DataDisplay.right(4) == ".oct") {  // Is it an Octave script?
	octave->startOctave();
	octave->runOctaveScript(sim->DataDisplay);
      }
      else
	slotChangePage(sim->DocName, sim->DataDisplay);
      sim->slotClose();   // close and delete simulation window
    }
    else
      if(w) if(!isTextDocument (sim->DocWidget))
	// load recent simulation data (if document is still open)
	((Schematic*)sim->DocWidget)->reloadGraphs();
  }

  if(!isTextDocument (sim->DocWidget))
    ((Schematic*)sim->DocWidget)->viewport()->update();

  // put all dataset files into "Content"-ListView (update)
/*  QStringList Elements = ProjDir.entryList("*.dat", QDir::Files, QDir::Name);
  for(it = Elements.begin(); it != Elements.end(); ++it)
    new QListViewItem(ConDatasets, (*it).ascii());*/
}

// ------------------------------------------------------------------------
void QucsApp::slotDCbias()
{
  getDoc()->showBias = 0;
  slotSimulate();
}

// ------------------------------------------------------------------------
// Changes to the corresponding data display page or vice versa.
void QucsApp::slotChangePage(QString& DocName, QString& DataDisplay)
{
  if(DataDisplay.isEmpty())  return;

  QFileInfo Info(DocName);
  QString Name = Info.dirPath() + QDir::separator() + DataDisplay;

  QWidget  *w = DocumentTab->currentPage();

  int z = 0;  // search, if page is already loaded
  QucsDoc * d = findDoc (Name, &z);

  if(d)
    DocumentTab->setCurrentPage(z);
  else {   // no open page found ?
    QString ext = QucsDoc::fileSuffix (DataDisplay);
    if (ext != "vhd" && ext != "vhdl" && ext != "v" && ext != "va" && 
	ext != "oct" && ext != "m")
      d = new Schematic (this, Name);
    else
      d = new TextDoc (this, Name);

    QFile file(Name);
    if(file.open(IO_ReadOnly)) {      // try to load document
      file.close();
      if(!d->load()) {
        delete d;
        view->drawn = false;
        return;
      }
    }
    else {
      if(file.open(IO_ReadWrite)) {  // if document doesn't exist, create
        new QListViewItem(ConDisplays, DataDisplay); // add new name
        d->DataDisplay = Info.fileName();
      }
      else {
        QMessageBox::critical(this, tr("Error"), tr("Cannot create ")+Name);
        return;
      }
      file.close();
    }

    d->becomeCurrent(true);
  }


  if(DocumentTab->currentPage() == w)      // if page not ...
    if(!isTextDocument (w))
      ((Schematic*)w)->reloadGraphs();  // ... changes, reload here !

  TabView->setCurrentPage (2);   // switch to "Component"-Tab
  if (Name.right(4) == ".dpl") {
    int i = Category::getModulesNr (QObject::tr("diagrams"));
    CompChoose->setCurrentItem (i);   // switch to diagrams
    slotSetCompView (i);
  }
}

// -------------------------------------------------------------------
// Changes to the data display of current page.
void QucsApp::slotToPage()
{
  QucsDoc *d = getDoc();
  if(d->DataDisplay.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No page set !"));
    return;
  }

  if(d->DocName.right(2) == ".m" ||
     d->DocName.right(4) == ".oct")
    slotViewOctaveDock(true);
  else
    slotChangePage(d->DocName, d->DataDisplay);
}

// -------------------------------------------------------------------
// Is called when a double-click is made in the content ListView.
void QucsApp::slotOpenContent(QListViewItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  if(item == 0) return;   // no item was double clicked
  if(item->parent() == 0) return; // no document, but item "schematic", ...

/*  
  QucsWorkDir.setPath(QucsHomeDir.path());
  QString p = ProjName+"_prj";
  if(!QucsWorkDir.cd(p)) {
    QMessageBox::critical(this, tr("Error"),
			  tr("Cannot access project directory: ")+
			  QucsWorkDir.path()+QDir::separator()+p);
    return;
  }*/

  QFileInfo Info(QucsWorkDir.filePath(item->text(0)));
  QString Suffix = Info.extension(false);

  if (Suffix == "sch" || Suffix == "dpl" || Suffix == "vhdl" ||
      Suffix == "vhd" || Suffix == "v" || Suffix == "va" ||
      Suffix == "m" || Suffix == "oct") {
    gotoPage(Info.absFilePath());

    if(item->text(1).isEmpty())     // is subcircuit ?
      if(Suffix == "sch") return;

    select->blockSignals(true);  // switch on the 'select' action ...
    select->setOn(true);
    select->blockSignals(false);

    activeAction = select;
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    return;
  }

  if(Suffix == "dat") {
    editFile(Info.absFilePath());  // open datasets with text editor
    return;
  }


  // File is no Qucs file, so go through list and search a user
  // defined program to open it.
  QStringList com;
  QStringList::Iterator it = QucsSettings.FileTypes.begin();
  while(it != QucsSettings.FileTypes.end()) {
    if(Suffix == (*it).section('/',0,0)) {
      com = QStringList::split(" ", (*it).section('/',1,1));
      com << Info.absFilePath();
      QProcess *Program = new QProcess(com);
      Program->setCommunication(0);
      if(!Program->start()) {
        QMessageBox::critical(this, tr("Error"),
               tr("Cannot start \"%1\"!").arg(Info.absFilePath()));
        delete Program;
      }
      return;
    }
    it++;
  }

  // If no appropriate program was found, open as text file.
  editFile(Info.absFilePath());  // open datasets with text editor
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectSubcircuit(QListViewItem *item)
{
  editText->setHidden(true); // disable text edit of component property

  if(item == 0) {   // mouse button pressed not over an item ?
    Content->clearSelection();  // deselect component in ListView
    return;
  }


  bool isVHDL = false;
  bool isVerilog = false;
  if(item->parent() == 0) return;
  if(item->parent()->text(0) == tr("Schematics")) {
    if(item->text(1).isEmpty())
      return;   // return, if not a subcircuit
  }
  else if(item->parent()->text(0) == tr("VHDL"))
    isVHDL = true;
  else if(item->parent()->text(0) == tr("Verilog"))
    isVerilog = true;
  else
    return;

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem = 0;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  Component *Comp;
  if(isVHDL)
    Comp = new VHDL_File();
  else if(isVerilog)
    Comp = new Verilog_File();
  else
    Comp = new Subcircuit();
  Comp->Props.first()->Value = item->text(0);
  Comp->recreate(0);
  view->selElem = Comp;

  if(view->drawn)
    ((QScrollView*)DocumentTab->currentPage())->viewport()->update();
  view->drawn = false;
  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
}

// ---------------------------------------------------------
// Is called when one of the Content ListView parents was expanded
// to show the files. It re-reads all files.
void QucsApp::slotExpandContentList(QListViewItem*)
{
  readProjectFiles();
}

// ---------------------------------------------------------
// This function is called if the document type changes, i.e.
// from schematic to text document or vice versa.
void QucsApp::switchSchematicDoc (bool SchematicMode)
{
  mainAccel->setEnabled (SchematicMode);

  // text document
  if (!SchematicMode) {
    if (activeAction) {
      activeAction->blockSignals (true); // do not call toggle slot
      activeAction->setOn (false);       // set last toolbar button off
      activeAction->blockSignals (false);
    }
    activeAction = select;
    select->blockSignals (true);
    select->setOn (true);
    select->blockSignals (false);
  }
  // schematic document
  else {
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  }

  selectMarker->setEnabled (SchematicMode);
  alignTop->setEnabled (SchematicMode);
  alignBottom->setEnabled (SchematicMode);
  alignLeft->setEnabled (SchematicMode);
  alignRight->setEnabled (SchematicMode);
  centerHor->setEnabled (SchematicMode);
  centerVert->setEnabled (SchematicMode);
  distrHor->setEnabled (SchematicMode);
  distrVert->setEnabled (SchematicMode);
  onGrid->setEnabled (SchematicMode);
  moveText->setEnabled (SchematicMode);
  filePrintFit->setEnabled (SchematicMode);
  editRotate->setEnabled (SchematicMode);
  editMirror->setEnabled (SchematicMode);
  editMirrorY->setEnabled (SchematicMode);
  intoH->setEnabled (SchematicMode);
  popH->setEnabled (SchematicMode);
  dcbias->setEnabled (SchematicMode);
  insWire->setEnabled (SchematicMode);
  insLabel->setEnabled (SchematicMode);
  insPort->setEnabled (SchematicMode);
  insGround->setEnabled (SchematicMode);
  insEquation->setEnabled (SchematicMode);
  setMarker->setEnabled (SchematicMode);

  editFind->setEnabled (!SchematicMode);
  editFindAgain->setEnabled (!SchematicMode);
  insEntity->setEnabled (!SchematicMode);
}

// ---------------------------------------------------------
void QucsApp::switchEditMode(bool SchematicMode)
{
  fillComboBox(SchematicMode);
  slotSetCompView(0);

  intoH->setEnabled(SchematicMode);
  popH->setEnabled(SchematicMode);
  editActivate->setEnabled(SchematicMode);
  changeProps->setEnabled(SchematicMode);
  insEquation->setEnabled(SchematicMode);
  insGround->setEnabled(SchematicMode);
  insPort->setEnabled(SchematicMode);
  insWire->setEnabled(SchematicMode);
  insLabel->setEnabled(SchematicMode);
  setMarker->setEnabled(SchematicMode);
  selectMarker->setEnabled(SchematicMode);
  simulate->setEnabled(SchematicMode);
}

// ---------------------------------------------------------
void QucsApp::changeSchematicSymbolMode(Schematic *Doc)
{
  if(Doc->symbolMode) {
    // go into select modus to avoid placing a forbidden element
    select->setOn(true);

    switchEditMode(false);
  }
  else
    switchEditMode(true);
}

// ---------------------------------------------------------
bool QucsApp::isTextDocument(QWidget *w) {
  if (w->inherits("QTextEdit"))
    return true;
  return false;
}

// ---------------------------------------------------------
// Is called if the "symEdit" action is activated, i.e. if the user
// switches between the two painting mode: Schematic and (subcircuit)
// symbol.
void QucsApp::slotSymbolEdit()
{
  QWidget *w = DocumentTab->currentPage();

  // in a text document (e.g. VHDL)
  if (isTextDocument (w)) {
    TextDoc *TDoc = (TextDoc*)w;
    // set 'DataDisplay' document of text file to symbol file
    QFileInfo Info(TDoc->DocName);
    QString sym = Info.baseName(true)+".sym";
    TDoc->DataDisplay = sym;

    // symbol file already loaded?
    int paint_mode = 0;
    if (!findDoc (QucsWorkDir.filePath(sym)))
      paint_mode = 1;

    // change current page to appropriate symbol file
    slotChangePage(TDoc->DocName,TDoc->DataDisplay);

    // set 'DataDisplay' document of symbol file to original text file
    Schematic *SDoc = (Schematic*)DocumentTab->currentPage();
    SDoc->DataDisplay = Info.fileName();

    // change into symbol mode
    if (paint_mode) // but only switch coordinates if newly loaded
      SDoc->switchPaintMode();
    SDoc->symbolMode = true;
    changeSchematicSymbolMode(SDoc);
    SDoc->becomeCurrent(true);
    SDoc->viewport()->update();
    view->drawn = false;
  }
  // in a normal schematic, data display or symbol file
  else {
    Schematic *SDoc = (Schematic*)w;
    // in a symbol file
    if(SDoc->DocName.right(4) == ".sym") {
      slotChangePage(SDoc->DocName, SDoc->DataDisplay);
    }
    // in a normal schematic
    else {
      editText->setHidden(true); // disable text edit of component property
      SDoc->switchPaintMode();   // twist the view coordinates
      changeSchematicSymbolMode(SDoc);
      SDoc->becomeCurrent(true);
      SDoc->viewport()->update();
      view->drawn = false;
    }
  }
}

// -----------------------------------------------------------
void QucsApp::slotPowerMatching()
{
  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

//  double Z0 = 50.0;
  QString Var = pm->pGraph->Var;
  double Imag = pm->VarPos[pm->nVarPos+1];
  if(Var == "Sopt")  // noise matching ?
    Imag *= -1.0;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->TwoCheck->setChecked(false);
  Dia->TwoCheck->setEnabled(false);
//  Dia->Ref1Edit->setText(QString::number(Z0));
  Dia->S11magEdit->setText(QString::number(pm->VarPos[pm->nVarPos]));
  Dia->S11degEdit->setText(QString::number(Imag));
  Dia->setFrequency(pm->VarPos[0]);

  slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
void QucsApp::slot2PortMatching()
{
  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

  QString DataSet;
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  int z = pm->pGraph->Var.find(':');
  if(z <= 0)  DataSet = Doc->DataSet;
  else  DataSet = pm->pGraph->Var.mid(z+1);
  double Freq = pm->VarPos[0];

  QFileInfo Info(Doc->DocName);
  DataSet = Info.dirPath()+QDir::separator()+DataSet;

  Diagram *Diag = new Diagram();

  Graph *pg = new Graph("S[1,1]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,1]."));
    return;
  }

  pg = new Graph("S[1,2]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,2]."));
    return;
  }

  pg = new Graph("S[2,1]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,1]."));
    return;
  }

  pg = new Graph("S[2,2]");
  Diag->Graphs.append(pg);
  if(!Diag->loadVarData(DataSet, pg)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,2]."));
    return;
  }

  DataX *Data = Diag->Graphs.getFirst()->cPointsX.first();
  if(Data->Var != "frequency") {
    QMessageBox::critical(0, tr("Error"), tr("Wrong dependency!"));
    return;
  }

  double *Value = Data->Points;
  // search for values for chosen frequency
  for(z=0; z<Data->count; z++)
    if(*(Value++) == Freq) break;

  // get S-parameters
  double S11real = *(Diag->Graphs.first()->cPointsY + 2*z);
  double S11imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S12real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S12imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S21real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S21imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);
  double S22real = *(Diag->Graphs.next()->cPointsY + 2*z);
  double S22imag = *(Diag->Graphs.current()->cPointsY + 2*z + 1);

  delete Diag;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->TwoCheck->setEnabled(false);
  Dia->setFrequency(Freq);
  Dia->S11magEdit->setText(QString::number(S11real));
  Dia->S11degEdit->setText(QString::number(S11imag));
  Dia->S12magEdit->setText(QString::number(S12real));
  Dia->S12degEdit->setText(QString::number(S12imag));
  Dia->S21magEdit->setText(QString::number(S21real));
  Dia->S21degEdit->setText(QString::number(S21imag));
  Dia->S22magEdit->setText(QString::number(S22real));
  Dia->S22degEdit->setText(QString::number(S22imag));

  slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
// Is called if the "edit" action is clicked on right mouse button menu.
void QucsApp::slotEditElement()
{
  if(view->focusMEvent)
    view->editElement((Schematic*)DocumentTab->currentPage(), view->focusMEvent);
}

// -----------------------------------------------------------
// Hides the edit for component property. Called e.g. if QLineEdit
// looses the focus.
void QucsApp::slotHideEdit()
{
  editText->setHidden(true);
}
