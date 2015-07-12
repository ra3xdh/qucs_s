/***************************************************************************
                             qucs2spice.cpp
                             ----------------
    begin                : Mon Dec 1 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
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




#include "qucs2spice.h"
#include "spicecompat.h"
#include "components/equation.h"

/*!
  \file qucs2spice.cpp
  \brief Implementation of qucs2spice namespace
*/

namespace qucs2spice
{
   QString convert_rcl(QString line);
   QString convert_header(QString line);
   QString convert_diode(QString line,bool xyce=false);
   QString convert_jfet(QString line, bool xyce=false);
   QString convert_mosfet(QString line, bool xyce=false);
   QString convert_bjt(QString line);
   QString convert_cccs(QString line);
   QString convert_ccvs(QString line);
   QString convert_ccs(QString line, bool voltage);
   QString convert_vccs(QString line);
   QString convert_vcvs(QString line);
   QString convert_vcs(QString line, bool voltage);
   QString convert_dc_src(QString line);
   QString convert_edd(QString line, QStringList &EqnsAndVars);
   QString convert_subckt(QString line);
   QString convert_gyrator(QString line);

   void ExtractVarsAndValues(QString line,QStringList& VarsAndVals);
   void subsVoltages(QStringList &tokens, QStringList& nods);
}

/*!
 * \brief qucs2spice::convert_netlist Convert Qucs netlist to Spice netlist
 * \param[in] netlist Qucs netlist as single string
 * \param xyce True if xyce-compatible netlist is needed.
 * \return Spice netlist as a single string
 */
QString qucs2spice::convert_netlist(QString netlist, bool xyce)
{
    QStringList net_lst=netlist.split("\n");

    QRegExp res_pattern("^[ \t]*R:[A-Za-z]+.*");
    QRegExp cap_pattern("^[ \t]*C:[A-Za-z]+.*");
    QRegExp ind_pattern("^[ \t]*L:[A-Za-z]+.*");
    QRegExp diode_pattern("^[ \t]*Diode:[A-Za-z]+.*");
    QRegExp mosfet_pattern("^[ \t]*MOSFET:[A-Za-z]+.*");
    QRegExp jfet_pattern("^[ \t]*JFET:[A-Za-z]+.*");
    QRegExp bjt_pattern("^[ \t]*BJT:[A-Za-z]+.*");
    QRegExp ccvs_pattern("^[ \t]*CCVS:[A-Za-z]+.*");
    QRegExp cccs_pattern("^[ \t]*CCCS:[A-Za-z]+.*");
    QRegExp vcvs_pattern("^[ \t]*VCVS:[A-Za-z]+.*");
    QRegExp vccs_pattern("^[ \t]*VCCS:[A-Za-z]+.*");
    QRegExp subckt_head_pattern("^[ \t]*\\.Def:[A-Za-z]+.*");
    QRegExp ends_pattern("^[ \t]*\\.Def:End[ \t]*$");
    QRegExp dc_pattern("^[ \t]*[VI]dc:[A-Za-z]+.*");
    QRegExp edd_pattern("^[ \t]*EDD:[A-Za-z]+.*");
    QRegExp eqn_pattern("^[ \t]*Eqn:[A-Za-z]+.*");
    QRegExp subckt_pattern("^[ \t]*Sub:[A-Za-z]+.*");
    QRegExp gyrator_pattern("^[ \t]*Gyrator:[A-Za-z]+.*");

    QString s="";

    QStringList EqnsAndVars;

    foreach(QString line,net_lst) {  // Find equations
        if (eqn_pattern.exactMatch(line)) {
            line.remove(QRegExp("^[ \t]*Eqn:[A-Za-z]+\\w+\\s+"));
            ExtractVarsAndValues(line,EqnsAndVars);
        }
    }
    EqnsAndVars.removeAll("Export");
    EqnsAndVars.removeAll("no");
    EqnsAndVars.removeAll("yes");


    foreach(QString line,net_lst) {
        if (subckt_head_pattern.exactMatch(line)) {
            if (ends_pattern.exactMatch(line)) s += ".ENDS\n";
            else s += convert_header(line);
        }
        if (res_pattern.exactMatch(line)) s += convert_rcl(line);
        if (cap_pattern.exactMatch(line)) s += convert_rcl(line);
        if (ind_pattern.exactMatch(line)) s += convert_rcl(line);
        if (diode_pattern.exactMatch(line)) s += convert_diode(line,xyce);
        if (mosfet_pattern.exactMatch(line)) s += convert_mosfet(line,xyce);
        if (jfet_pattern.exactMatch(line)) s += convert_jfet(line,xyce);
        if (bjt_pattern.exactMatch(line)) s += convert_bjt(line);
        if (vccs_pattern.exactMatch(line)) s += convert_vccs(line);
        if (vcvs_pattern.exactMatch(line)) s += convert_vcvs(line);
        if (cccs_pattern.exactMatch(line)) s+= convert_cccs(line);
        if (ccvs_pattern.exactMatch(line)) s+= convert_ccvs(line);
        if (dc_pattern.exactMatch(line)) s += convert_dc_src(line);
        if (edd_pattern.exactMatch(line)) s += convert_edd(line,EqnsAndVars);
        if (subckt_pattern.exactMatch(line)) s+= convert_subckt(line);
        if (gyrator_pattern.exactMatch(line)) s+= convert_gyrator(line);
    }

    //s.replace(" gnd "," 0 ");
    return s;
}

QString qucs2spice::convert_rcl(QString line)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString s1 = lst.takeFirst();
    s += s1.remove(':');
    s += " " + lst.takeFirst();
    s += " " + lst.takeFirst() + " ";
    s1 = lst.takeFirst().remove("\"");
    int idx = s1.indexOf('=');
    s += s1.right(s1.count()-idx-1);
    s += "\n";
    return s;
}

QString qucs2spice::convert_header(QString line)
{
    QString s = line;
    s.replace(".Def:",".SUBCKT ");
    QStringList lst = s.split(' ',QString::SkipEmptyParts);
    lst.insert(2," gnd "); // ground
    s = lst.join(" ");
    s += "\n";
    return s;
}

QString qucs2spice::convert_diode(QString line,bool xyce)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString name = lst.takeFirst();
    int idx = name.indexOf(':');
    name =  name.right(name.count()-idx-1); // name
    QString K = lst.takeFirst();
    QString A = lst.takeFirst();
    s += QString("D%1 %2 %3 DMOD_%4 \n").arg(name).arg(A).arg(K).arg(name);
    QString mod_params = lst.join(" ");
    mod_params.remove('\"');
    if (xyce) s += QString(".MODEL DMOD_%1 D(LEVEL=2 %2) \n").arg(name).arg(mod_params);
    else  s += QString(".MODEL DMOD_%1 D(%2) \n").arg(name).arg(mod_params);
    return s;
}

QString qucs2spice::convert_mosfet(QString line, bool xyce)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString name = lst.takeFirst();
    int idx = name.indexOf(':');
    name =  name.right(name.count()-idx-1); // name
    QString G = lst.takeFirst();
    QString D = lst.takeFirst();
    QString S = lst.takeFirst();
    QString Sub = lst.takeFirst();
    QString L = "";
    QString W = "";
    QString Typ = "NMOS";
    QStringList par_lst;
    par_lst.clear();
    for(int i=0;i<lst.count();i++) {
        QString s1 = lst.at(i);
        if (s1.startsWith("L=\"")) {
            s1.remove('\"');
            L = s1;
        } else if (s1.startsWith("W=\"")) {
            s1.remove('\"');
            W = s1;
        } else if (s1.startsWith("Type=\"nfet\"")) {
            Typ = "NMOS";
        } else if (s1.startsWith("Type=\"pfet\"")) {
            Typ = "PMOS";
        } else {
            if (!s1.startsWith("N=")) // add ignore list
            par_lst.append(s1); // usual parameter
        }
    }
    s += QString("M%1 %2 %3 %4 %5 MMOD_%6 %7 %8 \n").arg(name).arg(D).arg(G).arg(S).arg(Sub)
            .arg(name).arg(L).arg(W);
    QString mod_params = par_lst.join(" ");
    mod_params.remove('\"');
    s += QString(".MODEL MMOD_%1 %2(%3) \n").arg(name).arg(Typ).arg(mod_params);
    if (xyce) s.replace(" Vt0="," VtO=");
    return s;
}

QString qucs2spice::convert_jfet(QString line, bool xyce)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString name = lst.takeFirst();
    int idx = name.indexOf(':');
    name =  name.right(name.count()-idx-1); // name
    QString G = lst.takeFirst();
    QString D = lst.takeFirst();
    QString S = lst.takeFirst();
    QString Typ = "NJF";
    QStringList par_lst;
    par_lst.clear();
    for(int i=0;i<lst.count();i++) {
        QString s1 = lst.at(i);
        if (s1.startsWith("Type=\"nfet\"")) {
            Typ = "NJF";
        } else if (s1.startsWith("Type=\"pfet\"")) {
            Typ = "PJF";
        } else {
            par_lst.append(s1); // usual parameter
        }
    }
    s += QString("J%1 %2 %3 %4 JMOD_%5 \n").arg(name).arg(D).arg(G).arg(S).arg(name);
    QString mod_params = par_lst.join(" ");
    mod_params.remove('\"');
    s += QString(".MODEL JMOD_%1 %2(%3) \n").arg(name).arg(Typ).arg(mod_params);
    if (xyce) s.replace(" Vt0="," VtO=");
    return s;
}

QString qucs2spice::convert_bjt(QString line)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString name = lst.takeFirst();
    int idx = name.indexOf(':');
    name =  name.right(name.count()-idx-1); // name
    QString B = lst.takeFirst();
    QString C = lst.takeFirst();
    QString E = lst.takeFirst();
    QString Sub = lst.takeFirst();
    QString Typ = "NPN";
    QStringList par_lst;
    par_lst.clear();

    QStringList spice_incompat; // spice incompatibel parameters;
    spice_incompat<<"Type="<<"Area="<<"Temp="<<"Ffe="<<"Kb="<<"Ab="<<"Fb=";
    for(int i=0;i<lst.count();i++) {
        QString s1 = lst.at(i);
        if (s1.startsWith("Type=\"npn\"")) {
            Typ = "NPN";
        } else if (s1.startsWith("Type=\"pnp\"")) {
            Typ = "PNP";
        } else {
            bool is_incompat = false;
            foreach (QString incompat,spice_incompat) {
               if (s1.startsWith(incompat)) {
                   is_incompat = true;
                   break;
               }
            }
            if (!is_incompat) par_lst.append(s1); // usual parameter
        }
    }
    s += QString("Q%1 %2 %3 %4 %5 QMOD_%6 \n").arg(name).arg(C).arg(B).arg(E).arg(Sub).arg(name);
    QString mod_params = par_lst.join(" ");
    mod_params.remove('\"');
    s += QString(".MODEL QMOD_%1 %2(%3) \n").arg(name).arg(Typ).arg(mod_params);
    return s;
}

QString qucs2spice::convert_cccs(QString line)
{
    return convert_ccs(line,false);
}

QString qucs2spice::convert_ccvs(QString line)
{
    return convert_ccs(line,true);
}

QString qucs2spice::convert_ccs(QString line, bool voltage)
{
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString name = lst.takeFirst();
    int idx = name.indexOf(':');
    name =  name.right(name.count()-idx-1); // name

    QString nod0 = lst.takeFirst();
    QString nod1 = lst.takeFirst();
    QString nod2 = lst.takeFirst();
    QString nod3 = lst.takeFirst();
    QString s1 = lst.takeFirst().remove("\"");
    idx = s1.indexOf('=');
    QString val = s1.right(s1.count()-idx-1);
    QString s;
    if (voltage) s="H";
    else s="F";
    s += QString("%1 %2 %3 V%4 %5\n").arg(name).arg(nod1).arg(nod2).arg(name).arg(val); // output source nodes
    s += QString("V%1 %2 %3 DC 0\n").arg(name).arg(nod0).arg(nod3);   // controlling 0V source
    return s;
}

QString qucs2spice::convert_vccs(QString line)
{
    return convert_vcs(line,false);
}

QString qucs2spice::convert_vcvs(QString line)
{
    return convert_vcs(line,true);
}

QString qucs2spice::convert_vcs(QString line,bool voltage)
{
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString name = lst.takeFirst();
    int idx = name.indexOf(':');
    name =  name.right(name.count()-idx-1); // name

    QString nod0 = lst.takeFirst();
    QString nod1 = lst.takeFirst();
    QString nod2 = lst.takeFirst();
    QString nod3 = lst.takeFirst();
    QString s1 = lst.takeFirst().remove("\"");
    idx = s1.indexOf('=');
    QString val = s1.right(s1.count()-idx-1);

    QString s;
    if (voltage) s="E";
    else s="G";
    s += QString("%1 %2 %3 %4 %5 %6\n").arg(name).arg(nod1).arg(nod2).arg(nod0).arg(nod3).arg(val);
    return s;
}

QString qucs2spice::convert_dc_src(QString line)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString s1 = lst.takeFirst();
    s += s1.remove(':');
    s += " " + lst.takeFirst();
    s += " " + lst.takeFirst() + " ";
    s1 = lst.takeFirst().remove("\"");
    int idx = s1.indexOf('=');
    QString val = s1.right(s1.count()-idx-1);
    s += "DC " + val + "\n";
    return s;
}

QString qucs2spice::convert_edd(QString line, QStringList &EqnsAndVars)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QStringList nods;
    QString nam = lst.takeFirst().remove(':');

    foreach (QString str,lst) {
        if (!str.contains('=')) {
            //str.replace("gnd","0");
            nods.append(str);
        } else break;
    }

    int Branch = nods.count()/2;

    for (int i=0;i<Branch;i++) {
        // current part
        QString Ivar = line.section('"',4*i+1,4*i+1,QString::SectionSkipEmpty);
        QString Ieqn = EqnsAndVars.at(EqnsAndVars.indexOf(Ivar)+1);

        QStringList Itokens;
        spicecompat::splitEqn(Ieqn,Itokens);
        subsVoltages(Itokens,nods);
        spicecompat::convert_functions(Itokens,false);
        QString plus = nods.at(2*i+1);
        QString minus = nods.at(2*i);

        s += QString("BI%1_%2 %3 %4 I=%5\n").arg(nam).arg(i).arg(plus).arg(minus).arg(Itokens.join(""));
        // charge part
        QString Qvar = line.section('"',2*i+3,2*i+3,QString::SectionSkipEmpty);
        QString Qeqn = EqnsAndVars.at(EqnsAndVars.indexOf(Qvar)+1);
        Qeqn.remove(' ');
        QRegExp zero_pattern("0+(\\.*0+|)");
        if (!zero_pattern.exactMatch(Qeqn)) {
            s += QString("G%1Q%2 %3 %4 n%1Q%2 %4 1.0\n").arg(nam).arg(i).arg(plus).arg(minus);
            s += QString("L%1Q%2 n%1Q%2 %3 1.0\n").arg(nam).arg(i).arg(minus);
            s += QString("B%1Q%2 n%1Q%2 %3 I=-(%4)\n").arg(nam).arg(i).arg(minus).arg(Qeqn);
        }
    }

    return s;
}


QString qucs2spice::convert_subckt(QString line)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString s1 = lst.takeFirst();
    s += "X" + s1.remove("Sub:") + " gnd ";

    QStringList::iterator it = lst.begin();

    while(!((*it).contains('='))) {
        //(*it).replace("gnd","0");
        s += " " + (*it) + " ";
        it++;
    };

    QStringList Par;
    ExtractVarsAndValues(lst.join(" "),Par);

    Par.removeFirst();
    QString sub_nam = Par.takeFirst();
    s += " " + sub_nam +" ";

    for (QStringList::iterator it = Par.begin();it!=Par.end();it++) {
        QString var = (*it);
        it++;
        QString val = (*it);
        val.remove(' ');
        s += QString("%1=%2 ").arg(var).arg(val);
    }
    s += '\n';

    return s;
}

QString qucs2spice::convert_gyrator(QString line)
{
    QString s="";
    QStringList lst = line.split(" ",QString::SkipEmptyParts);
    QString Name = lst.takeFirst();
    Name = Name.section(':',1,1);
    QString n1 = lst.takeFirst();
    QString n2 = lst.takeFirst();
    QString n3 = lst.takeFirst();
    QString n4 = lst.takeFirst();
    QString R = line.section('"',1,1);
    s +=  QString("B%1_1 %2 %3 I=(1/(%4))*(V(%5)-V(%6))\n").arg(Name).arg(n1).arg(n4).arg(R).arg(n2).arg(n3);
    s +=  QString("B%1_2 %2 %3 I=-1.0*(1/(%4))*(V(%5)-V(%6))\n").arg(Name).arg(n2).arg(n3).arg(R).arg(n1).arg(n4);
    return s;
}

void qucs2spice::ExtractVarsAndValues(QString line,QStringList& VarsAndVals)
{
    QString var;
    for(QString::iterator it = line.begin();it != line.end(); it++) {
        if ((*it).isLetterOrNumber()) {
            while ((*it)!='=') {
                var.append(*it);
                it++;
            }
            VarsAndVals.append(var);
            var.clear();
        } else if ((*it)=='"') {
            it++;
            do {
                var.append(*it);
                it++;
            } while ((*it)!='"');
            VarsAndVals.append(var);
            var.clear();
        }
    }
}


void qucs2spice::subsVoltages(QStringList &tokens, QStringList& nods)
{
    QRegExp volt_pattern("^V[0-9]+$");
    for (QStringList::iterator it = tokens.begin();it != tokens.end();it++) {
        if (volt_pattern.exactMatch(*it)) {
            QString volt = *it;
            volt.remove('V').remove(' ');
            int i = volt.toInt();
            QString plus = nods.at(2*(i-1)+1);
            QString minus = nods.at(2*(i-1));
            //if (plus=="gnd") plus="0";
            //if (minus=="gnd") minus="0";
            *it = QString("(V(%1)-V(%2))").arg(plus).arg(minus);
        }
    }
}

