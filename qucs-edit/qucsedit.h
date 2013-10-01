/***************************************************************************
                                qucsedit.h
                               ------------
    begin                : Mon Nov 17 2003
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef QUCSEDIT_H
#define QUCSEDIT_H

#include <QMainWindow>
#include <QApplication>
#include <QCloseEvent>
#include <QLabel>

class QLabel;
class QTextEdit;


struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  QString LangDir;
  QString Language;
};

extern tQucsSettings QucsSettings;


class QucsEdit : public QMainWindow  {
   Q_OBJECT
public:
  QucsEdit(const QString&, bool readOnly=false);

private slots:
  void slotAbout();
  void slotLoad();
  void slotSave();
  void slotQuit();
  void slotPrintCursorPosition();

private:
  void closeEvent(QCloseEvent*);
  bool loadFile(const QString&);
  bool closeFile();

  QString FileName;
  QTextEdit *text;
  QLabel *PosText;
};

#endif
