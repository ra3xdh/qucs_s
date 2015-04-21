/***************************************************************************
                           abstractspicekernel.h
                             ----------------
    begin                : Sat Jan 10 2014
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



#ifndef ABSTRACTSPICEKERNEL_H
#define ABSTRACTSPICEKERNEL_H

#include <QtCore>
#include "schematic.h"

class AbstractSpiceKernel : public QObject
{
    Q_OBJECT
private:
    void normalizeVarsNames(QStringList &var_list);

protected:
    QString netlist,workdir, simulator_cmd,
            simulator_parameters, output;
    QProcess *SimProcess;

    QStringList sims,vars,output_files;


    Schematic *Sch;

    bool prepareSpiceNetlist(QTextStream &stream, bool xyce = false);
    virtual void startNetlist(QTextStream& stream, bool xyce = false);
    virtual void createNetlist(QTextStream& stream, int NumPorts,QStringList& simulations,
                               QStringList& vars, QStringList &outputs);

public:

    explicit AbstractSpiceKernel(Schematic *sch_, QObject *parent = 0);
    ~AbstractSpiceKernel();

    virtual void createSubNetlsit(QTextStream& stream, bool xyce = false);

    void parseNgSpiceSimOutput(QString ngspice_file,
                          QList< QList<double> > &sim_points,
                          QStringList &var_list, bool &isComplex);
    void parseHBOutput(QString ngspice_file, QList< QList<double> > &sim_points,
                       QStringList &var_list);
    void parseSTEPOutput(QString ngspice_file,
                         QList< QList<double> > &sim_points,
                         QStringList &var_list, bool &isComplex);
    void parseHBSTEPOutput(QString ngspice_file,
                           QList< QList<double> > &sim_points,
                           QStringList &var_list, bool &isComplex);
    void parseResFile(QString resfile, QString &var, QStringList &values);
    void convertToQucsData(const QString &qucs_dataset, bool xyce = false);
    QString getOutput();

    void setSimulatorCmd(QString cmd);
    
signals:
    void started();
    void finished();
    void errors(QProcess::ProcessError);

protected slots:
    virtual void slotFinished();

public slots:
    virtual void slotSimulate();
    void killThemAll();
    void slotErrors(QProcess::ProcessError err);
    
};

#endif // ABSTRACTSPICEKERNEL_H
