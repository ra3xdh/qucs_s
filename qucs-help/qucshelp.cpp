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

#include <QPushButton>
#include <QAction>
#include <QPixmap>
#include <QFile>
#include <QMenuBar>
#include <QApplication>
#include <QListWidget>
#include <QToolBar>
#include <QDockWidget>
#include <QDebug>
#include <QToolBar>
#include <QDir>



QucsHelp::QucsHelp(const QString& page)
{
  currentIndex = 0;
  dataFetcher = new HtmlDataFetcher();
  links = dataFetcher->fetchLinksToFiles(QucsHelpDir.filePath("index.html"));
  // set application icon
  // APPLE sets the QApplication icon with Info.plist
#ifndef __APPLE__
  setWindowIcon (QPixmap(":/bitmaps/big.qucs.xpm"));
#endif
  setWindowTitle(tr("Qucs Help System"));

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
  QToolBar *toolbar = new QToolBar("main_toolbar",this);

  this->addToolBar(toolbar);

  QMenuBar *bar = menuBar();

  const QKeySequence ks = QKeySequence();

  QAction *quitAction = new QAction(QIcon((":/bitmaps/quit.png")),
                                    tr("&Quit"), this);
    quitAction->setShortcut((const QKeySequence&)Qt::CTRL+Qt::Key_Q);
    
  QAction *backAction = new QAction(QIcon((":/bitmaps/back.png")),
                                    tr("&Back"), this);
    backAction->setShortcut( Qt::ALT+Qt::Key_Left);
 
    
  QAction *forwardAction = new QAction(QIcon((":/bitmaps/forward.png")),
                                       tr("&Forward"),  this);
   forwardAction->setShortcut(Qt::ALT+Qt::Key_Right);
    
  QAction *homeAction = new QAction(QIcon((":/bitmaps/home.png")),
                                    tr("&Home"),this);
   homeAction->setShortcut(Qt::CTRL+Qt::Key_H);
    
  previousAction = new QAction(QIcon((":/bitmaps/previous.png")),tr("&Previous"),
                               this);
   previousAction->setShortcut( ks);
    
  nextAction = new QAction(QIcon((":/bitmaps/next.png")),
                           tr("&Next"), this);
   nextAction->setShortcut( ks);
    
  viewBrowseDock = new QAction(tr("&Table of Contents"), this);
    
  viewBrowseDock->setCheckable(true);
  viewBrowseDock->setChecked(true);
  viewBrowseDock->setStatusTip(tr("Enables/disables the table of contents"));
  viewBrowseDock->setWhatsThis(tr("Table of Contents\n\nEnables/disables the table of contents"));

  connect(quitAction,SIGNAL(activated()),qApp,SLOT(quit()));

  connect(backAction,SIGNAL(activated()),textBrowser,SLOT(backward()));
  connect(textBrowser,SIGNAL(backwardAvailable(bool)),backAction,SLOT(setEnabled(bool)));

  connect(forwardAction,SIGNAL(activated()),textBrowser,SLOT(forward()));
  connect(textBrowser,SIGNAL(forwardAvailable(bool)),forwardAction,SLOT(setEnabled(bool)));

  connect(homeAction,SIGNAL(activated()),textBrowser,SLOT(home()));
  connect(homeAction,SIGNAL(activated()),this,SLOT(gohome()));

  connect(textBrowser,SIGNAL(sourceChanged(const QUrl &)),this,SLOT(slotSourceChanged(const QUrl &)));
  connect(previousAction,SIGNAL(activated()),this,SLOT(previousLink()));
  connect(nextAction,SIGNAL(activated()),this,SLOT(nextLink()));
  connect(viewBrowseDock, SIGNAL(toggled(bool)), SLOT(slotToggleSidebar(bool)));

  //backAction->addTo(toolbar);
    toolbar->addAction(backAction);
  //forwardAction->addTo(toolbar);
    toolbar->addAction(forwardAction);
  toolbar->addSeparator();
  //homeAction->addTo(toolbar);
    toolbar->addAction(homeAction);
  //previousAction->addTo(toolbar);
    toolbar->addAction(previousAction);
  //nextAction->addTo(toolbar);
    toolbar->addAction(nextAction);
  toolbar->addSeparator();
  //quitAction->addTo(toolbar);
    toolbar->addAction(quitAction);

  QMenu *fileMenu = new QMenu(this);
  //quitAction->addTo(fileMenu);
    fileMenu->addAction(quitAction);

  QMenu *viewMenu = new QMenu(this);
  //backAction->addTo(viewMenu);
    viewMenu->addAction(backAction);
  //forwardAction->addTo(viewMenu);
    viewMenu->addAction(forwardAction);

  //homeAction->addTo(viewMenu);
    viewMenu->addAction(homeAction);

  //previousAction->addTo(viewMenu);
    viewMenu->addAction(previousAction);

  //nextAction->addTo(viewMenu);
    viewMenu->addAction(nextAction);

  //viewMenu->insertSeparator();
    viewMenu->addSeparator();

  //viewBrowseDock->addTo(viewMenu);
    

  QMenu *helpMenu = new QMenu(this);
  helpMenu->addAction(tr("&About Qt"),qApp,SLOT(aboutQt()));

  bar->addMenu(new QMenu(tr("&File"), fileMenu));
  bar->addMenu(new QMenu(tr("&View"),viewMenu));
  bar->addSeparator();
  bar->addMenu(new QMenu(tr("&Help"),helpMenu));
}

void QucsHelp::createSidebar()
{
  dock = new QDockWidget(tr("Contents"),this);
  connect(dock,SIGNAL(visibilityChanged(bool)),this,SLOT(slotToggleSidebarAction(bool)));

  chaptersView = new QListWidget(dock);
  dock->setWidget(chaptersView);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->addDockWidget(Qt::LeftDockWidgetArea, dock);

  QStringList l = dataFetcher->fetchChapterTexts(QucsHelpDir.filePath("index.html"));
  for(int i=0; i < (l.count()-1); i++) {
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(l[i]);
    chaptersView->addItem(newItem);
  }

  QListWidgetItem *curItem = new QListWidgetItem(tr("Home"));
  chaptersView->insertItem(0,curItem);
  chaptersView->setCurrentItem(curItem);

  connect(chaptersView,SIGNAL(itemSelectionChanged()),this,SLOT(displaySelectedChapter()));
}

void QucsHelp::displaySelectedChapter()
{
  if(chaptersView->selectedItems().size() == 0)
      return;
  int y = chaptersView->currentRow();
  Q_ASSERT(y < links.count());
  textBrowser->setSource(QucsHelpDir.filePath(links[y]));
  cachedSelectedText = chaptersView->currentItem()->text();
}

void QucsHelp::gohome()
{
    chaptersView->setCurrentRow(0);
}

//This slot updates next and previous actions i.e enabling/disabling
void QucsHelp::slotSourceChanged(const QUrl & _str)
{
  QString str = _str.toString();
  // Remove '#*' chars in link since we don't check '#top,etc' while tracking previous actions
  int hashPos = str.lastIndexOf('#');
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
      if(chaptersView->selectedItems().size())
         temp = chaptersView->currentItem()->text();
      if(temp.toUInt() != i)
      {
        QListWidgetItem *item = chaptersView->item(i);
        if(item != 0l)
        {
          chaptersView->blockSignals(true);
          chaptersView->setCurrentItem(item);
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
    qDebug("QucsHelp::slotSourceChanged():  Link mismatch \n Link: %s",str.toAscii().data());
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
  viewBrowseDock->setChecked(b);
  viewBrowseDock->blockSignals(false);
}
