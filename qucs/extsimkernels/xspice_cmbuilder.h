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

    Schematic *Sch;

public:
    XSPICE_CMbuilder(Schematic *sch_);
    ~XSPICE_CMbuilder();

    void createSpiceinit();
    bool needCompile();
    void cleanCModelTree();
    void createCModelTree();
    void compileCMlib(QString &ouptut);

private:
    QString normalizeModelName(QString &file,QString &destdir);
    bool removeDir(const QString &dirName);
};

#endif // XSPICE_CMBUILDER_H
