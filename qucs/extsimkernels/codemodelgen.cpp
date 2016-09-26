/*
 * codemodelgen.cpp - Subcircuit to XSPICE CodeModel C-code converter implementation
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


#include "codemodelgen.h"
#include <QPlainTextEdit>

#include "paintings/id_text.h"

/*!
  \file codemodelgen.cpp
  \brief Implementation of the CodeModelGen class and vacompat namespace.
*/


CodeModelGen::CodeModelGen()
{

}

CodeModelGen::~CodeModelGen()
{

}

bool CodeModelGen::prepare(Schematic *sch)
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

bool CodeModelGen::createIFS(QTextStream &stream, Schematic *sch)
{
    prepare(sch);
    QMap<QString,QString> ports;
    for(Component *pc = sch->DocComps.first(); pc != 0; pc = sch->DocComps.next()) {
        if (pc->Model=="Port") { // Find module ports
            QString pnet = pc->Ports.first()->Connection->Name;
            QString ptype = pc->Props.at(2)->Value;
            if(!ptype.endsWith("d")) {
                if (!ports.contains(pnet)) ports.insert(pnet,ptype);
            }
        }
    }

    QStringList bound_ports;
    for(unsigned int i=0;i<sch->DocComps.count();i++) {
        Component *pc_i = sch->DocComps.at(i);
        if(pc_i->Model == "Port") {
            QString pp = pc_i->Props.at(3)->Value;
            QString pnet1 = pc_i->Ports.first()->Connection->Name;
            QString ptype = pc_i->Props.at(2)->Value;
            if(!ptype.endsWith("d")) continue;
            if(bound_ports.contains(pc_i->Name)) continue;
            for(unsigned int j=0;j<sch->DocComps.count();j++) {
                Component *pc_j = sch->DocComps.at(j);
                if((pc_j->Model=="Port")&&(pc_j->Name==pp)) {
                    bound_ports.append(pc_j->Name);
                    QString pnet2 = pc_j->Ports.first()->Connection->Name;
                    QString p_nam = pnet1 + "_" + pnet2;
                    if (!ports.contains(p_nam)) ports.insert(p_nam,ptype);
                }
            }
        }
    }

    QFileInfo inf(sch->DocName);
    QString base = inf.completeBaseName();
    base.remove('-').remove(' ');

    stream<<"NAME TABLE:\n";
    stream<<QString("C_Function_Name: cm_%1\n").arg(base);
    stream<<QString("Spice_Model_Name: %1\n").arg(base);


    if (ports.isEmpty()) return false; // Not a subcircuit

    QMap<QString,QString>::iterator it = ports.begin();
    for(;it!=ports.end();it++) {
        QString pname = it.key();
        QString ptype = it.value();
        stream<<"\nPORT TABLE:\n";
        stream<<QString("Port_Name: %1\n").arg(pname);
        stream<<"Description: \" \"\n";
        stream<<"Direction: inout\n";
        stream<<QString("Default_Type: %1\n").arg(ptype);
        stream<<QString("Allowed_Types: [%1]\n").arg(ptype);
        stream<<"Vector: no\n";
        stream<<"Vector_Bounds: - \n";
        stream<<"Null_Allowed: no\n\n";
    }

    Painting *pi; // Find IFS parameters
    for(pi = sch->SymbolPaints.first(); pi != 0; pi = sch->SymbolPaints.next())
      if(pi->Name == ".ID ") {
        ID_Text *pid = (ID_Text*)pi;
        QList<SubParameter *>::const_iterator it;
        for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
            stream<<"PARAMETER_TABLE:\n";
            stream<<QString("Parameter_Name: %1\n").arg((*it)->Name.toLower());
            stream<<QString("Description: %1\n").arg((*it)->Description);
            stream<<"Data_Type: real\n";
            stream<<QString("Default_Value: %1\n").arg((*it)->Type);
            stream<<"Limits: -\n"
                    "Vector: no\n"
                    "Vector_Bounds: -\n"
                    "Null_Allowed: no\n\n";
        }
        break;
      }
    return true;
}

bool CodeModelGen::createMOD(QTextStream &stream, Schematic *sch)
{
    QFileInfo inf(sch->DocName);
    QString base = inf.completeBaseName();
    base.remove('-').remove(' ');

    stream<<QString("/* XSPICE codemodel %1 auto-generated template */\n\n").arg(base);
    stream<<QString("void cm_%1(ARGS)\n").arg(base);
    stream<<"{\n\n}\n";
    return true;
}
