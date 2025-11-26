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

#include "./../../QucsS/QucsSExporter.h"

// Microstrip open circuit model
QString QucsSExporter::parseMicrostripOpen_QucsS(ComponentInfo Comp) {
  //  Open model example
  // <MOPEN MS8 1 550 540 15 -12 0 1 "Sub1" 0 "0.13 mm" 1 "Hammerstad" 0
  // "Kirschning" 0 "Kirschning" 0>
  int status = 1;

  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset + 30;

  int x_text = 25;
  int y_text = 0;

  // Parameters
  QString Width = Comp.val["Width"];
  int Width_visibility = 1;

  x_text = 20;
  y_text = -30;

  // Save pin position. This is needed for wiring later
  ComponentPinMap[Comp.ID][0] = QPoint(x_pos, y_pos - 30); // Pin 1

  QString componentLine =
      QString("<MOPEN %1 %2 %3 %4 %5 %6 1 3 \"Subst1\" 0 \"%7\" %8 "
              "\"Hammerstad\" 0 \"Kirschning\" 0 \"Kirschning\" 0>\n")
          .arg(Comp.ID)
          .arg(status)
          .arg(x_pos)
          .arg(y_pos)
          .arg(x_text)
          .arg(y_text)
          .arg(Width)
          .arg(Width_visibility);

  return componentLine;
}
