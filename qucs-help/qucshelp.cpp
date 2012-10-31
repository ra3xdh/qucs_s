/***************************************************************************
                          qucshelp.cpp  -  description
                             -------------------
    begin                : Sun Jan 11 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "qucshelp.h"
#include "htmldatafetcher.h"

#include <qpushbutton.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qfile.h>
#include <q3textstream.h>
#include <q3popupmenu.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <q3listview.h>



QucsHelp::QucsHelp(const QString& page)
{
  currentIndex = 0;
  dataFetcher = new HtmlDataFetcher();
  links = dataFetcher->fetchLinksToFiles(QucsHelpDir.filePath("index.html"));
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption(tr("Qucs Help System"));

  textBrowser = new TextBrowser(this);
  textBrowser->setMinimumSize(400,200);
  setCentralWidget(textBrowser);
  setupActions();
  createSidebar();

  textBrowser->setSource(QucsHelpDir.filePath(links[0]));

  // .......................................
  if(!page.isEmpty())
    textBrowser->setSource(QucsHelpDir.filePath(page));
}

QucsHelp::~QucsHelp()
{}

void QucsHelp::setupActions()
{
  Q3ToolBar *toolbar = new Q3ToolBar(this,"main_toolbar");
  QMenuBar *bar = menuBar();
  statusBar();

  const QKeySequence ks = QKeySequence();

  QAction *quitAction = new QAction(QIcon((QucsSettings.BitmapDir + "quit.png")),
                                    tr("&Quit"), (const QKeySequence&)Qt::CTRL+Qt::Key_Q, this,"");
  QAction *backAction = new QAction(QIcon((QucsSettings.BitmapDir + "back.png")),
                                    tr("&Back"), Qt::ALT+Qt::Key_Left, this,"");
  QAction *forwardAction = new QAction(QIcon((QucsSettings.BitmapDir + "forward.png")),
                                       tr("&Forward"), Qt::ALT+Qt::Key_Right, this,"");
  QAction *homeAction = new QAction(QIcon((QucsSettings.BitmapDir + "home.png")),
                                    tr("&Home"),Qt::CTRL+Qt::Key_H,this,"");
  previousAction = new QAction(QIcon((QucsSettings.BitmapDir + "previous.png")),tr("&Previous"),
                               ks, this,"");
  nextAction = new QAction(QIcon((QucsSettings.BitmapDir + "next.png")),
                           tr("&Next"), ks, this,"");
  viewBrowseDock = new QAction(tr("&Table of Contents"), 0, this,"");
  viewBrowseDock->setToggleAction(true);
  viewBrowseDock->setOn(true);
  viewBrowseDock->setStatusTip(tr("Enables/disables the table of contents"));
  viewBrowseDock->setWhatsThis(tr("Table of Contents\n\nEnables/disables the table of contents"));

  connect(quitAction,SIGNAL(activated()),qApp,SLOT(quit()));

  connect(backAction,SIGNAL(activated()),textBrowser,SLOT(backward()));
  connect(textBrowser,SIGNAL(backwardAvailable(bool)),backAction,SLOT(setEnabled(bool)));

  connect(forwardAction,SIGNAL(activated()),textBrowser,SLOT(forward()));
  connect(textBrowser,SIGNAL(forwardAvailable(bool)),forwardAction,SLOT(setEnabled(bool)));

  connect(homeAction,SIGNAL(activated()),textBrowser,SLOT(home()));

  connect(textBrowser,SIGNAL(sourceChanged(const QString &)),this,SLOT(slotSourceChanged(const QString&)));
  connect(previousAction,SIGNAL(activated()),this,SLOT(previousLink()));
  connect(nextAction,SIGNAL(activated()),this,SLOT(nextLink()));
  connect(viewBrowseDock, SIGNAL(toggled(bool)), SLOT(slotToggleSidebar(bool)));

  backAction->addTo(toolbar);
  forwardAction->addTo(toolbar);
  toolbar->addSeparator();
  homeAction->addTo(toolbar);
  previousAction->addTo(toolbar);
  nextAction->addTo(toolbar);
  toolbar->addSeparator();
  quitAction->addTo(toolbar);

  Q3PopupMenu *fileMenu = new Q3PopupMenu(this);
  quitAction->addTo(fileMenu);

  Q3PopupMenu *viewMenu = new Q3PopupMenu(this);
  backAction->addTo(viewMenu);
  forwardAction->addTo(viewMenu);
  homeAction->addTo(viewMenu);
  previousAction->addTo(viewMenu);
  nextAction->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewBrowseDock->addTo(viewMenu);

  Q3PopupMenu *helpMenu = new Q3PopupMenu(this);
  helpMenu->insertItem(tr("&About Qt"),qApp,SLOT(aboutQt()));

  bar->insertItem(tr("&File"), fileMenu );
  bar->insertItem(tr("&View"),viewMenu);
  bar->insertSeparator();
  bar->insertItem(tr("&Help"),helpMenu);

}


void QucsHelp::createSidebar()
{
  dock = new Q3DockWindow(Q3DockWindow::InDock,this);
  dock->setResizeEnabled(true);
  dock->setCloseMode(Q3DockWindow::Always);
  connect(dock,SIGNAL(visibilityChanged(bool)),this,SLOT(slotToggleSidebarAction(bool)));

  chaptersView = new Q3ListView(dock,"chapters_view");
  chaptersView->setRootIsDecorated(false);
  chaptersView->addColumn(tr("Contents"));
  chaptersView->setSorting(-1);
  chaptersView->setSelectionMode(Q3ListView::Single);

  dock->setWidget(chaptersView);
  moveDockWindow(dock,Qt::Left);


  QStringList l = dataFetcher->fetchChapterTexts(QucsHelpDir.filePath("index.html"));
  for(int i=l.count()-1;i>=0;i--)
    chaptersView->insertItem(new Q3ListViewItem(chaptersView,l[i],QString::number(i+1)));

  Q3ListViewItem *curItem = new Q3ListViewItem(chaptersView,tr("Home"),QString::number(0));
  chaptersView->insertItem(curItem);
  chaptersView->setSelected(curItem,true);

  connect(chaptersView,SIGNAL(selectionChanged()),this,SLOT(displaySelectedChapter()));
}

void QucsHelp::displaySelectedChapter()
{
  if(chaptersView->selectedItem() == 0)
      return;
  uint y = chaptersView->selectedItem()->text(1).toUInt();
  Q_ASSERT(y < links.count());
  textBrowser->setSource(QucsHelpDir.filePath(links[y]));
  cachedSelectedText = chaptersView->selectedItem()->text(1);
}
//This slot updates next and previous actions i.e enabling/disabling
void QucsHelp::slotSourceChanged(const QString& _str)
{
  QString str(_str);
  // Remove '#*' chars in link since we don't check '#top,etc' while tracking previous actions
  int hashPos = str.findRev('#');
  if(hashPos != -1)
    str = str.left(hashPos);
  // Don't do anything if accessing same page
  if(str == currentSource)
    return;
    
  bool found = false;
  for(unsigned int i=0;i < links.count(); i++)
  {
    if(str.endsWith(links[i]))
    {
      currentIndex = i;
      previousAction->setEnabled(bool(i!=0));
      nextAction->setEnabled(bool(i+1 != links.count()));
      QString temp = cachedSelectedText;
      if(chaptersView->selectedItem())
         temp = chaptersView->selectedItem()->text(1);
      if(temp.toUInt() != i)
      {
        Q3ListViewItem *item = chaptersView->findItem(QString::number(i),1);
        if(item != 0l)
        {
          chaptersView->blockSignals(true);
          chaptersView->setSelected(item,true);
          chaptersView->blockSignals(false);
        }
      }
      found = true;
      break;
    }
  }
  if(found == false) // some error
  {
    textBrowser->setSource(QucsHelpDir.filePath(links[0]));
    currentSource = QucsHelpDir.filePath(links[0]);
    qDebug("QucsHelp::slotSourceChanged():  Link mismatch \n Link: %s",str.ascii());
  }
  else
    currentSource = str;
}


void QucsHelp::previousLink()
{
  if(currentIndex > 0)
    --currentIndex;
  textBrowser->setSource(QucsHelpDir.filePath(links[currentIndex]));
}

void QucsHelp::nextLink()
{
  ++currentIndex;
  if(currentIndex >= links.count())
    currentIndex = links.count();
  textBrowser->setSource(QucsHelpDir.filePath(links[currentIndex]));
}

void QucsHelp::slotToggleSidebar(bool b)
{
  dock->setShown(b);
}

void QucsHelp::slotToggleSidebarAction(bool b)
{
  viewBrowseDock->blockSignals(true);
  viewBrowseDock->setOn(b);
  viewBrowseDock->blockSignals(false);
}
