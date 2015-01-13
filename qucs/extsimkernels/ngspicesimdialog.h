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

#ifndef NGSPICESIMDIALOG_H
#define NGSPICESIMDIALOG_H

#include <QtGui>
#include <QtCore>
#include "schematic.h"
#include "ngspice.h"

class ExternSimDialog : public QDialog
{
    Q_OBJECT
private:

    Schematic *Sch;

    QPushButton *buttonSimulate;
    QPushButton *buttonStopSim;

    QTextEdit *editSimConsole;

    QString workdir;

    Ngspice *ngspice;

public:
    explicit ExternSimDialog(Schematic *sch,QWidget *parent = 0);
    ~ExternSimDialog();
    
signals:
    
public slots:

private slots:
    void slotProcessNgSpiceOutput();
    void slotNgspiceStarted();
    void slotNgspiceStartError();
    void slotStart();
    void slotStop();
    
};

#endif // NGSPICESIMDIALOG_H
