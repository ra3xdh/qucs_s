/// @file parseMicrostripOpen_QucsS.cpp
/// @brief Converts a microstrip open-end discontinuity to Qucs-S format
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../QucsS/QucsSExporter.h"

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
