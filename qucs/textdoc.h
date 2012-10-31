/***************************************************************************
                                textdoc.h
                               -----------
    begin                : Sat Mar 11 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef TEXTDOC_H
#define TEXTDOC_H

#include <q3textedit.h>
#include <qfont.h>

#include "qucsdoc.h"
//Added by qt3to4:
#include <Q3PopupMenu>

class SyntaxHighlighter;

extern const char *smallsave_xpm[];// icon for unsaved files (diskette)
extern const char *empty_xpm[];    // provides same height than "smallsave_xpm"

// device type flags
#define DEV_BJT      0x0001
#define DEV_MOS      0x0002
#define DEV_MASK_DEV 0x00FF
#define DEV_DIG      0x0100
#define DEV_ANA      0x0200
#define DEV_ALL      0x0300
#define DEV_MASK_TYP 0xFF00
#define DEV_DEF      0x0200 // default value

class TextDoc : public Q3TextEdit, public QucsDoc {
  Q_OBJECT
public: 
  TextDoc (QucsApp *, const QString&);
 ~TextDoc ();

  void  setName (const QString&);
  bool  load ();
  int   save ();
  void  print (QPrinter*, QPainter*, bool, bool);
  float zoomBy (float);
  void  showAll ();
  void  showNoZoom ();
  void  becomeCurrent (bool);
  bool  loadSimulationTime (QString&);
  void  commentSelected ();
  void  insertSkeleton ();
  void  setLanguage (int);
  void  setLanguage (const QString&);
  QString getModuleName (void);

  QFont TextFont;

  bool simulation;   // simulation or module
  QString Library;   // library this document belongs to
  QString Libraries; // libraries to be linked with
  QString ShortDesc; // icon description
  QString LongDesc;  // component description
  QString Icon;      // icon file
  bool recreate;     // recreate output file
  int devtype;       // device type

  bool SetChanged;
  int language;

  bool loadSettings (void);
  bool saveSettings (void);

  Q3PopupMenu * createPopupMenu (const QPoint&);

public slots:
  void slotCursorPosChanged (int, int);
  void slotSetChanged ();

private:
  SyntaxHighlighter * syntaxHighlight;
};

#endif
