/***************************************************************************
                                textdoc.h
                               -----------
Copyright (C) 2006 by Michael Margraf <michael.margraf@alumni.tu-berlin.de>
Copyright (C) 2014 by Guilherme Brondani Torri <guitorri@gmail.com>

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

#include <QPlainTextEdit>
#include <QFont>

#include "qucsdoc.h"

/*!
 * \file textdoc.h
 * \brief Definition of the TextDoc class.
 */

class SyntaxHighlighter;
class QString;

// device type flags
#define DEV_BJT      0x0001
#define DEV_MOS      0x0002
#define DEV_MASK_DEV 0x00FF
#define DEV_DIG      0x0100
#define DEV_ANA      0x0200
#define DEV_ALL      0x0300
#define DEV_MASK_TYP 0xFF00
#define DEV_DEF      0x0200 // default value

/*!
 * \brief The TextDoc class definition
 */
class TextDoc : public QPlainTextEdit, public QucsDoc {
  Q_OBJECT
public:
  TextDoc (QucsApp *, const QString&);
 ~TextDoc ();

  void  setName (const QString&);
  bool  load ();
  int   save ();
  virtual float zoomBy (float zoom) override;
  virtual void showNoZoom () override;
  void  becomeCurrent (bool);
  bool  loadSimulationTime (QString&);
  void  commentSelected ();
  void  insertSkeleton ();
  void  setLanguage (int);
  void  setLanguage (const QString&);
  QString getModuleName (void);

  virtual void wheelEvent(QWheelEvent* event) override;

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
  void refreshLanguage(void);

  QMenu* createStandardContextMenu();

signals:
  void signalCursorPosChanged(int, int, QString);
  void signalFileChanged(bool);
  void signalUndoState(bool);
  void signalRedoState(bool);

public slots:
  void search(const QString &str, bool CaseSensitive, bool wordOnly, bool backward);
  void replace(const QString &str, const QString &str2, bool needConfirmed,
               bool CaseSensitive, bool wordOnly, bool backward);
  void slotCursorPosChanged ();
  void slotSetChanged ();

private:
  SyntaxHighlighter * syntaxHighlight;

private slots:
  void highlightCurrentLine();
  bool baseSearch(const QString &, bool, bool, bool);
};

#endif
