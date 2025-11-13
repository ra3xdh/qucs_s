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

#include "QucsSExporter.h"
#include <QMap>
#include <QRegularExpression>

QucsSExporter::QucsSExporter() {

  // Scale with respect to the internal schematic
  scale_x = 2;
  scale_y = 1.2;

  // Offset, introduced by some components (e.g. MS coupled lines) that
  // don't match the internal schematic symbol
  x_offset = 0;
  y_offset = 0;
}

void QucsSExporter::setSchematiContent(const SchematicContent &SC) {
  schematic = &SC;
}

QString QucsSExporter::exportSchematic() {}
