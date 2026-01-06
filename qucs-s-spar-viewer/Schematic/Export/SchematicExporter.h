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

///
/// Abstract base class for exporting schematics
///
class SchematicExporter {
public:
  ///
  /// @brief Class destructor
  ///
  virtual ~SchematicExporter() = default;

  ///
  /// @brief Export function
  /// \note The implementation will be provided in the classes inheriting this base class
  ///
  virtual QString exportSchematic() = 0;

protected:
    // Scaling factor wrt the tool's schematic viewer
    int scale_x; ///< x-axis scale factor
    int scale_y; ///< y-axis scale factor

    // Offset, introduced for parsing some components
    int x_offset; ///< x-axis offset
    int y_offset; ///< y-axis offset

    ///
    /// @brief Maps component identifiers to their pin position lists
    /// @details Each component ID (QString) maps to a QList of QPoint objects
    ///          representing the coordinates of all pins for that component.
    ///
    QMap<QString, QList<QPoint>> ComponentPinMap;
};

#endif // SCHEMATICEXPORTER_H
