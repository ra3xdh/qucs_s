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

#include <QModelIndex>
#include <QAction>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDockWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QShortcut>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QUrl>
#include <QSettings>
#include <QVariant>
#include <QDebug>

#include "main.h"
#include "qucs.h"
#include "qucsdoc.h"
#include "textdoc.h"
#include "schematic.h"
#include "mouseactions.h"
#include "messagedock.h"
#include "wire.h"
#include "module.h"
#include "projectView.h"
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
#include "dialogs/exportdialog.h"
//#include "dialogs/vtabwidget.h"
//#include "dialogs/vtabbeddockwidget.h"
#include "extsimkernels/externsimdialog.h"
#include "octave_window.h"
#include "printerwriter.h"
#include "imagewriter.h"
#include "../qucs-lib/qucslib_common.h"
#include "misc.h"
#include "extsimkernels/verilogawriter.h"
#include "extsimkernels/simsettingsdialog.h"

// icon for unsaved files (diskette)
const char *smallsave_xpm[] = {
"16 17 66 1", " 	c None",
".	c #595963","+	c #E6E6F1","@	c #465460","#	c #FEFEFF",
"$	c #DEDEEE","%	c #43535F","&	c #D1D1E6","*	c #5E5E66",
"=	c #FFFFFF","-	c #C5C5DF",";	c #FCF8F9",">	c #BDBDDA",
",	c #BFBFDC","'	c #C4C4DF",")	c #FBF7F7","!	c #D6D6E9",
"~	c #CBCBE3","{	c #B5B5D6","]	c #BCBCDA","^	c #C6C6E0",
"/	c #CFCFE5","(	c #CEC9DC","_	c #D8D8EA",":	c #DADAEB",
"<	c #313134","[	c #807FB3","}	c #AEAED1","|	c #B7B7D7",
"1	c #E2E2EF","2	c #9393C0","3	c #E3E3F0","4	c #DDD5E1",
"5	c #E8E8F3","6	c #2F2F31","7	c #7B7BAF","8	c #8383B5",
"9	c #151518","0	c #000000","a	c #C0C0DC","b	c #8E8FBD",
"c	c #8989BA","d	c #E7EEF6","e	c #282829","f	c #6867A1",
"g	c #7373A9","h	c #A7A7CD","i	c #8080B3","j	c #7B7CB0",
"k	c #7070A8","l	c #6D6DA5","m	c #6E6EA6","n	c #6969A2",
"o	c #7A79AF","p	c #DCDCEC","q	c #60609A","r	c #7777AC",
"s	c #5D5D98","t	c #7676AB","u	c #484785","v	c #575793",
"w	c #50506A","x	c #8787B8","y	c #53536E","z	c #07070E",
"A	c #666688",
"        .       ",
"       .+.      ",
"      .+@#.     ",
"     .$%###.    ",
"    .&*####=.   ",
"   .-.#;#####.  ",
"  .>,'.#)!!!!~. ",
" .{].'^./(!_:<[.",
".}|.1./2.3456789",
"0a.$11.bc.defg9 ",
" 011h11.ij9kl9  ",
"  0_1h1h.mno9   ",
"   0p12h9qr9    ",
"    0hh9st9     ",
"     09uv9w     ",
"      0x9y      ",
"       zA       "};

const char *empty_xpm[] = {  // provides same height than "smallsave_xpm"
"1 17 1 1", "  c None", " ", " ", " ", " ", " ",
" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "};

struct iconCompInfoStruct
{
  int catIdx; // index of the component Category
  int compIdx; // index of the component itself in the Category
};

Q_DECLARE_METATYPE(iconCompInfoStruct)

QucsApp::QucsApp()
{
  setWindowTitle(QUCS_NAME " " PACKAGE_VERSION);

  QucsFileFilter =
    tr("Schematic") + " (*.sch);;" +
    tr("Data Display") + " (*.dpl);;" +
    tr("Qucs Documents") + " (*.sch *.dpl);;" +
    tr("VHDL Sources") + " (*.vhdl *.vhd);;" +
    tr("Verilog Sources") + " (*.v);;" +
    tr("Verilog-A Sources") + " (*.va);;" +
    tr("Octave Scripts") + " (*.m *.oct);;" +
    tr("Spice Files") + QString(" (") + QucsSettings.spiceExtensions.join(" ") + QString(");;") +
    tr("Any File")+" (*)";

  updateSchNameHash();
  updateSpiceNameHash();

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
  viewToolBar->setChecked(true);
  viewStatusBar->setChecked(true);
  viewBrowseDock->setChecked(true);
  slotViewOctaveDock(false);
  slotUpdateRecentFiles();
  initCursorMenu();
  Module::registerModules ();

  // instance of small text search dialog
  SearchDia = new SearchDialog(this);

  // creates a document called "untitled"
  Schematic *d = new Schematic(this, "");
  int i = DocumentTab->addTab(d, QPixmap(empty_xpm), QObject::tr("untitled"));
  DocumentTab->setCurrentIndex(i);

  select->setChecked(true);  // switch on the 'select' action
  switchSchematicDoc(true);  // "untitled" document is schematic

  lastExportFilename = QDir::homePath() + QDir::separator() + "export.png";

  // load documents given as command line arguments
  for(int z=1; z<qApp->argc(); z++) {
    QString arg = qApp->argv()[z];
    if(*(arg) != '-') {
      QFileInfo Info(arg);
      QucsSettings.QucsWorkDir.setPath(Info.absoluteDir().absolutePath());
      arg = QucsSettings.QucsWorkDir.filePath(Info.fileName());
      gotoPage(arg);
    }
  }

  if (QucsSettings.DefaultSimulator == spicecompat::simNotSpecified) {
      QMessageBox::information(this,tr("Qucs"),tr("Default simulator is not specified yet.\n"
                                         "Please setup it in the next dialog window.\n"
                                         "If you have no simulators except Qucs installed\n"
                                         "in your system leave default Qucsator setting\n"
                                         "and simple press Apply button"));
      slotSimSettings();
  }
}

QucsApp::~QucsApp()
{
  Module::unregisterModules ();
}


// #######################################################################
// ##########                                                   ##########
// ##########     Creates the working area (QTabWidget etc.)    ##########
// ##########                                                   ##########
// #######################################################################
/**
 * @brief QucsApp::initView Setup the layour of all widgets
 */
void QucsApp::initView()
{


  // set application icon
  // APPLE sets the QApplication icon with Info.plist
#ifndef __APPLE__
  setWindowIcon (QPixmap(":/bitmaps/big.qucs.xpm"));
#endif

  DocumentTab = new QTabWidget(this);
  setCentralWidget(DocumentTab);

  connect(DocumentTab,
          SIGNAL(currentChanged(QWidget*)), SLOT(slotChangeView(QWidget*)));

  // Give every tab a close button, and connect the button's signal to
  // slotFileClose
  DocumentTab->setTabsClosable(true);
  connect(DocumentTab,
          SIGNAL(tabCloseRequested(int)), SLOT(slotFileClose(int)));
#ifdef HAVE_QTABWIDGET_SETMOVABLE
  // make tabs draggable if supported
  DocumentTab->setMovable (true);
#endif

  dock = new QDockWidget(tr("Main Dock"),this);
  TabView = new QTabWidget(dock);
  TabView->setTabPosition(QTabWidget::West);

  connect(dock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleDock(bool)));

  view = new MouseActions(this);

  editText = new QLineEdit(this);  // for editing component properties
  editText->setFrame(false);
  editText->setHidden(true);

  QPalette p = palette();
  p.setColor(backgroundRole(), QucsSettings.BGColor);
  editText->setPalette(p);

  connect(editText, SIGNAL(returnPressed()), SLOT(slotApplyCompText()));
  connect(editText, SIGNAL(textChanged(const QString&)),
          SLOT(slotResizePropEdit(const QString&)));
  connect(editText, SIGNAL(lostFocus()), SLOT(slotHideEdit()));

  // ----------------------------------------------------------
  // "Project Tab" of the left QTabWidget
  QWidget *ProjGroup = new QWidget();
  QVBoxLayout *ProjGroupLayout = new QVBoxLayout();
  QWidget *ProjButts = new QWidget();
  QPushButton *ProjNew = new QPushButton(tr("New"));
  connect(ProjNew, SIGNAL(clicked()), SLOT(slotButtonProjNew()));
  QPushButton *ProjOpen = new QPushButton(tr("Open"));
  connect(ProjOpen, SIGNAL(clicked()), SLOT(slotButtonProjOpen()));
  QPushButton *ProjDel = new QPushButton(tr("Delete"));
  connect(ProjDel, SIGNAL(clicked()), SLOT(slotButtonProjDel()));

  QHBoxLayout *ProjButtsLayout = new QHBoxLayout();
  ProjButtsLayout->addWidget(ProjNew);
  ProjButtsLayout->addWidget(ProjOpen);
  ProjButtsLayout->addWidget(ProjDel);
  ProjButts->setLayout(ProjButtsLayout);

  ProjGroupLayout->addWidget(ProjButts);

  Projects = new QListView();

  ProjGroupLayout->addWidget(Projects);
  ProjGroup->setLayout(ProjGroupLayout);

  TabView->addTab(ProjGroup, tr("Projects"));
  TabView->setTabToolTip(TabView->indexOf(ProjGroup), tr("content of project directory"));

  connect(Projects, SIGNAL(doubleClicked(const QModelIndex &)),
          this, SLOT(slotListProjOpen(const QModelIndex &)));

  // ----------------------------------------------------------
  // "Content" Tab of the left QTabWidget
  Content = new ProjectView(this);
  Content->setContextMenuPolicy(Qt::CustomContextMenu);

  TabView->addTab(Content, tr("Content"));
  TabView->setTabToolTip(TabView->indexOf(Content), tr("content of current project"));

  connect(Content, SIGNAL(clicked(const QModelIndex &)), 
          SLOT(slotSelectSubcircuit(const QModelIndex &)));

  connect(Content, SIGNAL(doubleClicked(const QModelIndex &)),
          SLOT(slotOpenContent(const QModelIndex &)));

  // ----------------------------------------------------------
  // "Component" Tab of the left QTabWidget
  QWidget *CompGroup  = new QWidget();
  QVBoxLayout *CompGroupLayout = new QVBoxLayout();
  QHBoxLayout *CompSearchLayout = new QHBoxLayout();

  CompChoose = new QComboBox(this);
  CompComps = new QListWidget(this);
  CompComps->setViewMode(QListView::IconMode);
  CompComps->setGridSize(QSize(110,90));
  CompSearch = new QLineEdit(this);
  CompSearch->setPlaceholderText(tr("Search Components"));
  CompSearchClear = new QPushButton(tr("Clear"));

  CompGroupLayout->setSpacing(5);
  CompGroupLayout->addWidget(CompChoose);
  CompGroupLayout->addWidget(CompComps);
  CompGroupLayout->addLayout(CompSearchLayout);
  CompSearchLayout->addWidget(CompSearch);
  CompSearchLayout->addWidget(CompSearchClear);
  CompGroup->setLayout(CompGroupLayout);

  TabView->addTab(CompGroup,tr("Components"));
  TabView->setTabToolTip(TabView->indexOf(CompGroup), tr("components and diagrams"));
  fillComboBox(true);

  slotSetCompView(0);
  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));
  connect(CompComps, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));
  connect(CompSearch, SIGNAL(textEdited(const QString &)), SLOT(slotSearchComponent(const QString &)));
  connect(CompSearchClear, SIGNAL(clicked()), SLOT(slotSearchClear()));

  // ----------------------------------------------------------
  // "Libraries" Tab of the left QTabWidget

  QWidget *LibGroup = new QWidget ();
  QVBoxLayout *LibGroupLayout = new QVBoxLayout ();
  QWidget *LibButts = new QWidget ();
  QPushButton *LibManage = new QPushButton (tr ("Manage Libraries"));
  connect(LibManage, SIGNAL(clicked()), SLOT(slotCallLibrary()));

  QHBoxLayout *LibButtsLayout = new QHBoxLayout();
  LibButtsLayout->addWidget (LibManage);
  LibButts->setLayout(LibButtsLayout);

  LibGroupLayout->addWidget(LibButts);


  libTreeWidget = new QTreeWidget (this);
  libTreeWidget->setColumnCount (1);
  QStringList headers;
  headers.clear ();
  headers << tr ("Libraries");
  libTreeWidget->setHeaderLabels (headers);

  LibGroupLayout->addWidget (libTreeWidget);
  LibGroup->setLayout (LibGroupLayout);

  fillLibrariesTreeView ();

  TabView->addTab (LibGroup, tr("Libraries"));
  TabView->setTabToolTip (TabView->indexOf (CompGroup), tr ("system and user component libraries"));

  connect(libTreeWidget, SIGNAL(itemPressed (QTreeWidgetItem*, int)),
           SLOT(slotSelectLibComponent (QTreeWidgetItem*)));

  // ----------------------------------------------------------
  // put the tab widget in the dock
  dock->setWidget(TabView);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->addDockWidget(Qt::LeftDockWidgetArea, dock);
  TabView->setCurrentIndex(0);

  // ----------------------------------------------------------
  // Octave docking window
  octDock = new QDockWidget(tr("Octave Dock"));

  connect(octDock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleOctave(bool)));
  octave = new OctaveWindow(octDock);
  this->addDockWidget(Qt::BottomDockWidgetArea, octDock);
  this->setCorner(Qt::BottomLeftCorner  , Qt::LeftDockWidgetArea);

  // ............................................

  messageDock = new MessageDock(this);

  // initial home directory model
  m_homeDirModel = new QFileSystemModel(this);
  QStringList filters;
  filters << "*_prj";
  m_homeDirModel->setNameFilters(filters);
  m_homeDirModel->setNameFilterDisables(false);
  m_homeDirModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

  // ............................................
  readProjects(); // reads all projects and inserts them into the ListBox
}

// Put all available libraries into ComboBox.
void QucsApp::fillLibrariesTreeView ()
{
    QStringList LibFiles;
    QStringList::iterator it;
    QList<QTreeWidgetItem *> topitems;

    libTreeWidget->clear();

    // make the system libraries section header
    QTreeWidgetItem* newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("System Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    QFont sectionFont = newitem->font(0);
    sectionFont.setItalic (true);
    sectionFont.setBold (true);
    newitem->setFont (0, sectionFont);
//    newitem->setBackground
    topitems.append (newitem);

    QDir LibDir(QucsSettings.LibDir);
    LibFiles = LibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    QStringList blacklist = getBlacklistedLibraries(QucsSettings.LibDir);
    foreach(QString ss, blacklist) { // exclude blacklisted files
        LibFiles.removeAll(ss);
    }

    // create top level library items, base on the library names
    for(it = LibFiles.begin(); it != LibFiles.end(); it++)
    {
        QString libPath(*it);
        libPath.chop(4); // remove extension

        ComponentLibrary parsedlibrary;

        int result = parseComponentLibrary (libPath , parsedlibrary);
        QStringList nameAndFileName;
        nameAndFileName.append (parsedlibrary.name);
        nameAndFileName.append (QucsSettings.LibDir + *it);

        QTreeWidgetItem* newlibitem = new QTreeWidgetItem((QTreeWidget*)0, nameAndFileName);

        switch (result)
        {
            case QUCS_COMP_LIB_IO_ERROR:
            {
                QString filename = getLibAbsPath(libPath);
                QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
                return;
            }
            case QUCS_COMP_LIB_CORRUPT:
                QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
                return;
            default:
                break;
        }

        for (int i = 0; i < parsedlibrary.components.count (); i++)
        {
            QStringList compNameAndDefinition;

            compNameAndDefinition.append (parsedlibrary.components[i].name);

            QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

            s +=  "<Components>\n  " +
                  parsedlibrary.components[i].modelString + "\n" +
                  "</Components>\n";

            compNameAndDefinition.append (s);

            QTreeWidgetItem* newcompitem = new QTreeWidgetItem(newlibitem, compNameAndDefinition);

            // Silence warning from the compiler about unused variable newcompitem
            // we pass the pointer to the parent item in the constructor
            Q_UNUSED( newcompitem )
        }

        topitems.append (newlibitem);
    }


    // make the user libraries section header
    newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("User Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    newitem->setFont (0, sectionFont);
    topitems.append (newitem);

    QDir UserLibDir = QDir (QucsSettings.QucsHomeDir.canonicalPath () + "/user_lib/");

    LibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    QDir UsrLibDir(UserLibDir);
    LibFiles = UsrLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    blacklist = getBlacklistedLibraries(QucsSettings.LibDir);
    foreach(QString ss, blacklist) { // exclude blacklisted files
        LibFiles.removeAll(ss);
    }
    int UserLibCount = LibFiles.count();

    if (UserLibCount > 0) // there are user libraries
    {

        // create top level library itmes, base on the library names
        for(it = LibFiles.begin(); it != LibFiles.end(); it++)
        {
            QString libPath(UserLibDir.absoluteFilePath(*it));
            libPath.chop(4); // remove extension

            ComponentLibrary parsedlibrary;

            int result = parseComponentLibrary (libPath, parsedlibrary);
            QStringList nameAndFileName;
            nameAndFileName.append (parsedlibrary.name);
            nameAndFileName.append (UserLibDir.absolutePath() +"/"+ *it);

            QTreeWidgetItem* newlibitem = new QTreeWidgetItem((QTreeWidget*)0, nameAndFileName);

            switch (result)
            {
                case QUCS_COMP_LIB_IO_ERROR:
                {
                    QString filename = getLibAbsPath(libPath);
                    QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
                    return;
                }
                case QUCS_COMP_LIB_CORRUPT:
                    QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
                    return;
                default:
                    break;
            }

            for (int i = 0; i < parsedlibrary.components.count (); i++)
            {
                QStringList compNameAndDefinition;

                compNameAndDefinition.append (parsedlibrary.components[i].name);

                QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

                s +=  "<Components>\n  " +
                      parsedlibrary.components[i].modelString + "\n" +
                      "</Components>\n";

                compNameAndDefinition.append (s);


                QTreeWidgetItem* newcompitem = new QTreeWidgetItem(newlibitem, compNameAndDefinition);

                // Silence warning from the compiler about unused variable newcompitem
                // we pass the pointer to the parent item in the constructor
                Q_UNUSED( newcompitem )
            }

            topitems.append (newlibitem);
        }
        libTreeWidget->insertTopLevelItems(0, topitems);
    }
    else
    {
        // make the user libraries section header
        newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("No User Libraries"));
        sectionFont.setBold (false);
        newitem->setFont (0, sectionFont);
        topitems.append (newitem);
    }

    libTreeWidget->insertTopLevelItems(0, topitems);
}


// ---------------------------------------------------------------
// Returns a pointer to the QucsDoc object whose number is "No".
// If No < 0 then a pointer to the current document is returned.
QucsDoc* QucsApp::getDoc(int No)
{
  QWidget *w;
  if(No < 0)
    w = DocumentTab->currentWidget();
  else
    w = DocumentTab->widget(No);

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

// if setAll, add all categories to combobox
// if not, set just paintings (symbol painting mode)
void QucsApp::fillComboBox (bool setAll)
{
  //CompChoose->setMaxVisibleItems (13); // Increase this if you add items below.
  CompChoose->clear ();
  CompSearch->clear(); // clear the search box, in case search was active...

  if (!setAll) {
    CompChoose->insertItem(CompChoose->count(), QObject::tr("paintings"));
  } else {
    QStringList cats = Category::getCategories ();
    foreach (QString it, cats) {
      CompChoose->insertItem(CompChoose->count(), it);
    }
  }
}

// ----------------------------------------------------------
// Whenever the Component Library ComboBox is changed, this slot fills the
// Component IconView with the appropriate components.
void QucsApp::slotSetCompView (int index)
{
  //qDebug() << "QucsApp::slotSetCompView(" << index << ")";

  editText->setHidden (true); // disable text edit of component property

  QList<Module *> Comps;
  if (CompChoose->count () <= 0) return;

  // was in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) {
    if (index == 0) // user selected "Search results" item
      return;
    CompChoose->removeItem(0);
    CompSearch->clear(); // clear the search box
    --index; // adjust requested index since item 0 was removed
  }
  CompComps->clear ();   // clear the IconView

  // make sure the right index is selected
  //  (might have been called by a cleared search and not by user action)
  CompChoose->setCurrentIndex(index);
  int compIdx;
  iconCompInfoStruct iconCompInfo;
  QVariant v;
  QString item = CompChoose->itemText (index);
  int catIdx = Category::getModulesNr(item);

  Comps = Category::getModules(item);
  QString Name;
  pInfoFunc Infos = 0;

  // if something was registered dynamicaly, get and draw icons into dock
  if (item == QObject::tr("verilog-a user devices")) {

    compIdx = 0;
    QMapIterator<QString, QString> i(Module::vaComponents);
    while (i.hasNext()) {
      i.next();

      // default icon initally matches the module name
      //Name = i.key();

      // Just need path to bitmap, do not create an object
      QString Name, vaBitmap;
      Component * c = (Component *)
              vacomponent::info (Name, vaBitmap, false, i.value());
      if (c) delete c;

      // check if icon exists, fall back to default
      QString iconPath = QucsSettings.QucsWorkDir.filePath(vaBitmap+".png");

      QFile iconFile(iconPath);
      QPixmap vaIcon;

      if(iconFile.exists())
      {
        // load bitmap defined on the JSON symbol file
        vaIcon = QPixmap(iconPath);
      }
      else
      {
        QMessageBox::information(this, tr("Info"),
                     tr("Default icon not found:\n %1.png").arg(vaBitmap));
        // default icon
        vaIcon = QPixmap(":/bitmaps/editdelete.png");
      }
      QListWidgetItem *icon = new QListWidgetItem(vaIcon, Name);
      icon->setToolTip(Name);
      iconCompInfo = iconCompInfoStruct{catIdx, compIdx};
      v.setValue(iconCompInfo);
      icon->setData(Qt::UserRole, v);
      CompComps->addItem(icon);
      compIdx++;
    }
  } else {
    // static components
    char * File;
    // Populate list of component bitmaps
    compIdx = 0;
    QList<Module *>::const_iterator it;
    for (it = Comps.constBegin(); it != Comps.constEnd(); it++) {
      Infos = (*it)->info;
      if (Infos) {
        /// \todo warning: expression result unused, can we rewrite this?
        (void) *((*it)->info) (Name, File, false);
        QListWidgetItem *icon = new QListWidgetItem(QPixmap(":/bitmaps/" + QString (File) + ".png"), Name);
        icon->setToolTip(Name);
        iconCompInfo = iconCompInfoStruct{catIdx, compIdx};
        v.setValue(iconCompInfo);
        icon->setData(Qt::UserRole, v);
        CompComps->addItem(icon);
      }
      compIdx++;
    }
  }
}

// ------------------------------------------------------------------
// When CompSearch is being edited, create a temp page show the
// search result
void QucsApp::slotSearchComponent(const QString &searchText)
{
  qDebug() << "User search: " << searchText;
  CompComps->clear ();   // clear the IconView

  // not already in "search mode"
  if (CompChoose->itemText(0) != tr("Search results")) {
    ccCurIdx = CompChoose->currentIndex(); // remember current panel
    // insert "Search results" at the beginning, so that it is visible
    CompChoose->insertItem(-1, tr("Search results"));
    CompChoose->setCurrentIndex(0);
  }

  if (searchText.isEmpty()) {
    slotSetCompView(CompChoose->currentIndex());
  } else {
    CompChoose->setCurrentIndex(0); // make sure the "Search results" category is selected
    editText->setHidden (true); // disable text edit of component property

    //traverse all component and match searchText with name
    QString Name;
    char * File;
    QList<Module *> Comps;
    iconCompInfoStruct iconCompInfo;
    QVariant v;

    QStringList cats = Category::getCategories ();
    int catIdx = 0;
    foreach(QString it, cats) {
      // this will go also over the "verilog-a user devices" category, if present
      //   but since modules there have no 'info' function it won't handle them
      Comps = Category::getModules(it);
      QList<Module *>::const_iterator modit;
      int compIdx = 0;
      for (modit = Comps.constBegin(); modit != Comps.constEnd(); modit++) {
        if ((*modit)->info) {
          /// \todo warning: expression result unused, can we rewrite this?
          (void) *((*modit)->info) (Name, File, false);

          if((Name.indexOf(searchText, 0, Qt::CaseInsensitive)) != -1) {
            //match
            QListWidgetItem *icon = new QListWidgetItem(QPixmap(":/bitmaps/" + QString (File) + ".png"), Name);
            icon->setToolTip(it + ": " + Name);
            // add component category and module indexes to the icon
            iconCompInfo = iconCompInfoStruct{catIdx, compIdx};
            v.setValue(iconCompInfo);
            icon->setData(Qt::UserRole, v);
            CompComps->addItem(icon);
          }
        }
        compIdx++;
      }
      catIdx++;
    }
    // the "verilog-a user devices" is the last category, if present
    QMapIterator<QString, QString> i(Module::vaComponents);
    int compIdx = 0;
    while (i.hasNext()) {
      i.next();
      // Just need path to bitmap, do not create an object
      QString Name, vaBitmap;
      vacomponent::info (Name, vaBitmap, false, i.value());

      if((Name.indexOf(searchText, 0, Qt::CaseInsensitive)) != -1) {
        //match

        // check if icon exists, fall back to default
        QString iconPath = QucsSettings.QucsWorkDir.filePath(vaBitmap+".png");

        QFile iconFile(iconPath);
        QPixmap vaIcon;

        if(iconFile.exists())
        {
          // load bitmap defined on the JSON symbol file
          vaIcon = QPixmap(iconPath);
        }
        else
        {
          // default icon
          vaIcon = QPixmap(":/bitmaps/editdelete.png");
        }

        // Add icon an name tag to dock
        QListWidgetItem *icon = new QListWidgetItem(vaIcon, Name);
        icon->setToolTip(tr("verilog-a user devices") + ": " + Name);
        // Verilog-A is the last category
        iconCompInfo = iconCompInfoStruct{catIdx-1, compIdx};
        v.setValue(iconCompInfo);
        icon->setData(Qt::UserRole, v);
        CompComps->addItem(icon);
      }
      compIdx++;
    }
  }
}

// ------------------------------------------------------------------
void QucsApp::slotSearchClear()
{
  // was in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) {
    CompChoose->removeItem(0); // remove the added "Search results" item
    CompSearch->clear();
    // go back to the panel selected before search started
    slotSetCompView(ccCurIdx);
    // the added "Search results" panel text will be removed by slotSetCompView()
  }
}

// ------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QListWidgetItem *item)
{
  slotHideEdit(); // disable text edit of component property

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem  = 0;   // no component/diagram/painting selected

  if(item == 0) {   // mouse button pressed not over an item ?
    CompComps->clearSelection();  // deselect component in ViewList
    return;
  }

  if(view->drawn)
    ((Q3ScrollView*)DocumentTab->currentWidget())->viewport()->update();
  view->drawn = false;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  pInfoFunc Infos = 0;
  pInfoVAFunc InfosVA = 0;

  int i = CompComps->row(item);
  QList<Module *> Comps;

  // if symbol mode, only paintings are enabled.
  Comps = Category::getModules(CompChoose->currentText());

  QString name = CompComps->item(i)->text();
  QString CompName;
  QString CompFile_qstr;
  char *CompFile_cptr;

  qDebug() << "pressed CompComps id" << i << name;
  QVariant v = CompComps->item(i)->data(Qt::UserRole);
  iconCompInfoStruct iconCompInfo = v.value<iconCompInfoStruct>();
  qDebug() << "slotSelectComponent()" << iconCompInfo.catIdx << iconCompInfo.compIdx;

  Category* cat = Category::Categories.at(iconCompInfo.catIdx);
  Module *mod = cat->Content.at(iconCompInfo.compIdx);
  qDebug() << "mod->info" << mod->info;
  qDebug() << "mod->infoVA" << mod->infoVA;
  Infos = mod->info;
  if (Infos) {
    // static component
    view->selElem = (*mod->info) (CompName, CompFile_cptr, true);
  } else {
    // Verilog-A component
    InfosVA = mod->infoVA;
    // get JSON file out of item name on widgetitem
    QString filename = Module::vaComponents[name];
    if (InfosVA) {
      view->selElem = (*InfosVA) (CompName, CompFile_qstr, true, filename);
    }
  }

  // in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) {
    if (Infos || InfosVA) {
      // change currently selected category, so the user will
      //   see where the component comes from
      CompChoose->setCurrentIndex(iconCompInfo.catIdx+1); // +1 due to the added "Search Results" item
      ccCurIdx = iconCompInfo.catIdx; // remember the category to select when exiting search
      //!! comment out the above two lines if you would like that the search
      //!!   returns back to the last selected category instead
    }
  }
}

// ####################################################################
// #####  Functions for the menu that appears when right-clicking #####
// #####  on a file in the "Content" ListView.                    #####
// ####################################################################

void QucsApp::initCursorMenu()
{
  ContentMenu = new QMenu(this);
#define APPEND_MENU(action, slot, text) \
  { \
  action = new QAction(tr(text), ContentMenu); \
  connect(action, SIGNAL(triggered()), SLOT(slot())); \
  ContentMenu->addAction(action); \
  }
  
  APPEND_MENU(ActionCMenuOpen, slotCMenuOpen, "Open")
  APPEND_MENU(ActionCMenuCopy, slotCMenuCopy, "Duplicate")
  APPEND_MENU(ActionCMenuRename, slotCMenuRename, "Rename")
  APPEND_MENU(ActionCMenuDelete, slotCMenuDelete, "Delete")
  APPEND_MENU(ActionCMenuInsert, slotCMenuInsert, "Insert")

#undef APPEND_MENU
  connect(Content, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotShowContentMenu(const QPoint&)));
}

// ----------------------------------------------------------
// Shows the menu.
void QucsApp::slotShowContentMenu(const QPoint& pos) 
{
  QModelIndex idx = Content->indexAt(pos);
  if (idx.isValid() && idx.parent().isValid()) {
    ActionCMenuInsert->setVisible(
        idx.sibling(idx.row(), 1).data().toString().contains(tr("-port"))
    );
    ContentMenu->popup(Content->mapToGlobal(pos));
  }    
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
  else if (Ext == "vhdl.cfg" || Ext == "vhd.cfg")
    Type = tr("VHDL configuration");
  else if (Ext == "cfg")
    Type = tr("configuration");
  return Type;
}

void QucsApp::slotCMenuOpen()
{
  slotOpenContent(Content->currentIndex());
}

void QucsApp::slotCMenuCopy()
{
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QDir dir(QucsSettings.QucsWorkDir);
  QString file(dir.filePath(filename));
  QFileInfo fileinfo(file);

  //check changed file save
  int z = 0; //search if the doc is loaded
  QucsDoc *d = findDoc(file, &z);
  if (d != NULL && d->DocChanged) {
    DocumentTab->setCurrentIndex(z);
    int ret = QMessageBox::question(this, tr("Copying Qucs document"), 
        tr("The document contains unsaved changes!\n") + 
        tr("Do you want to save the changes before copying?"),
        tr("&Ignore"), tr("&Save"), 0, 1);
    if (ret == 1) {
      d->save();
    }
  }

  QString suffix = fileinfo.suffix();
  QString base = fileinfo.completeBaseName();
  if(base.isEmpty()) {
    base = filename;
  }

  bool exists = true;   //generate unique name
  int i = 0;
  QString defaultName;
  while (exists) {
    ++i;
    defaultName = base + "_copy" + QString::number(i) + "." + suffix;
    exists = QFile::exists(dir.filePath(defaultName));
  }

  bool ok;
  QString s = QInputDialog::getText(this, tr("Copy file"), tr("Enter new name:"), QLineEdit::Normal, defaultName, &ok);

  if(ok && !s.isEmpty()) {
    if (!s.endsWith(suffix)) {
      s += QString(".") + suffix;
    }

    if (QFile::exists(dir.filePath(s))) {  //check New Name exists
      QMessageBox::critical(this, tr("error"), tr("Cannot copy file to identical name: %1").arg(filename));
      return;
    }

    if (!QFile::copy(dir.filePath(filename), dir.filePath(s))) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot copy schematic: %1").arg(filename));
      return;
    }
    //TODO: maybe require disable edit here

    // refresh the schematic file path
    this->updateSchNameHash();
    this->updateSpiceNameHash();

    slotUpdateTreeview();
  }
}

void QucsApp::slotCMenuRename()
{
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString file(QucsSettings.QucsWorkDir.filePath(filename));
  QFileInfo fileinfo(file);

  if (findDoc(file)) {
    QMessageBox::critical(this, tr("Error"),
        tr("Cannot rename an open file!"));
    return;
  }

  QString suffix = fileinfo.suffix();
  QString base = fileinfo.completeBaseName();
  if(base.isEmpty()) {
    base = filename;
  }

  bool ok;
  QString s = QInputDialog::getText(this, tr("Rename file"), tr("Enter new filename:"), QLineEdit::Normal, base, &ok);

  if(ok && !s.isEmpty()) { 
    if (!s.endsWith(suffix)) {
      s += QString(".") + suffix;
    }
    QDir dir(QucsSettings.QucsWorkDir.path());
    if(!dir.rename(filename, s)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot rename file: %1").arg(filename));
      return;
    }

    slotUpdateTreeview();
  }
}

void QucsApp::slotCMenuDelete()
{
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString file(QucsSettings.QucsWorkDir.filePath(filename));

  if (findDoc (file)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot delete an open file!"));
    return;
  }

  int No;
  No = QMessageBox::warning(this, tr("Warning"),
      tr("This will delete the file permanently! Continue ?"),
      tr("No"), tr("Yes"));
  if(No == 1) {
    if(!QFile::remove(file)) {
      QMessageBox::critical(this, tr("Error"),
      tr("Cannot delete file: %1").arg(filename));
      return;
    }
  }

  slotUpdateTreeview();
}

void QucsApp::slotCMenuInsert()
{
  slotSelectSubcircuit(Content->currentIndex());
}

// ################################################################
// #####    Functions that handle the project operations.     #####
// ################################################################

// Checks for qucs directory and reads all existing Qucs projects.
void QucsApp::readProjects()
{
  QString path = QucsSettings.QucsHomeDir.absolutePath();
  QDir ProjDir(path);

  // create home dir if not exist
  if(!ProjDir.exists()) {
    if(!ProjDir.mkdir(path)) {
      QMessageBox::warning(this, tr("Warning"),
          tr("Cannot create work directory !"));
      return;
    }
  }
  m_homeDirModel->setRootPath(path);
  Projects->setModel(m_homeDirModel);
  Projects->setRootIndex(m_homeDirModel->index(path));
}

// ----------------------------------------------------------
// Is called, when "Create New Project" button is pressed.
void QucsApp::slotButtonProjNew()
{
  slotHideEdit(); // disable text edit of component property

  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir projDir(QucsSettings.QucsHomeDir.path());
  QString name = d->ProjName->text();
  bool open = d->OpenProj->isChecked();

  if (!name.endsWith("_prj")) {
    name += "_prj";
  }

  if(!projDir.mkdir(name)) {
    QMessageBox::information(this, tr("Info"),
        tr("Cannot create project directory !"));
  }
  if(open) {
    openProject(QucsSettings.QucsHomeDir.filePath(name));
  }
}

// ----------------------------------------------------------
// Opens an existing project.
void QucsApp::openProject(const QString& Path)
{
  slotHideEdit(); // disable text edit of component property

  QDir ProjDir(QDir::cleanPath(Path)); // the full path
  QString openProjName = ProjDir.dirName(); // only the project directory name

  if(!ProjDir.exists() || !ProjDir.isReadable()) { // check project directory
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: %1").arg(Path));
    return;
  }

  if (!openProjName.endsWith("_prj")) { // should not happen
    QMessageBox::critical(this, tr("Error"),
                          tr("Project directory name does not end in '_prj'(%1)").arg(openProjName));
    return;
  }

  if(!closeAllFiles()) return;   // close files and ask for saving them
  Schematic *d = new Schematic(this, "");
  int i = DocumentTab->addTab(d, QPixmap(empty_xpm), QObject::tr("untitled"));
  DocumentTab->setCurrentIndex(i);

  view->drawn = false;

  slotResetWarnings();

  QucsSettings.QucsWorkDir.setPath(ProjDir.path());
  octave->adjustDirectory();

  Content->setProjPath(QucsSettings.QucsWorkDir.absolutePath());

  TabView->setCurrentIndex(1);   // switch to "Content"-Tab

  openProjName.chop(4); // remove "_prj" from name
  ProjName = openProjName;   // remember the name of project

  // show name in title of main window
  setWindowTitle("Qucs " PACKAGE_VERSION + tr(" - Project: ")+ProjName);
}

// ----------------------------------------------------------
// Is called when the open project menu is called.
void QucsApp::slotMenuProjOpen()
{
  QString d = QFileDialog::getExistingDirectory(
      this, tr("Choose Project Directory for Opening"),
      QucsSettings.QucsHomeDir.path(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if(d.isEmpty()) return;

  openProject(d);
}

// ----------------------------------------------------------
// Is called, when "Open Project" button is pressed.
void QucsApp::slotButtonProjOpen()
{
  slotHideEdit();

  QModelIndex idx = Projects->currentIndex();
  if (!idx.isValid()) {
    QMessageBox::information(this, tr("Info"),
				tr("No project is selected !"));
  } else {
    slotListProjOpen(idx);
  }
}

// ----------------------------------------------------------
// Is called when project is double-clicked to open it.
void QucsApp::slotListProjOpen(const QModelIndex &idx)
{
  openProject(QucsSettings.QucsHomeDir.filePath(
      idx.data().toString()));
}

// ----------------------------------------------------------
// Is called when the close project menu is called.
void QucsApp::slotMenuProjClose()
{
  slotHideEdit(); // disable text edit of component property

  if(!closeAllFiles()) return;   // close files and ask for saving them
  Schematic *d = new Schematic(this, "");
  int i = DocumentTab->addTab(d, QPixmap(empty_xpm), QObject::tr("untitled"));
  DocumentTab->setCurrentIndex(i);

  view->drawn = false;

  slotResetWarnings();
  setWindowTitle("Qucs " PACKAGE_VERSION + tr(" - Project: "));
  QucsSettings.QucsWorkDir.setPath(QDir::homePath()+QDir::convertSeparators ("/.qucs"));
  octave->adjustDirectory();

  Content->setProjPath("");

  TabView->setCurrentIndex(0);   // switch to "Projects"-Tab
  ProjName = "";
}

// remove a directory recursively
bool QucsApp::recurRemove(const QString &Path)
{
  bool result = true;
  QDir projDir = QDir(Path);

  if (projDir.exists(Path)) {
    Q_FOREACH(QFileInfo info, 
        projDir.entryInfoList(
            QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::AllEntries, QDir::DirsFirst)) {
      if (info.isDir()) {
        result = recurRemove(info.absoluteFilePath());
        if (!result) {
          QMessageBox::information(this, tr("Info"),
              tr("Cannot remove directory: %1").arg(Path));
          return false;
        }
      }
      else if(info.isFile()) {
        result = QFile::remove(info.absoluteFilePath());
        if (!result) {
          QMessageBox::information(this, tr("Info"),
              tr("Cannot delete file: %1").arg(info.fileName()));
          return false;
        }
      }
    }
    result = projDir.rmdir(Path);
  }
  return result;
}

// ----------------------------------------------------------
bool QucsApp::deleteProject(const QString& Path)
{
  slotHideEdit();

  if(Path.isEmpty()) return false;

  QString delProjName = QDir(Path).dirName(); // only project directory name

  if (!delProjName.endsWith("_prj")) { // should not happen
    QMessageBox::critical(this, tr("Error"),
                          tr("Project directory name does not end in '_prj' (%1)").arg(delProjName));
    return false;
  }

  delProjName.chop(4); // remove "_prj" from name

  if(delProjName == ProjName) {
    QMessageBox::information(this, tr("Info"),
        tr("Cannot delete an open project !"));
    return false;
  }

  // first ask, if really delete project ?
  if(QMessageBox::warning(this, tr("Warning"),
      tr("This will destroy all the project files permanently ! Continue ?"),
      tr("&Yes"), tr("&No"), 0,1,1))  return false;

  if (!recurRemove(Path)) {
    QMessageBox::information(this, tr("Info"),
        tr("Cannot remove project directory!"));
    return false;
  }
  return true;
}

// ----------------------------------------------------------
// Is called, when "Delete Project" menu is activated.
void QucsApp::slotMenuProjDel()
{
  QString d = QFileDialog::getExistingDirectory(
      this, tr("Choose Project Directory for Deleting"),
      QucsSettings.QucsHomeDir.path(),
      QFileDialog::ShowDirsOnly
      | QFileDialog::DontResolveSymlinks);

  deleteProject(d);
}

// ----------------------------------------------------------
// Is called, when "Delete Project" button is pressed.
void QucsApp::slotButtonProjDel()
{
  QModelIndex idx = Projects->currentIndex();
  if(!idx.isValid()) {
    QMessageBox::information(this, tr("Info"),
        tr("No project is selected!"));
    return;
  }

  deleteProject(QucsSettings.QucsHomeDir.filePath(idx.data().toString()));
}


// ################################################################
// #####  Functions that handle the file operations for the   #####
// #####  documents.                                          #####
// ################################################################

void QucsApp::slotFileNew()
{
  statusBar()->showMessage(tr("Creating new schematic..."));
  slotHideEdit(); // disable text edit of component property

  Schematic *d = new Schematic(this, "");
  int i = DocumentTab->addTab(d, QPixmap(empty_xpm), QObject::tr("untitled"));
  DocumentTab->setCurrentIndex(i);

  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotTextNew()
{
  statusBar()->showMessage(tr("Creating new text editor..."));
  slotHideEdit(); // disable text edit of component property
  TextDoc *d = new TextDoc(this, "");
  int i = DocumentTab->addTab(d, QPixmap(empty_xpm), QObject::tr("untitled"));
  DocumentTab->setCurrentIndex(i);

  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------
// Changes to the document "Name". If already open then it goes to it
// directly, otherwise it loads it.
bool QucsApp::gotoPage(const QString& Name)
{
  int No = DocumentTab->currentIndex();

  int i = 0;
  QucsDoc * d = findDoc (Name, &i);  // search, if page is already loaded

  if(d) {   // open page found ?
    d->becomeCurrent(true);
    DocumentTab->setCurrentIndex(i);  // make new document the current
    return true;
  }

  QFileInfo Info(Name);
  if(Info.suffix() == "sch" || Info.suffix() == "dpl" ||
     Info.suffix() == "sym") {
    d = new Schematic(this, Name);
    i = DocumentTab->addTab((Schematic *)d, QPixmap(empty_xpm), Info.fileName()); 
  }
  else {
    d = new TextDoc(this, Name);
    i = DocumentTab->addTab((TextDoc *)d, QPixmap(empty_xpm), Info.fileName());
  }
  DocumentTab->setCurrentIndex(i);

  if(!d->load()) {    // load document if possible
    delete d;
    DocumentTab->setCurrentIndex(No);
    view->drawn = false;
    return false;
  }
  slotChangeView(DocumentTab->currentWidget());

  // if only an untitled document was open -> close it
  if(getDoc(0)->DocName.isEmpty())
    if(!getDoc(0)->DocChanged)
      delete DocumentTab->widget(0);

  view->drawn = false;
  return true;
}

QString lastDirOpenSave; // to remember last directory and file

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  slotHideEdit(); // disable text edit of component property

  statusBar()->showMessage(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(this, tr("Enter a Schematic Name"),
    lastDirOpenSave.isEmpty() ? QString(".") : lastDirOpenSave, QucsFileFilter);

  if(s.isEmpty())
    statusBar()->showMessage(tr("Opening aborted"), 2000);
  else {
    updateRecentFilesList(s);

    gotoPage(s);
    lastDirOpenSave = s;   // remember last directory and file

    statusBar()->showMessage(tr("Ready."));
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
  slotUpdateTreeview();
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSave()
{
  statusBar()->showMessage(tr("Saving file..."));
  DocumentTab->blockSignals(true);   // no user interaction during that time
  slotHideEdit(); // disable text edit of component property

  if(!saveFile()) {
    DocumentTab->blockSignals(false);
    statusBar()->showMessage(tr("Saving aborted"), 2000);
    statusBar()->showMessage(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->showMessage(tr("Ready."));

  if(!ProjName.isEmpty())
    slotUpdateTreeview();
}

// --------------------------------------------------------------
bool QucsApp::saveAs()
{
  QWidget *w = DocumentTab->currentWidget();
  QucsDoc *Doc = getDoc();

  int n = -1;
  QString s, Filter;
  QFileInfo Info;
  while(true) {
    s = Doc->DocName;
    Info.setFile(s);
    if(s.isEmpty()) {   // which is default directory ?
      if(ProjName.isEmpty()) {
        if(lastDirOpenSave.isEmpty())  s = QDir::currentPath();
        else  s = lastDirOpenSave;
      }
      else s = QucsSettings.QucsWorkDir.path();
    }

    // list of known file extensions
    QString ext = "vhdl;vhd;v;va;sch;dpl;m;oct;net;qnet;txt";
    QStringList extlist = ext.split (';');

    if(isTextDocument (w))
      Filter = tr("VHDL Sources")+" (*.vhdl *.vhd);;" +
	       tr("Verilog Sources")+" (*.v);;"+
	       tr("Verilog-A Sources")+" (*.va);;"+
	       tr("Octave Scripts")+" (*.m *.oct);;"+
	       tr("Qucs Netlist")+" (*.net *.qnet);;"+
	       tr("Plain Text")+" (*.txt);;"+
	       tr("Any File")+" (*)";
    else
      Filter = QucsFileFilter;

    s = QFileDialog::getSaveFileName(this, tr("Enter a Document Name"),
                                     QucsSettings.QucsWorkDir.absolutePath(),
                                     Filter);
    if(s.isEmpty())  return false;
    Info.setFile(s);               // try to guess the best extension ...
    ext = Info.suffix();

    if(ext.isEmpty() || !extlist.contains(ext))
    {
      // if no extension was specified or is unknown
      if (!isTextDocument (w))
      {
        // assume it is a schematic
        s += ".sch";
      }
    }

    Info.setFile(s);
    if(QFile::exists(s)) {
      n = QMessageBox::warning(this, tr("Warning"),
		tr("The file '")+Info.fileName()+tr("' already exists!\n")+
		tr("Saving will overwrite the old one! Continue?"),
		tr("No"), tr("Yes"), tr("Cancel"));
      if(n == 2) return false;    // cancel
      if(n == 0) continue;
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
  DocumentTab->setTabText(DocumentTab->indexOf(w), misc::properFileName(s));
  lastDirOpenSave = Info.absolutePath();  // remember last directory and file

  n = Doc->save();   // SAVE
  if(n < 0)  return false;

  updatePortNumber(Doc, n);
  slotUpdateTreeview();
  updateRecentFilesList(s);
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSaveAs()
{
  statusBar()->showMessage(tr("Saving file under new filename..."));
  DocumentTab->blockSignals(true);   // no user interaction during the time
  slotHideEdit(); // disable text edit of component property

  if(!saveAs()) {
    DocumentTab->blockSignals(false);
    statusBar()->showMessage(tr("Saving aborted"), 3000);
    statusBar()->showMessage(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->showMessage(tr("Ready."));

  // refresh the schematic file path
  slotRefreshSchPath();

  if(!ProjName.isEmpty())
    slotUpdateTreeview();
}


// --------------------------------------------------------------
void QucsApp::slotFileSaveAll()
{
  statusBar()->showMessage(tr("Saving all files..."));
  slotHideEdit(); // disable text edit of component property
  DocumentTab->blockSignals(true);   // no user interaction during the time

  int No=0;
  QucsDoc *Doc;  // search, if page is already loaded
  while((Doc=getDoc(No++)) != 0) {
    if(Doc->DocName.isEmpty())  // make document the current ?
      DocumentTab->setCurrentIndex(No-1);
    if (saveFile(Doc)) { // Hack! TODO: Maybe it's better to let slotFileChanged()
        DocumentTab->setTabIcon(No-1,QPixmap(empty_xpm)); // know about Tab number?
    }
  }

  DocumentTab->blockSignals(false);
  // Call update() to update subcircuit symbols in current Schematic document.
  // TextDoc has no viewport, it needs no update.
  QString tabType = DocumentTab->currentWidget()->metaObject()->className();

  if (tabType == "Schematic") {
    ((Q3ScrollView*)DocumentTab->currentWidget())->viewport()->update();
  }
  view->drawn = false;
  statusBar()->showMessage(tr("Ready."));

  // refresh the schematic file path
  slotRefreshSchPath();
  slotUpdateTreeview();
}

// --------------------------------------------------------------
// Close the currently active file tab
void QucsApp::slotFileClose()
{
    // Using file index -1 closes the currently active file window
    closeFile(-1);
}

// --------------------------------------------------------------
// Close the file tab specified by its index
void QucsApp::slotFileClose(int index)
{
    // Call closeFile with a specific tab index
    closeFile(index);
}

// --------------------------------------------------------------
// Common function to close a file tab specified by its index
// checking for changes in the file before doing so. If called
// index == -1, the active document will be closed
void QucsApp::closeFile(int index)
{
    statusBar()->showMessage(tr("Closing file..."));

    slotHideEdit(); // disable text edit of component property

    QucsDoc *Doc = getDoc(index);
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

    DocumentTab->removeTab(index);
    delete Doc;

    if(DocumentTab->count() < 1) { // if no document left, create an untitled
      Schematic *d = new Schematic(this, "");
      DocumentTab->addTab(d, QPixmap(empty_xpm), QObject::tr("untitled")); 
      DocumentTab->setCurrentIndex(0);
    }

    statusBar()->showMessage(tr("Ready."));
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


void QucsApp::slotFileExamples()
{
  statusBar()->showMessage(tr("Open examples directory..."));
  // pass the QUrl representation of a local file
  QDesktopServices::openUrl(QUrl::fromLocalFile(QucsSettings.ExamplesDir));
  statusBar()->showMessage(tr("Ready."));
}

void QucsApp::slotHelpTutorial()
{
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/tutorial/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpTechnical()
{
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/technical/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpReport()
{
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/report/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}



// --------------------------------------------------------------
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView(QWidget *w)
{

  editText->setHidden (true); // disable text edit of component property
  QucsDoc * Doc;
  if(w==NULL)return;
  // for text documents
  if (isTextDocument (w)) {
    TextDoc *d = (TextDoc*)w;
    Doc = (QucsDoc*)d;
    // update menu entries, etc. if neccesary
    magAll->setDisabled(true);
    if(cursorLeft->isEnabled())
      switchSchematicDoc (false);
  }
  // for schematic documents
  else {
    Schematic *d = (Schematic*)w;
    Doc = (QucsDoc*)d;
    magAll->setDisabled(false);
    // already in schematic?
    if(cursorLeft->isEnabled()) {
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

  HierarchyHistory.clear();
  popH->setEnabled(false);
}

// --------------------------------------------------------------
void QucsApp::slotFileSettings ()
{
  editText->setHidden (true); // disable text edit of component property

  QWidget * w = DocumentTab->currentWidget ();
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
  slotHideEdit(); // disable text edit of component property

  QucsSettingsDialog *d = new QucsSettingsDialog(this);
  d->exec();
  view->drawn = false;
}

// --------------------------------------------------------------
void QucsApp::slotRefreshSchPath()
{
  this->updateSchNameHash();
  this->updateSpiceNameHash();

  statusBar()->showMessage(tr("The schematic search path has been refreshed."), 2000);
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
  while((w=DocumentTab->widget(No++)) != 0) {
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
// printCurrentDocument: call printerwriter to print document
void QucsApp::printCurrentDocument(bool fitToPage)
{
  statusBar()->showMessage(tr("Printing..."));
  slotHideEdit(); // disable text edit of component property

  PrinterWriter *writer = new PrinterWriter();
  writer->setFitToPage(fitToPage);
  writer->print(DocumentTab->currentWidget());
  delete writer;

  statusBar()->showMessage(tr("Ready."));
  return;
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
  statusBar()->showMessage(tr("Exiting application..."));
  slotHideEdit(); // disable text edit of component property

  int exit = QMessageBox::information(this,
      tr("Quit..."), tr("Do you really want to quit?"),
      tr("Yes"), tr("No"));

  if(exit == 0)
    if(closeAllFiles()) {
      emit signalKillEmAll();   // kill all subprocesses
      qApp->quit();
    }

  statusBar()->showMessage(tr("Ready."));
}

//-----------------------------------------------------------------
// To get all close events.
void QucsApp::closeEvent(QCloseEvent* Event)
{
    qDebug()<<"x"<<pos().x()<<" ,y"<<pos().y();
    qDebug()<<"dx"<<size().width()<<" ,dy"<<size().height();
    QucsSettings.x=pos().x();
    QucsSettings.y=pos().y();
    QucsSettings.dx=size().width();
    QucsSettings.dy=size().height();
    saveApplSettings();

   if(closeAllFiles()) {
      emit signalKillEmAll();   // kill all subprocesses
      Event->accept();
      qApp->quit();
   }
   else
      Event->ignore();
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to go into a subcircuit.
void QucsApp::slotIntoHierarchy()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  Component *pc = Doc->searchSelSubcircuit();
  if(pc == 0) { return; }

  QString s = pc->getSubcircuitFile();
  if(!gotoPage(s)) { return; }

  HierarchyHistory.push(Doc->DocName); //remember for the way back
  popH->setEnabled(true);
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to leave a subcircuit.
void QucsApp::slotPopHierarchy()
{
  slotHideEdit(); // disable text edit of component property

  if(HierarchyHistory.size() == 0) return;

  QString Doc = HierarchyHistory.pop();

  if(!gotoPage(Doc)) {
    HierarchyHistory.push(Doc);
    return;
  }

  if(HierarchyHistory.size() == 0) {
    popH->setEnabled(false);
  }
}

// --------------------------------------------------------------
void QucsApp::slotShowAll()
{
  slotHideEdit(); // disable text edit of component property
  getDoc()->showAll();
}

// -----------------------------------------------------------
// Sets the scale factor to 1.
void QucsApp::slotShowOne()
{
  slotHideEdit(); // disable text edit of component property
  getDoc()->showNoZoom();
}

// -----------------------------------------------------------
void QucsApp::slotZoomOut()
{
  slotHideEdit(); // disable text edit of component property
  getDoc()->zoomBy(0.5f);
}


/*!
 * \brief QucsApp::slotSimulate
 *  is called when the simulate toolbar button is pressed.
 */
void QucsApp::slotSimulate()
{

  if (QucsSettings.DefaultSimulator!=spicecompat::simQucsator) {
      slotSimulateWithSpice();
      return;
  }

  slotHideEdit(); // disable text edit of component property

  QucsDoc *Doc;
  QWidget *w = DocumentTab->currentWidget();
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

  if(Info.suffix() == "m" || Info.suffix() == "oct") {
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

  if(sim->ErrText->document()->lineCount() > 1)   // were there warnings ?
    slotShowWarnings();

  int i=0;
  QWidget *w;  // search, if page is still open
  while((w=DocumentTab->widget(i++)) != 0)
    if(w == sim->DocWidget)
      break;

  if(sim->showBias == 0) {  // paint dc bias into schematic ?
    sim->slotClose();   // close and delete simulation window
    if(w) {  // schematic still open ?
      SweepDialog *Dia = new SweepDialog((Schematic*)sim->DocWidget);

      // silence warning about unused variable.
      Q_UNUSED(Dia)
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
  QString Name = Info.path() + QDir::separator() + DataDisplay;

  QWidget  *w = DocumentTab->currentWidget();

  int z = 0;  // search, if page is already loaded
  QucsDoc * d = findDoc (Name, &z);

  if(d)
    DocumentTab->setCurrentIndex(z);
  else {   // no open page found ?
    QString ext = QucsDoc::fileSuffix (DataDisplay);

    int i = 0;
    if (ext != "vhd" && ext != "vhdl" && ext != "v" && ext != "va" &&
	ext != "oct" && ext != "m") {
      d = new Schematic(this, Name);
      i = DocumentTab->addTab((Schematic *)d, QPixmap(empty_xpm), DataDisplay);
    }
    else {
      d = new TextDoc(this, Name);
      i = DocumentTab->addTab((TextDoc *)d, QPixmap(empty_xpm), DataDisplay);
    }
    DocumentTab->setCurrentIndex(i);

    QFile file(Name);
    if(file.open(QIODevice::ReadOnly)) {      // try to load document
      file.close();
      if(!d->load()) {
        delete d;
        view->drawn = false;
        return;
      }
    }
    else {
      if(file.open(QIODevice::ReadWrite)) {  // if document doesn't exist, create
        d->DataDisplay = Info.fileName();
        slotUpdateTreeview();
      }
      else {
        QMessageBox::critical(this, tr("Error"), tr("Cannot create ")+Name);
        return;
      }
      file.close();
    }

    d->becomeCurrent(true);
  }


  if(DocumentTab->currentWidget() == w)      // if page not ...
    if(!isTextDocument (w))
      ((Schematic*)w)->reloadGraphs();  // ... changes, reload here !

  TabView->setCurrentIndex(2);   // switch to "Component"-Tab
  if (Name.right(4) == ".dpl") {
    int i = Category::getModulesNr (QObject::tr("diagrams"));
    CompChoose->setCurrentIndex(i);   // switch to diagrams
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
// Called when file name in Project View is double-clicked
//   or open is selected in the context menu
void QucsApp::slotOpenContent(const QModelIndex &idx)
{
  editText->setHidden(true); // disable text edit of component property

  //test the item is valid
  if (!idx.isValid()) { return; }
  if (!idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString note = idx.sibling(idx.row(), 1).data().toString();
  QFileInfo Info(QucsSettings.QucsWorkDir.filePath(filename));
  QString extName = Info.suffix();

  if (extName == "sch" || extName == "dpl" || extName == "vhdl" ||
      extName == "vhd" || extName == "v" || extName == "va" ||
      extName == "m" || extName == "oct") {
    gotoPage(Info.absoluteFilePath());
    updateRecentFilesList(Info.absoluteFilePath());
    slotUpdateRecentFiles();

    if(note.isEmpty())     // is subcircuit ?
      if(extName == "sch") return;

    select->blockSignals(true);  // switch on the 'select' action ...
    select->setChecked(true);
    select->blockSignals(false);

    activeAction = select;
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    return;
  }

  if(extName == "dat") {
    editFile(Info.absoluteFilePath());  // open datasets with text editor
    return;
  }

  // File is no Qucs file, so go through list and search a user
  // defined program to open it.
  QStringList com;
  QStringList::const_iterator it = QucsSettings.FileTypes.constBegin();
  while(it != QucsSettings.FileTypes.constEnd()) {
    if(extName == (*it).section('/',0,0)) {
      QString progName = (*it).section('/',1,1);
      com = progName.split(" ");
      com << Info.absoluteFilePath();

      QProcess *Program = new QProcess();
      //Program->setCommunication(0);
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      env.insert("PATH", env.value("PATH") );
      Program->setProcessEnvironment(env);
      Program->start(com.join(" "));
      if(Program->state()!=QProcess::Running&&
              Program->state()!=QProcess::Starting) {
        QMessageBox::critical(this, tr("Error"),
               tr("Cannot start \"%1\"!").arg(Info.absoluteFilePath()));
        delete Program;
      }
      return;
    }
    it++;
  }

  // If no appropriate program was found, open as text file.
  editFile(Info.absoluteFilePath());  // open datasets with text editor
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectSubcircuit(const QModelIndex &idx)
{
  editText->setHidden(true); // disable text edit of component property

  if(!idx.isValid()) {   // mouse button pressed not over an item ?
    Content->clearSelection();  // deselect component in ListView
    return;
  }

  bool isVHDL = false;
  bool isVerilog = false;
  QModelIndex parentIdx = idx.parent();
  if(!parentIdx.isValid()) { return; }

  QString category = parentIdx.data().toString();

  if(category == tr("Schematics")) {
    if(idx.sibling(idx.row(), 1).data().toString().isEmpty())
      return;   // return, if not a subcircuit
  }
  else if(category == tr("VHDL"))
    isVHDL = true;
  else if(category == tr("Verilog"))
    isVerilog = true;
  else
    return;

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString note = idx.sibling(idx.row(), 1).data().toString();
  int idx_pag = DocumentTab->currentIndex();
  QString tab_titl = "";
  if (idx_pag>=0) tab_titl = DocumentTab->tabText(idx_pag);
  if (filename == tab_titl ) return; // Forbid to paste subcircuit into itself.

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem = 0;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setChecked(false);       // set last toolbar button off
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
  Comp->Props.first()->Value = idx.sibling(idx.row(), 0).data().toString();
  Comp->recreate(0);
  view->selElem = Comp;

  if(view->drawn)
    ((Q3ScrollView*)DocumentTab->currentWidget())->viewport()->update();
  view->drawn = false;
  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectLibComponent(QTreeWidgetItem *item)
{
    // get the current document
    Schematic *Doc = (Schematic*)DocumentTab->currentWidget();

    // if the current document is a schematic activate the paste
    if(!isTextDocument(Doc))
    {
        // if theres not a higher level item, this is a top level item,
        // not a component item so return
        if(item->parent() == 0) return;
        if(item->text(1).isEmpty()) return;   // return, if not a subcircuit

        // copy the subcircuit schematic to the clipboard
        QClipboard *cb = QApplication::clipboard();
        cb->setText(item->text(1));

        // activate the paste command
        slotEditPaste (true);
    }

}


// ---------------------------------------------------------
// This function is called if the document type changes, i.e.
// from schematic to text document or vice versa.
void QucsApp::switchSchematicDoc (bool SchematicMode)
{
  // switch our scroll key actions on/off according to SchematicMode
  cursorLeft->setEnabled(SchematicMode);
  cursorRight->setEnabled(SchematicMode);
  cursorUp->setEnabled(SchematicMode);
  cursorDown->setEnabled(SchematicMode);

  // text document
  if (!SchematicMode) {
    if (activeAction) {
      activeAction->blockSignals (true); // do not call toggle slot
      activeAction->setChecked(false);       // set last toolbar button off
      activeAction->blockSignals (false);
    }
    activeAction = select;
    select->blockSignals (true);
    select->setChecked(true);
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

  exportAsImage->setEnabled (SchematicMode); // only export schematic, no text

  editFind->setEnabled (!SchematicMode);
  insEntity->setEnabled (!SchematicMode);

  buildModule->setEnabled(!SchematicMode); // only build if VA document
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
  // no search in "symbol painting mode" as only paintings should be used
  CompSearch->setEnabled(SchematicMode);
  CompSearchClear->setEnabled(SchematicMode);
}

// ---------------------------------------------------------
void QucsApp::changeSchematicSymbolMode(Schematic *Doc)
{
  if(Doc->symbolMode) {
    // go into select modus to avoid placing a forbidden element
    select->setChecked(true);

    switchEditMode(false);
  }
  else
    switchEditMode(true);
}

// ---------------------------------------------------------
bool QucsApp::isTextDocument(QWidget *w) {
  return w->inherits("QPlainTextEdit");
}

// ---------------------------------------------------------
// Is called if the "symEdit" action is activated, i.e. if the user
// switches between the two painting mode: Schematic and (subcircuit)
// symbol.
void QucsApp::slotSymbolEdit()
{
  QWidget *w = DocumentTab->currentWidget();

  // in a text document (e.g. VHDL)
  if (isTextDocument (w)) {
    TextDoc *TDoc = (TextDoc*)w;
    // set 'DataDisplay' document of text file to symbol file
    QFileInfo Info(TDoc->DocName);
    QString sym = Info.completeBaseName()+".sym";
    TDoc->DataDisplay = sym;

    // symbol file already loaded?
    int paint_mode = 0;
    if (!findDoc (QucsSettings.QucsWorkDir.filePath(sym)))
      paint_mode = 1;

    // change current page to appropriate symbol file
    slotChangePage(TDoc->DocName,TDoc->DataDisplay);

    // set 'DataDisplay' document of symbol file to original text file
    Schematic *SDoc = (Schematic*)DocumentTab->currentWidget();
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
      slotHideEdit(); // disable text edit of component property
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
  double Imag = pm->powImag();
  if(Var == "Sopt")  // noise matching ?
    Imag *= -1.0;

  MatchDialog *Dia = new MatchDialog(this);
//  Dia->Ref1Edit->setText(QString::number(Z0));
  Dia->setS11LineEdits(pm->powReal(), Imag);
  Dia->setFrequency(pm->powFreq());
  Dia->setTwoPortMatch(false); // will also cause the corresponding impedance LineEdit to be updated

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
  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  int z = pm->pGraph->Var.indexOf(':');
  if(z <= 0)  DataSet = Doc->DataSet;
  else  DataSet = pm->pGraph->Var.mid(z+1);
  double Freq = pm->powFreq();

  QFileInfo Info(Doc->DocName);
  DataSet = Info.path()+QDir::separator()+DataSet;

  Diagram *Diag = new Diagram();

  // FIXME: use normal Diagrams.
  Graph *pg = new Graph(Diag, "S[1,1]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,1]."));
    return;
  }

  pg = new Graph(Diag, "S[1,2]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,2]."));
    return;
  }

  pg = new Graph(Diag, "S[2,1]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,1]."));
    return;
  }

  pg = new Graph(Diag, "S[2,2]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,2]."));
    return;
  }

  DataX const *Data = Diag->Graphs.first()->axis(0);
  if(Data->Var != "frequency") {
    QMessageBox::critical(0, tr("Error"), tr("Wrong dependency!"));
    return;
  }

  double *Value = Data->Points;
  // search for values for chosen frequency
  for(z=0; z<Data->count; z++)
    if(*(Value++) == Freq) break;

  // get S-parameters
  double S11real = *(Diag->Graphs.at(0)->cPointsY + 2*z);
  double S11imag = *(Diag->Graphs.at(0)->cPointsY + 2*z + 1);
  double S12real = *(Diag->Graphs.at(1)->cPointsY + 2*z);
  double S12imag = *(Diag->Graphs.at(1)->cPointsY + 2*z + 1);
  double S21real = *(Diag->Graphs.at(2)->cPointsY + 2*z);
  double S21imag = *(Diag->Graphs.at(2)->cPointsY + 2*z + 1);
  double S22real = *(Diag->Graphs.at(3)->cPointsY + 2*z);
  double S22imag = *(Diag->Graphs.at(3)->cPointsY + 2*z + 1);
  delete Diag;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->setTwoPortMatch(true);
  Dia->setFrequency(Freq);
  Dia->setS11LineEdits(S11real, S11imag);
  Dia->setS12LineEdits(S12real, S12imag);
  Dia->setS21LineEdits(S21real, S21imag);
  Dia->setS22LineEdits(S22real, S22imag);

  slotToPage();
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
// Is called if the "edit" action is clicked on right mouse button menu.
void QucsApp::slotEditElement()
{
  if(view->focusMEvent)
    view->editElement((Schematic*)DocumentTab->currentWidget(), view->focusMEvent);
}

// -----------------------------------------------------------
// Hides the edit for component property. Called e.g. if QLineEdit
// looses the focus.
void QucsApp::slotHideEdit()
{
  editText->setParent(this, 0);
  editText->setHidden(true);
}

// -----------------------------------------------------------
// set document tab icon to smallsave_xpm or empty_xpm
void QucsApp::slotFileChanged(bool changed)
{
  DocumentTab->setTabIcon(DocumentTab->currentIndex(),
      QPixmap((changed)? smallsave_xpm : empty_xpm));
}

// -----------------------------------------------------------
// Update project view by call refresh function
// looses the focus.
void QucsApp::slotUpdateTreeview()
{
  Content->refresh();
}

// -----------------------------------------------------------
// Searches the qucs path list for all schematic files and creates
// a hash for lookup later
void QucsApp::updateSchNameHash(void)
{
    // update the list of paths to search in qucsPathList, this
    // removes nonexisting entries
    updatePathList();

    // now go through the paths creating a map to all the schematic files
    // found in the directories. Note that we go through the list of paths from
    // first index to last index. Since keys are unique it means schematic files
    // in directories at the end of the list take precendence over those at the
    // start of the list, we should warn about shadowing of schematic files in
    // this way in the future
    QStringList nameFilter;
    nameFilter << "*.sch";

    // clear out any existing hash table entriess
    schNameHash.clear();

    foreach (QString qucspath, qucsPathList) {
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        foreach (QFileInfo schfile, schfilesList) {
            QString bn = schfile.completeBaseName();
            schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    foreach (QFileInfo schfile, schfilesList) {
        schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
    }
}

// -----------------------------------------------------------
// Searches the qucs path list for all spice files and creates
// a hash for lookup later
void QucsApp::updateSpiceNameHash(void)
{
    // update the list of paths to search in qucsPathList, this
    // removes nonexisting entries
    updatePathList();

    // now go through the paths creating a map to all the spice files
    // found in the directories. Note that we go through the list of paths from
    // first index to last index. Since keys are unique it means spice files
    // in directories at the end of the list take precendence over those at the
    // start of the list, we should warn about shadowing of spice files in
    // this way in the future

    // clear out any existing hash table entriess
    spiceNameHash.clear();

    foreach (QString qucspath, qucsPathList) {
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList spicefilesList = thispath.entryInfoList( QucsSettings.spiceExtensions, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        foreach (QFileInfo spicefile, spicefilesList) {
            QString bn = spicefile.completeBaseName();
            schNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList spicefilesList = thispath.entryInfoList( QucsSettings.spiceExtensions, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    foreach (QFileInfo spicefile, spicefilesList) {
        spiceNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
    }
}

// -----------------------------------------------------------
// update the list of paths, pruning non-existing paths
void QucsApp::updatePathList(void)
{
    // check each path actually exists, if not remove it
    QMutableListIterator<QString> i(qucsPathList);
    while (i.hasNext()) {
        i.next();
        QDir thispath(i.value());
        if (!thispath.exists())
        {
            // the path does not exist, remove it from the list
            i.remove();
        }
    }
}

// replace the old path list with a new one
void QucsApp::updatePathList(QStringList newPathList)
{
    // clear out the old path list
    qucsPathList.clear();

    // copy the new path into the path list
    foreach(QString path, newPathList)
    {
        qucsPathList.append(path);
    }
    // do the normal path update operations
    updatePathList();
}


void QucsApp::updateRecentFilesList(QString s)
{
  QSettings* settings = new QSettings("qucs","qucs");
  QucsSettings.RecentDocs.removeAll(s);
  QucsSettings.RecentDocs.prepend(s);
  if (QucsSettings.RecentDocs.size() > MaxRecentFiles) {
    QucsSettings.RecentDocs.removeLast();
  }
  settings->setValue("RecentDocs",QucsSettings.RecentDocs.join("*"));
  delete settings;
  slotUpdateRecentFiles();
}

void QucsApp::slotSaveDiagramToGraphicsFile()
{
  slotSaveSchematicToGraphicsFile(true);
}

void QucsApp::slotSaveSchematicToGraphicsFile(bool diagram)
{
  ImageWriter *writer = new ImageWriter(lastExportFilename);
  writer->setDiagram(diagram);
  if (!writer->print(DocumentTab->currentWidget())) {
    lastExportFilename = writer->getLastSavedFile();
    statusBar()->showMessage(QObject::tr("Successfully exported"), 2000);
  }
  delete writer;
}


void QucsApp::slotSimSettings()
{
    SimSettingsDialog *SetDlg = new SimSettingsDialog(this);
    SetDlg->exec();
    delete SetDlg;
    SimulatorLabel->setText(spicecompat::getDefaultSimulatorName());
}

void QucsApp::slotSimulateWithSpice()
{
    if (!isTextDocument(DocumentTab->currentPage())) {
        Schematic *sch = (Schematic*)DocumentTab->currentPage();

        ExternSimDialog *SimDlg = new ExternSimDialog(sch);
        connect(SimDlg,SIGNAL(simulated()),this,SLOT(slotAfterSpiceSimulation()));
        connect(SimDlg,SIGNAL(warnings()),this,SLOT(slotShowWarnings()));
        connect(SimDlg,SIGNAL(success()),this,SLOT(slotResetWarnings()));
        SimDlg->exec();
        disconnect(SimDlg,SIGNAL(simulated()),this,SLOT(slotAfterSpiceSimulation()));
        disconnect(SimDlg,SIGNAL(warnings()),this,SLOT(slotShowWarnings()));
        disconnect(SimDlg,SIGNAL(success()),this,SLOT(slotResetWarnings()));
        if (SimDlg->wasSimulated && sch->SimOpenDpl)
            if (sch->showBias < 1) slotChangePage(sch->DocName,sch->DataDisplay);
        delete SimDlg;
    }
}

void QucsApp::slotAfterSpiceSimulation()
{
    Schematic *sch = (Schematic*)DocumentTab->currentPage();
    sch->reloadGraphs();
    sch->viewport()->update();
    if(sch->SimRunScript) {
      // run script
      octave->startOctave();
      octave->runOctaveScript(sch->Script);
    }
}

void QucsApp::slotBuildVAModule()
{
    if (!isTextDocument(DocumentTab->currentPage())) {
        Schematic *Sch = (Schematic*)DocumentTab->currentPage();

        QFileInfo inf(Sch->DocName);
        QString filename = QFileDialog::getSaveFileName(this,tr("Save Verilog-A module"),
                                                        inf.path()+QDir::separator()+"testmodule.va",
                                                        "Verilog-A (*.va)");
        if (filename.isEmpty()) return;

        QFile f(filename);
        if (f.open(QIODevice::WriteOnly)) {
            QTextStream stream(&f);
            VerilogAwriter *writer = new VerilogAwriter;
            if (!writer->createVA_module(stream,Sch)) {
                QMessageBox::critical(this,tr("Build Verilog-A module"),
                                      tr("This schematic is not a subcircuit!\n"
                                         "Use subcircuit to crete Verilog-A module!"),
                                          QMessageBox::Ok);
            }
            delete writer;
            f.close();
            Content->refresh();
        }
    }

}
