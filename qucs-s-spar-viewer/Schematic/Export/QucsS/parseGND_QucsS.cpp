/// @file parseGND_QucsS.cpp
/// @brief Converts a ground reference component to Qucs-S format
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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
