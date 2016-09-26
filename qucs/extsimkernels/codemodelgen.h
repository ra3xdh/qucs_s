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

#include <QtCore>
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
protected:
    bool prepare(Schematic *sch);
public:
    bool createIFS(QTextStream &stream, Schematic *sch);
    bool createMOD(QTextStream &stream, Schematic *sch);
    
};

#endif // VERILOGAWRITER_H
