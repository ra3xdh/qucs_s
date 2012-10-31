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

#include <qmenubar.h>
#include <qaction.h>
#include <q3popupmenu.h>
#include <qcombobox.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qlayout.h>
#include <q3hbox.h>
#include <q3vgroupbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <q3textedit.h>
#include <q3listbox.h>
#include <qregexp.h>

#include "qucslib.h"
//Added by qt3to4:
#include <Q3TextStream>
#include <QCloseEvent>
#include <QPixmap>
#include <Q3VBoxLayout>
#include "librarydialog.h"
#include "displaydialog.h"
#include "symbolwidget.h"
#include "searchdialog.h"


/* Constructor setups the GUI. */
QucsLib::QucsLib()
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("Qucs Library Tool " PACKAGE_VERSION);

  QMenuBar * menuBar = new QMenuBar (this);

  // create file menu
  Q3PopupMenu * fileMenu = new Q3PopupMenu ();
  QAction * manageLib =
    new QAction (tr("Manage User &Libraries..."), Qt::CTRL+Qt::Key_M, this,"");
  manageLib->addTo (fileMenu);
  connect(manageLib, SIGNAL(activated()), SLOT(slotManageLib()));

  fileMenu->insertSeparator();

  QAction * fileQuit =
    new QAction (tr("&Quit"), Qt::CTRL+Qt::Key_Q, this,"");
  fileQuit->addTo (fileMenu);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  // create help menu
  Q3PopupMenu * helpMenu = new Q3PopupMenu ();
  QAction * helpHelp =
    new QAction (tr("&Help"), Qt::Key_F1, this,"");
  helpHelp->addTo (helpMenu);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelp()));
  QAction * helpAbout =
    new QAction (tr("About"), 0, helpMenu,"");
  helpAbout->addTo (helpMenu);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotAbout()));

  // setup menu bar
  menuBar->insertItem (tr("&File"), fileMenu);
  menuBar->insertSeparator ();
  menuBar->insertItem (tr("&Help"), helpMenu);

  // main box
  Q3VBoxLayout * all = new Q3VBoxLayout (this);
  all->setSpacing (0);
  all->setMargin (0);

  // reserve space for menubar
  QWidget * Space = new QWidget (this);
  Space->setFixedSize(5, menuBar->height() + 2);
  all->addWidget (Space);

  // main layout
  Q3HBox * h = new Q3HBox (this);
  h->setSpacing (5);
  h->setMargin (3);
  all->addWidget (h);

  // library and component choice
  Q3VGroupBox * LibGroup = new Q3VGroupBox (tr("Component Selection"), h);
  Library = new QComboBox (LibGroup);
  connect(Library, SIGNAL(activated(int)), SLOT(slotSelectLibrary(int)));
  CompList = new Q3ListBox(LibGroup);
  connect(CompList, SIGNAL(highlighted(Q3ListBoxItem*)),
	SLOT(slotShowComponent(Q3ListBoxItem*)));

  Q3HBox * h1 = new Q3HBox (LibGroup);
  QPushButton * SearchButton = new QPushButton (tr("Search..."), h1);
  connect(SearchButton, SIGNAL(clicked()), SLOT(slotSearchComponent()));
  h1->setStretchFactor(new QWidget(h1), 5); // stretchable placeholder


  // component display
  Q3VGroupBox *CompGroup = new Q3VGroupBox (tr("Component"), h);
  CompDescr = new Q3TextEdit(CompGroup);
  CompDescr->setTextFormat(Qt::PlainText);
  CompDescr->setReadOnly(true);
  CompDescr->setWordWrap(Q3TextEdit::NoWrap);

  Symbol = new SymbolWidget (CompGroup);

  Q3HBox * h2 = new Q3HBox (CompGroup);
  QPushButton * CopyButton = new QPushButton (tr("Copy to clipboard"), h2);
  connect(CopyButton, SIGNAL(clicked()), SLOT(slotCopyToClipBoard()));
  QPushButton * ShowButton = new QPushButton (tr("Show Model"), h2);
  connect(ShowButton, SIGNAL(clicked()), SLOT(slotShowModel()));

  // ......................................................
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
  Library->clear();

  UserLibCount = 0;
  QStringList LibFiles;
  QStringList::iterator it;
  if(UserLibDir.cd(".")) { // user library directory exists ?
    LibFiles = UserLibDir.entryList("*.lib", QDir::Files, QDir::Name);
    UserLibCount = LibFiles.count();

    for(it = LibFiles.begin(); it != LibFiles.end(); it++)
      Library->insertItem((*it).left((*it).length()-4));
  }

  QDir LibDir(QucsSettings.LibDir);
  LibFiles = LibDir.entryList("*.lib", QDir::Files, QDir::Name);

  for(it = LibFiles.begin(); it != LibFiles.end(); it++)
    Library->insertItem((*it).left((*it).length()-4));

  slotSelectLibrary(0);
}

// ----------------------------------------------------
void QucsLib::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsLib Version " PACKAGE_VERSION "\n"+
    tr("Library Manager for Qucs\n")+
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

  accept();
}

// ----------------------------------------------------
void QucsLib::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  Event->accept();
}

// ----------------------------------------------------
void QucsLib::slotManageLib()
{
  (new LibraryDialog(this))->exec();
  putLibrariesIntoCombobox();
}

// ----------------------------------------------------
void QucsLib::slotHelp()
{
  DisplayDialog *d = new DisplayDialog(this);
  d->setCaption(tr("QucsLib Help"));
  d->resize(250, 325);
  d->Text->setText(
     tr("QucsLib is a program to manage Qucs component libraries. "
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
  d->show();
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
  DisplayDialog *d = new DisplayDialog(this, false);
  d->setCaption(tr("Model"));
  d->resize(500, 150);
  d->Text->setText(Symbol->ModelString);
  d->Text->setWordWrap(Q3TextEdit::NoWrap);
  d->VHDLText->setText(Symbol->VHDLModelString);
  d->VHDLText->setWordWrap(Q3TextEdit::NoWrap);
  d->VerilogText->setText(Symbol->VerilogModelString);
  d->VerilogText->setWordWrap(Q3TextEdit::NoWrap);
  d->show();
}

// ----------------------------------------------------
void QucsLib::slotSelectLibrary(int Index)
{
  int Start, End, NameStart, NameEnd;
  End = Library->count()-1;
  if(Library->text(End) == tr("Search result")) {
    if(Index < End)
      Library->removeItem(End); // if search result still there -> remove it
    else  return;
  }

  CompList->clear();
  LibraryComps.clear();
  DefaultSymbol = "";

  QFile file;
  if(Index < UserLibCount)  // Is it user library ?
    file.setName(UserLibDir.absPath() + QDir::separator() + Library->text(Index) + ".lib");
  else
    file.setName(QucsSettings.LibDir + Library->text(Index) + ".lib");

  if(!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("Error"),
        tr("Cannot open \"%1\".").arg(
           QucsSettings.LibDir + Library->text(Index) + ".lib"));
    return;
  }

  Q3TextStream ReadWhole(&file);
  QString LibraryString = ReadWhole.read();
  file.close();

  Start = LibraryString.find("<Qucs Library ");
  if(Start < 0) {
    QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
    return;
  }
  End = LibraryString.find('>', Start);
  if(End < 0) {
    QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
    return;
  }
  QString LibName = LibraryString.mid(Start, End-Start).section('"', 1, 1);

  Start = LibraryString.find("\n<", End);
  if(Start < 0) return;
  if(LibraryString.mid(Start+2, 14) == "DefaultSymbol>") {
    End = LibraryString.find("\n</DefaultSymbol>");
    if(End < 0) {
      QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
      return;
    }

    DefaultSymbol = LibraryString.mid(Start+16, End-Start-16);
    Start = End + 3;
  }

  while((Start=LibraryString.find("\n<Component ", Start)) > 0) {
    Start++;
    NameStart = Start + 11;
    NameEnd = LibraryString.find('>', NameStart);
    if(NameEnd < 0)  continue;

    End = LibraryString.find("\n</Component>", NameEnd);
    if(End < 0)  continue;
    End += 13;

    CompList->insertItem(LibraryString.mid(NameStart, NameEnd-NameStart));
    LibraryComps.append(LibName+'\n'+LibraryString.mid(Start, End-Start));
    Start = End;
  }

  CompList->setSelected(0, true);  // select first item
}

// ----------------------------------------------------
void QucsLib::slotSearchComponent()
{
  SearchDialog *d = new SearchDialog(this);
  d->setCaption(tr("Search Library Component"));
  if(d->exec() == QDialog::Accepted)
    QMessageBox::information(this, tr("Result"),
                             tr("No appropriate component found."));
}

// ----------------------------------------------------
void QucsLib::slotShowComponent(Q3ListBoxItem *Item)
{
  if(!Item) return;

  QString CompString = LibraryComps.at(CompList->index(Item));
  QString LibName = (CompString).section('\n', 0, 0);
  CompDescr->setText("Name: " + Item->text());
  CompDescr->append("Library: " + LibName);
  CompDescr->append("----------------------------");

  if(Library->currentItem() < UserLibCount)
    LibName = UserLibDir.absPath() + QDir::separator() + LibName;

  QString content;
  if(!getSection("Description", CompString, content))
    return;
  CompDescr->append(content);

  if(!getSection("Model", CompString, content))
    return;
  Symbol->ModelString = content;
  if(Symbol->ModelString.count('\n') < 2)
    Symbol->createSymbol(LibName, Item->text());

  if(!getSection("VHDLModel", CompString, content))
    return;
  Symbol->VHDLModelString = content;

  if(!getSection("VerilogModel", CompString, content))
    return;
  Symbol->VerilogModelString = content;

  if(!getSection("Symbol", CompString, content))
    return;
  if(!content.isEmpty())
    Symbol->setSymbol(content, LibName, Item->text());
  else if(!DefaultSymbol.isEmpty())   // has library a default symbol ?
    Symbol->setSymbol(DefaultSymbol, LibName, Item->text());
}

// ----------------------------------------------------
bool QucsLib::getSection(QString section, QString &list, QString &content)
{
  int Start, End;
  Start = list.find("<"+section+">");
  content = "";
  if(Start > 0) {
    Start += section.length()+2;
    End = list.find("</"+section+">", Start);
    if(End < 0) {
      QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
      return false;
    }
    content = list.mid(Start, End-Start);
    content.replace(QRegExp("\\n\\x20+"), "\n").remove(0, 1);
  }
  return true;
}
