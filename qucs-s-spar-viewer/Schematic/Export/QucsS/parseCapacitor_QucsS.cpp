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

QString QucsSExporter::parseCapacitor_QucsS(ComponentInfo Comp) {
  // Format: <C ID status x y text_x text_y C C_visibility>
  // ID: e.g. Cx, where x is a positive integer
  // status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // text_x: x-axis position of the texts showing the properties
  // text_y: y-axis position of the texts showing the properties
  // 0: Unidentified behaviour
  // rotation: Component's rotation {0, 1, 2, 3}
  // C: Capacitance value (double)
  // C_visibility: Visibility of C. {0: Hide; 1: Show}

  // Example: <C C1 1 520 420 17 -26 0 1 "1.074pF" 1 "" 0 "neutral" 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset;
  int x_text = 25;
  int y_text = 0;
  int rotation = static_cast<int>(Comp.Rotation / 90) + 1;
  QString C = Comp.val["C"];
  int C_visibility = 1;

  // Adjust text position depending on orientation
  switch (rotation) {
  case 3:
  case 1: // Vertical orientation
    x_text = 20;
    y_text = -20;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos, y_pos + 30); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos, y_pos - 30); // Pin 2
    break;

  case 0:
  case 2: // Horizontal orientation
    x_text = -30;
    y_text = -50;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos - 30, y_pos); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos + 30, y_pos); // Pin 2
    break;
  }

  QString componentLine = QString("<C %1 %2 %3 %4 %5 %6 0 %7 \"%8\" %9>\n")
                              .arg(Comp.ID)
                              .arg(status)
                              .arg(x_pos)
                              .arg(y_pos)
                              .arg(x_text)
                              .arg(y_text)
                              .arg(rotation)
                              .arg(C)
                              .arg(C_visibility);

  return componentLine;
}
