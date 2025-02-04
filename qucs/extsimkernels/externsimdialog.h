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

#include "schematic.h"
#include "ngspice.h"
#include "xyce.h"
#include "spicecompat.h"

class ExternSimDialog : public QDialog
{
    Q_OBJECT

private:
    Schematic* a_schematic;

    QPushButton *a_buttonStopSim;
    QPushButton *a_buttonSaveNetlist;
    QPushButton *a_buttonExit;

    QPlainTextEdit *a_editSimConsole;
    QListWidget *a_simStatusLog;

    QProgressBar *a_simProgress;

    Ngspice *a_ngspice;
    Xyce *a_xyce;

    bool a_wasSimulated;
    bool a_hasError;
    bool a_netlist2Console;

public:
    explicit ExternSimDialog(
            Schematic* sch,
            bool netlist2Console,
            bool netlist_mode = false);
    ~ExternSimDialog();

    bool wasSimulated() const { return a_wasSimulated; }
    bool hasError() const { return a_hasError; }

private:
    void saveLog();
    void addLogEntry(const QString&text, const QIcon &icon);
    bool logContainsError(const QString &out);
    bool logContainsWarning(const QString &out);

signals:
    void simulated(ExternSimDialog *);
    void warnings();
    void success();

public slots:
    void slotSaveNetlist();
    void slotStart();

private slots:
    void slotProcessOutput();
    //void slotProcessXyceOutput();
    void slotNgspiceStarted();
    void slotNgspiceStartError(QProcess::ProcessError err);
    void slotStop();
    void slotSetSimulator();
    void slotExit();
};

#endif // EXTERNSIMDIALOG_H
