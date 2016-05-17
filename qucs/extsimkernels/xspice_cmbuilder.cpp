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
#include "components/libcomp.h"
#include "spicecomponents/xsp_cmlib.h"
#include "main.h"

XSPICE_CMbuilder::XSPICE_CMbuilder(Schematic *sch_)
{
    workdir = QucsSettings.S4Qworkdir;
    Sch = sch_;
    cmsubdir = "qucs_cmlib/";
    cmdir = QDir::convertSeparators(workdir+"/"+cmsubdir);
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

/*!
 * \brief XSPICE_CMbuilder::needCompile
 * \return Returns true if schematic contains CodeModels that need
 *         to be compiled. False otherwise.
 */
bool XSPICE_CMbuilder::needCompile()
{
    bool r = false;
    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if (pc->Model=="XSP_CMod") r = true;
        if (pc->Model=="Lib") {
            LibComp *libpc = (LibComp *)pc;
            if ((!libpc->getAttachedIFS().isEmpty())&&
                (!libpc->getAttachedMOD().isEmpty())) r = true;
        }
        if (pc->Model=="Sub") { // Scan subcircuits recursively
            Schematic *d = new Schematic(0, ((Subcircuit *)pc)->getSubcircuitFile());
            if(!d->loadDocument())      // load document if possible
            {
                delete d;
                continue;
            }
            XSPICE_CMbuilder *bld = new XSPICE_CMbuilder(d);
            if (bld->needCompile()) r = true;
            delete bld;
            delete d;
        }
    }
    return r;
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
    mod_ifs_pairs.clear();
}

/*!
 * \brief Ngspice::createCModelTree Obtain all cfunc.mod and ifspec.ifs files paths and
 *        copy it into woriking directory of dynamic XSPICE CodeModels builder
 */
void XSPICE_CMbuilder::createCModelTree(QString &output)
{
    QStringList lst_entries; // For modpath.lst
    QStringList objects; // Object files that need to be compiled

    ExtractModIfsFiles(objects,lst_entries,"",output);

    // Form modpath.lst. List all subdirectories entries in it
    QFile modpath_lst(cmdir+"/modpath.lst");
    if (modpath_lst.open(QIODevice::WriteOnly)) {
        QTextStream stream(&modpath_lst);
        lst_entries<<"";
        stream<<lst_entries.join("\n");
        modpath_lst.close();
    }

    // Create empty udnpath.lst
    QFile udnpath_lst(cmdir+"/udnpath.lst");
    if (udnpath_lst.open(QIODevice::WriteOnly))
        udnpath_lst.close();

    // Form proper Makefile
    QString ngsp_root = getNgspiceRoot();
    QFile mkfile(cmdir+"/Makefile");
    if (mkfile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&mkfile);
#ifdef __MINGW32__
        QString rules_file = QucsSettings.BinDir+"../share/qucs/xspice_cmlib/cmlib.mingw32.rules.mk";
#else
        QString rules_file = QucsSettings.BinDir+"../share/qucs/xspice_cmlib/cmlib.linux.rules.mk";
#endif
        QFileInfo inf(rules_file);  
        if (!inf.exists())
            output += QString("Make rules file %1 doesn't exist\n").arg(rules_file);
        stream<<"TARGET=qucs_xspice.cm\n";
        stream<<QString("NGSPICEROOT=%1\n").arg(ngsp_root);
        stream<<QString("OBJECTS=dlmain.o %1\n\n").arg(objects.join(" "));
        stream<<"include "+rules_file +"\n";
        mkfile.close();
    }

    // Extract dlmain.c from the Ngspice installation
   // QFileInfo inf(QucsSettings.NgspiceExecutable);
    QFile::copy(ngsp_root+"/share/ngspice/dlmain.c",cmdir+"/dlmain.c");
}

/*!
 * \brief XSPICE_CMbuilder::ModIfsPairProcessed
 * \param mod[in] full cfunc.mod file name
 * \param ifs[in] full ifspec.ifs file name
 * \return True if this model files already copied into build tree. False otherwise
 */
bool XSPICE_CMbuilder::ModIfsPairProcessed(const QString &mod, const QString &ifs)
{
    foreach (QStringList lst, mod_ifs_pairs) {
        if ((lst.at(0)==mod)&&(lst.at(1)==ifs)) return true;
    }
    return false;
}

/*!
 * \brief XSPICE_CMbuilder::ExtractModIfsFiles Scan schematic on "XSPICE CodeModel"
 *        components, extract cfunc.mod and ifspec.ifs, and copy then into build tree.
 * \param objects[out] QStringList to add *.o targets
 * \param lst_entries[out] Subdirectories names for modpath.lst
 * \param prefix[in] Output subdirectory name prefix. It is need for correct processing
 *        of subcircuits.
 */
void XSPICE_CMbuilder::ExtractModIfsFiles(QStringList &objects, QStringList &lst_entries,
                                          const QString &prefix, QString &output)
{
    QDir dir_cm(cmdir);

    for(Component *pc = Sch->DocComps.first(); pc != 0; pc = Sch->DocComps.next()) {
        if ((pc->Model=="XSP_CMod")||(pc->Model=="Lib")) {
            // Copy every cfunc.mod and ifspe.ifs pair into
            // unique subdirectory in qucs_cmlib/
            QStringList mod_lst,ifs_lst;
            if (pc->Model=="Lib") {
                LibComp *libpc = (LibComp *)pc;
                mod_lst.append(libpc->getAttachedMOD());
                ifs_lst.append(libpc->getAttachedIFS());
            } else {
                mod_lst.append(spicecompat::convert_relative_filename(pc->Props.at(0)->Value));
                ifs_lst.append(spicecompat::convert_relative_filename(pc->Props.at(1)->Value));
            }
            QStringList::iterator mod = mod_lst.begin();
            QStringList::iterator ifs = ifs_lst.begin();

            if (mod_lst.count()!=ifs_lst.count()) { // Something is missing. Skip such component.
                output += QString("Some MOD or IFS files are missing for component %1\n"
                                  "Skipping component \n"
                                  "MOD files are: %2\n"
                                  "IFS files are: %2\n").arg(pc->Name)
                                               .arg(mod_lst.join(" ").arg(ifs_lst.join(" ")));
                continue;
            }

            for (int i=0;mod!=mod_lst.end();mod++,ifs++,i++) {
                QStringList lst1;
                lst1<<(*mod)<<(*ifs);
                // If model is duplicated don't process it (don't copy files)
                if (!ModIfsPairProcessed((*mod),(*ifs))) {
                    QString destdir = QDir::convertSeparators(prefix + pc->Name + QString::number(i));
                    if (!dir_cm.mkdir(destdir))
                        output += QString("Cannot create directory %1 \n").arg(destdir);
                    lst_entries += destdir;
                    // Add cfunc.mod file
                    QString destfile = normalizeModelName((*mod),destdir);
                    if (!QFile::copy((*mod),destfile))
                        output += QString("Cannot copy file %1 to %2 \n").arg(*mod).arg(destfile);
                    destfile.chop(4); // Add cfunc.o to objects
                    destfile+=".o";
                    objects.append(destfile);
                    // Add ifspec.ifs file
                    destfile = normalizeModelName(*ifs,destdir);
                    if (!QFile::copy(*ifs,destfile))
                        output += QString("Cannot copy file %1 to %2 \n").arg(*ifs).arg(destfile);
                    destfile.chop(4); // Add ifspec.o to objects
                    destfile+=".o";
                    objects.append(destfile); // Add ifspec.o to objects
                }
                mod_ifs_pairs.append(lst1); // This *.mod and *.ifs pair already processed
            }
        }

        if (pc->Model=="Sub") { // Scan subcircuits recursively
            Schematic *d = new Schematic(0, ((Subcircuit *)pc)->getSubcircuitFile());
            if(!d->loadDocument())      // load document if possible
            {
                delete d;
                continue;
            }
            XSPICE_CMbuilder *bld = new XSPICE_CMbuilder(d);
            bld->setProcessedFiles(mod_ifs_pairs);
            bld->ExtractModIfsFiles(objects,lst_entries,pc->Name,output);
            bld->getProcessedFiles(mod_ifs_pairs);
            delete bld;
            delete d;
        }
    }
}

/*!
 * \brief XSPICE_CMbuilder::normalizeModelName Get normalized model name for Nsgpice.
 *        Rename *.mod file for cfunc.mod. Rename *.ifs to ifspec.ifs.
 * \param file[in] Input file
 * \param destdir[in] Destination directory
 * \return Normalized file name *.mod -> destdir/cfunc.mod
 *                              *.ifs -> destdir/ifspec.ifs
 */
QString XSPICE_CMbuilder::normalizeModelName(QString &file, QString &destdir)
{
    QFileInfo inf(file);
    QString filenam = inf.fileName();
    if (filenam.endsWith(".mod"))
        return (cmdir+'/'+destdir+"/cfunc.mod");
    if (filenam.endsWith(".ifs"))
        return (cmdir+'/'+destdir+"/ifspec.ifs");
    return (cmdir+'/'+destdir+'/'+filenam);
}

/*!
 * \brief Ngspice::compileCMlib Compile all models and obtain qucs_xspice.cm
 */
void XSPICE_CMbuilder::compileCMlib(QString &output)
{
    output += "Executing make to build XSPICE CodeModels ...\n";
    output += QString("Working directory is %1\n").arg(cmdir);

    QProcess *make = new QProcess();
    make->setReadChannelMode(QProcess::MergedChannels);
    make->setWorkingDirectory(cmdir);
#ifdef __MINGW32__
    make->start("mingw32-make.exe"); // For Unix
#else
    make->start("make"); // For Unix
#endif
    make->waitForFinished();
    output += make->readAll();
    delete make;
}

/*!
 * \brief XSPICE_CMbuilder::appendProcessedModIfsPairs This method is needed to avoid
 *        duplicated compilation of models in nested subcircuits.
 * \param processed_pair[in] The list of Ifs and Mod files that are already processed.
 */
void XSPICE_CMbuilder::setProcessedFiles(QList<QStringList> processed_pairs)
{
    mod_ifs_pairs.append(processed_pairs);
}

/*!
 * \brief XSPICE_CMbuilder::GetProcessedFiles Obtain information about Mod and Ifs files
 *        that are already processed. This method is needed to avoid duplicated compilation
 *        of models in nested subcircuits.
 * \param processed_pair[out]
 */
void XSPICE_CMbuilder::getProcessedFiles(QList<QStringList> &processed_pairs)
{
    processed_pairs.append(mod_ifs_pairs);
}

/*!
 * \brief XSPICE_CMbuilder::getNgspiceRoot Get Ngspice root installation directory.
 *        For example /usr/ for /usr/bin/ngspice
 * \return Path to ngspice root
 */
QString XSPICE_CMbuilder::getNgspiceRoot()
{
    QFileInfo inf(QucsSettings.NgspiceExecutable);
    QString s;
    if (!inf.exists()) { // It may be in $PATH
        char *p = getenv("PATH");
        QStringList paths;
        if (p!=NULL) paths = QString(p).split(':');
        foreach (QString pp,paths) {
            inf.setFile(pp+QDir::separator()+QucsSettings.NgspiceExecutable);
            if (inf.exists()) s = inf.canonicalPath();
        }
    } else s = inf.canonicalPath();
    if (s.endsWith("bin")) s.chop(3);
    return s;
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
