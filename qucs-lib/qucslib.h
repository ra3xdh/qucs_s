/***************************************************************************
                               qucslib.h
                              -----------
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

#ifndef QUCSLIB_H
#define QUCSLIB_H

#include <QMainWindow>
#include <QDir>
#include <QFont>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QComboBox>

#include "symbolwidget.h"


// Application settings.
struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;          // font
  QString BitmapDir;   // pixmap directory
  QString LangDir;     // translation directory
  QString LibDir;      // library directory
  QString Language;
  QDir QucsWorkDir;  // Qucs user directory where user works (usually same as QucsWorkDir
  QDir QucsHomeDir;  // Qucs user directory where all projects are located
};

extern tQucsSettings QucsSettings;
extern QDir UserLibDir;

class QucsLib : public QMainWindow  {
   Q_OBJECT
public:
  QucsLib();
  ~QucsLib();

  QListWidget *CompList;
  QStringList   LibraryComps;
  QComboBox    *Library;
  QString DefaultSymbol;

private slots:
  void slotAbout();
  void slotQuit();
  void slotHelp();
  void slotCopyToClipBoard();
  void slotShowModel();
  void slotSelectLibrary(int);
  void slotSearchComponent();
  void slotShowComponent(QListWidgetItem*);
  void slotManageLib();

private:
  void closeEvent(QCloseEvent*);
  void putLibrariesIntoCombobox();
  bool getSection(QString, QString &, QString &);

  QMenu *fileMenu, *helpMenu;

  int UserLibCount;
  SymbolWidget *Symbol;
  QTextEdit    *CompDescr;
  QVBoxLayout  *all;

};

#endif /* QUCSLIB_H */
