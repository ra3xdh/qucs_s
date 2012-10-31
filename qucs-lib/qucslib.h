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

#include <qdialog.h>
#include <qdir.h>
#include <qfont.h>
#include <qstring.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QCloseEvent>

class SymbolWidget;
class Q3TextEdit;
class QComboBox;
class Q3ListBox;
class Q3VGroupBox;
class Q3VBoxLayout;
class Q3ListBoxItem;


// Application settings.
struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;          // font
  QString BitmapDir;   // pixmap directory
  QString LangDir;     // translation directory
  QString LibDir;      // library directory
  QString Language;
};

extern tQucsSettings QucsSettings;
extern QDir UserLibDir;

class QucsLib : public QDialog  {
   Q_OBJECT
public:
  QucsLib();
 ~QucsLib();

  Q3ListBox     *CompList;
  QStringList   LibraryComps;
  QComboBox    *Library;
  QString       DefaultSymbol;

private slots:
  void slotAbout();
  void slotQuit();
  void slotHelp();
  void slotCopyToClipBoard();
  void slotShowModel();
  void slotSelectLibrary(int);
  void slotSearchComponent();
  void slotShowComponent(Q3ListBoxItem*);
  void slotManageLib();

private:
  void closeEvent(QCloseEvent*);
  void putLibrariesIntoCombobox();
  bool getSection(QString, QString &, QString &);

  int UserLibCount;
  SymbolWidget *Symbol;
  Q3TextEdit    *CompDescr;
  Q3VBoxLayout  *all;

  QDir QucsHomeDir;  // Qucs user directory where all projects are located
};

#endif /* QUCSLIB_H */
