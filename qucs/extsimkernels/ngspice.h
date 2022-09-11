/***************************************************************************
                           mgspice.h
                             ----------------
    begin                : Sat Jan 10 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
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


#ifndef NGSPICE_H
#define NGSPICE_H

#include <QString>
#include <QStringList>
#include <QDataStream>
#include "schematic.h"
#include "abstractspicekernel.h"

/*!
  \file ngspice.h
  \brief Declaration of the Ngspice class
*/

/*!
 * \brief The Ngspice class Responsible for Ngspice simulator execution.
 */
class Ngspice : public AbstractSpiceKernel
{
    Q_OBJECT
private:

    bool checkNodeNames(QStringList &incompat);
    static QString collectSpiceinit(Schematic *sch);
    bool findMathFuncInc(QString &mathf_inc);
    QString getParentSWPscript(Component *pc_swp, QString sim, bool before, bool &hasDblSWP);
    QString getParentSWPCntVar(Component *pc_swp, QString sim);

public:
    explicit Ngspice(Schematic *sch_, QObject *parent = 0);
    void SaveNetlist(QString filename);
    void setSimulatorCmd(QString cmd);
    void setSimulatorParameters(QString parameters);
    
protected:
    void createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                       QStringList &vars, QStringList &outputs);

public slots:
    void slotSimulate();

protected slots:
    void slotProcessOutput();
};

#endif // NGSPICE_H
