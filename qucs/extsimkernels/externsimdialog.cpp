/***************************************************************************
                           abstractspicekernel.cpp
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

#include "externsimdialog.h"
#include "simsettingsdialog.h"
#include "main.h"
#include "qucs.h"

ExternSimDialog::ExternSimDialog(Schematic *sch, bool netlist_mode) :
    QDialog(sch),
    a_schematic(sch),
    a_buttonStopSim(new QPushButton(tr("Stop"),this)),
    a_buttonSaveNetlist(new QPushButton(tr("Save netlist"),this)),
    a_buttonExit(new QPushButton(tr("Exit"),this)),
    a_editSimConsole(new QPlainTextEdit(this)),
    a_simStatusLog(new QListWidget),
    a_simProgress(new QProgressBar(this)),
    a_ngspice(new Ngspice(sch,this)),
    a_xyce(new Xyce(sch,this)),
    a_wasSimulated(true),
    a_hasError(false)
{
    const QString workdir(QucsSettings.S4Qworkdir);

    QSettings settings("qucs", "qucs_s");
    restoreGeometry(settings.value("ExternSimDialog/geometry").toByteArray());

    setWindowTitle(tr("Simulate with external simulator"));
    setMinimumWidth(500);

    QFileInfo inf(workdir);
    if (!inf.exists()) {
        QDir dir;
        dir.mkpath(workdir);
    }

    connect(a_buttonStopSim,SIGNAL(clicked()),a_ngspice,SLOT(killThemAll()));
    connect(a_buttonStopSim,SIGNAL(clicked()),a_xyce,SLOT(killThemAll()));
    a_buttonStopSim->setEnabled(false);

    connect(a_buttonSaveNetlist,SIGNAL(clicked()),this,SLOT(slotSaveNetlist()));

    connect(a_buttonExit,SIGNAL(clicked()),this,SLOT(slotExit()));
    connect(a_buttonExit,SIGNAL(clicked()),a_ngspice,SLOT(killThemAll()));
    connect(a_buttonExit,SIGNAL(clicked()),a_xyce,SLOT(killThemAll()));

    QGroupBox *grp_1 = new QGroupBox(tr("Simulation console"),this);
    QVBoxLayout *vbl1 = new QVBoxLayout;

    QFont font;
    font.setFamily("monospace");
    font.setPointSize(10);
    a_editSimConsole->setFont(font);
    a_editSimConsole->setReadOnly(true);
    vbl1->addWidget(a_editSimConsole);
    grp_1->setLayout(vbl1);
    a_ngspice->setConsole(a_editSimConsole);
    a_xyce->setConsole(a_editSimConsole);

    connect(a_ngspice,SIGNAL(progress(int)),a_simProgress,SLOT(setValue(int)));
    connect(a_xyce,SIGNAL(progress(int)),a_simProgress,SLOT(setValue(int)));

    QVBoxLayout *vl_top = new QVBoxLayout;
    vl_top->addWidget(grp_1,3);
    vl_top->addWidget(a_simStatusLog,1);
    vl_top->addWidget(a_simProgress);
    QHBoxLayout *hl1 = new QHBoxLayout;
    hl1->addWidget(a_buttonStopSim);
    hl1->addWidget(a_buttonSaveNetlist);
    hl1->addWidget(a_buttonExit);
    vl_top->addLayout(hl1);
    setLayout(vl_top);

    slotSetSimulator();
    if (!netlist_mode && !QucsMain->TuningMode && a_schematic->getShowBias() != 0)
        slotStart(); // Start simulation

}

ExternSimDialog::~ExternSimDialog()
{
    a_ngspice->killThemAll();
}

void ExternSimDialog::slotSetSimulator()
{
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice: {
        a_xyce->setParallel(false);
        connect(a_ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(a_ngspice,SIGNAL(finished()),this,SLOT(slotProcessOutput()));
        connect(a_ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        QString cmd;
        if (QFileInfo(QucsSettings.NgspiceExecutable).isRelative()) { // this check is related to MacOS
            cmd = QFileInfo(QucsSettings.BinDir + QucsSettings.NgspiceExecutable).absoluteFilePath();
        } else {
            cmd = QFileInfo(QucsSettings.NgspiceExecutable).absoluteFilePath();
        }
        if (QFileInfo::exists(cmd)) {
            a_ngspice->setSimulatorCmd(cmd);
        } else {
            a_ngspice->setSimulatorCmd(QucsSettings.NgspiceExecutable); //rely on $PATH
        }
        a_ngspice->setSimulatorParameters(QucsSettings.SimParameters);
    }
        break;
    case spicecompat::simXyce: {
        a_xyce->setParallel(false);
        connect(a_xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
        connect(a_xyce,SIGNAL(finished()),this,SLOT(slotProcessOutput()));
        connect(a_xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
        a_xyce->setSimulatorParameters(QucsSettings.SimParameters);
    }
        break;
//    case spicecompat::simXycePar: {
//#ifdef Q_OS_UNIX
//        a_xyce->setParallel(true);
//#else
//        a_xyce->setParallel(false);
//#endif
//        connect(a_xyce,SIGNAL(started()),this,SLOT(slotNgspiceStarted()));
//        connect(a_xyce,SIGNAL(finished()),this,SLOT(slotProcessOutput()));
//        connect(a_xyce,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)));
//        connect(buttonSimulate,SIGNAL(clicked()),a_xyce,SLOT(slotSimulate()));
//        a_xyce->setSimulatorParameters(QucsSettings.SimParameters);
//    }
//        break;
    case spicecompat::simSpiceOpus: {
        a_xyce->setParallel(false);
        connect(a_ngspice,SIGNAL(started()),this,SLOT(slotNgspiceStarted()),Qt::UniqueConnection);
        connect(a_ngspice,SIGNAL(finished()),this,SLOT(slotProcessOutput()),Qt::UniqueConnection);
        connect(a_ngspice,SIGNAL(errors(QProcess::ProcessError)),this,SLOT(slotNgspiceStartError(QProcess::ProcessError)),Qt::UniqueConnection);
        a_ngspice->setSimulatorCmd(QucsSettings.SpiceOpusExecutable);
        a_ngspice->setSimulatorParameters(QucsSettings.SimParameters);
    }
        break;
    default: break;
    }
}


void ExternSimDialog::slotProcessOutput()
{
    a_buttonSaveNetlist->setEnabled(true);
    a_buttonStopSim->setEnabled(false);
    QString out;

    // Set temporary safe output name

    QString ext;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice:
        ext = ".dat.ngspice";
        out = a_ngspice->getOutput();
        break;
    case spicecompat::simXyce:
        ext = ".dat.xyce";
        out = a_xyce->getOutput();
        break;
    case spicecompat::simSpiceOpus:
        out = a_ngspice->getOutput();
        ext = ".dat.spopus";
        break;
    default:
        out = "dummy";
        ext = ".dat";
        break;
    }

    if (logContainsError(out)) {
        addLogEntry(tr("There were simulation errors. Please check log."),
                    this->style()->standardIcon(QStyle::SP_MessageBoxCritical));
        a_hasError = true;
        a_wasSimulated = false;
        emit warnings();
    } else if (logContainsWarning(out)) {
        addLogEntry(tr("There were simulation warnings. Please check log."),
                    this->style()->standardIcon(QStyle::SP_MessageBoxWarning));
        addLogEntry(tr("Simulation finished. Now place diagram on schematic to plot the result."),
                    QIcon(":/bitmaps/svg/ok_apply.svg"));
        emit warnings();
    } else  {
        if ( !a_hasError ) {
            addLogEntry(tr("Simulation successful. Now place diagram on schematic to plot the result."),
                    QIcon(":/bitmaps/svg/ok_apply.svg"));
            emit success();
        }
    }
    //a_editSimConsole->clear();
    /*a_editSimConsole->insertPlainText(out);
    a_editSimConsole->moveCursor(QTextCursor::End);*/
    saveLog();
    a_editSimConsole->insertPlainText("Simulation finished\n");

    if ( !a_hasError ) {
        QFileInfo inf(a_schematic->getDocName());
        //QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.baseName()+"_ngspice.dat";
        QString qucs_dataset = inf.canonicalPath()+QDir::separator()+inf.completeBaseName()+ext;
        switch (QucsSettings.DefaultSimulator) {
            case spicecompat::simNgspice:
            case spicecompat::simSpiceOpus:
                a_ngspice->convertToQucsData(qucs_dataset);
                break;
            case spicecompat::simXyce:
                a_xyce->convertToQucsData(qucs_dataset);
                break;
            default:
                break;
        }
    }
    //a_wasSimulated = true;
    //if (out.contains("error",Qt::CaseInsensitive))
    //    a_hasError = true;
    emit simulated(this);
    //if (a_schematic->getShowBias()>0 || QucsMain->TuningMode) this->close();
}


void ExternSimDialog::slotNgspiceStarted()
{
    a_editSimConsole->clear();
    QString sim = spicecompat::getDefaultSimulatorName(QucsSettings.DefaultSimulator);
    a_editSimConsole->insertPlainText(sim + tr(" started...\n"));
    addLogEntry(tr("Simulation started on: ") + QDateTime::currentDateTime().toString(),
                this->style()->standardIcon(QStyle::SP_MessageBoxInformation));
}

void ExternSimDialog::slotNgspiceStartError(QProcess::ProcessError err)
{
    QString msg;
    switch (err) {
    case QProcess::FailedToStart:
        msg = tr("Failed to start simulator!");
        break;
    case QProcess::Crashed:
        msg = tr("Simulator crashed!");
        break;
    default:
        msg = tr("Simulator error!");
    }

    //QMessageBox::critical(this,tr("Simulate with SPICE"),msg,QMessageBox::Ok);
    addLogEntry(msg,this->style()->standardIcon(QStyle::SP_MessageBoxCritical));

    QString sim = spicecompat::getDefaultSimulatorName(QucsSettings.DefaultSimulator);
    a_editSimConsole->insertPlainText(sim + tr(" error..."));

    a_wasSimulated = false;
    a_hasError = true;
}

void ExternSimDialog::slotStart()
{
    a_buttonStopSim->setEnabled(true);
    a_buttonSaveNetlist->setEnabled(false);
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice:
        a_ngspice->slotSimulate();
        break;
    case spicecompat::simXyce:
        a_xyce->slotSimulate();
        break;
    case spicecompat::simSpiceOpus:
        a_ngspice->slotSimulate();
        break;
    default: break;
    }
}

void ExternSimDialog::slotStop()
{
    a_buttonStopSim->setEnabled(false);
    a_buttonSaveNetlist->setEnabled(true);
    a_ngspice->killThemAll();
}

void ExternSimDialog::slotSaveNetlist()
{
    QFileInfo inf(a_schematic->getDocName());
    QString filename = QFileDialog::getSaveFileName(this,tr("Save netlist"),inf.path()+QDir::separator()+"netlist.cir",
                       "All files (*)");
    if (filename.isEmpty()) return;

    switch (QucsSettings.DefaultSimulator) {
        case spicecompat::simNgspice:
        case spicecompat::simSpiceOpus: {
            a_ngspice->SaveNetlist(filename);
        }
            break;
        case spicecompat::simXyce: {
            a_xyce->SaveNetlist(filename);
        }
            break;
        default:
            break;
    }

    if (!QFile::exists(filename)) {
      QMessageBox::critical(0, QObject::tr("Save netlist"),
          QObject::tr("Disk write error!"), QMessageBox::Ok);
    }
}

void ExternSimDialog::slotExit()
{
    // Save window size / position and close this dialog.
    QSettings settings("qucs","qucs_s");
    settings.setValue("ExternSimDialog/geometry", saveGeometry());  

    accept();
}

void ExternSimDialog::saveLog()
{
    QString filename = QucsSettings.tempFilesDir.filePath("log.txt");
    QFile log(filename);
    if (log.open(QIODevice::WriteOnly)) {
        QTextStream ts_log(&log);
        ts_log<<a_editSimConsole->toPlainText();
        log.flush();
        log.close();
    }
}

void ExternSimDialog::addLogEntry(const QString &text, const QIcon &icon)
{
    QListWidgetItem *itm = new QListWidgetItem;
    itm->setText(text);
    itm->setIcon(icon);
    a_simStatusLog->addItem(itm);
}

bool ExternSimDialog::logContainsError(const QString &out)
{
    bool found = false;
    QStringList err_patterns;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice:
        err_patterns<<"Error:"<<"ERROR"<<"Error "
                    <<"Syntax error:"<<"Expression err:"
                    <<"errors:"<<"simulation(s) aborted"
                    <<"simulation aborted"<<"analysis aborted";
        break;
    case spicecompat::simXyce:
        err_patterns<<"Error:"<<"ERROR"<<"MSG_ERROR"
                    <<"error:"<<"MSG_FATAL";
        break;
    default: err_patterns<<"error";
        break;
    }
    for(const auto &err_str: err_patterns) {
        if (out.contains(err_str)) {
            found = true;
            break;
        }
    }
    return found;
}

bool ExternSimDialog::logContainsWarning(const QString &out)
{
    bool found = false;
    QStringList warn_patterns;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simNgspice:
        warn_patterns<<"Warning:"<<"WARNING"<<"Warning "
                    <<"warning:";
        break;
    case spicecompat::simXyce:
        warn_patterns<<"Warning:"<<"WARNING"<<"Warning "
                    <<"warning:";
        break;
    default: warn_patterns<<"warning";
        break;
    }
    for(const auto &warn_str: warn_patterns) {
        if (out.contains(warn_str)) {
            found = true;
            break;
        }
    }
    return found;
}

