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

#include <QMainWindow>
#include <QTextBrowser>
#include <QDir>
#include <QFont>
#include <QStringList>
#include <QTextBrowser>
#include <QUrl>
#include <QDebug>

struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  QString LangDir;
  QString DocDir;
  QString Language;
};

extern tQucsSettings QucsSettings;
extern QDir QucsHelpDir;
class QAction;
class QListWidget;
class HtmlDataFetcher;
class QDockWidget;

class TextBrowser : public QTextBrowser
{
   Q_OBJECT
   public:
      TextBrowser(QWidget *parent = 0) : QTextBrowser(parent)
      {}
};

class QucsHelp : public QMainWindow  {
  Q_OBJECT
  public:
    QucsHelp(const QString& page);
    ~QucsHelp();

  private slots:
    void slotSourceChanged(const QUrl &str);
    void slotToggleSidebar(bool);
    void slotToggleSidebarAction(bool);
    void previousLink();
    void nextLink();
    void displaySelectedChapter();
    void gohome();

  private:
    void setupActions();
    void createSidebar();

    TextBrowser *textBrowser;
    uint currentIndex;
    QStringList links;
    QAction *previousAction;
    QAction *nextAction;
    QAction *viewBrowseDock;
    QListWidget *chaptersView;
    QDockWidget *dock;
    HtmlDataFetcher *dataFetcher;
    QString currentSource;
    QString cachedSelectedText;
};

#endif
