/***************************************************************************
                           abstractspicekernel.h
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




#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "abstractspicekernel.h"
#include "misc.h"
#include "../paintings/id_text.h"
#include <QTextEdit>



AbstractSpiceKernel::AbstractSpiceKernel(Schematic *sch_, QObject *parent) :
    QObject(parent)
{
    Sch = sch_;

    workdir = QDir::convertSeparators(QDir::homePath()+"/.qucs/spice4qucs");
    QFileInfo inf(workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(workdir);
    }

    SimProcess = new QProcess(this);
    SimProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(SimProcess,SIGNAL(finished(int)),this,SLOT(slotFinished()));
    connect(this,SIGNAL(destroyed()),this,SLOT(killThemAll()));

}


AbstractSpiceKernel::~AbstractSpiceKernel()
{
    killThemAll();
}

void AbstractSpiceKernel::killThemAll()
{
    if (SimProcess->state()!=QProcess::NotRunning) {
        SimProcess->kill();
    }
}


bool AbstractSpiceKernel::prepareSpiceNetlist(QTextStream &stream, bool xyce)
{
    QStringList collect;
    QTextEdit *err = new QTextEdit;
    Sch->prepareNetlist(stream,collect,err,true,xyce);
    delete err;
    Sch->clearSignalsAndFileList(); // for proper build of subckts
    return true; // TODO: Add feature to determine ability of spice simulation
}

void AbstractSpiceKernel::startNetlist(QTextStream &stream, bool xyce)
{
        QString s;
        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
            if (pc->isEquation) {
                s = pc->getExpression(xyce);
                stream<<s;
            }
        }

        for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
          if(Sch->isAnalog &&
             !(pc->isSimulation) &&
             !(pc->isEquation)) {
            s = pc->getSpiceNetlist(xyce);
            stream<<s;
          }
        }
}

void AbstractSpiceKernel::createNetlist(QTextStream&, int ,QStringList&,
                                        QStringList&, QStringList &)
{

}

void AbstractSpiceKernel::createSubNetlsit(QTextStream &stream, bool xyce)
{
    QString header;
    QString f = misc::properFileName(Sch->DocName);
    header = QString(".SUBCKT %1 ").arg(misc::properName(f));

    QList< QPair<int,QString> > ports;
    if(!prepareSpiceNetlist(stream,xyce)) return; // Unable to perform spice simulation
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->Model=="Port") {
            ports.append(qMakePair(pc->Props.first()->Value.toInt(),
                                   pc->Ports.first()->Connection->Name));
        }
    }
    qSort(ports);
    QPair<int,QString> pp;
    foreach(pp,ports) {
        header += pp.second + " ";
    }

    Painting *pi;
    for(pi = Sch->SymbolPaints.first(); pi != 0; pi = Sch->SymbolPaints.next())
      if(pi->Name == ".ID ") {
        ID_Text *pid = (ID_Text*)pi;
        QList<SubParameter *>::const_iterator it;
        for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
            header += (*it)->Name + " "; // keep 'Name' unchanged
          //(*tstream) << " " << s.replace("=", "=\"") << '"';
        }
        break;
      }

    header += "\n";
    stream<<header;
    startNetlist(stream,xyce);
    stream<<".ENDS\n";
}

void AbstractSpiceKernel::slotSimulate()
{

}




void AbstractSpiceKernel::parseNgSpiceSimOutput(QString ngspice_file,QList< QList<double> > &sim_points,QStringList &var_list, bool &isComplex)
{
    isComplex = false;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        sim_points.clear();
        bool start_values_sec = false;
        int NumVars=0; // Number of dep. and indep.variables
        while (!ngsp_data.atEnd()) {
            QRegExp sep("[ \t,]");
            QString lin = ngsp_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.contains("Flags")&&lin.contains("complex")) { // output consists of
                isComplex = true; // complex numbers
                continue;         // maybe ac_analysis
            }
            if (lin.contains("No. Variables")) {  // get number of variables
                NumVars=lin.section(sep,2,2,QString::SectionSkipEmpty).toInt();
                qDebug()<<NumVars;
                continue;
            }
            if (lin=="Variables:") {
                var_list.clear();
                QString indep_var = ngsp_data.readLine().section(sep,1,1,QString::SectionSkipEmpty);
                var_list.append(indep_var);

                for (int i=1;i<NumVars;i++) {
                    lin = ngsp_data.readLine();
                    QString dep_var = lin.section(sep,1,1,QString::SectionSkipEmpty);
                    qDebug()<<dep_var;
                    var_list.append(dep_var);
                }
                qDebug()<<var_list;
                continue;
            }
            if (lin=="Values:") {
                start_values_sec = true;
                continue;
            }
            if (start_values_sec) {
                QList<double> sim_point;
                double indep_val = lin.section(sep,1,1,QString::SectionSkipEmpty).toDouble();
                sim_point.append(indep_val);
                for (int i=0;i<NumVars;i++) {
                    if (isComplex) {
                        QStringList lst = ngsp_data.readLine().split(sep,QString::SkipEmptyParts);
                        if (lst.count()==2) {
                            double re_dep_val = lst.at(0).toDouble();  // for complex sim results
                            double im_dep_val = lst.at(1).toDouble();  // imaginary part follows
                            sim_point.append(re_dep_val);              // real part
                            sim_point.append(im_dep_val);
                        }
                    } else {
                        double dep_val = ngsp_data.readLine().remove(sep).toDouble();
                        sim_point.append(dep_val);
                    }
                }
                sim_points.append(sim_point);
            }
        }
        ofile.close();
    }
}

void AbstractSpiceKernel::parseHBOutput(QString ngspice_file,
                                        QList<QList<double> > &sim_points, QStringList &var_list)
{
    var_list.clear();
    sim_points.clear();
    int NumVars=0;
    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream hb_data(&ofile);
        var_list.append("hbfrequency");
        bool first_head = true;
        while (!hb_data.atEnd()) {
            QRegExp sep("[ \t,]");
            QRegExp heading("^[a-zA-Z_][a-zA-Z0-9]{1,},");
            QRegExp dataline("^[0-9]{1,},");
            QString lin = hb_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.startsWith("Index")) { // CSV heading
                if (first_head) {
                    QStringList vars1 = lin.split(" ",QString::SkipEmptyParts);
                    vars1.removeFirst();
                    vars1.removeFirst();
                    var_list.append(vars1);
                    NumVars = var_list.count();
                    qDebug()<<vars1;
                }
                first_head = false;
            }
            if ((lin.contains(QRegExp("\\d*\\.\\d+[+-]*[eE]*[\\d]*")))&&(!first_head)) { // CSV dataline
                QStringList vals = lin.split(" ",QString::SkipEmptyParts);
                QList <double> sim_point;
                sim_point.clear();
                for (int i=NumVars+1;i<vals.count();i++) {
                    sim_point.append(vals.at(i).toDouble());
                }
                sim_points.append(sim_point);
                qDebug()<<sim_point;
            }
        }
        ofile.close();
    }
}

void AbstractSpiceKernel::parseSTEPOutput(QString ngspice_file,
                     QList< QList<double> > &sim_points,
                     QStringList &var_list, bool &isComplex)
{
    isComplex = false;

    QFile ofile(ngspice_file);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream ngsp_data(&ofile);
        sim_points.clear();
        bool start_values_sec = false;
        bool header_parsed = false;
        int NumVars=0; // Number of dep. and indep.variables
        while (!ngsp_data.atEnd()) {
            QRegExp sep("[ \t,]");
            QString lin = ngsp_data.readLine();
            if (lin.isEmpty()) continue;
            if (lin.contains("Plotname:")&&  // skip operating point
                (lin.contains("DC operating point"))) {
                for(bool t = false; !t; t = (ngsp_data.readLine().startsWith("Plotname:")));
            }
            if (!header_parsed) {
                if (lin.contains("Flags")&&lin.contains("complex")) { // output consists of
                    isComplex = true; // complex numbers
                    continue;         // maybe ac_analysis
                }
                if (lin.contains("No. Variables")) {  // get number of variables
                    NumVars=lin.section(sep,2,2,QString::SectionSkipEmpty).toInt();
                    qDebug()<<NumVars;
                    continue;
                }
                if (lin=="Variables:") {
                    var_list.clear();
                    QString indep_var = ngsp_data.readLine().section(sep,1,1,QString::SectionSkipEmpty);
                    var_list.append(indep_var);

                    for (int i=1;i<NumVars;i++) {
                        lin = ngsp_data.readLine();
                        QString dep_var = lin.section(sep,1,1,QString::SectionSkipEmpty);
                        qDebug()<<dep_var;
                        var_list.append(dep_var);
                    }
                    qDebug()<<var_list;
                    header_parsed = true;
                    continue;
                }
            }

            if (lin=="Values:") {
                start_values_sec = true;
                continue;
            }
            if (start_values_sec) {
                QList<double> sim_point;
                bool ok = false;
                qDebug()<<lin;
                QRegExp dataline_patter("^ *[0-9]+[ \t]+.*");
                if (!dataline_patter.exactMatch(lin)) continue;
                double indep_val = lin.section(sep,1,1,QString::SectionSkipEmpty).toDouble(&ok);
                //double indep_val = lin.split(sep,QString::SkipEmptyParts).at(1).toDouble(&ok); // only real indep vars
                if (!ok) continue;
                sim_point.append(indep_val);
                for (int i=0;i<NumVars;i++) {
                    if (isComplex) {
                        QStringList lst = ngsp_data.readLine().split(sep,QString::SkipEmptyParts);
                        if (lst.count()==2) {
                            double re_dep_val = lst.at(0).toDouble();  // for complex sim results
                            double im_dep_val = lst.at(1).toDouble();  // imaginary part follows
                            sim_point.append(re_dep_val);              // real part
                            sim_point.append(im_dep_val);
                        }
                    } else {
                        double dep_val = ngsp_data.readLine().remove(sep).toDouble();
                        sim_point.append(dep_val);
                    }
                }
                sim_points.append(sim_point);
            }

        }
        ofile.close();
    }
}

void AbstractSpiceKernel::parseHBSTEPOutput(QString , QList<QList<double> >&,
                                            QStringList&, bool&)
{

}

void AbstractSpiceKernel::parseResFile(QString resfile, QString &var, QStringList &values)
{
    var.clear();
    values.clear();

    QFile ofile(resfile);
    if (ofile.open(QFile::ReadOnly)) {
        QTextStream swp_data(&ofile);
        while (!swp_data.atEnd()) {
            QRegExp point_pattern("^\\s*[0-9]+ .*");
            QRegExp var_pattern("^STEP\\s+.*");
            QRegExp sep("\\s");
            QString lin = swp_data.readLine();
            if (var_pattern.exactMatch(lin)) {
                var = lin.split(sep,QString::SkipEmptyParts).last();
            }
            if (point_pattern.exactMatch(lin)) {
                values.append(lin.split(sep,QString::SkipEmptyParts).last());
            }
        }
        ofile.close();
    }
}

void AbstractSpiceKernel::convertToQucsData(const QString &qucs_dataset, bool xyce)
{
    QFile dataset(qucs_dataset);
    if (dataset.open(QFile::WriteOnly)) {
        QTextStream ds_stream(&dataset);

        ds_stream<<"<Qucs Dataset " PACKAGE_VERSION ">\n";

        QString sim,indep;
        QStringList indep_vars;

        QString swp_var;
        QStringList swp_var_val;
        swp_var.clear();
        swp_var_val.clear();

        QList< QList<double> > sim_points;
        QStringList var_list;
        bool isComplex = false;
        bool hasParSweep = false;

        QString ngspice_output_filename;
        foreach(ngspice_output_filename,output_files) { // For every simulation convert results to Qucs dataset
            QString full_outfile = workdir+QDir::separator()+ngspice_output_filename;
            if (ngspice_output_filename.endsWith("_hb.txt")) {
                parseHBOutput(full_outfile,sim_points,var_list);
                isComplex = true;
            } else if (ngspice_output_filename.endsWith("_swp.txt")) {
                hasParSweep = true;
                QString simstr = full_outfile;
                simstr.remove("_swp.txt");
                simstr = simstr.split('_').last();
                QString res_file;
                if (xyce) res_file = QDir::convertSeparators(workdir + QDir::separator()
                                                        + "spice4qucs." + simstr + ".cir.res");
                else res_file = QDir::convertSeparators(workdir + QDir::separator()
                                                        + "spice4qucs.cir.res");
                qDebug()<<res_file;
                parseResFile(res_file,swp_var,swp_var_val);

                if (ngspice_output_filename.endsWith("_hb_swp.txt")) {
                    parseHBSTEPOutput(full_outfile,sim_points,var_list,isComplex);
                } else {
                    parseSTEPOutput(full_outfile,sim_points,var_list,isComplex);
                }

            } else {
                parseNgSpiceSimOutput(full_outfile,sim_points,var_list,isComplex);
            }
            if (var_list.isEmpty()) continue; // notning to convert
            normalizeVarsNames(var_list);

            QString indep = var_list.first();
            QList<double> sim_point;


            if (hasParSweep) {

                int indep_cnt = sim_points.count()/swp_var_val.count();
                ds_stream<<QString("<indep %1 %2>\n").arg(indep).arg(indep_cnt); // output indep var: TODO: parameter sweep
                for (int i=0;i<indep_cnt;i++) {
                    ds_stream<<QString::number(sim_points.at(i).at(0),'e',12)<<endl;
                }
                ds_stream<<"</indep>\n";

                ds_stream<<QString("<indep %1 %2>\n").arg(swp_var).arg(swp_var_val.count());
                foreach (QString val,swp_var_val) {
                    ds_stream<<val<<endl;
                }
                ds_stream<<"</indep>\n";
                indep += " " + swp_var;
            } else {
                ds_stream<<QString("<indep %1 %2>\n").arg(indep).arg(sim_points.count()); // output indep var: TODO: parameter sweep
                foreach (sim_point,sim_points) {
                    ds_stream<<QString::number(sim_point.at(0),'e',12)<<endl;
                }
                ds_stream<<"</indep>\n";
            }

            for(int i=1;i<var_list.count();i++) { // output dep var
                ds_stream<<QString("<dep %1 %2>\n").arg(var_list.at(i)).arg(indep);
                foreach (sim_point,sim_points) {
                    if (isComplex) {
                        double re=sim_point.at(2*(i-1)+1);
                        double im = sim_point.at(2*i);
                        QString s;
                        s += QString::number(re,'e',12);
                        if (im<0) s += "-j";
                        else s += "+j";
                        s += QString::number(fabs(im),'e',12) + "\n";
                        ds_stream<<s;
                    } else {
                        ds_stream<<QString::number(sim_point.at(i),'e',12)<<endl;
                    }
                }
                ds_stream<<"</dep>\n";
            }
            hasParSweep = false;
        }

        dataset.close();
    }
}

// Normalize Ngspice and Xyce variables naming
void AbstractSpiceKernel::normalizeVarsNames(QStringList &var_list)
{
    QString prefix="";
    QString iprefix="";
    QString indep = var_list.first();
    bool HB = false;
    qDebug()<<"norm:"<<indep;
    indep = indep.toLower();
    if (indep=="time") {
        prefix = "tran.";
        iprefix = "i(tran.";
    } else if (indep=="frequency") {
        prefix = "ac.";
        iprefix = "i(ac.";
    } else if (indep=="hbfrequency") {
        HB = true;
    }

    QStringList::iterator it=var_list.begin();

    for (it++;it!=var_list.end();it++) {
        if ((!(it->startsWith(prefix)||it->startsWith(iprefix)))||(HB)) {
            if (HB) {
                int idx = it->indexOf('(');  // Add .Vb suffix for correct display
                int cnt = it->count();
                *it = it->right(cnt-idx-1); // Only node.Vb is displayed correctly
                it->remove(')');
                *it += ".Vb";
            } else {
                *it = prefix + *it;
            }
        }
        QStringList lst = it->split('(');
        if (lst.count()>1) {
            QRegExp ivprobe_pattern("^[Vv][Pp][Rr][0-9]+.*");
            QRegExp ivprobe_pattern_ngspice("^(ac\\.|tran\\.)[Vv][Pp][Rr][0-9]+.*");
            if (ivprobe_pattern.exactMatch(lst.at(1))) {
                lst[1].remove(0,1);
                *it = lst.join("(");
            } else if (ivprobe_pattern_ngspice.exactMatch(lst.at(1))) {
                lst[1].replace(".v",".",Qt::CaseInsensitive);
                *it = lst.join("(");
            }
        }
    }

}

void AbstractSpiceKernel::slotErrors(QProcess::ProcessError err)
{
    emit errors(err);
}

void AbstractSpiceKernel::slotFinished()
{
    output.clear();
    output = SimProcess->readAllStandardOutput();
    emit finished();
}

QString AbstractSpiceKernel::getOutput()
{
    return output;
}

void AbstractSpiceKernel::setSimulatorCmd(QString cmd)
{
    simulator_cmd = cmd;
}
