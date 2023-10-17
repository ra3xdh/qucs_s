/***************************************************************************
                                 qucsdoc.h
                                -----------
    begin                : Wed Sep 3 2003
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

#ifndef QUCSDOC_H
#define QUCSDOC_H

#include <QString>
#include <QDateTime>

class QucsApp;
class QPrinter;
class QPainter;

class QucsDoc {
public: 
  QucsDoc(QucsApp*, const QString&);
  virtual ~QucsDoc() {};

  virtual void  setName(const QString&) {};
  virtual bool  load() { return true; };
  virtual int   save() { return 0; };
  virtual void  print(QPrinter*, QPainter*, bool, bool) {};
  virtual void  becomeCurrent(bool) {};
  virtual float zoomBy(float) { return 1.0; };
  virtual void  showAll() {};
  virtual void  zoomToSelection() {};
  virtual void  showNoZoom() {};

  static QString fileSuffix (const QString&);
  QString fileSuffix (void);
  static QString fileBase (const QString&);
  QString fileBase (void);

  QString DocName;
  QString DataSet;     // name of the default dataset
  QString DataDisplay; // name of the default data display
  QString Script;
  QString SimTime;     // used for VHDL simulation, but stored in datadisplay
  QDateTime lastSaved;

  float Scale;
  QucsApp *App;
  bool DocChanged;
  bool SimOpenDpl;   // open data display after simulation ?
  bool SimRunScript; // run script after simulation ?
  int  showBias;     // -1=no, 0=calculation running, >0=show DC bias points
  bool GridOn;
  int  tmpPosX, tmpPosY;
};

#endif
