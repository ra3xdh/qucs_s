/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SCHEMATICEXPORTER_H
#define SCHEMATICEXPORTER_H
 
#include "../SchematicContent.h"

class SchematicContent;

class SchematicExporter {
public:
    virtual ~SchematicExporter() = default;
    virtual QString exportSchematic() = 0;

protected:
    // Scaling factor wrt the tool's schematic viewer
    int scale_x;
    int scale_y;

    // Offset, introduced for parsing some components
    int x_offset;
    int y_offset;

    // Map relating each component ID with a list which contains the position of all its pins
    QMap<QString, QList<QPoint>> ComponentPinMap;
};

#endif // SCHEMATICEXPORTER_H
