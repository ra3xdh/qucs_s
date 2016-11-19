/**************************************************************************
                               qucslib.cpp
                              -------------
    begin                : Sat May 28 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include <QMenu>
#include <QVBoxLayout>
#include <QListWidget>
#include <QtDebug>
#include <QGroupBox>
#include <QTextStream>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QComboBox>
#include <QClipboard>
#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QRegExp>
#include <QCloseEvent>
#include <QPixmap>
#include <QWidget>
#include <QBrush>
#include <QLineEdit>
#include <QPair>

#include "qucslib.h"
#include "qucslib_common.h"
#include "librarydialog.h"
#include "displaydialog.h"
#include "symbolwidget.h"

struct compInfoStruct
{
  QString libName; // the name of the library where the component is defined
  QString libPath; // the library path (absolute for  user libs, relative for system libs)
  QString compDef; // the component definition string
};

struct libInfoStruct // a struct is not really needed but useful if we need to add further data...
{
  QString libPath; // the library path (absolute for  user libs, relative for system libs)
};

Q_DECLARE_METATYPE(compInfoStruct)
Q_DECLARE_METATYPE(libInfoStruct)

/* Constructor setups the GUI. */
QucsLib::QucsLib()
{
    // set application icon
    setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
    setWindowTitle("Qucs Library Tool " PACKAGE_VERSION);

    // create file menu
    fileMenu = new QMenu(tr("&File"));

    QAction * manageLib =new QAction (tr("Manage User &Libraries..."), this);
    manageLib->setShortcut(Qt::CTRL+Qt::Key_M);
    connect(manageLib, SIGNAL(triggered()), SLOT(slotManageLib()));

    QAction * fileQuit = new QAction(tr("&Quit"), this);
    fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    connect(fileQuit, SIGNAL(triggered()), SLOT(slotQuit()));

    fileMenu->addAction(manageLib);
    fileMenu->addSeparator();
    fileMenu->addAction(fileQuit);


    // create help menu
    helpMenu = new QMenu(tr("&Help"));

    QAction * helpHelp = new QAction(tr("&Help"), this);
    helpHelp->setShortcut(Qt::Key_F1);
    helpMenu->addAction(helpHelp);
    connect(helpHelp, SIGNAL(triggered()), SLOT(slotHelp()));

    QAction * helpAbout = new QAction(tr("About"), this);
    helpMenu->addAction(helpAbout);
    connect(helpAbout, SIGNAL(triggered()), SLOT(slotAbout()));

    // use Escape key to clear search
    QAction *escape = new QAction(this);
    escape->setShortcut(Qt::Key_Escape);
    connect(escape, SIGNAL(triggered()), SLOT(slotSearchClear()));
    this->addAction(escape);

    // setup menu bar
    menuBar()->addMenu(fileMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);

    // main box
    QWidget *main = new QWidget(this);
    setCentralWidget(main);
    all = new QVBoxLayout();
    main->setLayout(all);
    all->setSpacing (0);
    all->setMargin (0);


    // library and component choice
    QGroupBox *LibGroup = new QGroupBox(tr("Component Selection"));
    QVBoxLayout *LibGroupLayout = new QVBoxLayout();
    Library = new QComboBox();
    connect(Library, SIGNAL(activated(int)), SLOT(slotSelectLibrary(int)));
    CompList = new QListWidget();
    connect(CompList, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(slotShowComponent(QListWidgetItem*)));
    connect(CompList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(slotShowComponent(QListWidgetItem*)));
    connect(CompList, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(slotShowComponent(QListWidgetItem*)));

    CompSearch = new QLineEdit(this);
    CompSearch->setPlaceholderText(tr("Search Lib Components"));
    QPushButton *CompSearchClear = new QPushButton(tr("Clear"));
    connect(CompSearch, SIGNAL(textEdited(const QString &)), SLOT(slotSearchComponent(const QString &)));
    connect(CompSearchClear, SIGNAL(clicked()), SLOT(slotSearchClear()));

    LibGroupLayout->addWidget(Library);
    LibGroupLayout->addWidget(CompList);
    QHBoxLayout *CompSearchLayout = new QHBoxLayout();
    LibGroupLayout->addLayout(CompSearchLayout);
    CompSearchLayout->addWidget(CompSearch);
    CompSearchLayout->addWidget(CompSearchClear);

    LibGroup->setLayout(LibGroupLayout);


    QGroupBox *CompGroup = new QGroupBox(tr("Component"));
    QVBoxLayout *CompGroupLayout = new QVBoxLayout();
    CompDescr = new QTextEdit();
    CompDescr->setReadOnly(true);
    CompDescr->setWordWrapMode(QTextOption::NoWrap);


    Symbol = new SymbolWidget();

    QGroupBox *ButtonGroup = new QGroupBox();
    QHBoxLayout *ButtonGroupLayout = new QHBoxLayout();

    QPushButton * CopyButton = new QPushButton (tr("Copy to clipboard"));
    connect(CopyButton, SIGNAL(clicked()), SLOT(slotCopyToClipBoard()));
    QPushButton * ShowButton = new QPushButton (tr("Show Model"));
    connect(ShowButton, SIGNAL(clicked()), SLOT(slotShowModel()));

    ButtonGroupLayout->addWidget(CopyButton);
    ButtonGroupLayout->addWidget(ShowButton);
    ButtonGroup->setLayout(ButtonGroupLayout);


    CompGroupLayout->addWidget(CompDescr);
    CompGroupLayout->addWidget(Symbol);
    CompGroupLayout->addWidget(ButtonGroup);
    CompGroup->setLayout(CompGroupLayout);


    // main layout
    QWidget *h = new QWidget();
    QHBoxLayout *lh = new QHBoxLayout();
    lh->addWidget(LibGroup);
    lh->addWidget(CompGroup);
    h->setLayout(lh);
    all->addWidget(h);

    putLibrariesIntoCombobox();

}

/* Destructor destroys the application. */
QucsLib::~QucsLib()
{
}

// ----------------------------------------------------
// Put all available libraries into ComboBox.
void QucsLib::putLibrariesIntoCombobox()
{
    QList<QPair<QString, bool> > LibFiles;
    ComponentLibrary parsedlib;
    QString libPath;
    libInfoStruct lineLibInfo;
    QVariant v;

    QPixmap userLibPixmap = QPixmap(":/bitmaps/home.png");
    QPixmap sysLibPixmap = QPixmap(":/bitmaps/big.qucs.xpm");

    Library->clear();
    UserLibCount = 0;

    // user libraries
    QStringList UserLibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    UserLibCount = UserLibFiles.count();
    foreach(QString s, UserLibFiles) // build list with full path
      LibFiles.append(qMakePair(UserLibDir.absoluteFilePath(s), false));

    // system libraries
    QStringList SysLibFiles = SysLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    foreach(QString s, SysLibFiles) // build list with relative path
      LibFiles.append(qMakePair(s, true));

    for (int i = 0; i < LibFiles.count(); ++i ) {
      libPath = LibFiles[i].first;
      libPath.chop(4); // remove extension
      int result = parseComponentLibrary(libPath, parsedlib, QUCS_COMP_LIB_HEADER_ONLY); // need just the library name
      switch (result) {
      case QUCS_COMP_LIB_IO_ERROR:
      {
        QString filename = getLibAbsPath(LibFiles[i].first);
        QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
        return;
      }
      case QUCS_COMP_LIB_CORRUPT:
        QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
        return;
      default:
        break;
      }

      lineLibInfo = libInfoStruct{libPath};
      v.setValue(lineLibInfo);
      if (LibFiles[i].second) { // it's a system library ?
        Library->addItem(sysLibPixmap, parsedlib.name, v);
      } else {
        Library->addItem(userLibPixmap, parsedlib.name, v);
      }
    }
    if (UserLibCount > 0) {
      // add a separator to distinguish between user libraries and system libs
      Library->insertSeparator(UserLibCount);
    }
    slotSelectLibrary(0);
}

// ----------------------------------------------------
void QucsLib::slotAbout()
{
    QMessageBox::about(this, tr("About..."),
                       "QucsLib Version " PACKAGE_VERSION "\n"+
                       tr("Library Manager for Qucs\n")+
                       tr("Copyright (C) 2011-2016 Qucs Team\n")+
                       tr("Copyright (C) 2005 by Michael Margraf\n")+
                       "\nThis is free software; see the source for copying conditions."
                       "\nThere is NO warranty; not even for MERCHANTABILITY or "
                       "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

// ----------------------------------------------------
void QucsLib::slotQuit()
{
    int tmp;
    tmp = x();		// call size and position function in order to ...
    tmp = y();		// ... set them correctly before closing the ...
    tmp = width();	// dialog !!!  Otherwise the frame of the window ...
    tmp = height();	// will not be recognized (a X11 problem).
    Q_UNUSED(tmp);

    qApp->quit();
}

// ----------------------------------------------------
void QucsLib::closeEvent(QCloseEvent *Event)
{
    int tmp;
    tmp = x();		// call size and position function in order to ...
    tmp = y();		// ... set them correctly before closing the ...
    tmp = width();	// dialog !!!  Otherwise the frame of the window ...
    tmp = height();	// will not be recognized (a X11 problem).
    Q_UNUSED(tmp);

    Event->accept();
}

// ----------------------------------------------------
void QucsLib::slotManageLib()
{
    (new LibraryDialog(this))->exec();
    putLibrariesIntoCombobox();
    CompSearch->clear(); // in case "search mode" was previously active
}

// ----------------------------------------------------
void QucsLib::slotHelp()
{
    QMessageBox helpBox;
    helpBox.setWindowTitle(tr("QucsLib Help"));
    helpBox.setText(tr("QucsLib is a program to manage Qucs component libraries. "
                       "On the left side of the application window the available "
                       "libraries can be browsed to find the wanted component. "
                       "By clicking on the component name its description can be "
                       "seen on the right side. The selected component is "
                       "transported to the Qucs application by clicking on the "
                       "button \"Copy to Clipboard\". Being back in the schematic "
                       "window the component can be inserted by pressing CTRL-V "
                       " (paste from clipboard).") + "\n" +
                    tr("A more comfortable way: The component can also be placed "
                       "onto the schematic by using Drag n'Drop."));
    helpBox.exec();
}

// ----------------------------------------------------
void QucsLib::slotCopyToClipBoard()
{
    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n  " +
         Symbol->theModel() +
         "\n</Components>\n";

    // put resulting schematic into clipboard
    QClipboard *cb = QApplication::clipboard();
    cb->setText(s);
}

// ----------------------------------------------------
void QucsLib::slotShowModel()
{
    DisplayDialog *d = new DisplayDialog(this, Symbol->ModelString, Symbol->VHDLModelString, Symbol->VerilogModelString);
    d->setWindowTitle(tr("Model"));
    d->resize(500, 150);
    d->show();
}

// ----------------------------------------------------
void QucsLib::slotSelectLibrary(int Index)
{
    compInfoStruct lineCompInfo;
    QVariant v;

    // was in "search mode" ?
    if (Library->itemText(0) == tr("Search results")) {
      if (Index == 0) // user selected "Search results" item
        return;
      Library->removeItem(0); // remove the added "Search results" item
      CompSearch->clear();
      --Index; // adjust requested index since item 0 was removed
    }

    // make sure the right index is selected
    //  (might have been called by a cleared search and not by user action)
    Library->setCurrentIndex(Index);

    CompList->clear ();

    libInfoStruct lineLibInfo;
    v = Library->itemData(Index, Qt::UserRole);
    lineLibInfo = v.value<libInfoStruct>();

    ComponentLibrary parsedlib;
    int result = parseComponentLibrary (lineLibInfo.libPath, parsedlib);

    switch (result)
    {
        case QUCS_COMP_LIB_IO_ERROR:
        {
            QString filename = getLibAbsPath(lineLibInfo.libPath);
            QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
            return;
        }
        case QUCS_COMP_LIB_CORRUPT:
            QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
            return;
        default:
            break;
    }

    // Now go through the rest of the component library, extracting each
    // component name
    for (int i = 0; i < parsedlib.components.count (); i++)
    {
        QListWidgetItem *CompItem = new QListWidgetItem(parsedlib.components[i].name);
        lineCompInfo = compInfoStruct{parsedlib.name, lineLibInfo.libPath, parsedlib.components[i].definition};
        v.setValue(lineCompInfo);
        CompItem->setData(Qt::UserRole, v);
        CompList->addItem(CompItem);
    }

    CompList->setCurrentRow(0); // select first item
    slotShowComponent(CompList->item(0)); // and make the corresponding component info appear as well...
}

// ----------------------------------------------------
void QucsLib::slotSearchComponent(const QString &searchText)
{
  QList<QPair<QString, bool> > LibFiles;
  QString libPath;
  compInfoStruct lineCompInfo;
  QVariant v;

  // clear the components view
  //   (search restarts anew at every keypress)
  CompList->clear ();

  if (Library->itemText(0) != tr("Search results")) {
    // not already in "search mode"
    libCurIdx = Library->currentIndex(); // remember current selected item
    // insert "Search results" at the beginning, so that it is visible
    Library->insertItem(-1, tr("Search results"));
    Library->setCurrentIndex(0);
  }

  if(searchText.isEmpty()) {
    return;
  }

  bool findComponent = false;

  // user libraries
  QStringList UserLibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
  foreach(QString s, UserLibFiles) // build list with full path
    LibFiles.append(qMakePair(UserLibDir.absoluteFilePath(s), false));

  // system libraries
  QStringList SysLibFiles = SysLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
  foreach(QString s, SysLibFiles) // build list with relative path
    LibFiles.append(qMakePair(s, true));

  QFile File;
  QTextStream ReadWhole;
  QString LibraryString, LibName, CompName;
  int Start, End, NameStart, NameEnd;
  for(auto it = LibFiles.begin(); it != LibFiles.end(); it++) { // all library files
    QString libPath = (*it).first;
    libPath.chop(4); // remove extension
    File.setFileName(getLibAbsPath(libPath));
    if(!File.open(QIODevice::ReadOnly))  continue;

    ReadWhole.setDevice(&File);
    LibraryString = ReadWhole.readAll();
    File.close();

    Start = LibraryString.indexOf("<Qucs Library ");
    if(Start < 0)  continue;
    End = LibraryString.indexOf('>', Start);
    if(End < 0)  continue;
    LibName = LibraryString.mid(Start, End-Start).section('"', 1, 1);

    // check all components of the current library
    while((Start=LibraryString.indexOf("\n<Component ", Start)) > 0) {
      Start++;
      NameStart = Start + 11;
      NameEnd = LibraryString.indexOf('>', NameStart);
      if(NameEnd < 0)  continue;
      CompName = LibraryString.mid(NameStart, NameEnd-NameStart);

      End = LibraryString.indexOf("\n</Component>", NameEnd);
      if(End < 0)  continue;
      End += 13;

      // does search criterion match ?
      if(CompName.indexOf(searchText, 0, Qt::CaseInsensitive) >= 0) {
        if(!findComponent) {
	  CompList->clear();
        }
        findComponent = true;
        QListWidgetItem *CompItem = new QListWidgetItem(CompName);

        lineCompInfo = compInfoStruct{LibName, libPath, LibraryString.mid(Start, End-Start)};
        v.setValue(lineCompInfo);
        CompItem->setData(Qt::UserRole, v);
        CompList->addItem(CompItem);
      }
      Start = End;
    }
  }
}


void QucsLib::slotSearchClear()
{
  // was in "search mode" ?
  if (Library->itemText(0) == tr("Search results")) {
    Library->removeItem(0); // remove the added "Search results" item
    CompSearch->clear();
    // go back to the panel selected before search started
    slotSelectLibrary(libCurIdx);
  }
}


// ----------------------------------------------------
void QucsLib::slotShowComponent(QListWidgetItem *Item)
{
    compInfoStruct lineCompInfo;
    QVariant v;
    libInfoStruct lineLibInfo;
    QVariant vLib;
    int i;

    if(!Item) return;

    // get component info
    v = Item->data(Qt::UserRole);
    lineCompInfo = v.value<compInfoStruct>();

    CompDescr->setText("Name: " + Item->text());
    CompDescr->append("Library: " + lineCompInfo.libName);
    CompDescr->append("----------------------------");

    // find corresponding library index in the libraries combobox

    // findData() below does not work: in Qt4 comparison operators for MetaTypes are not available (but are in Qt5)
    //lineLibInfo = libInfoStruct{lineCompInfo.libPath};
    //vLib.setValue(lineLibInfo);
    //int i = Library->findData(lineLibInfo, Qt::UserRole);

    // compare component library path and combobox selected library path
    for (i = 0; i < Library->count(); i++ )
    {
        vLib = Library->itemData(i);
        lineLibInfo = vLib.value<libInfoStruct>();
        if (lineLibInfo.libPath == lineCompInfo.libPath)
          break;
    }

    QString content;
    if(!getSection("Description", lineCompInfo.compDef, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    CompDescr->append(content);

    if(!getSection("Model", lineCompInfo.compDef, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    Symbol->ModelString = content;

    if(!getSection("VHDLModel", lineCompInfo.compDef, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    Symbol->VHDLModelString = content;

    if(!getSection("VerilogModel", lineCompInfo.compDef, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    Symbol->VerilogModelString = content;

    if(!getSection("Symbol", lineCompInfo.compDef, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    Symbol->setSymbol(content, lineCompInfo.libPath, Item->text());

    // change currently selected category, so the user will 
    //   learn where the component comes from
    Library->setCurrentIndex(i);
    // remove 1 to find the actual index (when in Search Mode ther is one more
    //   item  due to the added "Search Results" item)
    libCurIdx = i-1; // remember the category to select when exiting search
    //!! comment out the above two lines if you would like that the search
    //!!   returns back to the last selected category instead
}


