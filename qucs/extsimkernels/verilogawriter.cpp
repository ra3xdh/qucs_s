/*
 * verilogawriter.cpp - Subcircuit to Verilog-A module converter implementation
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


#include "verilogawriter.h"
#include <QPlainTextEdit>
#include "paintings/id_text.h"

void vacompat::convert_functions(QStringList &tokens)
{
    QStringList conv_list; // Put here functions need to be converted
    conv_list<<"q"<<"`P_Q"
            <<"kB"<<"`P_K"
            <<"pi"<<"`M_PI";

    for(QStringList::iterator it = tokens.begin();it != tokens.end(); it++) {
        for(int i=0;i<conv_list.count();i+=2) {
            if (conv_list.at(i)==(*it))
                (*it) = conv_list.at(i+1);
        }
    }

}

QString vacompat::normalize_value(QString Value)
{
    QRegExp r_pattern("^[0-9]+.*Ohm$");
    QRegExp c_pattern("^[0-9]+.*F$");
    QRegExp l_pattern("^[0-9]+.*H$");
    QRegExp v_pattern("^[0-9]+.*V$");
    QRegExp hz_pattern("^[0-9]+.*Hz$");
    QRegExp s_pattern("^[0-9]+.*S$");

    QString s = Value.remove(' ');
    if (s.startsWith('\'')&&s.endsWith('\'')) return Value.remove('\''); // Expression detected

    if (r_pattern.exactMatch(s)) { // Component value
        s.remove("Ohm");
    } else if (c_pattern.exactMatch(s)) {
        s.remove("F");
    } else if (l_pattern.exactMatch(s)) {
        s.remove("H");
    } else if (v_pattern.exactMatch(s)) {
        s.remove("V");
    } else if (hz_pattern.exactMatch(s)) {
        s.remove("Hz");
    } else if (s_pattern.exactMatch(s)) {
        s.remove("S");
    } else

    return s;
}


VerilogAwriter::VerilogAwriter()
{
}

VerilogAwriter::~VerilogAwriter()
{

}

bool VerilogAwriter::prepareToVerilogA(Schematic *sch)
{
    QStringList collect;
    QPlainTextEdit *err = new QPlainTextEdit;
    QTextStream stream;
    if (sch->prepareNetlist(stream,collect,err)==-10) { // Broken netlist
        delete err;
        return false;
    }
    delete err;
    sch->clearSignalsAndFileList(); // for proper build of subckts
    return true;
}

bool VerilogAwriter::createVA_module(QTextStream &stream, Schematic *sch)
{
    prepareToVerilogA(sch);

    QString s;
    stream<<"`include \"disciplines.vams\"\n";
    stream<<"`include \"constants.vams\"\n";

    QStringList ports;
    QStringList nodes;
    for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
        if (pc->Model=="Port") { // Find module ports
            QString s = pc->Ports.first()->Connection->Name;
            if (!ports.contains(s)) ports.append(s);
        } else {
            foreach(Port *pp,pc->Ports) { // Find all signals
                QString s = pp->Connection->Name;
                if (!nodes.contains(s)) nodes.append(s);
            }
        }
    }

    QFileInfo inf(sch->DocName);
    QString base = inf.completeBaseName();
    base.remove('-').remove(' ');

    stream<<QString("module %1(%2);\n").arg(base).arg(ports.join(", "));
    stream<<QString("inout %1;\n").arg(ports.join(", "));
    stream<<QString("electrical %1;\n").arg(nodes.join(", "));

    Painting *pi; // Find module parameters
    for(pi = sch->SymbolPaints.first(); pi != 0; pi = sch->SymbolPaints.next())
      if(pi->Name == ".ID ") {
        ID_Text *pid = (ID_Text*)pi;
        QList<SubParameter *>::const_iterator it;
        for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
            QString s = "parameter real " + (*it)->Name + ";\n";
            stream<<s;
        }
        break;
      }


    // List all variables
    for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
        if (pc->isEquation) {
            stream<<pc->getVAvariables();
        }
    }

    stream<<"analog begin \n"
            "@(initial model)\n"
            "begin \n";
    // Output expressions
    for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
        if (pc->isEquation) {
            stream<<pc->getVAExpressions();
        }
    }

    stream<<"end\n";

    // Convert components to current equations.
    for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
         stream<<pc->getVerilogACode();
    }

    stream<<"end\n"
            "endmodule\n";
    return true;
}
