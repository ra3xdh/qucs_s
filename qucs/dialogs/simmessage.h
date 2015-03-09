/***************************************************************************
                               simmessage.h
                              --------------
    begin                : Sat Sep 6 2003
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

#ifndef SIMMESSAGE_H
#define SIMMESSAGE_H

#include <QDialog>
#include <QProcess>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>

class QPlainTextEdit;
class QTextStream;
class QVBoxLayout;
class QPushButton;
class QProgressBar;
class QProcess;
class QFile;
class Component;
class Schematic;

// #define SPEEDUP_PROGRESSBAR


class SimMessage : public QDialog  {
Q_OBJECT
public:
  SimMessage(QWidget*, QWidget *parent=0);
 ~SimMessage();

  bool startProcess();

signals:
  void SimulationEnded(int, SimMessage*);
  void displayDataPage(QString&, QString&);

public slots:
  void slotClose();

private slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotCloseStdin();
  void slotSimEnded(int exitCode, QProcess::ExitStatus exitStatus);
  void slotDisplayButton();
  void AbortSim();

  void slotReadSpiceNetlist();
  void slotFinishSpiceNetlist(int status);

/* #ifdef SPEEDUP_PROGRESSBAR
  void slotUpdateProgressBar();
private:
  int  iProgress;
  bool waitForUpdate;
#endif
*/

private:
  void FinishSimulation(int);
  void nextSPICE();
  void startSimulator();
  Component * findOptimization(Schematic *);

public:
  QWidget *DocWidget;
  int showBias;
  bool SimOpenDpl;
  bool SimRunScript;
  QString DocName, DataSet, DataDisplay, Script;

  QProcess       SimProcess;
  QPlainTextEdit *ProgText, *ErrText;
  bool           wasLF;   // linefeed for "ProgText"
  bool           simKilled; // true if simulation was aborted by the user
  QPushButton    *Display, *Abort;
  QProgressBar   *SimProgress;
  QString        ProgressText;

  Component      *SimOpt;
  int            SimPorts;
  bool           makeSubcircuit, insertSim;
  QStringList    Collect;
  QFile          NetlistFile;
  QTextStream    Stream;

  QVBoxLayout    *all;
};

#endif
