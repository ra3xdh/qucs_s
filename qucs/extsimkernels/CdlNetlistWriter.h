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

class QTextStream;
class Schematic;

class CdlNetlistWriter
{
public:
    CdlNetlistWriter(QTextStream& netlistStream, Schematic* schematic);
    ~CdlNetlistWriter() {};

    bool write();

private:
    int prepareNetlist();
    void startNetlist();

    QTextStream& a_netlistStream;
    Schematic* a_schematic;
};

#endif // CDL_NETLIST_WRITER_H
