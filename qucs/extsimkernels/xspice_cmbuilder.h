/***************************************************************************
                            xspice_cmbuilder.h
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


#ifndef XSPICE_CMBUILDER_H
#define XSPICE_CMBUILDER_H

#include <QtCore>
#include "schematic.h"

class XSPICE_CMbuilder
{
private:
    QString cmdir,cmsubdir;
    QString workdir,spinit_name;

    QList<QStringList> mod_ifs_pairs;

    Schematic *Sch;

public:
    XSPICE_CMbuilder(Schematic *sch_);
    ~XSPICE_CMbuilder();

    void cleanSpiceinit();
    void createSpiceinit(const QString &initial_spiceinit);
    bool needCompile();
    void cleanCModelTree();
    void createCModelTree(QString &output);
    void compileCMlib(QString &ouptut);
    void getModIfsFileList(QStringList &files);

protected:
    void setProcessedFiles(QList<QStringList> processed_pairs);
    void getProcessedFiles(QList<QStringList> &processed_pairs);

private:
    QString normalizeModelName(QString &file,QString &destdir);
    bool removeDir(const QString &dirName);
    void ExtractSpiceinitdata(QTextStream &stream);
    void ExtractModIfsFiles(QStringList &objects, QStringList &lst_entries,
                            const QString &prefix, QString &output);
    bool ModIfsPairProcessed(const QString &mod, const QString &ifs);
    QString getNgspiceRoot();
};

#endif // XSPICE_CMBUILDER_H
