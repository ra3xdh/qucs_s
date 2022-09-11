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

#ifndef CODEMODELGEN_H
#define CODEMODELGEN_H

#include <QString>
#include <QStringList>
#include <QTextStream>

#include <schematic.h>

/*!
  \file codemodegen.h
  \brief Declaration of the CodeModelGen class
*/

class CodeModelGen
{

public:
    CodeModelGen();
    ~CodeModelGen();

private:
    QString log;

    bool isGinacFunc(QString &funcname);
    bool executeGinacCmd(QString &cmd, QString &result);
    bool GinacDiff(QString &eq,QString &var,QString &res);
    bool GinacDiffTernaryOp(QString &eq,QString &var,QString &res);
    bool GinacConvToC(QString &eq,QString &res);
    bool GinacConvToCTernaryOp(QString &eq,QString &res);
    void splitTernary(QString &eq, QStringList &tokens);
    void normalize_functions(QString &Eqn);
    void conv_to_safe_functions(QString &Eqn);
    void scanEquations(Schematic *sch,QStringList &pars,
                       QStringList &init_pars, QStringList &InitEqns);
protected:
    bool prepare(Schematic *sch);
public:
    bool createIFS(QTextStream &stream, Schematic *sch);
    bool createIFSfromEDD(QTextStream &stream, Schematic *sch, Component *pc);
    bool createMOD(QTextStream &stream, Schematic *sch);
    bool createMODfromEDD(QTextStream &stream, Schematic *sch, Component *pc);
    QString getLog();
    
};

#endif // VERILOGAWRITER_H
