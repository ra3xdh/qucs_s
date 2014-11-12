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
#include "schematic.h"

class NgspiceSimDialog : public QDialog
{
    Q_OBJECT
private:

    Schematic *Sch;

    QProcess *ngspice;

    QPushButton *buttonSimulate;
    QPushButton *buttonStopSim;

    QTextEdit *editSimConsole;

    QString workdir;
    QStringList sims, // Lists of simulations,
                vars, // output variables,
                output_files; // and ngspice output files


    void createSpiceNetlist(QTextStream& stream, int NumPorts,QStringList& simulations, QStringList& vars,
                                          QStringList &outputs);
    void startNgSpice(QString netlist);
    void convertToQucsData(const QString &qucs_dataset);
    void parseNgSpiceSimOutput(QString ngspice_file,QList< QList<double> > &sim_points, QStringList &var_list);

public:
    explicit NgspiceSimDialog(Schematic *sch,QWidget *parent = 0);
    ~NgspiceSimDialog();
    
signals:
    
public slots:

private slots:
    void slotSimulate();
    void killThemAll();
    void slotProcessNgSpiceOutput(int exitCode);
    
};

#endif // NGSPICESIMDIALOG_H
