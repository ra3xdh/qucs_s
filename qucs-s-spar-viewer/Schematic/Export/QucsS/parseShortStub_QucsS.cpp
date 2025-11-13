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

// The short stub is just a transmission line terminated with a GND
QString QucsSExporter::parseShortStub_QucsS(ComponentInfo Comp) {
  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset;
  int x_text = 25;
  int y_text = 0;
  int rotation = static_cast<int>(Comp.Rotation / 90) + 1;

  // Parameters
  QString Z0 = Comp.val["Z0"];
  int Z0_visibility = 1;

  QString Length = Comp.val["Length"];
  int Length_visibility = 1;

  // Adjust text position depending on orientation
  switch (rotation) {
  case 2:
  case 0: // Horizontal orientation
    x_text = -30;
    y_text = -60;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos - 30, y_pos); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos + 30, y_pos); // Pin 2
    break;
  case 1: // Vertical orientation
    x_text = 10;
    y_text = -20;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos, y_pos + 30); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos, y_pos - 30); // Pin 2
    break;
  }

  // Line
  QString TL_Line = QString("<TLIN %1 %2 %3 %4 %5 %6 1 %7 \"%8\" %9 "
                            "\"%10\" %11 \"0 dB\" 0 \"26.85\" 0>\n")
                        .arg(Comp.ID)
                        .arg(status)
                        .arg(x_pos)
                        .arg(y_pos)
                        .arg(x_text)
                        .arg(y_text)
                        .arg(rotation)
                        .arg(Z0)
                        .arg(Z0_visibility)
                        .arg(Length)
                        .arg(Length_visibility);

  // GND
  QString GND_Line = QString("<GND * %1 %2 %3 0 0 0 0>\n")
                         .arg(status)
                         .arg(x_pos)
                         .arg(y_pos + 30);

  QString componentLine = TL_Line + GND_Line;

  return componentLine;
}
