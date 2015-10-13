/***************************************************************************
                           ngspicesimdialog.h
                             ----------------
    begin                : Sun Nov 9 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EXTERNSIMDIALOG_H
#define EXTERNSIMDIALOG_H

#include <QtGui>
#include <QtCore>
#include "schematic.h"
#include "ngspice.h"
#include "xyce.h"

class ExternSimDialog : public QDialog
{
    Q_OBJECT
private:

    Schematic *Sch;

    QLabel *lblSimulator;
    QComboBox *cbxSimualor;
    QPushButton *buttonSimulate;
    QPushButton *buttonStopSim;
    QPushButton *buttonSimSettings;
    QPushButton *buttonSaveNetlist;
    QPushButton *buttonExit;

    QPlainTextEdit *editSimConsole;

    QProgressBar *simProgress;

    QString workdir;

    Ngspice *ngspice;
    Xyce *xyce;

    enum Simulator {simNgspice = 0, simXyceSer = 1, simXycePar = 2, simSpiceOpus = 3};

public:
    explicit ExternSimDialog(Schematic *sch,QWidget *parent = 0);
    ~ExternSimDialog();

    bool wasSimulated;

private:
    void saveLog();
    
signals:
    void simulated();
    void warnings();
    void success();
    
public slots:

private slots:
    void slotProcessOutput();
    //void slotProcessXyceOutput();
    void slotNgspiceStarted();
    void slotNgspiceStartError(QProcess::ProcessError err);
    void slotStart();
    void slotStop();
    void slotSetSimulator();
    void slotSimSettings();
    void slotSaveNetlist();
    
};

#endif // EXTERNSIMDIALOG_H
