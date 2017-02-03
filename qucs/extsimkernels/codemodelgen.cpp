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
    stream<<QString("Spice_Model_Name: %1\n").arg(base.toLower());
    stream<<"Description: \" \"\n\n";


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
    stream<<QString("Spice_Model_Name: %1\n").arg(base.toLower());
    stream<<"Description: \" \"\n\n";

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
        QString Ieqn = pc->Props.at(2*(i+1))->Value;
        QString Qeqn = pc->Props.at(2*(i+1)+1)->Value;
        QStringList tokens,tokens1;
        spicecompat::splitEqn(Ieqn,tokens);
        spicecompat::splitEqn(Qeqn,tokens1);
        tokens.append(tokens1);
        foreach(QString tok,tokens){
            bool isNum = true;
            tok.toFloat(&isNum);
            QRegExp inp_pattern("[IV][0-9]+");
            bool isInput = inp_pattern.exactMatch(tok);
            if ((!isGinacFunc(tok))&&(!isNum)&&(!isInput))
                if(!pars.contains(tok)) pars.append(tok);
        }
    }

    QStringList dummy1,dummy2; // output drain
    scanEquations(sch,pars,dummy1,dummy2); // Recursively extract all parameter from Eqns.

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
    stream<<"#include <math.h>\n";
    stream<<"#include \"xspice_mathfunc.h\"\n\n";

    // The start of main() function
    stream<<QString("void cm_%1(ARGS)\n").arg(base);
    stream<<"{\n";

    QStringList pars,init_pars,Ieqns,Qeqns,InitEqns,inputs;
    foreach(QString port,ports) {
        QString Ieqn,Qeqn;
        for(int i=0;i<Nbranch;i++) {
            QString net1 = pc->Ports.at(2*i)->Connection->Name;
            QString net2 = pc->Ports.at(2*i+1)->Connection->Name;
            QString pname = net1 + "_" + net2;
            if (pname == port) {
                if (Ieqn.isEmpty()) Ieqn = pc->Props.at(2*(i+1))->Value;
                else Ieqn = Ieqn + " + " + pc->Props.at(2*(i+1))->Value;
                if (Qeqn.isEmpty()) Qeqn = pc->Props.at(2*(i+1)+1)->Value;
                else Qeqn = Qeqn + " + " + pc->Props.at(2*(i+1)+1)->Value;
            }
        }
        normalize_functions(Ieqn);
        normalize_functions(Qeqn);
        Ieqns.append(Ieqn);
        Qeqns.append(Qeqn);
        QStringList tokens,tokens1;
        spicecompat::splitEqn(Ieqn,tokens);
        spicecompat::splitEqn(Qeqn,tokens1);
        tokens.append(tokens1);
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

    scanEquations(sch,pars,init_pars,InitEqns);

    QStringList inputs_old; // Variables for charge eqns.
    foreach(QString inp,inputs) {
        inputs_old.append(inp+"_old");
    }


    QList<QStringList> Geqns; // Partial derivatives
    for(int i=0;i<ports.count();i++) {
        QStringList Geqp;
        Geqp.clear();
        for(int j=0;j<ports.count();j++) {
            QString gi;
            QString xvar = QString("V%1").arg(j+1);
            if (Ieqns[i].contains("?")) GinacDiffTernaryOp(Ieqns[i],xvar,gi);
            else GinacDiff(Ieqns[i],xvar,gi);
            conv_to_safe_functions(gi);
            Geqp.append(gi);
        }
        Geqns.append(Geqp);
    }

    // Declare parameter variables
    QString acg;
    for (int i=0;i<ports.count();i++) {
        for (int j=0;j<ports.count();j++) {
            acg += QString(", ac_gain%1%2").arg(i).arg(j);
        }
    }
    acg.remove(0,2); // remove leading comma
    stream<<"\tComplex_t " + acg + ";\n";
    stream<<"\tstatic double "+pars.join(",")+";\n";
    if (!init_pars.isEmpty())
        stream<<"\tstatic double "+init_pars.join(",")+";\n";
    stream<<"\tstatic double "+inputs.join(",")+","+inputs_old.join(",")+";\n";
    QString Qvars;
    for (int i=0;i<ports.count();i++) {
        Qvars += QString(", Q%1, cQ%1").arg(i);
    }
    Qvars.remove(0,2); // remove leading comma
    stream<<"\tdouble " + Qvars + ";\n";
    stream<<"\tdouble delta_t;\n\n";

    stream<<"\tif(INIT) {\n";
    foreach (QString par, pars) {
        stream<<"\t\t"+ par + " = PARAM(" + par.toLower() + ");\n";
    }
    auto it_ip=init_pars.begin();
    auto it_ie=InitEqns.begin();
    while(it_ip!=init_pars.end()) {
        stream<<QString("\t\t%1=%2;\n").arg(*it_ip).arg(*it_ie);
        it_ip++;
        it_ie++;
    }
    stream<<"\t}\n";


    stream<<"\tif (ANALYSIS != AC) {\n";
    stream<<"\tif (TIME == 0) {\n";
    //QStringList::iterator it1 = inputs.begin();
    for(auto it1 = inputs.begin();it1!=inputs.end();it1++) {
        QString vv = *it1; // Get input voltage number
        int vn = vv.remove(0,1).toInt()-1;
        stream<<QString("\t\t%1_old = %1 = INPUT(%2);\n").arg(*it1).arg(ports.at(vn));
    }
    for (int i=0;i<ports.count();i++) {
        stream<<QString("\t\tQ%1=0.0;\n").arg(i);
        stream<<QString("\t\tcQ%1=0.0;\n").arg(i);
    }
    stream<<"\t} else {\n";
    // Get input voltages
    //QStringList::iterator it = inputs.begin();
    for(auto it = inputs.begin();it!=inputs.end();it++) {
        QString vv = *it; // Get input voltage number
        int vn = vv.remove(0,1).toInt()-1;
        stream<<QString("\t\t%1 = INPUT(%2);\n").arg(*it).arg(ports.at(vn));
    }
    // Time variable for charge eqns.
    stream<<"\t\tdelta_t=TIME-T(1);\n";
    // Calculate charge parts
    for(int i=0;i<ports.count();i++) {
        QString Ceq,rCeq;
        Ceq = QString("expand((%1)/V%2)").arg(Qeqns.at(i)).arg(i+1);
        GinacConvToC(Ceq,rCeq);
        bool ok = false;
        float cc = rCeq.toFloat(&ok);
        if ((cc!=0)||(!ok)) {
            stream<<QString("\t\tQ%1 = (%2)*(V%3-V%3_old)/(delta_t+1e-20);\n").arg(i).arg(rCeq).arg(i+1);
            stream<<QString("\t\tcQ%1 = (%2)/(delta_t+1e-20);\n").arg(i).arg(rCeq);
        }
    }
    foreach(QString inp,inputs) {
        stream<<QString("\t\t%1_old = %1;\n").arg(inp);
    }
    stream<<"\t}\n";
    // Write current output
    for(int i=0;i<ports.count();i++) {
        QString Ieq;
        if (Ieqns[i].contains("?")) GinacConvToCTernaryOp(Ieqns[i],Ieq);
        else GinacConvToC(Ieqns[i],Ieq);
        stream<<QString("\t\tOUTPUT(%1) = %2 + Q%3;\n").arg(ports.at(i)).arg(Ieq).arg(i);
    }
    for (int i=0;i<ports.count();i++) {
        for (int j=0;j<ports.count();j++) {
            QString Geq = Geqns[i][j];
            if (i==j) stream<<QString("\t\tPARTIAL(%1,%2) = %3 + cQ%4;\n").arg(ports.at(i))
                              .arg(ports.at(j)).arg(Geq).arg(i);
            else stream<<QString("\t\tPARTIAL(%1,%2) = %3;\n").arg(ports.at(i))
                              .arg(ports.at(j)).arg(Geq);
        }
    }
    stream<<"\t} else {\n";
    for (int i=0;i<ports.count();i++) {
        for (int j=0;j<ports.count();j++) {
            stream<<QString("\t\tac_gain%1%2.real = %3;\n")
                    .arg(i).arg(j).arg(Geqns[i][j]);
            if (i == j) {
                QString Ceq,rCeq;
                Ceq = QString("expand((%1)/V%2)").arg(Qeqns.at(i)).arg(i+1);
                GinacConvToC(Ceq,rCeq);
                stream<<QString("\t\tac_gain%1%2.imag = (%3)*RAD_FREQ;\n")
                        .arg(i).arg(j).arg(rCeq);
            } else {
                stream<<QString("\t\tac_gain%1%2.imag = 0.0;\n").arg(i).arg(j);
            }
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
          <<"exp"<<"log"<<"u"
          <<"="<<"("<<")"<<"*"<<"/"<<"+"<<"-"<<"^"<<"<"<<">"<<":"<<"?"
          <<"kB"<<"q";
    return f_list.contains(funcname);

}


bool CodeModelGen::executeGinacCmd(QString &cmd, QString &result)
{
    QProcess ginac;
    QTemporaryFile ginac_task;
    if(ginac_task.open()) {
        QTextStream ts(&ginac_task);
        ts<<cmd;
        ginac_task.close();
    } else return false;

    ginac.setStandardInputFile(ginac_task.fileName());
    ginac.start("ginsh");
    ginac.waitForFinished();
    result = ginac.readAllStandardOutput();
    result.chop(1); // remove newline char

    QString err = ginac.readAllStandardError();
    if (!err.isEmpty()) {
        log += QString("Executing Ginac: %1").arg(cmd);
        log += QString("\n[fatal..]: %1").arg(err);
    }

    return true;
}

bool CodeModelGen::GinacDiff(QString &eq, QString &var, QString &res)
{
    QString cmd = QString("print_csrc(diff(%1,%2));\nexit;").arg(eq).arg(var);
    return executeGinacCmd(cmd,res);
}

bool CodeModelGen::GinacDiffTernaryOp(QString &eq, QString &var, QString &res)
{
    QStringList tokens;
    splitTernary(eq,tokens);

    bool r = false;
    QStringList::iterator subeq = tokens.begin();
    for(;subeq!=tokens.end();subeq++) {
        if (!(*subeq).contains(QRegExp("[?:<>=]"))) {
            QString subres;
            r = GinacDiff(*subeq,var,subres);
            if (!r) return false;
            else *subeq = subres;
        }
    }
    res = tokens.join("");
    return r;
}

bool CodeModelGen::GinacConvToC(QString &eq, QString &res)
{
    QString cmd = QString("print_csrc(%1);\nexit;").arg(eq);
    return executeGinacCmd(cmd,res);
}

bool CodeModelGen::GinacConvToCTernaryOp(QString &eq, QString &res)
{
    QStringList tokens;
    splitTernary(eq,tokens);

    bool r = false;
    QStringList::iterator subeq = tokens.begin();
    for(;subeq!=tokens.end();subeq++) {
        if (!(*subeq).contains(QRegExp("[?:<>=]"))) {
            QString subres;
            r = GinacConvToC(*subeq,subres);
            if (!r) return false;
            else *subeq = subres;
        }
    }
    res = tokens.join("");
    return r;
}

void CodeModelGen::splitTernary(QString &eq, QStringList &tokens)
{
    QString tok = "";
    for (QString::iterator it=eq.begin();it!=eq.end();it++) {
        QString delim = "?:";
        if (it->isSpace()) continue;
        if (delim.contains(*it)) {
            if (!tok.isEmpty()) tokens.append(tok);
            tokens.append(*it);
            tok.clear();
            continue;
        }
        tok += *it;
    }
    if (!tok.isEmpty()) tokens.append(tok);
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

/*!
 * \brief CodeModelGen::conv_to_safe_functions
 *        Replace C-functions by safe macros that never gives NaN
 *        Such functions have the same behavior as for Ngspice
 * \param Eqn
 */
void CodeModelGen::conv_to_safe_functions(QString &Eqn)
{
    Eqn.remove(' ');
    QStringList conv_list; // Put here functions need to be converted
    conv_list<<"pow"<<"Xpow";

    QStringList tokens;
    spicecompat::splitEqn(Eqn,tokens);
    for(QStringList::iterator it = tokens.begin();it != tokens.end(); it++) {
        for(int i=0;i<conv_list.count();i+=2) {
            if (conv_list.at(i) == *it) *it = conv_list.at(i+1);
        }
    }
    Eqn = tokens.join("");
}

void CodeModelGen::scanEquations(Schematic *sch,QStringList &pars,
                                 QStringList &init_pars, QStringList &InitEqns)
{
    bool found = false;
    for(Component *pc=sch->DocComps.first();pc!=0;pc=sch->DocComps.next()) {
        if(pc->Model=="Eqn") {
            int Np = pc->Props.count();
            for(int i=0;i<Np-1;i++) {
                Property *pp = pc->Props.at(i);
                QString nam = pp->Name;
                if(pars.contains(nam)) {
                    found =  true;
                    pars.remove(nam);
                    if(!init_pars.contains(nam)) {
                        init_pars.append(nam);
                        QStringList tokens;
                        QString InitEqn = pp->Value;
                        normalize_functions(InitEqn);
                        QString res;
                        spicecompat::splitEqn(InitEqn,tokens);
                        GinacConvToC(InitEqn,res);
                        InitEqn = res;
                        InitEqns.append(InitEqn);
                        foreach(QString tok,tokens) {
                            bool isNum = true;
                            tok.toFloat(&isNum);
                            if ((!isGinacFunc(tok))&&(!isNum))
                                if(!pars.contains(tok)) pars.append(tok);
                        }
                    }
                    //pp = pc->Props.prev();
                }
            }
        }
    }
    if (found) {
        scanEquations(sch,pars,init_pars,InitEqns); // Remain parameters --- scan again
    } else {
        int Nv = init_pars.count(); // Reverse init parameters list before exit
        for(int i = 0; i < (Nv/2); i++) {
            init_pars.swap(i,Nv-(1+i));
            InitEqns.swap(i,Nv-(1+i));
        }
        return;
    }
}

QString CodeModelGen::getLog()
{
    return log;
}
