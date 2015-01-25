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

#include <QtCore>
#include "schematic.h"
#include "abstractspicekernel.h"

class Ngspice : public AbstractSpiceKernel
{
    Q_OBJECT
public:
    explicit Ngspice(Schematic *sch_, QObject *parent = 0);
    
protected:
    void createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                  QStringList &vars, QStringList &outputs);
public slots:
    void slotSimulate();
};

#endif // NGSPICE_H
