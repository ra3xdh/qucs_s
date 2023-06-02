/*
 * verilogawriter.h - Subcircuit to Verilog-A module converter declaration
 *
 * Copyright (C) 2015, Vadim Kuznetsov, ra3xdh@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef VERILOGAWRITER_H
#define VERILOGAWRITER_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <schematic.h>
#include <QRegularExpression>

/*!
  \file verilogawriter.h
  \brief Declaration of the AbstractSpiceKernel class and vacompat namespace
*/

namespace vacompat {
    QString normalize_value(QString Value);
    void convert_functions(QStringList &tokens);
    QString normalize_voltage(QString &plus, QString &minus, bool left_side = false); // Exclude gnd nodes from
    QString normalize_current(QString &plus, QString &minus, bool left_side = false); // currents and voltages
}

class VerilogAwriter
{

public:
    VerilogAwriter();
    ~VerilogAwriter();
protected:
    bool prepareToVerilogA(Schematic *sch);
public:
    bool createVA_module(QTextStream &stream, Schematic *sch);
    
};

#endif // VERILOGAWRITER_H
