/***************************************************************************
                               xyce.h
                             ----------------
    begin                : Fri Jan 16 2015
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

#ifndef XYCE_H
#define XYCE_H

#include <QtCore>
#include "abstractspicekernel.h"

/*!
  \file xyce.h
  \brief Declaration of the Xyce class
*/

/*!
 * \brief The Xyce class responsible for execution of Xyce simulator.
 */
class Xyce : public AbstractSpiceKernel
{
    Q_OBJECT
private:
    unsigned int Nprocs;
    QStringList simulationsQueue;
    QStringList netlistQueue;
    void determineUsedSimulations();
    void nextSimulation();

public:
    explicit Xyce(Schematic *sch_, QObject *parent = 0);

    void SaveNetlist(QString filename);
    void setParallel(bool par);
    bool waitEndOfSimulation();
    
protected:
    void createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                  QStringList &vars, QStringList &outputs);
protected slots:
    void slotFinished();
    void slotProcessOutput();

public slots:
    void slotSimulate();
    
};

#endif // XYCE_H
