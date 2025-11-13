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

#include "./../QucsS/QucsSExporter.h"

QString QucsSExporter::parseGND_QucsS(ComponentInfo Comp) {
  // Format: <GND * status x y 0 0 0 0>
  // status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // 0's: Unidentified behaviour

  // Example: <GND * 1 60 350 0 0 0 0>

  int status = 1;

  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset -
              20; // Additional correction needed: The GND in the
                  // internal schematic was a small wire connection
  int mirror = 0;

  if (Comp.Rotation == 180) {
    mirror = 1;
    y_pos += 20; // Correct y-axis position
  }

  QString componentLine = QString("<GND * %1 %2 %3 0 0 %4 0>\n")
                              .arg(status)
                              .arg(x_pos)
                              .arg(y_pos)
                              .arg(mirror);

  return componentLine;
}
