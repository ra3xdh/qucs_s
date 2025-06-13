/*
 * CdlNetlistWriter.h - Schematic to CDL netlist converter declaration
 *
 * This file is part of Qucs-s
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

#ifndef CDL_NETLIST_WRITER_H
#define CDL_NETLIST_WRITER_H

#include <QMap>
#include <QStringList>
#include <QTextStream>

class Schematic;

class CdlNetlistWriter
{
public:
    CdlNetlistWriter(QTextStream& netlistStream, Schematic* schematic, bool resolveSpicePrefix);
    ~CdlNetlistWriter() {};

    bool write();

private:
    int prepareNetlist();
    void startNetlist();
    void resolveNetListContinuation(QStringList& netList);
    void resolveSpicePrefix();

    QTextStream& a_netlistStream;
    Schematic* a_schematic;
    const bool a_resolveSpicePrefix;
    QString a_netListString;
    QTextStream a_netListStringStream;
    QTextStream& a_effectiveNetlistStream;
};

#endif // CDL_NETLIST_WRITER_H
