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

  float getScale() const { return a_Scale; }
  bool getDocChanged() const { return a_DocChanged; }
  void setDocChanged(bool value) { a_DocChanged = value; }
  bool getSimOpenDpl() const { return a_SimOpenDpl; }
  void setSimOpenDpl(bool value) { a_SimOpenDpl = value; }
  bool getSimRunScript() const { return a_SimRunScript; }
  void setSimRunScript(bool value) { a_SimRunScript = value; }
  bool getGridOn() const { return a_GridOn; }
  void setGridOn(bool value) { a_GridOn = value; }
  QucsApp* getApp() const { return a_App; }
  int getShowBias() const { return a_showBias; }
  void setShowBias(int value) { a_showBias = value; }
  QString getDocName() const { return a_DocName; }
  void setDocName(const QString& value) { a_DocName = value; }
  QString getDataSet() const { return a_DataSet; }
  void setDataSet(const QString& value) { a_DataSet = value; }
  QString getDataDisplay() const { return a_DataDisplay; }
  void setDataDisplay(const QString& value) { a_DataDisplay = value; }
  QString getScript() const { return a_Script; }
  void setScript(const QString& value) { a_Script = value; }
  QString getSimTime() const { return a_SimTime; }
  void setSimTime(const QString& value) { a_SimTime = value; }
  QDateTime getLastSaved() const { return a_lastSaved; }
  void setLastSaved(const QDateTime& value) { a_lastSaved = value; }

protected:
  QString a_DocName;
  QString a_DataSet;     // name of the default dataset
  QString a_DataDisplay; // name of the default data display
  QString a_Script;
  QString a_SimTime;     // used for VHDL simulation, but stored in datadisplay
  QDateTime a_lastSaved;

  float a_Scale;
  QucsApp* a_App;
  bool a_DocChanged;
  bool a_SimOpenDpl;   // open data display after simulation ?
  bool a_SimRunScript; // run script after simulation ?
  int a_showBias;     // -1=no, 0=calculation running, >0=show DC bias points
  bool a_GridOn;
  int a_tmpPosX;
  int a_tmpPosY;
};

#endif
