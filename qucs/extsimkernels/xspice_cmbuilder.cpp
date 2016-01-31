/***************************************************************************
                           xspice_cmbuilder.cpp
                             ----------------
    begin                : Sun Jan 31 2015
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

#include "xspice_cmbuilder.h"
#include "components/subcircuit.h"
#include "spicecomponents/xsp_cmlib.h"
#include "main.h"

XSPICE_CMbuilder::XSPICE_CMbuilder(Schematic *sch_)
{
    workdir = QucsSettings.S4Qworkdir;
    Sch = sch_;
    cmsubdir = "qucs_cmlib/";
    cmdir = QDir::convertSeparators(workdir+cmsubdir);
    spinit_name=QDir::convertSeparators(workdir+"/.spiceinit");
}

XSPICE_CMbuilder::~XSPICE_CMbuilder()
{

}

void XSPICE_CMbuilder::cleanSpiceinit()
{
    QFileInfo inf(spinit_name);
    if (inf.exists()) QFile::remove(spinit_name);
}

/*!
 * \brief XSPICE_CMbuilder::createSpiceinit Extract precompiled *.cm libraries names from
 *        all components and subcircuits recursively.
 */
void XSPICE_CMbuilder::createSpiceinit()
{
    QFile spinit(spinit_name);
    if (spinit.open(QIODevice::WriteOnly)) {
        QTextStream stream(&spinit);
        ExtractSpiceinitdata(stream);
        if (needCompile()) stream<<"codemodel "+cmdir+"qucs_xspice.cm";
        spinit.close();
    }
}

void XSPICE_CMbuilder::ExtractSpiceinitdata(QTextStream &stream)
{
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->Model=="XSP_CMlib") {
            stream<<((XSP_CMlib *)pc)->getSpiceInit();
        }
        if (pc->Model=="Sub") {
            //s = pc->Props.first()->Value;
            Schematic *d = new Schematic(0, ((Subcircuit *)pc)->getSubcircuitFile());
            if(!d->loadDocument())      // load document if possible
            {
                delete d;
                continue;
            }
            XSPICE_CMbuilder *bld = new XSPICE_CMbuilder(d);
            bld->ExtractSpiceinitdata(stream);
            delete bld;
            delete d;
        }
    }
}

bool XSPICE_CMbuilder::needCompile()
{
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->Model=="XSP_CMod") return true;
    }
    return false;
}

/*!
 * \brief Ngspice::cleanCModelTree Removes qucs_cmlib/ and all its contents.
 *        Then creates an empty qucs_cmdir/
 */
void XSPICE_CMbuilder::cleanCModelTree()
{
    removeDir(cmdir);
    QDir wd(workdir);
    wd.mkdir(cmsubdir);
}

/*!
 * \brief Ngspice::createCModelTree Obtain all cfunc.mod and ifspec.ifs files paths and
 *        copy it into woriking directory of dynamic XSPICE CodeModels builder
 */
void XSPICE_CMbuilder::createCModelTree()
{
    QDir dir_cm(cmdir);
    QString lst_entries; // For modpath.lst
    QStringList objects; // Object files that need to be compiled

    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->Model=="XSP_CMod") {
            // Copy every cfunc.mod and ifspe.ifs pair into
            // unique subdirectory in qucs_cmlib/
            QString destdir = QDir::convertSeparators(pc->Name);
            dir_cm.mkdir(destdir);
            lst_entries += destdir + "\n";
            // Add cfunc.mod file
            QString file = pc->Props.at(0)->Value;
            QString destfile = normalizeModelName(file,destdir);
            QFile::copy(file,destfile);
            destfile.chop(4); // Add cfunc.o to objects
            destfile+=".o";
            objects.append(destfile);
            // Add ifspec.ifs file
            file = pc->Props.at(1)->Value;
            destfile = normalizeModelName(file,destdir);
            QFile::copy(file,destfile);
            destfile.chop(4); // Add ifspec.o to objects
            destfile+=".o";
            objects.append(destfile); // Add ifspec.o to objects
        }
    }

    // Form modpath.lst. List all subdirectories entries in it
    QFile modpath_lst(cmdir+"/modpath.lst");
    if (modpath_lst.open(QIODevice::WriteOnly)) {
        QTextStream stream(&modpath_lst);
        stream<<lst_entries;
        modpath_lst.close();
    }

    // Create empty udnpath.lst
    QFile udnpath_lst(cmdir+"/udnpath.lst");
    if (udnpath_lst.open(QIODevice::WriteOnly))
        udnpath_lst.close();

    // Form proper Makefile
    QFile mkfile(cmdir+"/Makefile");
    if (mkfile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&mkfile);
        QString rules_file = QucsSettings.BinDir+"../share/qucs/xspice_cmlib/cmlib.linux.rules.mk";
        stream<<"TARGET=qucs_xspice.cm\n";
        stream<<QString("OBJECTS=dlmain.o %1\n\n").arg(objects.join(" "));
        stream<<"include "+rules_file +"\n";
        mkfile.close();
    }

    // Extract dlmain.c from the Ngspice installation
    QFileInfo inf(QucsSettings.NgspiceExecutable);
    QFile::copy(inf.path()+"/../share/ngspice/dlmain.c",cmdir+"/dlmain.c");
}

QString XSPICE_CMbuilder::normalizeModelName(QString &file, QString &destdir)
{
    QFileInfo inf(file);
    QString filenam = inf.fileName();
    if (filenam.endsWith(".mod"))
        return QDir::convertSeparators(cmdir+'/'+destdir+"/cfunc.mod");
    if (filenam.endsWith(".ifs"))
        return QDir::convertSeparators(cmdir+'/'+destdir+"/ifspec.ifs");
    return QDir::convertSeparators(cmdir+'/'+destdir+'/'+filenam);
}

/*!
 * \brief Ngspice::compileCMlib Compile all models and obtain qucs_xspice.cm
 */
void XSPICE_CMbuilder::compileCMlib(QString &output)
{
    QProcess *make = new QProcess();
    make->setReadChannelMode(QProcess::MergedChannels);
    make->setWorkingDirectory(cmdir);
    make->start("make"); // For Unix
    make->waitForFinished();
    output += make->readAll();
    delete make;
}

bool XSPICE_CMbuilder::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}
