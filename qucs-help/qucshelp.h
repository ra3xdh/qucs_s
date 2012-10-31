/***************************************************************************
                                qucshelp.h
                               ------------
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

#ifndef QUCSHELP_H
#define QUCSHELP_H

#include <q3mainwindow.h>
#include <q3textbrowser.h>
#include <qdir.h>
#include <qfont.h>
#include <qstringlist.h>

struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  QString BitmapDir;
  QString LangDir;
  QString DocDir;
  QString Language;
};

extern tQucsSettings QucsSettings;
extern QDir QucsHelpDir;
class QAction;
class Q3ListViewItem;
class Q3ListView;
class HtmlDataFetcher;
class Q3DockWindow;

class TextBrowser : public Q3TextBrowser
{
   Q_OBJECT
   public:
      TextBrowser(QWidget *parent = 0) : Q3TextBrowser(parent)
      {}
   public slots:
      void setSource(const QString& name)
      {
         // Dont do anything if the clicked link is web url
         if(!name.startsWith("http://"))
            Q3TextBrowser::setSource(name);
      }
};

class QucsHelp : public Q3MainWindow  {
  Q_OBJECT
  public:
    QucsHelp(const QString& page);
    ~QucsHelp();

  private slots:
    void slotSourceChanged(const QString& str);
    void slotToggleSidebar(bool);
    void slotToggleSidebarAction(bool);
    void previousLink();
    void nextLink();
    void displaySelectedChapter();

  private:
    void setupActions();
    void createSidebar();

    TextBrowser *textBrowser;
    uint currentIndex;
    QStringList links;
    QAction *previousAction;
    QAction *nextAction;
    QAction *viewBrowseDock;
    Q3ListView *chaptersView;
    Q3DockWindow *dock;
    HtmlDataFetcher *dataFetcher;
    QString currentSource;
    QString cachedSelectedText;
};

#endif
