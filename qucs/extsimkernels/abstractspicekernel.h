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

#include <QList>
#include <QString>
#include <QStringList>
#include <QDataStream>
#include <QTextStream>
#include <QProcess>

#include "schematic.h"

/*!
  \file abstractspicekernel.h
  \brief Implementation of the AbstractSpiceKernel class
*/

/*!
 * \brief AbstractSpiceKernel class contains common methods for
 *        Ngspice and Xyce simulation kernels. Contains spice netlist builder
 *        and responsible for simulator execution. Ngspice and Xyce classes
 *        inherit this class.
 */
class AbstractSpiceKernel : public QObject
{
    Q_OBJECT
private:
    enum outType {xyceSTD, spiceRaw, spiceRawSwp, xyceSTDswp, Unknown};

    void normalizeVarsNames(QStringList &var_list);
    int checkRawOutupt(QString ngspice_file, QStringList &values);
    void extractBinSamples(QDataStream &dbl, QList< QList<double> > &sim_points,
                           int NumPoints, int NumVars, bool isComplex);
    bool extractASCIISamples(QString &lin, QTextStream &ngsp_data, QList< QList<double> > &sim_points,
                             int NumVars, bool isComplex);

protected:
    QString netlist,workdir, simulator_cmd,
            simulator_parameters, output;
    QProcess *SimProcess;

    QStringList sims,vars,output_files;

    bool DC_OP_only; // only calculate operating point to show DC bias
    Schematic *Sch;

    bool prepareSpiceNetlist(QTextStream &stream, bool isSubckt = false);
    virtual void startNetlist(QTextStream& stream, bool xyce = false);
    virtual void createNetlist(QTextStream& stream, int NumPorts,QStringList& simulations,
                               QStringList& vars, QStringList &outputs);
    void removeAllSimulatorOutputs();
    bool checkGround();
    bool checkSimulations();
    bool checkDCSimulation();

public:

    explicit AbstractSpiceKernel(Schematic *sch_, QObject *parent = 0);
    ~AbstractSpiceKernel();

    bool checkSchematic(QStringList &incompat);
    virtual void createSubNetlsit(QTextStream& stream, bool lib = false);

    void parseNgSpiceSimOutput(QString ngspice_file,
                          QList< QList<double> > &sim_points,
                          QStringList &var_list, bool &isComplex);
    void parseHBOutput(QString ngspice_file, QList< QList<double> > &sim_points,
                       QStringList &var_list, bool &hasParSweep);
    void parseFourierOutput(QString ngspice_file, QList< QList<double> > &sim_points,
                            QStringList &var_list);
    void parseNoiseOutput(QString ngspice_file, QList< QList<double> > &sim_points,
                          QStringList &var_list, bool &ParSwp);
    void parsePZOutput(QString ngspice_file, QList< QList<double> > &sim_points,
                       QStringList &var_list, bool &ParSwp);
    void parseSENSOutput(QString ngspice_file, QList< QList<double> > &sim_points,
                         QStringList &var_list);
    void parseDC_OPoutput(QString ngspice_file);
    void parseDC_OPoutputXY(QString xyce_file);
    void parseSTEPOutput(QString ngspice_file,
                         QList< QList<double> > &sim_points,
                         QStringList &var_list, bool &isComplex);
    void parseXYCESTDOutput(QString std_file,
                            QList< QList<double> > &sim_points,
                            QStringList &var_list, bool &isComplex);
    void parseXYCENoiseLog(QString logfile, QList< QList<double> > &sim_points,
                           QStringList &var_list);
    void parseResFile(QString resfile, QString &var, QStringList &values);
    void convertToQucsData(const QString &qucs_dataset);
    QString getOutput();

    virtual void setSimulatorCmd(QString cmd);
    virtual void setSimulatorParameters(QString parameters);
    void setWorkdir(QString path);
    virtual void SaveNetlist(QString filename);
    virtual bool waitEndOfSimulation();
    
signals:
    void started();
    void finished();
    void errors(QProcess::ProcessError);
    void progress(int);

protected slots:
    virtual void slotFinished();
    virtual void slotProcessOutput();

public slots:
    virtual void slotSimulate();
    void killThemAll();
    void slotErrors(QProcess::ProcessError err);
    
};

#endif // ABSTRACTSPICEKERNEL_H
