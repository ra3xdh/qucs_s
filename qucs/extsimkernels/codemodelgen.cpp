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
#include "extsimkernels/spicecompat.h"
#include <QPlainTextEdit>
#include <QProcess>
#include "misc.h"

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

    stream<<"NAME_TABLE:\n";
    stream<<QString("C_Function_Name: cm_%1\n").arg(base);
    stream<<QString("Spice_Model_Name: %1\n").arg(base);


    if (ports.isEmpty()) return false; // Not a subcircuit

    QMap<QString,QString>::iterator it = ports.begin();
    for(;it!=ports.end();it++) {
        QString pname = it.key();
        QString ptype = it.value();
        stream<<"\nPORT_TABLE:\n";
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
            QString pp = (*it)->Name.toLower();
            QString pnam = pp.section('=',0,0).trimmed().toLower();
            QString pval = pp.section('=',1,1).trimmed();
            double val,fac;
            QString unit;
            misc::str2num(pval,val,unit,fac);
            stream<<"PARAMETER_TABLE:\n";
            stream<<QString("Parameter_Name: %1\n").arg(pnam);
            stream<<QString("Description: \" %1\"\n").arg((*it)->Description);
            stream<<"Data_Type: real\n";
            stream<<QString("Default_Value: %1\n").arg(val*fac);
            stream<<"Limits: -\n"
                    "Vector: no\n"
                    "Vector_Bounds: -\n"
                    "Null_Allowed: no\n\n";
        }
        break;
      }
    return true;
}

bool CodeModelGen::createIFSfromEDD(QTextStream &stream, Schematic *sch, Component *pc)
{
    prepare(sch);
    if (pc->Model!="EDD") return false;
    int Nbranch = pc->Props.at(1)->Value.toInt();
    QStringList ports;
    for(int i=0;i<Nbranch;i++) {
        QString net1 = pc->Ports.at(2*i)->Connection->Name;
        QString net2 = pc->Ports.at(2*i+1)->Connection->Name;
        QString pname = net1+"_"+net2;
        if (!ports.contains(pname)) ports.append(pname);
    }

    QFileInfo inf(sch->DocName);
    QString base = inf.completeBaseName();
    base.remove('-').remove(' ');
    stream<<"NAME_TABLE:\n";
    stream<<QString("C_Function_Name: cm_%1\n").arg(base);
    stream<<QString("Spice_Model_Name: %1\n").arg(base);

    foreach(QString pp,ports) {
        stream<<"\nPORT_TABLE:\n";
        stream<<QString("Port_Name: %1\n").arg(pp);
        stream<<"Description: \" \"\n";
        stream<<"Direction: inout\n";
        stream<<"Default_Type: gd\n";
        stream<<"Allowed_Types: [gd]\n";
        stream<<"Vector: no\n";
        stream<<"Vector_Bounds: - \n";
        stream<<"Null_Allowed: no\n\n";
    }


    // Find parameters. Parameters are symbols
    // that are not Ginac function or input
    QStringList pars;
    for(int i=0;i<Nbranch;i++) {
        QString Ieqn;
        Ieqn = pc->Props.at(2*(i+1))->Value;
        QStringList tokens;
        spicecompat::splitEqn(Ieqn,tokens);
        foreach(QString tok,tokens){
            bool isNum = true;
            tok.toFloat(&isNum);
            QRegExp inp_pattern("[IV][0-9]+");
            bool isInput = inp_pattern.exactMatch(tok);
            if ((!isGinacFunc(tok))&&(!isNum)&&(!isInput))
                if(!pars.contains(tok)) pars.append(tok);
        }
    }

    // Form parameter table
    foreach(QString par,pars) {
        stream<<"PARAMETER_TABLE:\n";
        stream<<QString("Parameter_Name: %1\n").arg(par.toLower());
        stream<<"Description: \"  \"\n"
                "Data_Type: real\n"
                "Default_Value: 0.0\n"
                "Limits: -\n"
                "Vector: no\n"
                "Vector_Bounds: -\n"
                "Null_Allowed: no\n\n";
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

bool CodeModelGen::createMODfromEDD(QTextStream &stream, Schematic *sch, Component *pc)
{
    QFileInfo inf(sch->DocName);
    QString base = inf.completeBaseName();
    base.remove('-').remove(' ');

    prepare(sch);
    if (pc->Model!="EDD") return false;

    int Nbranch = pc->Props.at(1)->Value.toInt();
    QStringList ports;
    for(int i=0;i<Nbranch;i++) {
        QString net1 = pc->Ports.at(2*i)->Connection->Name;
        QString net2 = pc->Ports.at(2*i+1)->Connection->Name;
        QString pname = net1+"_"+net2;
        if (!ports.contains(pname)) ports.append(pname);
    }

    stream<<QString("/* XSPICE codemodel %1 auto-generated template */\n\n").arg(base);
    stream<<"#include <math.h>\n\n";
    stream<<QString("void cm_%1(ARGS)\n").arg(base);
    stream<<"{\n";

    QStringList pars,Ieqns,inputs;
    foreach(QString port,ports) {
        QString Ieqn;
        for(int i=0;i<Nbranch;i++) {
            QString net1 = pc->Ports.at(2*i)->Connection->Name;
            QString net2 = pc->Ports.at(2*i+1)->Connection->Name;
            QString pname = net1 + "_" + net2;
            if (pname == port) {
                if (Ieqn.isEmpty()) Ieqn = pc->Props.at(2*(i+1))->Value;
                else Ieqn = Ieqn + " + " + pc->Props.at(2*(i+1))->Value;
            }
        }
        normalize_functions(Ieqn);
        Ieqns.append(Ieqn);
        QStringList tokens;
        spicecompat::splitEqn(Ieqn,tokens);
        foreach(QString tok,tokens){
            bool isNum = true;
            tok.toFloat(&isNum);
            QRegExp inp_pattern("[IV][0-9]+");
            bool isInput = inp_pattern.exactMatch(tok);
            if ((isInput)&&(!inputs.contains(tok))) inputs.append(tok);
            if ((!isGinacFunc(tok))&&(!isNum)&&(!isInput))
                if(!pars.contains(tok)) pars.append(tok);
        }
    }



    QList<QStringList> Geqns; // Partial derivatives
    for(int i=0;i<ports.count();i++) {
        QStringList Geqp;
        Geqp.clear();
        for(int j=0;j<ports.count();j++) {

            QString gi;
            QString xvar = QString("V%1").arg(j+1);
            GinacDiff(Ieqns[i],xvar,gi);
            Geqp.append(gi);
        }
        Geqns.append(Geqp);
    }

    // Declare parameter variables
    QString acg = "ac_gain00";
    for (int i=1;i<ports.count();i++) {
        for (int j=1;j<ports.count();j++) {
            acg += QString(", ac_gain%1%2").arg(i).arg(j);
        }
    }
    stream<<"\tComplex_t " + acg + ";\n";
    stream<<"\tstatic double "+pars.join(",")+";\n";
    stream<<"\tstatic double "+inputs.join(",")+";\n";
    stream<<"\tif(INIT) {\n";
    foreach (QString par, pars) {
        stream<<"\t\t"+ par + " = PARAM(" + par.toLower() + ");\n";
    }
    stream<<"\t}\n";


    stream<<"\tif (ANALYSIS != AC) {\n";
    // Get input voltages
    QStringList::iterator it = inputs.begin();
    for(int i=0;it!=inputs.end();it++,i++) {
        stream<<QString("\t\t%1 = INPUT(%2);\n").arg(*it).arg(ports.at(i));
    }
    // Write output
    for(int i=0;i<ports.count();i++) {
        QString Ieq;
        GinacConvToC(Ieqns[i],Ieq);
        stream<<QString("\t\tOUTPUT(%1) = %2;\n").arg(ports.at(i)).arg(Ieq);
        stream<<QString("\t\tPARTIAL(%1,%1) = %2;\n").arg(ports.at(i)).arg(Geqns[i][0]);
    }
    stream<<"\t} else {\n";
    for (int i=0;i<ports.count();i++) {
        for (int j=0;j<ports.count();j++) {
            stream<<QString("\t\tac_gain%1%2.real = %3;\n")
                    .arg(i).arg(j).arg(Geqns[i][j]);
            stream<<QString("\t\tac_gain%1%2.imag = 0.0;\n").arg(i).arg(j);
            stream<<QString("\t\tAC_GAIN(%1,%2) = ac_gain%3%4;\n")
                    .arg(ports.at(i)).arg(ports.at(j)).arg(i).arg(j);
        }
    }
    stream<<"\t}\n";
    stream<<"}\n";
    return true;
}

bool CodeModelGen::isGinacFunc(QString &funcname)
{
    QStringList f_list;
    f_list<<"abs"<<"step"
          <<"sqrt"<<"pow"
          <<"sin"<<"cos"<<"tan"
          <<"asin"<<"acos"<<"atan"<<"atan2"
          <<"sinh"<<"cosh"<<"tanh"
          <<"asinh"<<"acosh"<<"atanh"
          <<"exp"<<"log"
          <<"="<<"("<<")"<<"*"<<"/"<<"+"<<"-"<<"^"<<"<"<<">"<<":";
    return f_list.contains(funcname);

}

bool CodeModelGen::GinacDiff(QString &eq, QString &var, QString &res)
{
    QProcess ginac;
    QTemporaryFile ginac_task;
    if(ginac_task.open()) {
        QTextStream ts(&ginac_task);
        ts<<QString("print_csrc(diff(%1,%2));\nexit;").arg(eq).arg(var);
        ginac_task.close();
    } else return false;

    ginac.setStandardInputFile(ginac_task.fileName());
    ginac.start("ginsh");
    ginac.waitForFinished();
    res = ginac.readAllStandardOutput();
    res.chop(1); // remove newline char

    return true;
}

bool CodeModelGen::GinacConvToC(QString &eq, QString &res)
{
    QProcess ginac;
    QTemporaryFile ginac_task;
    if(ginac_task.open()) {
        QTextStream ts(&ginac_task);
        ts<<QString("print_csrc(%1);\nexit;").arg(eq);
        ginac_task.close();
    } else return false;

    ginac.setStandardInputFile(ginac_task.fileName());
    ginac.start("ginsh");
    ginac.waitForFinished();
    res = ginac.readAllStandardOutput();
    res.chop(1); // remove newline char

    return true;
}


void CodeModelGen::normalize_functions(QString &Eqn)
{
    QStringList conv_list; // Put here functions need to be converted
    conv_list<<"q"<<"1.6021765e-19"
             <<"kB"<<"1.38065e-23"
             <<"u"<<"step";

    QStringList tokens;
    spicecompat::splitEqn(Eqn,tokens);
    for(QStringList::iterator it = tokens.begin();it != tokens.end(); it++) {
        for(int i=0;i<conv_list.count();i+=2) {
            if (conv_list.at(i) == *it) *it = conv_list.at(i+1);
        }
    }
    Eqn = tokens.join("");
}
